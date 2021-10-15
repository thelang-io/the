/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "error.h"
#include "lexer.h"
#include "parser.h"

expr_t *parser_expr (reader_t *reader) {
  reader_location_t start = reader->loc;
  token_t *tok = lexer_next(reader);

  if (
    tok->type == litChar ||
    tok->type == litFloat ||
    tok->type == litId ||
    tok->type == litIntDec ||
    tok->type == litStr
  ) {
    expr_t *expr = expr_init(exprLiteral, start, reader->loc);
    expr->literal->tok = tok;
    return expr;
  }

  reader_seek(reader, start);
  return NULL;
}

void parser_stmt_whitespace (reader_t *reader) {
  while (true) {
    reader_location_t loc = reader->loc;
    token_t *tok = lexer_next(reader);

    if (tok->type != whitespace) {
      reader_seek(reader, loc);
      token_free(tok);
      break;
    }

    token_free(tok);
  }
}

stmt_t *parser_next (reader_t *reader) {
  parser_stmt_whitespace(reader);
  reader_location_t start = reader->loc;

  if (reader_eof(reader)) {
    return stmt_init(stmtEnd, start, reader->loc);
  }

  token_t *tok1 = lexer_next(reader);

  if (tok1->type == kwMain) {
    token_free(tok1);
    parser_stmt_whitespace(reader);
    token_t *tok2 = lexer_next(reader);

    if (tok2->type != opLBrace) {
      // TODO throw
    }

    token_free(tok2);
    stmt_t **body = NULL;
    size_t body_len = 0;

    while (true) {
      parser_stmt_whitespace(reader);
      reader_location_t rbrace_loc = reader->loc;
      token_t *tok3 = lexer_next(reader);

      if (tok3->type == opRBrace) {
        token_free(tok3);
        break;
      } else if (tok3->type == eof) {
        // TODO throw
      }

      token_free(tok3);
      reader_seek(reader, rbrace_loc);

      if (body_len == 0) {
        body = malloc(++body_len * sizeof(stmt_t));
      } else {
        body = realloc(body, ++body_len * sizeof(stmt_t));
      }

      if (body == NULL) {
        throw_error("Unable to re-allocate memory for main statement body");
      }

      body[body_len - 1] = parser_next(reader);
    }

    stmt_t *stmt = stmt_init(stmtMain, start, reader->loc);
    stmt->main->body = body;
    stmt->main->body_len = body_len;

    return stmt;
  } else if (tok1->type == litId) {
    parser_stmt_whitespace(reader);
    token_t *tok2 = lexer_next(reader);

    if (tok2->type == opLPar) {
      token_free(tok2);
      parser_stmt_whitespace(reader);
      reader_location_t rpar_loc = reader->loc;
      token_t *tok3 = lexer_next(reader);
      expr_t **args = NULL;
      size_t args_len = 0;

      while (tok3->type != opRPar) {
        if (tok3->type == eof) {
          // TODO throw
        }

        token_free(tok3);
        reader_seek(reader, rpar_loc);
        expr_t *expr = parser_expr(reader);

        if (expr == NULL) {
          // TODO throw
        } else if (args_len == 0) {
          args = malloc(++args_len * sizeof(expr_t));
        } else {
          args = realloc(args, ++args_len * sizeof(expr_t));
        }

        if (args == NULL) {
          throw_error("Unable to re-allocate memory for call expression arguments");
        }

        args[args_len - 1] = expr;
        parser_stmt_whitespace(reader);
        tok3 = lexer_next(reader);

        if (tok3->type == opComma) {
          token_free(tok3);
          parser_stmt_whitespace(reader);
          rpar_loc = reader->loc;
          tok3 = lexer_next(reader);
        }
      }

      token_free(tok3);
      stmt_t *stmt = stmt_init(stmtCallExpr, start, reader->loc);
      stmt->call_expr->args = args;
      stmt->call_expr->args_len = args_len;
      stmt->call_expr->callee = tok1;

      return stmt;
    } else if (tok2->type == opColonEq) {
      token_free(tok2);
      parser_stmt_whitespace(reader);
      expr_t *expr = parser_expr(reader);

      if (expr == NULL) {
        // TODO throw
      }

      stmt_t *stmt = stmt_init(stmtShortVarDecl, start,reader->loc);
      stmt->short_var_decl->id = tok1;
      stmt->short_var_decl->init = expr;

      return stmt;
    }

    token_free(tok2);
  }

  throw_syntax_error(reader, start, E0100);
}
