# Changelog

## v0.12.0 — 2026-09-09

- Continue First is now the first watch Settings row, with its placement explained.
  Continue emphasizes the full saved prayer name above the supporting label.
- A thin divider separates home shortcuts from navigation. Chevrons identify
  submenu destinations, and a narrow position indicator tracks long lists,
  including wraparound navigation. Wrapped labels reserve space for both.
- Short menu headers share a compact 32-pixel minimum; longer titles expand.
  Header text is positioned to preserve descenders such as g, y, and p.
- Today's Mysteries previews the Rosary set and local weekday. All Mysteries
  separates bold names from smaller weekday details.
- A readable first-use notice explains “Hold Select for options”. Settings →
  Menu Help keeps the instruction available after dismissal.
- Optional Compact Menus uses 48-pixel minimum rows with full-size text and
  measured expansion. The spacious 54-pixel layout remains the default.
  This preference is watch-local, survives upgrades and relaunch, and resets
  through watch Restore Defaults. Phone settings preserve it.
- Fix the missing Preces blessing cross (✠) in Large text using a small bundled
  font for the blessing paragraph. Extra Large retains its existing font.
- Expand regression checks for menu layout, header clearance, settings migration,
  interrupted writes, compact persistence, and blessing-font coverage.

Prayer wording, Rosary weekday mappings, default shortcuts, and store artwork
are unchanged. Existing Continue First and appearance preferences are preserved.

## v0.9.0 — 2026-09-07

- Add Come, Holy Spirit and Litany of Humility in All Prayers → Daily Prayer, with individual watch and phone shortcuts.
- Show the Litany's concise attribution, “Cardinal Merry del Val · after Mass”, in a small 14 px note above the prayer.
- Package the new texts as offline resources loaded on first use, keeping the executable within Pebble's image limit.
- Extend content/resource and phone shortcut checks, and retain screenshots of both new prayers.

Existing prayer wording, Rosary data, default shortcuts, and saved settings are preserved. See [release verification](docs/verification.md) for emulator checks and physical installation results.

## v0.8.1 — 2026-09-05

- Fix Save Settings in the mobile Clay configuration page so changes reach the watch.
- Recover pending settings after restarting Orationes, and test the actual Clay parser with mobile timer behavior and watch acknowledgments.

## v0.8.0 — 2026-09-05

- Choose Classic, Amber, Tangerine, Violet, Magenta, or Lime navigation highlights independently of the existing Title Accent palettes.
- Preview text size, appearance, title accent, and navigation colors before saving on the watch; see the combined appearance in phone Settings.
- Show the saved prayer name in Continue, category icons in All Prayers, and a brief confirmation when pinning or clearing a shortcut.
- Improve Settings spacing and wrap longer menu labels to measured row heights.
- Replace the dotted reading shadow with a four-pixel progress indicator.
- Adopt the approved connected-cross launcher icon and matching Orationes header.
- Preserve existing preferences during settings migration and stabilize italic layout after font-size changes.
- Refresh native screenshots and the realistic promotional banner; archive obsolete artwork and generate optional frames outside tracked assets.

Prayer wording, Rosary mysteries, and weekday selection are unchanged. No guided Rosary was added.

See [release verification](docs/verification.md) for candidate details, checks, and remaining hands-on QA.

## v0.7.0 — 2026-09-05

- Continue your last saved prayer with Resume or Start again. Remember Place can be disabled in Settings.
- Browse the complete library through All Prayers, open an entry, or pin it to one of seven main-menu slots.
- Recover settings from interrupted writes and keep phone changes pending until the watch confirms a successful save.
- Generate watch and phone catalogs from shared stable IDs; add host regression tests, content integrity checks, screenshot comparisons, and build budgets.

Prayer wording, Rosary mysteries, and weekday selection are unchanged. No guided Rosary was added.

See [release verification](docs/verification.md) for test results and remaining device QA.
