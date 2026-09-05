#!/usr/bin/env python3
"""Local release gate: host checks, clean Emery build, metadata and budgets.

Does not publish, tag, or change a version. Screenshot regression is optional
because it installs into and changes settings in the local Emery emulator.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--screenshots', action='store_true')
args = parser.parse_args()
def run(*command):
    subprocess.run(command, cwd=ROOT, check=True)

run(sys.executable, 'scripts/test.py')
run('pebble', 'clean')
result = subprocess.run(['pebble', 'build'], cwd=ROOT, text=True,
                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print(result.stdout)
result.check_returncode()
patterns = {'resources': r'Total size of resources:\s+(\d+)',
            'ram': r'Total footprint in RAM:\s+(\d+)',
            'heap': r'Free RAM available \(heap\):\s+(\d+)'}
metrics = {}
for key, pattern in patterns.items():
    match = re.search(pattern, result.stdout)
    assert match, f'Missing build metric: {key}'
    metrics[key] = int(match.group(1))
budgets = json.loads((ROOT / 'tests/build-budgets.json').read_text())
assert metrics['resources'] <= budgets['max_resources_bytes']
assert metrics['ram'] <= budgets['max_ram_bytes']
assert metrics['heap'] >= budgets['min_heap_bytes']
bundle = ROOT / 'build/pebble-orationes.pbw'
metrics['pbw'] = bundle.stat().st_size
assert metrics['pbw'] <= budgets['max_pbw_bytes']
with zipfile.ZipFile(bundle) as archive:
    info = json.loads(archive.read('appinfo.json'))
    assert info['targetPlatforms'] == ['emery']
    assert info['watchapp']['watchface'] is False
    assert 'pebble-js-app.js' in archive.namelist()
run('git', 'diff', '--check')
(ROOT / 'build/regression-metrics.json').write_text(json.dumps(metrics, indent=2) + '\n')
if args.screenshots:
    run('pebble', 'install', '--emulator', 'emery')
    run(sys.executable, 'scripts/capture_prayers.py', 'build/regression-screenshots',
        '--compare', 'tests/screenshots')
print('Release checks passed:', metrics)
