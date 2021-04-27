/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "ast.h"

void ast_free (ast_t *ast) {
  duc_array_free(ast->parsers, parser_free_cb);
  free(ast);
}

ast_t *ast_new (duc_file_t *file) {
  ast_t *ast = malloc(sizeof(ast_t));
  ast->parsers = duc_array_new();

  while (!duc_file_eof(file)) {
    parser_t *parser = parser_new(file);

    if (parser->token == PARSER_UNKNOWN) {
      duc_throw("SyntaxError: Unexpected expression");
    } else if (parser->token == PARSER_WS) {
      parser_free(parser);
      continue;
    }

    duc_array_push(ast->parsers, parser);
  }

  for (size_t i = 0, size = duc_array_length(ast->parsers); i < size; i++) {
    parser_t *parser = duc_array_at(ast->parsers, i);

    if (parser->token == PARSER_CALL_EXPR) {
      parser_call_expr_t *call_expr = parser->call_expr;

      if (memcmp(call_expr->id->lexer->str, "print", 6) != 0) {
        duc_throw("SyntaxError: Unexpected call expression");
      }

      parser_arglist_t *arglist = call_expr->arglist;

      for (size_t j = 0, size_arglist = duc_array_length(arglist->exprs); j < size_arglist; j++) {
        parser_expr_t *expr = duc_array_at(arglist->exprs, j);

        if (expr->token != PARSER_LITERAL) {
          duc_throw("SyntaxError: Unexpected call expression argument");
        }
      }
    }
  }

  return ast;
}
