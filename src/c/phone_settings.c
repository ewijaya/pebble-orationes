#include "phone_settings.h"

#include <pebble.h>

#include "app_settings.h"
#include "noon_reminder.h"

enum {
  APP_MESSAGE_INBOX_SIZE = 256,
  APP_MESSAGE_OUTBOX_SIZE = 256,
};

static PhoneSettingsChangedHandler s_changed_handler;
static bool s_app_message_open;

static uint32_t main_menu_slot_message_key(uint8_t slot) {
  switch (slot) {
    case 0:
      return MESSAGE_KEY_MainMenuSlot1;
    case 1:
      return MESSAGE_KEY_MainMenuSlot2;
    case 2:
      return MESSAGE_KEY_MainMenuSlot3;
    case 3:
      return MESSAGE_KEY_MainMenuSlot4;
    case 4:
      return MESSAGE_KEY_MainMenuSlot5;
    case 5:
      return MESSAGE_KEY_MainMenuSlot6;
    case 6:
      return MESSAGE_KEY_MainMenuSlot7;
  }
  return MESSAGE_KEY_MainMenuSlot1;
}

static bool tuple_bool(const Tuple *tuple) {
  return tuple && tuple->value->int32 != 0;
}

static bool apply_text_size(const Tuple *tuple) {
  if (!tuple) {
    return false;
  }
  const AppTextSize value = (AppTextSize)tuple->value->int32;
  return value != app_settings_get_text_size() &&
         app_settings_set_text_size(value);
}

static bool apply_appearance(const Tuple *tuple) {
  if (!tuple) {
    return false;
  }
  const AppAppearance value = (AppAppearance)tuple->value->int32;
  return value != app_settings_get_appearance() &&
         app_settings_set_appearance(value);
}

static bool apply_accent_color(const Tuple *tuple) {
  if (!tuple) {
    return false;
  }
  const AppAccentColor value = (AppAccentColor)tuple->value->int32;
  return value != app_settings_get_accent_color() &&
         app_settings_set_accent_color(value);
}

static bool apply_noon_duration(const Tuple *tuple) {
  if (!tuple) {
    return false;
  }
  const AppNoonReminderDuration value =
      (AppNoonReminderDuration)tuple->value->int32;
  return value != app_settings_get_noon_reminder_duration() &&
         app_settings_set_noon_reminder_duration(value);
}

static bool apply_noon_enabled(const Tuple *tuple) {
  if (!tuple) {
    return false;
  }
  const bool value = tuple_bool(tuple);
  return value != app_settings_get_noon_reminder_enabled() &&
         noon_reminder_set_enabled(value);
}

static bool apply_main_menu_slots(DictionaryIterator *iterator,
                                  bool *received_slot) {
  bool received_any_slot = false;
  bool differs = false;
  MainMenuEntryId slots[APP_MAIN_MENU_SLOT_COUNT];

  for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
    const MainMenuEntryId current = app_settings_get_main_menu_slot(slot);
    const Tuple *tuple =
        dict_find(iterator, main_menu_slot_message_key(slot));
    slots[slot] = tuple ? (MainMenuEntryId)tuple->value->int32 : current;
    received_any_slot |= tuple != NULL;
    differs |= slots[slot] != current;
  }

  if (received_any_slot) {
    *received_slot = true;
  }
  return received_any_slot && differs &&
         app_settings_set_main_menu_slots(slots);
}

void phone_settings_send_current(void) {
  if (!s_app_message_open) {
    return;
  }

  DictionaryIterator *iterator;
  const AppMessageResult begin_result = app_message_outbox_begin(&iterator);
  if (begin_result != APP_MSG_OK || !iterator) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Settings outbox unavailable: %d",
            begin_result);
    return;
  }

  dict_write_uint8(iterator, MESSAGE_KEY_TextSize,
                   app_settings_get_text_size());
  dict_write_uint8(iterator, MESSAGE_KEY_Appearance,
                   app_settings_get_appearance());
  dict_write_uint8(iterator, MESSAGE_KEY_AccentColor,
                   app_settings_get_accent_color());
  dict_write_uint8(iterator, MESSAGE_KEY_NoonReminderEnabled,
                   app_settings_get_noon_reminder_enabled());
  dict_write_uint8(iterator, MESSAGE_KEY_NoonReminderDuration,
                   app_settings_get_noon_reminder_duration());
  for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
    dict_write_uint8(iterator, main_menu_slot_message_key(slot),
                     app_settings_get_main_menu_slot(slot));
  }

  const AppMessageResult send_result = app_message_outbox_send();
  if (send_result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Settings snapshot send failed: %d",
            send_result);
  }
}

static void inbox_received_handler(DictionaryIterator *iterator,
                                   void *context) {
  (void)context;

  const bool is_request =
      dict_find(iterator, MESSAGE_KEY_SettingsRequest) != NULL;
  bool changed = false;
  bool received_main_menu_slot = false;

  changed |= apply_text_size(dict_find(iterator, MESSAGE_KEY_TextSize));
  changed |= apply_appearance(dict_find(iterator, MESSAGE_KEY_Appearance));
  changed |= apply_accent_color(dict_find(iterator, MESSAGE_KEY_AccentColor));
  changed |= apply_noon_duration(
      dict_find(iterator, MESSAGE_KEY_NoonReminderDuration));
  changed |= apply_noon_enabled(
      dict_find(iterator, MESSAGE_KEY_NoonReminderEnabled));
  changed |= apply_main_menu_slots(iterator, &received_main_menu_slot);

  if (changed && s_changed_handler) {
    s_changed_handler();
  }
  if (changed || is_request || received_main_menu_slot) {
    phone_settings_send_current();
  }
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  (void)context;
  APP_LOG(APP_LOG_LEVEL_WARNING, "Settings inbox dropped: %d", reason);
}

void phone_settings_init(PhoneSettingsChangedHandler changed_handler) {
  s_changed_handler = changed_handler;
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);

  const AppMessageResult result =
      app_message_open(APP_MESSAGE_INBOX_SIZE, APP_MESSAGE_OUTBOX_SIZE);
  s_app_message_open = result == APP_MSG_OK;
  if (!s_app_message_open) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Settings AppMessage open failed: %d",
            result);
  }
}

void phone_settings_deinit(void) {
  s_app_message_open = false;
  s_changed_handler = NULL;
  app_message_deregister_callbacks();
}
