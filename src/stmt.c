/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "error.h"
#include "stmt.h"

stmt_t *stmt_init (stmt_type_t type, reader_location_t start, reader_location_t end) {
  stmt_t *this;

  if (type == stmtCallExpr) {
    this = malloc(sizeof(stmt_call_expr_t));
  } else if (type == stmtMain) {
    this = malloc(sizeof(stmt_main_t));
  } else if (type == stmtShortVarDecl) {
    this = malloc(sizeof(stmt_short_var_decl_t));
  } else {
    this = malloc(sizeof(stmt_t));
  }

  if (this == NULL) {
    throw_error("Unable to allocate memory for stmt");
  }

  this->type = type;
  this->start = start;
  this->end = end;

  return this;
}

void stmt_free (stmt_t *this) {
  if (this->type == stmtCallExpr) {
    stmt_call_expr_t *stmt_call_expr = (stmt_call_expr_t *) this;

    for (size_t i = 0; i < stmt_call_expr->args_len; i++) {
      expr_free(stmt_call_expr->args[i]);
    }

    token_free(stmt_call_expr->callee);
    free(stmt_call_expr->args);
  } else if (this->type == stmtShortVarDecl) {
    stmt_short_var_decl_t *stmt_short_var_decl = (stmt_short_var_decl_t *) this;
    token_free(stmt_short_var_decl->id);
    expr_free(stmt_short_var_decl->init);
  }

  free(this);
}
