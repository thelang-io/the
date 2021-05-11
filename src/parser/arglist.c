/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "arglist.h"
#include "ws.h"

void parser_arglist_free (parser_t *parser) {
  parser_arglist_t *parser_arglist = (parser_arglist_t *) parser;
  array_free(parser_arglist->exprs, parser_expr_free_cb);
  free(parser_arglist);
}

parser_t *parser_arglist_new (file_t *file) {
  array_t *exprs = array_new();
  size_t pos = file_position(file);
  lexer_token rpar_tok = lexer_lookup(file);

  if (rpar_tok != LEXER_OP_RPAR) {
    while (true) {
      parser_t *expr = parser_expr_new(file);

      if (expr == NULL) {
        array_free(exprs, parser_expr_free_cb);
        file_seek(file, pos);
        return NULL;
      }

      array_push(exprs, expr);
      size_t bu_pos = file_position(file);
      parser_ws_new(file, false);

      if (file_eof(file)) {
        array_free(exprs, parser_expr_free_cb);
        file_seek(file, pos);
        return NULL;
      }

      lexer_t *lexer = lexer_new(file);

      if (lexer->token == LEXER_OP_COMMA) {
        parser_ws_new(file, false);
        lexer_free(lexer);

        if (file_eof(file)) {
          file_seek(file, pos);
          array_free(exprs, parser_expr_free_cb);
          return NULL;
        }

        continue;
      } else if (lexer->token == LEXER_OP_RPAR) {
        file_seek(file, bu_pos);
        lexer_free(lexer);
        break;
      }

      file_seek(file, pos);
      array_free(exprs, parser_expr_free_cb);
      lexer_free(lexer);
      return NULL;
    }
  }

  parser_arglist_t *parser_arglist = malloc(sizeof(parser_arglist_t));
  parser_arglist->tok = PARSER_ARGLIST;
  parser_arglist->exprs = exprs;
  return (parser_t *) parser_arglist;
}
