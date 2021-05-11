/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_EXPR_H
#define SRC_PARSER_EXPR_H

#include "id.h"
#include "literal.h"

typedef struct parser_expr_s parser_expr_t;

struct parser_expr_s {
  parser_token tok;
  parser_t *parser;
};

void parser_expr_free (parser_expr_t *parser);
void parser_expr_free_cb (void *it);
parser_t *parser_expr_new (file_t *file);

#endif
