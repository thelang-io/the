/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include "litint.h"

bool lexer_litint (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);
  size_t len = 1;

  if (ch1 == '0') {
    if (duc_file_eof(file)) {
      goto lexer_is_litint_dec;
    } else {
      size_t bu_pos = duc_file_position(file);
      unsigned char ch2 = duc_file_readchar(file);
      bool is_bin = ch2 == 'B' || ch2 == 'b';
      bool is_hex = ch2 == 'X' || ch2 == 'x';
      bool is_oct = ch2 == 'O' || ch2 == 'o';

      if ((is_bin || is_hex || is_oct) && !duc_file_eof(file)) {
        unsigned char ch3 = duc_file_readchar(file);

        if (
          (is_bin && strchr("01", ch3) == NULL) ||
          (is_oct && strchr("01234567", ch3) == NULL) ||
          (is_hex && strchr("0123456789abcdefABCDEF", ch3) == NULL)
        ) {
          duc_throw("Invalid number");
        } else if (is_bin) {
          lexer->token = LEXER_LITINT_BIN;
        } else if (is_oct) {
          lexer->token = LEXER_LITINT_OCT;
        } else {
          lexer->token = LEXER_LITINT_HEX;
        }

        len += 2;
        lexer->raw = malloc(len + 1);
        lexer->raw[len - 3] = ch1;
        lexer->raw[len - 2] = ch2;
        lexer->raw[len - 1] = ch3;
        lexer->raw[len] = '\0';
      } else if (strchr("123456789", ch2) != NULL) {
        lexer->raw = malloc(++len + 1);
        lexer->raw[len - 2] = ch1;
        lexer->raw[len - 1] = ch2;
        lexer->raw[len] = '\0';
        lexer->token = LEXER_LITINT_OCT;
      } else {
        duc_file_seek(file, bu_pos);

        lexer->raw = malloc(len + 1);
        lexer->str = malloc(len + 1);
        lexer->raw[len - 1] = ch1;
        lexer->raw[len] = '\0';
        memcpy(lexer->str, lexer->raw, len + 1);
        lexer->token = LEXER_LITINT_DEC;

        return true;
      }
    }
  } else if (strchr("123456789", ch1) != NULL) {
lexer_is_litint_dec:
    lexer->raw = malloc(len + 1);
    lexer->raw[len - 1] = ch1;
    lexer->raw[len] = '\0';
    lexer->token = LEXER_LITINT_DEC;
  } else {
    duc_file_seek(file, pos);
    return false;
  }

  while (true) {
    if (duc_file_eof(file)) {
      break;
    }

    size_t bu_pos = duc_file_position(file);
    unsigned char ch = duc_file_readchar(file);
    bool is_bin = lexer->token == LEXER_LITINT_BIN;
    bool is_dec = lexer->token == LEXER_LITINT_DEC;
    bool is_hex = lexer->token == LEXER_LITINT_HEX;
    bool is_oct = lexer->token == LEXER_LITINT_OCT;

    if (
      (is_bin && strchr("01", ch) == NULL) ||
      (is_oct && strchr("01234567", ch) == NULL) ||
      (is_dec && strchr("0123456789", ch) == NULL) ||
      (is_hex && strchr("0123456789abcdefABCDEF", ch) == NULL)
    ) {
      duc_file_seek(file, bu_pos);
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
