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

void exprVectorParent (std::vector<CodegenASTExpr> &items, CodegenASTExpr *parent) {
  for (auto &item : items) {
    item.parent = parent;
  }
}

CodegenASTExpr exprInit (const CodegenASTExprBody &body) {
  auto result = CodegenASTExpr{std::make_shared<CodegenASTExprBody>(body)};

  if (result.isAccess()) {
    if (std::holds_alternative<CodegenASTExpr>(result.asAccess().obj)) {
      std::get<CodegenASTExpr>(result.asAccess().obj).parent = &result;
    }
    if (result.asAccess().elem != std::nullopt) {
      result.asAccess().elem->parent = &result;
    }
  } else if (result.isAssign()) {
    result.asAssign().left.parent = &result;
    result.asAssign().right.parent = &result;
  } else if (result.isBinary()) {
    result.asBinary().left.parent = &result;
    result.asBinary().right.parent = &result;
  } else if (result.isCall()) {
    result.asCall().callee.parent = &result;
    exprVectorParent(result.asCall().exprArgs, &result);
  } else if (result.isCast()) {
    result.asCast().arg.parent = &result;
  } else if (result.isCond()) {
    result.asCond().cond.parent = &result;
    result.asCond().body.parent = &result;
    result.asCond().alt.parent = &result;
  } else if (result.isInitList()) {
    exprVectorParent(result.asInitList().items, &result);
  } else if (result.isUnary()) {
    result.asUnary().arg.parent = &result;
  }

  return result;
}

CodegenASTExprAccess &CodegenASTExpr::asAccess () { return std::get<CodegenASTExprAccess>(*this->body); }
const CodegenASTExprAccess &CodegenASTExpr::asAccess () const { return std::get<CodegenASTExprAccess>(*this->body); }
CodegenASTExprAssign &CodegenASTExpr::asAssign () { return std::get<CodegenASTExprAssign>(*this->body); }
const CodegenASTExprAssign &CodegenASTExpr::asAssign () const { return std::get<CodegenASTExprAssign>(*this->body); }
CodegenASTExprBinary &CodegenASTExpr::asBinary () { return std::get<CodegenASTExprBinary>(*this->body); }
const CodegenASTExprBinary &CodegenASTExpr::asBinary () const { return std::get<CodegenASTExprBinary>(*this->body); }
CodegenASTExprCall &CodegenASTExpr::asCall () { return std::get<CodegenASTExprCall>(*this->body); }
const CodegenASTExprCall &CodegenASTExpr::asCall () const { return std::get<CodegenASTExprCall>(*this->body); }
CodegenASTExprCast &CodegenASTExpr::asCast () { return std::get<CodegenASTExprCast>(*this->body); }
const CodegenASTExprCast &CodegenASTExpr::asCast () const { return std::get<CodegenASTExprCast>(*this->body); }
CodegenASTExprCond &CodegenASTExpr::asCond () { return std::get<CodegenASTExprCond>(*this->body); }
const CodegenASTExprCond &CodegenASTExpr::asCond () const { return std::get<CodegenASTExprCond>(*this->body); }
CodegenASTExprInitList &CodegenASTExpr::asInitList () { return std::get<CodegenASTExprInitList>(*this->body); }
const CodegenASTExprInitList &CodegenASTExpr::asInitList () const { return std::get<CodegenASTExprInitList>(*this->body); }
CodegenASTExprLiteral &CodegenASTExpr::asLiteral () { return std::get<CodegenASTExprLiteral>(*this->body); }
const CodegenASTExprLiteral &CodegenASTExpr::asLiteral () const { return std::get<CodegenASTExprLiteral>(*this->body); }
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

bool CodegenASTExpr::isEmptyString () const {
  return this->isLiteral() && this->asLiteral().val == R"("")";
}

bool CodegenASTExpr::isPointer () const {
  return this->isUnary() && this->asUnary().prefix && this->asUnary().op == "*";
}

CodegenASTStmt CodegenASTExpr::stmt () const {
  return CodegenASTStmtExpr::create(*this);
}

std::string CodegenASTExpr::str () const {
  if (this->isAccess()) {
    return this->asAccess().str();
  } else if (this->isAssign()) {
    return this->asAssign().str();
  } else if (this->isBinary()) {
    return this->asBinary().str();
  } else if (this->isCall()) {
    return this->asCall().str();
  } else if (this->isCast()) {
    return this->asCast().str();
  } else if (this->isCond()) {
    return this->asCond().str();
  } else if (this->isInitList()) {
    return this->asInitList().str();
  } else if (this->isLiteral()) {
    return this->asLiteral().str();
  } else if (this->isUnary()) {
    return this->asUnary().str();
  }

  unreachable();
}

CodegenASTExpr CodegenASTExpr::wrap () const {
  auto copy = *this;
  copy.parenthesized = true;
  return copy;
}

CodegenASTExpr CodegenASTExprAccess::create (const std::string &obj) {
  return exprInit(CodegenASTExprAccess{obj});
}

CodegenASTExpr CodegenASTExprAccess::create (const CodegenASTExpr &obj, const std::string &prop, bool pointed) {
  return exprInit(CodegenASTExprAccess{obj, prop, std::nullopt, pointed});
}

CodegenASTExpr CodegenASTExprAccess::create (const CodegenASTExpr &obj, const CodegenASTExpr &elem) {
  return exprInit(CodegenASTExprAccess{obj, std::nullopt, elem});
}

std::string CodegenASTExprAccess::str () const {
  if (std::holds_alternative<std::string>(this->obj)) {
    return std::get<std::string>(this->obj);
  }

  auto objExpr = std::get<CodegenASTExpr>(this->obj);

  if (this->prop != std::nullopt && this->pointed) {
    return objExpr.str() + "->" + *this->prop;
  } else if (this->prop != std::nullopt) {
    return objExpr.str() + "." + *this->prop;
  } else {
    return objExpr.str() + "[" + this->elem->str() + "]";
  }
}

CodegenASTExpr CodegenASTExprAssign::create (const CodegenASTExpr &left, const std::string &op, const CodegenASTExpr &right) {
  return exprInit(CodegenASTExprAssign{left, op, right});
}

std::string CodegenASTExprAssign::str () const {
  return this->left.str() + " " + this->op + " " + this->right.str();
}

CodegenASTExpr CodegenASTExprBinary::create (const CodegenASTExpr &left, const std::string &op, const CodegenASTExpr &right) {
  return exprInit(CodegenASTExprBinary{left, op, right});
}

std::string CodegenASTExprBinary::str () const {
  return this->left.str() + " " + this->op + " " + this->right.str();
}

CodegenASTExpr CodegenASTExprCall::create (const CodegenASTExpr &callee) {
  return exprInit(CodegenASTExprCall{callee});
}

CodegenASTExpr CodegenASTExprCall::create (const CodegenASTExpr &callee, const std::vector<CodegenASTExpr> &exprArgs) {
  return exprInit(CodegenASTExprCall{callee, exprArgs});
}

CodegenASTExpr CodegenASTExprCall::create (const CodegenASTExpr &callee, const std::vector<CodegenASTType> &typeArgs) {
  return exprInit(CodegenASTExprCall{callee, {}, typeArgs});
}

std::string CodegenASTExprCall::str () const {
  auto argsStr = std::string();
  for (const auto &arg : this->exprArgs) {
    argsStr = ", " + arg.str();
  }
  for (const auto &arg : this->typeArgs) {
    argsStr = ", " + arg.str();
  }
  argsStr = argsStr.empty() ? argsStr : argsStr.substr(2);
  return this->callee.str() + "(" + argsStr + ")";
}

CodegenASTExpr CodegenASTExprCast::create (const CodegenASTType &type, const CodegenASTExpr &arg) {
  return exprInit(CodegenASTExprCast{type, arg});
}

std::string CodegenASTExprCast::str () const {
  return "(" + this->type.str() + ") " + this->arg.str();
}

CodegenASTExpr CodegenASTExprCond::create (const CodegenASTExpr &cond, const CodegenASTExpr &body, const CodegenASTExpr &alt) {
  return exprInit(CodegenASTExprCond{cond, body, alt});
}

std::string CodegenASTExprCond::str () const {
  return this->cond.str() + " ? " + this->body.str() + " : " + this->alt.str();
}

CodegenASTExpr CodegenASTExprInitList::create (const std::vector<CodegenASTExpr> &items) {
  return exprInit(CodegenASTExprInitList{items});
}

std::string CodegenASTExprInitList::str () const {
  auto itemsStr = std::string();
  for (const auto &item : this->items) {
    itemsStr = ", " + item.str();
  }
  itemsStr = itemsStr.empty() ? itemsStr : itemsStr.substr(2);
  return "{" + itemsStr + "}";
}

CodegenASTExpr CodegenASTExprLiteral::create (const std::string &val) {
  return exprInit(CodegenASTExprLiteral{val});
}

std::string CodegenASTExprLiteral::str () const {
  return this->val;
}

CodegenASTExpr CodegenASTExprUnary::create (const std::string &op, const CodegenASTExpr &arg) {
  return exprInit(CodegenASTExprUnary{arg, op, true});
}

CodegenASTExpr CodegenASTExprUnary::create (const CodegenASTExpr &arg, const std::string &op) {
  return exprInit(CodegenASTExprUnary{arg, op});
}

std::string CodegenASTExprUnary::str () const {
  return (this->prefix ? this->op : "") + this->arg.str() + (this->prefix ? "" : this->op);
}
