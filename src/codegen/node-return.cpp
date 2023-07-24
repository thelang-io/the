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

    if (parentNotRoot && this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN)) {
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
    (*c)->append(
      ASTChecker(node).insideMain()
        ? CodegenASTStmtReturn::create(CodegenASTExprLiteral::create("0"))
        : this->state.returnType->isVoid()
          ? CodegenASTStmtReturn::create()
          : CodegenASTStmtReturn::create(CodegenASTExprAccess::create("v"))
    );
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
    auto returnTypeInfo = this->_typeInfo(this->state.returnType);

    (*c)->append(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create(returnTypeInfo.typeCode),
        CodegenASTExprAccess::create("_v"),
        cExpr
      )
    );

    if (this->state.returnType->shouldBeFreed()) {
      (*c)->append(
        CodegenASTExprCond::create(
          CodegenASTExprBinary::create(
            CodegenASTExprAccess::create(this->state.cleanUp.currentValueVar()),
            "==",
            CodegenASTExprAccess::create(this->_("NULL"))
          ),
          this->_genFreeFn(this->state.returnType, CodegenASTExprAccess::create("_v")),
          CodegenASTExprAssign::create(
            CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentValueVar())),
            "=",
            CodegenASTExprAccess::create("_v")
          )->wrap()
        )->stmt()
      );
    } else {
      (*c)->append(
        CodegenASTStmtIf::create(
          CodegenASTExprBinary::create(
            CodegenASTExprAccess::create(this->state.cleanUp.currentValueVar()),
            "!=",
            CodegenASTExprAccess::create(this->_("NULL"))
          ),
          CodegenASTExprAssign::create(
            CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentValueVar())),
            "=",
            CodegenASTExprAccess::create("_v")
          )->stmt()
        )
      );
    }
  }

  auto nodeParentFunction = ASTChecker(node.parent).is<ASTNodeFnDecl>() || ASTChecker(node.parent).is<ASTNodeObjDecl>();
  auto nodeIsLast = ASTChecker(node).isLast();

  if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN) && (!nodeParentFunction || !nodeIsLast)) {
    (*c)->append(this->_genAsyncReturn(this->state.cleanUp.currentLabelAsync()));
  }
}
