/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_PARSER_EXPR_HPP
#define SRC_PARSER_EXPR_HPP

#include <memory>
#include <variant>
#include <vector>
#include "Lexer.hpp"
#include "ParserType.hpp"

struct ParserExprAccess;
struct ParserExprArray;
struct ParserExprAssign;
struct ParserExprBinary;
struct ParserExprCall;
struct ParserExprCond;
struct ParserExprIs;
struct ParserExprLit;
struct ParserExprObj;
struct ParserExprRef;
struct ParserExprUnary;

using ParserExpr = std::variant<
  ParserExprAccess,
  ParserExprArray,
  ParserExprAssign,
  ParserExprBinary,
  ParserExprCall,
  ParserExprCond,
  ParserExprIs,
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
  std::optional<std::variant<Token, ParserStmtExpr>> expr;
  std::optional<ParserStmtExpr> elem;
  std::optional<Token> prop;
};

struct ParserExprArray {
  std::vector<ParserStmtExpr> elements;
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

struct ParserExprIs {
  ParserStmtExpr expr;
  ParserType type;
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
