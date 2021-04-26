/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include <duc/fs.h>
#include <inttypes.h>
#include <stdint.h>

typedef struct lexer_s lexer_t;

typedef enum {
  LEXER_COMMA,
  LEXER_ID,
  LEXER_LITSTR,
  LEXER_LBRACE,
  LEXER_LBRACK,
  LEXER_LPAR,
  LEXER_RBRACE,
  LEXER_RBRACK,
  LEXER_RPAR,
  LEXER_UNKNOWN,
  LEXER_WS
} lexer_token;

struct lexer_s {
  uint8_t *raw;
  uint8_t *str;
  lexer_token token;
};

void lexer_free (lexer_t *lexer);
void lexer_free_cb (void *it);
lexer_t *lexer_new (duc_file_t *file);

bool lexer_is_bracket_ (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_id_ (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_litstr_ (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_mark_ (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_ws_ (duc_file_t *file, lexer_t *lexer, size_t pos);

#endif
