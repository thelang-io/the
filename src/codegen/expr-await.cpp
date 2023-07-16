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

std::shared_ptr<CodegenASTExpr> Codegen::_exprAwait (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto exprAwait = std::get<ASTExprAwait>(*nodeExpr.body);

  (*c)->append(
    CodegenASTExprAssign::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("job"), "step", true),
      "=",
      CodegenASTExprLiteral::create(std::to_string(this->state.asyncCounter + 1))
    )->stmt()
  );

  (*c)->append(
    CodegenASTStmtReturn::create(this->_nodeExpr(exprAwait.arg, exprAwait.arg.type, parent, c, false, exprAwait.id))
  );

  *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);

  if (this->throws) {
    (*c)->append(
      CodegenASTStmtIf::create(
        CodegenASTExprBinary::create(
          this->_genErrState(false, false, "id"),
          "!=",
          CodegenASTExprLiteral::create("-1")
        ),
        this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN)
          ? this->_genAsyncReturn(this->state.cleanUp.currentLabelAsync())
          : CodegenASTStmtBreak::create()
      )
    );
  }

  auto expr = nodeExpr.type->isVoid() || root
    ? CodegenASTExprNull::create()
    : CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("t" + std::to_string(exprAwait.id)));

  if (!root && nodeExpr.type->isRef() && !targetType->isRef()) {
    expr = this->_genCopyFn(targetType, CodegenASTExprUnary::create("*", expr));
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
