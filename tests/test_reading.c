#include "pebble.h"
#include "storage.h"
#include "app_settings.h"
#include "durable_store.h"
#include "reading_position.h"
#include "prayer_document.h"
#include "aspirations.h"
#include "litany.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void check_document(MainMenuEntryId entry, const PrayerTranslation *translation) {
  PrayerDocument document;
  if (!prayer_document_build(&document, entry, translation)) return;
  const unsigned char *source = (const unsigned char *)translation->text;
  unsigned sections = 0;
  for (uint16_t i = 0; i < document.count; ++i) {
    if (prayer_document_section(entry, document.paragraphs, document.count, i)) ++sections;
    if (document.anchors[i] == UINT32_MAX) {
      assert(document.paragraphs[i].style == PRAYER_PARAGRAPH_NOTE);
      continue;
    }
    const unsigned char *text = (const unsigned char *)document.paragraphs[i].text;
    // Every non-whitespace source byte appears exactly once, in order.
    for (; *text; ++text) {
      if (isspace(*text)) continue;
      while (isspace(*source)) ++source;
      assert(*source++ == *text);
    }
  }
  while (isspace(*source)) ++source;
  assert(!*source);
  assert(sections);
  if (entry == MAIN_MENU_ENTRY_COME_HOLY_SPIRIT) assert(sections == 10);
  if (entry == MAIN_MENU_ENTRY_LITANY_OF_HUMILITY) assert(sections == 4);
  if (entry == MAIN_MENU_ENTRY_LITANY_OF_LORETO) assert(sections == 6);
  prayer_document_free(&document);
}
void run_reading_tests(void) {
  for (unsigned i = 0; i < prayers_count(); ++i) {
    const Prayer *prayer = prayers_get(i);
    const PrayerTranslation *translation = prayer_get_translation(prayer, prayer->default_language);
    if (translation) check_document(main_menu_catalog_find_prayer(prayer), translation);
  }
  PrayerTranslation litany = {.text = litany_of_loreto_text()};
  check_document(MAIN_MENU_ENTRY_LITANY_OF_LORETO, &litany);
  const PrayerTranslation *aspirations = aspirations_translations;
  FILE *file = fopen("resources/data/aspirations.bin", "rb");
  assert(file);
  unsigned sections = 0, groups = 0;
  for (uint16_t i = 0; i < aspirations->paragraph_count; ++i) {
    const PrayerParagraph *p = &aspirations->paragraphs[i];
    assert(fgetc(file) == p->style && fgetc(file) == p->space_after);
    for (size_t j = 0; j <= strlen(p->text); ++j) assert(fgetc(file) == (unsigned char)p->text[j]);
    groups += p->space_after;
    sections += prayer_document_section(MAIN_MENU_ENTRY_ASPIRATIONS, aspirations->paragraphs, aspirations->paragraph_count, i);
  }
  assert(fgetc(file) == EOF); fclose(file);
  assert(groups == 92 && sections == 10);

  storage_reset(); app_settings_init();
  ReadingPosition legacy = {.entry = MAIN_MENU_ENTRY_PRECES, .kind = READING_POSITION_PLAIN, .anchor = 40};
  assert(durable_store_write(42, 1, &legacy, sizeof(legacy)));
  ReadingPosition position;
  assert(reading_position_get(&position) && position.anchor == 40);
  ReadingPosition next = {.entry = MAIN_MENU_ENTRY_ANGELUS, .kind = READING_POSITION_PLAIN, .anchor = 100};
  for (int torn = 0; torn < 16 + READING_RECENT_COUNT * sizeof(ReadingPosition); ++torn) {
    storage_fail_next_write(torn);
    assert(!reading_position_save(&next));
    assert(reading_position_get(&position) && position.entry == legacy.entry);
  }
  assert(reading_position_save(&next));
  assert(reading_position_get_entry(legacy.entry, &position) && position.anchor == 40);
  assert(reading_position_get(&position) && position.entry == next.entry);
  assert(reading_position_save(&legacy));
  assert(reading_position_recent(1, &position) && position.entry == next.entry);
  assert(!reading_position_recent(2, &position));
  for (unsigned entry = MAIN_MENU_ENTRY_CARD_JOSEMARIA; entry < MAIN_MENU_ENTRY_CARD_JOSEMARIA + 10; ++entry) {
    next.entry = entry;
    assert(reading_position_save(&next));
  }
  assert(!reading_position_get_entry(legacy.entry, &position));
  assert(reading_position_recent(7, &position));
  assert(!reading_position_recent(8, &position));
  reading_position_clear();
  assert(!reading_position_get(&position));
  assert(!app_settings_get_continue_first());
  storage_fail_next_write(0);
  assert(!app_settings_set_continue_first(true));
  assert(!app_settings_get_continue_first());
  assert(app_settings_set_continue_first(true));
  app_settings_init(); assert(app_settings_get_continue_first());
  AppSettings settings = app_settings_get();
  settings.appearance = APP_APPEARANCE_DARK;
  assert(app_settings_apply(&settings));
  app_settings_init(); assert(app_settings_get_continue_first());
  const uint8_t invalid = 255;
  assert(durable_store_write(46, 1, &invalid, 1));
  app_settings_init(); assert(!app_settings_get_continue_first());
}
