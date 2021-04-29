/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include "lit-float.h"
#include "lit-int-bin.h"

bool lex_lit_int_bin (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);
  unsigned char ch2;

  if (ch1 == '0' && !duc_file_eof(file)) {
    ch2 = duc_file_readchar(file);

    if (lexer_lit_int_bin_is_special(ch2)) {
      if (duc_file_eof(file)) {
        duc_throw("SyntaxError: Invalid binary literal");
      }

      lexer->token = LEXER_LIT_INT_BIN;
    }
  }

  if (lexer->token != LEXER_LIT_INT_BIN) {
    duc_file_seek(file, pos);
    return false;
  }

  size_t len = 2;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 2] = ch1;
  lexer->raw[len - 1] = ch2;
  lexer->raw[len] = '\0';

  while (!duc_file_eof(file)) {
    size_t bu_pos = duc_file_position(file);
    unsigned char ch = duc_file_readchar(file);

    if (strchr("23456789", ch) != NULL) {
      duc_throw("SyntaxError: Invalid digit for binary literal");
    } else if (lexer_lit_float_is_char_special(ch)) {
      duc_throw("SyntaxError: Binary float literal is not supported");
    } else if (strchr("01", ch) == NULL) {
      if (len == 2) {
        duc_throw("SyntaxError: Invalid binary literal");
      } else {
        duc_file_seek(file, bu_pos);
        break;
      }
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  return true;
}

bool lexer_lit_int_bin_is_special (unsigned char ch) {
  return strchr("Bb", ch) != NULL;
}
