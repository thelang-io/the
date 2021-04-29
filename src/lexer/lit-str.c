/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include "lit-char.h"
#include "lit-str.h"

bool lex_lit_str (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);

  if (ch1 != '"') {
    duc_file_seek(file, pos);
    return false;
  } else if (duc_file_eof(file)) {
    duc_throw("SyntaxError: Unterminated string literal");
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch1;
  lexer->raw[len] = '\0';
  lexer->token = LEXER_LIT_STR;

  while (true) {
    unsigned char ch = duc_file_readchar(file);

    if (ch == '\\' && !duc_file_eof(file)) {
      unsigned char ch_esc = duc_file_readchar(file);

      if (!lexer_lit_char_is_escape(ch_esc) && ch_esc != '$') {
        duc_throw("SyntaxError: Illegal character escape");
      }

      len += 2;
      lexer->raw = realloc(lexer->raw, len + 1);
      lexer->raw[len - 2] = ch;
      lexer->raw[len - 1] = ch_esc;
      lexer->raw[len] = '\0';
    } else {
      lexer->raw = realloc(lexer->raw, ++len + 1);
      lexer->raw[len - 1] = ch;
      lexer->raw[len] = '\0';
    }

    if (ch == '"') {
      break;
    } else if (duc_file_eof(file)) {
      duc_throw("SyntaxError: Unterminated string literal");
    }
  }

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}
