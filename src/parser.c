/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "parser/call-expr.h"
#include "parser/ws.h"
#include "parser.h"

const char *parser_token_str[] = {
  FOREACH_PARSER_TOKEN(GEN_ENUM_STR)
};

void parser_free (parser_t *parser) {
  if (parser->tok == PARSER_ARGLIST) {
    parser_arglist_free((parser_arglist_t *) parser);
  } else if (parser->tok == PARSER_CALL_EXPR) {
    parser_call_expr_free((parser_call_expr_t *) parser);
  } else if (parser->tok == PARSER_EXPR) {
    parser_expr_free((parser_expr_t *) parser);
  } else if (parser->tok == PARSER_ID) {
    parser_id_free((parser_id_t *) parser);
  } else if (parser->tok == PARSER_LITERAL) {
    parser_literal_free((parser_literal_t *) parser);
  } else if (parser->tok == PARSER_WS) {
    parser_ws_free((parser_ws_t *) parser);
  }
}

void parser_free_cb (void *it) {
  parser_free((parser_t *) it);
}

parser_t *parser_new (file_t *file) {
  parser_t *parser = NULL;

  if (
    (parser = (parser_t *) parser_ws_new(file, true)) != NULL ||
    (parser = (parser_t *) parser_call_expr_new(file)) != NULL
  ) {
    return parser;
  }

  return NULL;
}
