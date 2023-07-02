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

std::shared_ptr<CodegenASTExpr> Codegen::_exprIs (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto exprIs = std::get<ASTExprIs>(*nodeExpr.body);
  auto exprIsExprCode = this->_nodeExpr(exprIs.expr, exprIs.expr.type, parent, c, true);
  auto exprIsTypeDef = this->_typeDef(exprIs.type);

  auto expr = CodegenASTExprBinary::create(
    CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_(exprIsTypeDef)), "t"),
    "==",
    CodegenASTExprAccess::create(this->_(exprIsTypeDef))
  );

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
