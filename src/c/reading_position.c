#include "reading_position.h"
#include "app_settings.h"
#include "durable_store.h"
#include <string.h>

enum { POSITION_KEY = 48, POSITION_SCHEMA = 1, LEGACY_POSITION_KEY = 42 };
typedef struct { ReadingPosition entries[READING_RECENT_COUNT]; } History;
static bool valid(const ReadingPosition *position) {
  const MainMenuEntry *entry = main_menu_catalog_get((MainMenuEntryId)position->entry);
  return entry && position->kind <= READING_POSITION_STYLED &&
    (entry->destination == MAIN_MENU_DESTINATION_LITANY ||
     entry->destination == MAIN_MENU_DESTINATION_COLLECTION_PRAYER ||
     (entry->destination == MAIN_MENU_DESTINATION_PRAYER &&
      entry->target != PRAYER_ID_HOLY_ROSARY));
}
static void load(History *history) {
  memset(history, 0, sizeof(*history));
  if (!durable_store_read(POSITION_KEY, POSITION_SCHEMA, history, sizeof(*history))) {
    ReadingPosition legacy;
    if (durable_store_read(LEGACY_POSITION_KEY, 1, &legacy, sizeof(legacy)) && valid(&legacy))
      history->entries[0] = legacy;
  }
}
bool reading_position_recent(uint8_t index, ReadingPosition *position) {
  if (!position || index >= READING_RECENT_COUNT || !app_settings_get_remember_place()) return false;
  History history;
  load(&history);
  if (!valid(&history.entries[index])) return false;
  *position = history.entries[index];
  return true;
}
bool reading_position_get(ReadingPosition *position) {
  return reading_position_recent(0, position);
}
bool reading_position_get_entry(uint8_t entry, ReadingPosition *position) {
  for (uint8_t i = 0; i < READING_RECENT_COUNT; ++i) {
    ReadingPosition candidate;
    if (reading_position_recent(i, &candidate) && candidate.entry == entry) {
      if (position) *position = candidate;
      return true;
    }
  }
  return false;
}
bool reading_position_save(const ReadingPosition *position) {
  if (!position || !app_settings_get_remember_place() || !valid(position)) return false;
  History old, updated = {0};
  load(&old);
  updated.entries[0] = *position;
  uint8_t next = 1;
  for (uint8_t i = 0; i < READING_RECENT_COUNT && next < READING_RECENT_COUNT; ++i)
    if (valid(&old.entries[i]) && old.entries[i].entry != position->entry)
      updated.entries[next++] = old.entries[i];
  if (memcmp(&old, &updated, sizeof(old)) == 0) return true;
  return durable_store_write(POSITION_KEY, POSITION_SCHEMA, &updated, sizeof(updated));
}
void reading_position_clear(void) {
  durable_store_clear(POSITION_KEY);
  durable_store_clear(LEGACY_POSITION_KEY);
}
uint16_t reading_position_fraction(int32_t offset, int32_t start, int32_t end) {
  if (offset <= start || end <= start) return 0;
  if (offset >= end) return UINT16_MAX;
  return ((int64_t)(offset - start) * UINT16_MAX + (end - start) / 2) / (end - start);
}
int32_t reading_position_offset(uint16_t fraction, int32_t start, int32_t end) {
  return start + ((int64_t)fraction * (end > start ? end - start : 0) + UINT16_MAX / 2) / UINT16_MAX;
}
