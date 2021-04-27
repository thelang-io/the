/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "lexer/bracket.h"
#include "lexer/id.h"
#include "lexer/keyword.h"
#include "lexer/litfloat.h"
#include "lexer/litint.h"
#include "lexer/litstr.h"
#include "lexer/mark.h"
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

  lexer->raw = NULL;
  lexer->str = NULL;
  lexer->token = LEXER_UNKNOWN;

  size_t pos = duc_file_position(file);

  if (
    lexer_ws(file, lexer, pos) ||
    lexer_litfloat(file, lexer, pos) ||
    lexer_litint(file, lexer, pos) ||
    lexer_litstr(file, lexer, pos) ||
    lexer_bracket(file, lexer, pos) ||
    lexer_mark(file, lexer, pos) ||
    lexer_keyword(file, lexer, pos) ||
    lexer_id(file, lexer, pos)
  ) {
    return lexer;
  }

  free(lexer);
  return NULL;
}
