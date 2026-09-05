#pragma once

#include <stdint.h>

#include "prayers.h"

typedef enum {
  PRAYER_COLLECTION_DAILY,
  PRAYER_COLLECTION_CONFESSION,
  PRAYER_COLLECTION_CARDS,
  PRAYER_COLLECTION_COUNT,
} PrayerCollectionId;

typedef struct {
  const char *name;
  const Prayer *prayers;
  uint8_t prayer_count;
} PrayerCollection;

const PrayerCollection *prayer_collections_get(
    PrayerCollectionId collection_id);
