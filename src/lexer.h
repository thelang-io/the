#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include <duc/fs.h>

typedef struct lexer_s lexer_t;

typedef enum {
  LEXER_UNKNOWN,
  LEXER_WS,

  LEXER_LPAR,
  LEXER_RPAR,

  LEXER_ID,

  LEXER_LITSTR_DQ,
  LEXER_LITSTR_SQ
} lexer_token;

struct lexer_s {
  unsigned char *raw;
  unsigned char *str;
  lexer_token token;
};

void lexer_free (lexer_t *lexer);
bool lexer_is_bracket (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_id (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_litstr (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_is_ws (duc_file_t *file, lexer_t *lexer, size_t pos);
lexer_t *lexer_new (duc_file_t *file);
char *lexer_token_to_string (lexer_token token);

#endif
