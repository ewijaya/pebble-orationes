#include <pebble.h>

#include "placeholder_screen.h"
#include "prayer_screen.h"
#include "prayers.h"

static Window *s_menu_window;
static MenuLayer *s_menu_layer;

static uint16_t menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index,
                                  void *context) {
  return prayers_count();
}

static void menu_draw_row(GContext *ctx, const Layer *cell_layer,
                          MenuIndex *cell_index, void *context) {
  const Prayer *prayer = prayers_get(cell_index->row);
  menu_cell_basic_draw(ctx, cell_layer, prayer->name, NULL, NULL);
}

static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index,
                              void *context) {
  const Prayer *prayer = prayers_get(cell_index->row);
  const PrayerTranslation *translation =
      prayer_get_translation(prayer, PRAYER_LANGUAGE_ENGLISH);

  if (translation) {
    prayer_screen_show(prayer->name, translation->text);
  } else {
    placeholder_screen_show(prayer->name);
  }
}

static void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
      .get_num_rows = menu_get_num_rows,
      .draw_row = menu_draw_row,
      .select_click = menu_select_click,
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void menu_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  s_menu_layer = NULL;
}

static void init(void) {
  prayer_screen_init();
  placeholder_screen_init();

  s_menu_window = window_create();
  window_set_window_handlers(s_menu_window, (WindowHandlers){
      .load = menu_window_load,
      .unload = menu_window_unload,
  });
  window_stack_push(s_menu_window, true);
}

static void deinit(void) {
  window_destroy(s_menu_window);
  s_menu_window = NULL;

  placeholder_screen_deinit();
  prayer_screen_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
