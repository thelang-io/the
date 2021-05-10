/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "lit-float.h"
#include "lit-int-oct.h"

bool lex_lit_int_oct (file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = file_readchar(file);
  unsigned char ch2;
  bool is_lit = false;

  if (ch1 == '0' && !file_eof(file)) {
    ch2 = file_readchar(file);

    if (lexer_lit_int_oct_is_special(ch2)) {
      if ((ch2 == 'O' || ch2 == 'o') && file_eof(file)) {
        throw("SyntaxError: Invalid octal literal");
      }

      is_lit = true;
    } else if (strchr("89", ch2) != NULL) {
      throw("SyntaxError: Invalid octal literal");
    }
  }

  if (!is_lit) {
    file_seek(file, pos);
    return false;
  }

  size_t len = 2;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 2] = ch1;
  lexer->raw[len - 1] = ch2;
  lexer->raw[len] = '\0';
  lexer->token = LEXER_LIT_INT_OCT;

  while (!file_eof(file)) {
    size_t bu_pos = file_position(file);
    unsigned char ch = file_readchar(file);

    if (strchr("89", ch) != NULL) {
      throw("SyntaxError: Invalid octal literal");
    } else if (lexer_lit_float_is_char_special(ch)) {
      if (ch == '.' && !file_eof(file)) {
        unsigned char ch_next = file_readchar(file);

        if (ch_next == '.') {
          file_seek(file, bu_pos);
          break;
        }
      }

      throw("SyntaxError: Octal float literal is not supported");
    } else if (strchr("01234567", ch) == NULL) {
      unsigned char ch_prev = lexer->raw[len - 1];

      if (len == 2 && (ch_prev == 'O' || ch_prev == 'o')) {
        throw("SyntaxError: Invalid octal literal");
      } else {
        file_seek(file, bu_pos);
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
