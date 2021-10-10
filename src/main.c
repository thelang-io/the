/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "lexer.h"

int main () {
  reader_t *reader = reader_init("program.adl");

  while (true) {
    token_t *tok = lexer_next(reader);

    if (tok->type == whitespace) {
      token_free(tok);
      continue;
    } else if (tok->type == eof) {
      token_free(tok);
      break;
    }

    char *tok_str = token_str(tok);
    printf("%s\n", tok_str);
    free(tok_str);
    token_free(tok);
  }

  reader_free(reader);
  return 0;
}
