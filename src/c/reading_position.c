#include "reading_position.h"
#include "app_settings.h"
#include "durable_store.h"
#include <string.h>

enum { POSITION_KEY = 42, POSITION_SCHEMA = 1 };
static bool valid(const ReadingPosition *position) {
  const MainMenuEntry *entry = main_menu_catalog_get((MainMenuEntryId)position->entry);
  return entry && position->kind <= READING_POSITION_STYLED &&
    (entry->destination == MAIN_MENU_DESTINATION_LITANY ||
     entry->destination == MAIN_MENU_DESTINATION_COLLECTION_PRAYER ||
     (entry->destination == MAIN_MENU_DESTINATION_PRAYER &&
      entry->target != PRAYER_ID_HOLY_ROSARY));
}
bool reading_position_get(ReadingPosition *position) {
  return position && app_settings_get_remember_place() &&
    durable_store_read(POSITION_KEY, POSITION_SCHEMA, position, sizeof(*position)) && valid(position);
}
bool reading_position_save(const ReadingPosition *position) {
  if (!position || !app_settings_get_remember_place() || !valid(position)) return false;
  ReadingPosition old;
  if (reading_position_get(&old) && memcmp(&old, position, sizeof(old)) == 0) return true;
  return durable_store_write(POSITION_KEY, POSITION_SCHEMA, position, sizeof(*position));
}
void reading_position_clear(void) { durable_store_clear(POSITION_KEY); }
uint16_t reading_position_fraction(int32_t offset, int32_t start, int32_t end) {
  if (offset <= start || end <= start) return 0;
  if (offset >= end) return UINT16_MAX;
  return ((int64_t)(offset - start) * UINT16_MAX + (end - start) / 2) / (end - start);
}
int32_t reading_position_offset(uint16_t fraction, int32_t start, int32_t end) {
  return start + ((int64_t)fraction * (end > start ? end - start : 0) + UINT16_MAX / 2) / UINT16_MAX;
}
