#include "accessible_menu.h"

enum {
  ROW_HORIZONTAL_MARGIN = 8,
  TEXT_VERTICAL_ADJUSTMENT = -1,
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
  menu_layer_set_normal_colors(menu_layer, GColorWhite, GColorBlack);
  menu_layer_set_highlight_colors(menu_layer, GColorBlack, GColorWhite);
}

int16_t accessible_menu_get_cell_height(MenuLayer *menu_layer,
                                        MenuIndex *cell_index, void *context) {
  return ACCESSIBLE_MENU_ROW_HEIGHT;
}

int16_t accessible_menu_get_header_height(MenuLayer *menu_layer,
                                          uint16_t section_index,
                                          void *context) {
  return ACCESSIBLE_MENU_HEADER_HEIGHT;
}

void accessible_menu_draw_header(GContext *ctx, const Layer *cell_layer,
                                 uint16_t section_index, void *context) {
  draw_centered_text(ctx, cell_layer, (const char *)context, GColorWhite,
                     GColorBlack);
}

void accessible_menu_draw_row(GContext *ctx, const Layer *cell_layer,
                              const char *text) {
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  draw_centered_text(ctx, cell_layer, text,
                     selected ? GColorBlack : GColorWhite,
                     selected ? GColorWhite : GColorBlack);
}
