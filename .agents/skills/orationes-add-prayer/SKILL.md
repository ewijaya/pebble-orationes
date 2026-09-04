---
name: orationes-add-prayer
description: Add or revise an Orationes prayer from a user-authorized canonical source while preserving exact wording and reusing the shared prayer UI. Do not use for Rosary data or unsolicited text sourcing.
---

# Add an Orationes prayer

Treat prayer content as data with stricter review than ordinary UI copy.

## Source rules

- Identify the canonical source named by the user. If it is a local file, do not replace its wording from the web.
- Use secondary sources only for the comparison or structure the user authorizes. Report discrepancies and unclear characters instead of guessing.
- Preserve spelling, punctuation, accents, ligatures, markers, rubrics, ordering, and useful paragraph breaks. Do not normalize or modernize silently.
- Never modify or track ignored canonical files under `content/` unless explicitly requested.

## Integration

- Inspect `src/c/prayers.h`, `src/c/prayers.c`, `src/c/prayer_screen.*`, and current menu routing.
- Reuse `Prayer`, `PrayerTranslation`, and the common scrollable prayer screen. Keep text out of UI/navigation code and retain a translation shape that can accept other languages later.
- Add the smallest necessary ID, translation table, prayer record, and navigation entry. Do not create a second scroller or unrelated language-selection UI.
- Check every new non-ASCII character against the font resource `characterRegex`, the bundled font, and an Emery rendering. Report unsupported glyphs before substitution.

## Validation

- Compare the implemented string with the canonical source and report display-only formatting changes.
- Run the content guard, `pebble clean`, and `pebble build`.
- In Emery, verify first text, last text, full scrolling, clamps, touch/buttons, fast hold, Back, double-Select exit, themes, and both sizes.
- Confirm existing prayers, Rosary data, settings, and reminders are unchanged unless the request includes them.

