/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "error.h"
#include "expr.h"

expr_t *expr_init (expr_type_t type, reader_location_t start, reader_location_t end) {
  expr_t *this;

  if (type == exprLiteral) {
    this = malloc(sizeof(expr_literal_t));
  } else {
    this = malloc(sizeof(expr_t));
  }

  if (this == NULL) {
    throw_error("Unable to allocate memory for expr");
  }

  this->type = type;
  this->start = start;
  this->end = end;

  return this;
}

void expr_free (expr_t *this) {
  if (this->type == exprLiteral) {
    expr_literal_t *literal_expr = (expr_literal_t *) this;
    token_free(literal_expr->tok);
  }

  free(this);
}
