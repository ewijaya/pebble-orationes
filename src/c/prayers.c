#include "prayers.h"

#include <stddef.h>

static const Prayer s_prayers[] = {
    {.name = "Angelus"},
    {.name = "Regina Caeli"},
    {.name = "Holy Rosary"},
    {.name = "Preces"},
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
