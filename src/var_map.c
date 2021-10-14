/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "var_map.h"

var_map_t *var_map_init () {
  var_map_t *this = malloc(sizeof(var_map_t));

  if (this == NULL) {
    throw_error("Unable to allocate memory for var map");
  }

  this->items = NULL;
  this->items_len = 0;

  return this;
}

void var_map_free (var_map_t *this) {
  for (size_t i = 0; i < this->items_len; i++) {
    var_map_item_t *it = this->items[i];
    free(it->name);
    free(it);
  }

  free(this->items);
  free(this);
}

void var_map_add (var_map_t *this, var_map_item_type_t type, const char *name, expr_t *val) {
  if (this->items_len == 0) {
    this->items = malloc(++this->items_len * sizeof(var_map_item_t));
  } else {
    this->items = realloc(this->items, ++this->items_len * sizeof(var_map_item_t));
  }

  if (this->items == NULL) {
    throw_error("Unable to re-allocate memory for var map");
  }

  var_map_item_t *it = malloc(sizeof(var_map_item_t));

  if (it == NULL) {
    throw_error("Unable to allocate memory for var map item");
  }

  it->type = type;
  it->name = malloc(strlen(name) + 1);
  it->val = val;

  if (it->name == NULL) {
    throw_error("Unable to allocate memory for var map item name");
  }

  strcpy(it->name, name);
  this->items[this->items_len - 1] = it;
}

const var_map_item_t *var_map_get (var_map_t *this, const char *name) {
  for (size_t i = 0; i < this->items_len; i++) {
    if (strcmp(this->items[i]->name, name) == 0) {
      return this->items[i];
    }
  }

  return NULL;
}
