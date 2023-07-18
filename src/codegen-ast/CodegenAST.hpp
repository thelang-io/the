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
struct CodegenASTExprNull;
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
  CodegenASTExprNull,
  CodegenASTExprUnary
>;

inline std::string notImplementedStr ([[maybe_unused]] std::size_t indent, [[maybe_unused]] bool root) {
  return "undefined";
}

struct CodegenASTType {
  std::string val;

  static CodegenASTType create (const std::string &);
  std::string strDecl () const;
  std::string strDef () const;
};

struct CodegenASTStmt : public std::enable_shared_from_this<CodegenASTStmt> {
  std::shared_ptr<CodegenASTStmtBody> body;
  std::shared_ptr<CodegenASTStmt> parent = nullptr;
  std::shared_ptr<CodegenASTStmt> prevSibling = nullptr;
  std::shared_ptr<CodegenASTStmt> nextSibling = nullptr;

  static std::shared_ptr<CodegenASTStmt> create (const CodegenASTStmtBody &);
  static bool emptyVector (const std::vector<std::shared_ptr<CodegenASTStmt>> &);

  std::shared_ptr<CodegenASTStmt> append (const std::shared_ptr<CodegenASTStmt> &);
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
  std::shared_ptr<CodegenASTStmt> exit () const;
  std::shared_ptr<CodegenASTStmt> getptr ();
  std::shared_ptr<CodegenASTStmt> getBody ();
  bool hasBody () const;
  std::shared_ptr<CodegenASTStmt> increaseAsyncCounter (std::size_t &);
  bool isBreak () const;
  bool isCase () const;
  bool isCompound () const;
  bool isContinue () const;
  bool isEnumDecl () const;
  bool isExpr () const;
  bool isExprNull () const;
  bool isFnDecl () const;
  bool isFor () const;
  bool isGoto () const;
  bool isIf () const;
  bool isLabel () const;
  bool isNull () const;
  bool isNullable () const;
  bool isReturn () const;
  bool isStructDecl () const;
  bool isSwitch () const;
  bool isVarDecl () const;
  bool isWhile () const;
  void merge (const std::vector<std::shared_ptr<CodegenASTStmt>> &);
  std::shared_ptr<CodegenASTStmt> prepend (const std::shared_ptr<CodegenASTStmt> &);
  void setIfAlt (const std::shared_ptr<CodegenASTStmt> &);
  std::string str (std::size_t = 0, bool = true) const;

  template <typename T>
  bool endsWith ();

 private:
  CodegenASTStmt () = default;
};

struct CodegenASTExpr : public std::enable_shared_from_this<CodegenASTExpr> {
  std::shared_ptr<CodegenASTExprBody> body;
  bool parenthesized = false;
  std::shared_ptr<CodegenASTExpr> parent = nullptr;
  std::shared_ptr<CodegenASTStmt> parentStmt = nullptr;

  static std::shared_ptr<CodegenASTExpr> create (const CodegenASTExprBody &, bool = false);
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
  CodegenASTExprNull &asNull ();
  const CodegenASTExprNull &asNull () const;
  CodegenASTExprUnary &asUnary ();
  const CodegenASTExprUnary &asUnary () const;
  std::shared_ptr<CodegenASTExpr> getptr ();
  bool isAccess () const;
  bool isAssign () const;
  bool isBinary () const;
  bool isBuiltinLiteral () const;
  bool isCall () const;
  bool isCast () const;
  bool isCond () const;
  bool isEmptyString () const;
  bool isInitList () const;
  bool isLiteral () const;
  bool isNull () const;
  bool isPointer () const;
  bool isUnary () const;
  std::shared_ptr<CodegenASTStmt> stmt ();
  std::string str () const;
  std::shared_ptr<CodegenASTExpr> wrap () const;

 private:
  CodegenASTExpr () = default;
};

struct CodegenASTExprAccess {
  std::optional<std::string> objId;
  std::shared_ptr<CodegenASTExpr> objExpr = nullptr;
  std::optional<std::string> prop = std::nullopt;
  std::shared_ptr<CodegenASTExpr> elem = nullptr;
  bool pointed = false;

  static std::shared_ptr<CodegenASTExpr> create (const std::string &);
  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::string &, bool = false);
  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTExpr> &);
  std::string str () const;
};

struct CodegenASTExprAssign {
  std::shared_ptr<CodegenASTExpr> left;
  std::string op;
  std::shared_ptr<CodegenASTExpr> right;

  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::string &, const std::shared_ptr<CodegenASTExpr> &);
  std::string str () const;
};

struct CodegenASTExprBinary {
  std::shared_ptr<CodegenASTExpr> left;
  std::string op;
  std::shared_ptr<CodegenASTExpr> right;

  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::string &, const std::shared_ptr<CodegenASTExpr> &);
  std::string str () const;
};

struct CodegenASTExprCall {
  std::shared_ptr<CodegenASTExpr> callee;
  std::vector<std::shared_ptr<CodegenASTExpr>> exprArgs = {};
  std::vector<CodegenASTType> typeArgs = {};

  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &);
  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::vector<std::shared_ptr<CodegenASTExpr>> &);
  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::vector<CodegenASTType> &);
  std::string str () const;
};

struct CodegenASTExprCast {
  CodegenASTType type;
  std::shared_ptr<CodegenASTExpr> arg;

  static std::shared_ptr<CodegenASTExpr> create (const CodegenASTType &, const std::shared_ptr<CodegenASTExpr> &);
  std::string str () const;
};

struct CodegenASTExprCond {
  std::shared_ptr<CodegenASTExpr> cond;
  std::shared_ptr<CodegenASTExpr> body;
  std::shared_ptr<CodegenASTExpr> alt;

  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTExpr> &);
  std::string str () const;
};

struct CodegenASTExprInitList {
  std::vector<std::shared_ptr<CodegenASTExpr>> items;

  static std::shared_ptr<CodegenASTExpr> create (const std::vector<std::shared_ptr<CodegenASTExpr>> & = {});
  std::string str () const;
};

struct CodegenASTExprLiteral {
  std::string val;
  std::shared_ptr<std::size_t> asyncVal = nullptr;

  static std::shared_ptr<CodegenASTExpr> create (const std::string &);
  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<std::size_t> &);
  std::string str () const;
};

struct CodegenASTExprNull {
  static std::shared_ptr<CodegenASTExpr> create ();
  std::string str () const;
};

struct CodegenASTExprUnary {
  std::shared_ptr<CodegenASTExpr> arg;
  std::string op;
  bool prefix = false;

  static std::shared_ptr<CodegenASTExpr> create (const std::string &, const std::shared_ptr<CodegenASTExpr> &);
  static std::shared_ptr<CodegenASTExpr> create (const std::shared_ptr<CodegenASTExpr> &, const std::string &);
  std::string str () const;
};

struct CodegenASTStmtBreak {
  static std::shared_ptr<CodegenASTStmt> create ();
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtCase {
  std::shared_ptr<CodegenASTExpr> test;
  std::shared_ptr<CodegenASTStmt> body;

  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTExpr> & = nullptr, const std::shared_ptr<CodegenASTStmt> & = nullptr);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtCompound {
  std::vector<std::shared_ptr<CodegenASTStmt>> body;

  static std::shared_ptr<CodegenASTStmt> create ();
  static std::shared_ptr<CodegenASTStmt> create (const std::vector<std::shared_ptr<CodegenASTStmt>> &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtContinue {
  static std::shared_ptr<CodegenASTStmt> create ();
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtEnumDecl {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static, readability-make-member-function-const)
  inline std::string str (std::size_t indent, bool root) const { return notImplementedStr(indent, root); };
};

struct CodegenASTStmtExpr {
  std::shared_ptr<CodegenASTExpr> expr;

  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTExpr> &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtFnDecl {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static, readability-make-member-function-const)
  inline std::string str (std::size_t indent, bool root) const { return notImplementedStr(indent, root); };
};

struct CodegenASTStmtFor {
  std::shared_ptr<CodegenASTStmt> init;
  std::shared_ptr<CodegenASTExpr> cond;
  std::shared_ptr<CodegenASTExpr> upd;
  std::shared_ptr<CodegenASTStmt> body;

  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTStmt> & = nullptr, const std::shared_ptr<CodegenASTExpr> & = nullptr, const std::shared_ptr<CodegenASTExpr> & = nullptr, const std::shared_ptr<CodegenASTStmt> & = nullptr);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtGoto {
  std::string label;

  static std::shared_ptr<CodegenASTStmt> create (const std::string &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtIf {
  std::shared_ptr<CodegenASTExpr> cond;
  std::shared_ptr<CodegenASTStmt> body;
  std::shared_ptr<CodegenASTStmt> alt;

  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTStmt> &, const std::shared_ptr<CodegenASTStmt> & = nullptr);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtLabel {
  std::string name;

  static std::shared_ptr<CodegenASTStmt> create (const std::string &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtNull {
  static std::shared_ptr<CodegenASTStmt> create ();
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtReturn {
  std::shared_ptr<CodegenASTExpr> arg = nullptr;

  static std::shared_ptr<CodegenASTStmt> create ();
  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTExpr> &);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtStructDecl {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static, readability-make-member-function-const)
  inline std::string str (std::size_t indent, bool root) const { return notImplementedStr(indent, root); };
};

struct CodegenASTStmtSwitch {
  std::shared_ptr<CodegenASTExpr> discriminant;
  std::vector<std::shared_ptr<CodegenASTStmt>> body;

  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTExpr> &, const std::vector<std::shared_ptr<CodegenASTStmt>> & = {});
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtVarDecl {
  CodegenASTType type;
  std::shared_ptr<CodegenASTExpr> id;
  std::shared_ptr<CodegenASTExpr> init;

  static std::shared_ptr<CodegenASTStmt> create (const CodegenASTType &, const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTExpr> & = nullptr);
  std::string str (std::size_t, bool) const;
};

struct CodegenASTStmtWhile {
  std::shared_ptr<CodegenASTExpr> cond;
  std::shared_ptr<CodegenASTStmt> body;

  static std::shared_ptr<CodegenASTStmt> create (const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTStmt> & = nullptr);
  std::string str (std::size_t, bool) const;
};

template <typename T>
bool CodegenASTStmt::endsWith () {
  if (this->hasBody()) {
    auto bodyStmt = this->getBody();

    return bodyStmt->isCompound() &&
      !bodyStmt->asCompound().body.empty() &&
      std::holds_alternative<T>(*bodyStmt->asCompound().body.back()->body);
  }

  return false;
}

#endif
