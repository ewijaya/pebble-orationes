# Menu refresh

Implemented for v0.12.0 without changing prayer content.

| Request | Behavior |
| --- | --- |
| Easier Continue access | Continue First is the first watch Settings row, with an explanation of its current placement. Existing choices and the Off default are preserved. |
| Separate shortcuts/navigation | A one-pixel inset divider precedes Recent Prayers, or All Prayers when there is no history. It does not add a selectable row. |
| Predictable submenu rows | Holy Rosary, collection shortcuts, Recent Prayers, All Prayers, Settings, and All Mysteries have drawn right chevrons. Direct prayer rows remain plain. |
| Long-list position | A three-pixel thumb and one-pixel track occupy a separate six-pixel gutter. The thumb follows the selected row and jumps to the correct end on wrap. Short lists hide it. |
| Compact headers | Short submenu headers have a 32-pixel minimum, matching home. Measured long titles grow. |
| Prominent saved name | Continue uses Gothic 28 Bold for the full saved prayer name and Gothic 24 for the supporting Continue label, with a bookmark icon. |
| Today's Rosary preview | Today's Mysteries shows the existing set name and weekday abbreviation. It refreshes on appearance and at midnight while visible. |
| Mystery hierarchy | All Mysteries uses Gothic 28 Bold names above Gothic 24 weekday details. The original weekday mapping is unchanged. |
| Readable pinning help | First use of a library prayer list opens a Gothic 24 Bold “Hold Select for options” notice. Select or Back dismisses it; Settings → Menu Help can reopen it without a timeout. |
| Optional density | Compact Menus is the second watch Settings row. Off preserves the 54-pixel minimum; On uses 48 pixels. Both measure wrapped text at the actual available width. Saving returns to the preceding screen. |

The gutter is outside the native MenuLayer, and chevrons reserve an additional 14 pixels when measuring and drawing labels. The menu wrapper owns and destroys its indicator alongside each menu. Selection callbacks update indicators for native and custom button navigation. Body and selection colors continue to use the existing theme functions.

Compact Menus is a watch-local preference. Settings schema 4 uses new dual banks at keys 52/53 and migrates schema 3's 15-byte prefix from keys 50/51, preserving earlier migration paths. Invalid compact values fall back to spacious. Failed writes preserve both live and stored settings. Watch Restore Defaults turns compact off; phone updates preserve it. Key 54 records whether the pinning hint has been shown, independently of settings and prayer history.

## Verification — 2026-09-09

- Pebble Tool 5.0.40, SDK 4.33.1, Emery only. Clean build followed by final incremental builds passed. The SDK's existing RWX LOAD-segment linker warning remains.
- Final resources: 42,485 bytes; RAM footprint: 61,839 bytes; free heap: 69,233 bytes; PBW: 794,859 bytes. All repository budgets pass. RAM increased 1,849 bytes against the recorded 59,990-byte baseline.
- `python3 scripts/test.py` passed, including content hashes, catalog routing, calendar mapping, phone synchronization, and settings recovery. Added compact migration, invalid-value, relaunch, reset, and every torn-write-length checks.
- `scripts/qa_menus.py` exercised 54/48-pixel layouts, relaunch persistence, Light/Dark and Large/Extra Large, home wrap endpoints, Continue placement and opening, Rosary hierarchy, long names, long Select options, and Help dismissal/reopening. Screenshots are retained under `build/qa-menus/`.
- `scripts/qa_phone.py` passed phone/watch Continue First changes, invalid transactions, Back, and relaunch in all four appearance/text-size combinations.
- `scripts/qa_defaults.py` passed native and phone reset, the read-only version row, default colors, and relaunch for both text sizes.
- The emulator reported 2026-09-09 01:04:18 UTC during verification; Today's Mysteries visibly showed Glorious · Wed.
- Prayer source hashes, Rosary data, and the liturgical calendar are unchanged. Canonical content and generated build artifacts remain ignored. Existing PebbleKit JS remains in the package; this change adds no phone configuration fields or network dependencies.

Physical Pebble Time 2 validation remains necessary for touch navigation, comfort of 48-pixel rows, wrist-distance readability, and rapid touch/button changes. Emulator button and screenshot checks do not establish those results.

## Font and header corrections

The compact header renderer now draws Gothic 28 Bold at y = -3, accounting for the font's glyph bearings. This leaves a clear scanline above the two-pixel divider without increasing the 32-pixel minimum. Settings, Prayer Cards, and a two-line prayer title were checked in both appearances; the final clear scanlines are y = 29 and y = 57 respectively. The menu QA script now checks that clearance.

Large-mode Preces uses a bundled DejaVu Sans Condensed Bold 28 subset for the blessing paragraph containing U+2720 (✠), which the system Gothic font cannot render. Extra Large already contains that glyph. The fallback covers the entire paragraph and is used for both measurement and drawing; all prayer bytes remain unchanged. The resource subset is checked against the source paragraph by the host suite, and each character was verified against the TTF's character map. Font handles remain cached for the app lifetime to preserve layout-cache behavior.

Focused screenshots are retained in `build/qa-glyph-fixes/`. The blessing cross and final Preces responses were inspected in Large and Extra Large. Resources remain within the 43,000-byte repository budget. These checks use the Emery emulator; the user's physical-watch photo established the original missing-glyph defect.
