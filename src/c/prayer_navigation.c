#include "prayer_navigation.h"
#include "litany.h"
#include "placeholder_screen.h"
#include "prayer_collection_menu.h"
#include "prayer_screen.h"
#include "rosary_menu.h"
#include <stddef.h>

void prayer_navigation_open(MainMenuEntryId entry_id, bool resume) {
  const MainMenuEntry *entry = main_menu_catalog_get(entry_id);
  if (!entry) return;
  const Prayer *prayer = NULL;
  switch (entry->destination) {
    case MAIN_MENU_DESTINATION_PRAYER:
      prayer = prayers_get_by_id((PrayerId)entry->target);
      if (prayer && prayer->destination == PRAYER_DESTINATION_ROSARY) {
        rosary_menu_show();
        return;
      }
      break;
    case MAIN_MENU_DESTINATION_LITANY: {
      const PrayerTranslation translation = {.text = litany_of_loreto_text()};
      prayer_screen_show_entry(entry_id, entry->name, &translation, resume);
      return;
    }
    case MAIN_MENU_DESTINATION_COLLECTION:
      prayer_collection_menu_show((PrayerCollectionId)entry->target);
      return;
    case MAIN_MENU_DESTINATION_COLLECTION_PRAYER: {
      const PrayerCollection *collection = prayer_collections_get((PrayerCollectionId)entry->target);
      if (collection && entry->item_index < collection->prayer_count) prayer = &collection->prayers[entry->item_index];
      break;
    }
    default: return;
  }
  const PrayerTranslation *translation = prayer ? prayer_get_translation(prayer, prayer->default_language) : NULL;
  if (translation) prayer_screen_show_entry(entry_id, prayer->name, translation, resume);
  else placeholder_screen_show(entry->name);
}
