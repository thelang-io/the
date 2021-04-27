/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include "lit-str.h"

bool lex_lit_str (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if (ch != '"' || duc_file_eof(file)) {
    duc_file_seek(file, pos);
    return false;
  } else {
    lexer->token = LEXER_LIT_STR;
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch;
  lexer->raw[len] = '\0';

  while (true) {
    ch = duc_file_readchar(file);

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';

    if (ch == '"') {
      break;
    } else if (duc_file_eof(file)) {
      duc_throw("SyntaxError: Unexpected end of file, expected end of string");
    }
  }

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}
