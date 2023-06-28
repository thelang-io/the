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

  CodegenASTStmt &append (CodegenASTStmt);
  CodegenASTStmt &exit ();
  CodegenASTStmt &prepend (CodegenASTStmt);
};

struct CodegenASTExpr {
  bool parenthesized;

  CodegenASTStmt stmt ();
  CodegenASTExpr wrap ();
};

struct CodegenASTExprAccess {
  static CodegenASTExpr create (const std::string &);
};

struct CodegenASTExprAssign {
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, const CodegenASTExpr &);
};

struct CodegenASTExprBinary {
  static CodegenASTExpr create (const CodegenASTExpr &, const std::string &, const CodegenASTExpr &);
};

struct CodegenASTExprLiteral {
  static CodegenASTExpr create (const std::string &);
};

struct CodegenASTExprUnary {
  static CodegenASTExpr create (const std::string &, const CodegenASTExpr &);
};

struct CodegenASTStmtBreak {
  static CodegenASTStmt create ();
};

struct CodegenASTStmtCase {
  static CodegenASTStmt create (const std::string &, std::optional<CodegenASTStmt> = std::nullopt);
};

struct CodegenASTStmtCompound {
  static CodegenASTStmt create ();
  static CodegenASTStmt create (std::vector<CodegenASTStmt>);
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
  CodegenASTStmtCompound body;

  static CodegenASTStmt create (const std::string &, std::vector<CodegenASTStmtFnParam> &, CodegenASTStmtCompound);
};

struct CodegenASTStmtFor {
  static CodegenASTStmt create (
    std::optional<CodegenASTStmt> = std::nullopt,
    std::optional<CodegenASTExpr> = std::nullopt,
    std::optional<CodegenASTExpr> = std::nullopt,
    CodegenASTStmt = CodegenASTStmtCompound::create()
  );
};

struct CodegenASTStmtGoto {
  static CodegenASTStmt create (const std::string &);
};

struct CodegenASTStmtIf {
  static CodegenASTStmt create (CodegenASTExpr, CodegenASTStmt, std::optional<CodegenASTStmt> = std::nullopt);
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
  static CodegenASTStmt create (CodegenASTExpr);
  static CodegenASTStmt create (std::shared_ptr<std::size_t>);
};

struct CodegenASTStmtStructDecl {
};

struct CodegenASTStmtSwitch {
};

struct CodegenASTStmtVarDecl {
  static CodegenASTStmt create (CodegenASTType, CodegenASTExpr, std::optional<CodegenASTExpr> = std::nullopt);
};

struct CodegenASTStmtWhile {
  static CodegenASTStmt create (CodegenASTExpr, std::optional<CodegenASTStmt> = std::nullopt);
};

#endif
