"""Release safety tests. No credentials, subprocess publication, or network."""
import importlib.util
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
import zipfile

sys.dont_write_bytecode = True
spec = importlib.util.spec_from_file_location('release', Path(__file__).parents[1] / 'scripts/release.py')
release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(release)

class ReleaseTests(unittest.TestCase):
    def test_identity_and_hash(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'candidate.pbw'
            with zipfile.ZipFile(path, 'w') as archive:
                archive.writestr('appinfo.json', json.dumps({'uuid': release.UUID,
                    'versionLabel': '0.10.0', 'targetPlatforms': ['emery'], 'watchapp': {'watchface': False}}))
            self.assertEqual(release.inspect_pbw(path, '0.10.0')['sha256'], release.digest(path.read_bytes()))
            with self.assertRaises(RuntimeError): release.inspect_pbw(path, '0.9.0')

    def test_candidate_tamper_is_rejected(self):
        with tempfile.TemporaryDirectory() as directory, patch.object(release, 'ROOT', Path(directory)):
            folder = release.candidate('0.10.0'); folder.mkdir(parents=True)
            path = folder / 'pebble-orationes.pbw'
            with zipfile.ZipFile(path, 'w') as archive:
                archive.writestr('appinfo.json', json.dumps({'uuid': release.UUID,
                    'versionLabel': '0.10.0', 'targetPlatforms': ['emery'], 'watchapp': {'watchface': False}}))
            state = {'version': '0.10.0', 'artifact': release.inspect_pbw(path, '0.10.0')}
            release.save(folder, state)
            release.read_manifest('0.10.0')
            with path.open('ab') as stream: stream.write(b'tampered')
            with self.assertRaises(RuntimeError): release.read_manifest('0.10.0')

    def test_listing_preserves_fields(self):
        app = {'title': 'Orationes', 'website': None, 'source': 'source', 'visible': True,
               'companion_apps': [{'platform': 'android', 'name': 'Companion', 'url': 'https://example.com', 'required': True}],
               'assets': [{'platform': 'emery', 'screenshots': ['one', 'two'], 'banner': 'banner', 'headers': []}]}
        fields = release.listing_fields(app, 'New description')
        self.assertEqual(fields['title'], app['title'])
        self.assertEqual(fields['companionAndroidRequired'], 'true')
        self.assertEqual(fields['companionAndroidUrl'], 'https://example.com')
        self.assertFalse(any('screenshot' in key.lower() or 'icon' in key.lower() for key in fields))
        self.assertEqual(release.preserved_fields(app)['assets'], app['assets'])

    def test_verify_records_partial_state_without_uploads(self):
        with tempfile.TemporaryDirectory() as directory:
            state = {'destinations': ['github', 'appstore'], 'status': {}}
            with patch.object(release, 'verify_github'), patch.object(release, 'verify_store', side_effect=RuntimeError('cached')):
                with self.assertRaises(RuntimeError): release.verify(Path(directory), state)
            self.assertEqual(state['status'], {'github': 'verified', 'appstore': 'pending verification'})
            self.assertTrue((Path(directory) / 'manifest.json').exists())

    def test_publish_requires_matching_approval(self):
        with patch.object(sys, 'argv', ['release.py', 'publish', '0.10.0', '--approve-publish', '0.9.0']), \
             patch.object(release, 'read_manifest', return_value=(Path('.'), {})), \
             patch.object(release, 'upload_github') as upload:
            with self.assertRaises(RuntimeError): release.main()
            upload.assert_not_called()

    def test_publish_orders_github_before_store(self):
        calls = []
        state = {'version': '0.10.0', 'commit': 'reviewed', 'physical_installed': True,
                 'destinations': ['appstore', 'github'], 'status': {}}
        with patch.object(sys, 'argv', ['release.py', 'publish', '0.10.0',
                                       '--approve-publish', '0.10.0', '--approve-physical']), \
             patch.object(release, 'read_manifest', return_value=(Path('.'), state)), \
             patch.object(release, 'clean_tree'), patch.object(release, 'run', return_value='reviewed'), \
             patch.object(release, 'save'), patch.object(release, 'verify'), \
             patch.object(release, 'record_docs'), \
             patch.object(release, 'upload_github', side_effect=lambda *_: calls.append('github')), \
             patch.object(release, 'upload_store', side_effect=lambda *_: calls.append('appstore')):
            release.main()
        self.assertEqual(calls, ['github', 'appstore'])

    def test_no_path_traversal(self):
        for version in ('../0.10.0', '0.10.0/../', '', 'latest'):
            with self.assertRaises(RuntimeError): release.candidate(version)

if __name__ == '__main__': unittest.main()
