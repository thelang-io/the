/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "ws.h"

void parser_ws_free (parser_ws_t *parser) {
  array_free(parser->lexers, lexer_free_cb);
  free(parser);
}

parser_ws_t *parser_ws_new (file_t *file, bool alloc) {
  array_t *lexers = alloc ? array_new() : NULL;

  for (size_t i = 0; !file_eof(file); i++) {
    size_t pos = file_position(file);
    lexer_t *lexer = lexer_new(file);

    if (lexer->tok != LEXER_WS) {
      file_seek(file, pos);
      lexer_free(lexer);

      if (!alloc) {
        return NULL;
      } else if (i == 0) {
        array_free(lexers, free_simple);
        return NULL;
      } else {
        break;
      }
    }

    alloc ? array_push(lexers, lexer) : lexer_free(lexer);
  }

  parser_ws_t *parser = malloc(sizeof(parser_ws_t));
  parser->tok = PARSER_WS;
  parser->lexers = lexers;
  return parser;
}
