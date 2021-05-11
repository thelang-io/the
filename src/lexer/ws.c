/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "ws.h"

bool lex_ws (file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = file_readchar(file);

  if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
    lexer->tok = LEXER_WS;
  } else {
    file_seek(file, pos);
    return false;
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch;
  lexer->raw[len] = '\0';

  while (!file_eof(file)) {
    size_t bu_pos = file_position(file);
    ch = file_readchar(file);

    if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t') {
      file_seek(file, bu_pos);
      break;
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  return true;
}
