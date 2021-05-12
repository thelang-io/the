/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

// #include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

int main (int argc, char *argv[]) {
  if (argc < 2) {
    throw("Error: Action is not set");
  } else if (strcmp(argv[1], "lex") == 0) {
    file_t *file = file_new(argv[2], FILE_READ);

    while (!file_eof(file)) {
      lexer_t *lexer = lexer_new(file);

      if (lexer == NULL) {
        throw("SyntaxError: Unexpected token");
      } else if (lexer->tok != LEXER_WS) {
        printf("%s: %s\n", lexer_token_str[lexer->tok], lexer->raw);
      }

      lexer_free(lexer);
    }

    file_free(file);
    return EXIT_SUCCESS;
  }

/*  file_t *file = file_new(argv[1], FILE_READ);
  ast_t *ast = ast_new(file);
  binary_t *bin = codegen(ast);

  if (bin == NULL) {
    throw("Error: Something went wrong");
  }

  binary_write(bin, "a.out");
  chmod("a.out", 0755);

  binary_free(bin);
  ast_free(ast);
  file_free(file); */

  return EXIT_SUCCESS;
}
