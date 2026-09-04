#pragma once

#include <stdbool.h>

// Returns true when the app was launched for a timely noon reminder.
bool noon_reminder_init(void);
void noon_reminder_deinit(void);

bool noon_reminder_set_enabled(bool enabled);
void noon_reminder_show(void);
