#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include <duc/array.h>
#include "lexer.h"

typedef struct parser_s parser_t;
typedef struct parser_arglist_s parser_arglist_t;
typedef struct parser_call_expr_s parser_call_expr_t;
typedef struct parser_expr_s parser_expr_t;
typedef struct parser_id_s parser_id_t;
typedef struct parser_literal_s parser_literal_t;
typedef struct parser_ws_s parser_ws_t;

typedef enum {
  PARSER_ARGLIST,
  PARSER_CALL_EXPR,
  PARSER_EXPR,
  PARSER_ID,
  PARSER_LITERAL,
  PARSER_UNKNOWN,
  PARSER_WS
} parser_token;

struct parser_id_s {
  lexer_t *lexer;
};

struct parser_literal_s {
  lexer_t *lexer;
};

struct parser_expr_s {
  parser_id_t *id;
  parser_literal_t *literal;
  parser_token token;
};

struct parser_arglist_s {
  duc_array_t *exprs;
};

struct parser_call_expr_s {
  parser_arglist_t *arglist;
  parser_id_t *id;
};

struct parser_ws_s {
  duc_array_t *lexers;
};

struct parser_s {
  parser_call_expr_t *call_expr;
  parser_token token;
  parser_ws_t *ws;
};

void parser_arglist_free (parser_arglist_t *parser);
parser_arglist_t *parser_arglist_new (duc_file_t *file);
void parser_call_expr_free (parser_call_expr_t *parser);
parser_call_expr_t *parser_call_expr_new (duc_file_t *file);
void parser_expr_free (parser_expr_t *parser);
void parser_expr_free_cb (void *it);
parser_expr_t *parser_expr_new (duc_file_t *file);
void parser_free (parser_t *parser);
void parser_free_cb (void *it);
void parser_id_free (parser_id_t *parser);
parser_id_t *parser_id_new (duc_file_t *file);
void parser_literal_free (parser_literal_t *parser);
parser_literal_t *parser_literal_new (duc_file_t *file);
parser_t *parser_new (duc_file_t *file);
void parser_ws_free (parser_ws_t *parser);
parser_ws_t *parser_ws_new (duc_file_t *file, bool alloc);

#endif
