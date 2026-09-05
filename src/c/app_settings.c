#include "app_settings.h"

#include "durable_store.h"
#include <pebble.h>
#include <stddef.h>
#include <string.h>

enum {
  PERSIST_KEY_TEXT_SIZE = 1,
  PERSIST_KEY_NOON_REMINDER_ENABLED = 2,
  PERSIST_KEY_NOON_REMINDER_DURATION = 3,
  PERSIST_KEY_ACCENT_COLOR = 4,
  PERSIST_KEY_APPEARANCE = 5,
  PERSIST_KEY_DAILY_PRAYERS_ENABLED = 6,
  PERSIST_KEY_CONFESSION_ENABLED = 7,
  PERSIST_KEY_MAIN_PRAYER_VISIBLE_BASE = 20,
  PERSIST_KEY_MAIN_MENU_SLOT_BASE = 30,
  LEGACY_MAIN_PRAYER_COUNT = PRAYER_ID_MEMORARE + 1,
};

enum { SETTINGS_RECORD_KEY = 44, SETTINGS_SCHEMA = 2, LEGACY_RECORD_KEY = 40 };
static AppSettings s_state;
static AppSettingsChangedHandler s_changed_handler;
static bool s_daily_prayers_enabled;
static bool s_confession_enabled;
static bool s_legacy_main_prayer_visible[LEGACY_MAIN_PRAYER_COUNT];

static bool text_size_is_valid(int32_t value) {
  return value >= APP_TEXT_SIZE_LARGE && value < APP_TEXT_SIZE_COUNT;
}

static bool accent_color_is_valid(int32_t value) {
  return value >= APP_ACCENT_COLOR_OCEAN && value < APP_ACCENT_COLOR_COUNT;
}

static bool appearance_is_valid(int32_t value) {
  return value >= APP_APPEARANCE_LIGHT && value < APP_APPEARANCE_COUNT;
}

static bool noon_reminder_duration_is_valid(int32_t value) {
  return value >= APP_NOON_REMINDER_DURATION_5_SECONDS &&
         value < APP_NOON_REMINDER_DURATION_COUNT;
}

static bool main_menu_entry_is_valid(int32_t value) {
  return value >= MAIN_MENU_ENTRY_NONE && value < MAIN_MENU_ENTRY_COUNT;
}

static uint32_t main_prayer_persist_key(PrayerId prayer_id) {
  return PERSIST_KEY_MAIN_PRAYER_VISIBLE_BASE + (uint32_t)prayer_id;
}

static uint32_t main_menu_slot_persist_key(uint8_t slot_index) {
  return PERSIST_KEY_MAIN_MENU_SLOT_BASE + slot_index;
}

static void set_default_main_menu_slots(void) {
  memcpy(s_state.slots, main_menu_default_slots,
         sizeof(s_state.slots));
}

static bool main_menu_slots_are_valid(
    const uint8_t slots[APP_MAIN_MENU_SLOT_COUNT]) {
  bool used[MAIN_MENU_ENTRY_COUNT] = {false};
  for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
    const MainMenuEntryId entry_id = slots[slot];
    if (!main_menu_entry_is_valid(entry_id)) {
      return false;
    }
    if (entry_id != MAIN_MENU_ENTRY_NONE) {
      if (used[entry_id]) {
        return false;
      }
      used[entry_id] = true;
    }
  }
  return true;
}

static void migrate_legacy_main_menu_settings(void) {
  static const MainMenuEntryId core_entries[LEGACY_MAIN_PRAYER_COUNT] = {
      MAIN_MENU_ENTRY_PRECES,
      MAIN_MENU_ENTRY_HOLY_ROSARY,
      MAIN_MENU_ENTRY_REGINA_CAELI,
      MAIN_MENU_ENTRY_ANGELUS,
      MAIN_MENU_ENTRY_MEMORARE,
  };
  uint8_t next_slot = 0;
  for (PrayerId prayer_id = PRAYER_ID_PRECES;
       prayer_id <= PRAYER_ID_MEMORARE &&
       next_slot < APP_MAIN_MENU_SLOT_COUNT;
       ++prayer_id) {
    if (s_legacy_main_prayer_visible[prayer_id]) {
      s_state.slots[next_slot++] = core_entries[prayer_id];
    }
  }
  if (s_daily_prayers_enabled && next_slot < APP_MAIN_MENU_SLOT_COUNT) {
    s_state.slots[next_slot++] = MAIN_MENU_ENTRY_MORE_PRAYERS;
  }
  if (s_confession_enabled && next_slot < APP_MAIN_MENU_SLOT_COUNT) {
    s_state.slots[next_slot++] = MAIN_MENU_ENTRY_CONFESSION;
  }
  while (next_slot < APP_MAIN_MENU_SLOT_COUNT) {
    s_state.slots[next_slot++] = MAIN_MENU_ENTRY_NONE;
  }
}

static void load_main_menu_slots(void) {
  bool has_saved_slots = false;
  uint8_t loaded[APP_MAIN_MENU_SLOT_COUNT];
  set_default_main_menu_slots();
  memcpy(loaded, s_state.slots, sizeof(loaded));

  for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
    const uint32_t persist_key = main_menu_slot_persist_key(slot);
    if (!persist_exists(persist_key)) {
      continue;
    }
    has_saved_slots = true;
    const int32_t value = persist_read_int(persist_key);
    loaded[slot] = main_menu_entry_is_valid(value)
                       ? (MainMenuEntryId)value
                       : MAIN_MENU_ENTRY_NONE;
  }

  if (has_saved_slots && main_menu_slots_are_valid(loaded)) {
    memcpy(s_state.slots, loaded, sizeof(s_state.slots));
    return;
  }

  migrate_legacy_main_menu_settings();
}

void app_settings_init(void) {
  s_state = (AppSettings){0};
  s_state.remember_place = true;
  s_state.text_size = APP_TEXT_SIZE_LARGE;
  s_state.accent_color = APP_ACCENT_COLOR_OCEAN;
  s_state.appearance = APP_APPEARANCE_LIGHT;
  s_state.noon_reminder_enabled = false;
  s_daily_prayers_enabled = false;
  s_confession_enabled = false;
  s_state.noon_reminder_duration = APP_NOON_REMINDER_DURATION_10_SECONDS;
  for (PrayerId prayer_id = PRAYER_ID_PRECES;
       prayer_id <= PRAYER_ID_MEMORARE; ++prayer_id) {
    s_legacy_main_prayer_visible[prayer_id] = true;
  }

  if (persist_exists(PERSIST_KEY_TEXT_SIZE)) {
    const int32_t stored_text_size = persist_read_int(PERSIST_KEY_TEXT_SIZE);
    if (text_size_is_valid(stored_text_size)) {
      s_state.text_size = (AppTextSize)stored_text_size;
    }
  }

  if (persist_exists(PERSIST_KEY_NOON_REMINDER_ENABLED)) {
    s_state.noon_reminder_enabled =
        persist_read_bool(PERSIST_KEY_NOON_REMINDER_ENABLED);
  }

  if (persist_exists(PERSIST_KEY_ACCENT_COLOR)) {
    const int32_t stored_accent_color =
        persist_read_int(PERSIST_KEY_ACCENT_COLOR);
    if (accent_color_is_valid(stored_accent_color)) {
      s_state.accent_color = (AppAccentColor)stored_accent_color;
    }
  }

  if (persist_exists(PERSIST_KEY_APPEARANCE)) {
    const int32_t stored_appearance = persist_read_int(PERSIST_KEY_APPEARANCE);
    if (appearance_is_valid(stored_appearance)) {
      s_state.appearance = (AppAppearance)stored_appearance;
    }
  }

  if (persist_exists(PERSIST_KEY_NOON_REMINDER_DURATION)) {
    const int32_t stored_duration =
        persist_read_int(PERSIST_KEY_NOON_REMINDER_DURATION);
    if (noon_reminder_duration_is_valid(stored_duration)) {
      s_state.noon_reminder_duration =
          (AppNoonReminderDuration)stored_duration;
    }
  }

  if (persist_exists(PERSIST_KEY_DAILY_PRAYERS_ENABLED)) {
    s_daily_prayers_enabled =
        persist_read_bool(PERSIST_KEY_DAILY_PRAYERS_ENABLED);
  }

  if (persist_exists(PERSIST_KEY_CONFESSION_ENABLED)) {
    s_confession_enabled = persist_read_bool(PERSIST_KEY_CONFESSION_ENABLED);
  }

  for (PrayerId prayer_id = PRAYER_ID_PRECES;
       prayer_id <= PRAYER_ID_MEMORARE; ++prayer_id) {
    const uint32_t persist_key = main_prayer_persist_key(prayer_id);
    if (persist_exists(persist_key)) {
      s_legacy_main_prayer_visible[prayer_id] =
          persist_read_bool(persist_key);
    }
  }
  load_main_menu_slots();
  AppSettings saved = {0};
  const bool loaded = durable_store_read(SETTINGS_RECORD_KEY, SETTINGS_SCHEMA,
                                         &saved, sizeof(saved));
  if (saved.navigation_highlight >= APP_NAVIGATION_COUNT)
    saved.navigation_highlight = APP_NAVIGATION_CLASSIC;
  if (loaded && app_settings_validate(&saved)) {
    s_state = saved;
  } else {
    // Schema 1 is the exact 13-byte prefix. Keep its banks for failed
    // migrations and downgrades; schema 2 uses a new pair, never overwriting
    // the source.
    saved = (AppSettings){0};
    if (durable_store_read(LEGACY_RECORD_KEY, 1, &saved,
                           offsetof(AppSettings, navigation_highlight)) &&
        app_settings_validate(&saved))
      s_state = saved;
  }
}

void app_settings_set_changed_handler(AppSettingsChangedHandler handler) {
  s_changed_handler = handler;
}
AppSettings app_settings_get(void) { return s_state; }
bool app_settings_validate(const AppSettings *settings) {
  return settings && text_size_is_valid(settings->text_size) &&
         accent_color_is_valid(settings->accent_color) &&
         appearance_is_valid(settings->appearance) &&
         noon_reminder_duration_is_valid(settings->noon_reminder_duration) &&
         settings->noon_reminder_enabled <= 1 &&
         settings->remember_place <= 1 &&
         main_menu_slots_are_valid(settings->slots) &&
         settings->navigation_highlight < APP_NAVIGATION_COUNT;
}
bool app_settings_apply(const AppSettings *settings) {
  if (!app_settings_validate(settings)) return false;
  if (memcmp(&s_state, settings, sizeof(s_state)) == 0) return true;
  if (!durable_store_write(SETTINGS_RECORD_KEY, SETTINGS_SCHEMA, settings, sizeof(*settings))) return false;
  s_state = *settings;
  if (s_changed_handler) s_changed_handler();
  return true;
}
bool app_settings_get_remember_place(void) { return s_state.remember_place; }
bool app_settings_set_remember_place(bool enabled) {
  AppSettings updated = s_state;
  updated.remember_place = enabled;
  return app_settings_apply(&updated);
}

AppTextSize app_settings_get_text_size(void) {
  return s_state.text_size;
}

bool app_settings_set_text_size(AppTextSize text_size) {
  if (!text_size_is_valid(text_size)) return false;
  AppSettings updated = s_state;
  updated.text_size = text_size;
  return app_settings_apply(&updated);
}

const char *app_settings_text_size_label(AppTextSize text_size) {
  switch (text_size) {
    case APP_TEXT_SIZE_LARGE:
      return "Large";
    case APP_TEXT_SIZE_EXTRA_LARGE:
      return "Extra Large";
    case APP_TEXT_SIZE_COUNT:
      break;
  }

  return "Large";
}

AppAccentColor app_settings_get_accent_color(void) {
  return s_state.accent_color;
}

bool app_settings_set_accent_color(AppAccentColor accent_color) {
  if (!accent_color_is_valid(accent_color)) return false;
  AppSettings updated = s_state;
  updated.accent_color = accent_color;
  return app_settings_apply(&updated);
}

const char *app_settings_accent_color_label(AppAccentColor accent_color) {
  switch (accent_color) {
    case APP_ACCENT_COLOR_OCEAN:
      return "Ocean";
    case APP_ACCENT_COLOR_BURGUNDY:
      return "Burgundy";
    case APP_ACCENT_COLOR_FOREST:
      return "Forest";
    case APP_ACCENT_COLOR_MONOCHROME:
      return "Monochrome";
    case APP_ACCENT_COLOR_COUNT:
      break;
  }

  return "Ocean";
}

AppAppearance app_settings_get_appearance(void) {
  return s_state.appearance;
}

bool app_settings_set_appearance(AppAppearance appearance) {
  if (!appearance_is_valid(appearance)) return false;
  AppSettings updated = s_state;
  updated.appearance = appearance;
  return app_settings_apply(&updated);
}

const char *app_settings_appearance_label(AppAppearance appearance) {
  switch (appearance) {
    case APP_APPEARANCE_LIGHT:
      return "Light";
    case APP_APPEARANCE_DARK:
      return "Dark";
    case APP_APPEARANCE_COUNT:
      break;
  }

  return "Light";
}

bool app_settings_get_noon_reminder_enabled(void) {
  return s_state.noon_reminder_enabled;
}

bool app_settings_set_noon_reminder_enabled(bool enabled) {
  AppSettings updated = s_state;
  updated.noon_reminder_enabled = enabled;
  return app_settings_apply(&updated);
}

MainMenuEntryId app_settings_get_main_menu_slot(uint8_t slot_index) {
  return slot_index < APP_MAIN_MENU_SLOT_COUNT
             ? s_state.slots[slot_index]
             : MAIN_MENU_ENTRY_NONE;
}

bool app_settings_set_main_menu_slots(
    const MainMenuEntryId slots[APP_MAIN_MENU_SLOT_COUNT]) {
  if (!slots) return false;
  AppSettings updated = s_state;
  for (uint8_t i = 0; i < APP_MAIN_MENU_SLOT_COUNT; ++i) {
    if (!main_menu_entry_is_valid(slots[i])) return false;
    updated.slots[i] = slots[i];
  }
  return app_settings_apply(&updated);
}

bool app_settings_set_main_menu_slot(uint8_t slot_index,
                                     MainMenuEntryId entry_id) {
  if (slot_index >= APP_MAIN_MENU_SLOT_COUNT ||
      !main_menu_entry_is_valid(entry_id)) {
    return false;
  }

  MainMenuEntryId updated[APP_MAIN_MENU_SLOT_COUNT];
  for (uint8_t i = 0; i < APP_MAIN_MENU_SLOT_COUNT; ++i) updated[i] = s_state.slots[i];
  const MainMenuEntryId prior_entry = updated[slot_index];

  if (entry_id != MAIN_MENU_ENTRY_NONE) {
    for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
      if (slot != slot_index && updated[slot] == entry_id) {
        updated[slot] = prior_entry;
        break;
      }
    }
  }
  updated[slot_index] = entry_id;
  return app_settings_set_main_menu_slots(updated);
}

bool app_settings_restore_main_menu_defaults(void) {
  AppSettings updated = s_state;
  memcpy(updated.slots, main_menu_default_slots, sizeof(updated.slots));
  return app_settings_apply(&updated);
}

AppNoonReminderDuration app_settings_get_noon_reminder_duration(void) {
  return s_state.noon_reminder_duration;
}

bool app_settings_set_noon_reminder_duration(AppNoonReminderDuration duration) {
  if (!noon_reminder_duration_is_valid(duration)) return false;
  AppSettings updated = s_state;
  updated.noon_reminder_duration = duration;
  return app_settings_apply(&updated);
}

uint32_t app_settings_get_noon_reminder_duration_seconds(void) {
  switch (s_state.noon_reminder_duration) {
    case APP_NOON_REMINDER_DURATION_5_SECONDS:
      return 5;
    case APP_NOON_REMINDER_DURATION_10_SECONDS:
      return 10;
    case APP_NOON_REMINDER_DURATION_30_SECONDS:
      return 30;
    case APP_NOON_REMINDER_DURATION_COUNT:
      break;
  }

  return 10;
}

const char *app_settings_noon_reminder_duration_label(
    AppNoonReminderDuration duration) {
  switch (duration) {
    case APP_NOON_REMINDER_DURATION_5_SECONDS:
      return "5 seconds";
    case APP_NOON_REMINDER_DURATION_10_SECONDS:
      return "10 seconds";
    case APP_NOON_REMINDER_DURATION_30_SECONDS:
      return "30 seconds";
    case APP_NOON_REMINDER_DURATION_COUNT:
      break;
  }

  return "10 seconds";
}

AppNavigationHighlight app_settings_get_navigation_highlight(void) {
  return s_state.navigation_highlight;
}
bool app_settings_set_navigation_highlight(AppNavigationHighlight value) {
  if ((unsigned)value >= APP_NAVIGATION_COUNT)
    return false;
  AppSettings next = s_state;
  next.navigation_highlight = value;
  return app_settings_apply(&next);
}
const char *
app_settings_navigation_highlight_label(AppNavigationHighlight value) {
  static const char *const labels[] = {"Classic", "Amber",   "Tangerine",
                                       "Violet",  "Magenta", "Lime"};
  return (unsigned)value < APP_NAVIGATION_COUNT ? labels[value] : labels[0];
}
