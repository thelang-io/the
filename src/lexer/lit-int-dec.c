/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "lit-float.h"
#include "lit-int-bin.h"
#include "lit-int-dec.h"
#include "lit-int-hex.h"
#include "lit-int-oct.h"

bool lex_lit_int_dec (file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = file_readchar(file);
  bool is_lit = false;

  if (ch1 == '0') {
    if (file_eof(file)) {
      is_lit = true;
    } else {
      size_t bu_pos = file_position(file);
      unsigned char ch_next = file_readchar(file);

      if (lexer_lit_float_is_char_special(ch_next)) {
        if (ch_next == '.' && !file_eof(file)) {
          unsigned char ch_next2 = file_readchar(file);

          if (ch_next2 == '.') {
            file_seek(file, bu_pos);
            is_lit = true;
          }
        }
      } else if (
        !lexer_lit_int_bin_is_special(ch_next) &&
        !lexer_lit_int_hex_is_special(ch_next) &&
        !lexer_lit_int_oct_is_special(ch_next)
      ) {
        file_seek(file, bu_pos);
        is_lit = true;
      }
    }
  } else if (strchr("123456789", ch1) != NULL) {
    is_lit = true;
  }

  if (!is_lit) {
    file_seek(file, pos);
    return false;
  }

  size_t len = 1;
  unsigned char *raw = malloc(len + 1);
  raw[len - 1] = ch1;
  raw[len] = '\0';

  if (ch1 != '0') {
    while (!file_eof(file)) {
      size_t bu_pos = file_position(file);
      unsigned char ch = file_readchar(file);

      if (lexer_lit_float_is_char_special(ch)) {
        if (ch == '.' && !file_eof(file)) {
          unsigned char ch_next = file_readchar(file);

          if (ch_next == '.') {
            file_seek(file, bu_pos);
            break;
          }
        }

        free(raw);
        file_seek(file, pos);
        return false;
      } else if (strchr("0123456789", ch) == NULL) {
        file_seek(file, bu_pos);
        break;
      }

      raw = realloc(raw, ++len + 1);
      raw[len - 1] = ch;
      raw[len] = '\0';
    }
  }

  lexer->raw = raw;
  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);
  lexer->token = LEXER_LIT_INT_DEC;

  return true;
}
