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
struct CodegenASTStmtEnumDecl;
struct CodegenASTStmtExpr;
struct CodegenASTStmtFnDecl;
struct CodegenASTStmtFor;
struct CodegenASTStmtGoto;
struct CodegenASTStmtIf;
struct CodegenASTStmtLabel;
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
  CodegenASTStmtEnumDecl,
  CodegenASTStmtExpr,
  CodegenASTStmtFnDecl,
  CodegenASTStmtFor,
  CodegenASTStmtGoto,
  CodegenASTStmtIf,
  CodegenASTStmtLabel,
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

std::string notImplementedStr ([[maybe_unused]] std::size_t indent, [[maybe_unused]] bool root) {
  return "undefined;";
}

struct CodegenASTType {
  std::string val;

  static CodegenASTType create (const std::string &);
  std::string str () const;
};

struct CodegenASTStmt {
  std::shared_ptr<CodegenASTStmtBody> body;
  CodegenASTStmt *parent = nullptr;
  CodegenASTStmt *prevSibling = nullptr;
  CodegenASTStmt *nextSibling = nullptr;

  CodegenASTStmt &append (const CodegenASTStmt &);
  CodegenASTStmtBreak &asBreak ();
  const CodegenASTStmtBreak &asBreak () const;
  CodegenASTStmtCase &asCase ();
  const CodegenASTStmtCase &asCase () const;
  CodegenASTStmtCompound &asCompound ();
  const CodegenASTStmtCompound &asCompound () const;
  CodegenASTStmtContinue &asContinue ();
  const CodegenASTStmtContinue &asContinue () const;
  CodegenASTStmtEnumDecl &asEnumDecl ();
  const CodegenASTStmtEnumDecl &asEnumDecl () const;
  CodegenASTStmtExpr &asExpr ();
  const CodegenASTStmtExpr &asExpr () const;
  CodegenASTStmtFnDecl &asFnDecl ();
  const CodegenASTStmtFnDecl &asFnDecl () const;
  CodegenASTStmtFor &asFor ();
  const CodegenASTStmtFor &asFor () const;
  CodegenASTStmtGoto &asGoto ();
  const CodegenASTStmtGoto &asGoto () const;
  CodegenASTStmtIf &asIf ();
  const CodegenASTStmtIf &asIf () const;
  CodegenASTStmtLabel &asLabel ();
  const CodegenASTStmtLabel &asLabel () const;
  CodegenASTStmtNull &asNull ();
  const CodegenASTStmtNull &asNull () const;
  CodegenASTStmtReturn &asReturn ();
  const CodegenASTStmtReturn &asReturn () const;
  CodegenASTStmtStructDecl &asStructDecl ();
  const CodegenASTStmtStructDecl &asStructDecl () const;
  CodegenASTStmtSwitch &asSwitch ();
  const CodegenASTStmtSwitch &asSwitch () const;
  CodegenASTStmtVarDecl &asVarDecl ();
  const CodegenASTStmtVarDecl &asVarDecl () const;
  CodegenASTStmtWhile &asWhile ();
  const CodegenASTStmtWhile &asWhile () const;
  CodegenASTStmt &exit () const;
  bool isBreak () const;
  bool isCase () const;
  bool isCompound () const;
  bool isContinue () const;
  bool isEnumDecl () const;
  bool isExpr () const;
  bool isFnDecl () const;
  bool isFor () const;
  bool isGoto () const;
  bool isIf () const;
  bool isLabel () const;
  bool isNull () const;
  bool isReturn () const;
  bool isStructDecl () const;
  bool isSwitch () const;
  bool isVarDecl () const;
  bool isWhile () const;
  void merge (const std::vector<CodegenASTStmt> &);
  CodegenASTStmt &prepend (const CodegenASTStmt &);
  std::string str (std::size_t = 0, bool = true) const;
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

struct CodegenASTStmtCompound {
  std::vector<CodegenASTStmt> body;

  static CodegenASTStmt create ();
  static CodegenASTStmt create (const std::vector<CodegenASTStmt> &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtBreak {
  static CodegenASTStmt create ();
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtCase {
  std::optional<CodegenASTExpr> test;
  CodegenASTStmt body;

  static CodegenASTStmt create (
    const std::optional<CodegenASTExpr> & = std::nullopt,
    const CodegenASTStmt & = CodegenASTStmtCompound::create()
  );
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtContinue {
  static CodegenASTStmt create ();
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtEnumDecl {
  // NOLINTNEXTLINE(readability-make-member-function-const)
  inline std::string str (std::size_t indent, bool root) const { return notImplementedStr(indent, root); };
};

struct CodegenASTStmtExpr {
  CodegenASTExpr expr;

  static CodegenASTStmt create (const CodegenASTExpr &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtFnDecl {
  // NOLINTNEXTLINE(readability-make-member-function-const)
  inline std::string str (std::size_t indent, bool root) const { return notImplementedStr(indent, root); };
};

struct CodegenASTStmtFor {
  std::optional<CodegenASTStmt> init;
  std::optional<CodegenASTExpr> cond;
  std::optional<CodegenASTExpr> upd;
  std::optional<CodegenASTStmt> body;

  static CodegenASTStmt create (
    const std::optional<CodegenASTStmt> & = std::nullopt,
    const std::optional<CodegenASTExpr> & = std::nullopt,
    const std::optional<CodegenASTExpr> & = std::nullopt,
    const std::optional<CodegenASTStmt> & = CodegenASTStmtCompound::create()
  );
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtGoto {
  std::string label;

  static CodegenASTStmt create (const std::string &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtIf {
  CodegenASTExpr cond;
  CodegenASTStmt body;
  std::optional<CodegenASTStmt> alt;

  static CodegenASTStmt create (const CodegenASTExpr &, const CodegenASTStmt &, const std::optional<CodegenASTStmt> & = std::nullopt);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtLabel {
  std::string name;

  static CodegenASTStmt create (const std::string &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtNull {
  static CodegenASTStmt create ();
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtReturn {
  std::optional<CodegenASTExpr> arg = std::nullopt;
  std::optional<std::shared_ptr<std::size_t>> asyncPtr = std::nullopt;

  static CodegenASTStmt create ();
  static CodegenASTStmt create (const CodegenASTExpr &);
  static CodegenASTStmt create (const std::shared_ptr<std::size_t> &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtStructDecl {
  // NOLINTNEXTLINE(readability-make-member-function-const)
  inline std::string str (std::size_t indent, bool root) const { return notImplementedStr(indent, root); };
};

struct CodegenASTStmtSwitch {
  CodegenASTExpr discriminant;
  std::vector<CodegenASTStmt> body;

  static CodegenASTStmt create (const CodegenASTExpr &, const std::vector<CodegenASTStmt> & = {});
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtVarDecl {
  CodegenASTType type;
  CodegenASTExpr id;
  std::optional<CodegenASTExpr> init;

  static CodegenASTStmt create (const CodegenASTType &, const CodegenASTExpr &, const std::optional<CodegenASTExpr> & = std::nullopt);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtWhile {
  CodegenASTExpr cond;
  std::optional<CodegenASTStmt> body;

  static CodegenASTStmt create (const CodegenASTExpr &, const std::optional<CodegenASTStmt> & = std::nullopt);
  std::string str (std::size_t, bool) const;
};

#endif
