/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_STMT_HPP
#define SRC_STMT_HPP

#include <vector>
#include "Expr.hpp"

struct Stmt;

enum StmtType {
  stmtAssignExpr,
  stmtCallExpr,
  stmtEnd,
  stmtMain,
  stmtShortVarDecl
};

struct StmtAssignExpr {
  Token left;
  Expr right;
};

struct StmtCallExpr {
  std::vector<Expr> args;
  Token callee;
};

struct StmtEnd {
  char reserved = '\0';
};

struct StmtMain {
  std::vector<Stmt> body;
};

struct StmtShortVarDecl {
  Token id;
  Expr init;
  bool mut = false;
};

struct Stmt {
  StmtType type;
  ReaderLocation start;
  ReaderLocation end;
  std::variant<StmtAssignExpr, StmtCallExpr, StmtEnd, StmtMain, StmtShortVarDecl> body;
};

#endif
