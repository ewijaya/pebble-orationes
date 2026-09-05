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
      text, font, measurement_bounds, GTextOverflowModeWordWrap,
      GTextAlignmentLeft);
  const int16_t text_y =
      ((bounds.size.h - text_size.h) / 2) + TEXT_VERTICAL_ADJUSTMENT;

  graphics_context_set_fill_color(ctx, background);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, foreground);
  graphics_draw_text(
      ctx, text, font,
      GRect(ROW_HORIZONTAL_MARGIN, text_y, text_width, text_size.h),
      GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
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
  const int16_t height =
      accessible_menu_wrapped_row_height(menu_layer, context);
  return height > ACCESSIBLE_MENU_ROW_HEIGHT ? height
                                             : ACCESSIBLE_MENU_HEADER_HEIGHT;
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
  graphics_draw_text(ctx, text, label_font,
                     GRect(ROW_HORIZONTAL_MARGIN, label_y, label_width, 34),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
      ctx, value, value_font,
      GRect(bounds.size.w - ROW_HORIZONTAL_MARGIN - value_width, value_y,
            value_width, value_size.h),
      GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
}

static int16_t measured(const char *text, GFont font, int16_t width) {
  return graphics_text_layout_get_content_size(
             text, font, GRect(0, 0, width, 300), GTextOverflowModeWordWrap,
             GTextAlignmentLeft)
      .h;
}
int16_t accessible_menu_detail_height(MenuLayer *menu, const char *title,
                                      const char *detail, UiSymbol icon) {
  int16_t width = layer_get_bounds(menu_layer_get_layer(menu)).size.w - 16 -
                  (icon ? 28 : 0);
  return 12 +
         measured(title, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                  width) +
         measured(detail, fonts_get_system_font(FONT_KEY_GOTHIC_24), width);
}
void accessible_menu_draw_detail(GContext *ctx, const Layer *cell,
                                 const char *title, const char *detail,
                                 UiSymbol icon) {
  const bool selected = menu_cell_layer_is_highlighted(cell);
  GColor bg = selected ? app_theme_selected_background_color()
                       : app_theme_background_color();
  GColor fg = selected ? app_theme_selected_foreground_color()
                       : app_theme_foreground_color();
  GRect b = layer_get_bounds(cell);
  int16_t x = 8 + (icon ? 28 : 0), w = b.size.w - x - 8;
  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, b, 0, GCornerNone);
  if (icon)
    ui_symbol_draw(ctx, icon, GPoint(8, 12), fg);
  GFont title_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
        detail_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  int16_t h = measured(title, title_font, w);
  graphics_context_set_text_color(ctx, fg);
  graphics_draw_text(ctx, title, title_font, GRect(x, 3, w, h),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, detail, detail_font,
                     GRect(x, 3 + h, w, b.size.h - 6 - h),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}
int16_t accessible_menu_icon_height(MenuLayer *menu, const char *text) {
  int16_t h =
      12 + measured(text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                    layer_get_bounds(menu_layer_get_layer(menu)).size.w - 44);
  return h > ACCESSIBLE_MENU_ROW_HEIGHT ? h : ACCESSIBLE_MENU_ROW_HEIGHT;
}
void accessible_menu_draw_icon_row(GContext *ctx, const Layer *cell,
                                   const char *text, UiSymbol icon) {
  const bool selected = menu_cell_layer_is_highlighted(cell);
  GRect b = layer_get_bounds(cell);
  GColor fg = selected ? app_theme_selected_foreground_color()
                       : app_theme_foreground_color();
  graphics_context_set_fill_color(
      ctx, selected ? app_theme_selected_background_color()
                    : app_theme_background_color());
  graphics_fill_rect(ctx, b, 0, GCornerNone);
  ui_symbol_draw(ctx, icon, GPoint(8, (b.size.h - 24) / 2), fg);
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  int16_t h = measured(text, font, b.size.w - 44);
  graphics_context_set_text_color(ctx, fg);
  graphics_draw_text(ctx, text, font,
                     GRect(36, (b.size.h - h) / 2 - 1, b.size.w - 44, h),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}
