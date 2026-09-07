#pragma once

#include "prayers.h"

// Bilingual reading: original wording followed immediately by its translation.
#ifdef PBL_PLATFORM_EMERY
extern PrayerTranslation aspirations_translations[1];
bool aspirations_load(void);
#else
extern const PrayerTranslation aspirations_translations[1];
#endif
