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

bool lex_lit_float (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);
  size_t len = 1;
  bool with_exp = false;

  if (ch1 == '.' && !duc_file_eof(file)) {
    lexer->raw = malloc(len + 1);
    lexer->raw[len - 1] = ch1;
    lexer->raw[len] = '\0';
    lexer->token = LEXER_LIT_FLOAT;
  } else if (ch1 == '0' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (lexer_lit_float_is_char_special(ch2)) {
      if (ch2 == 'E' || ch2 == 'e') {
        with_exp = true;
      }
    } else {
      duc_file_seek(file, pos);
      return false;
    }

    lexer->raw = malloc(++len + 1);
    lexer->raw[len - 2] = ch1;
    lexer->raw[len - 1] = ch2;
    lexer->raw[len] = '\0';
    lexer->token = LEXER_LIT_FLOAT;
  } else if (strchr("123456789", ch1) != NULL && !duc_file_eof(file)) {
    unsigned char *raw = malloc(len + 1);
    raw[len - 1] = ch1;
    raw[len] = '\0';

    while (true) {
      unsigned char ch = duc_file_readchar(file);

      raw = realloc(raw, ++len + 1);
      raw[len - 1] = ch;
      raw[len] = '\0';

      if (lexer_lit_float_is_char_special(ch)) {
        if (ch == 'E' || ch == 'e') {
          with_exp = true;
        }

        break;
      } else if (strchr("123456789", ch1) == NULL || duc_file_eof(file)) {
        free(raw);
        duc_file_seek(file, pos);
        return false;
      }
    }

    lexer->raw = raw;
    lexer->token = LEXER_LIT_FLOAT;
  } else {
    duc_file_seek(file, pos);
    return false;
  }

  if (!with_exp) {
    while (!duc_file_eof(file)) {
      size_t bu_pos = duc_file_position(file);
      unsigned char ch = duc_file_readchar(file);

      if (ch == 'E' || ch == 'e') {
        with_exp = true;
      } else if (strchr("0123456789", ch) == NULL) {
        duc_file_seek(file, bu_pos);
        break;
      }

      lexer->raw = realloc(lexer->raw, ++len + 1);
      lexer->raw[len - 1] = ch;
      lexer->raw[len] = '\0';

      if (ch == 'E' || ch == 'e') {
        break;
      }
    }
  }

  if (with_exp) {
    if (duc_file_eof(file)) {
      duc_throw("Unexpected end of file, expected floating-point literal exponent");
    }

    unsigned char ch_op = duc_file_readchar(file);

    if (ch_op == '+' || ch_op == '-') {
      if (duc_file_eof(file)) {
        duc_throw("Unexpected end of file, expected floating-point literal exponent");
      }

      unsigned char ch_dec = duc_file_readchar(file);

      if (strchr("0123456789", ch_dec) == NULL) {
        duc_throw("Unexpected token, expected floating-point literal exponent");
      }

      len += 2;
      lexer->raw = realloc(lexer->raw, len + 1);
      lexer->raw[len - 2] = ch_op;
      lexer->raw[len - 1] = ch_dec;
      lexer->raw[len] = '\0';
    } else if (strchr("0123456789", ch_op) == NULL) {
      duc_throw("Unexpected token, expected floating-point literal exponent");
    } else {
      lexer->raw = realloc(lexer->raw, ++len + 1);
      lexer->raw[len - 1] = ch_op;
      lexer->raw[len] = '\0';
    }

    while (!duc_file_eof(file)) {
      size_t bu_pos = duc_file_position(file);
      unsigned char ch = duc_file_readchar(file);

      if (strchr("0123456789", ch) == NULL) {
        duc_file_seek(file, bu_pos);
        break;
      }

      lexer->raw = realloc(lexer->raw, ++len + 1);
      lexer->raw[len - 1] = ch;
      lexer->raw[len] = '\0';
    }
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  return true;
}

bool lexer_lit_float_is_char_special (unsigned char ch) {
  return ch == 'E' || ch == 'e' || ch == '.';
}
