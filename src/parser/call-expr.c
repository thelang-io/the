/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "call-expr.h"
#include "ws.h"

void parser_call_expr_free (parser_call_expr_t *parser) {
  parser_arglist_free(parser->arglist);
  parser_id_free(parser->id);
  free(parser);
}

parser_t *parser_call_expr_new (file_t *file) {
  parser_arglist_t *parser_arglist = NULL;
  parser_id_t *parser_id = NULL;
  size_t pos = file_position(file);

  if ((parser_id = (parser_id_t *) parser_id_new(file)) == NULL) {
    file_seek(file, pos);
    return NULL;
  }

  parser_ws_new(file, false);

  if (file_eof(file)) {
    file_seek(file, pos);
    parser_id_free(parser_id);
    return NULL;
  }

  lexer_t *lpar = lexer_new(file);

  if (lpar->tok != LEXER_OP_LPAR) {
    file_seek(file, pos);
    parser_id_free(parser_id);
    lexer_free(lpar);
    return NULL;
  }

  lexer_free(lpar);
  parser_ws_new(file, false);

  if (file_eof(file)) {
    file_seek(file, pos);
    parser_id_free(parser_id);
    return NULL;
  }

  if ((parser_arglist = (parser_arglist_t *) parser_arglist_new(file)) == NULL) {
    file_seek(file, pos);
    parser_id_free(parser_id);
    return NULL;
  }

  parser_ws_new(file, false);

  if (file_eof(file)) {
    file_seek(file, pos);
    parser_id_free(parser_id);
    parser_arglist_free(parser_arglist);
    return NULL;
  }

  lexer_t *rpar = lexer_new(file);

  if (rpar->tok != LEXER_OP_RPAR) {
    file_seek(file, pos);
    parser_id_free(parser_id);
    parser_arglist_free(parser_arglist);
    lexer_free(rpar);
    return NULL;
  }

  lexer_free(rpar);

  parser_call_expr_t *parser = malloc(sizeof(parser_call_expr_t));
  parser->tok = PARSER_CALL_EXPR;
  parser->arglist = parser_arglist;
  parser->id = parser_id;
  return (parser_t *) parser;
}
