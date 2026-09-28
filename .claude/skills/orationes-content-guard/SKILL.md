---
name: orationes-content-guard
description: Audits an Orationes change for accidental edits to prayer, Litany, Rosary, aspiration, weekday, or canonical-source text. Use before content-sensitive commits and releases, or whenever wording must not change.
---

# Orationes content guard

Prove that content changes match exactly what the user authorized. This is an
audit: report findings, and do not silently repair wording along the way.

## Establish the comparison

Pick the baseline explicitly — working tree vs. `HEAD`, a staged commit, a tag,
or a named source file — and state it in the report.

## Check

- Run `python3 scripts/test.py`. It verifies `tests/content-sha256.json`
  (byte-for-byte hashes of `prayers.c`, `prayer_collections.c`,
  `prayer_cards.c`, `aspirations.c`, `litany.c`, `rosary_data.c`,
  `liturgical_calendar.c`) and that `resources/data/*.bin` and the generated
  catalog are not stale.
- Read the textual diff of those files; `--stat` alone hides what changed. If
  the hash baseline itself was edited, treat that as a content change needing
  justification.
- Separate wording changes from C escaping, line wrapping, and display-only
  paragraph formatting. Report each display-only change that alters rendered
  spacing.
- For an authorized edit, compare the full implemented text with its canonical
  source and list discrepancies, uncertain characters, and restored markers.
- Check accents, ligatures, ✠ crosses, ellipses, curly quotes, V./R. markers,
  rubrics, final lines, mystery labels, and the weekday → mysteries mapping.
- Run `git ls-files content` and `git check-ignore -v content/*`. The canonical
  files must remain ignored, untracked, and unchanged unless authorized.
- Confirm each font's `characterRegex` in `package.json` still covers every
  introduced character, then check the glyphs in Emery.

## Report

End with three short lists: authorized content changes, unexpected content
changes, and files proven unchanged (and how).
