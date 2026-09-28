---
name: orationes-add-prayer
description: Adds or revises an Orationes prayer from a user-named canonical source, preserving exact wording and reusing the shared reader and catalog. Not for Rosary data or sourcing text the user did not supply.
---

# Add an Orationes prayer

Prayer text is liturgical content, not UI copy. A silent "fix" to spelling or
punctuation changes a prayer people recite, so treat it as data under stricter
review than code.

## Source rules

- Use the canonical source the user names. If it is a local file (for example
  under `content/`), do not replace its wording with a web version.
- Use secondary sources only for the comparison or structure the user
  authorizes. Report discrepancies and unclear characters instead of guessing.
- Preserve spelling, punctuation, accents, ligatures (æ, œ, ǽ), ✠ crosses,
  V./R. markers, rubrics, ordering, and meaningful paragraph breaks. Do not
  normalize or modernize.
- Files in `content/` are deliberately ignored and untracked. Do not edit,
  stage, or un-ignore them unless the user explicitly asks.

## Integration

Read `docs/development.md` → "Content and catalog" before editing. Then:

1. Put the wording in the existing content modules (`src/c/prayers.c` and
   siblings), reusing `Prayer`, `PrayerTranslation`, and the shared reader in
   `prayer_screen.*` / `prayer_document.*`. Keep text out of navigation code and
   keep the translation shape able to accept other languages later.
2. Register it in `data/catalog.json`, then run
   `python3 scripts/generate_catalog.py`. Append a new ID; never renumber or
   reuse one, because saved shortcuts and bookmarks on users' watches refer to
   IDs (`tests/catalog-ids.json` guards this).
3. Long prayers may be resource-backed (`resources/data/*.bin`). After an
   authorized edit to one, run `python3 scripts/generate_text_resource.py`.
4. Check each new non-ASCII character against the relevant font's
   `characterRegex` in `package.json`. Report unsupported glyphs before
   substituting anything.
5. `tests/content-sha256.json` hashes the content modules byte for byte. Update
   it only for this authorized change, and tell the user which hashes changed.
6. Update `docs/prayer-list.md` when the prayer directory changes.

## Validate

- Diff the implemented string against the canonical source; list any
  display-only formatting differences.
- Run `python3 scripts/test.py` and `python3 scripts/check_release.py`.
- In Emery, run `scripts/qa_reading.py --entries <new id>` with the Pebble Tool
  Python, then check first/last text, clamps, touch and buttons, fast hold,
  Back, double-Select exit, both themes, and both sizes.
- Confirm existing prayers, Rosary data, settings, and reminders are unchanged
  unless the request included them. The `orationes-content-guard` skill covers
  this audit.
