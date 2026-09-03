#pragma once

#include <stdint.h>

typedef enum {
  PRAYER_LANGUAGE_ENGLISH,
  PRAYER_LANGUAGE_LATIN,
} PrayerLanguage;

typedef struct {
  PrayerLanguage language;
  const char *text;
} PrayerTranslation;

typedef struct {
  const char *name;
  const PrayerTranslation *translations;
  uint8_t translation_count;
} Prayer;

uint16_t prayers_count(void);
const Prayer *prayers_get(uint16_t index);
const PrayerTranslation *prayer_get_translation(const Prayer *prayer,
                                                PrayerLanguage language);
