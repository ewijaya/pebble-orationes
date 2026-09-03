#include "prayer_screen.h"

#include <pebble.h>

enum {
  HORIZONTAL_MARGIN = 12,
  TITLE_TOP_MARGIN = 8,
  TITLE_HEIGHT = 38,
  BODY_TOP_MARGIN = 8,
  BOTTOM_MARGIN = 18,
  BODY_LAYOUT_HEIGHT = 16000,
};

static Window *s_window;
static ScrollLayer *s_scroll_layer;
static TextLayer *s_title_layer;
static TextLayer *s_body_layer;
static const char *s_title;
static const char *s_text;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  const GRect bounds = layer_get_bounds(window_layer);
  const int16_t text_width = bounds.size.w - (2 * HORIZONTAL_MARGIN);
  const int16_t body_y = TITLE_TOP_MARGIN + TITLE_HEIGHT + BODY_TOP_MARGIN;

  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  s_title_layer = text_layer_create(
      GRect(HORIZONTAL_MARGIN, TITLE_TOP_MARGIN, text_width, TITLE_HEIGHT));
  text_layer_set_background_color(s_title_layer, GColorClear);
  text_layer_set_text_color(s_title_layer, GColorBlack);
  text_layer_set_font(
      s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_text(s_title_layer, s_title);
  scroll_layer_add_child(s_scroll_layer,
                         text_layer_get_layer(s_title_layer));

  s_body_layer = text_layer_create(
      GRect(HORIZONTAL_MARGIN, body_y, text_width, BODY_LAYOUT_HEIGHT));
  text_layer_set_background_color(s_body_layer, GColorClear);
  text_layer_set_text_color(s_body_layer, GColorBlack);
  text_layer_set_font(s_body_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_body_layer, GTextAlignmentLeft);
  text_layer_set_overflow_mode(s_body_layer, GTextOverflowModeWordWrap);
  text_layer_set_text(s_body_layer, s_text);

  const GSize body_size = text_layer_get_content_size(s_body_layer);
  layer_set_frame(text_layer_get_layer(s_body_layer),
                  GRect(HORIZONTAL_MARGIN, body_y, text_width, body_size.h));
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_body_layer));

  const int16_t content_height = body_y + body_size.h + BOTTOM_MARGIN;
  scroll_layer_set_content_size(
      s_scroll_layer,
      GSize(bounds.size.w,
            content_height > bounds.size.h ? content_height : bounds.size.h));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_body_layer);
  s_body_layer = NULL;

  text_layer_destroy(s_title_layer);
  s_title_layer = NULL;

  scroll_layer_destroy(s_scroll_layer);
  s_scroll_layer = NULL;
}

void prayer_screen_init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers){
      .load = window_load,
      .unload = window_unload,
  });
}

void prayer_screen_deinit(void) {
  window_destroy(s_window);
  s_window = NULL;
}

void prayer_screen_show(const char *title, const char *text) {
  s_title = title;
  s_text = text;
  window_stack_push(s_window, true);
}
