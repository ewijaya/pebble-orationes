#pragma once

#include "prayers.h"
#include "main_menu_catalog.h"

void prayer_screen_init(void);
void prayer_screen_deinit(void);
void prayer_screen_show(const char *title, const char *text);
void prayer_screen_show_translation(const char *title,
                                     const PrayerTranslation *translation);
void prayer_screen_refresh(void);
void prayer_screen_show_entry(MainMenuEntryId entry_id, const char *title,
                              const PrayerTranslation *translation, bool resume);
