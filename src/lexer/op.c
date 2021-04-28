/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "op.h"

bool lex_op (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  switch (ch) {
    case '&': {
      lexer->token = LEXER_OP_AND;
      break;
    }
    case ':': {
      lexer->token = LEXER_OP_COLON;
      break;
    }
    case ',': {
      lexer->token = LEXER_OP_COMMA;
      break;
    }
    case '.': {
      if (!duc_file_eof(file)) {
        size_t bu_pos = duc_file_position(file);
        unsigned char ch_next = duc_file_readchar(file);

        if (strchr("0123456789", ch_next) == NULL) {
          duc_file_seek(file, bu_pos);
        } else {
          duc_file_seek(file, pos);
          return false;
        }
      }

      lexer->token = LEXER_OP_DOT;
      break;
    }
    case '=': {
      lexer->token = LEXER_OP_EQ;
      break;
    }
    case '!': {
      lexer->token = LEXER_OP_EXCL;
      break;
    }
    case '>': {
      lexer->token = LEXER_OP_GT;
      break;
    }
    case '{': {
      lexer->token = LEXER_OP_LBRACE;
      break;
    }
    case '[': {
      lexer->token = LEXER_OP_LBRACK;
      break;
    }
    case '(': {
      lexer->token = LEXER_OP_LPAR;
      break;
    }
    case '<': {
      lexer->token = LEXER_OP_LT;
      break;
    }
    case '-': {
      lexer->token = LEXER_OP_MINUS;
      break;
    }
    case '|': {
      lexer->token = LEXER_OP_OR;
      break;
    }
    case '%': {
      lexer->token = LEXER_OP_PERCENT;
      break;
    }
    case '+': {
      lexer->token = LEXER_OP_PLUS;
      break;
    }
    case '}': {
      lexer->token = LEXER_OP_RBRACE;
      break;
    }
    case ']': {
      lexer->token = LEXER_OP_RBRACK;
      break;
    }
    case ')': {
      lexer->token = LEXER_OP_RPAR;
      break;
    }
    case '/': {
      lexer->token = LEXER_OP_SLASH;
      break;
    }
    case '*': {
      lexer->token = LEXER_OP_STAR;
      break;
    }
    default: {
      duc_file_seek(file, pos);
      return false;
    }
  }

  lexer->raw = malloc(2);
  lexer->raw[0] = ch;
  lexer->raw[1] = '\0';
  lexer->str = malloc(2);
  memcpy(lexer->str, lexer->raw, 2);

  return true;
}
