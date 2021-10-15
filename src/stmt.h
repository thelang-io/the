/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_STMT_H
#define SRC_STMT_H

#include "expr.h"

typedef struct stmt_s stmt_t;
typedef struct stmt_call_expr_s stmt_call_expr_t;
typedef struct stmt_main_s stmt_main_t;
typedef struct stmt_short_var_decl_s stmt_short_var_decl_t;

typedef enum {
  stmtEnd,

  stmtCallExpr,
  stmtMain,
  stmtShortVarDecl
} stmt_type_t;

struct stmt_s {
  stmt_type_t type;
  reader_location_t start;
  reader_location_t end;

  union {
    stmt_call_expr_t *call_expr;
    stmt_main_t *main;
    stmt_short_var_decl_t *short_var_decl;
  };
};

struct stmt_call_expr_s {
  expr_t **args;
  size_t args_len;
  token_t *callee;
};

struct stmt_main_s {
  stmt_t **body;
  size_t body_len;
};

struct stmt_short_var_decl_s {
  token_t *id;
  expr_t *init;
  bool mut;
};

stmt_t *stmt_init (stmt_type_t type, reader_location_t start, reader_location_t end);
void stmt_free (stmt_t *this);

#endif
