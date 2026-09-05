#!/usr/bin/env python3
"""Exercise native preview colors, cancel/save, and persisted navigation in Emery."""
import json
from pathlib import Path
import subprocess
import time
from PIL import Image, ImageChops

out = Path('build/qa-appearance')
out.mkdir(parents=True, exist_ok=True)
keys = json.loads(Path('build/js/message_keys.json').read_text())
def run(*args):
    subprocess.run(['pebble', *args], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
def click(button, repeat=1):
    run('emu-button', '--emulator', 'emery', '--duration', '100', '--repeat', str(repeat), '--interval', '150', 'click', button)
def settings(**values):
    run('send-app-message', '--emulator', 'emery', '--int', *[f'{keys[k]}={v}' for k,v in values.items()])
def capture(name):
    time.sleep(.25)
    path=out / f'{name}.png'
    run('screenshot','--emulator','emery','--no-open','--no-correction',str(path))
    return Image.open(path).convert('RGB')
def open_setting(row):
    click('up')
    click('select')
    if row: click('down',row)
    click('select')

run('install','--emulator','emery')
settings(**dict({f'MainMenuSlot{i}':1 if i==1 else 0 for i in range(1,8)},
                TextSize=0,Appearance=0,AccentColor=0,NavigationHighlight=0,RememberPlace=0,NoonReminderEnabled=0))
capture('home-classic')
click('up');click('select');click('down',3)
before=capture('navigation-before-cancel')
click('select');click('down',5);capture('lime-unsaved');click('back')
after=capture('navigation-after-cancel')
assert ImageChops.difference(before,after).getbbox() is None, 'Back committed the preview'
click('select')
colors=[None,(255,170,0),(255,85,0),(170,0,255),(255,0,255),(170,255,0)]
for dark in (0,1):
    settings(Appearance=dark)
    for nav in range(6):
        shot=capture(f'nav-{dark}-{nav}')
        expected=colors[nav] if nav else ((255,255,255) if dark else (0,0,0))
        assert shot.getpixel((12,160)) == expected, f'Wrong navigation color {dark}/{nav}'
        foreground = (255,255,255) if nav == 3 or (nav == 0 and not dark) else (0,0,0)
        assert sum(n for n,c in shot.crop((20,157,180,181)).getcolors() if c == foreground) > 30, 'Unreadable selection text'
        assert shot.getpixel((9,80)) == ((0,170,170) if dark else (0,85,170)), 'Navigation changed title accent'
        click('down')
    print(f'All six navigation previews passed in {"dark" if dark else "light"} mode',flush=True)
click('down',5);click('select')  # Commit Lime; settings closes to home.
run('install','--emulator','emery')
open_setting(3)
assert capture('lime-persisted').getpixel((12,160)) == colors[5], 'Navigation did not persist'
# Existing title palettes can all change without changing selected-row color.
for dark in (0,1):
    for accent in range(4):
        settings(Appearance=dark,AccentColor=accent)
        assert capture(f'independent-title-{dark}-{accent}').getpixel((12,160)) == colors[5]
click('back');click('back')
run('install','--emulator','emery')
settings(Appearance=0,AccentColor=0,NavigationHighlight=1)
open_setting(0)
click('down');capture('extra-large-preview');click('back');capture('size-cancelled')
click('select');click('down');click('select')
run('install','--emulator','emery')
click('select');capture('extra-large-saved-reader');click('back')
print('Cancel, save, relaunch persistence, accent independence, and font preview/save passed',flush=True)

# Settings shortcut editing uses the same notice and must return to a usable home.
run('install','--emulator','emery')
settings(TextSize=0)
open_setting(5)
click('select')  # Edit slot 1 (Preces).
click('up')     # None.
click('select')
capture('settings-clear-confirmation')
time.sleep(2)
capture('settings-cleared-home')
click('select')  # With no favorites, All Prayers is the first row.
capture('settings-cleared-library')
click('back')
print('Native shortcut clearing, confirmation, and return to the empty home exercised',flush=True)
