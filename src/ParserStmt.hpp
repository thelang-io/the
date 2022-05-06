/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_STMT_HPP
#define SRC_PARSER_STMT_HPP

#include "ParserExpr.hpp"

struct ParserStmtBreak;
struct ParserStmtContinue;
struct ParserStmtEof;
struct ParserStmtFnDecl;
struct ParserStmtIf;
struct ParserStmtLoop;
struct ParserStmtMain;
struct ParserStmtObjDecl;
struct ParserStmtReturn;
struct ParserStmtVarDecl;
struct ParserTypeFn;
struct ParserTypeId;

using ParserStmtBody = std::variant<
  ParserStmtBreak,
  ParserStmtContinue,
  ParserStmtEof,
  ParserStmtExpr,
  ParserStmtFnDecl,
  ParserStmtIf,
  ParserStmtLoop,
  ParserStmtMain,
  ParserStmtObjDecl,
  ParserStmtReturn,
  ParserStmtVarDecl
>;

struct ParserStmt {
  std::shared_ptr<ParserStmtBody> body;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

using ParserBlock = std::vector<ParserStmt>;
using ParserStmtIfCond = std::variant<ParserBlock, ParserStmtIf>;
using ParserTypeBody = std::variant<ParserTypeFn, ParserTypeId>;

struct ParserType {
  std::shared_ptr<ParserTypeBody> body;
  bool parenthesized;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

struct ParserTypeFnParam {
  ParserType type;
  bool variadic;
};

struct ParserTypeFn {
  std::vector<ParserTypeFnParam> params;
  ParserType returnType;
};

struct ParserTypeId {
  Token id;
};

struct ParserStmtBreak {
};

struct ParserStmtContinue {
};

struct ParserStmtEof {
};

struct ParserStmtFnDeclParam {
  Token id;
  std::optional<ParserType> type;
  bool variadic;
  std::optional<ParserStmtExpr> init;
};

struct ParserStmtFnDecl {
  Token id;
  std::vector<ParserStmtFnDeclParam> params;
  std::optional<ParserType> returnType;
  ParserBlock body;
};

struct ParserStmtIf {
  ParserStmt cond;
  ParserBlock body;
  std::optional<std::shared_ptr<ParserStmtIfCond>> alt;

  std::string xml (std::size_t = 0) const;
};

struct ParserStmtLoop {
  std::optional<ParserStmt> init;
  std::optional<ParserStmtExpr> cond;
  std::optional<ParserStmtExpr> upd;
  ParserBlock body;
};

struct ParserStmtMain {
  ParserBlock body;
};

struct ParserStmtObjDeclField {
  Token id;
  ParserType type;
};

struct ParserStmtObjDecl {
  Token id;
  std::vector<ParserStmtObjDeclField> fields;
};

struct ParserStmtReturn {
  std::optional<ParserStmtExpr> body;
};

struct ParserStmtVarDecl {
  Token id;
  std::optional<ParserType> type;
  std::optional<ParserStmtExpr> init;
  bool mut = false;
};

#endif
