#include "pebble.h"
#include "storage.h"
#include "app_settings.h"
#include "phone_settings.h"
#include <assert.h>
#include <string.h>

static DictionaryIterator s_outbox;
static void (*s_receiver)(DictionaryIterator *, void *);
static bool s_can_schedule = true;
static unsigned s_changes;
Tuple *dict_find(DictionaryIterator *iterator, uint32_t key) {
  for (unsigned i = 0; i < iterator->count; ++i) if (iterator->tuples[i].key == key) return &iterator->tuples[i];
  return NULL;
}
void dict_write_uint32(DictionaryIterator *iterator, uint32_t key, uint32_t value) {
  const unsigned index = iterator->count++;
  assert(index < 32);
  iterator->values[index].uint32 = value;
  iterator->tuples[index] = (Tuple){key, TUPLE_UINT, 4, &iterator->values[index]};
}
void dict_write_uint8(DictionaryIterator *iterator, uint32_t key, uint8_t value) {
  dict_write_uint32(iterator, key, value);
  iterator->tuples[iterator->count - 1].length = 1;
}
AppMessageResult app_message_outbox_begin(DictionaryIterator **iterator) {
  s_outbox.count = 0; *iterator = &s_outbox; return APP_MSG_OK;
}
AppMessageResult app_message_outbox_send(void) { return APP_MSG_OK; }
AppMessageResult app_message_open(uint32_t inbox, uint32_t outbox) { (void)inbox; (void)outbox; return APP_MSG_OK; }
void app_message_register_inbox_received(void (*handler)(DictionaryIterator *, void *)) { s_receiver = handler; }
void app_message_register_inbox_dropped(void (*handler)(AppMessageResult, void *)) { (void)handler; }
void app_message_deregister_callbacks(void) { s_receiver = NULL; }
bool noon_reminder_apply_settings(const AppSettings *settings) {
  return (!settings->noon_reminder_enabled || s_can_schedule) && app_settings_apply(settings);
}
static void changed(void) { ++s_changes; }
static uint32_t reply(uint32_t key) { return dict_find(&s_outbox, key)->value->uint32; }
void run_phone_tests(void) {
  storage_reset();
  app_settings_init();
  phone_settings_init(changed);
  DictionaryIterator incoming = {0};
  dict_write_uint32(&incoming, MESSAGE_KEY_SettingsTransaction, 17);
  dict_write_uint8(&incoming, MESSAGE_KEY_TextSize, 1);
  dict_write_uint32(&incoming, MESSAGE_KEY_MainMenuSlot1, MAIN_MENU_ENTRY_MEMORARE);
  dict_write_uint32(&incoming, MESSAGE_KEY_MainMenuSlot5, MAIN_MENU_ENTRY_PRECES);
  s_receiver(&incoming, NULL);
  assert(reply(MESSAGE_KEY_SettingsAck) == 17 && reply(MESSAGE_KEY_SettingsStatus) == 0);
  assert(s_changes == 1 && app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE);
  app_settings_init();
  assert(app_settings_get_main_menu_slot(0) == MAIN_MENU_ENTRY_MEMORARE);
  incoming.count = 0;
  dict_write_uint32(&incoming, MESSAGE_KEY_SettingsTransaction, 18);
  dict_write_uint32(&incoming, MESSAGE_KEY_Appearance, 1);
  dict_write_uint32(&incoming, MESSAGE_KEY_MainMenuSlot2, MAIN_MENU_ENTRY_MEMORARE);
  s_receiver(&incoming, NULL);
  assert(reply(MESSAGE_KEY_SettingsStatus) == 1);
  assert(app_settings_get_appearance() == APP_APPEARANCE_LIGHT); // Reject whole duplicate batch.
  incoming.count = 0;
  dict_write_uint32(&incoming, MESSAGE_KEY_TextSize, 256);
  s_receiver(&incoming, NULL);
  assert(reply(MESSAGE_KEY_SettingsStatus) == 1); // Never truncate to a valid uint8.
  incoming.count = 0;
  dict_write_uint32(&incoming, MESSAGE_KEY_TextSize, 0);
  storage_fail_next_write(6);
  s_receiver(&incoming, NULL);
  assert(reply(MESSAGE_KEY_SettingsStatus) == 1);
  app_settings_init();
  assert(app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE);
  incoming.count = 0;
  dict_write_uint32(&incoming, MESSAGE_KEY_NoonReminderEnabled, 1);
  dict_write_uint32(&incoming, MESSAGE_KEY_Appearance, 1);
  s_can_schedule = false;
  s_receiver(&incoming, NULL);
  assert(reply(MESSAGE_KEY_SettingsStatus) == 1);
  assert(app_settings_get_appearance() == APP_APPEARANCE_LIGHT);
  incoming.count=0;
  dict_write_uint32(&incoming,MESSAGE_KEY_NavigationHighlight,APP_NAVIGATION_VIOLET);
  s_receiver(&incoming,NULL);
  assert(reply(MESSAGE_KEY_SettingsStatus)==0);
  assert(reply(MESSAGE_KEY_NavigationHighlight)==APP_NAVIGATION_VIOLET);
  app_settings_init();assert(app_settings_get_navigation_highlight()==APP_NAVIGATION_VIOLET);
  incoming.count=0;
  dict_write_uint32(&incoming,MESSAGE_KEY_NavigationHighlight,APP_NAVIGATION_COUNT);
  dict_write_uint32(&incoming,MESSAGE_KEY_AccentColor,APP_ACCENT_COLOR_FOREST);
  s_receiver(&incoming,NULL);
  assert(reply(MESSAGE_KEY_SettingsStatus)==1);
  assert(app_settings_get_accent_color()==APP_ACCENT_COLOR_OCEAN);
  assert(app_settings_get_navigation_highlight()==APP_NAVIGATION_VIOLET);
  phone_settings_deinit();
}
