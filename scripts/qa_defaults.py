#!/usr/bin/env python3
"""Verify the single settings reset through native UI and phone protocol; Emery only."""
import json
from pathlib import Path
from queue import Queue, Empty
import subprocess
import time
from uuid import UUID
from PIL import Image
from pebble_tool.sdk.emulator import get_emulator_info
from pebble_tool.commands.emucontrol import send_data_to_qemu, EmuButtonCommand
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.communication.transports.qemu.protocol import QemuButton
from libpebble2.protocol.apps import AppRunState, AppRunStateStart, AppRunStateStop
from libpebble2.services.appmessage import AppMessageService, Int32
from libpebble2.services.screenshot import Screenshot

subprocess.run(['pebble', 'install', '--emulator', 'emery'], check=True)
keys = json.loads(Path('build/js/message_keys.json').read_text())
app_uuid = UUID(json.loads(Path('package.json').read_text())['pebble']['uuid'])
port = get_emulator_info('emery')['pypkjs']['port']
pebble = PebbleConnection(WebsocketTransport(f'ws://localhost:{port}/'))
pebble.connect(); pebble.run_async()
service = AppMessageService(pebble)
messages = Queue()
service.register_handler('appmessage', lambda _, uuid, data: messages.put(data) if uuid == app_uuid else None)
out = Path('build/qa-defaults'); out.mkdir(parents=True, exist_ok=True)
sequence = 8000

def wait_for(predicate):
    deadline = time.monotonic() + 8
    while time.monotonic() < deadline:
        try: data = messages.get(timeout=max(.01, deadline-time.monotonic()))
        except Empty: break
        if predicate(data): return data
    raise AssertionError('No matching watch settings snapshot')

def settings(**values):
    global sequence
    sequence += 1
    service.send_message(app_uuid, {keys[k]: Int32(v) for k, v in dict(values, SettingsTransaction=sequence).items()})
    data = wait_for(lambda d: d.get(keys['SettingsAck']) == sequence)
    assert data[keys['SettingsStatus']] == 0
    time.sleep(.4)
    return data

def restart():
    pebble.send_packet(AppRunState(data=AppRunStateStop(uuid=app_uuid))); time.sleep(.6)
    pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid))); time.sleep(1.2)

def click(button, repeat=1):
    for _ in range(repeat):
        send_data_to_qemu(pebble.transport, QemuButton(state=EmuButtonCommand.BUTTON_MAP[button]))
        time.sleep(.1)
        send_data_to_qemu(pebble.transport, QemuButton(state=0)); time.sleep(.4)

def capture(name):
    rows = Screenshot(pebble).grab_image()
    image = Image.frombytes('RGB', (len(rows[0])//3, len(rows)), bytes(v for row in rows for v in row))
    image.save(out / f'{name}.png')
    return image

defaults = dict(Appearance=1, AccentColor=0, NavigationHighlight=5,
                TextSize=0, RememberPlace=1, ContinueFirst=0,
                NoonReminderEnabled=0, NoonReminderDuration=1,
                **{f'MainMenuSlot{i}': i if i <= 5 else 0 for i in range(1,8)})

def check_defaults(data):
    for key, value in defaults.items():
        assert data[keys[key]] == value, (key, data)

try:
    for size in (0, 1):
        restart()
        settings(**dict({f'MainMenuSlot{i}': 1 if i == 1 else 0 for i in range(1,8)},
                        Appearance=0, AccentColor=2, NavigationHighlight=3, TextSize=size,
                        RememberPlace=0, ContinueFirst=1, NoonReminderEnabled=1,
                        NoonReminderDuration=2))
        click('up'); click('select'); click('down', 9)
        footer = capture(f'{size}-version-footer')
        click('select')  # Version is read-only, not another reset action.
        assert capture(f'{size}-version-selected').tobytes() == footer.tobytes()
        click('up')
        capture(f'{size}-reset-control')
        click('select')
        data = wait_for(lambda d: d.get(keys['Appearance']) == 1 and
                        d.get(keys['NavigationHighlight']) == 5)
        check_defaults(data)
        restart(); check_defaults(settings())
        image = capture(f'{size}-reset-home')
        colors = {color for _, color in image.getcolors(200*228)}
        assert {(0,170,170), (170,255,0), (0,0,0), (255,255,255)} <= colors
        # Full draft payload sent by Clay's Restore Defaults + Save Settings.
        settings(Appearance=0, AccentColor=1, NavigationHighlight=2, TextSize=1,
                 RememberPlace=0, ContinueFirst=1, NoonReminderEnabled=1,
                 NoonReminderDuration=0, MainMenuSlot1=40)
        check_defaults(settings(**defaults))
        restart(); check_defaults(settings())
        print(f'{size=}: single native/phone reset, all defaults, colors and relaunch passed', flush=True)
finally:
    service.shutdown(); pebble.transport.ws.close()
