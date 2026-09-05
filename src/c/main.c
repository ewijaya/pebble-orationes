#include <pebble.h>

#include "accessible_menu.h"
#include "app_settings.h"
#include "litany.h"
#include "main_menu_catalog.h"
#include "noon_reminder.h"
#include "phone_settings.h"
#include "placeholder_screen.h"
#include "prayer_collection_menu.h"
#include "prayer_screen.h"
#include "prayers.h"
#include "rosary_menu.h"
#include "settings_menu.h"
#include "prayer_library.h"
#include "prayer_navigation.h"
#include "reading_position.h"

enum {
  MAIN_MENU_FIXED_ITEMS = 2,
  MAIN_MENU_REPEAT_INTERVAL_MS = 100,
};

static Window *s_menu_window;
static MenuLayer *s_menu_layer;

static uint16_t configured_entry_count(void) {
  uint16_t count = 0;
  for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
    if (app_settings_get_main_menu_slot(slot) != MAIN_MENU_ENTRY_NONE) {
      ++count;
    }
  }
  return count;
}

static MainMenuEntryId configured_entry_for_row(uint16_t row) {
  uint16_t visible_row = 0;
  for (uint8_t slot = 0; slot < APP_MAIN_MENU_SLOT_COUNT; ++slot) {
    const MainMenuEntryId entry_id = app_settings_get_main_menu_slot(slot);
    if (entry_id == MAIN_MENU_ENTRY_NONE) {
      continue;
    }
    if (visible_row == row) {
      return entry_id;
    }
    ++visible_row;
  }
  return MAIN_MENU_ENTRY_NONE;
}

static bool has_continue(void) {
  ReadingPosition position;
  return reading_position_get(&position);
}
static uint16_t menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index,
                                  void *context) {
  return configured_entry_count() + MAIN_MENU_FIXED_ITEMS + (has_continue() ? 1 : 0);
}

static void menu_draw_row(GContext *ctx, const Layer *cell_layer,
                          MenuIndex *cell_index, void *context) {
  uint16_t row = cell_index->row;
  const uint16_t entry_count = configured_entry_count();
  if (row < entry_count) {
    const MainMenuEntry *entry =
        main_menu_catalog_get(configured_entry_for_row(row));
    if (entry) {
      accessible_menu_draw_row(ctx, cell_layer, entry->name);
      return;
    }
  }

  row -= entry_count;
  const bool resume = has_continue();
  accessible_menu_draw_row(ctx, cell_layer,
      resume && row == 0 ? "Continue" : row == (resume ? 1 : 0) ? "All Prayers" : "Settings");
}

static int16_t menu_get_cell_height(MenuLayer *menu_layer,
                                    MenuIndex *cell_index, void *context) {
  const MainMenuEntryId entry_id = configured_entry_for_row(cell_index->row);
  return main_menu_catalog_is_card(entry_id)
      ? accessible_menu_wrapped_row_height(
            menu_layer, main_menu_catalog_get(entry_id)->name)
      : ACCESSIBLE_MENU_ROW_HEIGHT;
}

static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index,
                              void *context) {
  uint16_t row = cell_index->row;
  const uint16_t entry_count = configured_entry_count();
  if (row < entry_count) {
    prayer_navigation_open(configured_entry_for_row(row), false);
    return;
  }
  row -= entry_count;
  if (has_continue()) {
    if (row == 0) { prayer_library_continue(); return; }
    --row;
  }
  if (row == 0) prayer_library_show();
  else settings_menu_show();
}

static void menu_move_selection(bool up) {
  if (!s_menu_layer) {
    return;
  }

  const uint16_t row_count =
      menu_get_num_rows(s_menu_layer, 0, NULL);
  if (row_count == 0) {
    return;
  }

  const MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
  const bool should_wrap =
      (up && selected.row == 0) ||
      (!up && selected.row == row_count - 1);
  if (should_wrap) {
    menu_layer_set_selected_index(
        s_menu_layer, MenuIndex(0, up ? row_count - 1 : 0),
        up ? MenuRowAlignBottom : MenuRowAlignTop, true);
    return;
  }

  menu_layer_set_selected_next(s_menu_layer, up, MenuRowAlignCenter, true);
}

static void menu_up_click_handler(ClickRecognizerRef recognizer,
                                  void *context) {
  (void)recognizer;
  (void)context;
  menu_move_selection(true);
}

static void menu_down_click_handler(ClickRecognizerRef recognizer,
                                    void *context) {
  (void)recognizer;
  (void)context;
  menu_move_selection(false);
}

static void menu_select_click_handler(ClickRecognizerRef recognizer,
                                      void *context) {
  (void)recognizer;
  (void)context;
  MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
  menu_select_click(s_menu_layer, &selected, NULL);
}

static void menu_click_config_provider(void *context) {
  (void)context;
  window_single_repeating_click_subscribe(
      BUTTON_ID_UP, MAIN_MENU_REPEAT_INTERVAL_MS, menu_up_click_handler);
  window_single_repeating_click_subscribe(
      BUTTON_ID_DOWN, MAIN_MENU_REPEAT_INTERVAL_MS, menu_down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT,
                                menu_select_click_handler);
}

static void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
      .get_num_rows = menu_get_num_rows,
      .get_cell_height = menu_get_cell_height,
      .draw_row = menu_draw_row,
      .select_click = menu_select_click,
  });
  accessible_menu_apply_colors(s_menu_layer);
  window_set_click_config_provider(window, menu_click_config_provider);
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
  menu_layer_reload_data(s_menu_layer);
  layer_mark_dirty(menu_layer_get_layer(s_menu_layer));
}

static void settings_changed_handler(void) {
  if (!app_settings_get_remember_place()) reading_position_clear();
  if (s_menu_layer) {
    const uint16_t row_count = menu_get_num_rows(s_menu_layer, 0, NULL);
    const MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
    accessible_menu_apply_colors(s_menu_layer);
    menu_layer_reload_data(s_menu_layer);
    if (selected.row >= row_count) {
      menu_layer_set_selected_index(
          s_menu_layer, MenuIndex(0, row_count - 1), MenuRowAlignBottom,
          false);
    }
    layer_mark_dirty(menu_layer_get_layer(s_menu_layer));
  }

  prayer_screen_refresh();
  settings_menu_refresh();
  rosary_menu_refresh();
  prayer_collection_menu_refresh();
  prayer_library_refresh();
}

static void shortcut_saved_handler(uint8_t slot_index) {
  if (!s_menu_layer) {
    return;
  }

  // Empty slots are hidden. Count preceding visible entries to locate the
  // edited shortcut (or the next entry/Settings when this slot was cleared).
  uint16_t row = 0;
  for (uint8_t slot = 0; slot < slot_index; ++slot) {
    if (app_settings_get_main_menu_slot(slot) != MAIN_MENU_ENTRY_NONE) {
      ++row;
    }
  }
  menu_layer_reload_data(s_menu_layer);
  menu_layer_set_selected_index(s_menu_layer, MenuIndex(0, row),
                                MenuRowAlignCenter, false);
}

static bool init(void) {
#if defined(PBL_TOUCH)
  app_touch_navigation_enable(true);
#endif

  app_settings_init();
  prayer_screen_init();
  placeholder_screen_init();
  rosary_menu_init();
  prayer_collection_menu_init();
  settings_menu_init(shortcut_saved_handler);
  prayer_library_init(shortcut_saved_handler);

  s_menu_window = window_create();
  window_set_window_handlers(s_menu_window, (WindowHandlers){
      .load = menu_window_load,
      .appear = menu_window_appear,
      .unload = menu_window_unload,
  });

  const bool show_noon_reminder = noon_reminder_init();
  phone_settings_init(settings_changed_handler);
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
  phone_settings_deinit();
  noon_reminder_deinit();

  window_destroy(s_menu_window);
  s_menu_window = NULL;

  prayer_library_deinit();
  settings_menu_deinit();
  prayer_collection_menu_deinit();
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
