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
  static CodegenASTType create (const std::string &);
};

struct CodegenASTStmt {
  CodegenASTStmt *parent;
  CodegenASTStmt *prevSibling;
  CodegenASTStmt *nextSibling;
  std::shared_ptr<CodegenASTStmtBody> body;

  CodegenASTStmt &append (const CodegenASTStmt &);
  CodegenASTStmt &exit () const;
  CodegenASTStmt &prepend (const CodegenASTStmt &);
  std::string str ();
};

struct CodegenASTExpr {
  bool parenthesized;
  CodegenASTExpr *parent;
  CodegenASTStmt *parentStmt;
  std::shared_ptr<CodegenASTExprBody> body;

  CodegenASTStmt stmt ();
  CodegenASTExpr wrap ();
};

struct CodegenASTExprAccess {
  static CodegenASTExpr create (const std::string &);
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, bool = false);
  static CodegenASTExpr create (const CodegenASTExpr &, const CodegenASTExpr &);
};

struct CodegenASTExprAssign {
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, const CodegenASTExpr &);
};

struct CodegenASTExprBinary {
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, const CodegenASTExpr &);
};

struct CodegenASTExprCall {
  static CodegenASTExpr create (const CodegenASTExpr &, const std::vector<CodegenASTExpr> &);
};

struct CodegenASTExprCast {
  static CodegenASTExpr create (const CodegenASTType &, const CodegenASTExpr &);
};

struct CodegenASTExprCond {
  static CodegenASTExpr create (const CodegenASTExpr &, const CodegenASTExpr &, const CodegenASTExpr &);
};

struct CodegenASTExprInitList {
  static CodegenASTExpr create (const std::vector<CodegenASTExpr> &);
};

struct CodegenASTExprLiteral {
  static CodegenASTExpr create (const std::string &);
};

struct CodegenASTExprUnary {
  static CodegenASTExpr create (const std::string &, const CodegenASTExpr &);
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &);
};

struct CodegenASTStmtBreak {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtCase {
  static CodegenASTStmt create (const CodegenASTExpr &, const std::optional<CodegenASTStmt> & = std::nullopt);
};

struct CodegenASTStmtCompound {
  static CodegenASTStmt create ();
  static CodegenASTStmt create (const std::vector<CodegenASTStmt> &);
};

struct CodegenASTStmtContinue {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtDefault {
};

struct CodegenASTStmtEnumDecl {
};

struct CodegenASTStmtExpr {
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
};

struct CodegenASTStmtMacroCondition {
};

struct CodegenASTStmtMacroInclude {
};

struct CodegenASTStmtMacroReplace {
};

struct CodegenASTStmtReturn {
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
