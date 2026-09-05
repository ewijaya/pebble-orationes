#pragma once

#include "ui_symbols.h"
#include <pebble.h>

enum {
  ACCESSIBLE_MENU_ROW_HEIGHT = 54,
  ACCESSIBLE_MENU_HEADER_HEIGHT = 40,
};

void accessible_menu_apply_colors(MenuLayer *menu_layer);
int16_t accessible_menu_wrapped_row_height(MenuLayer *menu_layer,
                                           const char *text);
int16_t accessible_menu_get_cell_height(MenuLayer *menu_layer,
                                        MenuIndex *cell_index, void *context);
int16_t accessible_menu_get_header_height(MenuLayer *menu_layer,
                                          uint16_t section_index,
                                          void *context);
void accessible_menu_draw_header(GContext *ctx, const Layer *cell_layer,
                                 uint16_t section_index, void *context);
void accessible_menu_draw_row(GContext *ctx, const Layer *cell_layer,
                              const char *text);
void accessible_menu_draw_row_with_value(GContext *ctx,
                                         const Layer *cell_layer,
                                         const char *text,
                                         const char *value);

void accessible_menu_draw_detail(GContext *ctx, const Layer *cell,
                                 const char *title, const char *detail,
                                 UiSymbol icon);
int16_t accessible_menu_detail_height(MenuLayer *menu, const char *title,
                                      const char *detail, UiSymbol icon);
void accessible_menu_draw_icon_row(GContext *ctx, const Layer *cell,
                                   const char *text, UiSymbol icon);
int16_t accessible_menu_icon_height(MenuLayer *menu, const char *text);
