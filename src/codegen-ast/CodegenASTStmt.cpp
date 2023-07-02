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

#include "CodegenAST.hpp"
#include "../config.hpp"
#include "../utils.hpp"

// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtBreak &CodegenASTStmt::asBreak () { return std::get<CodegenASTStmtBreak>(*this->body); }
const CodegenASTStmtBreak &CodegenASTStmt::asBreak () const { return std::get<CodegenASTStmtBreak>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtCase &CodegenASTStmt::asCase () { return std::get<CodegenASTStmtCase>(*this->body); }
const CodegenASTStmtCase &CodegenASTStmt::asCase () const { return std::get<CodegenASTStmtCase>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtCompound &CodegenASTStmt::asCompound () { return std::get<CodegenASTStmtCompound>(*this->body); }
const CodegenASTStmtCompound &CodegenASTStmt::asCompound () const { return std::get<CodegenASTStmtCompound>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtContinue &CodegenASTStmt::asContinue () { return std::get<CodegenASTStmtContinue>(*this->body); }
const CodegenASTStmtContinue &CodegenASTStmt::asContinue () const { return std::get<CodegenASTStmtContinue>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtEnumDecl &CodegenASTStmt::asEnumDecl () { return std::get<CodegenASTStmtEnumDecl>(*this->body); }
const CodegenASTStmtEnumDecl &CodegenASTStmt::asEnumDecl () const { return std::get<CodegenASTStmtEnumDecl>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtExpr &CodegenASTStmt::asExpr () { return std::get<CodegenASTStmtExpr>(*this->body); }
const CodegenASTStmtExpr &CodegenASTStmt::asExpr () const { return std::get<CodegenASTStmtExpr>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtFnDecl &CodegenASTStmt::asFnDecl () { return std::get<CodegenASTStmtFnDecl>(*this->body); }
const CodegenASTStmtFnDecl &CodegenASTStmt::asFnDecl () const { return std::get<CodegenASTStmtFnDecl>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtFor &CodegenASTStmt::asFor () { return std::get<CodegenASTStmtFor>(*this->body); }
const CodegenASTStmtFor &CodegenASTStmt::asFor () const { return std::get<CodegenASTStmtFor>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtGoto &CodegenASTStmt::asGoto () { return std::get<CodegenASTStmtGoto>(*this->body); }
const CodegenASTStmtGoto &CodegenASTStmt::asGoto () const { return std::get<CodegenASTStmtGoto>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtIf &CodegenASTStmt::asIf () { return std::get<CodegenASTStmtIf>(*this->body); }
const CodegenASTStmtIf &CodegenASTStmt::asIf () const { return std::get<CodegenASTStmtIf>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtLabel &CodegenASTStmt::asLabel () { return std::get<CodegenASTStmtLabel>(*this->body); }
const CodegenASTStmtLabel &CodegenASTStmt::asLabel () const { return std::get<CodegenASTStmtLabel>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtNull &CodegenASTStmt::asNull () { return std::get<CodegenASTStmtNull>(*this->body); }
const CodegenASTStmtNull &CodegenASTStmt::asNull () const { return std::get<CodegenASTStmtNull>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtReturn &CodegenASTStmt::asReturn () { return std::get<CodegenASTStmtReturn>(*this->body); }
const CodegenASTStmtReturn &CodegenASTStmt::asReturn () const { return std::get<CodegenASTStmtReturn>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtStructDecl &CodegenASTStmt::asStructDecl () { return std::get<CodegenASTStmtStructDecl>(*this->body); }
const CodegenASTStmtStructDecl &CodegenASTStmt::asStructDecl () const { return std::get<CodegenASTStmtStructDecl>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtSwitch &CodegenASTStmt::asSwitch () { return std::get<CodegenASTStmtSwitch>(*this->body); }
const CodegenASTStmtSwitch &CodegenASTStmt::asSwitch () const { return std::get<CodegenASTStmtSwitch>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtVarDecl &CodegenASTStmt::asVarDecl () { return std::get<CodegenASTStmtVarDecl>(*this->body); }
const CodegenASTStmtVarDecl &CodegenASTStmt::asVarDecl () const { return std::get<CodegenASTStmtVarDecl>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTStmtWhile &CodegenASTStmt::asWhile () { return std::get<CodegenASTStmtWhile>(*this->body); }
const CodegenASTStmtWhile &CodegenASTStmt::asWhile () const { return std::get<CodegenASTStmtWhile>(*this->body); }
bool CodegenASTStmt::isBreak () const { return std::holds_alternative<CodegenASTStmtBreak>(*this->body); }
bool CodegenASTStmt::isCase () const { return std::holds_alternative<CodegenASTStmtCase>(*this->body); }
bool CodegenASTStmt::isCompound () const { return std::holds_alternative<CodegenASTStmtCompound>(*this->body); }
bool CodegenASTStmt::isContinue () const { return std::holds_alternative<CodegenASTStmtContinue>(*this->body); }
bool CodegenASTStmt::isEnumDecl () const { return std::holds_alternative<CodegenASTStmtEnumDecl>(*this->body); }
bool CodegenASTStmt::isExpr () const { return std::holds_alternative<CodegenASTStmtExpr>(*this->body); }
bool CodegenASTStmt::isFnDecl () const { return std::holds_alternative<CodegenASTStmtFnDecl>(*this->body); }
bool CodegenASTStmt::isFor () const { return std::holds_alternative<CodegenASTStmtFor>(*this->body); }
bool CodegenASTStmt::isGoto () const { return std::holds_alternative<CodegenASTStmtGoto>(*this->body); }
bool CodegenASTStmt::isIf () const { return std::holds_alternative<CodegenASTStmtIf>(*this->body); }
bool CodegenASTStmt::isLabel () const { return std::holds_alternative<CodegenASTStmtLabel>(*this->body); }
bool CodegenASTStmt::isNull () const { return std::holds_alternative<CodegenASTStmtNull>(*this->body); }
bool CodegenASTStmt::isReturn () const { return std::holds_alternative<CodegenASTStmtReturn>(*this->body); }
bool CodegenASTStmt::isStructDecl () const { return std::holds_alternative<CodegenASTStmtStructDecl>(*this->body); }
bool CodegenASTStmt::isSwitch () const { return std::holds_alternative<CodegenASTStmtSwitch>(*this->body); }
bool CodegenASTStmt::isVarDecl () const { return std::holds_alternative<CodegenASTStmtVarDecl>(*this->body); }
bool CodegenASTStmt::isWhile () const { return std::holds_alternative<CodegenASTStmtWhile>(*this->body); }

void stmtVectorParent (std::vector<CodegenASTStmt> &items, CodegenASTStmt *parent, bool withSiblings = true) {
  for (auto i = static_cast<std::size_t>(0); i < items.size(); i++) {
    items[i].parent = parent;
    if (withSiblings) {
      items[i].nextSibling = i + 1 < items.size() ? &items[i + 1] : nullptr;
      items[i].prevSibling = i != 0 ? &items[i - 1] : nullptr;
    }
  }
}

CodegenASTStmt stmtInit (const CodegenASTStmtBody &body) {
  auto result = CodegenASTStmt{std::make_shared<CodegenASTStmtBody>(body)};

   if (result.isCase()) {
     if (result.asCase().test != std::nullopt) {
       result.asCase().test->parentStmt = &result;
     }
     result.asCase().body.parent = &result;
   } else if (result.isCompound()) {
     stmtVectorParent(result.asCompound().body, &result, false);
   } else if (result.isExpr()) {
     result.asExpr().expr.parentStmt = &result;
   } else if (result.isFor()) {
     if (result.asFor().init != std::nullopt) {
       result.asFor().init->parent = &result;
     }
     if (result.asFor().cond != std::nullopt) {
       result.asFor().cond->parentStmt = &result;
     }
     if (result.asFor().upd != std::nullopt) {
       result.asFor().upd->parentStmt = &result;
     }
     if (result.asFor().body != std::nullopt) {
       result.asFor().body->parent = &result;
     }
   } else if (result.isIf()) {
     result.asIf().cond.parentStmt = &result;
     result.asIf().body.parent = &result;
     if (result.asIf().alt != std::nullopt) {
       result.asIf().alt->parent = &result;
     }
   } else if (result.isReturn()) {
     if (result.asReturn().arg != std::nullopt) {
       result.asReturn().arg->parentStmt = &result;
     }
   } else if (result.isSwitch()) {
     result.asSwitch().discriminant.parentStmt = &result;
     stmtVectorParent(result.asSwitch().body, &result, false);
   } else if (result.isVarDecl()) {
     result.asVarDecl().id.parentStmt = &result;
     if (result.asVarDecl().init != std::nullopt) {
       result.asVarDecl().init->parentStmt = &result;
     }
   } else if (result.isWhile()) {
     result.asWhile().cond.parentStmt = &result;
     if (result.asWhile().body != std::nullopt) {
       result.asWhile().body->parent = &result;
     }
   }

  return result;
}

CodegenASTStmt &CodegenASTStmt::append (const CodegenASTStmt &stmt) {
  if (this->isCase() && this->asCase().body.isCompound()) {
    this->asCase().body.asCompound().body.push_back(stmt);
    stmtVectorParent(this->asCase().body.asCompound().body, &this->asCase().body);
    return this->asCase().body.asCompound().body.back();
  } else if (this->isCompound()) {
    this->asCompound().body.push_back(stmt);
    stmtVectorParent(this->asCompound().body, this);
    return this->asCompound().body.back();
  } else if (this->isFor() && this->asFor().body != std::nullopt && this->asFor().body->isCompound()) {
    this->asFor().body->asCompound().body.push_back(stmt);
    stmtVectorParent(this->asFor().body->asCompound().body, &*this->asFor().body);
    return this->asFor().body->asCompound().body.back();
  } else if (this->isIf() && this->asIf().body.isCompound()) {
    this->asIf().body.asCompound().body.push_back(stmt);
    stmtVectorParent(this->asIf().body.asCompound().body, &this->asIf().body);
    return this->asIf().body.asCompound().body.back();
  } else if (this->isSwitch()) {
    this->asSwitch().body.push_back(stmt);
    stmtVectorParent(this->asSwitch().body, this);
    return this->asSwitch().body.back();
  } else if (this->isWhile() && this->asWhile().body != std::nullopt && this->asWhile().body->isCompound()) {
    this->asWhile().body->asCompound().body.push_back(stmt);
    stmtVectorParent(this->asWhile().body->asCompound().body, &*this->asWhile().body);
    return this->asWhile().body->asCompound().body.back();
  }

  return *this;
}

CodegenASTStmt &CodegenASTStmt::exit () const {
  return *this->parent;
}

void CodegenASTStmt::merge (const std::vector<CodegenASTStmt> &items) {
  for (const auto &item : items) {
    this->append(item);
  }
}

CodegenASTStmt &CodegenASTStmt::prepend (const CodegenASTStmt &stmt) {
  if (this->isCase() && this->asCase().body.isCompound()) {
    this->asCase().body.asCompound().body.insert(this->asCase().body.asCompound().body.begin(), stmt);
    stmtVectorParent(this->asCase().body.asCompound().body, &this->asCase().body);
    return this->asCase().body.asCompound().body.front();
  } else if (this->isCompound()) {
    this->asCompound().body.insert(this->asCompound().body.begin(), stmt);
    stmtVectorParent(this->asCompound().body, this);
    return this->asCompound().body.front();
  } else if (this->isFor() && this->asFor().body != std::nullopt && this->asFor().body->isCompound()) {
    this->asFor().body->asCompound().body.insert(this->asFor().body->asCompound().body.begin(), stmt);
    stmtVectorParent(this->asFor().body->asCompound().body, &*this->asFor().body);
    return this->asFor().body->asCompound().body.front();
  } else if (this->isIf() && this->asIf().body.isCompound()) {
    this->asIf().body.asCompound().body.insert(this->asIf().body.asCompound().body.begin(), stmt);
    stmtVectorParent(this->asIf().body.asCompound().body, &this->asIf().body);
    return this->asIf().body.asCompound().body.front();
  } else if (this->isSwitch()) {
    this->asSwitch().body.insert(this->asSwitch().body.begin(), stmt);
    stmtVectorParent(this->asSwitch().body, this);
    return this->asSwitch().body.front();
  } else if (this->isWhile() && this->asWhile().body != std::nullopt && this->asWhile().body->isCompound()) {
    this->asWhile().body->asCompound().body.insert(this->asWhile().body->asCompound().body.begin(), stmt);
    stmtVectorParent(this->asWhile().body->asCompound().body, &*this->asWhile().body);
    return this->asWhile().body->asCompound().body.front();
  }

  return *this;
}

std::string CodegenASTStmt::str (std::size_t indent, bool root) const {
  if (this->isBreak()) { return this->asBreak().str(indent, root); }
  else if (this->isCase()) { return this->asCase().str(indent, root); }
  else if (this->isCompound()) { return this->asCompound().str(indent, root); }
  else if (this->isContinue()) { return this->asContinue().str(indent, root); }
  else if (this->isEnumDecl()) { return this->asEnumDecl().str(indent, root); }
  else if (this->isExpr()) { return this->asExpr().str(indent, root); }
  else if (this->isFnDecl()) { return this->asFnDecl().str(indent, root); }
  else if (this->isFor()) { return this->asFor().str(indent, root); }
  else if (this->isGoto()) { return this->asGoto().str(indent, root); }
  else if (this->isIf()) { return this->asIf().str(indent, root); }
  else if (this->isLabel()) { return this->asLabel().str(indent, root); }
  else if (this->isNull()) { return this->asNull().str(indent, root); }
  else if (this->isReturn()) { return this->asReturn().str(indent, root); }
  else if (this->isStructDecl()) { return this->asStructDecl().str(indent, root); }
  else if (this->isSwitch()) { return this->asSwitch().str(indent, root); }
  else if (this->isVarDecl()) { return this->asVarDecl().str(indent, root); }
  else if (this->isWhile()) { return this->asWhile().str(indent, root); }

  unreachable();
}

CodegenASTStmt CodegenASTStmtBreak::create () {
  return stmtInit(CodegenASTStmtBreak{});
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string CodegenASTStmtBreak::str (std::size_t indent, bool root) const {
  return (root ? std::string(indent, ' ') : "") + "break;" + (root ? EOL : "");
}

CodegenASTStmt CodegenASTStmtCase::create (const std::optional<CodegenASTExpr> &test, const CodegenASTStmt &body) {
  return stmtInit(CodegenASTStmtCase{test, body});
}

std::string CodegenASTStmtCase::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += this->test == std::nullopt ? "default" : "case " + this->test->str();
  result += ": " + this->body.str(indent, false);
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtCompound::create () {
  return stmtInit(CodegenASTStmtCompound{});
}

CodegenASTStmt CodegenASTStmtCompound::create (const std::vector<CodegenASTStmt> &body) {
  return stmtInit(CodegenASTStmtCompound{body});
}

std::string CodegenASTStmtCompound::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += "{";
  for (const auto &it : this->body) {
    result += it.str(indent + 2);
  }
  result += "}";
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtContinue::create () {
  return stmtInit(CodegenASTStmtContinue{});
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string CodegenASTStmtContinue::str (std::size_t indent, bool root) const {
  return (root ? std::string(indent, ' ') : "") + "continue;" + (root ? EOL : "");
}

CodegenASTStmt CodegenASTStmtExpr::create (const CodegenASTExpr &expr) {
  return stmtInit(CodegenASTStmtExpr{expr});
}

std::string CodegenASTStmtExpr::str (std::size_t indent, bool root) const {
  return (root ? std::string(indent, ' ') : "") + this->expr.str() + ";" + (root ? EOL : "");
}

CodegenASTStmt CodegenASTStmtFor::create (
  const std::optional<CodegenASTStmt> &init,
  const std::optional<CodegenASTExpr> &cond,
  const std::optional<CodegenASTExpr> &upd,
  const std::optional<CodegenASTStmt> &body
) {
  return stmtInit(CodegenASTStmtFor{init, cond, upd, body});
}

std::string CodegenASTStmtFor::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += "for (";
  if (this->init != std::nullopt) {
    result += this->init->str(indent, false);
  } else {
    result += ";";
  }
  if (this->cond != std::nullopt) {
    result += this->cond->str();
  }
  result += ";";
  if (this->upd != std::nullopt) {
    result += this->upd->str();
  }
  result += ")";
  if (this->body != std::nullopt) {
    result += " ";
    result += this->body->str(indent, false);
  } else {
    result += ";";
  }
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtGoto::create (const std::string &label) {
  return stmtInit(CodegenASTStmtGoto{label});
}

std::string CodegenASTStmtGoto::str (std::size_t indent, bool root) const {
  return (root ? std::string(indent, ' ') : "") + "goto " + this->label + ";" + (root ? EOL : "");
}

CodegenASTStmt CodegenASTStmtIf::create (
  const CodegenASTExpr &cond,
  const CodegenASTStmt &body,
  const std::optional<CodegenASTStmt> &alt
) {
  return stmtInit(CodegenASTStmtIf{cond, body, alt});
}

std::string CodegenASTStmtIf::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += "if (" + this->cond.str() + ") " + this->body.str(indent, false);
  if (this->alt != std::nullopt) {
    result += " else " + this->alt->str(indent, false);
  }
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtLabel::create (const std::string &name) {
  return stmtInit(CodegenASTStmtLabel{name});
}

std::string CodegenASTStmtLabel::str ([[maybe_unused]] std::size_t indent, bool root) const {
  return this->name + ":" + (root ? EOL : "");
}

CodegenASTStmt CodegenASTStmtNull::create () {
  return stmtInit(CodegenASTStmtNull{});
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string CodegenASTStmtNull::str (std::size_t indent, bool root) const {
  return (root ? std::string(indent, ' ') : "") + ";" + (root ? EOL : "");
}

CodegenASTStmt CodegenASTStmtReturn::create () {
  return stmtInit(CodegenASTStmtReturn{});
}

CodegenASTStmt CodegenASTStmtReturn::create (const CodegenASTExpr &arg) {
  return stmtInit(CodegenASTStmtReturn{arg});
}

CodegenASTStmt CodegenASTStmtReturn::create (const std::shared_ptr<std::size_t> &asyncPtr) {
  return stmtInit(CodegenASTStmtReturn{std::nullopt, asyncPtr});
}

std::string CodegenASTStmtReturn::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += "return";
  if (this->arg != std::nullopt) {
    result += " " + this->arg->str();
  } else if (this->asyncPtr != std::nullopt) {
    result += " " + std::to_string(**this->asyncPtr);
  }
  result += ";";
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtSwitch::create (
  const CodegenASTExpr &discriminant,
  const std::vector<CodegenASTStmt> &body
) {
  return stmtInit(CodegenASTStmtSwitch{discriminant, body});
}

std::string CodegenASTStmtSwitch::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += "switch (" + this->discriminant.str() + ") {";
  for (const auto &it : this->body) {
    result += it.str(indent + 2);
  }
  result += "}";
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtVarDecl::create (
  const CodegenASTType &type,
  const CodegenASTExpr &id,
  const std::optional<CodegenASTExpr> &init
) {
  return stmtInit(CodegenASTStmtVarDecl{type, id, init});
}

std::string CodegenASTStmtVarDecl::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += this->type.str() + this->id.str();
  if (this->init != std::nullopt) {
    result += this->init->str();
  }
  result += ";";
  result += root ? EOL : "";
  return result;
}

CodegenASTStmt CodegenASTStmtWhile::create (const CodegenASTExpr &cond, const std::optional<CodegenASTStmt> &body) {
  return stmtInit(CodegenASTStmtWhile{cond, body});
}

std::string CodegenASTStmtWhile::str (std::size_t indent, bool root) const {
  auto result = root ? std::string(indent, ' ') : "";
  result += "while (" + this->cond.str() + ")";
  if (this->body != std::nullopt) {
    result += this->body->str(indent, false);
  } else {
    result += ";";
  }
  result += root ? EOL : "";
  return result;
}
