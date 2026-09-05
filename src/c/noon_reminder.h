#pragma once

#include "app_settings.h"

#include <stdbool.h>

// Returns true when the app was launched for a timely noon reminder.
bool noon_reminder_init(void);
void noon_reminder_deinit(void);

bool noon_reminder_set_enabled(bool enabled);
bool noon_reminder_apply_settings(const AppSettings *settings);
void noon_reminder_show(void);
