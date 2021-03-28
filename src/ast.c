#include <stdlib.h>
#include "ast.h"

void ast_free(ast_t *ast) {
  duc_array_free(ast->parsers, parser_free_cb);
  free(ast);
}

ast_t *ast_new (duc_file_t *file) {
  ast_t *ast = malloc(sizeof(ast_t));
  ast->parsers = duc_array_new();

  while (!duc_file_eof(file)) {
    parser_t *parser = parser_new(file);

    if (parser == NULL) {
      ast_free(ast);
      return NULL;
    } else if (parser->token == PARSER_WS) {
      parser_free(parser);
      continue;
    }

    duc_array_push(ast->parsers, parser);
  }

  return ast;
}
