/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_EXPR_H
#define SRC_EXPR_H

#include "token.h"

typedef struct expr_s expr_t;
typedef struct expr_literal_s expr_literal_t;

typedef enum {
  exprLiteral
} expr_type_t;

struct expr_s {
  expr_type_t type;
  reader_location_t start;
  reader_location_t end;
};

struct expr_literal_s {
  expr_type_t type;
  reader_location_t start;
  reader_location_t end;
  token_t *tok;
};

expr_t *expr_init (expr_type_t type, reader_location_t start, reader_location_t end);
void expr_free (expr_t *this);

#endif
