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

#ifndef SRC_PARSER_HPP
#define SRC_PARSER_HPP

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "Lexer.hpp"

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
struct ParserStmtBreak;
struct ParserStmtComment;
struct ParserStmtContinue;
struct ParserStmtEmpty;
struct ParserStmtEnumDecl;
struct ParserStmtEof;
struct ParserStmtExportDecl;
struct ParserStmtExpr;
struct ParserStmtFnDecl;
struct ParserStmtIf;
struct ParserStmtImportDecl;
struct ParserStmtLoop;
struct ParserStmtMain;
struct ParserStmtObjDecl;
struct ParserStmtReturn;
struct ParserStmtThrow;
struct ParserStmtTry;
struct ParserStmtTypeDecl;
struct ParserStmtVarDecl;
struct ParserTypeArray;
struct ParserTypeFn;
struct ParserTypeId;
struct ParserTypeMap;
struct ParserTypeOptional;
struct ParserTypeRef;
struct ParserTypeUnion;

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

using ParserStmtBody = std::variant<
  ParserStmtBreak,
  ParserStmtComment,
  ParserStmtContinue,
  ParserStmtEmpty,
  ParserStmtEnumDecl,
  ParserStmtEof,
  ParserStmtExportDecl,
  ParserStmtExpr,
  ParserStmtFnDecl,
  ParserStmtIf,
  ParserStmtImportDecl,
  ParserStmtLoop,
  ParserStmtMain,
  ParserStmtObjDecl,
  ParserStmtReturn,
  ParserStmtThrow,
  ParserStmtTry,
  ParserStmtTypeDecl,
  ParserStmtVarDecl
>;

using ParserTypeBody = std::variant<
  ParserTypeArray,
  ParserTypeFn,
  ParserTypeId,
  ParserTypeMap,
  ParserTypeOptional,
  ParserTypeRef,
  ParserTypeUnion
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

struct ParserStmtExpr {
  std::shared_ptr<ParserExpr> body;
  bool parenthesized = false;
  ReaderLocation start;
  ReaderLocation end;

  std::string stringify () const;
  std::string xml (std::size_t = 0) const;
};

struct ParserType {
  std::shared_ptr<ParserTypeBody> body;
  bool parenthesized;
  ReaderLocation start;
  ReaderLocation end;

  std::string stringify () const;
  std::string xml (std::size_t = 0) const;
};

using ParserBlock = std::vector<ParserStmt>;

struct ParserCatchClause {
  ParserStmt param;
  ParserBlock body;

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
  ParserType returnType;
  ParserBlock body;
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

struct ParserStmtExportDecl {
  ParserStmt declaration;
};

struct ParserStmtFnDecl {
  Token id;
  std::vector<ParserFnParam> params;
  std::optional<ParserType> returnType;
  std::optional<ParserBlock> body;
  bool async;
};

struct ParserStmtIf {
  ParserStmtExpr cond;
  ParserBlock body;
  std::optional<std::variant<ParserBlock, ParserStmt>> alt;
};

struct ParserStmtImportDeclSpecifier {
  std::optional<ParserStmtExpr> imported;
  ParserStmtExpr local;
};

struct ParserStmtImportDecl {
  std::vector<ParserStmtImportDeclSpecifier> specifiers;
  ParserStmtExpr source;
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

struct ParserStmtThrow {
  ParserStmtExpr arg;
};

struct ParserStmtTry {
  ParserBlock body;
  std::vector<ParserCatchClause> handlers;
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

struct ParserTypeArray {
  ParserType elementType;
};

struct ParserTypeFnParam {
  std::optional<Token> id;
  ParserType type;
  bool mut;
  bool variadic;
};

struct ParserTypeFn {
  std::vector<ParserTypeFnParam> params;
  ParserType returnType;
  bool async;
};

struct ParserTypeId {
  Token id;
};

struct ParserTypeMap {
  ParserType keyType;
  ParserType valueType;
};

struct ParserTypeOptional {
  ParserType type;
};

struct ParserTypeRef {
  ParserType refType;
};

struct ParserTypeUnion {
  std::vector<ParserType> subTypes;
};

class Parser {
 public:
  Lexer *lexer;
  Reader *reader;

  explicit Parser (Lexer *);

  virtual std::string doc ();
  virtual ParserStmt next (bool = true, bool = false);
  virtual std::string xml ();

 private:
  Parser (const Parser &);
  Parser &operator= (const Parser &);

  ParserBlock _block (bool = false);
  ParserStmtExpr _exprClosure (bool, const Token &, bool = false);
  std::vector<ParserFnParam> _fnParamList ();
  std::optional<ParserStmtExpr> _stmtExpr (bool = true);
  std::optional<ParserType> _type ();
  std::tuple<ParserStmtExpr, bool> _wrapExpr (
    const ParserStmtExpr &,
    ReaderLocation,
    const Token &,
    int,
    const std::function<std::tuple<ParserStmtExpr, bool> (const ParserStmtExpr &, ReaderLocation, const Token &)> &
  );
  std::tuple<ParserStmtExpr, bool> _wrapExprAccess (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprAs (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprAssign (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprBinary (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCall (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCond (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprIs (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprObj (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprUnary (const ParserStmtExpr &, ReaderLocation, const Token &);
  ParserStmt _wrapStmt (bool, const ParserStmtBody &, ReaderLocation) const;
  ParserStmtExpr _wrapStmtExpr (const ParserStmtExpr &);
  ParserStmt _wrapStmtLoop (bool, const Token &, bool, bool = false);
  ParserType _wrapType (const ParserType &);
};

#endif
