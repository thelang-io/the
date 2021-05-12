/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_CALL_EXPR_H
#define SRC_PARSER_CALL_EXPR_H

#include "arglist.h"

typedef struct parser_call_expr_s parser_call_expr_t;

struct parser_call_expr_s {
  parser_token tok;
  parser_arglist_t *arglist;
  parser_id_t *id;
};

void parser_call_expr_free (parser_call_expr_t *parser);
parser_call_expr_t *parser_call_expr_new (file_t *file);

#endif
