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
struct ParserExprCallArg;
struct ParserExprCond;
struct ParserExprLit;
struct ParserExprObj;
struct ParserExprObjProp;
struct ParserExprUnary;
struct ParserMember;
struct ParserStmtExpr;

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

using ParserMemberObj = std::variant<Token, std::shared_ptr<ParserMember>>;

struct ParserMember {
  ParserMemberObj obj;
  Token prop;
};

struct ParserExprAccess {
  ParserMemberObj body;

  std::string xml (std::size_t = 0) const;
};

struct ParserExprAssign {
  ParserExprAccess left;
  Token op;
  std::shared_ptr<ParserStmtExpr> right;
};

struct ParserExprBinary {
  std::shared_ptr<ParserStmtExpr> left;
  Token op;
  std::shared_ptr<ParserStmtExpr> right;
};

struct ParserExprCall {
  ParserExprAccess callee;
  std::vector<ParserExprCallArg> args;
};

struct ParserExprCallArg {
  std::optional<Token> id;
  std::shared_ptr<ParserStmtExpr> expr;
};

struct ParserExprCond {
  std::shared_ptr<ParserStmtExpr> cond;
  std::shared_ptr<ParserStmtExpr> body;
  std::shared_ptr<ParserStmtExpr> alt;
};

struct ParserExprLit {
  Token body;
};

struct ParserExprObj {
  Token id;
  std::vector<ParserExprObjProp> props;
};

struct ParserExprObjProp {
  Token id;
  std::shared_ptr<ParserStmtExpr> init;
};

struct ParserExprUnary {
  std::shared_ptr<ParserStmtExpr> arg;
  Token op;
  bool prefix = false;
};

struct ParserStmtExpr {
  ParserExpr body;
  bool parenthesized = false;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

#endif
