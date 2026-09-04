---
name: orationes-content-guard
description: Audit an Orationes change for accidental prayer, Litany, Rosary, weekday, or canonical-source modifications. Use before content-sensitive commits and releases or when the user requires no wording changes.
---

# Orationes content guard

Prove that content changes match the user's authorized scope.

- Establish the comparison point: working tree versus `HEAD`, staged commit, tag, or explicit source file.
- Inspect textual diffs in `src/c/prayers.c`, `src/c/litany.c`, `src/c/rosary_data.c`, and liturgical calendar data. Do not rely only on `--stat`.
- Separate wording changes from C escaping, line wrapping, and display-only paragraph formatting. Report each display-only change that alters rendered spacing.
- For an authorized prayer edit, compare the complete implemented text against its canonical local source and enumerate discrepancies, uncertain characters, or restored structural markers.
- Check accents, ligatures, crosses, ellipses, apostrophes, V./R. markers, rubrics, final lines, mystery labels, and weekday mapping.
- Run `git ls-files content` and `git check-ignore -v content/*`; canonical files must remain ignored, untracked, and unchanged unless explicitly authorized.
- Confirm the font resource's `characterRegex` still covers every introduced character, then validate glyphs in Emery.

End with a concise statement of authorized content changes, unexpected content changes, and files proven unchanged. Never silently repair wording during an audit.

