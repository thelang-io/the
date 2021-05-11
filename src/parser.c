/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "parser.h"

void parser_free (parser_t *parser) {
  if (parser->tok == PARSER_CALL_EXPR && parser->call_expr != NULL) {
    parser_call_expr_free_(parser->call_expr);
  } else if (parser->tok == PARSER_WS && parser->ws != NULL) {
    parser_ws_free_(parser->ws);
  }

  free(parser);
}

void parser_free_cb (void *it) {
  parser_free((parser_t *) it);
}

parser_t *parser_new (file_t *file) {
  parser_t *parser = malloc(sizeof(parser_t));

  if ((parser->ws = parser_ws_new_(file, true)) != NULL) {
    parser->tok = PARSER_WS;
  } else if ((parser->call_expr = parser_call_expr_new_(file)) != NULL) {
    parser->tok = PARSER_CALL_EXPR;
  } else {
    parser->call_expr = NULL;
    parser->tok = PARSER_UNKNOWN;
    parser->ws = NULL;
  }

  return parser;
}
