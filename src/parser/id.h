/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_ID_H
#define SRC_PARSER_ID_H

#include "../parser.h"

typedef struct parser_id_s parser_id_t;

struct parser_id_s {
  parser_token tok;
  lexer_t *lexer;
};

void parser_id_free (parser_id_t *parser);
parser_t *parser_id_new (file_t *file);

#endif
