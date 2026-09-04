#include "app_theme.h"

#include "app_settings.h"

static bool is_dark(void) {
  return app_settings_get_appearance() == APP_APPEARANCE_DARK;
}

static GColor accent_color(void) {
  switch (app_settings_get_accent_color()) {
    case APP_ACCENT_COLOR_OCEAN:
      return is_dark() ? GColorTiffanyBlue : GColorCobaltBlue;
    case APP_ACCENT_COLOR_BURGUNDY:
      return is_dark() ? GColorDarkCandyAppleRed : GColorBulgarianRose;
    case APP_ACCENT_COLOR_FOREST:
      return is_dark() ? GColorMayGreen : GColorDarkGreen;
    case APP_ACCENT_COLOR_MONOCHROME:
      return is_dark() ? GColorWhite : GColorBlack;
    case APP_ACCENT_COLOR_COUNT:
      break;
  }

  return is_dark() ? GColorTiffanyBlue : GColorCobaltBlue;
}

GColor app_theme_background_color(void) {
  return is_dark() ? GColorBlack : GColorWhite;
}

GColor app_theme_foreground_color(void) {
  return is_dark() ? GColorWhite : GColorBlack;
}

GColor app_theme_selected_background_color(void) {
  return is_dark() ? GColorWhite : GColorBlack;
}

GColor app_theme_selected_foreground_color(void) {
  return is_dark() ? GColorBlack : GColorWhite;
}

GColor app_theme_title_background_color(void) {
  return accent_color();
}

GColor app_theme_title_foreground_color(void) {
  switch (app_settings_get_accent_color()) {
    case APP_ACCENT_COLOR_OCEAN:
    case APP_ACCENT_COLOR_FOREST:
    case APP_ACCENT_COLOR_MONOCHROME:
      return is_dark() ? GColorBlack : GColorWhite;
    case APP_ACCENT_COLOR_BURGUNDY:
    case APP_ACCENT_COLOR_COUNT:
      return GColorWhite;
  }

  return GColorWhite;
}
