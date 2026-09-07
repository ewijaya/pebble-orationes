# Orationes

[Latest release](https://github.com/ewijaya/pebble-orationes/releases/latest)

Your Pebble tells time. Orationes helps you make time for prayer.

**[Install from the Pebble App Store](https://apps.repebble.com/9882f741750c43eb8309777e)**

Orationes is a personal native Pebble C app built specifically for Pebble Time 2 (`emery`). It favors readability on a small display through large bold text, strong contrast, touchscreen scrolling, and hardware-button navigation.

<p align="center">
  <a href="https://apps.repebble.com/9882f741750c43eb8309777e"><img src="docs/images/store/orationes-banner-realistic-main-menu-master.png" alt="Make time for prayer. Catholic prayers for Pebble Time 2, with large text, clear contrast, and offline access. Orationes main menu shown on a watch mockup." width="960"></a>
</p>

## What's included

This checkout documents v0.10.1. See [Install](#install) for verified public availability.

- **Preces** — complete Latin text in a large-print, scrollable view.
- **Holy Rosary** — Today's Mysteries, All Mysteries, and the Litany of Loreto. The watch chooses Joyful (Mon/Sat), Sorrowful (Tue/Fri), Glorious (Wed/Sun), or Luminous (Thu) mysteries from its local weekday.
- **Regina Caeli** — English text in a scrollable view.
- **Angelus** — English text in a scrollable view, with each Hail Mary abbreviated as `Hail Mary ...`.
- **Memorare** — English text in a scrollable view.
- **Come, Holy Spirit** and **Litany of Humility** — English texts under All Prayers → Daily Prayer, also available as individual shortcuts.
- **More Prayers** *(optional)* — prayers for Mental Prayer, Visit & Communion, Before Work, and Night Examination.
- **Confession** *(optional)* — an examination of conscience, Act of Contrition, and prayers before and after Confession.
- **Prayer library** — Psalm 50 (51), Psalm 2, Acceptance of Death, Prayer for Vocations, Blessed Be Your Purity, and the Canticle of the Three Children.
- **Aspirations** *(optional)* — 92 short prayers, with italic Latin paired with English where supplied, and concise Scripture references.
- **Prayer Cards** *(optional)* — a collection of 14 intercessory prayers, also available as individual shortcuts. Supplied private-use declarations are retained in small print.

The default menu stays focused on the five core entries. On the watch or in the Pebble app, **Prayer Shortcuts** can fill up to seven main-menu slots from the wider library while preserving those defaults until changed. Settings also offer Large or Extra Large text, light and dark appearances, adaptive accents, and an optional noon Angelus/Regina Caeli reminder. Holding Up or Down moves quickly through long prayers; double-clicking Select exits directly to the watchface, and long menus wrap in both directions.

**Continue** opens your last saved prayer directly. **Recent Prayers** keeps separate reading places for up to eight prayers. Ordinary library and shortcut openings start at the top. **Remember Place** can be disabled in watch or phone Settings; doing so clears the saved places. **Continue First**, available in watch and phone Settings, can place Continue above your shortcuts and is Off by default.

In **All Prayers**, select a prayer to open it directly; hold Select and release for Open/Pin options. While reading, Select opens **Reading Options**, with **Start again** and **Jump to section** where sections are available. Responses and stanzas have clearer spacing without changing prayer wording. Settings recover from interrupted writes, and phone changes wait for confirmation from the watch before being marked saved.

**New in v0.10.0:** direct opening and Continue, section jumps, response/stanza formatting, eight-prayer history, optional Continue First, and a one-candidate release workflow. See the [reading refresh](docs/reading-refresh.md), [release notes](docs/releases/0.10.0.md), and [release workflow](docs/releasing.md).

**New in v0.10.1:** Continue First now synchronizes between Clay and the watch, preserving your existing preference. Clay displays the bundled app version automatically. See the [release notes](docs/releases/0.10.1.md). These changes require the v0.10.1 PBW; the earlier store build does not include them.

Version 0.9.0 added Come, Holy Spirit and Litany of Humility, including its concise, small-print attribution. See [release verification](docs/verification.md). Version 0.8.0 introduced independent navigation highlights, appearance previews, a cleaner reading progress indicator, and refreshed menus. See the [UI changes](docs/ui-refresh.md).

Selecting a prayer for a shortcut on the watch saves it and returns to the main menu with that prayer highlighted.

Browse the [prayer directory](docs/prayer-list.md) for the complete library, languages, and shortcut names.

## Install

**[Install Orationes from the Pebble App Store](https://apps.repebble.com/9882f741750c43eb8309777e)**

1. Open the Orationes listing.
2. Add it to your apps.
3. Install or sync it to your Pebble Time 2.

Orationes **v0.9.0** targets Pebble Time 2 (`emery`) and is available from GitHub Releases and the Pebble App Store.

For manual installation, download `pebble-orationes.pbw` from [GitHub Releases](https://github.com/ewijaya/pebble-orationes/releases) and install it through the Pebble/RePebble app workflow.

Developer-installed PBWs can appear without artwork in My Apps because the mobile
app treats them as local entries rather than store-linked entries. See
[My Apps artwork](docs/mobile-artwork.md) before removing a working installation.

## Development

<details>
<summary>Build, architecture, and screenshot workflow</summary>

### Build

Requires Pebble Tool 5.0.40 or newer and Pebble SDK 4.33.1 or a compatible current SDK.

```sh
pebble clean
pebble build
```

The application bundle is written to `build/pebble-orationes.pbw`.

```sh
pebble install --emulator emery
pebble install --cloudpebble build/pebble-orationes.pbw
```

See the [development guide](docs/development.md) for the generated catalog, regression tests, memory budgets, and emulator checks, and the [release verification](docs/verification.md) for results.

### Architecture

Orationes is an Emery-only native Pebble C SDK application. A small bundled PebbleKit JS companion provides phone-side settings; prayer content and normal use remain offline. A reusable prayer screen renders scrollable texts, a reusable accessible menu renderer provides high-contrast navigation, and prayer data is separated from UI code where practical. Builds produce a `.pbw` application bundle.

### Updating screenshots

Optional framed exports can be generated from the retained raw screenshots with Python 3 and Pillow:

```sh
python3 -m pip install Pillow
python3 scripts/frame_screenshots.py --input-dir docs/images/raw
```

Frames are written to `build/framed-screenshots/` and remain ignored by Git. With no arguments, the script reads fresh Emery captures from `build/emulator-screenshots/` and also copies them to `docs/images/raw/`. Use the native raw images for App Store screenshots.

</details>

## Disclaimer

Orationes is an independent personal project. It is not an official application of, or endorsed by, Opus Dei, the Catholic Church, Core Devices, or Pebble.
