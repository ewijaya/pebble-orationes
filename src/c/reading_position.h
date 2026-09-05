#pragma once
#include "main_menu_catalog.h"

enum { READING_POSITION_TOP, READING_POSITION_PLAIN, READING_POSITION_STYLED };
typedef struct {
  uint32_t anchor; // Plain text byte offset or styled paragraph index.
  uint16_t fraction; // Fraction of that paragraph, independent of font size.
  uint8_t kind;
  uint8_t entry;
} ReadingPosition;
bool reading_position_get(ReadingPosition *position);
bool reading_position_save(const ReadingPosition *position);
void reading_position_clear(void);
uint16_t reading_position_fraction(int32_t offset, int32_t start, int32_t end);
int32_t reading_position_offset(uint16_t fraction, int32_t start, int32_t end);
