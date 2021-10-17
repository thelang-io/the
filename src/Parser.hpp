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
  EXPR_CALL
};

struct ExprAssign {
  Token *left;
  StmtExpr *right;

  ~ExprAssign ();
};

struct ExprCall {
  std::vector<StmtExpr *> args;
  Token *callee;

  ~ExprCall ();
};

struct Expr {
  ExprType type;
  std::variant<ExprAssign *, ExprCall *> body;

  inline ~Expr () {
    if (this->type == EXPR_ASSIGN) {
      delete std::get<ExprAssign *>(this->body);
    } else if (this->type == EXPR_CALL) {
      delete std::get<ExprCall *>(this->body);
    }
  }
};

struct Identifier {
  Token *name;

  inline ~Identifier () {
    delete this->name;
  }
};

struct Literal {
  Token *val;

  inline ~Literal () {
    delete this->val;
  }
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

  inline ~StmtExpr () {
    if (this->type == STMT_EXPR_EXPR) {
      delete std::get<Expr *>(this->body);
    } else if (this->type == STMT_EXPR_IDENTIFIER) {
      delete std::get<Identifier *>(this->body);
    } else if (this->type == STMT_EXPR_LITERAL) {
      delete std::get<Literal *>(this->body);
    }
  }
};

struct StmtMain {
  std::vector<Stmt *> body;
};

struct StmtShortVarDecl {
  Token *id;
  StmtExpr *init;
  bool mut = false;

  inline ~StmtShortVarDecl () {
    delete this->id;
    delete this->init;
  }
};

struct Stmt {
  StmtType type;
  std::variant<StmtEnd *, StmtExpr *, StmtMain *, StmtShortVarDecl *> body;

  inline ~Stmt () {
    if (this->type == STMT_END) {
      delete std::get<StmtEnd *>(this->body);
    } else if (this->type == STMT_EXPR) {
      delete std::get<StmtExpr *>(this->body);
    } else if (this->type == STMT_MAIN) {
      delete std::get<StmtMain *>(this->body);
    } else if (this->type == STMT_SHORT_VAR_DECL) {
      delete std::get<StmtShortVarDecl *>(this->body);
    }
  }
};

Stmt *parse (Reader *reader);

#endif
