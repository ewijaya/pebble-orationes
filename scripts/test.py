#!/usr/bin/env python3
"""Host regression checks; no watch, network, or Pebble SDK required."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
os.chdir(ROOT)
subprocess.run(["python3", "scripts/generate_catalog.py", "--check"], check=True)
for filename, expected in json.loads(Path("tests/content-sha256.json").read_text()).items():
    actual = hashlib.sha256(Path(filename).read_bytes()).hexdigest()
    assert actual == expected, f"Prayer source changed: {filename}; review wording before updating baseline"
with tempfile.TemporaryDirectory(prefix="orationes-tests-") as directory:
    executable = str(Path(directory) / "core")
    keys = Path(directory) / 'message_keys.h'
    names = json.loads(Path('package.json').read_text())['pebble']['messageKeys']
    keys.write_text('\n'.join(f'#define MESSAGE_KEY_{name} {index}' for index, name in enumerate(names)))
    sources = ["tests/test_core.c", "tests/test_phone.c", "tests/storage.c", "src/c/app_settings.c", "src/c/phone_settings.c",
               "src/c/durable_store.c", "src/c/reading_position.c", "src/c/reminder_schedule.c", "src/c/main_menu_catalog.c", "src/c/prayers.c", "src/c/prayer_collections.c", "src/c/prayer_cards.c", "src/c/aspirations.c", "src/c/liturgical_calendar.c", "src/c/rosary_data.c"]
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra",
                    "-Werror", "-Wno-sign-compare", "-Wno-unused-parameter", "-fsanitize=address,undefined", "-include", str(keys),
                    "-Itests/include", "-Itests", "-Isrc/c", *sources,
                    "-o", executable], check=True)
    subprocess.run([executable], check=True)
for script in sorted(Path("tests").glob("test_*.js")):
    subprocess.run(["node", str(script)], check=True)
print("Content integrity and host regression checks passed")
