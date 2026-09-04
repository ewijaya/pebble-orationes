---
name: orationes-qa
description: Run an Orationes regression test in the Emery emulator or on a physical Pebble Time 2. Use for navigation, prayer, settings, reminder, theme, and lifecycle verification rather than compiler-only checks.
---

# Orationes QA

Test observable behavior and distinguish results actually exercised from checks inferred from source.

## Setup

- Inspect the current menu, prayer data, settings, and reminder source before forming the matrix; do not assume a past feature list is current.
- Build successfully, install with `pebble install --emulator emery`, and use installed CLI help before assuming emulator input or screenshot commands.
- Start each focused test from a known screen. Capture evidence only when it helps diagnose or document behavior.

## Regression matrix

- Main menu order, row contrast, Settings entry, Up/Down movement, Select, Back, and app exit.
- Angelus, Regina Caeli, Memorare, Preces, and Litany: open at top, tap scroll, touch scroll, hold-to-scroll, Up symmetry, exact bottom reached, clamps at both ends, Back, and reopen.
- Prayer shortcut: double-click Select exits to the watchface without affecting ordinary Select behavior elsewhere.
- Holy Rosary: Today's Mysteries matches local weekday; all four groups have five correct mysteries; Litany opens; Back returns through the intended levels.
- Settings: Large/Extra Large, Light/Dark, every accent, selected state, persistence after relaunch, and return to the state preceding Settings where applicable.
- Noon reminder: off and each duration option, current seasonal prayer choice, Select to open, Back to dismiss, automatic dismissal, and no stale reminder state.
- Lifecycle: navigate into long text, leave, reopen, exit the app, and relaunch without crashes or stuck button/touch state.

## Reporting

- Report emulator date/time when validating weekday or noon behavior.
- Call out tests that require the user's physical watch; never claim physical success from emulator evidence.
- Include failures with reproduction steps, expected/actual behavior, and relevant logs. Do not alter prayer wording to make a UI test pass.

