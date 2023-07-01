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

#ifndef SRC_CODEGEN_AST_CODEGEN_AST_HPP
#define SRC_CODEGEN_AST_CODEGEN_AST_HPP

#include <optional>
#include <string>
#include <variant>
#include <vector>

struct CodegenASTExprAccess;
struct CodegenASTExprAssign;
struct CodegenASTExprBinary;
struct CodegenASTExprCall;
struct CodegenASTExprCast;
struct CodegenASTExprCond;
struct CodegenASTExprInitList;
struct CodegenASTExprLiteral;
struct CodegenASTExprUnary;

struct CodegenASTStmtBreak;
struct CodegenASTStmtCase;
struct CodegenASTStmtCompound;
struct CodegenASTStmtContinue;
struct CodegenASTStmtDefault;
struct CodegenASTStmtEnumDecl;
struct CodegenASTStmtExpr;
struct CodegenASTStmtFnDecl;
struct CodegenASTStmtFor;
struct CodegenASTStmtGoto;
struct CodegenASTStmtIf;
struct CodegenASTStmtLabel;
struct CodegenASTStmtMacroCondition;
struct CodegenASTStmtMacroInclude;
struct CodegenASTStmtMacroReplace;
struct CodegenASTStmtNull;
struct CodegenASTStmtReturn;
struct CodegenASTStmtStructDecl;
struct CodegenASTStmtSwitch;
struct CodegenASTStmtVarDecl;
struct CodegenASTStmtWhile;

using CodegenASTStmtBody = std::variant<
  CodegenASTStmtBreak,
  CodegenASTStmtCase,
  CodegenASTStmtCompound,
  CodegenASTStmtContinue,
  CodegenASTStmtDefault,
  CodegenASTStmtEnumDecl,
  CodegenASTStmtExpr,
  CodegenASTStmtFnDecl,
  CodegenASTStmtFor,
  CodegenASTStmtGoto,
  CodegenASTStmtIf,
  CodegenASTStmtLabel,
  CodegenASTStmtMacroCondition,
  CodegenASTStmtMacroInclude,
  CodegenASTStmtMacroReplace,
  CodegenASTStmtNull,
  CodegenASTStmtReturn,
  CodegenASTStmtStructDecl,
  CodegenASTStmtSwitch,
  CodegenASTStmtVarDecl,
  CodegenASTStmtWhile
>;

using CodegenASTExprBody = std::variant<
  CodegenASTExprAccess,
  CodegenASTExprAssign,
  CodegenASTExprBinary,
  CodegenASTExprCall,
  CodegenASTExprCast,
  CodegenASTExprCond,
  CodegenASTExprInitList,
  CodegenASTExprLiteral,
  CodegenASTExprUnary
>;

struct CodegenASTType {
  std::string val;

  static CodegenASTType create (const std::string &);
  std::string str () const;
};

struct CodegenASTStmt {
  CodegenASTStmt *parent;
  CodegenASTStmt *prevSibling;
  CodegenASTStmt *nextSibling;
  std::shared_ptr<CodegenASTStmtBody> body;

  CodegenASTStmt &append (const CodegenASTStmt &);
  const CodegenASTStmtCompound &asCompound () const;
  CodegenASTStmt &exit () const;
  bool isNull () const;
  void merge (const std::vector<CodegenASTStmt> &);
  CodegenASTStmt &prepend (const CodegenASTStmt &);
  std::string str () const;
};

struct CodegenASTExpr {
  std::shared_ptr<CodegenASTExprBody> body;
  bool parenthesized = false;
  CodegenASTExpr *parent = nullptr;
  CodegenASTStmt *parentStmt = nullptr;

  CodegenASTExprAccess &asAccess ();
  const CodegenASTExprAccess &asAccess () const;
  CodegenASTExprAssign &asAssign ();
  const CodegenASTExprAssign &asAssign () const;
  CodegenASTExprBinary &asBinary ();
  const CodegenASTExprBinary &asBinary () const;
  CodegenASTExprCall &asCall ();
  const CodegenASTExprCall &asCall () const;
  CodegenASTExprCast &asCast ();
  const CodegenASTExprCast &asCast () const;
  CodegenASTExprCond &asCond ();
  const CodegenASTExprCond &asCond () const;
  CodegenASTExprInitList &asInitList ();
  const CodegenASTExprInitList &asInitList () const;
  CodegenASTExprLiteral &asLiteral ();
  const CodegenASTExprLiteral &asLiteral () const;
  CodegenASTExprUnary &asUnary ();
  const CodegenASTExprUnary &asUnary () const;
  bool isAccess () const;
  bool isAssign () const;
  bool isBinary () const;
  bool isCall () const;
  bool isCast () const;
  bool isCond () const;
  bool isEmptyString () const;
  bool isInitList () const;
  bool isLiteral () const;
  bool isPointer () const;
  bool isUnary () const;
  void link (CodegenASTExpr *);
  CodegenASTStmt stmt () const;
  std::string str () const;
  CodegenASTExpr wrap () const;
};

struct CodegenASTExprAccess {
  std::variant<CodegenASTExpr, std::string> obj;
  std::optional<std::string> prop = std::nullopt;
  std::optional<CodegenASTExpr> elem = std::nullopt;
  bool pointed = false;

  static CodegenASTExpr create (const std::string &);
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, bool = false);
  static CodegenASTExpr create (const CodegenASTExpr &, const CodegenASTExpr &);
  std::string str () const;
};

struct CodegenASTExprAssign {
  CodegenASTExpr left;
  std::string op;
  CodegenASTExpr right;

  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, const CodegenASTExpr &);
  std::string str () const;
};

struct CodegenASTExprBinary {
  CodegenASTExpr left;
  std::string op;
  CodegenASTExpr right;

  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, const CodegenASTExpr &);
  std::string str () const;
};

struct CodegenASTExprCall {
  CodegenASTExpr callee;
  std::vector<CodegenASTExpr> exprArgs = {};
  std::vector<CodegenASTType> typeArgs = {};

  static CodegenASTExpr create (const CodegenASTExpr &);
  static CodegenASTExpr create (const CodegenASTExpr &, const std::vector<CodegenASTExpr> &);
  static CodegenASTExpr create (const CodegenASTExpr &, const std::vector<CodegenASTType> &);
  std::string str () const;
};

struct CodegenASTExprCast {
  CodegenASTType type;
  CodegenASTExpr arg;

  static CodegenASTExpr create (const CodegenASTType &, const CodegenASTExpr &);
  std::string str () const;
};

struct CodegenASTExprCond {
  CodegenASTExpr cond;
  CodegenASTExpr body;
  CodegenASTExpr alt;

  static CodegenASTExpr create (const CodegenASTExpr &, const CodegenASTExpr &, const CodegenASTExpr &);
  std::string str () const;
};

struct CodegenASTExprInitList {
  std::vector<CodegenASTExpr> items;

  static CodegenASTExpr create (const std::vector<CodegenASTExpr> & = {});
  std::string str () const;
};

struct CodegenASTExprLiteral {
  std::string val;

  static CodegenASTExpr create (const std::string &);
  std::string str () const;
};

struct CodegenASTExprUnary {
  CodegenASTExpr arg;
  std::string op;
  bool prefix = false;

  static CodegenASTExpr create (const std::string &, const CodegenASTExpr &);
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &);
  std::string str () const;
};

struct CodegenASTStmtBreak {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtCase {
  static CodegenASTStmt create (const CodegenASTExpr &, const std::optional<CodegenASTStmt> & = std::nullopt);
};

struct CodegenASTStmtCompound {
  std::vector<CodegenASTStmt> items;

  static CodegenASTStmt create ();
  static CodegenASTStmt create (const std::vector<CodegenASTStmt> &);
};

struct CodegenASTStmtContinue {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtDefault {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtEnumDecl {
};

struct CodegenASTStmtExpr {
  static CodegenASTStmt create (const CodegenASTExpr &);
};

struct CodegenASTStmtFnParam {
};

struct CodegenASTStmtFnDecl {
  std::string name;
  std::vector<CodegenASTStmtFnParam> params;
  CodegenASTStmt body;

  static CodegenASTStmt create (const std::string &, const std::vector<CodegenASTStmtFnParam> &, const CodegenASTStmt &);
};

struct CodegenASTStmtFor {
  static CodegenASTStmt create (
    const std::optional<CodegenASTStmt> & = std::nullopt,
    const std::optional<CodegenASTExpr> & = std::nullopt,
    const std::optional<CodegenASTExpr> & = std::nullopt,
    const CodegenASTStmt & = CodegenASTStmtCompound::create()
  );
};

struct CodegenASTStmtGoto {
  static CodegenASTStmt create (const std::string &);
};

struct CodegenASTStmtIf {
  static CodegenASTStmt create (const CodegenASTExpr &, const CodegenASTStmt &, const std::optional<CodegenASTStmt> & = std::nullopt);
};

struct CodegenASTStmtLabel {
  static CodegenASTStmt create (const std::string &);
};

struct CodegenASTStmtMacroCondition {
};

struct CodegenASTStmtMacroInclude {
};

struct CodegenASTStmtMacroReplace {
};

struct CodegenASTStmtNull {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtReturn {
  static CodegenASTStmt create ();
  static CodegenASTStmt create (const CodegenASTExpr &);
  static CodegenASTStmt create (const std::shared_ptr<std::size_t> &);
};

struct CodegenASTStmtStructDecl {
};

struct CodegenASTStmtSwitch {
  static CodegenASTStmt create (const CodegenASTExpr &, const CodegenASTStmt & = CodegenASTStmtCompound::create());
};

struct CodegenASTStmtVarDecl {
  static CodegenASTStmt create (const CodegenASTType &, const CodegenASTExpr &, const std::optional<CodegenASTExpr> & = std::nullopt);
};

struct CodegenASTStmtWhile {
  static CodegenASTStmt create (const CodegenASTExpr &, const std::optional<CodegenASTStmt> & = std::nullopt);
};

#endif
