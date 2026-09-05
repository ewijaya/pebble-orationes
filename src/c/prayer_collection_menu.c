#include "prayer_collection_menu.h"

#include <pebble.h>

#include "accessible_menu.h"
#include "prayer_navigation.h"

static Window *s_window;
static MenuLayer *s_menu_layer;
static const PrayerCollection *s_collection;
static bool s_is_cards;

static int16_t menu_get_cell_height(MenuLayer *menu_layer,
                                    MenuIndex *cell_index, void *context) {
  return s_is_cards && s_collection && cell_index->row < s_collection->prayer_count
      ? accessible_menu_wrapped_row_height(
            menu_layer, s_collection->prayers[cell_index->row].name)
      : ACCESSIBLE_MENU_ROW_HEIGHT;
}

static uint16_t menu_get_num_rows(MenuLayer *menu_layer,
                                  uint16_t section_index, void *context) {
  return s_collection ? s_collection->prayer_count : 0;
}

static void menu_draw_header(GContext *ctx, const Layer *cell_layer,
                             uint16_t section_index, void *context) {
  accessible_menu_draw_header(
      ctx, cell_layer, section_index,
      (void *)(s_collection ? s_collection->name : "Prayers"));
}

static void menu_draw_row(GContext *ctx, const Layer *cell_layer,
                          MenuIndex *cell_index, void *context) {
  if (!s_collection || cell_index->row >= s_collection->prayer_count) {
    return;
  }

  accessible_menu_draw_row(ctx, cell_layer,
                           s_collection->prayers[cell_index->row].name);
}

static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index,
                              void *context) {
  if (!s_collection || cell_index->row >= s_collection->prayer_count) {
    return;
  }

  const Prayer *prayer = &s_collection->prayers[cell_index->row];
  prayer_navigation_open(main_menu_catalog_find_prayer(prayer), false);
}

static void cards_move_selection(bool up) {
  const MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
  const uint16_t count = s_collection->prayer_count;
  if ((up && selected.row == 0) || (!up && selected.row == count - 1)) {
    menu_layer_set_selected_index(
        s_menu_layer, MenuIndex(0, up ? count - 1 : 0),
        up ? MenuRowAlignBottom : MenuRowAlignTop, true);
  } else {
    menu_layer_set_selected_next(s_menu_layer, up, MenuRowAlignCenter, true);
  }
}

static void cards_up_handler(ClickRecognizerRef recognizer, void *context) {
  cards_move_selection(true);
}

static void cards_down_handler(ClickRecognizerRef recognizer, void *context) {
  cards_move_selection(false);
}

static void cards_select_handler(ClickRecognizerRef recognizer, void *context) {
  MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
  menu_select_click(s_menu_layer, &selected, NULL);
}

static void cards_click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, cards_up_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, cards_down_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, cards_select_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
      .get_num_rows = menu_get_num_rows,
      .get_cell_height = menu_get_cell_height,
      .get_header_height = accessible_menu_get_header_height,
      .draw_row = menu_draw_row,
      .draw_header = menu_draw_header,
      .select_click = menu_select_click,
  });
  accessible_menu_apply_colors(s_menu_layer);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  if (s_is_cards) {
    window_set_click_config_provider(window, cards_click_config_provider);
  }
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_appear(Window *window) {
  if (!s_menu_layer) {
    return;
  }

  accessible_menu_apply_colors(s_menu_layer);
  menu_layer_reload_data(s_menu_layer);
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  s_menu_layer = NULL;
}

void prayer_collection_menu_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
      .load = window_load,
      .appear = window_appear,
      .unload = window_unload,
  });
}

void prayer_collection_menu_deinit(void) {
  window_destroy(s_window);
  s_window = NULL;
}

void prayer_collection_menu_show(PrayerCollectionId collection_id) {
  const PrayerCollection *collection =
      prayer_collections_get(collection_id);
  if (!collection) {
    return;
  }

  if (window_stack_contains_window(s_window)) {
    window_stack_remove(s_window, false);
  }

  s_collection = collection;
  s_is_cards = collection_id == PRAYER_COLLECTION_CARDS;
  window_stack_push(s_window, true);
}

void prayer_collection_menu_refresh(void) {
  if (!s_menu_layer) {
    return;
  }
  accessible_menu_apply_colors(s_menu_layer);
  menu_layer_reload_data(s_menu_layer);
  layer_mark_dirty(menu_layer_get_layer(s_menu_layer));
}
