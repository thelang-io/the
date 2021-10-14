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

  if (tok->type == litStr) {
    expr_literal_t *expr = (expr_literal_t *) expr_init(exprLiteral, start, reader->loc);
    expr->tok = tok;
    return (expr_t *) expr;
  }

  reader_seek(reader, start);
  return NULL;
}

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

    stmt_main_t *stmt_main = (stmt_main_t *) stmt_init(stmtMain, start, reader->loc);
    stmt_main->body = body;
    stmt_main->body_len = body_len;

    return (stmt_t *) stmt_main;
  } else if (tok->type == litId) {
    parser_stmt_whitespace(reader);
    token_t *lpar = lexer_next(reader);

    if (lpar->type == opLPar) {
      token_free(lpar);
      parser_stmt_whitespace(reader);
      reader_location_t rpar_loc = reader->loc;
      token_t *rpar = lexer_next(reader);
      expr_t **args = NULL;
      size_t args_len = 0;

      while (rpar->type != opRPar) {
        if (rpar->type == eof) {
          // TODO throw
        }

        token_free(rpar);
        reader_seek(reader, rpar_loc);
        expr_t *expr = parser_expr(reader);

        if (expr == NULL) {
          // TODO throw
        } else if (args_len == 0) {
          args = malloc(++args_len * sizeof(expr_t));
        } else {
          args = realloc(args, ++args_len * sizeof(expr_t));
        }

        args[args_len - 1] = expr;
        parser_stmt_whitespace(reader);
        rpar = lexer_next(reader);

        if (rpar->type == opComma) {
          token_free(rpar);
          parser_stmt_whitespace(reader);
          rpar_loc = reader->loc;
          rpar = lexer_next(reader);
        }
      }

      token_free(rpar);
      stmt_call_expr_t *stmt_call_expr = (stmt_call_expr_t *) stmt_init(stmtCallExpr, start, reader->loc);
      stmt_call_expr->args = args;
      stmt_call_expr->args_len = args_len;
      stmt_call_expr->callee = tok;

      return (stmt_t *) stmt_call_expr;
    } else if (lpar->type == opColonEq) {
      token_free(lpar);
      parser_stmt_whitespace(reader);
      expr_t *expr = parser_expr(reader);

      if (expr == NULL) {
        // TODO throw
      }

      stmt_short_var_decl_t *stmt_short_var_decl = (stmt_short_var_decl_t *) stmt_init(
        stmtShortVarDecl,
        start,
        reader->loc
      );

      stmt_short_var_decl->id = tok;
      stmt_short_var_decl->init = expr;

      return (stmt_t *) stmt_short_var_decl;
    }

    token_free(lpar);
  }

  throw_syntax_error(reader, start, E0100);
}
