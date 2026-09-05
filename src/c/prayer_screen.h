#pragma once

#include "prayers.h"

void prayer_screen_init(void);
void prayer_screen_deinit(void);
void prayer_screen_show(const char *title, const char *text);
void prayer_screen_show_translation(const char *title,
                                     const PrayerTranslation *translation);
void prayer_screen_refresh(void);
