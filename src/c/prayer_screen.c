#include "prayer_screen.h"

#include <pebble.h>
#include <limits.h>
#include <string.h>
#include "reading_position.h"

#include "app_settings.h"
#include "app_theme.h"

enum {
  HORIZONTAL_MARGIN = 8,
  TITLE_TOP_MARGIN = 0,
  TITLE_MIN_HEIGHT = 44,
  TITLE_LAYOUT_HEIGHT = 300,
  BODY_TOP_MARGIN = 8,
  BOTTOM_MARGIN = 18,
  BODY_LAYOUT_HEIGHT = 30000,
  PARAGRAPH_GAP = 14,
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
static int16_t s_title_height;
static AppTimer *s_fast_scroll_timer;
static int8_t s_fast_scroll_direction;
static GFont s_custom_body_font;
static GFont s_italic_body_font;
static GFont s_rendered_body_font;
static Layer *s_styled_body_layer;
static const PrayerParagraph *s_paragraphs;
static uint16_t s_paragraph_count;
static int16_t *s_paragraph_heights;
static int32_t s_styled_content_height;
static MainMenuEntryId s_entry_id;
static bool s_resume;
static bool s_rebuilding;
static bool capture_position(ReadingPosition *position);
static void restore_position(const ReadingPosition *position);
static void save_position(void);
static void restore_saved_position(void);

// ScrollLayer uses int16 coordinates. Long styled documents keep int32 layout
// positions and map their range onto the native range, drawing in the viewport.
static int32_t styled_scroll_range(void) {
  const int16_t viewport =
      layer_get_bounds(scroll_layer_get_layer(s_scroll_layer)).size.h;
  return s_styled_content_height > viewport
             ? s_styled_content_height - viewport : 0;
}

static int16_t native_scroll_range(void) {
  const int16_t viewport =
      layer_get_bounds(scroll_layer_get_layer(s_scroll_layer)).size.h;
  const int16_t content = scroll_layer_get_content_size(s_scroll_layer).h;
  return content > viewport ? content - viewport : 0;
}

static int32_t styled_scroll_offset(void) {
  const int16_t range = native_scroll_range();
  return range ? (int64_t)scroll_layer_get_content_offset(s_scroll_layer).y *
                     styled_scroll_range() / range : 0;
}

static void styled_offset_changed(ScrollLayer *scroll_layer, void *context) {
  (void)context;
  if (!s_styled_body_layer) {
    return;
  }
  const int16_t native_offset = scroll_layer_get_content_offset(scroll_layer).y;
  const int32_t logical_offset = styled_scroll_offset();
  GRect frame = layer_get_frame(s_styled_body_layer);
  frame.origin.y = -native_offset;
  layer_set_frame(s_styled_body_layer, frame);
  Layer *title = text_layer_get_layer(s_title_layer);
  layer_set_hidden(title, logical_offset <= -s_title_height);
  if (logical_offset > -s_title_height) {
    GRect title_frame = layer_get_frame(title);
    title_frame.origin.y = (int16_t)(logical_offset - native_offset);
    layer_set_frame(title, title_frame);
  }
  layer_mark_dirty(s_styled_body_layer);
}

static GFont get_italic_font(void) {
  if (!s_italic_body_font) {
    const uint32_t resource =
        app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE
            ? RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_OBLIQUE_34
            : RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_OBLIQUE_28;
    s_italic_body_font = fonts_load_custom_font(resource_get_handle(resource));
  }
  return s_italic_body_font;
}

static GFont get_body_font(void) {
  if (app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE) {
    if (!s_custom_body_font) {
      s_custom_body_font = fonts_load_custom_font(
          resource_get_handle(
              RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_34));
    }
    if (s_custom_body_font) {
      return s_custom_body_font;
    }
  }

  return fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
}

static GFont paragraph_font(const PrayerParagraph *paragraph) {
  switch (paragraph->style) {
    case PRAYER_PARAGRAPH_LATIN:
      return get_italic_font();
    case PRAYER_PARAGRAPH_TRANSLATION:
      return fonts_get_system_font(
          app_settings_get_text_size() == APP_TEXT_SIZE_EXTRA_LARGE
              ? FONT_KEY_GOTHIC_28_BOLD : FONT_KEY_GOTHIC_24_BOLD);
    case PRAYER_PARAGRAPH_REFERENCE:
      return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    case PRAYER_PARAGRAPH_NOTE:
      return fonts_get_system_font(FONT_KEY_GOTHIC_14);
    default:
      return get_body_font();
  }
}

// Measure once on open/size change. Scrolling only draws visible paragraphs.
static int32_t measure_styled_body(int16_t width) {
  if (!s_paragraph_heights) {
    return 0;
  }
  // Load the italic face first when needed; cards without Latin don't need it.
  for (uint16_t i = 0; i < s_paragraph_count; ++i) {
    if (s_paragraphs[i].style == PRAYER_PARAGRAPH_LATIN && !get_italic_font()) {
      return 0;
    }
  }
  int32_t total = 0;
  for (uint16_t i = 0; i < s_paragraph_count; ++i) {
    const PrayerParagraph *paragraph = &s_paragraphs[i];
    const GSize size = graphics_text_layout_get_content_size(
        paragraph->text, paragraph_font(paragraph),
        GRect(0, 0, width, BODY_LAYOUT_HEIGHT), GTextOverflowModeWordWrap,
        GTextAlignmentLeft);
    s_paragraph_heights[i] = size.h;
    total += size.h;
    if (paragraph->space_after && i + 1 < s_paragraph_count) {
      total += PARAGRAPH_GAP;
    }
  }
  return total;
}

static void styled_body_update(Layer *layer, GContext *ctx) {
  const GRect bounds = layer_get_bounds(layer);
  const int32_t body_y = TITLE_TOP_MARGIN + s_title_height + BODY_TOP_MARGIN;
  const int32_t offset = styled_scroll_offset();
  const int16_t viewport_height =
      layer_get_bounds(scroll_layer_get_layer(s_scroll_layer)).size.h;
  graphics_context_set_text_color(ctx, app_theme_foreground_color());
  int32_t y = 0;
  for (uint16_t i = 0; i < s_paragraph_count; ++i) {
    const PrayerParagraph *paragraph = &s_paragraphs[i];
    const int16_t height = s_paragraph_heights[i];
    const int32_t screen_y = y + body_y + offset;
    if (screen_y > viewport_height) {
      break;
    }
    if (screen_y + height >= 0) {
      graphics_draw_text(ctx, paragraph->text,
                         paragraph_font(paragraph),
                         GRect(0, (int16_t)screen_y, bounds.size.w, height),
                         GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    }
    y += height + (paragraph->space_after ? PARAGRAPH_GAP : 0);
  }
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
  int16_t jump =
      (viewport_height * FAST_SCROLL_PERCENT) / 100;
  if (s_styled_body_layer && styled_scroll_range() > native_scroll_range()) {
    jump = (int64_t)jump * native_scroll_range() / styled_scroll_range();
    if (jump < 1) {
      jump = 1;
    }
  }

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

  window_set_background_color(window, app_theme_background_color());

  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_callbacks(s_scroll_layer, (ScrollLayerCallbacks){
      .click_config_provider = prayer_click_config_provider,
      .content_offset_changed_handler = styled_offset_changed,
  });
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  s_title_layer = text_layer_create(
      GRect(0, TITLE_TOP_MARGIN, bounds.size.w, TITLE_LAYOUT_HEIGHT));
  text_layer_set_background_color(s_title_layer,
                                  app_theme_title_background_color());
  text_layer_set_text_color(s_title_layer,
                            app_theme_title_foreground_color());
  text_layer_set_font(
      s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_text(s_title_layer, s_title);
  const GSize title_size = text_layer_get_content_size(s_title_layer);
  s_title_height = title_size.h > TITLE_MIN_HEIGHT
                       ? title_size.h
                       : TITLE_MIN_HEIGHT;
  layer_set_frame(text_layer_get_layer(s_title_layer),
                  GRect(0, TITLE_TOP_MARGIN, bounds.size.w,
                        s_title_height));
  scroll_layer_add_child(s_scroll_layer,
                         text_layer_get_layer(s_title_layer));

  const int16_t body_y =
      TITLE_TOP_MARGIN + s_title_height + BODY_TOP_MARGIN;
  if (s_paragraphs) {
    s_paragraph_heights = calloc(s_paragraph_count, sizeof(int16_t));
    const int32_t height = measure_styled_body(text_width);
    if (height > 0) {
      s_styled_body_layer = layer_create(
          GRect(HORIZONTAL_MARGIN, 0, text_width, bounds.size.h));
    }
    if (s_styled_body_layer) {
      layer_set_update_proc(s_styled_body_layer, styled_body_update);
      scroll_layer_add_child(s_scroll_layer, s_styled_body_layer);
      s_styled_content_height = body_y + height + BOTTOM_MARGIN;
      const int16_t content_height = s_styled_content_height > INT16_MAX
                                        ? INT16_MAX
                                        : (int16_t)s_styled_content_height;
      scroll_layer_set_content_size(s_scroll_layer,
          GSize(bounds.size.w, content_height > bounds.size.h
                                  ? content_height : bounds.size.h));
      styled_offset_changed(s_scroll_layer, NULL);
      restore_saved_position();
      return;
    }
    s_text = "Unable to display this prayer. Please reopen it.";
  }
  s_body_layer = text_layer_create(
      GRect(HORIZONTAL_MARGIN, body_y, text_width, BODY_LAYOUT_HEIGHT));
  text_layer_set_background_color(s_body_layer, GColorClear);
  text_layer_set_text_color(s_body_layer, app_theme_foreground_color());
  s_rendered_body_font = get_body_font();
  text_layer_set_font(s_body_layer, s_rendered_body_font);
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
  restore_saved_position();
}

static void window_unload(Window *window) {
  stop_fast_scrolling();
  if (!s_rebuilding) save_position();

  if (s_styled_body_layer) {
    layer_destroy(s_styled_body_layer);
    s_styled_body_layer = NULL;
  }
  free(s_paragraph_heights);
  s_paragraph_heights = NULL;
  s_styled_content_height = 0;
  if (s_italic_body_font) {
    fonts_unload_custom_font(s_italic_body_font);
    s_italic_body_font = NULL;
  }

  if (s_body_layer) {
    text_layer_destroy(s_body_layer);
    s_body_layer = NULL;
  }

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

void prayer_screen_show_entry(MainMenuEntryId entry_id, const char *title,
                              const PrayerTranslation *translation, bool resume) {
  if (!translation) return;
  if (window_stack_contains_window(s_window)) window_stack_remove(s_window, false);
  s_entry_id = entry_id;
  s_resume = resume;
  s_title = title;
  s_text = translation->text;
  s_paragraphs = translation->paragraph_count ? translation->paragraphs : NULL;
  s_paragraph_count = s_paragraphs ? translation->paragraph_count : 0;
  window_stack_push(s_window, true);
}
void prayer_screen_show(const char *title, const char *text) {
  const PrayerTranslation translation = {.text = text};
  prayer_screen_show_entry(MAIN_MENU_ENTRY_NONE, title, &translation, false);
}
void prayer_screen_show_translation(const char *title, const PrayerTranslation *translation) {
  prayer_screen_show_entry(MAIN_MENU_ENTRY_NONE, title, translation, false);
}

// Measure prefixes only when saving/restoring, never during scrolling. The
// original plain TextLayer is retained, preserving its exact line wrapping.
static int32_t plain_boundary_y(char *copy, uint32_t boundary) {
  if (!boundary) return 0;
  const char saved = copy[boundary];
  copy[boundary] = '\0';
  const int16_t width = layer_get_bounds(scroll_layer_get_layer(s_scroll_layer)).size.w - 2 * HORIZONTAL_MARGIN;
  const int32_t height = graphics_text_layout_get_content_size(copy, s_rendered_body_font,
      GRect(0, 0, width, BODY_LAYOUT_HEIGHT), GTextOverflowModeWordWrap, GTextAlignmentLeft).h;
  copy[boundary] = saved;
  return height;
}
static bool capture_position(ReadingPosition *position) {
  if (!s_scroll_layer || (!s_body_layer && !s_styled_body_layer)) return false;
  *position = (ReadingPosition){.entry = s_entry_id};
  const int32_t y = -(s_styled_body_layer ? styled_scroll_offset() :
      scroll_layer_get_content_offset(s_scroll_layer).y) - s_title_height - BODY_TOP_MARGIN;
  if (y <= 0) return true;
  if (s_styled_body_layer) {
    int32_t start = 0;
    for (uint16_t i = 0; i < s_paragraph_count; ++i) {
      const int32_t end = start + s_paragraph_heights[i] +
          (s_paragraphs[i].space_after ? PARAGRAPH_GAP : 0);
      if (y < end || i + 1 == s_paragraph_count) {
        position->kind = READING_POSITION_STYLED;
        position->anchor = i;
        position->fraction = reading_position_fraction(y, start, end);
        return true;
      }
      start = end;
    }
    return false;
  }
  if (!s_text) return false;
  const size_t length = strlen(s_text);
  char *copy = malloc(length + 1);
  uint32_t count = 1;
  for (size_t i = 0; i < length; ++i) if (s_text[i] == '\n') ++count;
  uint32_t *boundaries = malloc((count + 1) * sizeof(uint32_t));
  if (!copy || !boundaries) { free(copy); free(boundaries); return false; }
  memcpy(copy, s_text, length + 1);
  uint32_t next = 0;
  boundaries[next++] = 0;
  for (size_t i = 0; i < length; ++i) if (s_text[i] == '\n') boundaries[next++] = i + 1;
  boundaries[count] = length;
  uint32_t low = 0, high = count;
  while (low + 1 < high) {
    const uint32_t middle = (low + high) / 2;
    if (plain_boundary_y(copy, boundaries[middle]) <= y) low = middle;
    else high = middle;
  }
  position->kind = READING_POSITION_PLAIN;
  position->anchor = boundaries[low];
  position->fraction = reading_position_fraction(y,
      plain_boundary_y(copy, boundaries[low]), plain_boundary_y(copy, boundaries[low + 1]));
  free(copy);
  free(boundaries);
  return true;
}
static void restore_position(const ReadingPosition *position) {
  int32_t y = 0;
  if (position->kind == READING_POSITION_STYLED && s_styled_body_layer && position->anchor < s_paragraph_count) {
    int32_t start = 0;
    for (uint32_t i = 0; i < position->anchor; ++i) {
      start += s_paragraph_heights[i] + (s_paragraphs[i].space_after ? PARAGRAPH_GAP : 0);
    }
    const int32_t end = start + s_paragraph_heights[position->anchor] +
        (s_paragraphs[position->anchor].space_after ? PARAGRAPH_GAP : 0);
    y = s_title_height + BODY_TOP_MARGIN + reading_position_offset(position->fraction, start, end);
  } else if (position->kind == READING_POSITION_PLAIN && s_body_layer && s_text) {
    const size_t length = strlen(s_text);
    if (position->anchor > length || (position->anchor && s_text[position->anchor - 1] != '\n')) return;
    char *copy = malloc(length + 1);
    if (!copy) return;
    memcpy(copy, s_text, length + 1);
    const char *newline = strchr(s_text + position->anchor, '\n');
    const uint32_t end = newline ? (uint32_t)(newline - s_text + 1) : length;
    y = s_title_height + BODY_TOP_MARGIN + reading_position_offset(position->fraction,
        plain_boundary_y(copy, position->anchor), plain_boundary_y(copy, end));
    free(copy);
  }
  const int32_t range = s_styled_body_layer ? styled_scroll_range() : native_scroll_range();
  if (y > range) y = range;
  const int16_t native_y = range ? (int64_t)y * native_scroll_range() / range : 0;
  scroll_layer_set_content_offset(s_scroll_layer, GPoint(0, -native_y), false);
  styled_offset_changed(s_scroll_layer, NULL);
}
static void save_position(void) {
  ReadingPosition position;
  if (s_entry_id != MAIN_MENU_ENTRY_NONE && capture_position(&position)) reading_position_save(&position);
}
static void restore_saved_position(void) {
  if (!s_resume) return;
  ReadingPosition position;
  if (reading_position_get(&position) && position.entry == s_entry_id) restore_position(&position);
  s_resume = false;
}
void prayer_screen_refresh(void) {
  if (!s_scroll_layer) return;
  ReadingPosition position;
  const bool captured = capture_position(&position);
  s_rebuilding = true;
  window_unload(s_window);
  window_load(s_window);
  s_rebuilding = false;
  if (captured) restore_position(&position);
}
