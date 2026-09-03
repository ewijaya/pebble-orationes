#pragma once

#include <stdint.h>

enum { ROSARY_MYSTERIES_PER_SET = 5 };

typedef enum {
  ROSARY_MYSTERY_JOYFUL,
  ROSARY_MYSTERY_SORROWFUL,
  ROSARY_MYSTERY_GLORIOUS,
  ROSARY_MYSTERY_LUMINOUS,
} RosaryMysterySetId;

typedef struct {
  const char *compact_label;
  const char *today_label;
} RosaryMystery;

typedef struct {
  const char *name;
  const char *weekday_label;
  RosaryMystery mysteries[ROSARY_MYSTERIES_PER_SET];
} RosaryMysterySet;

uint8_t rosary_mystery_set_count(void);
const RosaryMysterySet *rosary_mystery_set_get(uint8_t index);
const RosaryMysterySet *rosary_mystery_set_for_weekday(int weekday);
const char *rosary_weekday_abbreviation(int weekday);
