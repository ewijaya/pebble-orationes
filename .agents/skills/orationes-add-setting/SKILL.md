---
name: orationes-add-setting
description: Add or change a persisted Orationes Settings option using the existing native menu and app settings architecture. Use for user-selectable behavior, not one-off hard-coded changes.
---

# Add an Orationes setting

Keep settings few, understandable, persistent, and immediately useful.

1. Inspect `app_settings.*`, `settings_menu.*`, the consuming component, and existing persistent keys. Never reuse a key with different semantics.
2. Define a compact enum or boolean, a validated default, getter/setter, and user-facing label. Invalid stored values must fall back safely for upgrades.
3. Persist only after validating the requested value. Handle persistence failure without leaving memory and storage inconsistent.
4. Reuse the accessible menu renderer and current theme. Show the current value clearly and initialize selection to it.
5. After selection, return to the state that preceded Settings when that is the established flow; do not strand the user in an unnecessary submenu.
6. Apply changes immediately where practical and ensure active windows redraw or rebuild safely. Keep new behavior opt-in when it can interrupt the user, such as reminders.
7. Test default state, every choice, persistence across relaunch, invalid-value fallback, Light/Dark, Large/Extra Large, Back, and unrelated existing settings.

Avoid introducing a phone configuration page, PebbleKit JS, or network dependency for watch-local options.

