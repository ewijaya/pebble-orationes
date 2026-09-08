#!/usr/bin/env python3
"""Freeze, publish, and verify ONE tested Orationes PBW. No publishing by default.

See docs/releasing.md. Imports of authenticated services are lazy; help and host
tests need neither credentials nor the Pebble SDK. Never rebuild during publish.
"""
import argparse
import hashlib
import html
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time
import zipfile

ROOT = Path(__file__).resolve().parents[1]
APP_ID = '9882f741750c43eb8309777e'
UUID = '9f17d477-cef1-4512-8536-f01d50bb07a3'
REPO = 'ewijaya/pebble-orationes'
STORE = f'https://apps.repebble.com/{APP_ID}'
API = 'https://appstore-api.repebble.com'
DASHBOARD = 'https://developer.repebble.com'

def run(*args, capture=False):
    return subprocess.run(args, cwd=ROOT, check=True, text=True,
                          stdout=subprocess.PIPE if capture else None).stdout

def digest(data):
    return hashlib.sha256(data).hexdigest()

def require(condition, message):
    if not condition:
        raise RuntimeError(message)

def clean_tree():
    require(not run('git', 'status', '--porcelain', capture=True).strip(),
            'Commit/review the development changes before preparing or publishing.')

def inspect_pbw(path, version):
    with zipfile.ZipFile(path) as archive:
        info = json.loads(archive.read('appinfo.json'))
        require(info['uuid'] == UUID and info['versionLabel'] == version,
                'PBW UUID/version does not match the release.')
        require(info['targetPlatforms'] == ['emery'] and not info['watchapp']['watchface'],
                'Expected the Emery-only Orationes watchapp.')
    return {'sha256': digest(path.read_bytes()), 'bytes': path.stat().st_size}

def candidate(version):
    require(bool(re.fullmatch(r'\d+\.\d+\.\d+', version)), 'Use a numeric semantic version.')
    return ROOT / '.release' / version

def read_manifest(version):
    folder = candidate(version)
    state = json.loads((folder / 'manifest.json').read_text())
    artifact = inspect_pbw(folder / 'pebble-orationes.pbw', version)
    require(artifact == state['artifact'], 'Frozen candidate changed; refusing publication.')
    require(state['version'] == version, 'Candidate version mismatch.')
    return folder, state

def save(folder, state):
    # A crash cannot replace a good journal with a partially written JSON file.
    temporary = folder / 'manifest.tmp'
    temporary.write_text(json.dumps(state, indent=2) + '\n')
    temporary.replace(folder / 'manifest.json')

def prepare(args):
    clean_tree()
    version = json.loads((ROOT / 'package.json').read_text())['version']
    require(version == args.version, 'Bump package.json and package-lock.json, review docs, and commit first.')
    lock = json.loads((ROOT / 'package-lock.json').read_text())
    require(lock['version'] == version and lock['packages']['']['version'] == version,
            'Lockfile version differs.')
    require(version in (ROOT / 'README.md').read_text(), 'README must describe this version.')
    folder = candidate(version)
    require(not folder.exists(), 'A frozen candidate already exists. Use it or choose a new version.')
    notes = args.notes.read_text().strip()
    description = args.description.read_text().strip() if args.description else None
    require(bool(notes), 'Release notes cannot be empty.')
    require('appstore' not in args.destination or bool(description), 'Store publication needs reviewed description text.')
    require(not run('git', 'ls-files', 'content', 'build', '.release', capture=True).strip(),
            'Canonical content, build output, and candidates must not be tracked.')
    commit = run('git', 'rev-parse', 'HEAD', capture=True).strip()
    run(sys.executable, 'scripts/check_release.py', '--screenshots')
    run(sys.executable, 'scripts/qa_reading.py', '--full')
    run(sys.executable, 'scripts/qa_reading.py', '--resume', '--full', '--entries', '1', '6', '25', '40')
    run(sys.executable, 'scripts/qa_defaults.py')
    if args.physical:
        run('pebble', 'install', '--cloudpebble', 'build/pebble-orationes.pbw')
    clean_tree()
    folder.mkdir(parents=True)
    artifact = folder / 'pebble-orationes.pbw'
    shutil.copy2(ROOT / 'build/pebble-orationes.pbw', artifact)
    for name in ('regression-screenshots', 'qa-reading', 'qa-defaults'):
        shutil.copytree(ROOT / 'build' / name, folder / name)
    state = {'version': version, 'commit': commit, 'artifact': inspect_pbw(artifact, version),
             'destinations': [d for d in ('github', 'appstore') if d in args.destination], 'notes': notes,
             'description': description, 'physical_installed': args.physical,
             'metrics': json.loads((ROOT / 'build/regression-metrics.json').read_text()),
             'status': {}}
    save(folder, state)
    print('Candidate frozen:', folder, state['artifact'], flush=True)
    print('Review emulator evidence and physical behavior, then explicitly approve publishing.')

def dashboard_session():
    import requests
    from pebble_tool.account import get_account
    account = get_account(auth_provider='firebase')
    require(account.is_logged_in, 'Run pebble login first.')
    token = account.get_access_token()  # Never logged or persisted by this tool.
    session = requests.Session()
    response = session.post(DASHBOARD + '/api/auth/firebase/session', json={'idToken': token}, timeout=30)
    response.raise_for_status()
    return session, token

def dashboard_app(session):
    response = session.get(f'{DASHBOARD}/api/dashboard/apps/{APP_ID}', timeout=30)
    response.raise_for_status()
    app = response.json()['app']
    require(app['id'] == APP_ID and app['app_uuid'] == UUID, 'Unexpected app identity.')
    return app

def preserved_fields(app):
    result = {k: app.get(k) for k in ('title', 'website', 'source', 'visible', 'category_id',
                                     'icon_small', 'icon_large', 'companion_apps')}
    result['assets'] = [{k: a.get(k) for k in ('platform', 'screenshots', 'headers', 'banner')}
                        for a in app['assets']]
    return result

def listing_fields(app, description):
    android = next((c for c in app.get('companion_apps', []) if c['platform'] == 'android'), {})
    return {'title': app['title'], 'description': description,
            'website': app.get('website') or '', 'source': app.get('source') or '',
            'visible': str(app['visible']).lower(), 'companionAndroidName': android.get('name', ''),
            'companionAndroidUrl': android.get('url', ''),
            'companionAndroidRequired': str(android.get('required', False)).lower()}

def github_release(state):
    result = subprocess.run(['gh', 'api', f'repos/{REPO}/releases/tags/v{state["version"]}'],
                            cwd=ROOT, text=True, capture_output=True)
    if result.returncode:
        require('404' in result.stderr, 'GitHub release lookup failed; not treating this as a missing release.')
        return None
    return json.loads(result.stdout)

def verify_github(state):
    import requests
    release = github_release(state)
    require(release and not release['draft'] and not release['prerelease'], 'GitHub release is not public.')
    assets = [a for a in release['assets'] if a['name'] == 'pebble-orationes.pbw']
    require(len(assets) == 1 and assets[0]['size'] == state['artifact']['bytes'], 'GitHub asset missing or wrong size.')
    response = requests.get(assets[0]['browser_download_url'], timeout=60)
    response.raise_for_status()
    require(digest(response.content) == state['artifact']['sha256'], 'GitHub PBW hash mismatch; never overwrite automatically.')
    latest = json.loads(run('gh', 'api', f'repos/{REPO}/releases/latest', capture=True))
    require(latest['tag_name'] == 'v' + state['version'], 'GitHub latest points to a different release.')

def upload_github(folder, state):
    tag = 'v' + state['version']
    require(run('git', 'branch', '--show-current', capture=True).strip() == 'main', 'Publish from main.')
    require(run('git', 'remote', 'get-url', 'origin', capture=True).strip().removesuffix('.git') in
            (f'https://github.com/{REPO}', f'git@github.com:{REPO}'), 'Unexpected publication remote.')
    run('git', 'push', 'origin', 'main')  # Commit must be remote before its annotated tag.
    found = subprocess.run(['git', 'rev-parse', '--verify', f'refs/tags/{tag}'], cwd=ROOT, capture_output=True)
    if found.returncode == 0:
        require(run('git', 'cat-file', '-t', tag, capture=True).strip() == 'tag', 'Expected an annotated tag.')
        require(run('git', 'rev-list', '-n', '1', tag, capture=True).strip() == state['commit'], 'Existing tag points elsewhere.')
    else:
        run('git', 'tag', '-a', tag, '-m', 'Orationes ' + tag, state['commit'])
    run('git', 'push', 'origin', tag)
    if not github_release(state):
        notes = folder / 'github-notes.md'
        notes.write_text(state['notes'] + '\n\nPBW SHA-256: `' + state['artifact']['sha256'] + '`.\n')
        run('gh', 'release', 'create', tag, str(folder / 'pebble-orationes.pbw'), '--verify-tag',
            '--title', 'Orationes ' + tag, '--notes-file', str(notes), '--latest')
    verify_github(state)

def upload_store(folder, state):
    from pebble_tool.commands.publish import PublishCommand
    import requests
    run('pebble', 'publish', '--help')
    session, token = dashboard_session()
    before = dashboard_app(session)
    preserved = preserved_fields(before)
    existing = next((r for r in before['releases'] if r['version'] == state['version']), None)
    if existing:
        require(existing['is_published'], 'Existing draft needs explicit review; not changing it silently.')
        response = requests.get(API + existing['pbw_url'], timeout=60)
        response.raise_for_status()
        require(digest(response.content) == state['artifact']['sha256'], 'Store version already exists with a different PBW.')
    else:
        # The CLI normally rebuilds; use its verified upload path on the frozen PBW.
        PublishCommand._upload_release(api_base=API, app_id=APP_ID, firebase_id_token=token,
            pbw_path=str(folder / 'pebble-orationes.pbw'), version=state['version'],
            release_notes=state['notes'], is_published=True, gif_paths=[], screenshot_paths=[],
            replace_screenshots=False)
    response = session.patch(f'{DASHBOARD}/api/dashboard/apps/{APP_ID}',
        files={k: (None, v) for k, v in listing_fields(before, state['description']).items()}, timeout=30)
    response.raise_for_status()
    after = dashboard_app(session)
    require(preserved_fields(after) == preserved, 'An unrelated listing field changed; stop for review.')
    require(after['description'] == state['description'], 'Dashboard description did not update.')
    require(all(a['description'] == state['description'] for a in after['assets'] if a['platform'] == 'emery'),
            'Emery description is not synchronized.')

def public_text(page):
    # Read both visible HTML and Next's serialized public data, never execute it.
    return html.unescape(page).replace('\\r\\n', '\n').replace('\\n', '\n').replace('\r\n', '\n')

def verify_store(state):
    import requests
    session, _ = dashboard_session()
    app = dashboard_app(session)
    release = app['latest_release']
    require(app['visible'] and release['version'] == state['version'] and release['is_published'], 'Dashboard version/visibility differs.')
    require(release['release_notes'] == state['notes'], 'Store release notes differ.')
    require(app['description'] == state['description'], 'Dashboard description differs.')
    response = requests.get(API + release['pbw_url'], timeout=60)
    response.raise_for_status()
    require(digest(response.content) == state['artifact']['sha256'], 'Store PBW hash mismatch.')
    for url, expected in ((STORE, [release['pbw_url'], state['description']]),
                          (STORE + '/changelog', [state['version'], state['notes']])):
        response = requests.get(url, timeout=30)
        response.raise_for_status()
        text = public_text(response.text)
        require(all(value.replace('\r\n', '\n') in text for value in expected),
                'Public page still pending synchronization: ' + url)

def verify(folder, state, attempts=1):
    for attempt in range(attempts):
        errors = []
        for destination in state['destinations']:
            try:
                (verify_github if destination == 'github' else verify_store)(state)
                state['status'][destination] = 'verified'
            except Exception as error:
                state['status'][destination] = 'pending verification'
                errors.append(str(error))
        save(folder, state)
        if not errors:
            print('All selected destinations verified:', state['status'], flush=True)
            return
        print('Verification pending:', '; '.join(errors), flush=True)
        if attempt + 1 < attempts:
            time.sleep(20)
    raise RuntimeError('Publication may be live, but verification is incomplete. Rerun verify; do not rebuild/reupload.')

def record_docs(folder, state):
    clean_tree()
    receipt = ROOT / 'docs/release-status.json'
    readme = ROOT / 'README.md'
    destinations = ' and '.join('GitHub Releases' if d == 'github' else 'the Pebble App Store' for d in state['destinations'])
    line = f'Orationes **v{state["version"]}** targets Pebble Time 2 (`emery`) and is available from {destinations}.'
    text, count = re.subn(r'^Orationes \*\*v[^\n]+targets Pebble Time 2[^\n]*$', line, readme.read_text(), flags=re.M)
    require(count == 1, 'README release-status line needs review.')
    receipt.write_text(json.dumps({k: state[k] for k in ('version', 'commit', 'artifact', 'metrics', 'status')}, indent=2) + '\n')
    readme.write_text(text)
    if 'github' in state['destinations']:
        notes = folder / 'github-notes.md'
        store_link = f'\n\nAlso available on the [Pebble App Store]({STORE}).' if 'appstore' in state['destinations'] else ''
        notes.write_text(state['notes'] + store_link + '\n\nPBW SHA-256: `' + state['artifact']['sha256'] + '`.\n')
        run('gh', 'release', 'edit', 'v' + state['version'], '--notes-file', str(notes))
    run('git', 'add', 'README.md', 'docs/release-status.json')
    if run('git', 'diff', '--cached', '--name-only', capture=True).strip():
        run('git', 'commit', '-m', f'docs: verify Orationes v{state["version"]} publication')
        run('git', 'push', 'origin', 'main')
    state['documentation_commit'] = run('git', 'rev-parse', 'HEAD', capture=True).strip()
    save(folder, state)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    p = commands.add_parser('prepare', help='Clean build, emulator QA, and freeze; NO publication')
    p.add_argument('version')
    p.add_argument('--notes', type=Path, required=True)
    p.add_argument('--description', type=Path)
    p.add_argument('--destination', action='append', choices=['github', 'appstore'], required=True)
    p.add_argument('--physical', action='store_true', help='Install this candidate on the connected PT2')
    p = commands.add_parser('publish', help='Publish the frozen artifact to explicitly selected destinations')
    p.add_argument('version')
    p.add_argument('--approve-publish', required=True, help='Repeat the version to authorize external writes')
    p.add_argument('--approve-physical', action='store_true', help='Confirm the installed candidate passed physical review')
    p = commands.add_parser('verify', help='Read-only public verification; safe to retry propagation')
    p.add_argument('version')
    p.add_argument('--attempts', type=int, default=3)
    args = parser.parse_args()
    if args.command == 'prepare':
        prepare(args); return
    folder, state = read_manifest(args.version)
    if args.command == 'verify':
        require(1 <= args.attempts <= 15, 'Use 1-15 bounded verification attempts.')
        verify(folder, state, args.attempts); return
    require(args.approve_publish == args.version, 'Publication approval must match the candidate version.')
    require(args.approve_physical and state['physical_installed'], 'Install and approve the exact physical candidate first.')
    clean_tree()
    require(run('git', 'rev-parse', 'HEAD', capture=True).strip() in
            (state['commit'], state.get('documentation_commit')), 'Source changed after preparation.')
    for destination in (d for d in ('github', 'appstore') if d in state['destinations']):
        (upload_github if destination == 'github' else upload_store)(folder, state)
        state['status'][destination] = 'uploaded; verification pending'
        save(folder, state)
    verify(folder, state, 3)
    record_docs(folder, state)

if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        print('Release stopped:', error, file=sys.stderr)
        sys.exit(1)
