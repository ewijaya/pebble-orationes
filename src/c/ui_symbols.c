#include "ui_symbols.h"
#include "brand_icon.inc"
void ui_symbol_draw(GContext *ctx, UiSymbol symbol, GPoint origin,
                    GColor color) {
  if (symbol == UI_SYMBOL_MARK) {
    graphics_context_set_fill_color(ctx, color);
    for (uint8_t row = 0; row < 25; ++row) {
      uint32_t pixels = s_brand_icon_rows[row];
      uint8_t column = 0;
      while (pixels) {
        if (!(pixels & 1)) { pixels >>= 1; ++column; continue; }
        const uint8_t start = column;
        do { pixels >>= 1; ++column; } while (pixels & 1);
        graphics_fill_rect(ctx, GRect(origin.x + start, origin.y + row,
                                      column - start, 1), 0, GCornerNone);
      }
    }
    return;
  }
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 2);
  const int x = origin.x, y = origin.y;
#define LINE(a, b, c, d)                                                       \
  graphics_draw_line(ctx, GPoint(x + (a), y + (b)), GPoint(x + (c), y + (d)))
  // Compact line segments avoid repeating coordinate/call setup for every
  // stroke.
  static const int8_t strokes[][4] = {
      {16, 0, 16, 15},  {12, 4, 21, 4},   {5, 2, 19, 2},    {5, 2, 5, 22},
      {19, 2, 19, 22},  {5, 22, 12, 17},  {12, 17, 19, 22}, {12, 0, 12, 3},
      {12, 21, 12, 24}, {0, 12, 3, 12},   {21, 12, 24, 12}, {3, 3, 5, 5},
      {19, 19, 21, 21}, {3, 21, 5, 19},   {19, 5, 21, 3},   {12, 1, 12, 23},
      {3, 8, 21, 8},    {12, 6, 7, 2},    {7, 2, 2, 6},     {2, 6, 2, 11},
      {2, 11, 12, 22},  {12, 22, 22, 11}, {22, 11, 22, 6},  {22, 6, 17, 2},
      {17, 2, 12, 6},   {12, 3, 12, 21},  {5, 8, 9, 8},     {15, 8, 19, 8},
      {7, 16, 17, 16},  {7, 19, 17, 19}};
  static const uint8_t offsets[] = {0, 0, 2, 7, 15, 17, 25, 28, 30};
  if (symbol > UI_SYMBOL_NONE && symbol <= UI_SYMBOL_CARD) {
    for (uint8_t i = offsets[symbol]; i < offsets[symbol + 1]; ++i) {
      const int8_t *p = strokes[i];
      LINE(p[0], p[1], p[2], p[3]);
    }
  }
  switch (symbol) {
  case UI_SYMBOL_SUN:
    graphics_draw_circle(ctx, GPoint(x + 12, y + 12), 5);
    break;
  case UI_SYMBOL_BOOK:
    graphics_draw_rect(ctx, GRect(x + 2, y + 3, 20, 18));
    break;
  case UI_SYMBOL_CARD:
    graphics_draw_round_rect(ctx, GRect(x + 3, y + 1, 18, 22), 2);
    graphics_draw_circle(ctx, GPoint(x + 12, y + 8), 3);
    break;
  default:
    break;
  }
#undef LINE
  graphics_context_set_stroke_width(ctx, 1);
}
