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
#include "lit-int.h"

bool lex_lit_int (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);
  bool is_bin = false;
  bool is_dec = false;
  bool is_hex = false;
  bool is_oct = false;
  size_t len = 1;
  unsigned char *raw = NULL;

  if (ch1 == '0') {
    if (duc_file_eof(file)) {
      lexer->raw = malloc(len + 1);
      lexer->raw[len - 1] = ch1;
      lexer->raw[len] = '\0';
      lexer->str = malloc(len + 1);
      memcpy(lexer->str, lexer->raw, len + 1);
      lexer->token = LEXER_LIT_INT_DEC;

      return true;
    } else {
      size_t bu_pos = duc_file_position(file);
      unsigned char ch2 = duc_file_readchar(file);

      is_bin = ch2 == 'B' || ch2 == 'b';
      is_hex = ch2 == 'X' || ch2 == 'x';
      is_oct = ch2 == 'O' || ch2 == 'o';

      if (lexer_lit_float_is_char_special(ch2)) {
        duc_file_seek(file, pos);
        return false;
      } else if (is_bin || is_hex || is_oct) {
        if (duc_file_eof(file)) {
          duc_throw("Invalid number");
        }

        unsigned char ch3 = duc_file_readchar(file);

        if (
          (is_bin && strchr("01", ch3) == NULL) ||
          (is_oct && strchr("01234567", ch3) == NULL) ||
          (is_hex && strchr("0123456789abcdefABCDEF", ch3) == NULL)
        ) {
          duc_throw("Invalid number");
        }

        len += 2;
        raw = malloc(len + 1);
        raw[len - 3] = ch1;
        raw[len - 2] = ch2;
        raw[len - 1] = ch3;
        raw[len] = '\0';
      } else if (strchr("1234567", ch2) != NULL) {
        raw = malloc(++len + 1);
        raw[len - 2] = ch1;
        raw[len - 1] = ch2;
        raw[len] = '\0';
        is_oct = true;
      } else if (strchr("089", ch2) != NULL) {
        duc_throw("Invalid number");
      } else {
        duc_file_seek(file, bu_pos);

        lexer->raw = malloc(len + 1);
        lexer->raw[len - 1] = ch1;
        lexer->raw[len] = '\0';
        lexer->str = malloc(len + 1);
        memcpy(lexer->str, lexer->raw, len + 1);
        lexer->token = LEXER_LIT_INT_DEC;

        return true;
      }
    }
  } else if (strchr("123456789", ch1) != NULL) {
    raw = malloc(len + 1);
    raw[len - 1] = ch1;
    raw[len] = '\0';
    is_dec = true;
  }

  if (!is_bin && !is_dec && !is_hex && !is_oct) {
    duc_file_seek(file, pos);
    return false;
  }

  while (!duc_file_eof(file)) {
    size_t bu_pos = duc_file_position(file);
    unsigned char ch = duc_file_readchar(file);

    if (lexer_lit_float_is_char_special(ch)) {
      free(raw);
      duc_file_seek(file, pos);
      return false;
    } else if (
      (is_bin && strchr("01", ch) == NULL) ||
      (is_oct && strchr("01234567", ch) == NULL) ||
      (is_dec && strchr("0123456789", ch) == NULL) ||
      (is_hex && strchr("0123456789abcdefABCDEF", ch) == NULL)
    ) {
      duc_file_seek(file, bu_pos);
      break;
    }

    raw = realloc(raw, ++len + 1);
    raw[len - 1] = ch;
    raw[len] = '\0';
  }

  lexer->raw = raw;
  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  if (is_bin) {
    lexer->token = LEXER_LIT_INT_BIN;
  } else if (is_dec) {
    lexer->token = LEXER_LIT_INT_DEC;
  } else if (is_hex) {
    lexer->token = LEXER_LIT_INT_HEX;
  } else if (is_oct) {
    lexer->token = LEXER_LIT_INT_OCT;
  }

  return true;
}
