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

struct ExprMember;
struct Identifier;
struct Stmt;
struct StmtExpr;
struct StmtIf;

struct Block {
  std::vector<Stmt *> body;
  ~Block();
};

enum CondType {
  COND_BLOCK,
  COND_STMT_IF
};

struct Cond {
  CondType type;
  std::variant<Block *, StmtIf *> body;

  ~Cond ();
};

enum ExprType {
  EXPR_ACCESS,
  EXPR_ASSIGN,
  EXPR_BINARY,
  EXPR_CALL,
  EXPR_COND,
  EXPR_MEMBER,
  EXPR_OBJ,
  EXPR_UNARY
};

enum ExprAccessType {
  EXPR_ACCESS_EXPR_MEMBER,
  EXPR_ACCESS_IDENTIFIER
};

struct ExprAccess {
  ExprAccessType type;
  std::variant<ExprMember *, Identifier *> body;

  ~ExprAccess ();
};

struct ExprAssign {
  ExprAccess *left;
  Token *op;
  StmtExpr *right;

  ~ExprAssign ();
};

struct ExprBinary {
  StmtExpr *left;
  Token *op;
  StmtExpr *right;

  ~ExprBinary ();
};

struct ExprCallArg {
  Identifier *id;
  StmtExpr *expr;
};

struct ExprCall {
  ExprAccess *callee;
  std::vector<ExprCallArg *> args;

  ~ExprCall ();
};

struct ExprCond {
  StmtExpr *cond;
  StmtExpr *body;
  StmtExpr *alt;

  ~ExprCond ();
};

struct ExprMember {
  ExprAccess *obj;
  Identifier *prop;

  ~ExprMember ();
};

struct ExprObjProp {
  Identifier *id;
  StmtExpr *init;
};

struct ExprObj {
  Identifier *type;
  std::vector<ExprObjProp *> props;

  ~ExprObj ();
};

struct ExprUnary {
  StmtExpr *arg;
  Token *op;
  bool prefix = false;

  ~ExprUnary ();
};

struct Expr {
  ExprType type;
  std::variant<
    ExprAccess *,
    ExprAssign *,
    ExprBinary *,
    ExprCall *,
    ExprCond *,
    ExprMember *,
    ExprObj *,
    ExprUnary *
  > body;

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
  STMT_BREAK,
  STMT_CONTINUE,
  STMT_END,
  STMT_EXPR,
  STMT_FN_DECL,
  STMT_IF,
  STMT_LOOP,
  STMT_MAIN,
  STMT_OBJ_DECL,
  STMT_RETURN,
  STMT_VAR_DECL
};

struct StmtBreak {
};

struct StmtContinue {
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

struct StmtFnDeclParam {
  Identifier *id;
  Identifier *type;
  StmtExpr *init;
};

struct StmtFnDecl {
  Identifier *id;
  std::vector<StmtFnDeclParam *> params;
  Identifier *returnType;
  Block *body;

  ~StmtFnDecl ();
};

struct StmtIf {
  StmtExpr *cond;
  Block *body;
  Cond *alt;

  ~StmtIf ();
};

struct StmtLoop {
  Stmt *init;
  StmtExpr *cond;
  StmtExpr *upd;
  Block *body;

  ~StmtLoop ();
};

struct StmtMain {
  Block *body;
  ~StmtMain ();
};

struct StmtObjDeclField {
  Identifier *id;
  Identifier *type;
};

struct StmtObjDecl {
  Identifier *id;
  std::vector<StmtObjDeclField *> fields;

  ~StmtObjDecl ();
};

struct StmtReturn {
  StmtExpr *arg;
  ~StmtReturn ();
};

struct StmtVarDecl {
  Identifier *id;
  Identifier *type;
  StmtExpr *init;
  bool mut = false;

  ~StmtVarDecl ();
};

struct Stmt {
  StmtType type;
  std::variant<
    StmtBreak *,
    StmtContinue *,
    StmtEnd *,
    StmtExpr *,
    StmtFnDecl *,
    StmtIf *,
    StmtLoop *,
    StmtMain *,
    StmtObjDecl *,
    StmtReturn *,
    StmtVarDecl *
  > body;

  ~Stmt ();
};

Stmt *parse (Reader *reader);

#endif
