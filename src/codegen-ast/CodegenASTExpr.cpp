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
#include "../utils.hpp"

// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprAccess &CodegenASTExpr::asAccess () { return std::get<CodegenASTExprAccess>(*this->body); }
const CodegenASTExprAccess &CodegenASTExpr::asAccess () const { return std::get<CodegenASTExprAccess>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprAssign &CodegenASTExpr::asAssign () { return std::get<CodegenASTExprAssign>(*this->body); }
const CodegenASTExprAssign &CodegenASTExpr::asAssign () const { return std::get<CodegenASTExprAssign>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprBinary &CodegenASTExpr::asBinary () { return std::get<CodegenASTExprBinary>(*this->body); }
const CodegenASTExprBinary &CodegenASTExpr::asBinary () const { return std::get<CodegenASTExprBinary>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprCall &CodegenASTExpr::asCall () { return std::get<CodegenASTExprCall>(*this->body); }
const CodegenASTExprCall &CodegenASTExpr::asCall () const { return std::get<CodegenASTExprCall>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprCast &CodegenASTExpr::asCast () { return std::get<CodegenASTExprCast>(*this->body); }
const CodegenASTExprCast &CodegenASTExpr::asCast () const { return std::get<CodegenASTExprCast>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprCond &CodegenASTExpr::asCond () { return std::get<CodegenASTExprCond>(*this->body); }
const CodegenASTExprCond &CodegenASTExpr::asCond () const { return std::get<CodegenASTExprCond>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprInitList &CodegenASTExpr::asInitList () { return std::get<CodegenASTExprInitList>(*this->body); }
const CodegenASTExprInitList &CodegenASTExpr::asInitList () const { return std::get<CodegenASTExprInitList>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprLiteral &CodegenASTExpr::asLiteral () { return std::get<CodegenASTExprLiteral>(*this->body); }
const CodegenASTExprLiteral &CodegenASTExpr::asLiteral () const { return std::get<CodegenASTExprLiteral>(*this->body); }
// NOLINTNEXTLINE(readability-make-member-function-const)
CodegenASTExprUnary &CodegenASTExpr::asUnary () { return std::get<CodegenASTExprUnary>(*this->body); }
const CodegenASTExprUnary &CodegenASTExpr::asUnary () const { return std::get<CodegenASTExprUnary>(*this->body); }
bool CodegenASTExpr::isAccess () const { return std::holds_alternative<CodegenASTExprAccess>(*this->body); }
bool CodegenASTExpr::isAssign () const { return std::holds_alternative<CodegenASTExprAssign>(*this->body); }
bool CodegenASTExpr::isBinary () const { return std::holds_alternative<CodegenASTExprBinary>(*this->body); }
bool CodegenASTExpr::isCall () const { return std::holds_alternative<CodegenASTExprCall>(*this->body); }
bool CodegenASTExpr::isCast () const { return std::holds_alternative<CodegenASTExprCast>(*this->body); }
bool CodegenASTExpr::isCond () const { return std::holds_alternative<CodegenASTExprCond>(*this->body); }
bool CodegenASTExpr::isInitList () const { return std::holds_alternative<CodegenASTExprInitList>(*this->body); }
bool CodegenASTExpr::isLiteral () const { return std::holds_alternative<CodegenASTExprLiteral>(*this->body); }
bool CodegenASTExpr::isUnary () const { return std::holds_alternative<CodegenASTExprUnary>(*this->body); }

void exprVectorParent (
  std::vector<std::shared_ptr<CodegenASTExpr>> &items,
  const std::shared_ptr<CodegenASTExpr> &parent
) {
  for (auto &item : items) {
    item->parent = parent;
  }
}

std::shared_ptr<CodegenASTExpr> CodegenASTExpr::create (const CodegenASTExprBody &b, bool parenthesized) {
  auto result = std::shared_ptr<CodegenASTExpr>(new CodegenASTExpr());
  result->body = std::make_shared<CodegenASTExprBody>(b);
  result->parenthesized = parenthesized;

  if (result->isAccess()) {
    if (result->asAccess().objExpr != nullptr) {
      result->asAccess().objExpr->parent = result;
    }
    if (result->asAccess().elem != nullptr) {
      result->asAccess().elem->parent = result;
    }
  } else if (result->isAssign()) {
    result->asAssign().left->parent = result;
    result->asAssign().right->parent = result;
  } else if (result->isBinary()) {
    result->asBinary().left->parent = result;
    result->asBinary().right->parent = result;
  } else if (result->isCall()) {
    result->asCall().callee->parent = result;
    exprVectorParent(result->asCall().exprArgs, result);
  } else if (result->isCast()) {
    result->asCast().arg->parent = result;
  } else if (result->isCond()) {
    result->asCond().cond->parent = result;
    result->asCond().body->parent = result;
    result->asCond().alt->parent = result;
  } else if (result->isInitList()) {
    exprVectorParent(result->asInitList().items, result);
  } else if (result->isUnary()) {
    result->asUnary().arg->parent = result;
  }

  return result;
}

std::shared_ptr<CodegenASTExpr> CodegenASTExpr::getptr () {
  return this->shared_from_this();
}

bool CodegenASTExpr::isBuiltinLiteral () const {
  return this->isAccess() &&
    this->asAccess().objId != std::nullopt &&
    (*this->asAccess().objId == "THE_EOL" || *this->asAccess().objId == "THE_PATH_SEP");
}

bool CodegenASTExpr::isEmptyString () const {
  return this->isLiteral() && this->asLiteral().val == R"("")";
}

bool CodegenASTExpr::isPointer () const {
  return this->isUnary() && this->asUnary().prefix && this->asUnary().op == "*";
}

std::shared_ptr<CodegenASTStmt> CodegenASTExpr::stmt () {
  return CodegenASTStmtExpr::create(this->getptr());
}

std::string CodegenASTExpr::str () const {
  auto result = std::string();

  if (this->isAccess()) {
    result = this->asAccess().str();
  } else if (this->isAssign()) {
    result = this->asAssign().str();
  } else if (this->isBinary()) {
    result = this->asBinary().str();
  } else if (this->isCall()) {
    result = this->asCall().str();
  } else if (this->isCast()) {
    result = this->asCast().str();
  } else if (this->isCond()) {
    result = this->asCond().str();
  } else if (this->isInitList()) {
    result = this->asInitList().str();
  } else if (this->isLiteral()) {
    result = this->asLiteral().str();
  } else if (this->isUnary()) {
    result = this->asUnary().str();
  }

  if (this->parenthesized) {
    result = "(" + result + ")";
  }

  return result;
}

std::shared_ptr<CodegenASTExpr> CodegenASTExpr::wrap () const {
  return CodegenASTExpr::create(*this->body, true);
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprAccess::create (const std::string &objId) {
  return CodegenASTExpr::create(CodegenASTExprAccess{objId});
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprAccess::create (
  const std::shared_ptr<CodegenASTExpr> &objExpr,
  const std::string &prop,
  bool pointed
) {
  return CodegenASTExpr::create(CodegenASTExprAccess{std::nullopt, objExpr, prop, nullptr, pointed});
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprAccess::create (
  const std::shared_ptr<CodegenASTExpr> &objExpr,
  const std::shared_ptr<CodegenASTExpr> &elem
) {
  return CodegenASTExpr::create(CodegenASTExprAccess{std::nullopt, objExpr, std::nullopt, elem});
}

std::string CodegenASTExprAccess::str () const {
  if (this->objId != std::nullopt) {
    return *this->objId;
  } else if (this->prop != std::nullopt && this->pointed) {
    return this->objExpr->str() + "->" + *this->prop;
  } else if (this->prop != std::nullopt) {
    return this->objExpr->str() + "." + *this->prop;
  } else {
    return this->objExpr->str() + "[" + this->elem->str() + "]";
  }
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprAssign::create (
  const std::shared_ptr<CodegenASTExpr> &left,
  const std::string &op,
  const std::shared_ptr<CodegenASTExpr> &right
) {
  return CodegenASTExpr::create(CodegenASTExprAssign{left, op, right});
}

std::string CodegenASTExprAssign::str () const {
  return this->left->str() + " " + this->op + " " + this->right->str();
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprBinary::create (
  const std::shared_ptr<CodegenASTExpr> &left,
  const std::string &op,
  const std::shared_ptr<CodegenASTExpr> &right
) {
  return CodegenASTExpr::create(CodegenASTExprBinary{left, op, right});
}

std::string CodegenASTExprBinary::str () const {
  return this->left->str() + " " + this->op + " " + this->right->str();
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprCall::create (const std::shared_ptr<CodegenASTExpr> &callee) {
  return CodegenASTExpr::create(CodegenASTExprCall{callee});
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprCall::create (
  const std::shared_ptr<CodegenASTExpr> &callee,
  const std::vector<std::shared_ptr<CodegenASTExpr>> &exprArgs
) {
  return CodegenASTExpr::create(CodegenASTExprCall{callee, exprArgs});
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprCall::create (
  const std::shared_ptr<CodegenASTExpr> &callee,
  const std::vector<CodegenASTType> &typeArgs
) {
  return CodegenASTExpr::create(CodegenASTExprCall{callee, {}, typeArgs});
}

std::string CodegenASTExprCall::str () const {
  auto argsStr = std::string();
  for (const auto &arg : this->exprArgs) {
    argsStr += ", " + arg->str();
  }
  for (const auto &arg : this->typeArgs) {
    argsStr += ", " + arg.strDef();
  }
  argsStr = argsStr.empty() ? argsStr : argsStr.substr(2);
  return this->callee->str() + "(" + argsStr + ")";
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprCast::create (
  const CodegenASTType &type,
  const std::shared_ptr<CodegenASTExpr> &arg
) {
  return CodegenASTExpr::create(CodegenASTExprCast{type, arg});
}

std::string CodegenASTExprCast::str () const {
  return "(" + this->type.strDef() + ") " + this->arg->str();
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprCond::create (
  const std::shared_ptr<CodegenASTExpr> &cond,
  const std::shared_ptr<CodegenASTExpr> &body,
  const std::shared_ptr<CodegenASTExpr> &alt
) {
  return CodegenASTExpr::create(CodegenASTExprCond{cond, body, alt});
}

std::string CodegenASTExprCond::str () const {
  return this->cond->str() + " ? " + this->body->str() + " : " + this->alt->str();
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprInitList::create (
  const std::vector<std::shared_ptr<CodegenASTExpr>> &items
) {
  return CodegenASTExpr::create(CodegenASTExprInitList{items});
}

std::string CodegenASTExprInitList::str () const {
  auto itemsStr = std::string();
  for (const auto &item : this->items) {
    itemsStr += ", " + item->str();
  }
  itemsStr = itemsStr.empty() ? itemsStr : itemsStr.substr(2);
  return "{" + itemsStr + "}";
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprLiteral::create (const std::string &val) {
  return CodegenASTExpr::create(CodegenASTExprLiteral{val});
}

std::string CodegenASTExprLiteral::str () const {
  return this->val;
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprUnary::create (
  const std::string &op,
  const std::shared_ptr<CodegenASTExpr> &arg
) {
  return CodegenASTExpr::create(CodegenASTExprUnary{arg, op, true});
}

std::shared_ptr<CodegenASTExpr> CodegenASTExprUnary::create (
  const std::shared_ptr<CodegenASTExpr> &arg,
  const std::string &op
) {
  return CodegenASTExpr::create(CodegenASTExprUnary{arg, op});
}

std::string CodegenASTExprUnary::str () const {
  return (this->prefix ? this->op : "") + this->arg->str() + (this->prefix ? "" : this->op);
}
