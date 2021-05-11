/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_WS_H
#define SRC_PARSER_WS_H

#include "../parser.h"

typedef struct parser_ws_s parser_ws_t;

struct parser_ws_s {
  parser_token tok;
  array_t *lexers;
};

void parser_ws_free (parser_ws_t *parser);
parser_t *parser_ws_new (file_t *file, bool alloc);

#endif
