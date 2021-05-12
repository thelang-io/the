/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include "array.h"
#include "lexer.h"

typedef struct parser_s parser_t;

typedef enum {
  PARSER_ARGLIST,
  PARSER_CALL_EXPR,
  PARSER_EXPR,
  PARSER_ID,
  PARSER_LITERAL,
  PARSER_WS
} parser_token;

struct parser_s {
  parser_token tok;
};

void parser_free (parser_t *parser);
void parser_free_cb (void *it);
parser_t *parser_new (file_t *file);

#endif
