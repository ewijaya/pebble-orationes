---
name: orationes-screenshots
description: Captures, frames, refreshes, and validates Orationes Emery screenshots for the README, GitHub, or Pebble App Store. Does not change runtime UI just to improve documentation images.
---

# Orationes screenshots

Maintain one repeatable set of documentation images without committing
`build/`.

## Capture

- Use the Emery emulator with a fresh build installed. Check CLI help before
  assuming screenshot or input syntax.
- `scripts/capture_store.py` drives the standard store set into
  `build/emulator-screenshots/` (main menu, Preces, Holy Rosary menu, Angelus,
  Continue, All Prayers, navigation colors, launcher icon, Come Holy Spirit,
  Litany of Humility). It takes no arguments and starts capturing as soon as it
  runs — even `--help` changes emulator state.
- Capture intentional, known states that show current appearance features
  while staying readable. Reject captures with cursor artifacts, unintended
  selection, clipped titles, or stale UI.
- Keep native pixels and aspect ratio. Never retouch prayer text.

## Frame and validate

- Run `scripts/frame_screenshots.py` with the Pebble Tool Python (it has
  Pillow; `head -1 "$(which pebble)"` shows its path) before `pebble clean`,
  which deletes `build/`. Use `--input-dir docs/images/raw` to reframe the
  existing raw set.
- It writes raw copies to `docs/images/raw/` and 600×800 composites to
  `build/framed-screenshots/`. Frames are generated exports, not checked-in
  assets.
- Look at every raw and framed image yourself. Regeneration can produce no Git
  diff for unchanged images — report that rather than forcing binary churn.
- Stage only deliberate documentation assets; keep `build/` ignored.

For the App Store, upload raw captures copied to temporary files named like
`emery_01_main-menu.png` — never framed mockups — and follow the
`orationes-appstore` skill, since replacing store screenshots needs explicit
authorization.
