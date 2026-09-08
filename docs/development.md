# Developing Orationes

The watch remains a native Emery app. Prayer content is bundled and offline;
PebbleKit JS provides phone settings through Clay.

## Checks

```sh
npm ci --ignore-scripts
python3 scripts/test.py
python3 scripts/check_release.py
```

Install the locked npm dependencies first; the Clay integration test uses the
actual installed parser. The host tests need Python 3, Node.js, and a C compiler
supporting AddressSanitizer and UndefinedBehaviorSanitizer. They run on GitHub
Actions without the Pebble SDK.
It checks settings migration, interrupted writes, recovery from corruption, phone
message validation and acknowledgments, reading-position persistence, Gregorian
Easter boundaries, weekdays, and noon scheduling across daylight-saving changes.

The release check also requires the locally installed Pebble toolchain. It performs
a clean build and checks Emery-only metadata, bundle size, resources, and static
RAM/heap budgets from `tests/build-budgets.json`. The existing companion is expected
in the bundle. The SDK's established RWX linker warning is non-fatal.

For screenshot regression, install Pillow and run:

```sh
python3 scripts/check_release.py --screenshots
python3 scripts/qa_reading.py --full
python3 scripts/qa_appearance.py
```

This installs into the local Emery emulator and changes its shortcuts, theme, font
size, and reminder preferences. It compares prayer/title pixels in five screens.
Preces and Angelus use reviewed response-layout references in
`tests/screenshots/reader-structure/`; the unchanged styled prayers use the original
`tests/screenshots/` baselines. The rightmost four pixels and bottom 28 pixels are
excluded because the edge progress indicator intentionally replaces the dotted
bottom shadow. All other pixels must match exactly.
It does not install on a physical watch. Re-capture baselines only after reviewing
an intentional visual change; never update them simply to make a failing test pass.
Start without firmware alerts covering the app; restart the emulator if necessary.

The reader script checks section jumps, scrolling, both clamps, double-Select exit,
font-size round trips, Start again, and reopening for six representative prayers in
both sizes and appearances. It also exercises direct library opening, held options,
pinning, independent recent places, Continue, Continue First, and persistence across
relaunch, leaving screenshots under `build/qa-reading/`. Use `--entries`, `--case`,
`--matrix-only`, or `--flows-only` for focused checks. The earlier `qa_flows.py`
script describes the pre-v0.10 navigation and is not the current release gate.

For v0.11.0 automatic-resume behavior, run
`python3 scripts/qa_reading.py --resume --full --entries 1 6 25 40`.
This verifies shortcut resume across relaunch, explicit Start again, Remember
Place Off, and cleared history after re-enabling in both sizes and appearances.
It also checks shared bookmarks through All Prayers (direct and held Open),
More Prayers, Prayer Cards, and Holy Rosary → Litany of Loreto. Host tests exercise
every catalog destination with Remember Place On/Off, including failed preference
saves. No prayer wording or persistent schema is changed.

These checks do not publish, tag, or bump a version. Physical touch behavior and
phone configuration in the actual Pebble app require separate device QA;
[release verification](verification.md) records what was completed.

Pebble Tool 5.0.40 resets the watch clock in its emulator `post_connect` hook on
every CLI command. A sequence of `emu-set-time`, settings, and screenshot commands
therefore does not preserve a simulated noon. For live reminder tests, use one
connection or suppress that hook in an isolated test process. Advancing the clock
can create a firmware missed-wakeup alert that must be dismissed or cleared by an
emulator restart before visual tests. The host scheduling tests avoid that issue.

## Content and catalog

`data/catalog.json` owns shortcut IDs, watch and phone labels, categories, and the
seven defaults. Run `python3 scripts/generate_catalog.py` after changing it.
Generated C and JS files are checked in so normal Pebble builds need no generator.
CI checks for stale generated output and protects existing IDs using
`tests/catalog-ids.json`. Append IDs; never renumber or reuse one.

Prayer wording remains in the original C content modules. `tests/content-sha256.json`
guards those files byte for byte. For an authorized text edit, compare it with the
approved source and explicitly review the baseline update. Canonical local files in
`content/` remain ignored. The catalog generator never rewrites prayer wording.

Preces is packaged verbatim in `resources/data/preces.bin`, including its NUL
terminator. Run `python3 scripts/generate_text_resource.py` after an authorized
resource-backed prayer edit; both build and host checks reject stale resources. A host test compares
the resource against the C compiler's original string, not merely a second parser.
The watch loads and caches those 3,712 bytes on first use. Come, Holy Spirit and
Litany of Humility use the same loading mechanism, generated from their literals in
`prayers.c` by the same script and checked against the compiled strings in host
tests. Their resources add 1,959 bytes including NUL terminators, so the
resource budget was 32,000 bytes for v0.9.0. In v0.10.0, Aspirations also loads from
a generated raw resource, with paragraph text, style, and spacing checked against
the compiled C data. The resource budget is now 43,000 bytes; the executable and
heap limits are unchanged.
This keeps the text outside Pebble's 16-bit loaded/virtual image limit while
preserving offline use.
Link-time optimization retains `__pbl_app_info` explicitly; the bundle gate checks
its signature and image-size fields before any install.

## Settings and storage

`AppSettings` is a compact byte-based record. All setters validate a candidate and
call `app_settings_apply()`. One change callback refreshes open UI for both watch
and phone edits. Reminder edits go through `noon_reminder_apply_settings()` so a
schedule must succeed before enabling is saved.

`durable_store.c` alternates checksummed, versioned records between two keys. It
keeps the prior valid record during a failed or partial write. Settings now use keys
50/51 (schema 3), including Continue First in the same atomic save as phone edits.
On upgrade, schema 2 at 44/45 (or schema 1 at 40/41) and the v0.10.0 Continue First
preference at 46/47 are read and retained untouched. Continue First defaults to Off
if absent or invalid; a saved schema 3 record takes precedence over the old banks.
The eight-prayer reading history uses 48/49; the older single position at 42/43 is
an upgrade fallback. Fresh development installations now use Dark/Ocean/Lime.
Valid saved settings retain their colors, including Classic for migrated schema-1
records that predate Navigation Highlight. Old settings keys 1–7, 20–24, and 30–36
are read for migration and are never repurposed. Wakeups retain keys 10/11.
Changing a record layout requires a schema migration.

The phone keeps a numbered transaction in localStorage until it receives a matching
successful watch acknowledgment. Retries are bounded per JS launch. A rejected save
keeps the draft pending but stops retries until the user saves again. The next phone
configuration page reports pending or failed saves. New submissions replace older
pending drafts, and stale acknowledgments cannot clear them. Both watch and phone
swap an already assigned shortcut with the edited slot's former entry.

The v0.10.1 Clay update exposes Continue First in Reading settings and sends
it in both directions using the appended `ContinueFirst` message key. Older phone
payloads that omit this key preserve the watch value. Watch-side toggles send an
updated snapshot to Clay. The Clay heading reads the package version directly,
so no separate version label needs updating at release time. This change needs a
newly installed PBW; it does not alter the already published v0.10.0 companion.

Run `python3 scripts/qa_phone.py` with the Pebble Tool Python environment after
building to check durable phone acknowledgments, native toggles, open Settings
refresh, invalid batches, Back, and relaunch in both themes and text sizes. It
installs only into the Emery emulator and writes captures to `build/qa-phone/`.

## Navigation and reading

`prayer_navigation_open(id, resume)` resolves catalog destinations for the main
menu, collections, library, and reminder. `navigation_menu.c` supplies library,
Recent Prayers, and reading-options menus with wrapping buttons, measured rows,
and shared colors.
Existing menus can migrate to it as future changes need them.

Up to seven shortcuts are followed by Continue and Recent Prayers when a saved
position exists, then All Prayers and Settings. Continue First optionally moves
Continue above the shortcuts. All Prayers groups every catalog entry into a
category; Select opens directly, while held Select opens Open/Pin options on
release. Pinning lets the user choose a slot and returns to the main menu with
that shortcut selected.

In v0.11.0, every catalog prayer opening follows
Remember Place: shortcuts, library Open/Pin routes, collections, Loreto in the
Rosary menu, Continue, Recent Prayers, and opening the noon-reminder prayer.
When On, an existing bookmark is restored; when Off or no bookmark exists, the
prayer starts at the top. Continue remains a shortcut to the last saved prayer.
The shared `prayer_navigation_open` API no longer accepts a caller-specific
resume flag. Select while reading offers Start again and Jump to section when available.
Remember Place is on by default and can be turned off on the watch or phone; turning
it off clears both old and new saved positions. Up to eight distinct supported
prayers are remembered; a ninth evicts the oldest. Rosary mystery lists are not
bookmarked.

The reader stores a source-byte boundary or styled paragraph index plus a
fraction within that paragraph. It reconstructs the offset after a font-size change.
`prayer_document.c` segments multiline text into display paragraphs while preserving
source-byte order, adding response insets and stanza spacing. It uses the shared
styled viewport renderer; single-block plain text retains TextLayer rendering.
Bookmarks are written on leaving the reader, rather than on every
scroll event. Abrupt loss of power before leaving can therefore retain the previous
checkpoint. The double-Select exit still saves through window cleanup.

### Automatic-resume development verification — 8 September 2026

This records the automatic-resume build before the color-default changes below.

The host regression suite passes, including every catalog destination with both
Remember Place values, failed preference writes, storage recovery, Clay integration,
and unchanged prayer-content hashes. A clean Emery build using Pebble Tool 5.0.40 /
SDK 4.33.1 passes with 40,318 bytes of resources, 59,519 bytes of RAM, and 71,553
bytes of free heap, unchanged from v0.10.1. The development PBW is 787,870 bytes;
the established SDK RWX linker warning remains non-fatal.

Emery checks passed for Preces in all four size/appearance combinations, Loreto
in Large/Light, and St. Josemaría's card and Humility in Extra Large/Dark. Each
checks direct shortcut resume, app relaunch, Start again, Off openings, and cleared
history after re-enabling. Shared bookmarks also passed through All Prayers
(direct and held Open), More Prayers, Prayer Cards, and Rosary → Loreto; submenu
openings with Remember Place Off returned to the top.
The existing `--flows-only` regression also passed: library Open/Pin, independent
saved positions, Continue, Recent Prayers, Continue First, and persistence across
relaunch remain working.

The initial broad run stopped when its Loreto re-enable capture showed the main
menu rather than the reader. The focused Loreto rerun passed. The harness now
checks app run-state responses around stop/start instead of relying solely on
fixed delays; subsequent card, Humility, and menu-route checks passed with it.
The full 16-case cross-product was not completed. Physical touch, noon-alert
activation, and the actual phone Settings page were not exercised in this pass.
No physical installation, version bump, commit, or publication was performed.

Development PBW SHA-256:
`dd808cae234793346a4a750227327dfab9018870fe8d6da2fc87c14d957ee898`.

## UI appearance

Version 0.11.0 defaults are Dark appearance, Ocean title accent and Lime navigation
highlight. C initialization and reset share `app_settings_get_defaults()`;
`src/pkjs/settings-defaults.js` supplies Clay defaults and its reset button.
Native Settings has one Restore Defaults action; the shortcuts submenu has none.
The reset restores every preference: original shortcuts, Dark/Ocean/Lime,
Large text, Remember Place On, Continue First Off, noon reminder Off and duration
10 seconds. It preserves saved prayer positions. The native action commits through
`noon_reminder_apply_settings()` so a successful reset also cancels a scheduled
reminder, sends a phone snapshot, and returns to the preceding screen. Failed
persistence leaves settings and reminder scheduling unchanged. Clay resets all
draft fields and the preview; Save Settings applies them through the same atomic
phone protocol. Closing Clay without saving does not apply the reset.
Empty space below a short main-menu list now follows the selected appearance too.
Both Settings pages end with the bundled version. The native read-only row uses
`ORATIONES_VERSION`, supplied by `wscript` from `package.json`; Clay's final text
item reads the same package version. No separately maintained version string or
persisted version setting is needed.

Run `python3 scripts/qa_defaults.py` in the Pebble tool Python environment
for native reset, phone synchronization, color-pixel and relaunch checks in both
text sizes. Fresh-storage defaults, existing-record preservation, interrupted
reset writes and bookmark preservation are covered by `python3 scripts/test.py`.

Single-reset verification on 8 September 2026: host tests and the clean Emery build
pass. `qa_defaults.py` passes native reset and full phone reset from both text sizes,
checking every setting in watch snapshots, cyan/lime/black/white pixels, and
persistence after relaunch. Native reset was exercised with the noon reminder
enabled; it becomes Off. Cancellation of the firmware wakeup is confirmed by the
reset's `noon_reminder_apply_settings()` code path, not a timed noon test.
The native reset row was visually inspected. Resources: 40,318 bytes; RAM: 59,577
bytes; free heap: 71,495 bytes; PBW: 790,255 bytes. The known SDK RWX warning is
non-fatal. No physical installation or publication was performed in this pass.
Development PBW SHA-256:
`f8e45d43e0cdbc2e4f90dc4b9bdc789c5e1032d47143479dafa56cb50f90efc3`.

Historical color-only verification on 8 September 2026 (before consolidation): the host suite and Emery reset
checks pass in Large and Extra Large, including watch-to-phone snapshots, exact
cyan/lime/black/white pixels, the empty menu background, unchanged unrelated
preferences, and persistence across relaunch. Reset labels and the resulting
menu were visually inspected. The build reports 40,318 resource bytes, 59,725
RAM bytes, 71,347 free heap bytes, and a 790,430-byte PBW (the SDK's known RWX
warning remains non-fatal). This is local development verification, not a
physical-phone test or publication. The frozen v0.10.1 release is unchanged.
Historical development PBW SHA-256:
`af156db3980620aa981ee82a65d2ae6ff9071525ce4832db5cceb8712d69c5d4`.

Title Accent controls title bands; Navigation Highlight controls selected menu rows.
The latter has Classic, Amber, Tangerine, Violet, Magenta, and Lime choices, with
fixed contrasting text colors. Both settings have independent native and phone
previews. Native Up/Down previews without saving; Select commits and Back cancels.
`app_fonts.c` shares stable custom-font handles until app exit so preview and reader
rebuilds cannot confuse cached metrics by reusing an unloaded font address.
`qa_appearance.py` checks raw palette values, readable text, cancel/save, relaunch,
and independence from every existing Title Accent in both appearances.

See [the UI refresh record](ui-refresh.md) for the complete change and verification.

## Release workflow

Use [releasing.md](releasing.md) and `scripts/release.py` for approved releases.
Prepare from a clean, versioned commit; freeze the tested PBW and evidence; obtain
physical approval; publish only the frozen artifact to approved destinations;
verify public versions and hashes before synchronizing availability documentation.
The `.release/` directory is local evidence and must remain ignored.
