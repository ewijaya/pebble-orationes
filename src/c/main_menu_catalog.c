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
    ENTRY("Psalm 50 (51)", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_PSALM_50, 0),
    ENTRY("Acceptance", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_ACCEPTANCE_OF_DEATH, 0),
    ENTRY("Vocations", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_PRAYER_FOR_VOCATIONS, 0),
    ENTRY("Blessed Purity", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_BLESSED_BE_YOUR_PURITY, 0),
    ENTRY("Three Children", MAIN_MENU_DESTINATION_PRAYER,
          PRAYER_ID_CANTICLE_THREE_CHILDREN, 0),
    ENTRY("Psalm 2", MAIN_MENU_DESTINATION_PRAYER, PRAYER_ID_PSALM_2, 0),
    ENTRY("Aspirations", MAIN_MENU_DESTINATION_PRAYER, PRAYER_ID_ASPIRATIONS, 0),
    ENTRY("Prayer Cards", MAIN_MENU_DESTINATION_COLLECTION,
          PRAYER_COLLECTION_CARDS, 0),
    ENTRY("St. Josemaría Escrivá", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 0),
    ENTRY("Bl. Álvaro del Portillo", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 1),
    ENTRY("Bl. Guadalupe Ortiz de Landázuri", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 2),
    ENTRY("Ven. Isidoro Zorzano", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 3),
    ENTRY("Ven. Montse Grases", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 4),
    ENTRY("Dora del Hoyo", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 5),
    ENTRY("Fr. Joseph Múzquiz", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 6),
    ENTRY("Fr. José María Hernández Garnica", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 7),
    ENTRY("Toni Zweifel", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 8),
    ENTRY("Ernesto Cofiño", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 9),
    ENTRY("Encarnita Ortega", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 10),
    ENTRY("Tomás & Paquita Alvira", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 11),
    ENTRY("Laura & Eduardo Ortiz de Landázuri", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 12),
    ENTRY("Pedro Ballester", MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
          PRAYER_COLLECTION_CARDS, 13),
};

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
