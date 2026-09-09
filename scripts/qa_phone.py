#!/usr/bin/env python3
"""Continue First and Compact Menus protocol/UI checks. Changes only Emery emulator settings."""
import json
from pathlib import Path
from queue import Empty, Queue
import subprocess
import time
from uuid import UUID

from PIL import Image
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.communication.transports.qemu.protocol import QemuButton
from libpebble2.protocol.apps import AppRunState, AppRunStateStart, AppRunStateStop
from libpebble2.services.appmessage import AppMessageService, Int32
from libpebble2.services.screenshot import Screenshot
from pebble_tool.sdk.emulator import get_emulator_info
from pebble_tool.commands.emucontrol import send_data_to_qemu, EmuButtonCommand
from pebble_tool.commands.screenshot import ScreenshotCommand

subprocess.run(['pebble', 'install', '--emulator', 'emery'], check=True)
keys = json.loads(Path('build/js/message_keys.json').read_text())
app_uuid = UUID(json.loads(Path('package.json').read_text())['pebble']['uuid'])
port = get_emulator_info('emery')['pypkjs']['port']
pebble = PebbleConnection(WebsocketTransport(f'ws://localhost:{port}/'))
pebble.connect()
pebble.run_async()
service = AppMessageService(pebble)
messages = Queue()
service.register_handler('appmessage', lambda _, uuid, data:
                         messages.put(data) if uuid == app_uuid else None)
out = Path('build/qa-phone')
out.mkdir(parents=True, exist_ok=True)
sequence = 7000


def wait_for(predicate):
    deadline = time.monotonic() + 8
    while time.monotonic() < deadline:
        try:
            data = messages.get(timeout=max(.01, deadline - time.monotonic()))
        except Empty:
            break
        if predicate(data):
            return data
    raise AssertionError('No matching durable settings snapshot from watch')


def settings(status=0, **values):
    global sequence
    sequence += 1
    service.send_message(app_uuid, {keys[k]: Int32(v) for k, v in
                                  dict(values, SettingsTransaction=sequence).items()})
    data = wait_for(lambda data: data.get(keys['SettingsAck']) == sequence)
    assert data[keys['SettingsStatus']] == status, data
    return data


def click(button, repeat=1):
    for _ in range(repeat):
        send_data_to_qemu(pebble.transport, QemuButton(state=EmuButtonCommand.BUTTON_MAP[button]))
        time.sleep(.1)
        send_data_to_qemu(pebble.transport, QemuButton(state=0))
        time.sleep(.35)


def restart():
    pebble.send_packet(AppRunState(data=AppRunStateStop(uuid=app_uuid)))
    time.sleep(.4)
    pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid)))
    time.sleep(.8)


def capture(name):
    rows = ScreenshotCommand()._correct_colours(Screenshot(pebble).grab_image())
    Image.frombytes('RGB', (len(rows[0]) // 3, len(rows)),
                    bytes(v for row in rows for v in row)).save(out / f'{name}.png')


try:
    for size in (0, 1):
        for dark in (0, 1):
            restart()
            values = {f'MainMenuSlot{i}': 0 for i in range(1, 8)}
            data = settings(**dict(values, TextSize=size, Appearance=dark,
                                   RememberPlace=0, ContinueFirst=0, CompactMenus=0,
                                   NoonReminderEnabled=0, NoonReminderDuration=1,
                                   AccentColor=0, NavigationHighlight=0))
            assert data[keys['ContinueFirst']] == 0
            click('down')
            click('select')  # Settings after All Prayers, no shortcuts/history.
            capture(f'{size}-{dark}-off')
            click('select')  # Actual native toggle must proactively update phone.
            wait_for(lambda data: data.get(keys['ContinueFirst']) == 1)
            capture(f'{size}-{dark}-watch-on')
            data = settings(ContinueFirst=0)
            assert data[keys['ContinueFirst']] == 0
            capture(f'{size}-{dark}-phone-off')  # Open native Settings redraws.
            data = settings(ContinueFirst=1)
            assert data[keys['ContinueFirst']] == 1
            data = settings(status=1, ContinueFirst=2, Appearance=1-dark)
            assert data[keys['ContinueFirst']] == 1 and data[keys['Appearance']] == dark
            click('back')
            capture(f'{size}-{dark}-back')
            restart()
            data = settings()  # Empty batch returns persisted values without changing them.
            assert data[keys['ContinueFirst']] == 1
            assert data[keys['TextSize']] == size and data[keys['Appearance']] == dark
            click('up'); click('select'); click('down')
            click('select')  # Compact toggle closes Settings and sends snapshot.
            wait_for(lambda data: data.get(keys['CompactMenus']) == 1)
            capture(f'{size}-{dark}-compact-watch-on')
            restart()
            assert settings()[keys['CompactMenus']] == 1
            click('up'); click('select')
            data = settings(CompactMenus=0)
            assert data[keys['CompactMenus']] == 0
            capture(f'{size}-{dark}-compact-phone-off')
            data = settings(CompactMenus=1)
            assert data[keys['CompactMenus']] == 1
            capture(f'{size}-{dark}-compact-phone-on')
            data = settings(status=1, CompactMenus=2, Appearance=1-dark)
            assert data[keys['CompactMenus']] == 1 and data[keys['Appearance']] == dark
            click('back'); restart()
            assert settings()[keys['CompactMenus']] == 1
            print(f'{size=}, {dark=}: phone/watch toggle, durable ACK, invalid batch, Back, relaunch passed', flush=True)
finally:
    service.shutdown()
    pebble.transport.ws.close()
