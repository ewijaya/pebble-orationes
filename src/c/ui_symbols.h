#pragma once
#include <pebble.h>
typedef enum {
  UI_SYMBOL_NONE,
  UI_SYMBOL_MARK,
  UI_SYMBOL_BOOKMARK,
  UI_SYMBOL_SUN,
  UI_SYMBOL_CROSS,
  UI_SYMBOL_HEART,
  UI_SYMBOL_BOOK,
  UI_SYMBOL_CARD
} UiSymbol;
void ui_symbol_draw(GContext *ctx, UiSymbol symbol, GPoint origin,
                    GColor color);
