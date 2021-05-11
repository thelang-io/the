/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "parser.h"

void parser_free (parser_t *parser) {
  if (parser->token == PARSER_CALL_EXPR && parser->call_expr != NULL) {
    parser_call_expr_free_(parser->call_expr);
  } else if (parser->token == PARSER_WS && parser->ws != NULL) {
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
    parser->token = PARSER_WS;
  } else if ((parser->call_expr = parser_call_expr_new_(file)) != NULL) {
    parser->token = PARSER_CALL_EXPR;
  } else {
    parser->call_expr = NULL;
    parser->token = PARSER_UNKNOWN;
    parser->ws = NULL;
  }

  return parser;
}

void parser_call_expr_free_ (parser_call_expr_t *parser) {
  parser_arglist_free_(parser->arglist);
  parser_id_free_(parser->id);
  free(parser);
}

parser_call_expr_t *parser_call_expr_new_ (file_t *file) {
  parser_call_expr_t *parser = malloc(sizeof(parser_call_expr_t));
  size_t pos = file_position(file);

  if ((parser->id = parser_id_new_(file)) == NULL) {
    free(parser);
    file_seek(file, pos);
    return NULL;
  }

  parser_ws_new_(file, false);

  if (file_eof(file)) {
    parser_id_free_(parser->id);
    free(parser);
    file_seek(file, pos);
    return NULL;
  }

  lexer_t *lpar = lexer_new(file);

  if (lpar->token != LEXER_OP_LPAR) {
    lexer_free(lpar);
    parser_id_free_(parser->id);
    free(parser);
    file_seek(file, pos);
    return NULL;
  }

  lexer_free(lpar);
  parser_ws_new_(file, false);

  if (file_eof(file)) {
    parser_id_free_(parser->id);
    free(parser);
    file_seek(file, pos);
    return NULL;
  }

  if ((parser->arglist = parser_arglist_new_(file)) == NULL) {
    parser_id_free_(parser->id);
    free(parser);
    file_seek(file, pos);
    return NULL;
  }

  parser_ws_new_(file, false);

  if (file_eof(file)) {
    parser_call_expr_free_(parser);
    file_seek(file, pos);
    return NULL;
  }

  lexer_t *rpar = lexer_new(file);

  if (rpar->token != LEXER_OP_RPAR) {
    lexer_free(rpar);
    parser_call_expr_free_(parser);
    file_seek(file, pos);
    return NULL;
  }

  lexer_free(rpar);
  return parser;
}
