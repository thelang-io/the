/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include "litfloat.h"

bool lexer_litfloat (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);
  size_t len = 1;
  bool parsing_exponent = false;

  if (ch1 == '.' && !duc_file_eof(file)) {
    lexer->raw = malloc(len + 1);
    lexer->raw[len - 1] = ch1;
    lexer->raw[len] = '\0';
    lexer->token = LEXER_LITFLOAT;
  } else if (ch1 == '0' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'E' || ch2 == 'e') {
      parsing_exponent = true;
    } else if (ch2 != '.') {
      duc_file_seek(file, pos);
      return false;
    }

    lexer->raw = malloc(++len + 1);
    lexer->raw[len - 2] = ch1;
    lexer->raw[len - 1] = ch2;
    lexer->raw[len] = '\0';
    lexer->token = LEXER_LITFLOAT;
  } else if (strchr("123456789", ch1) != NULL && !duc_file_eof(file)) {
    unsigned char *raw = malloc(len + 1);
    raw[len - 1] = ch1;
    raw[len] = '\0';

    while (true) {
      unsigned char ch = duc_file_readchar(file);

      raw = realloc(raw, ++len + 1);
      raw[len - 1] = ch;
      raw[len] = '\0';

      if (ch == 'E' || ch == 'e') {
        parsing_exponent = true;
        break;
      } else if (ch == '.') {
        break;
      } else if (strchr("123456789", ch1) == NULL || duc_file_eof(file)) {
        free(raw);
        duc_file_seek(file, pos);
        return false;
      }
    }

    lexer->raw = raw;
    lexer->token = LEXER_LITFLOAT;
  } else {
    duc_file_seek(file, pos);
    return false;
  }

  if (!parsing_exponent) {
    while (!duc_file_eof(file)) {
      size_t bu_pos = duc_file_position(file);
      unsigned char ch = duc_file_readchar(file);

      if (ch == 'E' || ch == 'e') {
        parsing_exponent = true;
        lexer->raw = realloc(lexer->raw, ++len + 1);
        lexer->raw[len - 1] = ch;
        lexer->raw[len] = '\0';

        break;
      } else if (strchr("0123456789", ch) == NULL) {
        duc_file_seek(file, bu_pos);
        break;
      }

      lexer->raw = realloc(lexer->raw, ++len + 1);
      lexer->raw[len - 1] = ch;
      lexer->raw[len] = '\0';
    }
  }

  if (parsing_exponent) {
    if (duc_file_eof(file)) {
      duc_throw("Unexpected end of file, expected floating-point literal exponent");
    }

    unsigned char ch_op = duc_file_readchar(file);

    if (ch_op == '+' || ch_op == '-') {
      if (duc_file_eof(file)) {
        duc_throw("Unexpected end of file, expected floating-point literal exponent");
      }
    } else if (strchr("0123456789", ch_op) == NULL) {
      duc_throw("Unexpected token, expected floating-point literal exponent");
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch_op;
    lexer->raw[len] = '\0';

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
