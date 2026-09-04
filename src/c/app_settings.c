#include "app_settings.h"

#include <pebble.h>

enum {
  PERSIST_KEY_TEXT_SIZE = 1,
  PERSIST_KEY_NOON_REMINDER_ENABLED = 2,
  PERSIST_KEY_NOON_REMINDER_DURATION = 3,
  PERSIST_KEY_ACCENT_COLOR = 4,
  PERSIST_KEY_APPEARANCE = 5,
};

static AppTextSize s_text_size = APP_TEXT_SIZE_LARGE;
static AppAccentColor s_accent_color = APP_ACCENT_COLOR_NAVY;
static AppAppearance s_appearance = APP_APPEARANCE_LIGHT;
static bool s_noon_reminder_enabled;
static AppNoonReminderDuration s_noon_reminder_duration =
    APP_NOON_REMINDER_DURATION_10_SECONDS;

static bool text_size_is_valid(int32_t value) {
  return value >= APP_TEXT_SIZE_LARGE && value < APP_TEXT_SIZE_COUNT;
}

static bool accent_color_is_valid(int32_t value) {
  return value >= APP_ACCENT_COLOR_NAVY && value < APP_ACCENT_COLOR_COUNT;
}

static bool appearance_is_valid(int32_t value) {
  return value >= APP_APPEARANCE_LIGHT && value < APP_APPEARANCE_COUNT;
}

static bool noon_reminder_duration_is_valid(int32_t value) {
  return value >= APP_NOON_REMINDER_DURATION_5_SECONDS &&
         value < APP_NOON_REMINDER_DURATION_COUNT;
}

void app_settings_init(void) {
  s_text_size = APP_TEXT_SIZE_LARGE;
  s_accent_color = APP_ACCENT_COLOR_NAVY;
  s_appearance = APP_APPEARANCE_LIGHT;
  s_noon_reminder_enabled = false;
  s_noon_reminder_duration = APP_NOON_REMINDER_DURATION_10_SECONDS;

  if (persist_exists(PERSIST_KEY_TEXT_SIZE)) {
    const int32_t stored_text_size = persist_read_int(PERSIST_KEY_TEXT_SIZE);
    if (text_size_is_valid(stored_text_size)) {
      s_text_size = (AppTextSize)stored_text_size;
    }
  }

  if (persist_exists(PERSIST_KEY_NOON_REMINDER_ENABLED)) {
    s_noon_reminder_enabled =
        persist_read_bool(PERSIST_KEY_NOON_REMINDER_ENABLED);
  }

  if (persist_exists(PERSIST_KEY_ACCENT_COLOR)) {
    const int32_t stored_accent_color =
        persist_read_int(PERSIST_KEY_ACCENT_COLOR);
    if (accent_color_is_valid(stored_accent_color)) {
      s_accent_color = (AppAccentColor)stored_accent_color;
    }
  }

  if (persist_exists(PERSIST_KEY_APPEARANCE)) {
    const int32_t stored_appearance = persist_read_int(PERSIST_KEY_APPEARANCE);
    if (appearance_is_valid(stored_appearance)) {
      s_appearance = (AppAppearance)stored_appearance;
    }
  }

  if (persist_exists(PERSIST_KEY_NOON_REMINDER_DURATION)) {
    const int32_t stored_duration =
        persist_read_int(PERSIST_KEY_NOON_REMINDER_DURATION);
    if (noon_reminder_duration_is_valid(stored_duration)) {
      s_noon_reminder_duration =
          (AppNoonReminderDuration)stored_duration;
    }
  }
}

AppTextSize app_settings_get_text_size(void) {
  return s_text_size;
}

bool app_settings_set_text_size(AppTextSize text_size) {
  if (!text_size_is_valid(text_size)) {
    return false;
  }

  if (persist_write_int(PERSIST_KEY_TEXT_SIZE, text_size) < 0) {
    return false;
  }

  s_text_size = text_size;
  return true;
}

const char *app_settings_text_size_label(AppTextSize text_size) {
  switch (text_size) {
    case APP_TEXT_SIZE_LARGE:
      return "Large";
    case APP_TEXT_SIZE_EXTRA_LARGE:
      return "Extra Large";
    case APP_TEXT_SIZE_COUNT:
      break;
  }

  return "Large";
}

AppAccentColor app_settings_get_accent_color(void) {
  return s_accent_color;
}

bool app_settings_set_accent_color(AppAccentColor accent_color) {
  if (!accent_color_is_valid(accent_color)) {
    return false;
  }

  if (persist_write_int(PERSIST_KEY_ACCENT_COLOR, accent_color) < 0) {
    return false;
  }

  s_accent_color = accent_color;
  return true;
}

const char *app_settings_accent_color_label(AppAccentColor accent_color) {
  switch (accent_color) {
    case APP_ACCENT_COLOR_NAVY:
      return "Navy";
    case APP_ACCENT_COLOR_BURGUNDY:
      return "Burgundy";
    case APP_ACCENT_COLOR_FOREST:
      return "Forest";
    case APP_ACCENT_COLOR_MONOCHROME:
      return "Monochrome";
    case APP_ACCENT_COLOR_COUNT:
      break;
  }

  return "Navy";
}

AppAppearance app_settings_get_appearance(void) {
  return s_appearance;
}

bool app_settings_set_appearance(AppAppearance appearance) {
  if (!appearance_is_valid(appearance)) {
    return false;
  }

  if (persist_write_int(PERSIST_KEY_APPEARANCE, appearance) < 0) {
    return false;
  }

  s_appearance = appearance;
  return true;
}

const char *app_settings_appearance_label(AppAppearance appearance) {
  switch (appearance) {
    case APP_APPEARANCE_LIGHT:
      return "Light";
    case APP_APPEARANCE_DARK:
      return "Dark";
    case APP_APPEARANCE_COUNT:
      break;
  }

  return "Light";
}

bool app_settings_get_noon_reminder_enabled(void) {
  return s_noon_reminder_enabled;
}

bool app_settings_set_noon_reminder_enabled(bool enabled) {
  if (persist_write_bool(PERSIST_KEY_NOON_REMINDER_ENABLED, enabled) < 0) {
    return false;
  }

  s_noon_reminder_enabled = enabled;
  return true;
}

AppNoonReminderDuration app_settings_get_noon_reminder_duration(void) {
  return s_noon_reminder_duration;
}

bool app_settings_set_noon_reminder_duration(
    AppNoonReminderDuration duration) {
  if (!noon_reminder_duration_is_valid(duration)) {
    return false;
  }

  if (persist_write_int(PERSIST_KEY_NOON_REMINDER_DURATION, duration) < 0) {
    return false;
  }

  s_noon_reminder_duration = duration;
  return true;
}

uint32_t app_settings_get_noon_reminder_duration_seconds(void) {
  switch (s_noon_reminder_duration) {
    case APP_NOON_REMINDER_DURATION_5_SECONDS:
      return 5;
    case APP_NOON_REMINDER_DURATION_10_SECONDS:
      return 10;
    case APP_NOON_REMINDER_DURATION_30_SECONDS:
      return 30;
    case APP_NOON_REMINDER_DURATION_COUNT:
      break;
  }

  return 10;
}

const char *app_settings_noon_reminder_duration_label(
    AppNoonReminderDuration duration) {
  switch (duration) {
    case APP_NOON_REMINDER_DURATION_5_SECONDS:
      return "5 seconds";
    case APP_NOON_REMINDER_DURATION_10_SECONDS:
      return "10 seconds";
    case APP_NOON_REMINDER_DURATION_30_SECONDS:
      return "30 seconds";
    case APP_NOON_REMINDER_DURATION_COUNT:
      break;
  }

  return "10 seconds";
}
