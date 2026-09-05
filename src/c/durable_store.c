#include "durable_store.h"
#include <pebble.h>
#include <string.h>

enum { HEADER_SIZE = 16, MAGIC = 0x4f524154 };
static uint32_t read_u32(const uint8_t *p) {
  return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
         (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static void write_u32(uint8_t *p, uint32_t value) {
  for (int i = 0; i < 4; ++i) p[i] = value >> (8 * i);
}
static uint32_t checksum(const uint8_t *record, size_t size) {
  uint32_t hash = 2166136261u;
  for (size_t i = 0; i < size; ++i) {
    if (i >= 12 && i < HEADER_SIZE) continue;
    hash = (hash ^ record[i]) * 16777619u;
  }
  return hash;
}
static bool read_record(uint32_t key, uint16_t schema, uint8_t *record, size_t size) {
  return persist_read_data(key, record, HEADER_SIZE + size) == (int)(HEADER_SIZE + size) &&
         read_u32(record) == MAGIC && read_u32(record + 4) == schema &&
         read_u32(record + 12) == checksum(record, HEADER_SIZE + size);
}
static int latest(uint32_t key, uint16_t schema, size_t size,
                  uint8_t *a, uint8_t *b) {
  const bool valid_a = read_record(key, schema, a, size);
  const bool valid_b = read_record(key + 1, schema, b, size);
  if (!valid_a) return valid_b ? 1 : -1;
  if (!valid_b) return 0;
  return (int32_t)(read_u32(b + 8) - read_u32(a + 8)) > 0 ? 1 : 0;
}
bool durable_store_read(uint32_t key, uint16_t schema, void *data, size_t size) {
  if (!data || size > PERSIST_DATA_MAX_LENGTH - HEADER_SIZE) return false;
  uint8_t a[PERSIST_DATA_MAX_LENGTH], b[PERSIST_DATA_MAX_LENGTH];
  const int bank = latest(key, schema, size, a, b);
  if (bank < 0) return false;
  memcpy(data, (bank ? b : a) + HEADER_SIZE, size);
  return true;
}
bool durable_store_write(uint32_t key, uint16_t schema, const void *data, size_t size) {
  if (!data || size > PERSIST_DATA_MAX_LENGTH - HEADER_SIZE) return false;
  uint8_t a[PERSIST_DATA_MAX_LENGTH], b[PERSIST_DATA_MAX_LENGTH];
  const int bank = latest(key, schema, size, a, b);
  const uint32_t generation = bank < 0 ? 0 : read_u32((bank ? b : a) + 8);
  uint8_t *record = bank == 0 ? b : a;
  const uint32_t target = key + (bank == 0 ? 1 : 0);
  write_u32(record, MAGIC);
  write_u32(record + 4, schema);
  write_u32(record + 8, generation + 1);
  memcpy(record + HEADER_SIZE, data, size);
  write_u32(record + 12, checksum(record, HEADER_SIZE + size));
  if (persist_write_data(target, record, HEADER_SIZE + size) != (int)(HEADER_SIZE + size)) {
    persist_delete(target);
    return false;
  }
  return true;
}
void durable_store_clear(uint32_t key) {
  persist_delete(key);
  persist_delete(key + 1);
}
