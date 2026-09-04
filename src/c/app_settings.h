#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  APP_TEXT_SIZE_LARGE,
  APP_TEXT_SIZE_EXTRA_LARGE,
  APP_TEXT_SIZE_COUNT,
} AppTextSize;

typedef enum {
  APP_NOON_REMINDER_DURATION_5_SECONDS,
  APP_NOON_REMINDER_DURATION_10_SECONDS,
  APP_NOON_REMINDER_DURATION_30_SECONDS,
  APP_NOON_REMINDER_DURATION_COUNT,
} AppNoonReminderDuration;

void app_settings_init(void);
AppTextSize app_settings_get_text_size(void);
bool app_settings_set_text_size(AppTextSize text_size);
const char *app_settings_text_size_label(AppTextSize text_size);
bool app_settings_get_noon_reminder_enabled(void);
bool app_settings_set_noon_reminder_enabled(bool enabled);
AppNoonReminderDuration app_settings_get_noon_reminder_duration(void);
bool app_settings_set_noon_reminder_duration(
    AppNoonReminderDuration duration);
uint32_t app_settings_get_noon_reminder_duration_seconds(void);
const char *app_settings_noon_reminder_duration_label(
    AppNoonReminderDuration duration);
