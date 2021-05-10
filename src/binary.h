/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_BINARY_H
#define SRC_BINARY_H

#include <duc/helpers.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define binary_append_int8(binary, ...) \
  binary_append_int8_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_int16(binary, ...) \
  binary_append_int16_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_int32(binary, ...) \
  binary_append_int32_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_int64(binary, ...) \
  binary_append_int64_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_uint8(binary, ...) \
  binary_append_uint8_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_uint16(binary, ...) \
  binary_append_uint16_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_uint32(binary, ...) \
  binary_append_uint32_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

#define binary_append_uint64(binary, ...) \
  binary_append_uint64_(binary, DUC_NARG(__VA_ARGS__), __VA_ARGS__)

typedef struct binary_s binary_t;

struct binary_s {
  uint8_t *data;
  size_t size;
};

void binary_append_binary (binary_t *dest, const binary_t *src);
void binary_append_data (binary_t *binary, const void *data, size_t size);
void binary_append_string (binary_t *binary, const char *string);
void binary_append_times (binary_t *binary, unsigned char ch, size_t times);
uint8_t binary_at (const binary_t *binary, size_t index);
const uint8_t *binary_data (const binary_t *binary);
bool binary_empty (const binary_t *binary);
void binary_free (binary_t *binary);
binary_t *binary_new ();
size_t binary_size (const binary_t *binary);
void binary_write (const binary_t *binary, const char *filepath);

void binary_append_int8_ (binary_t *binary, size_t n, ...);
void binary_append_int16_ (binary_t *binary, size_t n, ...);
void binary_append_int32_ (binary_t *binary, size_t n, ...);
void binary_append_int64_ (binary_t *binary, size_t n, ...);
void binary_append_uint8_ (binary_t *binary, size_t n, ...);
void binary_append_uint16_ (binary_t *binary, size_t n, ...);
void binary_append_uint32_ (binary_t *binary, size_t n, ...);
void binary_append_uint64_ (binary_t *binary, size_t n, ...);

#endif
