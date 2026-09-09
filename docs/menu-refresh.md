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


## Stronger Compact Menus — v0.12.1

The v0.12.1 layout lowers compact short rows from 48 to 36 pixels, versus
54 with Compact Menus Off. The default home shortcuts fit five complete rows
below the 32-pixel header instead of three. Gothic 28 Bold labels and Gothic 24
supporting text retain their existing sizes.

In compact Settings, a label and value share one line only when their measured
widths fit with a six-pixel gap. Longer pairs, including Continue First and its
placement explanation, retain separate lines. Wrapped label rows use four pixels
of padding instead of twelve; title/detail rows use six instead of twelve.
Continue prayer names and Rosary weekday details remain fully visible. Text
placement accounts for Gothic descenders. Headers and prayer reading layout are
unchanged, and Compact Menus Off retains the spacious rendering.

This reuses the existing preference and storage format. The follow-up below
adds Clay synchronization. The v0.12.1 candidate is separate from
the frozen v0.12.0 artifact. See README Install for verified public availability.

Validation on 2026-09-09:

- Clean Emery build and host regression suite passed; prayer content hashes are unchanged.
- Resources: 42,485 bytes; RAM: 62,399 bytes; heap: 68,673 bytes; PBW: 795,751 bytes. All repository budgets pass. The existing SDK RWX linker warning remains.
- Emery menu QA passed: 54/36-pixel geometry, identical glyph masks for the first three home labels, relaunch persistence, both appearances and text sizes, inline Settings, Rosary details, long names, Continue, long-Select options, Help, and restoration of the spacious viewport. Screenshots are in `build/qa-menus/`.
- The emulator initially rejected binary transfers; installation succeeded with fresh emulator storage. The previous state was preserved separately.
- Physical PT2 touch accuracy and wrist-distance comfort for 36-pixel rows still need testing.


## Shared Compact Menus and general Help — v0.12.1

Compact Menus is now a Clay toggle as well as a watch setting. Its AppMessage
key is appended after ContinueFirst, preserving all installed key IDs. Full watch
snapshots include its value; phone saves validate and persist it with the other
settings before acknowledging success. A native toggle sends the updated
snapshot immediately. Older payloads that omit the field preserve it. Both
watch reset and Clay Restore Defaults + Save Settings now turn it off. Existing
watch choices are retained on upgrade, using the same storage schema.

Opening Clay now requests a fresh watch snapshot and waits up to two seconds
before showing the form. A timeout opens the cached values with an explicit
notice; a late reply updates the cache without reopening the page. Pending phone
edits retain the existing transaction behavior.

To sync, open Orationes on the connected watch. In MyApps, open its Settings,
make changes and tap Save Settings. Close and reopen the phone Settings page to
see subsequent watch changes. The Clay UI is bundled with the PBW; installing
the new build is required to expose the new toggle.

Settings → Help replaces the single Menu Help notice with selectable topics:
Navigation, Reading, Resume Prayers, Pin Shortcuts, Menu Layout, Phone Settings,
Holy Rosary, and Noon Reminder. Up/Down scrolls a topic; Select or Back returns
to the topics. The shared compact header preserves descenders. Body text uses
Gothic 24 Bold in Large and Gothic 28 Bold in Extra Large, with measured content
height and native scrolling. Phone appearance/text-size changes refresh open Help.
Help owns separate windows and never enters prayer history or writes bookmarks.

The first-use hint remains dismissible and now explicitly says “All Prayers
lists” and instructs the user to hold Select, then release. The full Pin Shortcuts
topic explains selecting a category and prayer, opening options, and choosing a
main-menu slot. The gesture remains scoped to the All Prayers entry lists.


Development validation before the v0.12.1 candidate, 2026-09-09:

- Host content-integrity, settings persistence/atomicity, and actual Clay parser tests pass. The Clay tests cover the new toggle, fresh configuration requests, two-second cached fallback, late replies, queued saves, and reset.
- `scripts/qa_help.py` passed all eight topics in Dark/Large and Light/Extra Large, including bottom/top clamps, Select/Back, topic wrap, and live phone theme/text-size refresh. Screenshot comparisons confirm headers stay fixed while scrolling.
- `scripts/qa_phone.py` passed native and phone Compact Menus toggles, snapshots, invalid batches, relaunch and Back in all four appearance/text-size combinations. `scripts/qa_defaults.py` passed both reset paths including Compact Menus Off.
- Emulator screenshots are retained in `build/qa-help/`, `build/qa-phone/`, and `build/qa-defaults/`. These do not establish physical PT2 touch comfort or the appearance of the user's mobile WebView. Browser preview was unavailable in this session.
- Prayer, Rosary, Litany and calendar content are unchanged. Existing message key IDs and settings storage schema are preserved; the new key is append-only. No release or App Store artifact was changed.

- `scripts/qa_menus.py` exercised the menu regression paths through its final reset. That last comparison initially failed after a missed startup button sequence. Capturing each reset step and repeating it restored a viewport identical to the spacious baseline; the test now uses the same longer restart waits as reset QA.
- Final development bundle: 800,464 bytes; resources: 42,485 bytes; RAM: 64,889 bytes; free heap: 66,183 bytes; load/virtual sizes: 63,228/64,892 bytes. All repository budgets pass. Clean C build and final JS packaging succeeded with only the established SDK RWX linker warning.
- The final PBW installed successfully on fresh Emery storage. The revised first-use hint stayed visible until Select, did not repeat after dismissal, and long Select opened the pin options. Compact sync and relaunch passed on that final bundle. Previous emulator storage was preserved before this onboarding check.
