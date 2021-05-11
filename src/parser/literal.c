/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "literal.h"

void parser_literal_free (parser_literal_t *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

parser_t *parser_literal_new (file_t *file) {
  size_t pos = file_position(file);
  lexer_t *lexer = lexer_new(file);

  if (lexer->token != LEXER_LIT_STR) {
    lexer_free(lexer);
    file_seek(file, pos);
    return NULL;
  }

  parser_literal_t *parser = malloc(sizeof(parser_literal_t));
  parser->tok = PARSER_LITERAL;
  parser->lexer = lexer;
  return (parser_t *) parser;
}
