#include "settings_menu.h"

#include <pebble.h>

#include "accessible_menu.h"
#include "app_settings.h"
#include "noon_reminder.h"

enum {
  SETTINGS_MENU_ITEM_TEXT_SIZE,
  SETTINGS_MENU_ITEM_APPEARANCE,
  SETTINGS_MENU_ITEM_ACCENT_COLOR,
  SETTINGS_MENU_ITEM_NOON_REMINDER,
  SETTINGS_MENU_ITEM_COUNT,
};

enum {
  NOON_REMINDER_OPTION_OFF,
  NOON_REMINDER_OPTION_DURATION_OFFSET,
};

static Window *s_settings_window;
static MenuLayer *s_settings_menu_layer;
static Window *s_text_size_window;
static MenuLayer *s_text_size_menu_layer;
static Window *s_appearance_window;
static MenuLayer *s_appearance_menu_layer;
static Window *s_accent_color_window;
static MenuLayer *s_accent_color_menu_layer;
static Window *s_noon_reminder_window;
static MenuLayer *s_noon_reminder_menu_layer;

static uint16_t settings_get_num_rows(MenuLayer *menu_layer,
                                      uint16_t section_index, void *context) {
  return SETTINGS_MENU_ITEM_COUNT;
}

static void settings_draw_row(GContext *ctx, const Layer *cell_layer,
                              MenuIndex *cell_index, void *context) {
  static const char *const labels[] = {
      "Text Size",
      "Appearance",
      "Accent Color",
      "Noon Reminder",
  };
  const char *label = labels[cell_index->row];
  accessible_menu_draw_row(ctx, cell_layer, label);
}

static void settings_select_click(MenuLayer *menu_layer,
                                  MenuIndex *cell_index, void *context) {
  if (cell_index->row == SETTINGS_MENU_ITEM_TEXT_SIZE) {
    window_stack_push(s_text_size_window, true);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_APPEARANCE) {
    window_stack_push(s_appearance_window, true);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_ACCENT_COLOR) {
    window_stack_push(s_accent_color_window, true);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_NOON_REMINDER) {
    window_stack_push(s_noon_reminder_window, true);
  }
}

static uint16_t appearance_get_num_rows(MenuLayer *menu_layer,
                                        uint16_t section_index,
                                        void *context) {
  return APP_APPEARANCE_COUNT;
}

static void appearance_draw_row(GContext *ctx, const Layer *cell_layer,
                                MenuIndex *cell_index, void *context) {
  accessible_menu_draw_row(
      ctx, cell_layer,
      app_settings_appearance_label((AppAppearance)cell_index->row));
}

static void appearance_select_click(MenuLayer *menu_layer,
                                    MenuIndex *cell_index, void *context) {
  if (app_settings_set_appearance((AppAppearance)cell_index->row)) {
    window_stack_remove(s_settings_window, false);
    window_stack_pop(true);
  }
}

static uint16_t accent_color_get_num_rows(MenuLayer *menu_layer,
                                          uint16_t section_index,
                                          void *context) {
  return APP_ACCENT_COLOR_COUNT;
}

static void accent_color_draw_row(GContext *ctx, const Layer *cell_layer,
                                  MenuIndex *cell_index, void *context) {
  accessible_menu_draw_row(
      ctx, cell_layer,
      app_settings_accent_color_label((AppAccentColor)cell_index->row));
}

static void accent_color_select_click(MenuLayer *menu_layer,
                                      MenuIndex *cell_index, void *context) {
  if (app_settings_set_accent_color((AppAccentColor)cell_index->row)) {
    window_stack_remove(s_settings_window, false);
    window_stack_pop(true);
  }
}

static uint16_t text_size_get_num_rows(MenuLayer *menu_layer,
                                       uint16_t section_index,
                                       void *context) {
  return APP_TEXT_SIZE_COUNT;
}

static void text_size_draw_row(GContext *ctx, const Layer *cell_layer,
                               MenuIndex *cell_index, void *context) {
  accessible_menu_draw_row(
      ctx, cell_layer,
      app_settings_text_size_label((AppTextSize)cell_index->row));
}

static void text_size_select_click(MenuLayer *menu_layer,
                                   MenuIndex *cell_index, void *context) {
  if (app_settings_set_text_size((AppTextSize)cell_index->row)) {
    window_stack_remove(s_settings_window, false);
    window_stack_pop(true);
  }
}

static uint16_t noon_reminder_get_num_rows(MenuLayer *menu_layer,
                                           uint16_t section_index,
                                           void *context) {
  return APP_NOON_REMINDER_DURATION_COUNT +
         NOON_REMINDER_OPTION_DURATION_OFFSET;
}

static void noon_reminder_draw_row(GContext *ctx, const Layer *cell_layer,
                                   MenuIndex *cell_index, void *context) {
  const char *label = "Off";
  if (cell_index->row >= NOON_REMINDER_OPTION_DURATION_OFFSET) {
    label = app_settings_noon_reminder_duration_label(
        (AppNoonReminderDuration)(cell_index->row -
                                  NOON_REMINDER_OPTION_DURATION_OFFSET));
  }
  accessible_menu_draw_row(ctx, cell_layer, label);
}

static void noon_reminder_select_click(MenuLayer *menu_layer,
                                       MenuIndex *cell_index, void *context) {
  bool saved = false;
  if (cell_index->row == NOON_REMINDER_OPTION_OFF) {
    saved = noon_reminder_set_enabled(false);
  } else {
    const AppNoonReminderDuration duration =
        (AppNoonReminderDuration)(cell_index->row -
                                  NOON_REMINDER_OPTION_DURATION_OFFSET);
    saved = app_settings_set_noon_reminder_duration(duration) &&
            noon_reminder_set_enabled(true);
  }

  if (saved) {
    window_stack_remove(s_settings_window, false);
    window_stack_pop(true);
  }
}

static MenuLayer *create_menu(Window *window, const char *header,
                              MenuLayerGetNumberOfRowsInSectionsCallback get_rows,
                              MenuLayerDrawRowCallback draw_row,
                              MenuLayerSelectCallback select_click) {
  Layer *window_layer = window_get_root_layer(window);
  MenuLayer *menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(menu_layer, (void *)header, (MenuLayerCallbacks){
      .get_num_rows = get_rows,
      .get_cell_height = accessible_menu_get_cell_height,
      .get_header_height = accessible_menu_get_header_height,
      .draw_row = draw_row,
      .draw_header = accessible_menu_draw_header,
      .select_click = select_click,
  });
  accessible_menu_apply_colors(menu_layer);
  menu_layer_set_click_config_onto_window(menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
  return menu_layer;
}

static void settings_window_load(Window *window) {
  s_settings_menu_layer =
      create_menu(window, "Settings", settings_get_num_rows,
                  settings_draw_row, settings_select_click);
}

static void settings_window_unload(Window *window) {
  menu_layer_destroy(s_settings_menu_layer);
  s_settings_menu_layer = NULL;
}

static void text_size_window_load(Window *window) {
  s_text_size_menu_layer =
      create_menu(window, "Text Size", text_size_get_num_rows,
                  text_size_draw_row, text_size_select_click);
  menu_layer_set_selected_index(
      s_text_size_menu_layer,
      MenuIndex(0, (uint16_t)app_settings_get_text_size()),
      MenuRowAlignCenter, false);
}

static void text_size_window_unload(Window *window) {
  menu_layer_destroy(s_text_size_menu_layer);
  s_text_size_menu_layer = NULL;
}

static void appearance_window_load(Window *window) {
  s_appearance_menu_layer =
      create_menu(window, "Appearance", appearance_get_num_rows,
                  appearance_draw_row, appearance_select_click);
  menu_layer_set_selected_index(
      s_appearance_menu_layer,
      MenuIndex(0, (uint16_t)app_settings_get_appearance()),
      MenuRowAlignCenter, false);
}

static void appearance_window_unload(Window *window) {
  menu_layer_destroy(s_appearance_menu_layer);
  s_appearance_menu_layer = NULL;
}

static void accent_color_window_load(Window *window) {
  s_accent_color_menu_layer =
      create_menu(window, "Accent Color", accent_color_get_num_rows,
                  accent_color_draw_row, accent_color_select_click);
  menu_layer_set_selected_index(
      s_accent_color_menu_layer,
      MenuIndex(0, (uint16_t)app_settings_get_accent_color()),
      MenuRowAlignCenter, false);
}

static void accent_color_window_unload(Window *window) {
  menu_layer_destroy(s_accent_color_menu_layer);
  s_accent_color_menu_layer = NULL;
}

static void noon_reminder_window_load(Window *window) {
  s_noon_reminder_menu_layer =
      create_menu(window, "Noon Reminder", noon_reminder_get_num_rows,
                  noon_reminder_draw_row, noon_reminder_select_click);
  menu_layer_set_selected_index(
      s_noon_reminder_menu_layer,
      MenuIndex(
          0, app_settings_get_noon_reminder_enabled()
                 ? app_settings_get_noon_reminder_duration() +
                       NOON_REMINDER_OPTION_DURATION_OFFSET
                 : NOON_REMINDER_OPTION_OFF),
      MenuRowAlignCenter, false);
}

static void noon_reminder_window_unload(Window *window) {
  menu_layer_destroy(s_noon_reminder_menu_layer);
  s_noon_reminder_menu_layer = NULL;
}

void settings_menu_init(void) {
  s_settings_window = window_create();
  window_set_window_handlers(s_settings_window, (WindowHandlers){
      .load = settings_window_load,
      .unload = settings_window_unload,
  });

  s_text_size_window = window_create();
  window_set_window_handlers(s_text_size_window, (WindowHandlers){
      .load = text_size_window_load,
      .unload = text_size_window_unload,
  });

  s_appearance_window = window_create();
  window_set_window_handlers(s_appearance_window, (WindowHandlers){
      .load = appearance_window_load,
      .unload = appearance_window_unload,
  });

  s_accent_color_window = window_create();
  window_set_window_handlers(s_accent_color_window, (WindowHandlers){
      .load = accent_color_window_load,
      .unload = accent_color_window_unload,
  });

  s_noon_reminder_window = window_create();
  window_set_window_handlers(s_noon_reminder_window, (WindowHandlers){
      .load = noon_reminder_window_load,
      .unload = noon_reminder_window_unload,
  });
}

void settings_menu_deinit(void) {
  window_destroy(s_noon_reminder_window);
  s_noon_reminder_window = NULL;

  window_destroy(s_accent_color_window);
  s_accent_color_window = NULL;

  window_destroy(s_appearance_window);
  s_appearance_window = NULL;

  window_destroy(s_text_size_window);
  s_text_size_window = NULL;

  window_destroy(s_settings_window);
  s_settings_window = NULL;
}

void settings_menu_show(void) {
  window_stack_push(s_settings_window, true);
}
