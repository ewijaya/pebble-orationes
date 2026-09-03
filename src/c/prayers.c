#include "prayers.h"

#include <stddef.h>

static const char s_angelus_english[] =
    "V. The Angel of the Lord declared unto Mary.\n"
    "R. And she conceived of the Holy Spirit.\n\n"
    "Hail Mary ...\n\n"
    "V. Behold the handmaid of the Lord.\n"
    "R. Be it done to me according to thy word.\n\n"
    "Hail Mary ...\n\n"
    "V. The Word was made flesh.\n"
    "R. And dwelt among us.\n\n"
    "Hail Mary ...\n\n"
    "V. Pray for us, O Holy Mother of God.\n"
    "R. That we may be made worthy of the promises of Christ.\n\n"
    "Let us pray.\n\n"
    "Pour forth, we beseech you, O Lord,\n"
    "your grace into our hearts,\n"
    "that we, to whom the Incarnation of Christ your Son\n"
    "was made known by the message of an angel,\n"
    "may by his passion and cross\n"
    "be brought to the glory of his resurrection.\n"
    "Through Christ our Lord.\n"
    "Amen.";

static const PrayerTranslation s_angelus_translations[] = {
    {
        .language = PRAYER_LANGUAGE_ENGLISH,
        .text = s_angelus_english,
    },
};

static const Prayer s_prayers[] = {
    {
        .name = "Angelus",
        .translations = s_angelus_translations,
        .translation_count =
            (uint8_t)(sizeof(s_angelus_translations) /
                      sizeof(s_angelus_translations[0])),
    },
    {
        .name = "Regina Caeli",
        .translations = NULL,
        .translation_count = 0,
    },
    {
        .name = "Holy Rosary",
        .translations = NULL,
        .translation_count = 0,
    },
    {
        .name = "Preces",
        .translations = NULL,
        .translation_count = 0,
    },
};

uint16_t prayers_count(void) {
  return (uint16_t)(sizeof(s_prayers) / sizeof(s_prayers[0]));
}

const Prayer *prayers_get(uint16_t index) {
  if (index >= prayers_count()) {
    return NULL;
  }

  return &s_prayers[index];
}

const PrayerTranslation *prayer_get_translation(const Prayer *prayer,
                                                PrayerLanguage language) {
  if (!prayer) {
    return NULL;
  }

  for (uint8_t index = 0; index < prayer->translation_count; ++index) {
    const PrayerTranslation *translation = &prayer->translations[index];
    if (translation->language == language) {
      return translation;
    }
  }

  return NULL;
}
