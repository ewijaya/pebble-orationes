#include "rosary_menu.h"

#include <pebble.h>
#include <stdio.h>

#include "accessible_menu.h"
#include "litany.h"
#include "prayer_screen.h"
#include "prayer_navigation.h"
#include "rosary_data.h"

enum {
  ROSARY_MENU_ITEM_TODAY,
  ROSARY_MENU_ITEM_ALL,
  ROSARY_MENU_ITEM_LITANY,
  ROSARY_MENU_ITEM_COUNT,
  MYSTERY_TITLE_BUFFER_SIZE = 32,
  MYSTERY_BODY_BUFFER_SIZE = 256,
};

static Window *s_rosary_window;
static MenuLayer *s_rosary_menu_layer;
static Window *s_all_mysteries_window;
static MenuLayer *s_all_mysteries_menu_layer;
static char s_mystery_title[MYSTERY_TITLE_BUFFER_SIZE];
static char s_mystery_body[MYSTERY_BODY_BUFFER_SIZE];

static void show_mystery_set(const RosaryMysterySet *set, bool for_today,
                             const char *weekday_label) {
  if (!set) {
    return;
  }

  snprintf(s_mystery_title, sizeof(s_mystery_title), "%s · %s", set->name,
           weekday_label);

  size_t used = 0;
  s_mystery_body[0] = '\0';
  for (uint8_t index = 0; index < ROSARY_MYSTERIES_PER_SET; ++index) {
    const RosaryMystery *mystery = &set->mysteries[index];
    const char *label = for_today ? mystery->today_label
                                  : mystery->compact_label;
    const int written = snprintf(s_mystery_body + used,
                                 sizeof(s_mystery_body) - used,
                                 "%u. %s%s", (unsigned)(index + 1), label,
                                 index + 1 < ROSARY_MYSTERIES_PER_SET ? "\n" : "");
    if (written < 0 || (size_t)written >= sizeof(s_mystery_body) - used) {
      break;
    }
    used += (size_t)written;
  }

  prayer_screen_show(s_mystery_title, s_mystery_body);
}

static uint16_t rosary_menu_get_num_rows(MenuLayer *menu_layer,
                                         uint16_t section_index,
                                         void *context) {
  return ROSARY_MENU_ITEM_COUNT;
}

static const char *const s_rosary_labels[] = {
    "Today's Mysteries",
    "All Mysteries",
    "Litany of Loreto",
};
static char s_today_detail[40];
static void update_today(void) {
  const time_t now = time(NULL);
  const struct tm *local = localtime(&now);
  if (!local) { s_today_detail[0] = '\0'; return; }
  const RosaryMysterySet *set = rosary_mystery_set_for_weekday(local->tm_wday);
  snprintf(s_today_detail, sizeof(s_today_detail), "%s · %s", set->name,
           rosary_weekday_abbreviation(local->tm_wday));
}
static int16_t rosary_height(MenuLayer *layer, MenuIndex *index,
                             void *context) {
  if (index->row == ROSARY_MENU_ITEM_TODAY)
    return accessible_menu_detail_height(layer, s_rosary_labels[0], s_today_detail, UI_SYMBOL_NONE);
  if (index->row == ROSARY_MENU_ITEM_ALL)
    return accessible_menu_submenu_height(layer, s_rosary_labels[index->row]);
  return accessible_menu_wrapped_row_height(layer, s_rosary_labels[index->row]);
}
static int16_t mystery_height(MenuLayer *layer, MenuIndex *index,
                              void *context) {
  const RosaryMysterySet *set = rosary_mystery_set_get(index->row);
  return accessible_menu_detail_height(layer, set->name, set->weekday_label, UI_SYMBOL_NONE);
}
static void rosary_menu_draw_row(GContext *ctx, const Layer *cell_layer,
                                 MenuIndex *cell_index, void *context) {
  if (cell_index->row == ROSARY_MENU_ITEM_TODAY)
    accessible_menu_draw_detail(ctx, cell_layer, s_rosary_labels[0], s_today_detail, UI_SYMBOL_NONE);
  else if (cell_index->row == ROSARY_MENU_ITEM_ALL)
    accessible_menu_draw_submenu(ctx, cell_layer, s_rosary_labels[cell_index->row]);
  else accessible_menu_draw_row(ctx, cell_layer, s_rosary_labels[cell_index->row]);
}

static void rosary_menu_select_click(MenuLayer *menu_layer,
                                     MenuIndex *cell_index, void *context) {
  switch (cell_index->row) {
    case ROSARY_MENU_ITEM_TODAY: {
      const time_t current_time = time(NULL);
      const struct tm *local_time = localtime(&current_time);
      if (local_time) {
        show_mystery_set(
            rosary_mystery_set_for_weekday(local_time->tm_wday), true,
            rosary_weekday_abbreviation(local_time->tm_wday));
      }
      break;
    }
    case ROSARY_MENU_ITEM_ALL:
      window_stack_push(s_all_mysteries_window, true);
      break;
    case ROSARY_MENU_ITEM_LITANY:
      prayer_navigation_open(MAIN_MENU_ENTRY_LITANY_OF_LORETO);
      break;
  }
}

static uint16_t all_mysteries_get_num_rows(MenuLayer *menu_layer,
                                           uint16_t section_index,
                                           void *context) {
  return rosary_mystery_set_count();
}

static void all_mysteries_draw_row(GContext *ctx, const Layer *cell_layer,
                                   MenuIndex *cell_index, void *context) {
  const RosaryMysterySet *set = rosary_mystery_set_get(cell_index->row);
  accessible_menu_draw_detail(ctx, cell_layer, set->name, set->weekday_label, UI_SYMBOL_NONE);
}

static void all_mysteries_select_click(MenuLayer *menu_layer,
                                       MenuIndex *cell_index, void *context) {
  const RosaryMysterySet *set = rosary_mystery_set_get(cell_index->row);
  show_mystery_set(set, false, set->weekday_label);
}

static void menu_window_load(Window *window, MenuLayer **menu_layer,
                             const char *header,
                             MenuLayerGetNumberOfRowsInSectionsCallback get_rows,
                             MenuLayerDrawRowCallback draw_row,
                             MenuLayerSelectCallback select_click) {
  Layer *window_layer = window_get_root_layer(window);
  *menu_layer = accessible_menu_create(layer_get_bounds(window_layer));
  accessible_menu_set_callbacks(
      *menu_layer, (void *)header,
      (MenuLayerCallbacks){
          .get_num_rows = get_rows,
          .get_cell_height =
              window == s_rosary_window ? rosary_height : mystery_height,
          .get_header_height = accessible_menu_get_header_height,
          .draw_row = draw_row,
          .draw_header = accessible_menu_draw_header,
          .select_click = select_click,
      });
  accessible_menu_apply_colors(*menu_layer);
  menu_layer_set_click_config_onto_window(*menu_layer, window);
  accessible_menu_add_to_layer(window_layer, *menu_layer);
}

static void rosary_window_load(Window *window) {
  update_today();
  menu_window_load(window, &s_rosary_menu_layer, "Holy Rosary",
                   rosary_menu_get_num_rows, rosary_menu_draw_row,
                   rosary_menu_select_click);
}

static void rosary_window_unload(Window *window) {
  accessible_menu_destroy(s_rosary_menu_layer);
  s_rosary_menu_layer = NULL;
}

static void all_mysteries_window_load(Window *window) {
  menu_window_load(window, &s_all_mysteries_menu_layer, "All Mysteries",
                   all_mysteries_get_num_rows, all_mysteries_draw_row,
                   all_mysteries_select_click);
}

static void all_mysteries_window_unload(Window *window) {
  accessible_menu_destroy(s_all_mysteries_menu_layer);
  s_all_mysteries_menu_layer = NULL;
}

static void day_changed(struct tm *tick, TimeUnits units) { rosary_menu_refresh(); }
static void rosary_appear(Window *window) {
  rosary_menu_refresh();
  tick_timer_service_subscribe(DAY_UNIT, day_changed);
}
static void rosary_disappear(Window *window) { tick_timer_service_unsubscribe(); }

void rosary_menu_init(void) {
  s_rosary_window = window_create();
  window_set_window_handlers(s_rosary_window, (WindowHandlers){
      .load = rosary_window_load,
      .appear = rosary_appear,
      .disappear = rosary_disappear,
      .unload = rosary_window_unload,
  });

  s_all_mysteries_window = window_create();
  window_set_window_handlers(s_all_mysteries_window, (WindowHandlers){
      .load = all_mysteries_window_load,
      .unload = all_mysteries_window_unload,
  });
}

void rosary_menu_deinit(void) {
  window_destroy(s_all_mysteries_window);
  s_all_mysteries_window = NULL;

  window_destroy(s_rosary_window);
  s_rosary_window = NULL;
}

void rosary_menu_show(void) {
  rosary_menu_refresh();
  window_stack_push(s_rosary_window, true);
}

void rosary_menu_refresh(void) {
  update_today();
  MenuLayer *menu_layers[] = {
      s_rosary_menu_layer,
      s_all_mysteries_menu_layer,
  };
  for (uint8_t index = 0;
       index < sizeof(menu_layers) / sizeof(menu_layers[0]); ++index) {
    if (!menu_layers[index]) {
      continue;
    }
    accessible_menu_apply_colors(menu_layers[index]);
    menu_layer_reload_data(menu_layers[index]);
    layer_mark_dirty(menu_layer_get_layer(menu_layers[index]));
  }
}
