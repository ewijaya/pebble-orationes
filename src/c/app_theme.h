#pragma once

#include "app_settings.h"
#include <pebble.h>

GColor app_theme_background_color(void);
GColor app_theme_foreground_color(void);
GColor app_theme_selected_background_color(void);
GColor app_theme_selected_foreground_color(void);
GColor app_theme_title_background_color(void);
GColor app_theme_title_foreground_color(void);

GColor app_theme_navigation_background(AppNavigationHighlight value, bool dark);
GColor app_theme_navigation_foreground(AppNavigationHighlight value, bool dark);
GColor app_theme_accent_background(AppAccentColor value, bool dark);
GColor app_theme_accent_foreground(AppAccentColor value, bool dark);
