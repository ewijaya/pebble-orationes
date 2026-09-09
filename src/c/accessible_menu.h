#pragma once

#include "ui_symbols.h"
#include <pebble.h>

enum {
  ACCESSIBLE_MENU_ROW_HEIGHT = 54,
  ACCESSIBLE_MENU_HEADER_HEIGHT = 32,
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

int16_t accessible_menu_min_row_height(void);
MenuLayer *accessible_menu_create(GRect frame);
void accessible_menu_destroy(MenuLayer *menu);
void accessible_menu_selection_changed(MenuLayer *menu, MenuIndex index,
                                       MenuIndex old, void *context);
void accessible_menu_draw_submenu(GContext *ctx, const Layer *cell, const char *text);
int16_t accessible_menu_submenu_height(MenuLayer *menu, const char *text);
void accessible_menu_draw_divider(GContext *ctx, const Layer *cell);

void accessible_menu_add_to_layer(Layer *parent, MenuLayer *menu);
void accessible_menu_set_callbacks(MenuLayer *menu, void *context, MenuLayerCallbacks callbacks);

int16_t accessible_menu_value_height(MenuLayer *menu, const char *text,
                                     const char *value);
