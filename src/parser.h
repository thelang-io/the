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

#define FOREACH_PARSER_TOKEN(f) \
  f(ARGLIST) \
  f(CALL_EXPR) \
  f(EXPR) \
  f(ID) \
  f(LITERAL) \
  f(WS)

#define GEN_PARSER_TOKEN_ENUM(x) PARSER_##x,

typedef struct parser_s parser_t;

typedef enum {
  FOREACH_PARSER_TOKEN(GEN_PARSER_TOKEN_ENUM)
} parser_token;

struct parser_s {
  parser_token tok;
};

extern const char *parser_token_str[];

void parser_free (parser_t *parser);
void parser_free_cb (void *it);
parser_t *parser_new (file_t *file);

#endif
