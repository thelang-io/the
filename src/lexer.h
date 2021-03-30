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
  LEXER_COMMA,
  LEXER_ID,
  LEXER_LITSTR_DQ,
  LEXER_LITSTR_SQ,
  LEXER_LPAR,
  LEXER_RPAR,
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
bool lexer_is_bracket (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_id (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_litstr (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_mark (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_ws (duc_file_t *file, lexer_t *lexer, size_t pos);
lexer_t *lexer_new (duc_file_t *file);

#endif
