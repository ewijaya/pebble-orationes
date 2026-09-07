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
an upgrade fallback. Navigation Highlight defaults to Classic, preserving the
previous selection colors. Old settings keys 1–7, 20–24, and 30–36
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

Normal prayer openings start at the top. Continue and Recent Prayers resume
directly. Select while reading offers Start again and Jump to section when available.
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

## UI appearance

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
