/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_STMT_HPP
#define SRC_PARSER_STMT_HPP

#include "ParserExpr.hpp"
#include "ParserType.hpp"

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

struct ParserStmtBreak {
};

struct ParserStmtContinue {
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
  ParserBlock body;
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
