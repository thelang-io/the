/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "lit-id.h"

bool lex_lit_id (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if (lexer_lit_id_is_char_start(ch)) {
    lexer->token = LEXER_LIT_ID;
  } else {
    duc_file_seek(file, pos);
    return false;
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch;
  lexer->raw[len] = '\0';

  while (!duc_file_eof(file)) {
    size_t bu_pos = duc_file_position(file);
    ch = duc_file_readchar(file);

    if (!lexer_lit_id_is_char(ch)) {
      duc_file_seek(file, bu_pos);
      break;
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  switch (len) {
    case 2: {
      if (memcmp(lexer->str, "as", len + 1) == 0) {
        if (!duc_file_eof(file)) {
          size_t bu_pos = duc_file_position(file);
          unsigned char ch_safe = duc_file_readchar(file);

          if (ch_safe == '?') {
            lexer->raw = realloc(lexer->raw, ++len + 1);
            lexer->str = realloc(lexer->str, len + 1);
            lexer->raw[len - 1] = lexer->str[len - 1] = ch_safe;
            lexer->raw[len] = lexer->str[len] = '\0';
            lexer->token = LEXER_KW_AS_SAFE;
          } else {
            duc_file_seek(file, bu_pos);
            lexer->token = LEXER_KW_AS;
          }
        } else {
          lexer->token = LEXER_KW_AS;
        }
      } else if (memcmp(lexer->str, "fn", len + 1) == 0) {
        lexer->token = LEXER_KW_FN;
      } else if (memcmp(lexer->str, "if", len + 1) == 0) {
        lexer->token = LEXER_KW_IF;
      } else if (memcmp(lexer->str, "in", len + 1) == 0) {
        lexer->token = LEXER_KW_IN;
      } else if (memcmp(lexer->str, "is", len + 1) == 0) {
        lexer->token = LEXER_KW_IS;
      } else if (memcmp(lexer->str, "op", len + 1) == 0) {
        lexer->token = LEXER_KW_OP;
      }

      break;
    }
    case 3: {
      if (memcmp(lexer->str, "mut", len + 1) == 0) {
        lexer->token = LEXER_KW_MUT;
      }

      break;
    }
    case 4: {
      if (memcmp(lexer->str, "loop", len + 1) == 0) {
        lexer->token = LEXER_KW_LOOP;
      } else if (memcmp(lexer->str, "main", len + 1) == 0) {
        lexer->token = LEXER_KW_MAIN;
      }

      break;
    }
    case 6: {
      if (memcmp(lexer->str, "return", len + 1) == 0) {
        lexer->token = LEXER_KW_RETURN;
      }

      break;
    }
  }

  return true;
}

bool lexer_lit_id_is_char (unsigned char ch) {
  const char *chs =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
  return strchr(chs, ch) != NULL;
}

bool lexer_lit_id_is_char_start (unsigned char ch) {
  const char *chs = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
  return strchr(chs, ch) != NULL;
}
