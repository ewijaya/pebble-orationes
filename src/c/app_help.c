#include "app_help.h"
#include "accessible_menu.h"
#include "app_settings.h"
#include "app_theme.h"
#include "navigation_menu.h"

static const struct { const char *title, *text; } s_topics[] = {
  {"Navigation", "Up/Down: move.\nSelect: open.\nBack: return.\n\nA right arrow opens another menu.\n\nIn Help, Up/Down scroll; Select or Back returns to topics."},
  {"Reading", "Up/Down: scroll. Hold to keep scrolling.\n\nSelect: Reading Options, with Start again and section jumps where available.\n\nBack: leave prayer.\nDouble-click Select in a prayer: exit to watchface."},
  {"Resume Prayers", "Turn on Remember Place to save progress. Continue opens your last prayer.\n\nContinue First puts it above shortcuts. Turning Remember Place off clears saved places."},
  {"Pin Shortcuts", "In All Prayers, open a category and highlight a prayer.\n\nHold Select for one second, then release. Choose Open or Pin to main menu, then a slot.\n\nThis gesture works in All Prayers lists."},
  {"Menu Layout", "Compact Menus fits more choices with the same text size. Turn it off for more space.\n\nLong names still wrap. Text Size changes prayer reading text."},
  {"Phone Settings", "Open Orationes on your watch to sync.\n\nIn MyApps, open Settings and tap Save Settings after changes. Close and reopen phone Settings to see watch changes.\n\nRestore Defaults resets settings and shortcuts."},
  {"Holy Rosary", "Today's Mysteries uses the current weekday.\n\nAll Mysteries lets you choose any set. Select a mystery to read; Back returns to the list."},
  {"Noon Reminder", "Enable Noon Reminder in Settings. Choose how long it stays visible.\n\nAt noon, Select opens the prayer. Back dismisses the reminder."},
};
static NavigationMenu s_topics_menu;
static Window *s_detail_window;
static Layer *s_header;
static ScrollLayer *s_scroll;
static TextLayer *s_body;
static uint16_t s_topic;

static uint16_t count(void *context) { return ARRAY_LENGTH(s_topics); }
static const char *label(uint16_t row, void *context) { return s_topics[row].title; }
static void header_draw(Layer *layer, GContext *ctx) {
  accessible_menu_draw_header(ctx, layer, 0, (void *)s_topics[s_topic].title);
}
static void close_detail(ClickRecognizerRef recognizer, void *context) {
  window_stack_remove(s_detail_window, true);
}
static void detail_clicks(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, close_detail);
}
static void layout_detail(void) {
  if (!s_body) return;
  const GRect bounds = layer_get_bounds(window_get_root_layer(s_detail_window));
  const GFont title_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  const GFont body_font = fonts_get_system_font(app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE
                                               ? FONT_KEY_GOTHIC_28_BOLD : FONT_KEY_GOTHIC_24_BOLD);
  int16_t header = graphics_text_layout_get_content_size(s_topics[s_topic].title, title_font,
      GRect(0, 0, bounds.size.w - 16, 300), GTextOverflowModeWordWrap, GTextAlignmentLeft).h + 4;
  if (header < ACCESSIBLE_MENU_HEADER_HEIGHT) header = ACCESSIBLE_MENU_HEADER_HEIGHT;
  const int16_t width = bounds.size.w - 20;
  const int16_t height = graphics_text_layout_get_content_size(s_topics[s_topic].text, body_font,
      GRect(0, 0, width, 3000), GTextOverflowModeWordWrap, GTextAlignmentLeft).h + 12;
  window_set_background_color(s_detail_window, app_theme_background_color());
  layer_set_frame(s_header, GRect(0, 0, bounds.size.w, header));
  layer_mark_dirty(s_header);
  scroll_layer_set_frame(s_scroll, GRect(0, header, bounds.size.w, bounds.size.h - header));
  layer_set_frame(text_layer_get_layer(s_body), GRect(8, 0, width, height));
  text_layer_set_font(s_body, body_font);
  text_layer_set_text_color(s_body, app_theme_foreground_color());
  text_layer_set_text(s_body, s_topics[s_topic].text);
  scroll_layer_set_content_size(s_scroll, GSize(bounds.size.w, height));
}
static void detail_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  s_header = layer_create(GRectZero);
  layer_set_update_proc(s_header, header_draw);
  s_scroll = scroll_layer_create(layer_get_bounds(root));
  s_body = text_layer_create(GRectZero);
  text_layer_set_background_color(s_body, GColorClear);
  scroll_layer_add_child(s_scroll, text_layer_get_layer(s_body));
  layer_add_child(root, scroll_layer_get_layer(s_scroll));
  layer_add_child(root, s_header);
  scroll_layer_set_callbacks(s_scroll, (ScrollLayerCallbacks){.click_config_provider = detail_clicks});
  scroll_layer_set_click_config_onto_window(s_scroll, window);
  layout_detail();
}
static void detail_unload(Window *window) {
  text_layer_destroy(s_body); s_body = NULL;
  scroll_layer_destroy(s_scroll); s_scroll = NULL;
  layer_destroy(s_header); s_header = NULL;
}
static void select_topic(uint16_t row, void *context) {
  s_topic = row;
  window_stack_push(s_detail_window, true);
}
void app_help_init(void) {
  navigation_menu_init(&s_topics_menu, "Help", count, label, select_topic, NULL);
  s_detail_window = window_create();
  window_set_window_handlers(s_detail_window, (WindowHandlers){.load = detail_load, .unload = detail_unload});
}
void app_help_show(void) { navigation_menu_show(&s_topics_menu); }
void app_help_refresh(void) {
  navigation_menu_refresh(&s_topics_menu);
  layout_detail();
}
void app_help_deinit(void) {
  window_destroy(s_detail_window);
  navigation_menu_deinit(&s_topics_menu);
}
