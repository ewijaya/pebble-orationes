#pragma once

#include <stdint.h>

typedef void (*SettingsShortcutSavedHandler)(uint8_t slot_index);

void settings_menu_init(SettingsShortcutSavedHandler shortcut_saved_handler);
void settings_menu_deinit(void);
void settings_menu_show(void);
void settings_menu_refresh(void);
