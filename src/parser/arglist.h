/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_ARGLIST_H
#define SRC_PARSER_ARGLIST_H

#include "expr.h"

typedef struct parser_arglist_s parser_arglist_t;

struct parser_arglist_s {
  parser_token tok;
  array_t *exprs;
};

void parser_arglist_free (parser_arglist_t *parser);
parser_arglist_t *parser_arglist_new (file_t *file);

#endif
