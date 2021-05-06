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
#include "lit-int-oct.h"

bool lex_lit_int_oct (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);
  unsigned char ch2;
  bool is_lit = false;

  if (ch1 == '0' && !duc_file_eof(file)) {
    ch2 = duc_file_readchar(file);

    if (lexer_lit_int_oct_is_special(ch2)) {
      if ((ch2 == 'O' || ch2 == 'o') && duc_file_eof(file)) {
        duc_throw("SyntaxError: Invalid octal literal");
      }

      is_lit = true;
    } else if (strchr("89", ch2) != NULL) {
      duc_throw("SyntaxError: Invalid octal literal");
    }
  }

  if (!is_lit) {
    duc_file_seek(file, pos);
    return false;
  }

  size_t len = 2;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 2] = ch1;
  lexer->raw[len - 1] = ch2;
  lexer->raw[len] = '\0';
  lexer->token = LEXER_LIT_INT_OCT;

  while (!duc_file_eof(file)) {
    size_t bu_pos = duc_file_position(file);
    unsigned char ch = duc_file_readchar(file);

    if (strchr("89", ch) != NULL) {
      duc_throw("SyntaxError: Invalid octal literal");
    } else if (lexer_lit_float_is_char_special(ch)) {
      if (ch == '.' && !duc_file_eof(file)) {
        unsigned char ch_next = duc_file_readchar(file);

        if (ch_next == '.') {
          duc_file_seek(file, bu_pos);
          break;
        }
      }

      duc_throw("SyntaxError: Octal float literal is not supported");
    } else if (strchr("01234567", ch) == NULL) {
      unsigned char ch_prev = lexer->raw[len - 1];

      if (len == 2 && (ch_prev == 'O' || ch_prev == 'o')) {
        duc_throw("SyntaxError: Invalid octal literal");
      } else {
        duc_file_seek(file, bu_pos);
      }

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

bool lexer_lit_int_oct_is_special (unsigned char ch) {
  return strchr("01234567Oo", ch) != NULL;
}
