#include "accessible_menu.h"

#include "app_theme.h"

enum {
  ROW_HORIZONTAL_MARGIN = 8,
  ROW_VALUE_GAP = 6,
  TEXT_VERTICAL_ADJUSTMENT = -1,
  HEADER_DIVIDER_HEIGHT = 2,
};

static void draw_centered_text(GContext *ctx, const Layer *cell_layer,
                               const char *text, GColor background,
                               GColor foreground) {
  const GRect bounds = layer_get_bounds(cell_layer);
  const GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  const int16_t text_width = bounds.size.w - (2 * ROW_HORIZONTAL_MARGIN);
  const GRect measurement_bounds =
      GRect(0, 0, text_width, bounds.size.h);
  const GSize text_size = graphics_text_layout_get_content_size(
      text, font, measurement_bounds, GTextOverflowModeTrailingEllipsis,
      GTextAlignmentLeft);
  const int16_t text_y =
      ((bounds.size.h - text_size.h) / 2) + TEXT_VERTICAL_ADJUSTMENT;

  graphics_context_set_fill_color(ctx, background);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, foreground);
  graphics_draw_text(
      ctx, text, font,
      GRect(ROW_HORIZONTAL_MARGIN, text_y, text_width, text_size.h),
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

void accessible_menu_apply_colors(MenuLayer *menu_layer) {
  menu_layer_set_normal_colors(menu_layer, app_theme_background_color(),
                               app_theme_foreground_color());
  menu_layer_set_highlight_colors(
      menu_layer, app_theme_selected_background_color(),
      app_theme_selected_foreground_color());
}

int16_t accessible_menu_get_cell_height(MenuLayer *menu_layer,
                                        MenuIndex *cell_index, void *context) {
  return ACCESSIBLE_MENU_ROW_HEIGHT;
}

int16_t accessible_menu_wrapped_row_height(MenuLayer *menu_layer,
                                           const char *text) {
  const int16_t width = layer_get_bounds(menu_layer_get_layer(menu_layer)).size.w -
                        2 * ROW_HORIZONTAL_MARGIN;
  const GSize size = graphics_text_layout_get_content_size(
      text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
      GRect(0, 0, width, 300), GTextOverflowModeWordWrap, GTextAlignmentLeft);
  const int16_t padded_height = size.h + 12;
  return padded_height > ACCESSIBLE_MENU_ROW_HEIGHT
             ? padded_height : ACCESSIBLE_MENU_ROW_HEIGHT;
}

int16_t accessible_menu_get_header_height(MenuLayer *menu_layer,
                                          uint16_t section_index,
                                          void *context) {
  return ACCESSIBLE_MENU_HEADER_HEIGHT;
}

void accessible_menu_draw_header(GContext *ctx, const Layer *cell_layer,
                                 uint16_t section_index, void *context) {
  draw_centered_text(ctx, cell_layer, (const char *)context,
                     app_theme_title_background_color(),
                     app_theme_title_foreground_color());

  const GRect bounds = layer_get_bounds(cell_layer);
  graphics_context_set_fill_color(ctx, app_theme_background_color());
  graphics_fill_rect(
      ctx,
      GRect(0, bounds.size.h - HEADER_DIVIDER_HEIGHT, bounds.size.w,
            HEADER_DIVIDER_HEIGHT),
      0, GCornerNone);
}

void accessible_menu_draw_row(GContext *ctx, const Layer *cell_layer,
                              const char *text) {
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  draw_centered_text(ctx, cell_layer, text,
                     selected ? app_theme_selected_background_color()
                              : app_theme_background_color(),
                     selected ? app_theme_selected_foreground_color()
                              : app_theme_foreground_color());
}

void accessible_menu_draw_row_with_value(GContext *ctx,
                                         const Layer *cell_layer,
                                         const char *text,
                                         const char *value) {
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  const GColor background =
      selected ? app_theme_selected_background_color()
               : app_theme_background_color();
  const GColor foreground =
      selected ? app_theme_selected_foreground_color()
               : app_theme_foreground_color();
  const GRect bounds = layer_get_bounds(cell_layer);
  const GFont label_font =
      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  const GFont value_font =
      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  const GSize value_size = graphics_text_layout_get_content_size(
      value, value_font, GRect(0, 0, bounds.size.w, bounds.size.h),
      GTextOverflowModeTrailingEllipsis, GTextAlignmentRight);
  const int16_t value_width = value_size.w;
  const int16_t label_width =
      bounds.size.w - (2 * ROW_HORIZONTAL_MARGIN) - ROW_VALUE_GAP -
      value_width;
  const int16_t label_y =
      ((bounds.size.h - 34) / 2) + TEXT_VERTICAL_ADJUSTMENT;
  const int16_t value_y =
      ((bounds.size.h - value_size.h) / 2) + TEXT_VERTICAL_ADJUSTMENT;

  graphics_context_set_fill_color(ctx, background);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, foreground);
  graphics_draw_text(
      ctx, text, label_font,
      GRect(ROW_HORIZONTAL_MARGIN, label_y, label_width, 34),
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  graphics_draw_text(
      ctx, value, value_font,
      GRect(bounds.size.w - ROW_HORIZONTAL_MARGIN - value_width, value_y,
            value_width, value_size.h),
      GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
}
