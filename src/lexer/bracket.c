/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "bracket.h"

bool lexer_bracket (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  switch (ch) {
    case '{': {
      lexer->token = LEXER_LBRACE;
      break;
    }
    case '[': {
      lexer->token = LEXER_LBRACK;
      break;
    }
    case '(': {
      lexer->token = LEXER_LPAR;
      break;
    }
    case '}': {
      lexer->token = LEXER_RBRACE;
      break;
    }
    case ']': {
      lexer->token = LEXER_RBRACK;
      break;
    }
    case ')': {
      lexer->token = LEXER_RPAR;
      break;
    }
    default: {
      duc_file_seek(file, pos);
      return false;
    }
  }

  lexer->raw = malloc(2);
  lexer->str = malloc(2);
  lexer->raw[0] = ch;
  lexer->raw[1] = '\0';
  memcpy(lexer->str, lexer->raw, 2);

  return true;
}
