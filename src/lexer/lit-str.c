/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "lit-char.h"
#include "lit-str.h"

bool lex_lit_str (file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = file_readchar(file);

  if (ch1 != '"') {
    file_seek(file, pos);
    return false;
  } else if (file_eof(file)) {
    throw("SyntaxError: Unterminated string literal");
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch1;
  lexer->raw[len] = '\0';
  lexer->token = LEXER_LIT_STR;

  lexer_lit_str_process_(file, lexer, &len);

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}

void lexer_lit_str_process_ (file_t *file, lexer_t *lexer, size_t *len) {
  size_t blocks = 0;

  while (true) {
    unsigned char ch = file_readchar(file);
    bool is_processed = false;

    if (ch == '\\') {
      if (file_eof(file)) {
        throw("SyntaxError: Unterminated string literal");
      }

      unsigned char ch_esc = file_readchar(file);

      if (!lexer_lit_char_is_escape(ch_esc) && ch_esc != '{') {
        throw("SyntaxError: Illegal character escape");
      }

      *len += 2;
      lexer->raw = realloc(lexer->raw, *len + 1);
      lexer->raw[*len - 2] = ch;
      lexer->raw[*len - 1] = ch_esc;
      lexer->raw[*len] = '\0';
      is_processed = true;
    }

    if (!is_processed) {
      *len += 1;
      lexer->raw = realloc(lexer->raw, *len + 1);
      lexer->raw[*len - 1] = ch;
      lexer->raw[*len] = '\0';
    }

    if (ch == '{') {
      blocks += 1;
    } else if (ch == '}' && blocks != 0) {
      blocks -= 1;
    } else if (ch == '\'' && blocks != 0) {
      lexer_lit_char_process_(file, lexer, len);
    } else if (ch == '"' && blocks != 0) {
      lexer_lit_str_process_(file, lexer, len);
    } else if (ch == '"') {
      break;
    } else if (file_eof(file)) {
      throw("SyntaxError: Unterminated string literal");
    }
  }
}
