#!/usr/bin/env python3
"""Capture the current UI for the store; changes only Emery emulator preferences."""
import json
from pathlib import Path
import subprocess
import time

output=Path('build/emulator-screenshots')
output.mkdir(parents=True,exist_ok=True)
keys=json.loads(Path('build/js/message_keys.json').read_text())
def run(*args):
    subprocess.run(['pebble',*args],check=True,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
def click(button,repeat=1):
    run('emu-button','--emulator','emery','--duration','100','--repeat',str(repeat),'--interval','150','click',button)
def settings(**values):
    run('send-app-message','--emulator','emery','--int',*[f'{keys[k]}={v}' for k,v in values.items()])
def capture(name):
    time.sleep(.35)
    run('screenshot','--emulator','emery','--no-open','--no-correction',str(output / (name+'.png')))
    print('Captured '+name,flush=True)

run('install','--emulator','emery')
settings(**dict({f'MainMenuSlot{i}': [1,2,4,5,0,0,0][i-1] for i in range(1,8)},
                Appearance=1,TextSize=0,AccentColor=0,NavigationHighlight=1,
                RememberPlace=0,NoonReminderEnabled=0))
capture('main-menu')
click('select');capture('preces');click('back')
click('down');click('select');capture('holy-rosary-menu');click('back')
click('down');click('select');settings(Appearance=0);capture('angelus')
settings(Appearance=1,RememberPlace=1)
click('down',5);click('back');click('down',2);capture('continue')
click('down');click('select');capture('all-prayers');click('back')
click('down');click('select');click('down',3);click('select');click('down',2)
capture('navigation-colors')
click('back');click('back');click('back')
capture('launcher-icon')
