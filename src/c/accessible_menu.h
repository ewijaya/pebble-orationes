#pragma once

#include <pebble.h>

enum {
  ACCESSIBLE_MENU_ROW_HEIGHT = 54,
  ACCESSIBLE_MENU_HEADER_HEIGHT = 40,
};

void accessible_menu_apply_colors(MenuLayer *menu_layer);
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
