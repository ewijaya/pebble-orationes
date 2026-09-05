#pragma once
#include <pebble.h>

typedef struct NavigationMenu NavigationMenu;
struct NavigationMenu {
  Window *window;
  MenuLayer *layer;
  const char *title;
  uint16_t (*count)(void *context);
  const char *(*label)(uint16_t row, void *context);
  void (*select)(uint16_t row, void *context);
  void *context;
};
void navigation_menu_init(NavigationMenu *menu, const char *title,
                          uint16_t (*count)(void *),
                          const char *(*label)(uint16_t, void *),
                          void (*select)(uint16_t, void *), void *context);
void navigation_menu_show(NavigationMenu *menu);
void navigation_menu_refresh(NavigationMenu *menu);
void navigation_menu_hide(NavigationMenu *menu);
void navigation_menu_deinit(NavigationMenu *menu);
