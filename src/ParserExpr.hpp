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

struct ParserStmt;
struct ParserExprAccess;
struct ParserExprArray;
struct ParserExprAs;
struct ParserExprAssign;
struct ParserExprAwait;
struct ParserExprBinary;
struct ParserExprCall;
struct ParserExprClosure;
struct ParserExprCond;
struct ParserExprIs;
struct ParserExprLit;
struct ParserExprMap;
struct ParserExprObj;
struct ParserExprRef;
struct ParserExprUnary;

using ParserExpr = std::variant<
  ParserExprAccess,
  ParserExprArray,
  ParserExprAs,
  ParserExprAssign,
  ParserExprAwait,
  ParserExprBinary,
  ParserExprCall,
  ParserExprClosure,
  ParserExprCond,
  ParserExprIs,
  ParserExprLit,
  ParserExprMap,
  ParserExprObj,
  ParserExprRef,
  ParserExprUnary
>;

struct ParserStmtExpr {
  std::shared_ptr<ParserExpr> body;
  bool parenthesized = false;
  ReaderLocation start;
  ReaderLocation end;

  std::string stringify () const;
  std::string xml (std::size_t = 0) const;
};

struct ParserFnParam {
  Token id;
  std::optional<ParserType> type = std::nullopt;
  bool mut = false;
  bool variadic = false;
  std::optional<ParserStmtExpr> init = std::nullopt;
};

struct ParserExprAccess {
  std::optional<std::variant<Token, ParserStmtExpr>> expr;
  std::optional<ParserStmtExpr> elem;
  std::optional<Token> prop;
};

struct ParserExprArray {
  std::vector<ParserStmtExpr> elements;
};

struct ParserExprAs {
  ParserStmtExpr expr;
  ParserType type;
};

struct ParserExprAssign {
  ParserStmtExpr left;
  Token op;
  ParserStmtExpr right;
};

struct ParserExprAwait {
  ParserStmtExpr arg;
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

struct ParserExprClosure {
  std::vector<ParserFnParam> params;
  std::optional<ParserType> returnType;
  std::shared_ptr<std::vector<ParserStmt>> body;
  bool async;
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

struct ParserExprMapProp {
  ParserStmtExpr name;
  ParserStmtExpr init;
};

struct ParserExprMap {
  std::vector<ParserExprMapProp> props;
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
