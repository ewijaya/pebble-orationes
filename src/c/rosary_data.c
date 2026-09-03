#include "rosary_data.h"

#include <stddef.h>

#define MYSTERY(label) {.compact_label = label, .today_label = label}

static const RosaryMysterySet s_mystery_sets[] = {
    {
        .name = "Joyful",
        .weekday_label = "Mon/Sat",
        .mysteries = {
            MYSTERY("Annunciation"),
            MYSTERY("Visitation"),
            MYSTERY("Nativity"),
            MYSTERY("Presentation"),
            MYSTERY("Finding in the Temple"),
        },
    },
    {
        .name = "Sorrowful",
        .weekday_label = "Tue/Fri",
        .mysteries = {
            MYSTERY("Agony in the Garden"),
            MYSTERY("Scourging at the Pillar"),
            MYSTERY("Crowning with Thorns"),
            MYSTERY("Carrying of the Cross"),
            MYSTERY("Crucifixion"),
        },
    },
    {
        .name = "Glorious",
        .weekday_label = "Wed/Sun",
        .mysteries = {
            MYSTERY("Resurrection"),
            MYSTERY("Ascension"),
            MYSTERY("Descent of Holy Spirit"),
            MYSTERY("Assumption of Mary"),
            MYSTERY("Coronation of Mary"),
        },
    },
    {
        .name = "Luminous",
        .weekday_label = "Thu",
        .mysteries = {
            MYSTERY("Baptism in the Jordan"),
            MYSTERY("Wedding at Cana"),
            {
                .compact_label = "Proclamation of Kingdom",
                .today_label = "Proclamation of the Kingdom",
            },
            MYSTERY("Transfiguration"),
            {
                .compact_label = "Institution of Eucharist",
                .today_label = "Institution of the Eucharist",
            },
        },
    },
};

#undef MYSTERY

static const RosaryMysterySetId s_weekday_mystery_sets[] = {
    ROSARY_MYSTERY_GLORIOUS,
    ROSARY_MYSTERY_JOYFUL,
    ROSARY_MYSTERY_SORROWFUL,
    ROSARY_MYSTERY_GLORIOUS,
    ROSARY_MYSTERY_LUMINOUS,
    ROSARY_MYSTERY_SORROWFUL,
    ROSARY_MYSTERY_JOYFUL,
};

static const char *const s_weekday_abbreviations[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

uint8_t rosary_mystery_set_count(void) {
  return (uint8_t)(sizeof(s_mystery_sets) / sizeof(s_mystery_sets[0]));
}

const RosaryMysterySet *rosary_mystery_set_get(uint8_t index) {
  if (index >= rosary_mystery_set_count()) {
    return NULL;
  }

  return &s_mystery_sets[index];
}

const RosaryMysterySet *rosary_mystery_set_for_weekday(int weekday) {
  if (weekday < 0 || weekday >= (int)(sizeof(s_weekday_mystery_sets) /
                                      sizeof(s_weekday_mystery_sets[0]))) {
    return NULL;
  }

  return rosary_mystery_set_get(s_weekday_mystery_sets[weekday]);
}

const char *rosary_weekday_abbreviation(int weekday) {
  if (weekday < 0 || weekday >= (int)(sizeof(s_weekday_abbreviations) /
                                      sizeof(s_weekday_abbreviations[0]))) {
    return "";
  }

  return s_weekday_abbreviations[weekday];
}
