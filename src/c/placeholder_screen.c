#include "placeholder_screen.h"

#include <pebble.h>

static Window *s_window;
static TextLayer *s_name_layer;
static TextLayer *s_hint_layer;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_name_layer = text_layer_create(GRect(12, 44, bounds.size.w - 24, 80));
  text_layer_set_background_color(s_name_layer, GColorClear);
  text_layer_set_text_color(s_name_layer, GColorBlack);
  text_layer_set_font(s_name_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_name_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_name_layer));

  s_hint_layer =
      text_layer_create(GRect(12, bounds.size.h - 54, bounds.size.w - 24, 30));
  text_layer_set_background_color(s_hint_layer, GColorClear);
  text_layer_set_text_color(s_hint_layer, GColorDarkGray);
  text_layer_set_font(s_hint_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_hint_layer, GTextAlignmentCenter);
  text_layer_set_text(s_hint_layer, "Press Back to return");
  layer_add_child(window_layer, text_layer_get_layer(s_hint_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_hint_layer);
  s_hint_layer = NULL;

  text_layer_destroy(s_name_layer);
  s_name_layer = NULL;
}

void placeholder_screen_init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers){
      .load = window_load,
      .unload = window_unload,
  });
}

void placeholder_screen_deinit(void) {
  window_destroy(s_window);
  s_window = NULL;
}

void placeholder_screen_show(const char *prayer_name) {
  window_stack_push(s_window, true);
  text_layer_set_text(s_name_layer, prayer_name);
}
