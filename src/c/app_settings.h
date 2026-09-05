#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "main_menu_catalog.h"

enum {
  APP_MAIN_MENU_SLOT_COUNT = 7,
};

typedef enum {
  APP_TEXT_SIZE_LARGE,
  APP_TEXT_SIZE_EXTRA_LARGE,
  APP_TEXT_SIZE_COUNT,
} AppTextSize;

typedef enum {
  APP_ACCENT_COLOR_OCEAN,
  APP_ACCENT_COLOR_BURGUNDY,
  APP_ACCENT_COLOR_FOREST,
  APP_ACCENT_COLOR_MONOCHROME,
  APP_ACCENT_COLOR_COUNT,
} AppAccentColor;

typedef enum {
  APP_NAVIGATION_CLASSIC,
  APP_NAVIGATION_AMBER,
  APP_NAVIGATION_TANGERINE,
  APP_NAVIGATION_VIOLET,
  APP_NAVIGATION_MAGENTA,
  APP_NAVIGATION_LIME,
  APP_NAVIGATION_COUNT,
} AppNavigationHighlight;

typedef enum {
  APP_APPEARANCE_LIGHT,
  APP_APPEARANCE_DARK,
  APP_APPEARANCE_COUNT,
} AppAppearance;

typedef enum {
  APP_NOON_REMINDER_DURATION_5_SECONDS,
  APP_NOON_REMINDER_DURATION_10_SECONDS,
  APP_NOON_REMINDER_DURATION_30_SECONDS,
  APP_NOON_REMINDER_DURATION_COUNT,
} AppNoonReminderDuration;

typedef struct {
  uint8_t text_size;
  uint8_t accent_color;
  uint8_t appearance;
  uint8_t noon_reminder_enabled;
  uint8_t noon_reminder_duration;
  uint8_t remember_place;
  uint8_t slots[APP_MAIN_MENU_SLOT_COUNT];
  uint8_t navigation_highlight;
} AppSettings;

typedef void (*AppSettingsChangedHandler)(void);
void app_settings_set_changed_handler(AppSettingsChangedHandler handler);
AppSettings app_settings_get(void);
bool app_settings_validate(const AppSettings *settings);
bool app_settings_apply(const AppSettings *settings);
bool app_settings_get_remember_place(void);
bool app_settings_set_remember_place(bool enabled);

void app_settings_init(void);
AppTextSize app_settings_get_text_size(void);
bool app_settings_set_text_size(AppTextSize text_size);
const char *app_settings_text_size_label(AppTextSize text_size);
AppAccentColor app_settings_get_accent_color(void);
bool app_settings_set_accent_color(AppAccentColor accent_color);
const char *app_settings_accent_color_label(AppAccentColor accent_color);
AppAppearance app_settings_get_appearance(void);
bool app_settings_set_appearance(AppAppearance appearance);
const char *app_settings_appearance_label(AppAppearance appearance);
bool app_settings_get_noon_reminder_enabled(void);
bool app_settings_set_noon_reminder_enabled(bool enabled);
MainMenuEntryId app_settings_get_main_menu_slot(uint8_t slot_index);
bool app_settings_set_main_menu_slot(uint8_t slot_index,
                                     MainMenuEntryId entry_id);
bool app_settings_set_main_menu_slots(
    const MainMenuEntryId slots[APP_MAIN_MENU_SLOT_COUNT]);
bool app_settings_restore_main_menu_defaults(void);
AppNoonReminderDuration app_settings_get_noon_reminder_duration(void);
bool app_settings_set_noon_reminder_duration(
    AppNoonReminderDuration duration);
uint32_t app_settings_get_noon_reminder_duration_seconds(void);
const char *app_settings_noon_reminder_duration_label(
    AppNoonReminderDuration duration);

AppNavigationHighlight app_settings_get_navigation_highlight(void);
bool app_settings_set_navigation_highlight(AppNavigationHighlight value);
const char *
app_settings_navigation_highlight_label(AppNavigationHighlight value);
