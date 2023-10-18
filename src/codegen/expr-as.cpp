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

#include "../Codegen.hpp"

std::shared_ptr<CodegenASTExpr> Codegen::_exprAs (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto exprAs = std::get<ASTExprAs>(*nodeExpr.body);
  auto cExpr = this->_nodeExpr(exprAs.expr, exprAs.expr.type, parent, c, true);
  auto typeDef = this->_typeDef(exprAs.type);
  auto t1 = Type::actual(exprAs.expr.type);
  auto t2 = Type::actual(nodeExpr.type);
  auto expr = cExpr;

  if (t1->isAny()) {
    auto typeName = this->_typeNameAny(t2);

    expr = CodegenASTExprAccess::create(
      CodegenASTExprCast::create(
        CodegenASTType::create("struct " + this->_(typeName) + " *"),
        CodegenASTExprAccess::create(expr, "d")
      )->wrap(),
      "d",
      true
    );
  } else if (
    (t1->isOpt() && Type::actual(std::get<TypeOptional>(t1->body).type)->matchStrict(t2, true)) ||
    (t1->isRef() && Type::actual(std::get<TypeRef>(t1->body).refType)->matchStrict(t2, true))
  ) {
    expr = CodegenASTExprUnary::create("*", expr);
  } else if (t1->isUnion() && t1->hasSubType(t2)) {
    expr = CodegenASTExprAccess::create(expr, "v" + this->_typeDefIdx(t2));
  } else {
    auto typeInfo = this->_typeInfo(t2);
    expr = CodegenASTExprCast::create(CodegenASTType::create(typeInfo.typeCode), expr);
  }

  if ((!t2->isRef() && targetType->isRef()) || targetType->isRefOf(t2)) {
    expr = CodegenASTExprUnary::create("&", expr);
  }

  if (!root) {
    expr = this->_genCopyFn(t2, expr);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
