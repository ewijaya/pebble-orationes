#include "accessible_menu.h"

#include "app_theme.h"
#include "app_settings.h"

enum {
  ROW_HORIZONTAL_MARGIN = 8,
  POSITION_GUTTER = 6,
  CHEVRON_SPACE = 14,
  ROW_VALUE_GAP = 6,
  TEXT_VERTICAL_ADJUSTMENT = -1,
  HEADER_DIVIDER_HEIGHT = 2,
  // Gothic's glyph ink sits below its reported line box. Move header text up
  // so descenders clear the divider within a compact 32-pixel band.
  HEADER_TEXT_Y = -3,
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
  accessible_menu_selection_changed(menu_layer, MenuIndex(0, 0), MenuIndex(0, 0), NULL);
  menu_layer_set_normal_colors(menu_layer, app_theme_background_color(),
                               app_theme_foreground_color());
  menu_layer_set_highlight_colors(
      menu_layer, app_theme_selected_background_color(),
      app_theme_selected_foreground_color());
}

int16_t accessible_menu_get_cell_height(MenuLayer *menu_layer,
                                        MenuIndex *cell_index, void *context) {
  return accessible_menu_min_row_height();
}

int16_t accessible_menu_wrapped_row_height(MenuLayer *menu_layer,
                                           const char *text) {
  const int16_t width = layer_get_bounds(menu_layer_get_layer(menu_layer)).size.w -
                        2 * ROW_HORIZONTAL_MARGIN;
  const GSize size = graphics_text_layout_get_content_size(
      text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
      GRect(0, 0, width, 300), GTextOverflowModeWordWrap, GTextAlignmentLeft);
  const int16_t padded_height = size.h + 12;
  return padded_height > accessible_menu_min_row_height()
             ? padded_height : accessible_menu_min_row_height();
}

int16_t accessible_menu_get_header_height(MenuLayer *menu_layer,
                                          uint16_t section_index,
                                          void *context) {
  const int16_t width = layer_get_bounds(menu_layer_get_layer(menu_layer)).size.w - 16;
  const int16_t height = graphics_text_layout_get_content_size(
      context, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
      GRect(0, 0, width, 300), GTextOverflowModeWordWrap, GTextAlignmentLeft).h;
  return height + 4 > ACCESSIBLE_MENU_HEADER_HEIGHT ? height + 4
                                                   : ACCESSIBLE_MENU_HEADER_HEIGHT;
}

void accessible_menu_draw_header(GContext *ctx, const Layer *cell_layer,
                                 uint16_t section_index, void *context) {
  const GRect bounds = layer_get_bounds(cell_layer);
  graphics_context_set_fill_color(ctx, app_theme_title_background_color());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, app_theme_title_foreground_color());
  graphics_draw_text(
      ctx, context, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
      GRect(ROW_HORIZONTAL_MARGIN, HEADER_TEXT_Y,
            bounds.size.w - 2 * ROW_HORIZONTAL_MARGIN,
            bounds.size.h - HEADER_DIVIDER_HEIGHT - HEADER_TEXT_Y),
      GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

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
         measured(title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
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
  GFont title_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
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
      12 + measured(text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
                    layer_get_bounds(menu_layer_get_layer(menu)).size.w - 44);
  return h > accessible_menu_min_row_height() ? h : accessible_menu_min_row_height();
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
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  int16_t h = measured(text, font, b.size.w - 44);
  graphics_context_set_text_color(ctx, fg);
  graphics_draw_text(ctx, text, font,
                     GRect(36, (b.size.h - h) / 2 - 1, b.size.w - 44, h),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

int16_t accessible_menu_min_row_height(void) {
  return app_settings_get_compact_menus() ? 48 : ACCESSIBLE_MENU_ROW_HEIGHT;
}

// Each indicator belongs to its menu and is freed with it. The gutter sits
// outside the ScrollLayer viewport, so neither wrapped text nor touch scroll
// can cover it. All current menu windows fit within this bounded registry.
static struct {
  MenuLayer *menu;
  Layer *indicator;
  MenuLayerGetNumberOfRowsInSectionsCallback count;
  void *context;
} s_indicators[16];
static void draw_position(Layer *layer, GContext *ctx) {
  MenuLayer *menu = *(MenuLayer **)layer_get_data(layer);
  ScrollLayer *scroll = menu_layer_get_scroll_layer(menu);
  const GRect bounds = layer_get_bounds(layer);
  const int16_t viewport = bounds.size.h;
  const int16_t content = scroll_layer_get_content_size(scroll).h;
  graphics_context_set_fill_color(ctx, app_theme_background_color());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  if (content <= viewport) return;
  uint16_t count = 0;
  for (unsigned i = 0; i < ARRAY_LENGTH(s_indicators); ++i)
    if (s_indicators[i].menu == menu && s_indicators[i].count)
      count = s_indicators[i].count(menu, 0, s_indicators[i].context);
  if (count < 2) return;
  const int16_t track = viewport - 8;
  int16_t thumb = (int32_t)track * viewport / content;
  if (thumb < 8) thumb = 8;
  const uint16_t row = menu_layer_get_selected_index(menu).row;
  // Selection position is stable during animation and jumps to either end on wrap.
  const int16_t y = 4 + (int32_t)(track - thumb) * row / (count - 1);
  graphics_context_set_stroke_color(ctx, app_theme_foreground_color());
  graphics_draw_line(ctx, GPoint(3, 4), GPoint(3, viewport - 5));
  graphics_context_set_fill_color(ctx, app_theme_foreground_color());
  graphics_fill_rect(ctx, GRect(2, y, 3, thumb), 1, GCornersAll);
}
MenuLayer *accessible_menu_create(GRect frame) {
  frame.size.w -= POSITION_GUTTER;
  MenuLayer *menu = menu_layer_create(frame);
  if (!menu) return NULL;

  for (unsigned i = 0; i < ARRAY_LENGTH(s_indicators); ++i) {
    if (s_indicators[i].menu) continue;
    Layer *indicator = layer_create_with_data(
        GRect(frame.origin.x + frame.size.w, frame.origin.y, POSITION_GUTTER, frame.size.h), sizeof(MenuLayer *));
    if (indicator) {
      *(MenuLayer **)layer_get_data(indicator) = menu;
      layer_set_update_proc(indicator, draw_position);
      s_indicators[i].menu = menu;
      s_indicators[i].indicator = indicator;
    }
    break;
  }
  return menu;
}
void accessible_menu_destroy(MenuLayer *menu) {
  for (unsigned i = 0; i < ARRAY_LENGTH(s_indicators); ++i) {
    if (s_indicators[i].menu != menu) continue;
    layer_destroy(s_indicators[i].indicator);
    s_indicators[i].menu = NULL;
    s_indicators[i].indicator = NULL;
    break;
  }
  menu_layer_destroy(menu);
}
void accessible_menu_selection_changed(MenuLayer *menu, MenuIndex index,
                                       MenuIndex old, void *context) {
  layer_mark_dirty(menu_layer_get_layer(menu));
  for (unsigned i = 0; i < ARRAY_LENGTH(s_indicators); ++i)
    if (s_indicators[i].menu == menu) layer_mark_dirty(s_indicators[i].indicator);
}
int16_t accessible_menu_submenu_height(MenuLayer *menu, const char *text) {
  const int16_t width = layer_get_bounds(menu_layer_get_layer(menu)).size.w -
                        16 - CHEVRON_SPACE;
  int16_t height = measured(text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), width) + 12;
  return height > accessible_menu_min_row_height() ? height : accessible_menu_min_row_height();
}
void accessible_menu_draw_submenu(GContext *ctx, const Layer *cell, const char *text) {
  GRect bounds = layer_get_bounds(cell);
  const bool selected = menu_cell_layer_is_highlighted(cell);
  const GColor fg = selected ? app_theme_selected_foreground_color() : app_theme_foreground_color();
  graphics_context_set_fill_color(ctx, selected ? app_theme_selected_background_color() : app_theme_background_color());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  const int16_t width = bounds.size.w - 16 - CHEVRON_SPACE;
  const GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  const int16_t height = measured(text, font, width);
  graphics_context_set_text_color(ctx, fg);
  graphics_draw_text(ctx, text, font, GRect(8, (bounds.size.h - height) / 2 - 1, width, height),
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  const int16_t x = bounds.size.w - 12, y = bounds.size.h / 2;
  graphics_context_set_stroke_color(ctx, fg);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_line(ctx, GPoint(x, y - 4), GPoint(x + 4, y));
  graphics_draw_line(ctx, GPoint(x + 4, y), GPoint(x, y + 4));
  graphics_context_set_stroke_width(ctx, 1);
}
void accessible_menu_draw_divider(GContext *ctx, const Layer *cell) {
  graphics_context_set_stroke_color(ctx, menu_cell_layer_is_highlighted(cell)
      ? app_theme_selected_foreground_color() : app_theme_foreground_color());
  graphics_draw_line(ctx, GPoint(8, 0), GPoint(layer_get_bounds(cell).size.w - 8, 0));
}

void accessible_menu_add_to_layer(Layer *parent, MenuLayer *menu) {
  layer_add_child(parent, menu_layer_get_layer(menu));
  for (unsigned i = 0; i < ARRAY_LENGTH(s_indicators); ++i)
    if (s_indicators[i].menu == menu) layer_add_child(parent, s_indicators[i].indicator);
}
void accessible_menu_set_callbacks(MenuLayer *menu, void *context, MenuLayerCallbacks callbacks) {
  for (unsigned i = 0; i < ARRAY_LENGTH(s_indicators); ++i) {
    if (s_indicators[i].menu != menu) continue;
    s_indicators[i].count = callbacks.get_num_rows;
    s_indicators[i].context = context;
  }
  callbacks.selection_changed = accessible_menu_selection_changed;
  menu_layer_set_callbacks(menu, context, callbacks);
}
