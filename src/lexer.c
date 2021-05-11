/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "lexer/comment-block.h"
#include "lexer/comment-line.h"
#include "lexer/lit-char.h"
#include "lexer/lit-float.h"
#include "lexer/lit-id.h"
#include "lexer/lit-int-bin.h"
#include "lexer/lit-int-dec.h"
#include "lexer/lit-int-hex.h"
#include "lexer/lit-int-oct.h"
#include "lexer/lit-str.h"
#include "lexer/op.h"
#include "lexer/ws.h"

const char *lexer_token_str[] = {
  FOREACH_LEXER_TOKEN(GEN_ENUM_STR)
};

void lexer_free (lexer_t *lexer) {
  if (lexer->raw != NULL) {
    free(lexer->raw);
  }

  if (lexer->str != NULL) {
    free(lexer->str);
  }

  free(lexer);
}

void lexer_free_cb (void *it) {
  lexer_free((lexer_t *) it);
}

lexer_token lexer_lookup (file_t *file) {
  size_t pos = file_position(file);
  lexer_t *lexer = lexer_new(file);
  lexer_token token = LEXER_UNKNOWN;

  if (lexer != NULL) {
    token = lexer->tok;
    lexer_free(lexer);
  }

  file_seek(file, pos);
  return token;
}

lexer_t *lexer_new (file_t *file) {
  lexer_t *lexer = malloc(sizeof(lexer_t));
  size_t pos = file_position(file);

  if (
    !lex_ws(file, lexer, pos) &&
    !lex_lit_id(file, lexer, pos) &&
    !lex_op(file, lexer, pos) &&
    !lex_lit_int_dec(file, lexer, pos) &&
    !lex_lit_str(file, lexer, pos) &&
    !lex_comment_line(file, lexer, pos) &&
    !lex_comment_block(file, lexer, pos) &&
    !lex_lit_float(file, lexer, pos) &&
    !lex_lit_int_bin(file, lexer, pos) &&
    !lex_lit_int_hex(file, lexer, pos) &&
    !lex_lit_int_oct(file, lexer, pos) &&
    !lex_lit_char(file, lexer, pos)
  ) {
    lexer->raw = NULL;
    lexer->str = NULL;
    lexer->tok = LEXER_UNKNOWN;
  }

  return lexer;
}
