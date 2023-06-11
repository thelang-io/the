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

std::string Codegen::_exprCond (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
  auto initialStateTypeCasts = this->state.typeCasts;
  auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(exprCond.cond, parent);
  auto condCode = this->_nodeExpr(exprCond.cond, this->ast->typeMap.get("bool"), parent, decl);

  bodyTypeCasts.merge(this->state.typeCasts);
  bodyTypeCasts.swap(this->state.typeCasts);
  auto bodyCode = this->_nodeExpr(exprCond.body, nodeExpr.type, parent, decl);
  this->state.typeCasts = initialStateTypeCasts;
  altTypeCasts.merge(this->state.typeCasts);
  altTypeCasts.swap(this->state.typeCasts);
  auto altCode = this->_nodeExpr(exprCond.alt, nodeExpr.type, parent, decl);
  this->state.typeCasts = initialStateTypeCasts;

  if (
    std::holds_alternative<ASTExprAssign>(*exprCond.alt.body) &&
    !exprCond.alt.parenthesized &&
    !exprCond.alt.type->isSafeForTernaryAlt()
  ) {
    altCode = "(" + altCode + ")";
  }

  auto code = condCode + " ? " + bodyCode + " : " + altCode;

  if (root && nodeExpr.type->shouldBeFreed()) {
    code = this->_genFreeFn(nodeExpr.type, "(" + code + ")");
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
