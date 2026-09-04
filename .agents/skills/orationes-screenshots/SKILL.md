---
name: orationes-screenshots
description: Capture, frame, refresh, and validate Orationes Emery screenshots for README, GitHub, or Pebble App Store presentation. Do not change runtime UI merely to improve documentation images.
---

# Orationes screenshots

Maintain one repeatable visual set without committing the build directory.

## Capture

- Use the Emery emulator and inspect CLI help before assuming screenshot or input syntax.
- Capture intentional known states. The standard set is `launcher-icon.png`, `main-menu.png`, `angelus.png`, `preces.png`, and `holy-rosary-menu.png` under `build/emulator-screenshots/`.
- Prefer captures that demonstrate current appearance features while keeping text readable. Avoid cursor artifacts, unintended selection, clipped titles, or stale UI.
- Preserve the screenshot's aspect ratio and native pixels. Do not retouch prayer text.

## Frame and validate

- Run `python3 scripts/frame_screenshots.py` before `pebble clean`; clean removes the build captures.
- If Pillow is missing, use a temporary virtual environment or document `python3 -m pip install Pillow`. Do not add a runtime dependency to the app.
- Confirm matching copies under `docs/images/raw/` and 600x800 composites under `docs/images/framed/`.
- Visually inspect every raw and framed result. Regeneration may produce no Git diff for unchanged images; report that rather than forcing binary changes.
- Keep `build/` ignored and stage only deliberate documentation assets.

For App Store upload, use raw captures copied to temporary files named like `emery_01_main-menu.png`; do not upload framed watch mockups as native screenshots.

