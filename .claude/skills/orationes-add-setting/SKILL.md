---
name: orationes-add-setting
description: Adds or changes a persisted Orationes setting across the watch Settings menu, durable storage, and the Clay phone configuration page. Use for user-selectable behavior, not one-off hard-coded changes.
---

# Add an Orationes setting

Keep settings few, understandable, persistent, and immediately useful. A
setting lives in three places — watch storage, the watch menu, and the Clay
phone page — and they must stay in sync, or a phone save can silently undo a
watch choice.

Read `docs/development.md` → "Settings and storage" first. It records the
current schema, storage keys, and the phone transaction protocol.

## Watch side

1. Inspect `app_settings.*`, `settings_menu.*`, `durable_store.*`, and the
   consuming component. Old storage keys are kept for migration and are never
   repurposed; reusing one with different meaning corrupts upgrades.
2. Add a compact enum or boolean with a validated default, getter/setter, and
   label. Setters validate a candidate and go through `app_settings_apply()`.
   An invalid stored value must fall back safely.
3. Changing the `AppSettings` record layout requires a schema bump and a
   migration that reads the previous schema without overwriting it.
4. Reuse the shared menu renderer and theme. Show the current value and start
   the selection on it. After a choice, return to the state that preceded
   Settings when that is the established flow.
5. Apply changes immediately where practical so open windows redraw safely.
   Behavior that can interrupt the user, such as reminders, stays opt-in.

## Phone side

6. Append a message key to `package.json` `messageKeys`; do not reorder.
7. Update `src/c/phone_settings.c` and `src/pkjs/` (`config.js`,
   `settings-defaults.js`, `settings-sync.js`). Older phone payloads that omit
   the key must preserve the watch value, and watch-side changes must send an
   updated snapshot back to Clay.

## Test

- Host: `python3 scripts/test.py` (covers migration, interrupted writes,
  phone validation, and Clay integration). Add cases for the new setting.
- Emery, with the Pebble Tool Python: `scripts/qa_phone.py` and the relevant
  `qa_*` script.
- Manually confirm the default, every choice, persistence across relaunch,
  invalid-value fallback, Light/Dark, Large/Extra Large, Back, and that
  unrelated settings are untouched.
- The phone page on a real device needs the user's hands-on check; say so
  rather than claiming it.
