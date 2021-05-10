/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_H
#define SRC_AST_H

#include "parser.h"

typedef struct ast_s ast_t;

struct ast_s {
  array_t *parsers;
};

void ast_free (ast_t *ast);
ast_t *ast_new (file_t *file);

#endif
