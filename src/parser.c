/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "parser/call-expr.h"
#include "parser/ws.h"
#include "parser.h"

void parser_free (parser_t *parser) {
  parser->tok == PARSER_CALL_EXPR && parser_call_expr_free(parser);
  parser->tok == PARSER_WS && parser_ws_free(parser);
}

void parser_free_cb (void *it) {
  parser_free((parser_t *) it);
}

// Make work like lexer or not, make it work in one way
parser_t *parser_new (file_t *file) {
  parser_t *parser = parser_ws_new(file, true);

  if (
    (parser = parser_ws_new(file, true)) != NULL ||
    (parser = parser_call_expr_new(file, true)) != NULL
  ) {
  }

  parser == NULL && (parser = parser_call_expr_new(file));

  return parser;
}
