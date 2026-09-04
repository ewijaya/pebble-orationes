#include "app_settings.h"

#include <pebble.h>

enum {
  PERSIST_KEY_TEXT_SIZE = 1,
  PERSIST_KEY_NOON_REMINDER_ENABLED = 2,
  PERSIST_KEY_NOON_REMINDER_DURATION = 3,
};

static AppTextSize s_text_size = APP_TEXT_SIZE_LARGE;
static bool s_noon_reminder_enabled;
static AppNoonReminderDuration s_noon_reminder_duration =
    APP_NOON_REMINDER_DURATION_10_SECONDS;

static bool text_size_is_valid(int32_t value) {
  return value >= APP_TEXT_SIZE_LARGE && value < APP_TEXT_SIZE_COUNT;
}

static bool noon_reminder_duration_is_valid(int32_t value) {
  return value >= APP_NOON_REMINDER_DURATION_5_SECONDS &&
         value < APP_NOON_REMINDER_DURATION_COUNT;
}

void app_settings_init(void) {
  s_text_size = APP_TEXT_SIZE_LARGE;
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
