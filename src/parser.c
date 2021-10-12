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

  token_free(tok);
}

stmt_t *parser_next (reader_t *reader) {
  parser_stmt_whitespace(reader);
  reader_location_t start = reader->loc;

  if (reader_eof(reader)) {
    return stmt_init(stmtEnd, start, reader->loc);
  }

  token_t *tok = lexer_next(reader);

  if (tok->type == kwMain) {
    token_free(tok);
    parser_stmt_whitespace(reader);
    token_t *lbrace = lexer_next(reader);

    if (lbrace->type != opLBrace) {
      // TODO throw
    }

    token_free(lbrace);
    stmt_t **body = NULL;
    size_t body_len = 0;

    while (true) {
      parser_stmt_whitespace(reader);
      reader_location_t rbrace_loc = reader->loc;
      token_t *rbrace = lexer_next(reader);

      if (rbrace->type == opRBrace) {
        token_free(rbrace);
        break;
      } else if (rbrace->type == eof) {
        // TODO throw
      }

      token_free(rbrace);
      reader_seek(reader, rbrace_loc);

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
    token_t *lpar = lexer_next(reader);

    if (lpar->type != opLPar) {
      // TODO throw
    }

    token_free(lpar);
    parser_stmt_whitespace(reader);
    token_t *rpar = lexer_next(reader);
    expr_t **args = NULL;
    size_t args_len = 0;

    while (rpar->type != opRPar) {
      if (rpar->type == eof) {
        // TODO throw
      }

      if (rpar->type == litStr) {
        expr_literal_t *expr = (expr_literal_t *) expr_init(exprLiteral, start, reader->loc);
        expr->tok = rpar;

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
      rpar = lexer_next(reader);

      if (rpar->type == opComma) {
        token_free(rpar);
        parser_stmt_whitespace(reader);
        rpar = lexer_next(reader);
      }
    }

    token_free(rpar);
    stmt_call_expr_t *call_expr_stmt = (stmt_call_expr_t *) stmt_init(stmtCallExpr, start, reader->loc);
    call_expr_stmt->args = args;
    call_expr_stmt->args_len = args_len;
    call_expr_stmt->callee = tok;

    return (stmt_t *) call_expr_stmt;
  }

  throw_syntax_error(reader, start, E0100);
}
