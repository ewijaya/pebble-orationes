#include "pebble.h"
#include "storage.h"
#include "app_settings.h"
#include "liturgical_calendar.h"
#include "rosary_data.h"
#include "durable_store.h"
#include "reading_position.h"
#include "reminder_schedule.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_settings(void) {
  storage_reset();
  app_settings_init();
  assert(app_settings_get_text_size() == APP_TEXT_SIZE_LARGE);
  assert(!app_settings_get_noon_reminder_enabled());
  for (int i = 0; i < 5; ++i) assert(app_settings_get_main_menu_slot(i) == i + 1);
  assert(app_settings_get_main_menu_slot(6) == MAIN_MENU_ENTRY_NONE);
  assert(app_settings_set_main_menu_slot(0, MAIN_MENU_ENTRY_MEMORARE));
  assert(app_settings_get_main_menu_slot(4) == MAIN_MENU_ENTRY_PRECES);
  app_settings_init();
  assert(app_settings_get_main_menu_slot(0) == MAIN_MENU_ENTRY_MEMORARE);
  assert(!app_settings_set_text_size(APP_TEXT_SIZE_COUNT));
  storage_fail_next_write(0);
  assert(!app_settings_set_text_size(APP_TEXT_SIZE_EXTRA_LARGE));
  assert(app_settings_get_text_size() == APP_TEXT_SIZE_LARGE);
  assert(app_settings_set_text_size(APP_TEXT_SIZE_EXTRA_LARGE));
  app_settings_init();
  assert(app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE);

  storage_reset();
  persist_write_bool(20, false); // Legacy hidden Preces.
  persist_write_bool(6, true);   // Legacy More Prayers.
  persist_write_int(1, 99);     // Invalid text size.
  app_settings_init();
  assert(app_settings_get_main_menu_slot(0) == MAIN_MENU_ENTRY_HOLY_ROSARY);
  assert(app_settings_get_main_menu_slot(4) == MAIN_MENU_ENTRY_MORE_PRAYERS);
  assert(app_settings_get_text_size() == APP_TEXT_SIZE_LARGE);

  storage_reset();
  for (unsigned i = 0; i < 7; ++i) persist_write_int(30 + i, i == 0 ? MAIN_MENU_ENTRY_CARD_PEDRO : 0);
  app_settings_init();
  assert(app_settings_get_main_menu_slot(0) == MAIN_MENU_ENTRY_CARD_PEDRO);
  assert(app_settings_get_main_menu_slot(4) == MAIN_MENU_ENTRY_NONE);
  assert(app_settings_set_appearance(APP_APPEARANCE_DARK));
  app_settings_init();
  assert(app_settings_get_main_menu_slot(0) == MAIN_MENU_ENTRY_CARD_PEDRO);
  for (int accent = 0; accent < APP_ACCENT_COLOR_COUNT; ++accent) {
    assert(app_settings_set_accent_color((AppAccentColor)accent));
    app_settings_init();
    assert(app_settings_get_accent_color() == accent);
  }
  for (int duration = 0; duration < APP_NOON_REMINDER_DURATION_COUNT; ++duration) {
    assert(app_settings_set_noon_reminder_duration((AppNoonReminderDuration)duration));
    app_settings_init();
    assert(app_settings_get_noon_reminder_duration() == duration);
  }
  assert(app_settings_restore_main_menu_defaults());
  app_settings_init();
  for (int i = 0; i < 5; ++i) assert(app_settings_get_main_menu_slot(i) == i + 1);
}

static void test_navigation_migration(void) {
  // Exact v0.7 schema: size, accent, appearance, reminder, duration, remember, slots.
  const uint8_t old[] = {1,2,1,0,2,1, 5,2,3,4,1,23,24};
  storage_reset();
  assert(durable_store_write(40,1,old,sizeof(old)));
  app_settings_init();
  assert(app_settings_get_text_size()==APP_TEXT_SIZE_EXTRA_LARGE);
  assert(app_settings_get_accent_color()==APP_ACCENT_COLOR_FOREST);
  assert(app_settings_get_appearance()==APP_APPEARANCE_DARK);
  assert(app_settings_get_main_menu_slot(5)==23);
  assert(app_settings_get_navigation_highlight()==APP_NAVIGATION_CLASSIC);
  storage_fail_next_write(10);
  assert(!app_settings_set_navigation_highlight(APP_NAVIGATION_AMBER));
  app_settings_init();
  assert(app_settings_get_main_menu_slot(5)==23);
  assert(app_settings_get_navigation_highlight()==APP_NAVIGATION_CLASSIC);
  for(int i=0;i<APP_NAVIGATION_COUNT;++i) {
    assert(app_settings_set_navigation_highlight(i));app_settings_init();
    assert(app_settings_get_navigation_highlight()==i);
    assert(app_settings_get_accent_color()==APP_ACCENT_COLOR_FOREST);
    assert(app_settings_get_main_menu_slot(5)==23);
  }
  assert(!app_settings_set_navigation_highlight(APP_NAVIGATION_COUNT));
  assert(!app_settings_set_navigation_highlight((AppNavigationHighlight)-1));
  uint8_t restored[sizeof(old)];
  assert(durable_store_read(40,1,restored,sizeof(restored)));
  assert(!memcmp(old,restored,sizeof(old)));
  AppSettings invalid=app_settings_get();invalid.navigation_highlight=255;
  assert(!app_settings_apply(&invalid));
  assert(durable_store_write(44,2,&invalid,sizeof(invalid)));
  app_settings_init();assert(app_settings_get_navigation_highlight()==APP_NAVIGATION_CLASSIC);
  assert(app_settings_get_main_menu_slot(5)==23);
}

static void test_calendar(void) {
  assert(!liturgical_calendar_is_eastertide_date(2026, 4, 4));
  assert(liturgical_calendar_is_eastertide_date(2026, 4, 5));
  assert(liturgical_calendar_is_eastertide_date(2026, 5, 24));
  assert(!liturgical_calendar_is_eastertide_date(2026, 5, 25));
  assert(liturgical_calendar_is_eastertide_date(2024, 3, 31));
  assert(!liturgical_calendar_is_eastertide_date(2024, 3, 30));
  const char *expected[] = {"Glorious", "Joyful", "Sorrowful", "Glorious",
                            "Luminous", "Sorrowful", "Joyful"};
  for (int day = 0; day < 7; ++day) {
    const RosaryMysterySet *set = rosary_mystery_set_for_weekday(day);
    assert(strcmp(set->name, expected[day]) == 0);
    for (int i = 0; i < 5; ++i) assert(set->mysteries[i].today_label[0]);
  }
  assert(!rosary_mystery_set_for_weekday(-1));
  assert(!rosary_mystery_set_for_weekday(7));
}
static void test_recoverable_settings(void) {
  for (int torn = 0; torn < 16 + sizeof(AppSettings); ++torn) {
    storage_reset();
    app_settings_init();
    assert(app_settings_set_text_size(APP_TEXT_SIZE_EXTRA_LARGE));
    AppSettings before = app_settings_get();
    AppSettings after = before;
    after.appearance = APP_APPEARANCE_DARK;
    after.slots[0] = MAIN_MENU_ENTRY_PRAYER_CARDS;
    storage_fail_next_write(torn);
    assert(!app_settings_apply(&after));
    AppSettings actual = app_settings_get();
    assert(memcmp(&actual, &before, sizeof(before)) == 0);
    app_settings_init();
    actual = app_settings_get();
    assert(memcmp(&actual, &before, sizeof(before)) == 0);
  }
  storage_reset();
  app_settings_init();
  assert(app_settings_set_text_size(APP_TEXT_SIZE_EXTRA_LARGE));
  assert(app_settings_set_appearance(APP_APPEARANCE_DARK));
  storage_corrupt(45); // Newest bank is corrupt; recover previous settings.
  app_settings_init();
  assert(app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE);
  assert(app_settings_get_appearance() == APP_APPEARANCE_LIGHT);
  AppSettings invalid = app_settings_get();
  invalid.slots[1] = invalid.slots[0];
  assert(!app_settings_apply(&invalid));
  assert(!app_settings_set_main_menu_slot(0, (MainMenuEntryId)256));
}
static void test_reading_position(void) {
  storage_reset();
  app_settings_init();
  ReadingPosition position = {.entry = MAIN_MENU_ENTRY_ASPIRATIONS,
    .kind = READING_POSITION_STYLED, .anchor = 80, .fraction = 32768};
  assert(reading_position_save(&position));
  ReadingPosition restored;
  app_settings_init();
  assert(reading_position_get(&restored));
  assert(memcmp(&position, &restored, sizeof(position)) == 0);
  storage_fail_next_write(4);
  position.anchor = 81;
  assert(!reading_position_save(&position));
  assert(reading_position_get(&restored) && restored.anchor == 80);
  assert(app_settings_set_remember_place(false));
  assert(!reading_position_get(&restored));
  reading_position_clear();
  assert(app_settings_set_remember_place(true));
  assert(!reading_position_get(&restored));
  position.entry = MAIN_MENU_ENTRY_HOLY_ROSARY;
  assert(!reading_position_save(&position));
  uint16_t fraction = reading_position_fraction(150, 100, 200);
  assert(reading_position_offset(fraction, 200, 400) >= 299);
  assert(reading_position_fraction(-10, 0, 0) == 0);
  assert(reading_position_fraction(900, 0, 100) == UINT16_MAX);
}
static void test_reminder_schedule(void) {
  setenv("TZ", "America/New_York", 1);
  tzset();
  struct tm start = {.tm_year = 126, .tm_mon = 2, .tm_mday = 7,
                    .tm_hour = 13, .tm_isdst = -1};
  time_t now = mktime(&start);
  time_t next = reminder_schedule_next_noon(now);
  struct tm result = *localtime(&next);
  assert(result.tm_mday == 8 && result.tm_hour == 12);
  assert(next - now == 22 * 60 * 60); // Spring DST transition.
  start = (struct tm){.tm_year = 126, .tm_mon = 9, .tm_mday = 31,
                     .tm_hour = 13, .tm_isdst = -1};
  now = mktime(&start);
  next = reminder_schedule_next_noon(now);
  result = *localtime(&next);
  assert(result.tm_mon == 10 && result.tm_mday == 1 && result.tm_hour == 12);
  assert(next - now == 24 * 60 * 60); // Autumn DST transition.
  assert(reminder_schedule_next_noon(next) > next);
  assert(reminder_schedule_is_timely(next + 900, next));
  assert(!reminder_schedule_is_timely(next + 901, next));
  assert(!reminder_schedule_is_timely(next - 61, next));
  assert(reminder_schedule_is_timely(next, 0));
}
static void test_catalog_destinations(void) {
  for (unsigned i = 1; i < main_menu_catalog_count(); ++i) {
    const MainMenuEntry *entry = main_menu_catalog_get((MainMenuEntryId)i);
    assert(entry && entry->name[0]);
    assert(entry->category < main_menu_catalog_category_count());
    const Prayer *prayer = NULL;
    if (entry->destination == MAIN_MENU_DESTINATION_PRAYER) {
      prayer = prayers_get_by_id((PrayerId)entry->target);
      assert(prayer);
    } else if (entry->destination == MAIN_MENU_DESTINATION_COLLECTION ||
               entry->destination == MAIN_MENU_DESTINATION_COLLECTION_PRAYER) {
      const PrayerCollection *collection = prayer_collections_get((PrayerCollectionId)entry->target);
      assert(collection && collection->prayer_count);
      if (entry->destination == MAIN_MENU_DESTINATION_COLLECTION_PRAYER) {
        assert(entry->item_index < collection->prayer_count);
        prayer = &collection->prayers[entry->item_index];
      }
    } else assert(entry->destination == MAIN_MENU_DESTINATION_LITANY);
    if (prayer) {
      assert(main_menu_catalog_find_prayer(prayer) == i);
      if (prayer->destination != PRAYER_DESTINATION_ROSARY) {
        const PrayerTranslation *translation = prayer_get_translation(prayer, prayer->default_language);
        assert(translation && (translation->text || translation->paragraph_count));
      }
    }
  }
}
static void test_packaged_preces(void) {
  const Prayer *prayer = prayers_get_by_id(PRAYER_ID_PRECES);
  const char *text = prayer_get_translation(prayer, prayer->default_language)->text;
  FILE *file = fopen("resources/data/preces.bin", "rb");
  assert(file);
  for (size_t i=0;i<=strlen(text);++i) assert(fgetc(file) == (unsigned char)text[i]);
  assert(fgetc(file) == EOF);
  fclose(file);
}
int main(void) {
  test_packaged_preces();
  extern void run_phone_tests(void);
  test_settings();
  test_navigation_migration();
  test_calendar();
  test_recoverable_settings();
  test_reading_position();
  test_reminder_schedule();
  test_catalog_destinations();
  run_phone_tests();
  puts("Core regression tests passed");
}
