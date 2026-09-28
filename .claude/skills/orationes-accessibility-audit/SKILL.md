---
name: orationes-accessibility-audit
description: Audits Orationes readability on Pebble Time 2 (Emery) — contrast, clipping, themes, text sizes, scrolling, touch, and glyph coverage. Use for accessibility-focused review, not general feature QA.
---

# Orationes accessibility audit

The app's guiding principle is that comfortable reading matters more than text
density. Many users read prayers at arm's length, in dim light, or with weaker
eyesight, so a denser layout that fits more words is usually a regression.

## Inspect shared rendering first

Accessibility problems in this app almost always live in shared code, so fix
them there rather than per screen:

- `src/c/prayer_screen.*`, `prayer_document.*` (paragraph segmentation, response
  insets, stanza spacing), `accessible_menu.*`, `navigation_menu.*`,
  `app_theme.*`, `app_fonts.*`, `app_settings.*`.
- Every font resource in `package.json` and its `characterRegex`.

Treat these as established behavior unless the user asks for a redesign: the
Large default, 8 px prayer margin, measured menu rows, black/white body text,
bold titles, and the right-edge progress indicator.

Evaluate accent and Navigation Highlight colors in both Light and Dark by
looking at the title text and band together. A color name that sounds high
contrast can still fail against a specific band.

## Test on Emery

- Large and Extra Large across the shortest and longest prayers.
- Preces and the Litany of Loreto from first line to final line: no clipped
  ending and no blank overscroll.
- Menu labels, selected vs. unselected contrast, long titles, Compact Menus,
  title centering, and body alignment.
- Touch scrolling, precise button taps, hold-to-scroll, immediate release,
  rapid direction changes, Back, and double-Select to the watchface.
- Every non-ASCII source character against the font's `characterRegex` and the
  rendered glyph.
- Reminder and Help screens in both appearances and each accent where color is
  used.

`scripts/qa_appearance.py` and `scripts/qa_reading.py --full` cover much of this
matrix; run them with the Pebble Tool Python, which has Pillow
(`head -1 "$(which pebble)"` shows its path).

## Report

Give exact geometry, fonts/resources, colors, and observed failures, with
screenshots where they help. Do not shorten or rewrite prayer text to solve a
layout problem — the wording is fixed content. Emulator evidence does not prove
wrist-distance readability, so say which checks still need the physical watch.
