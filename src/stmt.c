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
  stmt_t *this = malloc(sizeof(stmt_t));

  if (this == NULL) {
    throw_error("Unable to allocate memory for stmt");
  }

  this->type = type;
  this->start = start;
  this->end = end;

  if (type == stmtCallExpr) {
    this->call_expr = malloc(sizeof(stmt_call_expr_t));

    if (this->call_expr == NULL) {
      throw_error("Unable to allocate memory for stmt call expr");
    }
  } else if (type == stmtMain) {
    this->main = malloc(sizeof(stmt_main_t));

    if (this->call_expr == NULL) {
      throw_error("Unable to allocate memory for stmt main");
    }
  } else if (type == stmtShortVarDecl) {
    this->short_var_decl = malloc(sizeof(stmt_short_var_decl_t));

    if (this->call_expr == NULL) {
      throw_error("Unable to allocate memory for stmt short var decl");
    }
  }

  return this;
}

void stmt_free (stmt_t *this) {
  if (this->type == stmtCallExpr) {
    for (size_t i = 0; i < this->call_expr->args_len; i++) {
      expr_free(this->call_expr->args[i]);
    }

    token_free(this->call_expr->callee);
    free(this->call_expr->args);
    free(this->call_expr);
  } else if (this->type == stmtMain) {
    free(this->main);
  } else if (this->type == stmtShortVarDecl) {
    token_free(this->short_var_decl->id);
    expr_free(this->short_var_decl->init);
    free(this->short_var_decl);
  }

  free(this);
}
