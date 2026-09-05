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
python3 scripts/qa_flows.py
python3 scripts/qa_appearance.py
```

This installs into the local Emery emulator and changes its shortcuts, theme, font
size, and reminder preferences. It compares prayer/title pixels in five screens against the original
`tests/screenshots/` baselines. The rightmost four pixels and bottom 28 pixels are
excluded because the edge progress indicator intentionally replaces the dotted
bottom shadow. All other pixels must match exactly.
It does not install on a physical watch. Re-capture baselines only after reviewing
an intentional visual change; never update them simply to make a failing test pass.
Start without firmware alerts covering the app; restart the emulator if necessary.

The flow script checks scrolling, double-Select exit, relaunch, Continue, font-size
round trips, and Start again for both Preces and Aspirations. It also exercises
category wrapping and pinning a prayer card into an empty slot, leaving screenshots
for visual review under `build/qa-flows/`.

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
Preces edit; both build and host checks reject a stale resource. A host test compares
the resource against the C compiler's original string, not merely a second parser.
The watch loads and caches those 3,712 bytes on first use. This keeps the text
outside Pebble's 16-bit loaded/virtual image limit while preserving offline use.
Link-time optimization retains `__pbl_app_info` explicitly; the bundle gate checks
its signature and image-size fields before any install.

## Settings and storage

`AppSettings` is a compact byte-based record. All setters validate a candidate and
call `app_settings_apply()`. One change callback refreshes open UI for both watch
and phone edits. Reminder edits go through `noon_reminder_apply_settings()` so a
schedule must succeed before enabling is saved.

`durable_store.c` alternates checksummed, versioned records between two keys. It
keeps the prior valid record during a failed or partial write. Settings use keys
44/45 (schema 2); the reading position uses 42/43. Schema 1 settings at
40/41 are read on upgrade and retained untouched. Navigation Highlight is appended
to the record and defaults to Classic, preserving the previous selection colors. Old settings keys 1–7, 20–24, and 30–36
are read for migration and are never repurposed. Wakeups retain keys 10/11.
Changing a record layout requires a schema migration.

The phone keeps a numbered transaction in localStorage until it receives a matching
successful watch acknowledgment. Retries are bounded per JS launch. A rejected save
keeps the draft pending but stops retries until the user saves again. The next phone
configuration page reports pending or failed saves. New submissions replace older
pending drafts, and stale acknowledgments cannot clear them. Both watch and phone
swap an already assigned shortcut with the edited slot's former entry.

## Navigation and reading

`prayer_navigation_open(id, resume)` resolves catalog destinations for the main
menu, collections, library, and reminder. `navigation_menu.c` supplies the new
library and Continue menus with wrapping buttons, measured rows, and shared colors.
Existing menus can migrate to it as future changes need them.

The seven favorites are followed by Continue when a saved position exists, All
Prayers, and Settings. All Prayers groups every catalog entry into a category;
an entry offers Open and Pin to main menu. Pinning lets the user choose a slot and
returns to the main menu with that shortcut selected.

Normal prayer openings start at the top. Continue offers Resume and Start again.
Remember Place is on by default and can be turned off on the watch or phone; turning
it off clears the saved position. Only the most recently closed supported prayer is
remembered. Rosary mystery lists are not bookmarked.

The reader stores a plain-text line boundary or styled paragraph index plus a
fraction within that paragraph. It reconstructs the offset after a font-size change.
Plain text retains its original TextLayer rendering; styled text retains its existing
viewport renderer. Bookmarks are written on leaving the reader, rather than on every
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
