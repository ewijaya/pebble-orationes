---
name: orationes-qa
description: Runs an Orationes regression pass in the Emery emulator or guides one on a physical Pebble Time 2 — navigation, reading, settings, phone sync, reminders, themes, and lifecycle. Use for behavior checks, not compiler-only checks.
---

# Orationes QA

Test observable behavior, and keep a clear line between what was actually
exercised and what was inferred from reading source.

## Setup

- Read the current menu, catalog (`data/catalog.json`), settings, and reminder
  code before building the test matrix. Past feature lists go stale.
- Read `docs/development.md` for the current QA scripts and known emulator
  caveats.
- Build, then `pebble install --emulator emery`. Check installed CLI help
  before assuming emulator input or screenshot syntax.
- Run the `scripts/qa_*.py` tools with the Pebble Tool Python, which has
  Pillow (`head -1 "$(which pebble)"` shows its path). They change emulator
  settings and write evidence under `build/qa-*/`.
- Start each focused test from a known screen, and dismiss firmware alerts
  first (restart the emulator if needed); they break pixel comparisons.

## Scripted coverage

| Area | Command |
| --- | --- |
| Reading matrix + navigation flows | `qa_reading.py --full` (`--entries`, `--case`, `--matrix-only`, `--flows-only`) |
| Automatic resume | `qa_reading.py --resume --full --entries 1 6 25 40` |
| Appearance | `qa_appearance.py` |
| Menus and headers | `qa_menus.py` |
| Help topics | `qa_help.py` |
| Fresh-install defaults | `qa_defaults.py` |
| Phone ↔ watch settings | `qa_phone.py` |

`qa_flows.py` describes pre-v0.10 navigation; do not treat it as a gate.

## Manual matrix

- Main menu: shortcuts, Continue / Recent Prayers when a place is saved,
  Continue First, All Prayers, Settings; Up/Down, Select, held Select
  (Open/Pin), Back, and app exit.
- Reader: opens at top or resumes per Remember Place; tap, touch, and hold
  scrolling; exact bottom; clamps at both ends; Select → Reading Options
  (Start again first, then Jump to section); quick double-Select exits.
- Holy Rosary: Today's Mysteries matches the local weekday; each group has five
  correct mysteries; Litany of Loreto opens; Back returns level by level.
- Settings: Large/Extra Large, Light/Dark, accents, Navigation Highlight,
  Compact Menus, persistence after relaunch.
- Noon reminder: off and each duration, seasonal prayer choice, Select opens,
  Back dismisses, automatic dismissal, no stale state.
- Lifecycle: leave long text, reopen, exit, relaunch — no crash and no stuck
  button or touch state.

The emulator resets its clock on every Pebble CLI command, so a sequence of
`emu-set-time` then other commands does not hold a simulated noon. Host tests
cover scheduling; for live reminder checks, use a single connection.

## Report

- State emulator date/time when weekday or noon behavior matters.
- List checks that need the user's physical watch or phone; never claim
  physical success from emulator evidence.
- For failures give repro steps, expected vs. actual, and relevant logs or
  screenshots. Do not alter prayer wording to make a UI test pass, and do not
  re-capture baselines in `tests/screenshots/` just to make a test pass.
