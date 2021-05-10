/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ARRAY_H
#define SRC_ARRAY_H

#include <duc/common.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct array_s array_t;
typedef struct array_item_s array_item_t;

struct array_s {
  array_item_t *items;
  size_t size;
};

struct array_item_s {
  array_item_t *next;
  void *value;
};

void *array_at (const array_t *arr, size_t index);
bool array_empty (const array_t *arr);
void array_free (array_t *arr, duc_free_cb free_cb);
size_t array_length (const array_t *arr);
array_t *array_new ();
void array_push (array_t *arr, void *value);
void *array_shift (array_t *arr);

#endif
