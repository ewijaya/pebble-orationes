#include "navigation_menu.h"
#include "accessible_menu.h"

static uint16_t count_rows(MenuLayer *layer, uint16_t section, void *context) {
  NavigationMenu *menu = context;
  return menu->count(menu->context);
}
static int16_t row_height(MenuLayer *layer, MenuIndex *index, void *context) {
  NavigationMenu *menu = context;
  return accessible_menu_wrapped_row_height(layer, menu->label(index->row, menu->context));
}
static void draw_row(GContext *ctx, const Layer *cell, MenuIndex *index, void *context) {
  NavigationMenu *menu = context;
  accessible_menu_draw_row(ctx, cell, menu->label(index->row, menu->context));
}
static void draw_header(GContext *ctx, const Layer *cell, uint16_t section, void *context) {
  NavigationMenu *menu = context;
  accessible_menu_draw_header(ctx, cell, section, (void *)menu->title);
}
static void move(NavigationMenu *menu, bool up) {
  const uint16_t count = menu->count(menu->context);
  if (!count) return;
  uint16_t row = menu_layer_get_selected_index(menu->layer).row;
  row = up ? (row ? row - 1 : count - 1) : (row + 1) % count;
  menu_layer_set_selected_index(menu->layer, MenuIndex(0, row), MenuRowAlignCenter, true);
}
static void up(ClickRecognizerRef recognizer, void *context) { move(context, true); }
static void down(ClickRecognizerRef recognizer, void *context) { move(context, false); }
static void select(ClickRecognizerRef recognizer, void *context) {
  NavigationMenu *menu = context;
  const uint16_t row = menu_layer_get_selected_index(menu->layer).row;
  if (row < menu->count(menu->context)) menu->select(row, menu->context);
}
static void clicks(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down);
  window_single_click_subscribe(BUTTON_ID_SELECT, select);
}
static void load(Window *window) {
  NavigationMenu *menu = window_get_user_data(window);
  Layer *root = window_get_root_layer(window);
  menu->layer = menu_layer_create(layer_get_bounds(root));
  menu_layer_set_callbacks(menu->layer, menu, (MenuLayerCallbacks){
    .get_num_rows = count_rows, .get_cell_height = row_height, .draw_row = draw_row,
    .get_header_height = accessible_menu_get_header_height, .draw_header = draw_header,
  });
  accessible_menu_apply_colors(menu->layer);
  window_set_click_config_provider_with_context(window, clicks, menu);
  layer_add_child(root, menu_layer_get_layer(menu->layer));
}
static void unload(Window *window) {
  NavigationMenu *menu = window_get_user_data(window);
  menu_layer_destroy(menu->layer);
  menu->layer = NULL;
}
static void appear(Window *window) { navigation_menu_refresh(window_get_user_data(window)); }
void navigation_menu_init(NavigationMenu *menu, const char *title,
                          uint16_t (*count)(void *), const char *(*label)(uint16_t, void *),
                          void (*on_select)(uint16_t, void *), void *context) {
  *menu = (NavigationMenu){.window = window_create(), .title = title,
    .count = count, .label = label, .select = on_select, .context = context};
  window_set_user_data(menu->window, menu);
  window_set_window_handlers(menu->window, (WindowHandlers){.load = load, .unload = unload, .appear = appear});
}
void navigation_menu_refresh(NavigationMenu *menu) {
  if (!menu->layer) return;
  accessible_menu_apply_colors(menu->layer);
  menu_layer_reload_data(menu->layer);
}
void navigation_menu_hide(NavigationMenu *menu) { window_stack_remove(menu->window, false); }
void navigation_menu_show(NavigationMenu *menu) {
  navigation_menu_hide(menu);
  window_stack_push(menu->window, true);
}
void navigation_menu_deinit(NavigationMenu *menu) { window_destroy(menu->window); menu->window = NULL; }
