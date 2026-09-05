#include "appearance_picker.h"
#include "app_fonts.h"
#include "app_theme.h"
#include "phone_settings.h"
#include <pebble.h>
#include <stdio.h>
static Window *s_window;
static Layer *s_layer;
static AppearancePickerKind s_kind;
static int s_choice;
static bool s_failed;
static void (*s_saved)(void);
static int count(void) {
  const int counts[] = {APP_TEXT_SIZE_COUNT, APP_APPEARANCE_COUNT,
                        APP_ACCENT_COLOR_COUNT, APP_NAVIGATION_COUNT};
  return counts[s_kind];
}
static AppSettings candidate(void) {
  AppSettings value = app_settings_get();
  if (s_kind == APPEARANCE_PICKER_SIZE)
    value.text_size = s_choice;
  else if (s_kind == APPEARANCE_PICKER_APPEARANCE)
    value.appearance = s_choice;
  else if (s_kind == APPEARANCE_PICKER_ACCENT)
    value.accent_color = s_choice;
  else
    value.navigation_highlight = s_choice;
  return value;
}
static const char *label(void) {
  if (s_kind == APPEARANCE_PICKER_SIZE)
    return app_settings_text_size_label(s_choice);
  if (s_kind == APPEARANCE_PICKER_APPEARANCE)
    return app_settings_appearance_label(s_choice);
  if (s_kind == APPEARANCE_PICKER_ACCENT)
    return app_settings_accent_color_label(s_choice);
  return app_settings_navigation_highlight_label(s_choice);
}
static void draw_text(GContext *ctx, const char *text, GFont font, GRect rect,
                      GColor color) {
  graphics_context_set_text_color(ctx, color);
  graphics_draw_text(ctx, text, font, rect, GTextOverflowModeWordWrap,
                     GTextAlignmentCenter, NULL);
}
static void draw(Layer *layer, GContext *ctx) {
  const AppSettings value = candidate();
  const bool dark = value.appearance == APP_APPEARANCE_DARK;
  const GColor bg = dark ? GColorBlack : GColorWhite,
               fg = dark ? GColorWhite : GColorBlack;
  const GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  static const char *const titles[] = {"Text Size", "Appearance",
                                       "Title Accent", "Navigation"};
  draw_text(ctx, titles[s_kind], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
            GRect(0, 0, 200, 30), fg);
  const GColor nav =
      app_theme_navigation_background(value.navigation_highlight, dark);
  if (s_kind == APPEARANCE_PICKER_NAVIGATION ||
      s_kind == APPEARANCE_PICKER_ACCENT) {
    graphics_context_set_fill_color(
        ctx, s_kind == APPEARANCE_PICKER_NAVIGATION
                 ? nav
                 : app_theme_accent_background(value.accent_color, dark));
    graphics_fill_rect(ctx, GRect(8, 43, 16, 16), 2, GCornersAll);
    graphics_context_set_stroke_color(ctx, fg);
    graphics_draw_round_rect(ctx, GRect(8, 43, 16, 16), 2);
  }
  draw_text(ctx, label(), fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
            GRect(27, 31, 170, 38), fg);
  graphics_context_set_fill_color(
      ctx, app_theme_accent_background(value.accent_color, dark));
  graphics_fill_rect(ctx, GRect(8, 78, 184, 31), 0, GCornerNone);
  draw_text(ctx, "Prayer preview",
            fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
            GRect(8, 77, 184, 31),
            app_theme_accent_foreground(value.accent_color, dark));
  draw_text(ctx, "Make time",
            app_fonts_body(value.text_size == APP_TEXT_SIZE_EXTRA_LARGE),
            GRect(8, 107, 184, 43), fg);
  graphics_context_set_fill_color(ctx, nav);
  graphics_fill_rect(ctx, GRect(8, 151, 184, 34), 2, GCornersAll);
  draw_text(ctx, "Selected prayer",
            fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
            GRect(8, 150, 184, 34),
            app_theme_navigation_foreground(value.navigation_highlight, dark));
  char footer[48];
  snprintf(footer, sizeof(footer), "%d/%d  UP/DOWN: choose", s_choice + 1,
           count());
  draw_text(ctx, s_failed ? "Save failed. Try again." : footer,
            fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(0, 184, 200, 23),
            fg);
  draw_text(ctx, "SELECT: save   BACK: cancel",
            fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(0, 203, 200, 25),
            fg);
}
static void up(ClickRecognizerRef r, void *c) {
  s_choice = (s_choice + count() - 1) % count();
  s_failed = false;
  layer_mark_dirty(s_layer);
}
static void down(ClickRecognizerRef r, void *c) {
  s_choice = (s_choice + 1) % count();
  s_failed = false;
  layer_mark_dirty(s_layer);
}
static void select(ClickRecognizerRef r, void *c) {
  AppSettings value = candidate();
  if (!app_settings_apply(&value)) {
    s_failed = true;
    layer_mark_dirty(s_layer);
    return;
  }
  phone_settings_send_current();
  window_stack_remove(s_window, true);
  if (s_saved)
    s_saved();
}
static void clicks(void *c) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 180, up);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 180, down);
  window_single_click_subscribe(BUTTON_ID_SELECT, select);
}
static void load(Window *window) {
  s_layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(s_layer, draw);
  layer_add_child(window_get_root_layer(window), s_layer);
  window_set_click_config_provider(window, clicks);
}
static void unload(Window *window) {
  layer_destroy(s_layer);
  s_layer = NULL;
}
void appearance_picker_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window,
                             (WindowHandlers){.load = load, .unload = unload});
}
void appearance_picker_show(AppearancePickerKind kind, void (*saved)(void)) {
  s_kind = kind;
  s_saved = saved;
  s_failed = false;
  AppSettings v = app_settings_get();
  s_choice = kind == APPEARANCE_PICKER_SIZE         ? v.text_size
             : kind == APPEARANCE_PICKER_APPEARANCE ? v.appearance
             : kind == APPEARANCE_PICKER_ACCENT     ? v.accent_color
                                                    : v.navigation_highlight;
  window_stack_push(s_window, true);
}
void appearance_picker_refresh(void) {
  if (s_layer)
    layer_mark_dirty(s_layer);
}
void appearance_picker_deinit(void) {
  window_destroy(s_window);
  s_window = NULL;
}
