# Orationes

Orationes is an offline native Pebble C application for Pebble Time 2
(`emery`). It will provide a small collection of Catholic prayers using the
watch's native interface.

The initial version contains a menu for Angelus, Regina Caeli, Holy Rosary,
and Preces. Each item currently opens a placeholder screen; prayer text will
be added in a later version.

## Requirements

- Pebble Tool 5.0.40 or newer
- Pebble SDK 4.33.1

## Build

```sh
pebble build
```

The application bundle is written to `build/pebble-orationes.pbw`.

## Run in the emulator

```sh
pebble install --emulator emery
```

The app is intentionally C-only and requires no PebbleKit JS, phone
companion, account, or network connection.

## Updating screenshots

Install [Pillow](https://pypi.org/project/pillow/) with
`python3 -m pip install Pillow`, place Emery captures in
`build/emulator-screenshots/`, then run:

```sh
python3 scripts/frame_screenshots.py
```

Raw copies are written to `docs/images/raw/`; consistently framed showcase
images are written to `docs/images/framed/`.
