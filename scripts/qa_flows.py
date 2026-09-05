#!/usr/bin/env python3
"""Exercise Continue, reflow, exit/relaunch, and library pinning in Emery.

Installs the current bundle and changes emulator preferences. Requires Pillow.
Images are retained under build/qa-flows for visual review of menu transitions.
"""
import argparse
import json
from pathlib import Path
import subprocess
import time
from PIL import Image, ImageChops

parser=argparse.ArgumentParser()
parser.add_argument('--bounds-only',action='store_true')
args=parser.parse_args()

output = Path('build/qa-flows')
output.mkdir(parents=True, exist_ok=True)
keys = json.loads(Path('build/js/message_keys.json').read_text())
def run(*args):
    subprocess.run(['pebble', *args], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
def click(button, repeat=1, duration=100):
    run('emu-button', '--emulator', 'emery', '--duration', str(duration),
        '--repeat', str(repeat), '--interval', '150', 'click', button)
def settings(**values):
    run('send-app-message', '--emulator', 'emery', '--int', *[f'{keys[k]}={v}' for k, v in values.items()])
def capture(name):
    time.sleep(.3)
    path = output / f'{name}.png'
    run('screenshot', '--emulator', 'emery', '--no-open', str(path))
    return Image.open(path).convert('RGB')
def same_reading(a, b):
    # ScrollLayer quantizes very long styled documents to native coordinates.
    # Allow at most three pixels vertically, ignoring the scrolling indicator.
    w, h = a.size
    for shift in range(-3, 4):
        ac = a.crop((0, 5, w - 8, h - 8))
        bc = b.crop((0, 5 + shift, w - 8, h - 8 + shift))
        if ImageChops.difference(ac, bc).getbbox() is None:
            return True
    return False

if not args.bounds_only:
    for name, entry, appearance in [('preces', 1, 0), ('aspirations', 23, 1)]:
        run('install', '--emulator', 'emery')
        settings(**dict({f'MainMenuSlot{i}': entry if i == 1 else 0 for i in range(1, 8)},
                        TextSize=0, Appearance=appearance, AccentColor=0,
                        NoonReminderEnabled=0, RememberPlace=1))
        click('select')
        top = capture(f'{name}-top')
        click('down', repeat=9)
        scrolled = capture(f'{name}-scrolled')
        assert not same_reading(top, scrolled), f'{name} did not scroll'
        click('select', repeat=2)  # Existing double-Select exit.
        capture(f'{name}-watchface')
        run('install', '--emulator', 'emery')  # Relaunch, keeping persisted state.
        click('down')
        capture(f'{name}-continue-home')
        click('select')
        capture(f'{name}-continue-menu')
        click('select')
        resumed = capture(f'{name}-resumed')
        assert same_reading(scrolled, resumed), f'{name} did not resume its saved position'
        settings(TextSize=1)
        capture(f'{name}-extra-large')
        settings(TextSize=0)
        assert same_reading(scrolled, capture(f'{name}-reflow-restored')), f'{name} lost its paragraph during reflow'
        click('back')
        click('down')
        click('select')
        assert same_reading(top, capture(f'{name}-start-again')), f'{name} did not start again'
        click('back')
        click('back')
        print(f'{name}: scroll, exit/relaunch, resume, reflow, start again passed', flush=True)

    run('install', '--emulator', 'emery')
    settings(RememberPlace=0, Appearance=0)
    click('down')
    click('select')
    capture('library-categories')
    click('up')  # Wrap from Daily Prayer to Prayer Cards.
    click('select')
    capture('library-cards')
    click('down')  # Individual St. Josemaría card after the collection entry.
    click('select')
    capture('library-actions')
    click('down')
    click('select')
    capture('library-slots')
    click('select')  # First empty slot is preselected.
    capture('library-pin-confirmation')
    time.sleep(2)  # Brief confirmation dismisses back to the selected shortcut.
    capture('library-pinned-main')
    click('select')
    capture('library-pinned-prayer')
    click('back')
    print('Library browse and pin flow exercised; review build/qa-flows screenshots', flush=True)

    # All Prayers must remain usable with no favorites; the first empty slot is 1.
    run('install', '--emulator', 'emery')
    settings(**{f'MainMenuSlot{i}': 0 for i in range(1, 8)})
    click('select')  # All Prayers.
    click('select')  # Daily Prayer.
    click('select')  # Preces.
    click('down')
    click('select')  # Pin to main menu.
    capture('empty-library-first-slot')
    click('select')
    time.sleep(2)
    capture('empty-library-pinned-main')
    click('select')
    preces = capture('empty-library-open-preces')
    baseline = Image.open('tests/screenshots/preces-large.png').convert('RGB')
    assert ImageChops.difference(preces.crop((0,0,196,200)), baseline.crop((0,0,196,200))).getbbox() is None, 'Could not browse and pin with all slots empty'
    click('back')
    print('Empty favorites: library access, pin, and prayer opening passed', flush=True)

# Exercise the replacement progress indicator at both ends of long readers.
for name, entry, size, dark in [('preces-bounds',1,0,0),('litany-bounds',6,1,1)]:
    run('install','--emulator','emery')
    settings(**dict({f'MainMenuSlot{i}':entry if i==1 else 0 for i in range(1,8)},
                    TextSize=size,Appearance=dark,RememberPlace=0,AccentColor=0))
    click('select')
    top=capture(name+'-top')
    previous=top
    for attempt in range(8):
        click('down',duration=6000)
        bottom=capture(name+'-bottom')
        if ImageChops.difference(previous,bottom).getbbox() is None:
            break
        previous=bottom
    else:
        raise AssertionError('Reader never reached a stable bottom')
    assert not same_reading(top,bottom), 'Long press did not advance'
    click('down',duration=1500)
    assert ImageChops.difference(bottom,capture(name+'-clamped-bottom')).getbbox() is None, 'Blank overscroll at bottom'
    for attempt in range(8):
        click('up',duration=6000)
        if ImageChops.difference(top,capture(name+'-returned-top')).getbbox() is None:
            break
    else:
        raise AssertionError('Up did not restore the top')
    click('up')
    assert ImageChops.difference(top,capture(name+'-clamped-top')).getbbox() is None, 'Overscroll at top'
    click('back')
    print(name+': hold/release, bottom/top clamps, and return-to-top passed',flush=True)
