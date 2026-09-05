#include "prayer_library.h"
#include "app_settings.h"
#include "navigation_menu.h"
#include "phone_settings.h"
#include "prayer_navigation.h"
#include "reading_position.h"
#include <stdio.h>

static NavigationMenu s_categories, s_entries, s_actions, s_slots, s_continue;
static uint8_t s_category;
static MainMenuEntryId s_entry;
static MainMenuEntryId s_resume_entry;
static SettingsShortcutSavedHandler s_saved_handler;
static char s_slot_label[128];
static char s_resume_label[128];

static uint16_t category_count(void *context) { return main_menu_catalog_category_count(); }
static const char *category_label(uint16_t row, void *context) { return main_menu_catalog_category_name(row); }
static uint16_t entry_count(void *context) {
  uint16_t count = 0;
  for (uint16_t i = 1; i < main_menu_catalog_count(); ++i) {
    if (main_menu_catalog_get((MainMenuEntryId)i)->category == s_category) ++count;
  }
  return count;
}
static MainMenuEntryId entry_at(uint16_t row) {
  for (uint16_t i = 1; i < main_menu_catalog_count(); ++i) {
    if (main_menu_catalog_get((MainMenuEntryId)i)->category == s_category && row-- == 0) return (MainMenuEntryId)i;
  }
  return MAIN_MENU_ENTRY_NONE;
}
static const char *entry_label(uint16_t row, void *context) {
  return main_menu_catalog_get(entry_at(row))->name;
}
static void select_category(uint16_t row, void *context) {
  s_category = row;
  s_entries.title = category_label(row, NULL);
  navigation_menu_show(&s_entries);
}
static uint16_t two_rows(void *context) { return 2; }
static const char *action_label(uint16_t row, void *context) {
  return row == 0 ? "Open" : "Pin to main menu";
}
static void select_entry(uint16_t row, void *context) {
  s_entry = entry_at(row);
  s_actions.title = "Prayer Options";
  navigation_menu_show(&s_actions);
}
static uint16_t slot_count(void *context) { return APP_MAIN_MENU_SLOT_COUNT; }
static const char *slot_label(uint16_t row, void *context) {
  snprintf(s_slot_label, sizeof(s_slot_label), "%u: %s", (unsigned)(row + 1),
      main_menu_catalog_get(app_settings_get_main_menu_slot(row))->name);
  return s_slot_label;
}
static void select_action(uint16_t row, void *context) {
  if (row == 0) {
    prayer_navigation_open(s_entry, false);
  } else {
    s_slots.title = "Choose Slot";
    navigation_menu_show(&s_slots);
    uint8_t selected = APP_MAIN_MENU_SLOT_COUNT;
    for (uint8_t i = 0; i < APP_MAIN_MENU_SLOT_COUNT; ++i) {
      if (app_settings_get_main_menu_slot(i) == s_entry) { selected = i; break; }
      if (app_settings_get_main_menu_slot(i) == MAIN_MENU_ENTRY_NONE &&
          selected == APP_MAIN_MENU_SLOT_COUNT) selected = i;
    }
    if (selected == APP_MAIN_MENU_SLOT_COUNT) selected = 0;
    menu_layer_set_selected_index(s_slots.layer, MenuIndex(0, selected), MenuRowAlignCenter, false);
  }
}
static void select_slot(uint16_t row, void *context) {
  if (!app_settings_set_main_menu_slot(row, s_entry)) {
    s_slots.title = "Save failed";
    navigation_menu_refresh(&s_slots);
    return;
  }
  phone_settings_send_current();
  navigation_menu_hide(&s_slots);
  navigation_menu_hide(&s_actions);
  navigation_menu_hide(&s_entries);
  navigation_menu_hide(&s_categories);
  if (s_saved_handler) s_saved_handler(row);
}
static const char *continue_label(uint16_t row, void *context) {
  return row == 0 ? s_resume_label : "Start again";
}
static void select_continue(uint16_t row, void *context) {
  prayer_navigation_open(s_resume_entry, row == 0);
}
void prayer_library_init(SettingsShortcutSavedHandler saved_handler) {
  s_saved_handler = saved_handler;
  navigation_menu_init(&s_categories, "All Prayers", category_count, category_label, select_category, NULL);
  navigation_menu_init(&s_entries, "Prayers", entry_count, entry_label, select_entry, NULL);
  navigation_menu_init(&s_actions, "Prayer Options", two_rows, action_label, select_action, NULL);
  navigation_menu_init(&s_slots, "Choose Slot", slot_count, slot_label, select_slot, NULL);
  navigation_menu_init(&s_continue, "Continue", two_rows, continue_label, select_continue, NULL);
}
void prayer_library_show(void) { navigation_menu_show(&s_categories); }
void prayer_library_continue(void) {
  ReadingPosition position;
  if (!reading_position_get(&position)) return;
  s_resume_entry = (MainMenuEntryId)position.entry;
  snprintf(s_resume_label, sizeof(s_resume_label), "Resume: %s", main_menu_catalog_get(s_resume_entry)->name);
  navigation_menu_show(&s_continue);
}
void prayer_library_refresh(void) {
  if (!app_settings_get_remember_place()) navigation_menu_hide(&s_continue);
  NavigationMenu *menus[] = {&s_categories, &s_entries, &s_actions, &s_slots, &s_continue};
  for (unsigned i = 0; i < sizeof(menus) / sizeof(menus[0]); ++i) navigation_menu_refresh(menus[i]);
}
void prayer_library_deinit(void) {
  NavigationMenu *menus[] = {&s_slots, &s_actions, &s_entries, &s_categories, &s_continue};
  for (unsigned i = 0; i < sizeof(menus) / sizeof(menus[0]); ++i) navigation_menu_deinit(menus[i]);
}
