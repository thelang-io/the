/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "parser.h"

// TODO Code split

void parser_free (parser_t *parser) {
  if (parser->token == PARSER_CALL_EXPR && parser->call_expr != NULL) {
    parser_call_expr_free_(parser->call_expr);
  } else if (parser->token == PARSER_WS && parser->ws != NULL) {
    parser_ws_free_(parser->ws);
  }

  free(parser);
}

void parser_free_cb (void *it) {
  parser_free((parser_t *) it);
}

parser_t *parser_new (duc_file_t *file) {
  parser_t *parser = malloc(sizeof(parser_t));

  if ((parser->ws = parser_ws_new_(file, true)) != NULL) {
    parser->token = PARSER_WS;
  } else if ((parser->call_expr = parser_call_expr_new_(file)) != NULL) {
    parser->token = PARSER_CALL_EXPR;
  } else {
    parser->call_expr = NULL;
    parser->token = PARSER_UNKNOWN;
    parser->ws = NULL;
  }

  return parser;
}

void parser_arglist_free_ (parser_arglist_t *parser) {
  duc_array_free(parser->exprs, parser_expr_free_cb_);
  free(parser);
}

parser_arglist_t *parser_arglist_new_ (duc_file_t *file) {
  parser_arglist_t *parser = malloc(sizeof(parser_arglist_t));
  parser->exprs = duc_array_new();

  size_t pos = duc_file_position(file);
  lexer_t *rpar = lexer_new(file);
  duc_file_seek(file, pos);

  while (rpar->token != LEXER_OP_RPAR) {
    lexer_free(rpar);
    parser_expr_t *expr = parser_expr_new_(file);

    if (expr == NULL) {
      parser_arglist_free_(parser);
      duc_file_seek(file, pos);
      return NULL;
    }

    duc_array_push(parser->exprs, expr);
    size_t bu_pos = duc_file_position(file);
    parser_ws_new_(file, false);

    if (duc_file_eof(file)) {
      parser_arglist_free_(parser);
      duc_file_seek(file, pos);
      return NULL;
    }

    rpar = lexer_new(file);

    if (rpar->token == LEXER_OP_COMMA) {
      parser_ws_new_(file, false);

      if (duc_file_eof(file)) {
        lexer_free(rpar);
        parser_arglist_free_(parser);
        duc_file_seek(file, pos);
        return NULL;
      }

      continue;
    } else if (rpar->token != LEXER_OP_RPAR) {
      lexer_free(rpar);
      parser_arglist_free_(parser);
      duc_file_seek(file, pos);
      return NULL;
    }

    duc_file_seek(file, bu_pos);
  }

  lexer_free(rpar);
  return parser;
}

void parser_call_expr_free_ (parser_call_expr_t *parser) {
  parser_arglist_free_(parser->arglist);
  parser_id_free_(parser->id);
  free(parser);
}

parser_call_expr_t *parser_call_expr_new_ (duc_file_t *file) {
  parser_call_expr_t *parser = malloc(sizeof(parser_call_expr_t));
  size_t pos = duc_file_position(file);

  if ((parser->id = parser_id_new_(file)) == NULL) {
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  parser_ws_new_(file, false);

  if (duc_file_eof(file)) {
    parser_id_free_(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_t *lpar = lexer_new(file);

  if (lpar->token != LEXER_OP_LPAR) {
    lexer_free(lpar);
    parser_id_free_(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_free(lpar);
  parser_ws_new_(file, false);

  if (duc_file_eof(file)) {
    parser_id_free_(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  if ((parser->arglist = parser_arglist_new_(file)) == NULL) {
    parser_id_free_(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  parser_ws_new_(file, false);

  if (duc_file_eof(file)) {
    parser_call_expr_free_(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_t *rpar = lexer_new(file);

  if (rpar->token != LEXER_OP_RPAR) {
    lexer_free(rpar);
    parser_call_expr_free_(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_free(rpar);
  return parser;
}

void parser_expr_free_ (parser_expr_t *parser) {
  if (parser->token == PARSER_ID) {
    parser_id_free_(parser->id);
  } else if (parser->token == PARSER_LITERAL) {
    parser_literal_free_(parser->literal);
  }

  free(parser);
}

void parser_expr_free_cb_ (void *it) {
  parser_expr_free_((parser_expr_t *) it);
}

parser_expr_t *parser_expr_new_ (duc_file_t *file) {
  parser_expr_t *parser = malloc(sizeof(parser_expr_t));

  parser->id = NULL;
  parser->literal = NULL;
  parser->token = PARSER_UNKNOWN;

  if ((parser->id = parser_id_new_(file)) != NULL) {
    parser->token = PARSER_ID;
    return parser;
  } else if ((parser->literal = parser_literal_new_(file)) != NULL) {
    parser->token = PARSER_LITERAL;
    return parser;
  }

  free(parser);
  return NULL;
}

void parser_id_free_ (parser_id_t *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

parser_id_t *parser_id_new_ (duc_file_t *file) {
  parser_id_t *parser = malloc(sizeof(parser_id_t));
  size_t pos = duc_file_position(file);
  parser->lexer = lexer_new(file);

  if (parser->lexer->token != LEXER_LIT_ID) {
    parser_id_free_(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  return parser;
}

void parser_literal_free_ (parser_literal_t *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

parser_literal_t *parser_literal_new_ (duc_file_t *file) {
  parser_literal_t *parser = malloc(sizeof(parser_literal_t));
  size_t pos = duc_file_position(file);
  parser->lexer = lexer_new(file);

  if (parser->lexer->token != LEXER_LIT_STR) {
    parser_literal_free_(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  return parser;
}

void parser_ws_free_ (parser_ws_t *parser) {
  duc_array_free(parser->lexers, lexer_free_cb);
  free(parser);
}

parser_ws_t *parser_ws_new_ (duc_file_t *file, bool alloc) {
  parser_ws_t *parser = NULL;

  if (alloc) {
    parser = malloc(sizeof(parser_ws_t));
    parser->lexers = duc_array_new();
  }

  for (size_t i = 0; !duc_file_eof(file); i++) {
    size_t bu_pos = duc_file_position(file);
    lexer_t *ws = lexer_new(file);

    if (ws->token != LEXER_WS) {
      lexer_free(ws);

      if (i == 0) {
        if (alloc) {
          parser_ws_free_(parser);
        }

        duc_file_seek(file, bu_pos);
        return NULL;
      } else {
        duc_file_seek(file, bu_pos);
        break;
      }
    } else if (alloc) {
      duc_array_push(parser->lexers, ws);
    } else {
      lexer_free(ws);
    }
  }

  return parser;
}
