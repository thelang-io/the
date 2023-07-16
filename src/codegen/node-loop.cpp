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

void Codegen::_nodeLoop (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;

  this->varMap.save();
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp);
  this->state.cleanUp.breakVarIdx += 1;
  this->state.cleanUp.continueVarIdx += 1;

  auto cBody = CodegenASTStmtCompound::create();

  if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
    auto cCond = nodeLoop.cond == std::nullopt
      ? CodegenASTExprLiteral::create("1")
      : this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, c, true);

    (*c)->append(CodegenASTStmtWhile::create(cCond, cBody));
    *c = cBody;
  } else {
    auto cInit = CodegenASTStmtNull::create();
    auto cCond = CodegenASTExprNull::create();
    auto cUpd = CodegenASTExprNull::create();

    if (nodeLoop.init != std::nullopt) {
      auto cCompound = CodegenASTStmtCompound::create();
      this->_node(&cCompound, *nodeLoop.init);

      if (!this->state.cleanUp.empty()) {
        *c = (*c)->append(cCompound);
      } else {
        cInit = cCompound->asCompound().body[0];
      }
    }

    if (nodeLoop.cond != std::nullopt) {
      cCond = this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, c, true);
    }

    if (nodeLoop.upd != std::nullopt) {
      cUpd = this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, c, true);
    }

    (*c)->append(CodegenASTStmtFor::create(cInit, cCond, cUpd, cBody));
    *c = cBody;
  }

  auto saveStateCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveStateCleanUp);
  this->_block(c, nodeLoop.body);

  if (this->state.cleanUp.continueVarUsed) {
    (*c)->prepend(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("unsigned char"),
        CodegenASTExprAccess::create(this->state.cleanUp.currentContinueVar()),
        CodegenASTExprLiteral::create("0")
      )
    );
  }

  if (this->state.cleanUp.breakVarUsed) {
    (*c)->prepend(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("unsigned char"),
        CodegenASTExprAccess::create(this->state.cleanUp.currentBreakVar()),
        CodegenASTExprLiteral::create("0")
      )
    );
  }

  *c = (*c)->exit()->exit();

  if (!saveStateCleanUp.empty()) {
    saveStateCleanUp.gen(c);

    if (saveStateCleanUp.returnVarUsed) {
      (*c)->append(
        CodegenASTStmtIf::create(
          CodegenASTExprBinary::create(CodegenASTExprAccess::create("r"), "==", CodegenASTExprLiteral::create("1")),
          initialStateCleanUp.hasCleanUp(CODEGEN_CLEANUP_FN)
            ? CodegenASTStmtGoto::create(initialStateCleanUp.currentLabel())
            : ASTChecker(node).insideMain()
              ? CodegenASTStmtReturn::create(CodegenASTExprLiteral::create("0"))
              : this->state.returnType->isVoid()
                ? CodegenASTStmtReturn::create()
                : CodegenASTStmtReturn::create(CodegenASTExprAccess::create("v"))
        )
      );
    }

    *c = (*c)->exit();
  }

  this->state.cleanUp.breakVarIdx -= 1;
  this->state.cleanUp.continueVarIdx -= 1;
  this->state.cleanUp = initialStateCleanUp;
  this->varMap.restore();
}

void Codegen::_nodeLoopAsync (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;
  auto initialStateAsyncCounterLoopBreak = this->state.asyncCounterLoopBreak;
  auto initialStateAsyncCounterLoopContinue = this->state.asyncCounterLoopContinue;

  this->varMap.save();
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp, true);
  this->state.cleanUp.breakVarIdx += 1;
  this->state.cleanUp.continueVarIdx += 1;
  this->state.asyncCounterLoopBreak = std::make_shared<std::size_t>(0);
  this->state.asyncCounterLoopContinue = std::make_shared<std::size_t>(0);

  if (nodeLoop.init != std::nullopt) {
    this->_nodeAsync(c, *nodeLoop.init);
  }

  *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);
  auto cAfterInit = *c;
  auto afterInitAsyncCounter = std::make_shared<std::size_t>(this->state.asyncCounter);

  if (nodeLoop.cond != std::nullopt) {
    auto cCond = this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, c, true);

    (*c)->append(
      CodegenASTStmtIf::create(
        CodegenASTExprUnary::create("!", cCond->wrap()),
        this->_genAsyncReturn(this->state.asyncCounterLoopBreak)
      )
    );
  }

  auto saveStateCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveStateCleanUp, true);
  this->_block(c, nodeLoop.body);

  *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);
  *this->state.asyncCounterLoopContinue = this->state.asyncCounter;

  if (nodeLoop.upd != std::nullopt) {
    auto cUpd = this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, c, true);
    (*c)->append(cUpd->stmt());
  }

  (*c)->append(this->_genAsyncReturn(afterInitAsyncCounter));
  *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);

  if (this->state.cleanUp.continueVarUsed) {
    cAfterInit->prepend(
      CodegenASTExprAssign::create(
        CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentContinueVar())),
        "=",
        CodegenASTExprLiteral::create("0")
      )->stmt()
    );
  }

  if (this->state.cleanUp.breakVarUsed) {
    cAfterInit->prepend(
      CodegenASTExprAssign::create(
        CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentBreakVar())),
        "=",
        CodegenASTExprLiteral::create("0")
      )->stmt()
    );
  }

  *this->state.asyncCounterLoopBreak = this->state.asyncCounter;

  if (!saveStateCleanUp.empty()) {
    saveStateCleanUp.genAsync(c, this->state.asyncCounter);

    if (saveStateCleanUp.returnVarUsed) {
      (*c)->append(
        CodegenASTStmtIf::create(
          CodegenASTExprBinary::create(
            CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("r")),
            "==",
            CodegenASTExprLiteral::create("1")
          ),
          initialStateCleanUp.hasCleanUp(CODEGEN_CLEANUP_FN)
            ? this->_genAsyncReturn(initialStateCleanUp.currentLabelAsync())
            : CodegenASTStmtBreak::create()
        )
      );
    }

    *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);
  }

  this->state.cleanUp.breakVarIdx -= 1;
  this->state.cleanUp.continueVarIdx -= 1;
  this->state.cleanUp = initialStateCleanUp;
  this->state.asyncCounterLoopBreak = initialStateAsyncCounterLoopBreak;
  this->state.asyncCounterLoopContinue = initialStateAsyncCounterLoopContinue;
  this->varMap.restore();
}
