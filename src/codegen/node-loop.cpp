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
#include "../config.hpp"

CodegenASTStmt &Codegen::_nodeLoop (CodegenASTStmt &c, ASTNode &node, bool root, CodegenPhase phase) {
  auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;

  this->varMap.save();
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp);
  this->state.cleanUp.breakVarIdx += 1;
  this->state.cleanUp.continueVarIdx += 1;

  if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
    auto cCond = nodeLoop.cond == std::nullopt
      ? CodegenASTExprLiteral::create("1")
      : this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, c, true);

    c = c.append(CodegenASTStmtWhile::create(cCond));
  } else {
    auto cInit = std::optional<CodegenASTStmt>{};
    auto cCond = std::optional<CodegenASTExpr>{};
    auto cUpd = std::optional<CodegenASTExpr>{};

    if (nodeLoop.init != std::nullopt) {
      auto cCompound = CodegenASTStmtCompound::create();
      this->_node(cCompound, *nodeLoop.init);

      if (!this->state.cleanUp.empty()) {
        c = c.append(cCompound);
      } else {
        cInit = this->_node(c, *nodeLoop.init, false);
      }
    }

    if (nodeLoop.cond != std::nullopt) {
      cCond = this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, c, true);
    }

    if (nodeLoop.upd != std::nullopt) {
      cUpd = this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, c, true);
    }

    c = c.append(CodegenASTStmtFor::create(cInit, cCond, cUpd));
  }

  auto saveStateCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveStateCleanUp);
  this->_block(c, nodeLoop.body);

  if (this->state.cleanUp.breakVarUsed) {
    c.prepend(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("unsigned char"),
        CodegenASTExprAccess::create(this->state.cleanUp.currentBreakVar()),
        CodegenASTExprLiteral::create("0")
      )
    );
  }

  if (this->state.cleanUp.continueVarUsed) {
    c.prepend(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("unsigned char"),
        CodegenASTExprAccess::create(this->state.cleanUp.currentContinueVar()),
        CodegenASTExprLiteral::create("0")
      )
    );
  }

  if (nodeLoop.init != std::nullopt && !saveStateCleanUp.empty()) {
    saveStateCleanUp.gen(c);

    if (saveStateCleanUp.returnVarUsed) {
      c.append(
        CodegenASTStmtIf::create(
          CodegenASTExprBinary::create(CodegenASTExprAccess::create("r"), "==", CodegenASTExprLiteral::create("1")),
          CodegenASTStmtGoto::create(initialStateCleanUp.currentLabel())
        )
      );
    }

    c = c.exit();
  }

  this->state.cleanUp.breakVarIdx -= 1;
  this->state.cleanUp.continueVarIdx -= 1;
  this->state.cleanUp = initialStateCleanUp;
  this->varMap.restore();

  return c;
}

CodegenASTStmt &Codegen::_nodeLoopAsync (CodegenASTStmt &c, ASTNode &node, bool root, CodegenPhase phase) {
  auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;

  this->varMap.save();
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp, true);
  this->state.cleanUp.breakVarIdx += 1;
  this->state.cleanUp.continueVarIdx += 1;

  if (nodeLoop.init != std::nullopt) {
    c = this->_nodeAsync(c, *nodeLoop.init);
  }

  auto &cInit = c;
  c = c.exit().append(CodegenASTStmtCase::create(std::to_string(++this->state.asyncCounter)));
  auto afterInitAsyncCounter = this->state.asyncCounter;

  if (nodeLoop.cond != std::nullopt) {
    auto cCond = this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, c, true);

    c.append(
      CodegenASTStmtIf::create(
        CodegenASTExprUnary::create("!", cCond.wrap()),
        CodegenASTStmtReturn::create(CodegenASTExprLiteral::create(std::to_string(this->state.asyncCounter)))
      )
    );
  }

  auto saveStateCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveStateCleanUp, true);
  c = this->_block(c, nodeLoop.body);
  c = c.exit().append(CodegenASTStmtCase::create(std::to_string(++this->state.asyncCounter)));

  if (nodeLoop.upd != std::nullopt) {
    auto cUpd = this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, c, true);
    c.append(cUpd.stmt());
  }

  c.append(CodegenASTStmtReturn::create(CodegenASTExprLiteral::create(std::to_string(afterInitAsyncCounter))));
  c = c.exit().append(CodegenASTStmtCase::create(std::to_string(++this->state.asyncCounter)));

  if (this->state.cleanUp.breakVarUsed) {
    cInit.append(
      CodegenASTExprAssign::create(
        CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentBreakVar())),
        "=",
        CodegenASTExprLiteral::create("0")
      ).stmt()
    );
  }

  if (this->state.cleanUp.continueVarUsed) {
    cInit.append(
      CodegenASTExprAssign::create(
        CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(this->state.cleanUp.currentContinueVar())),
        "=",
        CodegenASTExprLiteral::create("0")
      ).stmt()
    );
  }

  if (!saveStateCleanUp.empty()) {
    c = saveStateCleanUp.genAsync(c, this->state.asyncCounter);
    c = c.exit().append(CodegenASTStmtCase::create(std::to_string(++this->state.asyncCounter)));

    // todo return
    // todo catch
  }

  this->state.cleanUp.breakVarIdx -= 1;
  this->state.cleanUp.continueVarIdx -= 1;
  this->state.cleanUp = initialStateCleanUp;
  this->varMap.restore();

  return c;
}
