#include "phone_settings.h"

#include <pebble.h>

#include "app_settings.h"
#include "noon_reminder.h"

enum {
  APP_MESSAGE_INBOX_SIZE = 256,
  APP_MESSAGE_OUTBOX_SIZE = 256,
};

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

static uint32_t s_ack;
static uint8_t s_status;

static bool read_value(const Tuple *tuple, uint32_t *value) {
  if (!tuple || (tuple->type != TUPLE_UINT && tuple->type != TUPLE_INT)) return false;
  switch (tuple->length) {
    case 1: *value = tuple->value->uint8; return true;
    case 2: *value = tuple->value->uint16; return true;
    case 4: *value = tuple->value->uint32; return true;
    default: return false;
  }
}
static bool read_setting(DictionaryIterator *iterator, uint32_t key, uint8_t *value) {
  const Tuple *tuple = dict_find(iterator, key);
  if (!tuple) return true;
  uint32_t incoming;
  if (!read_value(tuple, &incoming) || incoming > UINT8_MAX) return false;
  *value = incoming;
  return true;
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

  dict_write_uint32(iterator, MESSAGE_KEY_SettingsAck, s_ack);
  dict_write_uint8(iterator, MESSAGE_KEY_SettingsStatus, s_status);
  dict_write_uint8(iterator, MESSAGE_KEY_NavigationHighlight,
                   app_settings_get_navigation_highlight());
  dict_write_uint8(iterator, MESSAGE_KEY_RememberPlace, app_settings_get_remember_place());
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

static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
  (void)context;
  if (dict_find(iterator, MESSAGE_KEY_SettingsRequest)) {
    phone_settings_send_current();
    return;
  }
  AppSettings updated = app_settings_get();
  bool valid = true;
  valid &= read_setting(iterator, MESSAGE_KEY_TextSize, &updated.text_size);
  valid &= read_setting(iterator, MESSAGE_KEY_Appearance, &updated.appearance);
  valid &= read_setting(iterator, MESSAGE_KEY_AccentColor, &updated.accent_color);
  valid &= read_setting(iterator, MESSAGE_KEY_NoonReminderEnabled, &updated.noon_reminder_enabled);
  valid &= read_setting(iterator, MESSAGE_KEY_NoonReminderDuration, &updated.noon_reminder_duration);
  valid &= read_setting(iterator, MESSAGE_KEY_NavigationHighlight,
                        &updated.navigation_highlight);
  valid &= read_setting(iterator, MESSAGE_KEY_RememberPlace, &updated.remember_place);
  for (uint8_t i = 0; i < APP_MAIN_MENU_SLOT_COUNT; ++i) {
    valid &= read_setting(iterator, main_menu_slot_message_key(i), &updated.slots[i]);
  }
  s_ack = 0;
  const Tuple *transaction = dict_find(iterator, MESSAGE_KEY_SettingsTransaction);
  if (transaction) valid &= read_value(transaction, &s_ack);
  s_status = valid && noon_reminder_apply_settings(&updated) ? 0 : 1;
  phone_settings_send_current();
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  (void)context;
  APP_LOG(APP_LOG_LEVEL_WARNING, "Settings inbox dropped: %d", reason);
}

void phone_settings_init(PhoneSettingsChangedHandler changed_handler) {
  app_settings_set_changed_handler(changed_handler);
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
  app_settings_set_changed_handler(NULL);
  app_message_deregister_callbacks();
}
