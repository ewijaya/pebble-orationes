#include "app_settings.h"
#include "prayer_navigation.h"
#include "prayer_screen.h"
#include "prayer_collection_menu.h"
#include "placeholder_screen.h"
#include "rosary_menu.h"
#include "reading_position.h"
#include "storage.h"
#include <assert.h>
#include <stdio.h>

static unsigned s_reader_calls, s_menu_calls;
static MainMenuEntryId s_entry;
static bool s_resume;

void prayer_screen_show_entry(MainMenuEntryId entry, const char *title,
                              const PrayerTranslation *translation, bool resume) {
  assert(title && translation);
  assert(translation->text || translation->paragraph_count);
  s_reader_calls++;
  s_entry = entry;
  s_resume = resume;
}
void prayer_collection_menu_show(PrayerCollectionId collection) { s_menu_calls++; }
void rosary_menu_show(void) { s_menu_calls++; }
void placeholder_screen_show(const char *name) { assert(!"Unexpected placeholder"); }

void run_prayer_navigation_tests(void) {
  storage_reset();
  app_settings_init();
  assert(app_settings_get_remember_place());
  // All menu callers share this entry point; none can override the preference.
  for (unsigned remember = 0; remember <= 1; ++remember) {
    assert(app_settings_set_remember_place(remember));
    app_settings_init();
    for (unsigned id = 1; id < main_menu_catalog_count(); ++id) {
      const MainMenuEntry *entry = main_menu_catalog_get(id);
      s_reader_calls = s_menu_calls = 0;
      prayer_navigation_open(id);
      if (entry->destination == MAIN_MENU_DESTINATION_COLLECTION ||
          id == MAIN_MENU_ENTRY_HOLY_ROSARY) {
        assert(s_menu_calls == 1 && s_reader_calls == 0);
      } else {
        assert(s_reader_calls == 1 && s_menu_calls == 0);
        assert(s_entry == id && s_resume == (bool)remember);
      }
    }
  }
  // Opening does not consume or replace a bookmark before the reader saves it.
  ReadingPosition saved = {.entry = MAIN_MENU_ENTRY_PRECES,
    .kind = READING_POSITION_PLAIN, .anchor = 40, .fraction = 12345};
  ReadingPosition actual;
  assert(reading_position_save(&saved));
  prayer_navigation_open(MAIN_MENU_ENTRY_PRECES);
  assert(s_resume);
  assert(reading_position_get_entry(saved.entry, &actual));
  assert(actual.anchor == saved.anchor && actual.fraction == saved.fraction);
  storage_fail_next_write(0);
  assert(!app_settings_set_remember_place(false));
  prayer_navigation_open(MAIN_MENU_ENTRY_PRECES);
  assert(s_resume); // Failed preference saves must not change opening behavior.
  assert(app_settings_set_remember_place(false));
  prayer_navigation_open(MAIN_MENU_ENTRY_PRECES);
  assert(!s_resume && !reading_position_get_entry(saved.entry, &actual));
  assert(!reading_position_save(&saved));
  s_reader_calls = s_menu_calls = 0;
  prayer_navigation_open(MAIN_MENU_ENTRY_NONE);
  prayer_navigation_open((MainMenuEntryId)255);
  assert(!s_reader_calls && !s_menu_calls);
  puts("All catalog openings obey Remember Place; menu routing and bookmarks preserved");
}
