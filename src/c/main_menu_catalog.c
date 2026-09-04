#include "main_menu_catalog.h"

#include <stddef.h>

#define ENTRY(label, destination_type, destination_target, collection_index) \
  {                                                                       \
    .name = label, .destination = destination_type,                       \
    .target = destination_target, .item_index = collection_index,         \
  }

static const MainMenuEntry s_entries[] = {
    ENTRY("Empty", MAIN_MENU_DESTINATION_NONE, 0, 0),
    ENTRY("Preces", MAIN_MENU_DESTINATION_PRAYER, PRAYER_ID_PRECES, 0),
    ENTRY("Holy Rosary", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_HOLY_ROSARY, 0),
    ENTRY("Regina Caeli", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_REGINA_CAELI, 0),
    ENTRY("Angelus", MAIN_MENU_DESTINATION_PRAYER, PRAYER_ID_ANGELUS, 0),
    ENTRY("Memorare", MAIN_MENU_DESTINATION_PRAYER, PRAYER_ID_MEMORARE, 0),
    ENTRY("Litany of Loreto", MAIN_MENU_DESTINATION_LITANY, 0, 0),
    ENTRY("More Prayers", MAIN_MENU_DESTINATION_COLLECTION,
          PRAYER_COLLECTION_DAILY, 0),
    ENTRY("Mental Prayer", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_DAILY, 0),
    ENTRY("Visit & Communion", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_DAILY, 1),
    ENTRY("Before Work", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_DAILY, 2),
    ENTRY("Night Examination", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_DAILY, 3),
    ENTRY("Confession", MAIN_MENU_DESTINATION_COLLECTION,
          PRAYER_COLLECTION_CONFESSION, 0),
    ENTRY("Examination", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CONFESSION, 0),
    ENTRY("Act of Contrition", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CONFESSION, 1),
    ENTRY("Before Confession", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CONFESSION, 2),
    ENTRY("After Confession", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CONFESSION, 3),
};

uint16_t main_menu_catalog_count(void) {
  return (uint16_t)(sizeof(s_entries) / sizeof(s_entries[0]));
}

const MainMenuEntry *main_menu_catalog_get(MainMenuEntryId entry_id) {
  return (uint32_t)entry_id < main_menu_catalog_count()
             ? &s_entries[entry_id]
             : NULL;
}
