#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Two adjacent keys hold alternating checksummed records. A failed or torn
// write never replaces the last valid record. Each payload is at most 240 bytes.
bool durable_store_read(uint32_t key, uint16_t schema, void *data, size_t size);
bool durable_store_write(uint32_t key, uint16_t schema, const void *data, size_t size);
void durable_store_clear(uint32_t key);
