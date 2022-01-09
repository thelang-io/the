/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_HPP
#define SRC_PARSER_HPP

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "Lexer.hpp"

struct ParserId;
struct ParserMember;
struct ParserExprAssign;
struct ParserExprBinary;
struct ParserExprCall;
struct ParserExprCond;
struct ParserExprLit;
struct ParserExprObj;
struct ParserExprUnary;
struct ParserStmtBreak;
struct ParserStmtContinue;
struct ParserStmtEnd;
struct ParserStmtExpr;
struct ParserStmtFnDecl;
struct ParserStmtIf;
struct ParserStmtLoop;
struct ParserStmtMain;
struct ParserStmtObjDecl;
struct ParserStmtReturn;
struct ParserStmtVarDecl;

using ParserStmt = std::variant<
  ParserStmtBreak,
  ParserStmtContinue,
  ParserStmtEnd,
  ParserStmtExpr,
  ParserStmtFnDecl,
  ParserStmtIf,
  ParserStmtLoop,
  ParserStmtMain,
  ParserStmtObjDecl,
  ParserStmtReturn,
  ParserStmtVarDecl
>;

using ParserBlock = std::vector<ParserStmt>;
using ParserExprAccess = std::variant<ParserId, ParserMember>;
using ParserStmtIfCond = std::variant<ParserBlock, ParserStmtIf>;

using ParserExpr = std::variant<
  ParserExprAccess,
  ParserExprAssign,
  ParserExprBinary,
  ParserExprCall,
  ParserExprCond,
  ParserExprLit,
  ParserExprObj,
  ParserExprUnary
>;

struct ParserId {
  Token name;
};

struct ParserMember {
  std::shared_ptr<ParserExprAccess> obj;
  ParserId prop;
};

struct ParserStmtExpr {
  std::shared_ptr<ParserExpr> expr;
  bool parenthesized = false;
};

struct ParserExprAssign {
  ParserExprAccess left;
  Token op;
  ParserStmtExpr right;
};

struct ParserExprBinary {
  ParserStmtExpr left;
  Token op;
  ParserStmtExpr right;
};

struct ParserExprCallArg {
  std::optional<ParserId> id;
  ParserStmtExpr expr;
};

struct ParserExprCall {
  ParserExprAccess callee;
  std::vector<ParserExprCallArg> args;
};

struct ParserExprCond {
  ParserStmtExpr cond;
  ParserStmtExpr body;
  ParserStmtExpr alt;
};

struct ParserExprLit {
  Token val;
};

struct ParserExprObjProp {
  ParserId id;
  ParserStmtExpr init;
};

struct ParserExprObj {
  ParserId id;
  std::vector<ParserExprObjProp> props;
};

struct ParserExprUnary {
  ParserStmtExpr arg;
  Token op;
  bool prefix = false;
};

struct ParserStmtBreak {
};

struct ParserStmtContinue {
};

struct ParserStmtEnd {
};

struct ParserStmtFnDeclParam {
  ParserId id;
  std::optional<ParserId> type;
  std::optional<ParserStmtExpr> init;
};

struct ParserStmtFnDecl {
  ParserId id;
  ParserId returnType;
  std::vector<ParserStmtFnDeclParam> params;
  ParserBlock body;
};

struct ParserStmtIf {
  ParserStmtExpr cond;
  ParserBlock body;
  std::optional<std::shared_ptr<ParserStmtIfCond>> alt;
};

struct ParserStmtLoop {
  std::optional<std::shared_ptr<ParserStmt>> init;
  std::optional<ParserStmtExpr> cond;
  std::optional<ParserStmtExpr> upd;
  ParserBlock body;
};

struct ParserStmtMain {
  ParserBlock body;
};

struct ParserStmtObjDeclField {
  ParserId id;
  ParserId type;
};

struct ParserStmtObjDecl {
  ParserId id;
  std::vector<ParserStmtObjDeclField> fields;
};

struct ParserStmtReturn {
  ParserStmtExpr arg;
};

struct ParserStmtVarDecl {
  ParserId id;
  std::optional<ParserId> type;
  std::optional<ParserStmtExpr> init;
  bool mut = false;
};

ParserStmt parse (Reader *);

#endif
