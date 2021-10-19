/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_HPP
#define SRC_PARSER_HPP

#include <variant>
#include <vector>
#include "Lexer.hpp"

struct Stmt;
struct StmtExpr;

enum ExprType {
  EXPR_ASSIGN,
  EXPR_BINARY,
  EXPR_CALL,
  EXPR_UNARY
};

struct ExprAssign {
  Token *left;
  StmtExpr *right;

  ~ExprAssign ();
};

struct ExprBinary {
  StmtExpr *left;
  Token *op;
  StmtExpr *right;

  ~ExprBinary ();
};

struct ExprCall {
  Token *callee;
  std::vector<StmtExpr *> args;

  ~ExprCall ();
};

struct ExprUnary {
  StmtExpr *arg;
  Token *op;
  bool prefix = false;

  ~ExprUnary ();
};

struct Expr {
  ExprType type;
  std::variant<ExprAssign *, ExprBinary *, ExprCall *, ExprUnary *> body;

  ~Expr ();
};

struct Identifier {
  Token *name;
  ~Identifier ();
};

struct Literal {
  Token *val;
  ~Literal ();
};

enum StmtType {
  STMT_END,
  STMT_EXPR,
  STMT_MAIN,
  STMT_SHORT_VAR_DECL
};

struct StmtEnd {
};

enum StmtExprType {
  STMT_EXPR_EXPR,
  STMT_EXPR_IDENTIFIER,
  STMT_EXPR_LITERAL
};

struct StmtExpr {
  StmtExprType type;
  std::variant<Expr *, Identifier *, Literal *> body;
  bool parenthesized = false;

  ~StmtExpr ();
};

struct StmtMain {
  std::vector<Stmt *> body;
};

struct StmtShortVarDecl {
  Token *id;
  StmtExpr *init;
  bool mut = false;

  ~StmtShortVarDecl ();
};

struct Stmt {
  StmtType type;
  std::variant<StmtEnd *, StmtExpr *, StmtMain *, StmtShortVarDecl *> body;

  ~Stmt ();
};

Stmt *parse (Reader *reader);

#endif
