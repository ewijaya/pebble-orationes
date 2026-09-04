#include "prayer_screen.h"

#include <pebble.h>

#include "app_settings.h"
#include "app_theme.h"

enum {
  HORIZONTAL_MARGIN = 8,
  TITLE_TOP_MARGIN = 0,
  TITLE_HEIGHT = 44,
  BODY_TOP_MARGIN = 8,
  BOTTOM_MARGIN = 18,
  BODY_LAYOUT_HEIGHT = 30000,
  FAST_SCROLL_PERCENT = 80,
  FAST_SCROLL_LONG_CLICK_MS = 500,
  FAST_SCROLL_REPEAT_MS = 325,
};

static Window *s_window;
static ScrollLayer *s_scroll_layer;
static TextLayer *s_title_layer;
static TextLayer *s_body_layer;
static const char *s_title;
static const char *s_text;
static AppTimer *s_fast_scroll_timer;
static int8_t s_fast_scroll_direction;
static GFont s_custom_body_font;

static GFont get_body_font(void) {
  if (app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE) {
    s_custom_body_font = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_34));
    if (s_custom_body_font) {
      return s_custom_body_font;
    }
  }

  return fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
}

static void stop_fast_scrolling(void) {
  s_fast_scroll_direction = 0;

  if (s_fast_scroll_timer) {
    app_timer_cancel(s_fast_scroll_timer);
    s_fast_scroll_timer = NULL;
  }
}

static bool fast_scroll_by_screen(void) {
  if (!s_scroll_layer || s_fast_scroll_direction == 0) {
    return false;
  }

  const int16_t viewport_height =
      layer_get_bounds(scroll_layer_get_layer(s_scroll_layer)).size.h;
  const int16_t content_height =
      scroll_layer_get_content_size(s_scroll_layer).h;
  const int16_t minimum_offset =
      content_height > viewport_height ? viewport_height - content_height : 0;
  const int16_t current_offset =
      scroll_layer_get_content_offset(s_scroll_layer).y;
  const int16_t jump =
      (viewport_height * FAST_SCROLL_PERCENT) / 100;

  int32_t target_offset =
      current_offset + (s_fast_scroll_direction * jump);
  if (target_offset > 0) {
    target_offset = 0;
  } else if (target_offset < minimum_offset) {
    target_offset = minimum_offset;
  }

  if (target_offset == current_offset) {
    return false;
  }

  scroll_layer_set_content_offset(
      s_scroll_layer, GPoint(0, (int16_t)target_offset), false);
  return true;
}

static void fast_scroll_timer_handler(void *context) {
  (void)context;
  s_fast_scroll_timer = NULL;

  if (fast_scroll_by_screen()) {
    s_fast_scroll_timer = app_timer_register(
        FAST_SCROLL_REPEAT_MS, fast_scroll_timer_handler, NULL);
  } else {
    s_fast_scroll_direction = 0;
  }
}

static void start_fast_scrolling(int8_t direction) {
  stop_fast_scrolling();
  s_fast_scroll_direction = direction;

  if (fast_scroll_by_screen()) {
    s_fast_scroll_timer = app_timer_register(
        FAST_SCROLL_REPEAT_MS, fast_scroll_timer_handler, NULL);
  } else {
    s_fast_scroll_direction = 0;
  }
}

static void fast_scroll_up_handler(ClickRecognizerRef recognizer,
                                   void *context) {
  (void)recognizer;
  (void)context;
  start_fast_scrolling(1);
}

static void fast_scroll_down_handler(ClickRecognizerRef recognizer,
                                     void *context) {
  (void)recognizer;
  (void)context;
  start_fast_scrolling(-1);
}

static void fast_scroll_release_handler(ClickRecognizerRef recognizer,
                                        void *context) {
  (void)recognizer;
  (void)context;
  stop_fast_scrolling();
}

static void exit_app_multi_click_handler(ClickRecognizerRef recognizer,
                                         void *context) {
  (void)recognizer;
  (void)context;
  stop_fast_scrolling();
  window_stack_pop_all(false);
}

static void prayer_click_config_provider(void *context) {
  (void)context;

  window_single_click_subscribe(BUTTON_ID_UP,
                                scroll_layer_scroll_up_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, FAST_SCROLL_LONG_CLICK_MS,
                              fast_scroll_up_handler,
                              fast_scroll_release_handler);
  window_set_click_context(BUTTON_ID_UP, s_scroll_layer);

  window_single_click_subscribe(BUTTON_ID_DOWN,
                                scroll_layer_scroll_down_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, FAST_SCROLL_LONG_CLICK_MS,
                              fast_scroll_down_handler,
                              fast_scroll_release_handler);
  window_set_click_context(BUTTON_ID_DOWN, s_scroll_layer);

  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 2, 0, true,
                               exit_app_multi_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  const GRect bounds = layer_get_bounds(window_layer);
  const int16_t text_width = bounds.size.w - (2 * HORIZONTAL_MARGIN);
  const int16_t body_y = TITLE_TOP_MARGIN + TITLE_HEIGHT + BODY_TOP_MARGIN;

  window_set_background_color(window, app_theme_background_color());

  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_callbacks(s_scroll_layer, (ScrollLayerCallbacks){
      .click_config_provider = prayer_click_config_provider,
  });
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  s_title_layer = text_layer_create(
      GRect(0, TITLE_TOP_MARGIN, bounds.size.w, TITLE_HEIGHT));
  text_layer_set_background_color(s_title_layer,
                                  app_theme_title_background_color());
  text_layer_set_text_color(s_title_layer,
                            app_theme_title_foreground_color());
  text_layer_set_font(
      s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_text(s_title_layer, s_title);
  scroll_layer_add_child(s_scroll_layer,
                         text_layer_get_layer(s_title_layer));

  s_body_layer = text_layer_create(
      GRect(HORIZONTAL_MARGIN, body_y, text_width, BODY_LAYOUT_HEIGHT));
  text_layer_set_background_color(s_body_layer, GColorClear);
  text_layer_set_text_color(s_body_layer, app_theme_foreground_color());
  text_layer_set_font(s_body_layer, get_body_font());
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
  stop_fast_scrolling();

  text_layer_destroy(s_body_layer);
  s_body_layer = NULL;

  if (s_custom_body_font) {
    fonts_unload_custom_font(s_custom_body_font);
    s_custom_body_font = NULL;
  }

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
  if (window_stack_contains_window(s_window)) {
    window_stack_remove(s_window, false);
  }

  s_title = title;
  s_text = text;
  window_stack_push(s_window, true);
}
