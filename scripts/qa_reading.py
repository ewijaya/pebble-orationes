#!/usr/bin/env python3
"""Reader/navigation integration checks. Changes ONLY Emery emulator settings."""
import argparse
import atexit
import json
from pathlib import Path
import subprocess
import time
from threading import Event
from uuid import UUID
from PIL import Image, ImageChops
from pebble_tool.sdk.emulator import get_emulator_info
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.protocol.apps import AppRunState, AppRunStateStart, AppRunStateStop, AppRunStateRequest
from libpebble2.services.appmessage import AppMessageService, Int32
from libpebble2.services.screenshot import Screenshot
from libpebble2.communication.transports.qemu.protocol import QemuButton
from pebble_tool.commands.emucontrol import send_data_to_qemu, EmuButtonCommand
from pebble_tool.commands.screenshot import ScreenshotCommand

parser = argparse.ArgumentParser()
parser.add_argument('--full', action='store_true', help='Exercise all four size/appearance combinations')
case_settings = {'large-light': (0, 0), 'large-dark': (0, 1),
                 'extra-light': (1, 0), 'extra-dark': (1, 1)}
parser.add_argument('--case', action='append', choices=case_settings,
                    help='Focused size/appearance case; repeat as needed (overrides --full)')
parser.add_argument('--entries', nargs='+', type=int, default=[1, 4, 6, 23, 39, 40])
parser.add_argument('--matrix-only', action='store_true', help='Skip the separate navigation/history flows')
parser.add_argument('--flows-only', action='store_true', help='Only navigation/history checks')
parser.add_argument('--resume', action='store_true', help='Automatic-resume regression checks only')
args = parser.parse_args()
out = Path('build/qa-reading')
out.mkdir(parents=True, exist_ok=True)
keys = json.loads(Path('build/js/message_keys.json').read_text())
def run(*command):
    result = subprocess.run(['pebble', *command], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True)
    if result.returncode:
        raise RuntimeError(f'{command[0]} failed: {result.stderr.strip()}')
def wait_running(expected):
    deadline = time.monotonic() + 10
    while time.monotonic() < deadline:
        state = pebble.send_and_read(AppRunState(data=AppRunStateRequest()), AppRunState, timeout=5)
        running = isinstance(state.data, AppRunStateStart) and state.data.uuid == app_uuid
        if running == expected:
            return
        time.sleep(.2)
    raise AssertionError(f'Emulator did not reach app running={expected}')

def restart():
    # Relaunch the installed artifact, rather than repeatedly rewriting emulator flash.
    pebble.send_packet(AppRunState(data=AppRunStateStop(uuid=app_uuid)))
    wait_running(False)
    time.sleep(.4)
    pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid)))
    wait_running(True)
    time.sleep(.8)
def click(button, repeat=1, duration=100):
    for i in range(repeat):
        if i: time.sleep(.15)
        send_data_to_qemu(pebble.transport, QemuButton(state=EmuButtonCommand.BUTTON_MAP[button]))
        time.sleep(duration / 1000)
        send_data_to_qemu(pebble.transport, QemuButton(state=0))
    # Reader Select waits for the SDK's double-click timeout before opening
    # options. A screenshot request itself need not wait for that callback.
    time.sleep(.85 if button == 'select' else .35)
def settings(**values):
    service = AppMessageService(pebble)
    acknowledged = Event()
    service.register_handler('ack', lambda *_: acknowledged.set())
    try:
        service.send_message(app_uuid, {keys[k]: Int32(v) for k, v in values.items()})
        assert acknowledged.wait(5), 'Settings were not acknowledged'
        time.sleep(.4)
    finally:
        service.shutdown()
def capture(name):
    path = out / (name + '.png')
    rows = ScreenshotCommand()._correct_colours(Screenshot(pebble).grab_image())
    result = Image.frombytes('RGB', (len(rows[0]) // 3, len(rows)), bytes(v for row in rows for v in row))
    result.save(path)
    return result
def dismiss_library_help():
    # On fresh storage, dismiss the first-use notice before selecting a prayer.
    image = capture('library-first-use-check')
    if image.getpixel((0, 0)) in ((0, 0, 0), (255, 255, 255)):
        click('select')

def same(a, b):
    return ImageChops.difference(a, b).getbbox() is None
def same_anchor(a, b):
    # Fraction -> integer-pixel -> fraction reflow can round by one pixel.
    # Require every reader pixel to match, allowing only that vertical shift;
    # exclude the independently rounded 4px progress indicator.
    return any(same(a.crop((0, 1, 196, 227)), b.crop((0, 1+dy, 196, 227+dy)))
               for dy in (-1, 0, 1))
def start(entry, size=0, dark=0, remember=0):
    restart()
    settings(**dict({f'MainMenuSlot{i}': entry if i == 1 else 0 for i in range(1, 8)},
                    TextSize=size, Appearance=dark, RememberPlace=0, NoonReminderEnabled=0,
                    AccentColor=0, NavigationHighlight=0, ContinueFirst=0))
    settings(RememberPlace=remember)

run('install', '--emulator', 'emery')
port = get_emulator_info('emery')['pypkjs']['port']
pebble = PebbleConnection(WebsocketTransport(f'ws://localhost:{port}/'))
pebble.connect(); pebble.run_async()
atexit.register(pebble.transport.ws.close)
app_uuid = UUID('9f17d477-cef1-4512-8536-f01d50bb07a3')
if args.resume:
    combinations = ([case_settings[name] for name in args.case] if args.case else
                    list(case_settings.values()) if args.full else [(0, 0)])
    for entry in args.entries:
        for size, dark in combinations:
            label = f'resume-{entry}-{size}-{dark}'
            start(entry, size, dark, remember=1)
            click('select'); top = capture(label + '-new-top')
            click('down', repeat=7); middle = capture(label + '-saved'); click('back')
            assert not same(top, middle), label + ': test prayer did not scroll'
            click('select')
            assert same_anchor(middle, capture(label + '-shortcut')), label + ': shortcut lost bookmark'
            click('select', repeat=2); restart(); click('select')
            assert same_anchor(middle, capture(label + '-relaunched')), label + ': relaunch lost bookmark'
            # The final Reading Options row is Start again (also for one-row menus).
            click('select'); click('up'); click('select')
            assert same(top, capture(label + '-start-again')), label + ': explicit restart failed'
            click('back'); click('select')
            assert same(top, capture(label + '-restart-saved')), label + ': explicit restart was not saved'
            click('down', repeat=7); click('back')
            settings(RememberPlace=0); restart(); click('select')
            assert same(top, capture(label + '-disabled')), label + ': Off resumed old history'
            click('down', repeat=7); click('back'); click('select')
            assert same(top, capture(label + '-disabled-reopen')), label + ': Off saved new history'
            click('back'); settings(RememberPlace=1); restart(); click('select')
            assert same(top, capture(label + '-reenabled')), label + ': cleared bookmark returned'
            print(label + ': direct resume, relaunch, Start again, Off and history clearing passed', flush=True)

    # A bookmark saved through a shortcut must also work through library Open.
    start(1, remember=1)
    click('select'); click('down', repeat=7); saved = capture('resume-library-saved'); click('back')
    click('down', repeat=3); click('select')  # All Prayers, after Continue and Recent.
    click('select'); dismiss_library_help(); click('select')  # Daily Prayer -> Preces.
    assert same_anchor(saved, capture('resume-library-direct')), 'Library did not reuse shortcut bookmark'
    click('back'); click('select', duration=800); click('select')
    assert same_anchor(saved, capture('resume-library-options')), 'Library Open action lost bookmark'
    print('All Prayers direct and held Open share the shortcut bookmark', flush=True)

    # Collection children and the Rosary's Loreto entry use the same saved identity.
    for entry, parent, row in [(8, 7, 0), (25, 24, 0), (6, 2, 2)]:
        start(entry, remember=1)
        click('select'); top = capture(f'resume-submenu-{entry}-top')
        click('down', repeat=7); saved = capture(f'resume-submenu-{entry}-saved'); click('back')
        assert not same(top, saved), 'Submenu test prayer did not scroll'
        settings(MainMenuSlot1=parent)
        click('select')
        if row: click('down', repeat=row)
        click('select')
        assert same_anchor(saved, capture(f'resume-submenu-{entry}-restored')), 'Submenu lost bookmark'
        click('back'); click('back'); settings(RememberPlace=0); restart()
        click('select')
        if row: click('down', repeat=row)
        click('select')
        assert same(top, capture(f'resume-submenu-{entry}-off')), 'Submenu resumed with Remember Place Off'
    print('More Prayers, Prayer Cards and Rosary/Loreto obey Remember Place On/Off', flush=True)
    raise SystemExit(0)

for entry in ([] if args.flows_only else args.entries):
    combinations = ([case_settings[name] for name in args.case] if args.case else
                    list(case_settings.values()) if args.full else [(0, 0)])
    for size, dark in combinations:
        label = f'{entry}-{size}-{dark}'
        start(entry, size, dark)
        click('select'); top = capture(label + '-top')
        click('select'); capture(label + '-options')
        click('select'); capture(label + '-sections')
        click('up'); click('select'); jumped = capture(label + '-jump-last')
        assert not same(top, jumped), label + ': jump did not move'
        click('down', duration=6000); bottom = capture(label + '-bottom')
        click('down', duration=1500)
        assert same(bottom, capture(label + '-clamp')), label + ': bottom not stable'
        click('select'); click('down'); click('select')
        assert same(top, capture(label + '-start-again')), label + ': restart failed'
        click('up'); assert same(top, capture(label + '-top-clamp'))
        click('down', repeat=7); middle = capture(label + '-middle')
        settings(TextSize=1-size); settings(TextSize=size)
        assert same_anchor(middle, capture(label + '-reflow')), label + ': font round trip lost anchor'
        click('back'); click('select')
        assert same(top, capture(label + '-reopened'))
        click('select', repeat=2)
        assert not same(top, capture(label + '-exited')), 'Double Select failed'
        print(label + ': sections, bounds, restart, reflow, reopen, exit passed', flush=True)

if args.matrix_only:
    raise SystemExit(0)

# Direct opening and release-of-hold pinning are different actions.
start(0)
click('select'); click('select'); dismiss_library_help(); capture('library-header'); click('up')
capture('library-humility-selected')
click('select'); capture('library-direct-open'); click('back')
click('select', duration=800); capture('library-held-options')
click('down'); click('select'); capture('pin-slot'); click('select')
time.sleep(2); capture('pinned-home')
click('select'); capture('pinned-prayer'); click('back')
print('Library direct open, held options, pin, and pinned open exercised', flush=True)

# Two distinct positions survive opening other prayers and process relaunch.
start(1, remember=1)
click('select'); click('down', repeat=7); preces = capture('preces-saved'); click('back')
settings(MainMenuSlot1=40)
click('select'); click('down', repeat=9); humility = capture('humility-saved'); click('back')
click('down'); click('select')
assert same(humility, capture('continue-direct'))
click('select', repeat=2); restart()
click('down', repeat=2); click('select'); capture('recent-list'); click('down'); click('select')
assert same(preces, capture('recent-preces-resumed'))
click('back'); click('back')
# Settings is last; toggle the first Continue First option.
click('down', repeat=2); click('select'); capture('continue-first-off'); click('select'); capture('continue-first-on')
click('back'); click('down')  # Wrap Settings to the newly first Continue row.
capture('continue-first-home'); click('select')
assert same(preces, capture('continue-first-resumed'))
click('select', repeat=2); restart()
capture('continue-first-relaunched'); click('select')
assert same(preces, capture('continue-first-persisted'))
click('back'); click('up'); click('select'); click('select'); click('back')
print('Independent history, direct Continue, recent selection, Continue First and persistence passed', flush=True)
