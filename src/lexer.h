/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include <duc/fs.h>

typedef struct lexer_s lexer_t;

typedef enum {
  LEXER_KW_FN,
  LEXER_KW_IN,
  LEXER_KW_LOOP,
  LEXER_KW_MAIN,
  LEXER_KW_MUT,
  LEXER_KW_RETURN,

  LEXER_LIT_FLOAT,
  LEXER_LIT_ID,
  LEXER_LIT_INT_BIN,
  LEXER_LIT_INT_DEC,
  LEXER_LIT_INT_HEX,
  LEXER_LIT_INT_OCT,
  LEXER_LIT_STR,

  LEXER_OP_AND,
  LEXER_OP_CARET,
  LEXER_OP_COLON,
  LEXER_OP_COMMA,
  LEXER_OP_DOT,
  LEXER_OP_EQ,
  LEXER_OP_EXCL,
  LEXER_OP_GT,
  LEXER_OP_LBRACE,
  LEXER_OP_LBRACK,
  LEXER_OP_LPAR,
  LEXER_OP_LT,
  LEXER_OP_MINUS,
  LEXER_OP_OR,
  LEXER_OP_PERCENT,
  LEXER_OP_PLUS,
  LEXER_OP_QN,
  LEXER_OP_RBRACE,
  LEXER_OP_RBRACK,
  LEXER_OP_RPAR,
  LEXER_OP_SEMI,
  LEXER_OP_SLASH,
  LEXER_OP_STAR,
  LEXER_OP_TILDE,

  LEXER_UNKNOWN,
  LEXER_WS
} lexer_token;

struct lexer_s {
  unsigned char *raw;
  unsigned char *str;
  lexer_token token;
};

void lexer_free (lexer_t *lexer);
void lexer_free_cb (void *it);
lexer_t *lexer_new (duc_file_t *file);

#endif
