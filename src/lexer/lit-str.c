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

  lex_lit_str_block_(file, lexer, &len);

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}

void lex_lit_str_block_ (duc_file_t *file, lexer_t *lexer, size_t *len) {
  size_t blocks = 0;

  while (true) {
    unsigned char ch = duc_file_readchar(file);
    bool is_processed = false;

    if (ch == '\\' && !duc_file_eof(file)) {
      unsigned char ch_esc = duc_file_readchar(file);

      if (!lexer_lit_char_is_escape(ch_esc) && ch_esc != '$') {
        duc_throw("SyntaxError: Illegal character escape");
      }

      *len += 2;
      lexer->raw = realloc(lexer->raw, *len + 1);
      lexer->raw[*len - 2] = ch;
      lexer->raw[*len - 1] = ch_esc;
      lexer->raw[*len] = '\0';
      is_processed = true;
    } else if (ch == '$' && !duc_file_eof(file)) {
      size_t bu_pos = duc_file_position(file);
      unsigned char ch_lpar = duc_file_readchar(file);

      if (ch_lpar == '{') {
        *len += 2;
        lexer->raw = realloc(lexer->raw, *len + 1);
        lexer->raw[*len - 2] = ch;
        lexer->raw[*len - 1] = ch_lpar;
        lexer->raw[*len] = '\0';
        blocks += 1;
        is_processed = true;
      } else {
        duc_file_seek(file, bu_pos);
      }
    }

    if (!is_processed) {
      *len += 1;
      lexer->raw = realloc(lexer->raw, *len + 1);
      lexer->raw[*len - 1] = ch;
      lexer->raw[*len] = '\0';
    }

    if (ch == '"' && blocks != 0) {
      lex_lit_str_block_(file, lexer, len);
    } else if (ch == '{' && blocks != 0) {
      blocks += 1;
    } else if (ch == '}' && blocks != 0) {
      blocks -= 1;
    } else if (ch == '"') {
      break;
    } else if (duc_file_eof(file)) {
      duc_throw("SyntaxError: Unterminated string literal");
    }
  }
}
