# Current Orationes screenshots

Refreshed 2026-09-05 from the final v0.8.0 release candidate, also installed on PT2.
PBW SHA-256: `47dc0aa94b30dd61e95d4f906e248b59258486dac9628439dd77fef44c10aadd`.
The demonstration menu uses Preces, Holy Rosary, Angelus, and Memorare; it does
not change the app's five default shortcuts or physical watch preferences.

Every PNG is exactly **200×228 pixels**, captured directly in Emery. Native
framebuffer colors (`--no-correction`) keep the actual app palette intact;
no prayer text, screen pixels, or layout were retouched. Physical display color
and lighting can differ from these digital screenshots.

## Suggested App Store order

| Order | File | What it shows |
| --- | --- | --- |
| **1** | **[main-menu.png](main-menu.png)** | Orationes branding, cyan header, amber selection, and Preces, Holy Rosary, Angelus, and Memorare. Use as the lead screenshot. |
| 2 | [preces.png](preces.png) | Latin prayer reading in dark mode, with the new clean progress indicator. |
| 3 | [rosary-menu.png](rosary-menu.png) | Today's Mysteries, All Mysteries, and Litany of Loreto. |
| 4 | [continue.png](continue.png) | Resume a saved prayer, with its name and bookmark icon. |
| 5 | [all-prayers.png](all-prayers.png) | Browse prayer categories with distinct icons. |
| 6 | [navigation-colors.png](navigation-colors.png) | Violet selection and cyan title shown independently in the live picker. |
| 7 | [angelus.png](angelus.png) | A clear English prayer screen in light mode. |

If using only four images, choose main-menu, preces, rosary-menu, and
navigation-colors. `launcher.png` is a current system-launcher reference for
project documentation; omit it from the store gallery because it shows less of
the app itself.

Upload these raw PNGs to the store's Pebble Time 2 screenshot field. Optional
600×800 documentation frames are generated under `build/framed-screenshots/`;
they are not stored in the repository. Updating these local files does not
publish them or change RePebble's OG-card template.

## Reproduce

From the repository root, with the matching build installed locally:

```sh
python3 scripts/capture_store.py
python3 scripts/frame_screenshots.py
```

The capture script installs the current PBW in Emery and changes only emulator
preferences. It uses Large text, Ocean titles, Amber navigation, and a temporary
Violet preview. Pillow is required for framing. Run framing before `pebble clean`,
which removes the capture directory.
