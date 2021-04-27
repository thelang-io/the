/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "lexer/lit-float.h"
#include "lexer/lit-id.h"
#include "lexer/lit-int.h"
#include "lexer/lit-str.h"
#include "lexer/op.h"
#include "lexer/ws.h"

void lexer_free (lexer_t *lexer) {
  free(lexer->raw);
  free(lexer->str);
  free(lexer);
}

void lexer_free_cb (void *it) {
  lexer_free((lexer_t *) it);
}

lexer_t *lexer_new (duc_file_t *file) {
  lexer_t *lexer = malloc(sizeof(lexer_t));
  size_t pos = duc_file_position(file);

  if (
    lex_ws(file, lexer, pos) ||
    lex_lit_id(file, lexer, pos) ||
    lex_op(file, lexer, pos) ||
    lex_lit_str(file, lexer, pos) ||
    lex_lit_int(file, lexer, pos) ||
    lex_lit_float(file, lexer, pos)
  ) {
    return lexer;
  } else {
    lexer->raw = NULL;
    lexer->str = NULL;
    lexer->token = LEXER_UNKNOWN;
  }

  return lexer;
}
