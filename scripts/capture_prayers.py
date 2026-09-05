#!/usr/bin/env python3
"""Capture fixed reader states; install the desired PBW first to start at home.

Changes emulator settings only. Use --compare tests/screenshots for regression.
"""
import argparse
import json
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser()
parser.add_argument('output', type=Path)
parser.add_argument('--compare', type=Path)
args = parser.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
keys = json.loads(Path('build/js/message_keys.json').read_text())

def run(*command):
    subprocess.run(['pebble', *command], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def settings(**values):
    run('send-app-message', '--emulator', 'emery', '--int', *[f'{keys[k]}={v}' for k, v in values.items()])

for name, entry, size, appearance in [
    ('preces-large', 1, 0, 0), ('angelus-large', 4, 0, 0),
    ('aspirations-large', 23, 0, 0), ('aspirations-extra-dark', 23, 1, 1),
    ('card-extra-dark', 28, 1, 1),
]:
    settings(**dict({f'MainMenuSlot{i}': entry if i == 1 else 0 for i in range(1, 8)},
                    TextSize=size, Appearance=appearance, AccentColor=0,
                    NoonReminderEnabled=0, RememberPlace=0))
    run('emu-button', '--emulator', 'emery', 'click', 'select')
    time.sleep(.3)
    target = args.output / f'{name}.png'
    run('screenshot', '--emulator', 'emery', '--no-open', str(target))
    if args.compare:
        from PIL import Image, ImageChops
        actual = Image.open(target).convert('RGB')
        expected = Image.open(args.compare / target.name).convert('RGB')
        assert actual.size == expected.size and ImageChops.difference(actual, expected).getbbox() is None, f'Render changed: {name}'
    run('emu-button', '--emulator', 'emery', 'click', 'back')
    print(f'{name}: captured' + ('; exact match' if args.compare else ''), flush=True)
