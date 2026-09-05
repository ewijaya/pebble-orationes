#include "ui_notice.h"
#include "app_theme.h"
#include <pebble.h>
#include <stdio.h>
static Window *s_window;
static TextLayer *s_title_layer, *s_detail_layer;
static AppTimer *s_timer;
static char s_title[40], s_detail[128];
static void dismiss(void *context) {
  s_timer = NULL;
  window_stack_remove(s_window, true);
}
static void click(ClickRecognizerRef recognizer, void *context) {
  window_stack_remove(s_window, true);
}
static void clicks(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, click);
}
static void unload(Window *window) {
  if (s_timer) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_detail_layer);
  s_title_layer = NULL;
  s_detail_layer = NULL;
}
static void load(Window *window) {
  window_set_background_color(window, app_theme_background_color());
  s_title_layer = text_layer_create(GRect(0, 32, 200, 42));
  text_layer_set_font(s_title_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_background_color(s_title_layer,
                                  app_theme_title_background_color());
  text_layer_set_text_color(s_title_layer, app_theme_title_foreground_color());
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_text(s_title_layer, s_title);
  s_detail_layer = text_layer_create(GRect(8, 86, 184, 138));
  text_layer_set_background_color(s_detail_layer, GColorClear);
  text_layer_set_text_color(s_detail_layer, app_theme_foreground_color());
  text_layer_set_font(s_detail_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_detail_layer, GTextAlignmentCenter);
  text_layer_set_text(s_detail_layer, s_detail);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_title_layer));
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_detail_layer));
  window_set_click_config_provider(window, clicks);
}
void ui_notice_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window,
                             (WindowHandlers){.load = load, .unload = unload});
}
void ui_notice_show(const char *title, const char *detail) {
  window_stack_remove(s_window, false);
  snprintf(s_title, sizeof(s_title), "%s", title);
  snprintf(s_detail, sizeof(s_detail), "%s", detail);
  window_stack_push(s_window, true);
  s_timer = app_timer_register(1600, dismiss, NULL);
}
void ui_notice_deinit(void) {
  if (s_timer) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
  window_destroy(s_window);
  s_window = NULL;
}
