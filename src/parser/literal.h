/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_LITERAL_H
#define SRC_PARSER_LITERAL_H

#include "../parser.h"

typedef struct parser_literal_s parser_literal_t;

struct parser_literal_s {
  parser_token tok;
  lexer_t *lexer;
};

void parser_literal_free (parser_literal_t *parser);
parser_t *parser_literal_new (file_t *file);

#endif
