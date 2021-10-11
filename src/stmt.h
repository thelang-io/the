/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_STMT_H
#define SRC_STMT_H

#include "expr.h"

#define FOREACH_STMT(fn) \
  fn(stmtEnd) \
  \
  fn(stmtCallExpr) \
  fn(stmtMain)

typedef struct stmt_s stmt_t;
typedef struct stmt_call_expr_s stmt_call_expr_t;
typedef struct stmt_main_s stmt_main_t;

typedef enum {
  #define GEN_TOKEN_ENUM(x) x,
  FOREACH_STMT(GEN_TOKEN_ENUM)
  #undef GEN_TOKEN_ENUM
} stmt_type_t;

struct stmt_s {
  stmt_type_t type;
  reader_location_t start;
  reader_location_t end;
};

struct stmt_call_expr_s {
  stmt_type_t type;
  reader_location_t start;
  reader_location_t end;
  expr_t **args;
  size_t args_len;
  token_t *callee;
};

struct stmt_main_s {
  stmt_type_t type;
  reader_location_t start;
  reader_location_t end;
  stmt_t **body;
  size_t body_len;
};

stmt_t *stmt_init (stmt_type_t type, reader_location_t start, reader_location_t end);
void stmt_free (stmt_t *this);

char *stmt_str (stmt_t *this, size_t indent);

#endif
