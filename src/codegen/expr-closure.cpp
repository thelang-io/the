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

std::shared_ptr<CodegenASTExpr> Codegen::_exprClosure (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto exprClosure = std::get<ASTExprClosure>(*nodeExpr.body);
  auto codeName = Codegen::name(exprClosure.var->codeName);
  this->_fnDecl(c, exprClosure.var, exprClosure.stack, exprClosure.params, exprClosure.body, parent, CODEGEN_PHASE_FULL);

  auto expr = CodegenASTExprAccess::create(codeName);

  if (this->state.contextVars.contains(codeName)) {
    expr = CodegenASTExprUnary::create("*", expr);
  }

  if (!root) {
    expr = this->_genCopyFn(exprClosure.var->type, expr);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
