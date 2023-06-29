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

CodegenASTExpr Codegen::_exprRef (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, CodegenASTStmt &c, bool root) {
  auto exprRef = std::get<ASTExprRef>(*nodeExpr.body);
  auto wrapAsRoot = targetType->isAny() || root;
  auto expr = CodegenASTExpr{};

  if (targetType->isOpt()) {
    auto optTargetType = std::get<TypeOptional>(targetType->body).type;
    expr = this->_nodeExpr(exprRef.expr, optTargetType, parent, c, optTargetType->isRef());
  } else {
    expr = this->_nodeExpr(exprRef.expr, targetType, parent, c, targetType->isRef());
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, wrapAsRoot, expr);
}
