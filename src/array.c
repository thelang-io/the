/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "array.h"

void *array_at (const array_t *arr, size_t index) {
  if (index >= arr->size) {
    return NULL;
  }

  array_item_t *it = arr->items;

  for (size_t i = 0; i < index; i++) {
    it = it->next;
  }

  return it->value;
}

bool array_empty (const array_t *arr) {
  return arr->size == 0;
}

void array_free (array_t *arr, free_cb cb) {
  while (arr->items != NULL) {
    array_item_t *next = arr->items->next;

    cb(arr->items->value);
    free(arr->items);
    arr->items = next;
  }

  free(arr);
}

size_t array_length (const array_t *arr) {
  return arr->size;
}

array_t *array_new () {
  array_t *arr = malloc(sizeof(array_t));

  if (arr == NULL) {
    return NULL;
  }

  arr->items = NULL;
  arr->size = 0;

  return arr;
}

void array_push (array_t *arr, void *value) {
  array_item_t *it = arr->items;

  while (it != NULL && it->next != NULL) {
    it = it->next;
  }

  array_item_t *in = malloc(sizeof(array_item_t));

  if (in == NULL) {
    return;
  }

  in->next = NULL;
  in->value = value;

  if (it == NULL) {
    arr->items = in;
  } else {
    it->next = in;
  }

  arr->size += 1;
}

void *array_shift (array_t *arr) {
  if (array_empty(arr)) {
    return NULL;
  }

  array_item_t *next = arr->items->next;
  void *value = arr->items->value;

  free(arr->items);
  arr->items = next;
  arr->size -= 1;

  return value;
}
