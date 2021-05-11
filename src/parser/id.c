/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "id.h"

void parser_id_free (parser_t *parser) {
  lexer_free(((parser_id_t *) parser)->lexer);
  free(parser);
}

parser_t *parser_id_new (file_t *file) {
  parser_id_t *parser = malloc(sizeof(parser_id_t));
  size_t pos = file_position(file);
  parser->lexer = lexer_new(file);

  if (parser->lexer->token != LEXER_LIT_ID) {
    parser_id_free((parser_t *) parser);
    file_seek(file, pos);
    return NULL;
  }

  return (parser_t *) parser;
}
