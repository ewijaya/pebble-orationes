#include "app_theme.h"

#include "app_settings.h"

static bool is_dark(void) {
  return app_settings_get_appearance() == APP_APPEARANCE_DARK;
}

GColor app_theme_accent_background(AppAccentColor value, bool dark) {
  switch (value) {
  case APP_ACCENT_COLOR_OCEAN:
    return dark ? GColorTiffanyBlue : GColorCobaltBlue;
  case APP_ACCENT_COLOR_BURGUNDY:
    return dark ? GColorDarkCandyAppleRed : GColorBulgarianRose;
  case APP_ACCENT_COLOR_FOREST:
    return dark ? GColorMayGreen : GColorDarkGreen;
  case APP_ACCENT_COLOR_MONOCHROME:
    return dark ? GColorWhite : GColorBlack;
  case APP_ACCENT_COLOR_COUNT:
    break;
  }

  return dark ? GColorTiffanyBlue : GColorCobaltBlue;
}

GColor app_theme_background_color(void) {
  return is_dark() ? GColorBlack : GColorWhite;
}

GColor app_theme_foreground_color(void) {
  return is_dark() ? GColorWhite : GColorBlack;
}

GColor app_theme_selected_background_color(void) {
  return app_theme_navigation_background(
      app_settings_get_navigation_highlight(), is_dark());
}

GColor app_theme_selected_foreground_color(void) {
  return app_theme_navigation_foreground(
      app_settings_get_navigation_highlight(), is_dark());
}

GColor app_theme_title_background_color(void) {
  return app_theme_accent_background(app_settings_get_accent_color(),
                                     is_dark());
}

GColor app_theme_accent_foreground(AppAccentColor value, bool dark) {
  switch (value) {
  case APP_ACCENT_COLOR_OCEAN:
  case APP_ACCENT_COLOR_FOREST:
  case APP_ACCENT_COLOR_MONOCHROME:
    return dark ? GColorBlack : GColorWhite;
  case APP_ACCENT_COLOR_BURGUNDY:
  case APP_ACCENT_COLOR_COUNT:
    return GColorWhite;
  }

  return GColorWhite;
}

GColor app_theme_title_foreground_color(void) {
  return app_theme_accent_foreground(app_settings_get_accent_color(),
                                     is_dark());
}
GColor app_theme_navigation_background(AppNavigationHighlight value,
                                       bool dark) {
  switch (value) {
  case APP_NAVIGATION_AMBER:
    return GColorChromeYellow;
  case APP_NAVIGATION_TANGERINE:
    return GColorOrange;
  case APP_NAVIGATION_VIOLET:
    return GColorVividViolet;
  case APP_NAVIGATION_MAGENTA:
    return GColorMagenta;
  case APP_NAVIGATION_LIME:
    return GColorSpringBud;
  default:
    return dark ? GColorWhite : GColorBlack;
  }
}
GColor app_theme_navigation_foreground(AppNavigationHighlight value,
                                       bool dark) {
  if (value == APP_NAVIGATION_CLASSIC)
    return dark ? GColorBlack : GColorWhite;
  return value == APP_NAVIGATION_VIOLET ? GColorWhite : GColorBlack;
}
