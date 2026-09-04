#include <pebble.h>

#include "accessible_menu.h"
#include "app_settings.h"
#include "noon_reminder.h"
#include "placeholder_screen.h"
#include "prayer_screen.h"
#include "prayers.h"
#include "rosary_menu.h"
#include "settings_menu.h"

enum {
  MAIN_MENU_ITEM_SETTINGS_OFFSET = 1,
};

static Window *s_menu_window;
static MenuLayer *s_menu_layer;

static uint16_t menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index,
                                  void *context) {
  return prayers_count() + MAIN_MENU_ITEM_SETTINGS_OFFSET;
}

static void menu_draw_row(GContext *ctx, const Layer *cell_layer,
                          MenuIndex *cell_index, void *context) {
  if (cell_index->row == prayers_count()) {
    accessible_menu_draw_row(ctx, cell_layer, "Settings");
    return;
  }

  const Prayer *prayer = prayers_get(cell_index->row);
  accessible_menu_draw_row(ctx, cell_layer, prayer->name);
}

static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index,
                              void *context) {
  if (cell_index->row == prayers_count()) {
    settings_menu_show();
    return;
  }

  const Prayer *prayer = prayers_get(cell_index->row);

  if (prayer->destination == PRAYER_DESTINATION_ROSARY) {
    rosary_menu_show();
    return;
  }

  const PrayerTranslation *translation =
      prayer_get_translation(prayer, prayer->default_language);

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
      .get_cell_height = accessible_menu_get_cell_height,
      .draw_row = menu_draw_row,
      .select_click = menu_select_click,
  });
  accessible_menu_apply_colors(s_menu_layer);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void menu_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  s_menu_layer = NULL;
}

static void menu_window_appear(Window *window) {
  if (!s_menu_layer) {
    return;
  }

  accessible_menu_apply_colors(s_menu_layer);
  layer_mark_dirty(menu_layer_get_layer(s_menu_layer));
}

static bool init(void) {
#if defined(PBL_TOUCH)
  app_touch_navigation_enable(true);
#endif

  app_settings_init();
  prayer_screen_init();
  placeholder_screen_init();
  rosary_menu_init();
  settings_menu_init();

  s_menu_window = window_create();
  window_set_window_handlers(s_menu_window, (WindowHandlers){
      .load = menu_window_load,
      .appear = menu_window_appear,
      .unload = menu_window_unload,
  });

  const bool show_noon_reminder = noon_reminder_init();
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    if (show_noon_reminder) {
      noon_reminder_show();
    }
    return show_noon_reminder;
  }

  window_stack_push(s_menu_window, true);
  return true;
}

static void deinit(void) {
  noon_reminder_deinit();

  window_destroy(s_menu_window);
  s_menu_window = NULL;

  settings_menu_deinit();
  rosary_menu_deinit();
  placeholder_screen_deinit();
  prayer_screen_deinit();
}

int main(void) {
  if (init()) {
    app_event_loop();
  }
  deinit();
}
