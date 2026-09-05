#include "settings_menu.h"

#include <pebble.h>

#include "accessible_menu.h"
#include "app_settings.h"
#include "appearance_picker.h"
#include "main_menu_catalog.h"
#include "noon_reminder.h"
#include "phone_settings.h"

enum {
  SETTINGS_MENU_ITEM_TEXT_SIZE,
  SETTINGS_MENU_ITEM_APPEARANCE,
  SETTINGS_MENU_ITEM_ACCENT_COLOR,
  SETTINGS_MENU_ITEM_NAVIGATION,
  SETTINGS_MENU_ITEM_NOON_REMINDER,
  SETTINGS_MENU_ITEM_MAIN_MENU,
  SETTINGS_MENU_ITEM_REMEMBER_PLACE,
  SETTINGS_MENU_ITEM_COUNT,
};

enum {
  MAIN_MENU_SLOT_ITEM_RESTORE_DEFAULTS = APP_MAIN_MENU_SLOT_COUNT,
  MAIN_MENU_SLOT_ITEM_COUNT,
};

enum {
  NOON_REMINDER_OPTION_OFF,
  NOON_REMINDER_OPTION_DURATION_OFFSET,
};

enum {
  MENU_WRAP_REPEAT_INTERVAL_MS = 100,
};

static Window *s_settings_window;
static MenuLayer *s_settings_menu_layer;
static Window *s_noon_reminder_window;
static MenuLayer *s_noon_reminder_menu_layer;
static Window *s_main_menu_slots_window;
static MenuLayer *s_main_menu_slots_menu_layer;
static Window *s_main_menu_entry_window;
static MenuLayer *s_main_menu_entry_menu_layer;
static uint8_t s_editing_slot_index;
static SettingsShortcutSavedHandler s_shortcut_saved_handler;

static uint16_t settings_get_num_rows(MenuLayer *menu_layer,
                                      uint16_t section_index, void *context) {
  return SETTINGS_MENU_ITEM_COUNT;
}

static const char *const s_settings_labels[] = {
    "Text Size",      "Appearance",
    "Title Accent",   "Navigation Highlight",
    "Noon Reminder",  "Prayer Shortcuts",
    "Remember Place",
};

static const char *settings_value(uint16_t row) {
  const char *value = "";
  switch (row) {
  case SETTINGS_MENU_ITEM_TEXT_SIZE:
    value = app_settings_text_size_label(app_settings_get_text_size());
    break;
  case SETTINGS_MENU_ITEM_APPEARANCE:
    value = app_settings_appearance_label(app_settings_get_appearance());
    break;
  case SETTINGS_MENU_ITEM_ACCENT_COLOR:
    value = app_settings_accent_color_label(app_settings_get_accent_color());
    break;
  case SETTINGS_MENU_ITEM_NAVIGATION:
    value = app_settings_navigation_highlight_label(
        app_settings_get_navigation_highlight());
    break;
  case SETTINGS_MENU_ITEM_NOON_REMINDER:
    value = app_settings_get_noon_reminder_enabled()
                ? app_settings_noon_reminder_duration_label(
                      app_settings_get_noon_reminder_duration())
                : "Off";
    break;
  case SETTINGS_MENU_ITEM_MAIN_MENU:
    value = "Arrange seven slots";
    break;
  case SETTINGS_MENU_ITEM_REMEMBER_PLACE:
    value = app_settings_get_remember_place() ? "On" : "Off";
    break;
  }
  return value;
}
static void settings_draw_row(GContext *ctx, const Layer *cell_layer,
                              MenuIndex *cell_index, void *context) {
  accessible_menu_draw_detail(ctx, cell_layer,
                              s_settings_labels[cell_index->row],
                              settings_value(cell_index->row), UI_SYMBOL_NONE);
}
static int16_t settings_height(MenuLayer *layer, MenuIndex *index,
                               void *context) {
  return accessible_menu_detail_height(layer, s_settings_labels[index->row],
                                       settings_value(index->row),
                                       UI_SYMBOL_NONE);
}
static void appearance_saved(void) {
  window_stack_remove(s_settings_window, true);
}

static void settings_select_click(MenuLayer *menu_layer,
                                  MenuIndex *cell_index, void *context) {
  if (cell_index->row == SETTINGS_MENU_ITEM_TEXT_SIZE) {
    appearance_picker_show(APPEARANCE_PICKER_SIZE, appearance_saved);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_APPEARANCE) {
    appearance_picker_show(APPEARANCE_PICKER_APPEARANCE, appearance_saved);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_ACCENT_COLOR) {
    appearance_picker_show(APPEARANCE_PICKER_ACCENT, appearance_saved);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_NAVIGATION) {
    appearance_picker_show(APPEARANCE_PICKER_NAVIGATION, appearance_saved);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_NOON_REMINDER) {
    window_stack_push(s_noon_reminder_window, true);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_MAIN_MENU) {
    window_stack_push(s_main_menu_slots_window, true);
  } else if (cell_index->row == SETTINGS_MENU_ITEM_REMEMBER_PLACE) {
    if (app_settings_set_remember_place(!app_settings_get_remember_place())) {
      phone_settings_send_current();
    }
  }
}

static uint16_t main_menu_slots_get_num_rows(
    MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return MAIN_MENU_SLOT_ITEM_COUNT;
}

static int16_t main_menu_slots_get_cell_height(MenuLayer *menu_layer,
                                               MenuIndex *cell_index,
                                               void *context) {
  if (cell_index->row < APP_MAIN_MENU_SLOT_COUNT) {
    const MainMenuEntryId entry_id =
        app_settings_get_main_menu_slot((uint8_t)cell_index->row);
    {
      char label[128];
      snprintf(label, sizeof(label), "%u. %s", (unsigned int)cell_index->row + 1,
               main_menu_catalog_get(entry_id)->name);
      return accessible_menu_wrapped_row_height(menu_layer, label);
    }
  }
  return ACCESSIBLE_MENU_ROW_HEIGHT;
}

static void main_menu_slots_draw_row(GContext *ctx,
                                     const Layer *cell_layer,
                                     MenuIndex *cell_index,
                                     void *context) {
  if (cell_index->row == MAIN_MENU_SLOT_ITEM_RESTORE_DEFAULTS) {
    accessible_menu_draw_row(ctx, cell_layer, "Restore Defaults");
    return;
  }

  const MainMenuEntry *entry = main_menu_catalog_get(
      app_settings_get_main_menu_slot((uint8_t)cell_index->row));
  if (entry) {
    char label[128];
    snprintf(label, sizeof(label), "%u. %s",
             (unsigned int)cell_index->row + 1, entry->name);
    accessible_menu_draw_row(ctx, cell_layer, label);
  }
}

static void main_menu_slots_select_click(MenuLayer *menu_layer,
                                         MenuIndex *cell_index,
                                         void *context) {
  if (cell_index->row == MAIN_MENU_SLOT_ITEM_RESTORE_DEFAULTS) {
    if (app_settings_restore_main_menu_defaults()) {
      menu_layer_reload_data(menu_layer);
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      phone_settings_send_current();
    }
    return;
  }

  s_editing_slot_index = (uint8_t)cell_index->row;
  window_stack_push(s_main_menu_entry_window, true);
}

static void move_menu_selection_with_wrap(MenuLayer *menu_layer,
                                          uint16_t row_count, bool up) {
  if (!menu_layer || row_count == 0) {
    return;
  }

  const MenuIndex selected = menu_layer_get_selected_index(menu_layer);
  const bool should_wrap =
      (up && selected.row == 0) ||
      (!up && selected.row == row_count - 1);
  if (should_wrap) {
    menu_layer_set_selected_index(
        menu_layer, MenuIndex(0, up ? row_count - 1 : 0),
        up ? MenuRowAlignBottom : MenuRowAlignTop, true);
    return;
  }

  menu_layer_set_selected_next(menu_layer, up, MenuRowAlignCenter, true);
}

static void main_menu_slots_up_click_handler(ClickRecognizerRef recognizer,
                                             void *context) {
  (void)recognizer;
  (void)context;
  move_menu_selection_with_wrap(s_main_menu_slots_menu_layer,
                                MAIN_MENU_SLOT_ITEM_COUNT, true);
}

static void main_menu_slots_down_click_handler(ClickRecognizerRef recognizer,
                                               void *context) {
  (void)recognizer;
  (void)context;
  move_menu_selection_with_wrap(s_main_menu_slots_menu_layer,
                                MAIN_MENU_SLOT_ITEM_COUNT, false);
}

static void main_menu_slots_select_click_handler(
    ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;
  MenuIndex selected =
      menu_layer_get_selected_index(s_main_menu_slots_menu_layer);
  main_menu_slots_select_click(s_main_menu_slots_menu_layer, &selected, NULL);
}

static void main_menu_slots_click_config_provider(void *context) {
  (void)context;
  window_single_repeating_click_subscribe(
      BUTTON_ID_UP, MENU_WRAP_REPEAT_INTERVAL_MS,
      main_menu_slots_up_click_handler);
  window_single_repeating_click_subscribe(
      BUTTON_ID_DOWN, MENU_WRAP_REPEAT_INTERVAL_MS,
      main_menu_slots_down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT,
                                main_menu_slots_select_click_handler);
}

static uint16_t main_menu_entry_get_num_rows(
    MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return main_menu_catalog_count();
}

static void main_menu_entry_draw_row(GContext *ctx,
                                     const Layer *cell_layer,
                                     MenuIndex *cell_index,
                                     void *context) {
  const MainMenuEntry *entry =
      main_menu_catalog_get((MainMenuEntryId)cell_index->row);
  if (entry) {
    if (main_menu_catalog_is_card((MainMenuEntryId)cell_index->row)) {
      char label[128];
      snprintf(label, sizeof(label), "Card: %s", entry->name);
      accessible_menu_draw_row(ctx, cell_layer, label);
    } else {
      accessible_menu_draw_row(ctx, cell_layer, entry->name);
    }
  }
}

static int16_t main_menu_entry_get_cell_height(MenuLayer *menu_layer,
                                               MenuIndex *cell_index,
                                               void *context) {
  const MainMenuEntryId entry_id = (MainMenuEntryId)cell_index->row;
  if (main_menu_catalog_is_card(entry_id)) {
    char label[128];
    snprintf(label, sizeof(label), "Card: %s",
             main_menu_catalog_get(entry_id)->name);
    return accessible_menu_wrapped_row_height(menu_layer, label);
  }
  return accessible_menu_wrapped_row_height(
      menu_layer, main_menu_catalog_get(entry_id)->name);
}

static void main_menu_entry_select_click(MenuLayer *menu_layer,
                                         MenuIndex *cell_index,
                                         void *context) {
  if (app_settings_set_main_menu_slot(
          s_editing_slot_index, (MainMenuEntryId)cell_index->row)) {
    phone_settings_send_current();
    // Remove the intermediate screens before revealing the main menu once.
    // BACK without a selection still follows the normal window stack.
    window_stack_remove(s_main_menu_slots_window, false);
    window_stack_remove(s_settings_window, false);
    window_stack_pop(true);
    if (s_shortcut_saved_handler)
      s_shortcut_saved_handler(s_editing_slot_index);
  }
}

static void main_menu_entry_move_selection(bool up) {
  move_menu_selection_with_wrap(s_main_menu_entry_menu_layer,
                                main_menu_catalog_count(), up);
}

static void main_menu_entry_up_click_handler(ClickRecognizerRef recognizer,
                                             void *context) {
  (void)recognizer;
  (void)context;
  main_menu_entry_move_selection(true);
}

static void main_menu_entry_down_click_handler(ClickRecognizerRef recognizer,
                                               void *context) {
  (void)recognizer;
  (void)context;
  main_menu_entry_move_selection(false);
}

static void main_menu_entry_select_click_handler(
    ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;
  MenuIndex selected =
      menu_layer_get_selected_index(s_main_menu_entry_menu_layer);
  main_menu_entry_select_click(s_main_menu_entry_menu_layer, &selected, NULL);
}

static void main_menu_entry_click_config_provider(void *context) {
  (void)context;
  window_single_repeating_click_subscribe(
      BUTTON_ID_UP, MENU_WRAP_REPEAT_INTERVAL_MS,
      main_menu_entry_up_click_handler);
  window_single_repeating_click_subscribe(
      BUTTON_ID_DOWN, MENU_WRAP_REPEAT_INTERVAL_MS,
      main_menu_entry_down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT,
                                main_menu_entry_select_click_handler);
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
    AppSettings updated = app_settings_get();
    updated.noon_reminder_duration = duration;
    updated.noon_reminder_enabled = true;
    saved = noon_reminder_apply_settings(&updated);
  }

  if (saved) {
    phone_settings_send_current();
    window_stack_remove(s_settings_window, false);
    window_stack_pop(true);
  }
}

static MenuLayer *create_menu(Window *window, const char *header,
                              MenuLayerGetNumberOfRowsInSectionsCallback get_rows,
                              MenuLayerDrawRowCallback draw_row,
                              MenuLayerSelectCallback select_click,
                              MenuLayerGetCellHeightCallback get_height) {
  Layer *window_layer = window_get_root_layer(window);
  MenuLayer *menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(menu_layer, (void *)header, (MenuLayerCallbacks){
      .get_num_rows = get_rows,
      .get_cell_height = get_height ? get_height : accessible_menu_get_cell_height,
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
      create_menu(window, "Settings", settings_get_num_rows, settings_draw_row,
                  settings_select_click, settings_height);
}

static void settings_window_unload(Window *window) {
  menu_layer_destroy(s_settings_menu_layer);
  s_settings_menu_layer = NULL;
}

static void noon_reminder_window_load(Window *window) {
  s_noon_reminder_menu_layer =
      create_menu(window, "Noon Reminder", noon_reminder_get_num_rows,
                  noon_reminder_draw_row, noon_reminder_select_click, NULL);
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

static void main_menu_entry_window_load(Window *window) {
  s_main_menu_entry_menu_layer = create_menu(
      window, "Choose Shortcut", main_menu_entry_get_num_rows,
      main_menu_entry_draw_row, main_menu_entry_select_click,
      main_menu_entry_get_cell_height);
  window_set_click_config_provider(window,
                                   main_menu_entry_click_config_provider);
  menu_layer_set_selected_index(
      s_main_menu_entry_menu_layer,
      MenuIndex(0, app_settings_get_main_menu_slot(s_editing_slot_index)),
      MenuRowAlignCenter, false);
}

static void main_menu_entry_window_unload(Window *window) {
  menu_layer_destroy(s_main_menu_entry_menu_layer);
  s_main_menu_entry_menu_layer = NULL;
}

static void main_menu_slots_window_load(Window *window) {
  s_main_menu_slots_menu_layer = create_menu(
      window, "Shortcuts", main_menu_slots_get_num_rows,
      main_menu_slots_draw_row, main_menu_slots_select_click,
      main_menu_slots_get_cell_height);
  window_set_click_config_provider(window,
                                   main_menu_slots_click_config_provider);
}

static void main_menu_slots_window_unload(Window *window) {
  menu_layer_destroy(s_main_menu_slots_menu_layer);
  s_main_menu_slots_menu_layer = NULL;
}

void settings_menu_init(SettingsShortcutSavedHandler shortcut_saved_handler) {
  appearance_picker_init();
  s_shortcut_saved_handler = shortcut_saved_handler;
  s_settings_window = window_create();
  window_set_window_handlers(s_settings_window, (WindowHandlers){
      .load = settings_window_load,
      .unload = settings_window_unload,
  });

  s_noon_reminder_window = window_create();
  window_set_window_handlers(s_noon_reminder_window, (WindowHandlers){
      .load = noon_reminder_window_load,
      .unload = noon_reminder_window_unload,
  });

  s_main_menu_entry_window = window_create();
  window_set_window_handlers(s_main_menu_entry_window, (WindowHandlers){
      .load = main_menu_entry_window_load,
      .unload = main_menu_entry_window_unload,
  });

  s_main_menu_slots_window = window_create();
  window_set_window_handlers(s_main_menu_slots_window, (WindowHandlers){
      .load = main_menu_slots_window_load,
      .unload = main_menu_slots_window_unload,
  });
}

void settings_menu_deinit(void) {
  appearance_picker_deinit();
  s_shortcut_saved_handler = NULL;
  window_destroy(s_main_menu_entry_window);
  s_main_menu_entry_window = NULL;

  window_destroy(s_main_menu_slots_window);
  s_main_menu_slots_window = NULL;

  window_destroy(s_noon_reminder_window);
  s_noon_reminder_window = NULL;




  window_destroy(s_settings_window);
  s_settings_window = NULL;
}

void settings_menu_show(void) {
  window_stack_push(s_settings_window, true);
}

void settings_menu_refresh(void) {
  appearance_picker_refresh();
  MenuLayer *menu_layers[] = {
      s_settings_menu_layer,
      s_noon_reminder_menu_layer,
      s_main_menu_slots_menu_layer,
      s_main_menu_entry_menu_layer,
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
