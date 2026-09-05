#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#define PERSIST_DATA_MAX_LENGTH 256
bool persist_exists(uint32_t key);
int32_t persist_read_int(uint32_t key);
bool persist_read_bool(uint32_t key);
int persist_write_int(uint32_t key, int32_t value);
int persist_write_bool(uint32_t key, bool value);
int persist_read_data(uint32_t key, void *buffer, size_t size);
int persist_write_data(uint32_t key, const void *buffer, size_t size);
int persist_delete(uint32_t key);

typedef enum { TUPLE_BYTE_ARRAY, TUPLE_CSTRING, TUPLE_UINT, TUPLE_INT } TupleType;
typedef union { uint8_t uint8; uint16_t uint16; uint32_t uint32; int32_t int32; } TupleValue;
typedef struct { uint32_t key; TupleType type; uint16_t length; TupleValue *value; } Tuple;
typedef struct { Tuple tuples[32]; TupleValue values[32]; unsigned count; } DictionaryIterator;
typedef int AppMessageResult;
#define APP_MSG_OK 0
#define APP_LOG(...) ((void)0)
Tuple *dict_find(DictionaryIterator *iterator, uint32_t key);
void dict_write_uint8(DictionaryIterator *iterator, uint32_t key, uint8_t value);
void dict_write_uint32(DictionaryIterator *iterator, uint32_t key, uint32_t value);
AppMessageResult app_message_outbox_begin(DictionaryIterator **iterator);
AppMessageResult app_message_outbox_send(void);
AppMessageResult app_message_open(uint32_t inbox, uint32_t outbox);
void app_message_register_inbox_received(void (*handler)(DictionaryIterator *, void *));
void app_message_register_inbox_dropped(void (*handler)(AppMessageResult, void *));
void app_message_deregister_callbacks(void);
