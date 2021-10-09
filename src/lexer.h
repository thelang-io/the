/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include "token.h"

typedef struct lexer_s lexer_t;

struct lexer_s {
  reader_t *reader;
  reader_location_t start;
  reader_location_t end;
  char *val;
  size_t val_len;
};

lexer_t *lexer_init (reader_t *reader);
void lexer_free (lexer_t *this);

token_t *lexer_lit_float (lexer_t *this, token_type_t type);
token_t *lexer_lit_num (lexer_t *this, bool (*fn) (char), token_type_t type);
token_t *lexer_next (lexer_t *this);
token_t *lexer_op_eq (lexer_t *this, token_type_t type1, token_type_t type2);
token_t *lexer_op_eq2 (lexer_t *this, char ch, token_type_t type1, token_type_t type2, token_type_t type3, token_type_t type4);
token_t *lexer_op_eq_double (lexer_t *this, char ch, token_type_t type1, token_type_t type2, token_type_t type3);
token_t *lexer_token (lexer_t *this, token_type_t type);
lexer_t *lexer_walk (lexer_t *this, bool (*fn) (char));
lexer_t *lexer_walk_lit_float_exp (lexer_t *this);
lexer_t *lexer_walk_lit_str (lexer_t *this);

#endif
