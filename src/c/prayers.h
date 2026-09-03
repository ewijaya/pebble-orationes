#pragma once

#include <stdint.h>

typedef struct {
  const char *name;
} Prayer;

uint16_t prayers_count(void);
const Prayer *prayers_get(uint16_t index);
