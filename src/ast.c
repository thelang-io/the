/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "ast.h"
#include "error.h"

ast_t *ast_init () {
  ast_t *this = malloc(sizeof(ast_t));

  if (this == NULL) {
    throw_error("Unable to allocate memory for ast");
  }

  this->main_present = false;
  this->main_body = NULL;
  this->main_body_len = 0;

  return this;
}

void ast_free (ast_t *this) {
  if (this->main_body_len > 0) {
    for (size_t i = 0; i < this->main_body_len; i++) {
      stmt_free(this->main_body[i]);
    }

    free(this->main_body);
  }

  free(this);
}

void ast_add (ast_t *this, stmt_t *stmt) {
  if (stmt->type == stmtMain) {
    stmt_main_t *main_stmt = (stmt_main_t *) stmt;

    this->main_present = true;
    this->main_body = main_stmt->body;
    this->main_body_len = main_stmt->body_len;
  }
}
