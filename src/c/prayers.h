#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  PRAYER_LANGUAGE_ENGLISH,
  PRAYER_LANGUAGE_LATIN,
} PrayerLanguage;

typedef enum {
  PRAYER_PARAGRAPH_PRIMARY,
  PRAYER_PARAGRAPH_LATIN,
  PRAYER_PARAGRAPH_TRANSLATION,
  PRAYER_PARAGRAPH_REFERENCE,
  PRAYER_PARAGRAPH_NOTE,
} PrayerParagraphStyle;

typedef struct {
  const char *text;
  // PrayerParagraphStyle, stored compactly for long collections.
  uint8_t style;
  bool space_after;
} PrayerParagraph;

typedef struct {
  PrayerLanguage language;
  const char *text;
  // Optional styled content; when present, used instead of the plain string.
  const PrayerParagraph *paragraphs;
  uint16_t paragraph_count;
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
  PRAYER_ID_MEMORARE,
  PRAYER_ID_PSALM_50,
  PRAYER_ID_ACCEPTANCE_OF_DEATH,
  PRAYER_ID_PRAYER_FOR_VOCATIONS,
  PRAYER_ID_BLESSED_BE_YOUR_PURITY,
  PRAYER_ID_CANTICLE_THREE_CHILDREN,
  PRAYER_ID_PSALM_2,
  PRAYER_ID_ASPIRATIONS,
  PRAYER_ID_COUNT,
} PrayerId;

uint16_t prayers_count(void);
const Prayer *prayers_get(uint16_t index);
const Prayer *prayers_get_by_id(PrayerId prayer_id);
const PrayerTranslation *prayer_get_translation(const Prayer *prayer,
                                                PrayerLanguage language);
