#!/usr/bin/env python3
"""Exercise the menu refresh on Emery; changes emulator settings, retains screenshots.

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
out = Path('build/qa-menus'); out.mkdir(parents=True, exist_ok=True)
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


def thumb_y(image):
    # Only the thumb occupies x=196; the one-pixel track is at x=197.
    return [y for y in range(36, 224) if image.getpixel((196, y)) == (255, 255, 255)]


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
    # Reset native settings, including the watch-local layout preference.
    open_settings(); click('down', 10); click('select'); restart()
    settings(RememberPlace=0, ContinueFirst=0)
    spacious = capture('home-spacious')
    first = thumb_y(spacious)
    assert first and min(first) == 37
    click('up'); last = thumb_y(capture('home-wrap-bottom'))
    assert last and max(last) == 222 and min(last) > min(first)
    click('down'); assert capture('home-wrap-top').tobytes() == spacious.tobytes()
    click('down'); click('select'); capture('rosary-today')
    click('down'); click('select'); capture('all-mysteries')
    click('down', 3); capture('all-mysteries-last'); click('back'); click('back')
    open_settings(); assert_header_clearance(capture('settings-promoted-continue')); click('down'); click('select')
    restart(); compact = capture('home-compact')
    # Same font/color, six fewer pixels per short row, persisted after relaunch.
    assert spacious.getpixel((1, 83)) != compact.getpixel((1, 83))
    restart(); assert capture('compact-relaunched').tobytes() == compact.tobytes()
    for dark in (0, 1):
        for size in (0, 1):
            settings(Appearance=dark, TextSize=size)
            capture(f'compact-{dark}-{size}')
    settings(Appearance=1, TextSize=0, RememberPlace=1)
    restart(); click('select'); click('down', 5); click('back')
    click('down', 5); capture('continue-name')
    open_settings(); click('select'); click('back'); restart()
    promoted = capture('continue-first')
    click('select'); capture('continue-resumed'); click('back')
    click('up', 3); capture('recent-chevron'); click('select'); capture('recent-prayers'); click('back')
    settings(RememberPlace=0); restart()
    click('up', 2); click('select'); capture('library-categories')
    click('up'); click('select'); first_use = capture('library-first-use-or-dismissed')
    # Open help is recognized by its title band beginning at y=32, whereas the
    # prayer list header starts at y=0. Preserve repeated runs after dismissal.
    if first_use.getpixel((0, 0)) == (0, 0, 0):
        click('select')
    assert_header_clearance(capture('library-long-names')); click('down'); click('select', duration=.8)
    assert_header_clearance(capture('pin-options')); click('back'); click('back'); click('select')
    capture('library-hint-dismissed')
    open_settings(); click('down', 9); click('select'); capture('menu-help')
    time.sleep(2); assert capture('menu-help-still-visible').tobytes() == capture('menu-help').tobytes()
    click('select'); capture('menu-help-dismissed')
    settings(RememberPlace=0, MainMenuSlot1=37, ContinueFirst=1)
    settings(RememberPlace=1)
    restart(); click('select'); click('back'); restart()
    capture('continue-long-name-dark')
    settings(Appearance=0, TextSize=1)
    capture('continue-long-name-light-extra')
    # Return the emulator to its spacious defaults after exercising compact.
    restart(); click('up'); click('select'); click('down', 10); click('select'); restart()
    capture('restored-spacious')
    print('Menu wrap, indicator endpoints, compact persistence, themes/sizes, Continue, Rosary, pin options and Help exercised.', flush=True)
finally:
    service.shutdown(); pebble.transport.ws.close()
