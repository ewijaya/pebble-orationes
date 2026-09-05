#pragma once

#include <stdint.h>

#include "prayer_collections.h"
#include "prayers.h"

#include "catalog_ids.h"

typedef enum {
  MAIN_MENU_DESTINATION_NONE,
  MAIN_MENU_DESTINATION_PRAYER,
  MAIN_MENU_DESTINATION_LITANY,
  MAIN_MENU_DESTINATION_COLLECTION,
  MAIN_MENU_DESTINATION_COLLECTION_PRAYER,
} MainMenuDestination;

typedef struct {
  const char *name;
  MainMenuDestination destination;
  uint8_t target;
  uint8_t item_index;
  uint8_t category;
} MainMenuEntry;

uint16_t main_menu_catalog_count(void);
const MainMenuEntry *main_menu_catalog_get(MainMenuEntryId entry_id);
bool main_menu_catalog_is_card(MainMenuEntryId entry_id);

extern const uint8_t main_menu_default_slots[7];
uint8_t main_menu_catalog_category_count(void);
const char *main_menu_catalog_category_name(uint8_t category);
MainMenuEntryId main_menu_catalog_find_prayer(const Prayer *prayer);
