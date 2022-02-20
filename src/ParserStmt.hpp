/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_STMT_HPP
#define SRC_PARSER_STMT_HPP

#include "ParserExpr.hpp"

struct ParserStmt;
struct ParserStmtBreak;
struct ParserStmtContinue;
struct ParserStmtEof;
struct ParserStmtFnDecl;
struct ParserStmtFnDeclParam;
struct ParserStmtIf;
struct ParserStmtLoop;
struct ParserStmtMain;
struct ParserStmtObjDecl;
struct ParserStmtObjDeclField;
struct ParserStmtReturn;
struct ParserStmtVarDecl;

using ParserBlock = std::vector<ParserStmt>;

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

using ParserStmtIfCond = std::variant<ParserBlock, ParserStmtIf>;

struct ParserStmtBreak {
};

struct ParserStmtContinue {
};

struct ParserStmtEof {
};

struct ParserStmtFnDecl {
  Token id;
  Token returnType;
  std::vector<ParserStmtFnDeclParam> params;
  ParserBlock body;
};

struct ParserStmtFnDeclParam {
  Token id;
  std::optional<Token> type;
  std::optional<ParserStmtExpr> init;
};

struct ParserStmtIf {
  std::shared_ptr<ParserStmt> cond;
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

struct ParserStmtObjDecl {
  Token id;
  std::vector<ParserStmtObjDeclField> fields;
};

struct ParserStmtObjDeclField {
  Token id;
  Token type;
};

struct ParserStmtReturn {
  ParserStmtExpr arg;
};

struct ParserStmtVarDecl {
  Token id;
  std::optional<Token> type;
  std::optional<ParserStmtExpr> init;
  bool mut = false;
};

struct ParserStmt {
  ParserStmtBody body;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

#endif
