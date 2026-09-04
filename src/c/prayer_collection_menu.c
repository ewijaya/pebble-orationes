#include "prayer_collection_menu.h"

#include <pebble.h>

#include "accessible_menu.h"
#include "prayer_screen.h"

static Window *s_window;
static MenuLayer *s_menu_layer;
static const PrayerCollection *s_collection;

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
  const PrayerTranslation *translation =
      prayer_get_translation(prayer, prayer->default_language);
  if (translation) {
    prayer_screen_show(prayer->name, translation->text);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
      .get_num_rows = menu_get_num_rows,
      .get_cell_height = accessible_menu_get_cell_height,
      .get_header_height = accessible_menu_get_header_height,
      .draw_row = menu_draw_row,
      .draw_header = menu_draw_header,
      .select_click = menu_select_click,
  });
  accessible_menu_apply_colors(s_menu_layer);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
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
  window_stack_push(s_window, true);
}
