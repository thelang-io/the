#include <stdlib.h>
#include "parser.h"

void parser_arglist_free (parser_arglist_t *parser) {
  duc_array_free(parser->exprs, parser_expr_free_cb);
  free(parser);
}

parser_arglist_t *parser_arglist_new (duc_file_t *file) {
  parser_arglist_t *parser = malloc(sizeof(parser_arglist_t));
  parser->exprs = duc_array_new();

  size_t pos = duc_file_position(file);
  lexer_t *rpar = lexer_new(file);
  duc_file_seek(file, pos);

  if (rpar == NULL) {
    parser_arglist_free(parser);
    return NULL;
  }

  while (rpar->token != LEXER_RPAR) {
    lexer_free(rpar);
    parser_expr_t *expr = parser_expr_new(file);

    if (expr == NULL) {
      parser_arglist_free(parser);
      duc_file_seek(file, pos);
      return NULL;
    }

    duc_array_push(parser->exprs, expr);
    parser_ws_new(file, false);

    if (duc_file_eof(file)) {
      parser_arglist_free(parser);
      duc_file_seek(file, pos);
      return NULL;
    }

    size_t bu_pos = duc_file_position(file);
    rpar = lexer_new(file);

    if (rpar == NULL) {
      parser_arglist_free(parser);
      duc_file_seek(file, pos);
      return NULL;
    } else if (rpar->token == LEXER_COMMA) {
      parser_ws_new(file, false);

      if (duc_file_eof(file)) {
        lexer_free(rpar);
        parser_arglist_free(parser);
        duc_file_seek(file, pos);
        return NULL;
      }

      continue;
    } else if (rpar->token != LEXER_RPAR) {
      lexer_free(rpar);
      parser_arglist_free(parser);
      duc_file_seek(file, pos);
      return NULL;
    }

    duc_file_seek(file, bu_pos);
  }

  lexer_free(rpar);
  return parser;
}

void parser_call_expr_free (parser_call_expr_t *parser) {
  parser_arglist_free(parser->arglist);
  parser_id_free(parser->id);
  free(parser);
}

parser_call_expr_t *parser_call_expr_new (duc_file_t *file) {
  parser_call_expr_t *parser = malloc(sizeof(parser_call_expr_t));
  size_t pos = duc_file_position(file);

  if ((parser->id = parser_id_new(file)) == NULL) {
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  parser_ws_new(file, false);

  if (duc_file_eof(file)) {
    parser_id_free(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_t *lpar = lexer_new(file);

  if (lpar == NULL) {
    parser_id_free(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  } else if (lpar->token != LEXER_LPAR) {
    lexer_free(lpar);
    parser_id_free(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_free(lpar);
  parser_ws_new(file, false);

  if (duc_file_eof(file)) {
    parser_id_free(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  if ((parser->arglist = parser_arglist_new(file)) == NULL) {
    parser_id_free(parser->id);
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_t *rpar = lexer_new(file);

  if (rpar == NULL) {
    parser_call_expr_free(parser);
    duc_file_seek(file, pos);
    return NULL;
  } else if (rpar->token != LEXER_RPAR) {
    lexer_free(rpar);
    parser_call_expr_free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  lexer_free(rpar);
  return parser;
}

void parser_expr_free (parser_expr_t *parser) {
  if (parser->token == PARSER_ID) {
    parser_id_free(parser->id);
  } else if (parser->token == PARSER_LITERAL) {
    parser_literal_free(parser->literal);
  }

  free(parser);
}

void parser_expr_free_cb (void *it) {
  parser_expr_free((parser_expr_t *) it);
}

parser_expr_t *parser_expr_new (duc_file_t *file) {
  parser_expr_t *parser = malloc(sizeof(parser_expr_t));

  parser->id = NULL;
  parser->literal = NULL;
  parser->token = PARSER_UNKNOWN;

  if ((parser->id = parser_id_new(file)) != NULL) {
    parser->token = PARSER_ID;
    return parser;
  } else if ((parser->literal = parser_literal_new(file)) != NULL) {
    parser->token = PARSER_LITERAL;
    return parser;
  }

  free(parser);
  return NULL;
}

void parser_free (parser_t *parser) {
  if (parser->token == PARSER_CALL_EXPR) {
    parser_call_expr_free(parser->call_expr);
  } else if (parser->token == PARSER_WS) {
    parser_ws_free(parser->ws);
  }

  free(parser);
}

void parser_free_cb (void *it) {
  parser_free((parser_t *) it);
}

void parser_id_free (parser_id_t *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

parser_id_t *parser_id_new (duc_file_t *file) {
  parser_id_t *parser = malloc(sizeof(parser_id_t));
  size_t pos = duc_file_position(file);
  parser->lexer = lexer_new(file);

  if (parser->lexer == NULL) {
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  } else if (parser->lexer->token != LEXER_ID) {
    parser_id_free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  return parser;
}

void parser_literal_free (parser_literal_t *parser) {
  lexer_free(parser->lexer);
  free(parser);
}

parser_literal_t *parser_literal_new (duc_file_t *file) {
  parser_literal_t *parser = malloc(sizeof(parser_literal_t));
  size_t pos = duc_file_position(file);
  parser->lexer = lexer_new(file);

  if (parser->lexer == NULL) {
    free(parser);
    duc_file_seek(file, pos);
    return NULL;
  } else if (
    parser->lexer->token != LEXER_LITSTR_SQ &&
    parser->lexer->token != LEXER_LITSTR_DQ
  ) {
    parser_literal_free(parser);
    duc_file_seek(file, pos);
    return NULL;
  }

  return parser;
}

parser_t *parser_new (duc_file_t *file) {
  parser_t *parser = malloc(sizeof(parser_t));

  parser->call_expr = NULL;
  parser->token = PARSER_UNKNOWN;
  parser->ws = NULL;

  if ((parser->ws = parser_ws_new(file, true)) != NULL) {
    parser->token = PARSER_WS;
    return parser;
  } else if ((parser->call_expr = parser_call_expr_new(file)) != NULL) {
    parser->token = PARSER_CALL_EXPR;
    return parser;
  }

  free(parser);
  return NULL;
}

void parser_ws_free (parser_ws_t *parser) {
  duc_array_free(parser->lexers, lexer_free_cb);
  free(parser);
}

parser_ws_t *parser_ws_new (duc_file_t *file, bool alloc) {
  parser_ws_t *parser = NULL;

  if (alloc) {
    parser = malloc(sizeof(parser_ws_t));
    parser->lexers = duc_array_new();
  }

  for (size_t i = 0; !duc_file_eof(file); i++) {
    size_t bu_pos = duc_file_position(file);
    lexer_t *ws = lexer_new(file);

    if (ws == NULL) {
      if (alloc) {
        parser_ws_free(parser);
      }

      duc_file_seek(file, bu_pos);
      return NULL;
    } else if (ws->token != LEXER_WS) {
      lexer_free(ws);

      if (i == 0) {
        if (alloc) {
          parser_ws_free(parser);
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
