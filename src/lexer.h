/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include "token.h"

token_t *lexer_next (reader_t *reader);

token_t *lexer_lit_float (reader_t *reader, token_t *tok, token_type_t type);
token_t *lexer_lit_num (reader_t *reader, token_t *tok, bool (*fn) (char), token_type_t type);
token_t *lexer_op_eq (reader_t *reader, token_t *tok, token_type_t type1, token_type_t type2);
token_t *lexer_op_eq2 (
  reader_t *reader,
  token_t *tok,
  char ch,
  token_type_t type1,
  token_type_t type2,
  token_type_t type3,
  token_type_t type4
);
token_t *lexer_op_eq_double (
  reader_t *reader,
  token_t *tok,
  char ch,
  token_type_t type1,
  token_type_t type2,
  token_type_t type3
);
token_t *lexer_token (reader_t *reader, token_t *tok, token_type_t type);
void lexer_walk (reader_t *reader, token_t *tok, bool (*fn) (char));
void lexer_walk_lit_float_exp (reader_t *reader, token_t *tok);
void lexer_walk_lit_str (reader_t *reader, token_t *tok);

#endif
