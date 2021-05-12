/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "id.h"

void parser_id_free (parser_id_t *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

parser_id_t *parser_id_new (file_t *file) {
  size_t pos = file_position(file);
  lexer_t *lexer = lexer_new(file);

  if (lexer->tok != LEXER_LIT_ID) {
    lexer_free(lexer);
    file_seek(file, pos);
    return NULL;
  }

  parser_id_t *parser = malloc(sizeof(parser_id_t));
  parser->tok = PARSER_ID;
  parser->lexer = lexer;
  return parser;
}
