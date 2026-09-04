---
name: orationes-accessibility-audit
description: Review or validate Orationes accessibility on Pebble Time 2, including readability, contrast, clipping, themes, text sizes, scrolling, and glyphs. Use for accessibility-focused work rather than general feature QA.
---

# Orationes accessibility audit

Preserve the principle that comfortable reading is more important than text density.

## Inspect

- Review shared rendering in `prayer_screen.*`, `accessible_menu.*`, `app_theme.*`, `app_settings.*`, and all custom font resources before proposing per-screen fixes.
- Treat the current Large setting, 8 px prayer margin, 54 px menu rows, black/white body contrast, and bold title treatment as established behavior unless the user requests a redesign.
- Verify adaptive accents separately in Light and Dark; evaluate title text and band colors together, not color names in isolation.

## Test on Emery

- Large and Extra Large across the shortest and longest prayers.
- Preces and Litany from first line to final line, with no clipped ending or blank overscroll.
- Menu labels, selected/unselected contrast, long titles, prayer title centering, and body alignment.
- Touch scrolling, precise button taps, hold-to-scroll overlap, immediate release, rapid direction changes, Back, and the watchface shortcut.
- Every non-ASCII source character against the resource character set and visible glyph output.
- Reminder screens in both appearances and each accent where color is used.

Report exact geometry, fonts/resources, colors, and observed failures. Do not shorten or rewrite prayer content to solve layout problems, and do not claim wrist-distance readability until physically tested.

