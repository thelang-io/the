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

void Codegen::_nodeReturn (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

  if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN) || this->state.cleanUp.returnVarUsed) {
    auto parentNotRoot = this->state.cleanUp.parent != nullptr && this->state.cleanUp.parent->type != CODEGEN_CLEANUP_ROOT;

    if (parentNotRoot && this->state.cleanUp.parent->hasCleanUp(CODEGEN_CLEANUP_FN)) {
      (*c)->append(
        CodegenASTExprAssign::create(
          CodegenASTExprAccess::create(this->state.cleanUp.currentReturnVar()),
          "=",
          CodegenASTExprLiteral::create("1")
        )->stmt()
      );
    }

    if (nodeReturn.body != std::nullopt) {
      (*c)->append(
        CodegenASTExprAssign::create(
          CodegenASTExprAccess::create(this->state.cleanUp.currentValueVar()),
          "=",
          this->_nodeExpr(*nodeReturn.body, this->state.returnType, node, c)
        )->stmt()
      );
    }

    auto nodeParentFunction = ASTChecker(node.parent).is<ASTNodeFnDecl>() || ASTChecker(node.parent).is<ASTNodeObjDecl>();
    auto nodeIsLast = node.parent != nullptr && ASTChecker(node).isLast();

    if ((!nodeParentFunction && this->state.cleanUp.empty()) || !nodeIsLast) {
      (*c)->append(CodegenASTStmtGoto::create(this->state.cleanUp.currentLabel()));
    }
  } else if (nodeReturn.body != std::nullopt) {
    auto cArg = this->_nodeExpr(*nodeReturn.body, this->state.returnType, node, c);
    (*c)->append(CodegenASTStmtReturn::create(cArg));
  } else {
    (*c)->append(CodegenASTStmtReturn::create());
  }
}

void Codegen::_nodeReturnAsync (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

  auto cExpr = nodeReturn.body != std::nullopt
    ? this->_nodeExpr(*nodeReturn.body, this->state.returnType, node, c)
    : CodegenASTExprNull::create();

  (*c)->append(
    CodegenASTExprAssign::create(
      CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentReturnVar())),
      "=",
      CodegenASTExprLiteral::create("1")
    )->stmt()
  );

  if (!cExpr->isNull()) {
    (*c)->append(
      CodegenASTExprAssign::create(
        CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentValueVar())),
        "=",
        cExpr
      )->stmt()
    );
  }

  auto nodeParentFunction = ASTChecker(node.parent).is<ASTNodeFnDecl>() || ASTChecker(node.parent).is<ASTNodeObjDecl>();
  auto nodeIsLast = ASTChecker(node).isLast();

  if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN)) {
    if (!nodeParentFunction || !nodeIsLast) {
      (*c)->append(
        CodegenASTStmtReturn::create(this->state.cleanUp.currentLabelAsync())
      );
    }
  } else if (!nodeParentFunction || !nodeIsLast) {
    (*c)->append(CodegenASTStmtReturn::create(CodegenASTExprLiteral::create("-1")));
  }
}
