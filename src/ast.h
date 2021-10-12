/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_H
#define SRC_AST_H

#include "stmt.h"

typedef struct ast_s ast_t;

struct ast_s {
  bool main_present;
  stmt_t **main_body;
  size_t main_body_len;
};

ast_t *ast_init ();
void ast_free (ast_t *this);

void ast_add (ast_t *this, stmt_t *stmt);

#endif
