/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "expr.h"

void parser_expr_free (parser_t *parser) {
  parser_expr_t *parser_expr = (parser_expr_t *) parser;

  if (parser_expr->parser->token == PARSER_ID) {
    parser_id_free(parser_expr->parser);
  } else if (parser_expr->parser->token == PARSER_LITERAL) {
    parser_literal_free(parser_expr->parser);
  }

  free(parser_expr);
}

void parser_expr_free_cb (void *it) {
  parser_expr_free((parser_t *) it);
}

parser_t *parser_expr_new (file_t *file) {
  parser_t *parser = NULL;

  if ((parser = parser_id_new(file)) == NULL) {
    if ((parser = parser_literal_new(file)) == NULL) {
      return NULL;
    }
  }

  parser_expr_t *parser_expr = malloc(sizeof(parser_expr_t));
  parser_expr->tok = PARSER_EXPR;
  parser_expr->parser = parser;
  return (parser_t *) parser_expr;
}
