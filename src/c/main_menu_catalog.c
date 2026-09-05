#include "main_menu_catalog.h"

#include <stddef.h>

#include "catalog_entries.inc"

uint16_t main_menu_catalog_count(void) {
  return (uint16_t)(sizeof(s_entries) / sizeof(s_entries[0]));
}

bool main_menu_catalog_is_card(MainMenuEntryId entry_id) {
  const MainMenuEntry *entry = main_menu_catalog_get(entry_id);
  return entry && entry->destination == MAIN_MENU_DESTINATION_COLLECTION_PRAYER &&
         entry->target == PRAYER_COLLECTION_CARDS;
}

const MainMenuEntry *main_menu_catalog_get(MainMenuEntryId entry_id) {
  return (uint32_t)entry_id < main_menu_catalog_count()
             ? &s_entries[entry_id]
             : NULL;
}

uint8_t main_menu_catalog_category_count(void) {
  return sizeof(s_categories) / sizeof(s_categories[0]);
}
const char *main_menu_catalog_category_name(uint8_t category) {
  return category < main_menu_catalog_category_count() ? s_categories[category] : "Prayers";
}
MainMenuEntryId main_menu_catalog_find_prayer(const Prayer *prayer) {
  if (!prayer) return MAIN_MENU_ENTRY_NONE;
  for (uint16_t i = 1; i < main_menu_catalog_count(); ++i) {
    const MainMenuEntry *entry = &s_entries[i];
    if (entry->destination == MAIN_MENU_DESTINATION_PRAYER &&
        prayers_get_by_id((PrayerId)entry->target) == prayer) return (MainMenuEntryId)i;
    if (entry->destination == MAIN_MENU_DESTINATION_COLLECTION_PRAYER) {
      const PrayerCollection *collection = prayer_collections_get((PrayerCollectionId)entry->target);
      if (collection && entry->item_index < collection->prayer_count &&
          &collection->prayers[entry->item_index] == prayer) return (MainMenuEntryId)i;
    }
  }
  return MAIN_MENU_ENTRY_NONE;
}
