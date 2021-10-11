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

void parser_stmt_whitespace (reader_t *reader) {
  reader_location_t loc = reader->loc;
  token_t *tok = lexer_next(reader);

  if (tok->type != whitespace) {
    reader_seek(reader, loc);
  }
}

stmt_t *parser_next (reader_t *reader) {
  parser_stmt_whitespace(reader);
  reader_location_t start = reader->loc;

  if (reader_eof(reader)) {
    return stmt_init(stmtEnd, start, reader->loc);
  }

  token_t *tok = lexer_next(reader);

  if (tok->type == kwMain) {
    parser_stmt_whitespace(reader);
    token_t *l_brace = lexer_next(reader);

    if (l_brace->type != opLBrace) {
      // TODO throw
    }

    stmt_t **body = NULL;
    size_t body_len = 0;

    while (true) {
      parser_stmt_whitespace(reader);
      reader_location_t r_brace_loc = reader->loc;
      token_t *r_brace = lexer_next(reader);

      if (r_brace->type == opRBrace) {
        break;
      } else if (r_brace->type == eof) {
        // TODO throw
      }

      reader_seek(reader, r_brace_loc);

      if (body_len == 0) {
        body = malloc(++body_len * sizeof(stmt_t));
      } else {
        body = realloc(body, ++body_len * sizeof(stmt_t));
      }

      body[body_len - 1] = parser_next(reader);
    }

    stmt_main_t *main_stmt = (stmt_main_t *) stmt_init(stmtMain, start, reader->loc);
    main_stmt->body = body;
    main_stmt->body_len = body_len;

    return (stmt_t *) main_stmt;
  } else if (tok->type == litId) {
    parser_stmt_whitespace(reader);
    token_t *l_par = lexer_next(reader);

    if (l_par->type != opLPar) {
      // TODO throw
    }

    parser_stmt_whitespace(reader);
    token_t *r_par = lexer_next(reader);
    expr_t **args = NULL;
    size_t args_len = 0;

    while (r_par->type != opRPar) {
      if (r_par->type == eof) {
        // TODO throw
      }

      if (r_par->type == litStr) {
        expr_literal_t *expr = (expr_literal_t *) expr_init(exprLiteral, start, reader->loc);
        expr->tok = r_par;

        if (args_len == 0) {
          args = malloc(++args_len * sizeof(expr_t));
        } else {
          args = realloc(args, ++args_len * sizeof(expr_t));
        }

        args[args_len - 1] = (expr_t *) expr;
      } else {
        // TODO throw
      }

      parser_stmt_whitespace(reader);
      r_par = lexer_next(reader);

      if (r_par->type == opComma) {
        parser_stmt_whitespace(reader);
        r_par = lexer_next(reader);
      }
    }

    stmt_call_expr_t *call_expr_stmt = (stmt_call_expr_t *) stmt_init(stmtCallExpr, start, reader->loc);
    call_expr_stmt->args = args;
    call_expr_stmt->args_len = args_len;
    call_expr_stmt->callee = tok;

    return (stmt_t *) call_expr_stmt;
  }

  throw_syntax_error(reader, start, E0100);
}
