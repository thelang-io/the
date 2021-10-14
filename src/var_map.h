/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_MAP_H
#define SRC_VAR_MAP_H

#include "expr.h"

typedef struct var_map_s var_map_t;
typedef struct var_map_item_s var_map_item_t;

typedef enum {
  varUnknown,
  varChar,
  varFloat,
  varIntDec,
  varStr
} var_map_item_type_t;

struct var_map_s {
  var_map_item_t **items;
  size_t items_len;
};

struct var_map_item_s {
  var_map_item_type_t type;
  char *name;
  expr_t *val;
};

var_map_t *var_map_init ();
void var_map_free (var_map_t *this);

void var_map_add (var_map_t *this, var_map_item_type_t type, const char *name, expr_t *val);
const var_map_item_t *var_map_get (var_map_t *this, const char *name);

#endif
