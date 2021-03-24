#include <duc/helpers.h>
#include <stdio.h>
#include "lexer.h"

int main (DUC_UNUSED int argc, char *argv[]) {
  duc_file_t *file = duc_file_new(argv[1], "r");

  while (!duc_file_eof(file)) {
    lexer_t *lexer = lexer_new(file);

    printf(
      "Token: %s, Raw: %s, Value: %s\n",
      lexer_token_to_string(lexer->token),
      lexer->raw,
      lexer->str
    );

    lexer_free(lexer);
  }

  return 0;
}
