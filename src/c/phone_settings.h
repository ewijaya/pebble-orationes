#pragma once

typedef void (*PhoneSettingsChangedHandler)(void);

void phone_settings_init(PhoneSettingsChangedHandler changed_handler);
void phone_settings_deinit(void);
void phone_settings_send_current(void);
