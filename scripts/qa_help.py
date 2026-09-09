#!/usr/bin/env python3
"""Exercise selectable Help topics on Emery; changes emulator settings, retains screenshots.

Run with the Python interpreter installed with pebble-tool. Physical touch and
wrist-distance readability require a Pebble Time 2 and are not asserted here.
"""
import json
from pathlib import Path
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
from queue import Queue, Empty

keys = json.loads(Path('build/js/message_keys.json').read_text())
app_uuid = UUID(json.loads(Path('package.json').read_text())['pebble']['uuid'])
port = get_emulator_info('emery')['pypkjs']['port']
pebble = PebbleConnection(WebsocketTransport(f'ws://localhost:{port}/'))
pebble.connect(); pebble.run_async()
service = AppMessageService(pebble)
messages = Queue()
service.register_handler('appmessage', lambda _, uuid, data: messages.put(data) if uuid == app_uuid else None)
out = Path('build/qa-help'); out.mkdir(parents=True, exist_ok=True)
sequence = 9000


def settings(**values):
    global sequence
    sequence += 1
    service.send_message(app_uuid, {keys[k]: Int32(v) for k, v in dict(values, SettingsTransaction=sequence).items()})
    deadline = time.monotonic() + 8
    while time.monotonic() < deadline:
        try: data = messages.get(timeout=max(.01, deadline-time.monotonic()))
        except Empty: break
        if data.get(keys['SettingsAck']) == sequence:
            assert data[keys['SettingsStatus']] == 0
            time.sleep(.3)
            return data
    raise AssertionError('Watch did not acknowledge settings')


def restart():
    pebble.send_packet(AppRunState(data=AppRunStateStop(uuid=app_uuid))); time.sleep(.4)
    pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid))); time.sleep(.8)


def click(button, repeat=1, duration=.1):
    for _ in range(repeat):
        send_data_to_qemu(pebble.transport, QemuButton(state=EmuButtonCommand.BUTTON_MAP[button]))
        time.sleep(duration)
        send_data_to_qemu(pebble.transport, QemuButton(state=0)); time.sleep(.35)
    if button == 'select': time.sleep(.4)


def capture(name):
    rows = Screenshot(pebble).grab_image()
    image = Image.frombytes('RGB', (len(rows[0])//3, len(rows)), bytes(v for row in rows for v in row))
    image.save(out / f'{name}.png')
    return image


def assert_header_clearance(image):
    # A clear scanline before the two-pixel divider protects g/y/p descenders.
    background = image.getpixel((0, 0))
    band_end = next(y for y in range(1, image.height)
                    if image.getpixel((0, y)) != background)
    assert all(image.getpixel((x, band_end - 1)) == background
               for x in range(8, 186)), 'Header text reaches the divider'


def open_settings():
    restart(); click('up'); click('select')


try:
    for dark, size in ((1, 0), (0, 1)):
        restart(); settings(Appearance=dark, TextSize=size, CompactMenus=1, RememberPlace=0)
        open_settings(); click('down', 9); click('select')
        topics = capture(f'{dark}-{size}-topics')
        assert_header_clearance(topics)
        for topic in range(8):
            click('select')
            top = capture(f'{dark}-{size}-{topic}-top')
            assert_header_clearance(top)
            click('down', duration=3)
            bottom = capture(f'{dark}-{size}-{topic}-bottom')
            click('down', 2)
            assert capture(f'{dark}-{size}-{topic}-bottom-clamped').tobytes() == bottom.tobytes()
            click('up', duration=3)
            assert capture(f'{dark}-{size}-{topic}-top-return').tobytes() == top.tobytes()
            click('select' if topic % 2 else 'back')
            click('down')
        # Eight topics wrap back to Navigation; neither Help nor reading it makes a bookmark.
        capture(f'{dark}-{size}-topics-wrapped'); click('select')
        assert capture(f'{dark}-{size}-wrapped-navigation').tobytes() == Image.open(out / f'{dark}-{size}-0-top.png').tobytes()
        click('back'); click('back'); click('back')
        print(f'{dark=}, {size=}: eight Help topics, scrolling clamps, Select/Back and menu wrap passed', flush=True)
    # Live phone theme/text-size updates while a Help topic is open.
    open_settings(); click('down', 9); click('select'); click('down', 3); click('select')
    settings(Appearance=1, TextSize=0); assert_header_clearance(capture('pin-help-live-dark-large'))
    settings(Appearance=0, TextSize=1); assert_header_clearance(capture('pin-help-live-light-extra'))
    click('back'); click('back'); click('back')
finally:
    service.shutdown(); pebble.transport.ws.close()
