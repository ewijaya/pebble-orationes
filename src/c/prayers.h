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

typedef enum {
  PRAYER_DESTINATION_TEXT,
  PRAYER_DESTINATION_ROSARY,
} PrayerDestination;

typedef struct {
  const char *name;
  PrayerDestination destination;
  PrayerLanguage default_language;
  const PrayerTranslation *translations;
  uint8_t translation_count;
} Prayer;

typedef enum {
  PRAYER_ID_PRECES,
  PRAYER_ID_HOLY_ROSARY,
  PRAYER_ID_REGINA_CAELI,
  PRAYER_ID_ANGELUS,
  PRAYER_ID_COUNT,
} PrayerId;

uint16_t prayers_count(void);
const Prayer *prayers_get(uint16_t index);
const Prayer *prayers_get_by_id(PrayerId prayer_id);
const PrayerTranslation *prayer_get_translation(const Prayer *prayer,
                                                PrayerLanguage language);
