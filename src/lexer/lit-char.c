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

bool lex_lit_char (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);

  if (ch1 != '\'') {
    duc_file_seek(file, pos);
    return false;
  } else if (duc_file_eof(file)) {
    duc_throw("SyntaxError: Unterminated character literal");
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch1;
  lexer->raw[len] = '\0';
  lexer->token = LEXER_LIT_CHAR;

  lexer_lit_char_process_(file, lexer, &len);

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}

bool lexer_lit_char_is_escape (unsigned char ch) {
  return strchr("0tnr\"'\\", ch) != NULL;
}

void lexer_lit_char_process_ (duc_file_t *file, lexer_t *lexer, size_t *len) {
  unsigned char ch2 = duc_file_readchar(file);

  if (ch2 == '\'') {
    duc_throw("SyntaxError: Empty character literal");
  } else if (ch2 == '\\') {
    if (duc_file_eof(file)) {
      duc_throw("SyntaxError: Unterminated character literal");
    }

    unsigned char ch_esc = duc_file_readchar(file);

    if (!lexer_lit_char_is_escape(ch_esc)) {
      duc_throw("SyntaxError: Illegal character escape");
    }

    *len += 2;
    lexer->raw = realloc(lexer->raw, *len + 1);
    lexer->raw[*len - 2] = ch2;
    lexer->raw[*len - 1] = ch_esc;
    lexer->raw[*len] = '\0';
  } else {
    *len += 1;
    lexer->raw = realloc(lexer->raw, *len + 1);
    lexer->raw[*len - 1] = ch2;
    lexer->raw[*len] = '\0';
  }

  if (duc_file_eof(file)) {
    duc_throw("SyntaxError: Unterminated character literal");
  }

  unsigned char ch3 = duc_file_readchar(file);

  if (ch3 != '\'') {
    duc_throw("SyntaxError: Too many characters in a character literal");
  }

  *len += 1;
  lexer->raw = realloc(lexer->raw, *len + 1);
  lexer->raw[*len - 1] = ch3;
  lexer->raw[*len] = '\0';
}
