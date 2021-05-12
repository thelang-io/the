/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "expr.h"

void parser_expr_free (parser_expr_t *parser) {
  if (parser->parser->tok == PARSER_ID) {
    parser_id_free((parser_id_t *) parser->parser);
  } else if (parser->parser->tok == PARSER_LITERAL) {
    parser_literal_free((parser_literal_t *) parser->parser);
  }

  free(parser);
}

void parser_expr_free_cb (void *it) {
  parser_expr_free((parser_expr_t *) it);
}

parser_expr_t *parser_expr_new (file_t *file) {
  parser_t *parser = NULL;

  if ((parser = parser_id_new(file)) == NULL) {
    if ((parser = parser_literal_new(file)) == NULL) {
      return NULL;
    }
  }

  parser_expr_t *parser_expr = malloc(sizeof(parser_expr_t));
  parser_expr->tok = PARSER_EXPR;
  parser_expr->parser = parser;
  return parser_expr;
}
