/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_EXPR_HPP
#define SRC_PARSER_EXPR_HPP

#include <memory>
#include <variant>
#include <vector>
#include "Lexer.hpp"

struct ParserExprAccess;
struct ParserExprAssign;
struct ParserExprBinary;
struct ParserExprCall;
struct ParserExprCond;
struct ParserExprLit;
struct ParserExprObj;
struct ParserExprRef;
struct ParserExprUnary;

using ParserExpr = std::variant<
  ParserExprAccess,
  ParserExprAssign,
  ParserExprBinary,
  ParserExprCall,
  ParserExprCond,
  ParserExprLit,
  ParserExprObj,
  ParserExprRef,
  ParserExprUnary
>;

struct ParserStmtExpr {
  std::shared_ptr<ParserExpr> body;
  bool parenthesized = false;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

struct ParserExprAccess {
  std::variant<Token, ParserStmtExpr> expr;
  std::optional<Token> prop;
};

struct ParserExprAssign {
  ParserStmtExpr left;
  Token op;
  ParserStmtExpr right;
};

struct ParserExprBinary {
  ParserStmtExpr left;
  Token op;
  ParserStmtExpr right;
};

struct ParserExprCallArg {
  std::optional<Token> id;
  ParserStmtExpr expr;
};

struct ParserExprCall {
  ParserStmtExpr callee;
  std::vector<ParserExprCallArg> args;
};

struct ParserExprCond {
  ParserStmtExpr cond;
  ParserStmtExpr body;
  ParserStmtExpr alt;
};

struct ParserExprLit {
  Token body;
};

struct ParserExprObjProp {
  Token id;
  ParserStmtExpr init;
};

struct ParserExprObj {
  Token id;
  std::vector<ParserExprObjProp> props;
};

struct ParserExprRef {
  ParserStmtExpr expr;
};

struct ParserExprUnary {
  ParserStmtExpr arg;
  Token op;
  bool prefix = false;
};

#endif
