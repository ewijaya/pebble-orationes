#include "pebble.h"
#include "storage.h"
#include <assert.h>
#include <string.h>
static unsigned char data[128][256];
static size_t lengths[128];
static int fail_bytes = -1;
void storage_reset(void) {
  memset(data, 0, sizeof(data));
  memset(lengths, 0, sizeof(lengths));
  fail_bytes = -1;
}
void storage_fail_next_write(int bytes) { fail_bytes = bytes; }
void storage_corrupt(unsigned key) { data[key][0] ^= 0xff; }
bool persist_exists(uint32_t key) { return lengths[key] != 0; }
int persist_read_data(uint32_t key, void *buffer, size_t size) {
  assert(key < 128);
  if (!lengths[key]) return -1;
  size_t count = lengths[key] < size ? lengths[key] : size;
  memcpy(buffer, data[key], count);
  return (int)count;
}
int persist_write_data(uint32_t key, const void *buffer, size_t size) {
  assert(key < 128 && size <= 256);
  if (fail_bytes >= 0) {
    size_t count = (size_t)fail_bytes < size ? (size_t)fail_bytes : size;
    fail_bytes = -1;
    if (count) {
      memcpy(data[key], buffer, count);
      lengths[key] = count;
    }
    return -1;
  }
  memcpy(data[key], buffer, size);
  lengths[key] = size;
  return (int)size;
}
int32_t persist_read_int(uint32_t key) {
  int32_t value = 0;
  persist_read_data(key, &value, sizeof(value));
  return value;
}
bool persist_read_bool(uint32_t key) { return persist_read_int(key) != 0; }
int persist_write_int(uint32_t key, int32_t value) {
  return persist_write_data(key, &value, sizeof(value));
}
int persist_write_bool(uint32_t key, bool value) {
  return persist_write_int(key, value);
}
int persist_delete(uint32_t key) { lengths[key] = 0; return 0; }
