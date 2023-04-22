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

#ifndef SRC_PARSER_STMT_HPP
#define SRC_PARSER_STMT_HPP

#include "ParserExpr.hpp"

struct ParserCatchClause;
struct ParserStmtBreak;
struct ParserStmtComment;
struct ParserStmtContinue;
struct ParserStmtEmpty;
struct ParserStmtEnumDecl;
struct ParserStmtEof;
struct ParserStmtFnDecl;
struct ParserStmtIf;
struct ParserStmtLoop;
struct ParserStmtMain;
struct ParserStmtObjDecl;
struct ParserStmtReturn;
struct ParserStmtThrow;
struct ParserStmtTry;
struct ParserStmtTypeDecl;
struct ParserStmtVarDecl;

using ParserStmtBody = std::variant<
  ParserStmtBreak,
  ParserStmtComment,
  ParserStmtContinue,
  ParserStmtEmpty,
  ParserStmtEnumDecl,
  ParserStmtEof,
  ParserStmtExpr,
  ParserStmtFnDecl,
  ParserStmtIf,
  ParserStmtLoop,
  ParserStmtMain,
  ParserStmtObjDecl,
  ParserStmtReturn,
  ParserStmtThrow,
  ParserStmtTry,
  ParserStmtTypeDecl,
  ParserStmtVarDecl
>;

struct ParserStmt {
  std::shared_ptr<ParserStmtBody> body;
  ReaderLocation start;
  ReaderLocation end;
  std::optional<ParserStmt *> nextSibling = std::nullopt;
  std::optional<ParserStmt *> prevSibling = std::nullopt;

  std::string doc (const std::string & = "") const;
  std::string xml (std::size_t = 0) const;
};

using ParserBlock = std::vector<ParserStmt>;

struct ParserCatchClause {
  ParserStmt param;
  ParserBlock body;

  std::string xml (std::size_t = 0) const;
};

struct ParserStmtBreak {
};

struct ParserStmtComment {
  std::string content;
};

struct ParserStmtContinue {
};

struct ParserStmtEmpty {
};

struct ParserStmtEnumDeclMember {
  Token id;
  std::optional<ParserStmtExpr> init;
};

struct ParserStmtEnumDecl {
  Token id;
  std::vector<ParserStmtEnumDeclMember> members;
};

struct ParserStmtEof {
};

struct ParserStmtFnDeclParam {
  Token id;
  std::optional<ParserType> type;
  bool mut;
  bool variadic;
  std::optional<ParserStmtExpr> init;
};

struct ParserStmtFnDecl {
  Token id;
  std::vector<ParserStmtFnDeclParam> params;
  std::optional<ParserType> returnType;
  std::optional<ParserBlock> body;
};

struct ParserStmtIf {
  ParserStmtExpr cond;
  ParserBlock body;
  std::optional<std::variant<ParserBlock, ParserStmt>> alt;
};

struct ParserStmtLoop {
  std::optional<ParserStmt> init;
  std::optional<ParserStmtExpr> cond;
  std::optional<ParserStmtExpr> upd;
  ParserBlock body;
  bool parenthesized;
};

struct ParserStmtMain {
  ParserBlock body;
};

struct ParserStmtObjDecl {
  Token id;
  ParserBlock members;
};

struct ParserStmtReturn {
  std::optional<ParserStmtExpr> body;
};

// todo change the way throw is handled to expr
struct ParserStmtThrow {
  ParserStmtExpr arg;
};

struct ParserStmtTry {
  ParserBlock body;
  std::vector<ParserCatchClause> handlers;
  std::optional<ParserBlock> finalizer;
};

struct ParserStmtTypeDecl {
  Token id;
  ParserType type;
};

struct ParserStmtVarDecl {
  Token id;
  std::optional<ParserType> type;
  std::optional<ParserStmtExpr> init;
  bool mut = false;
  bool constant = false;
};

#endif
