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

std::string Codegen::_block (const ASTBlock &nodes, bool saveCleanUp, const std::string &cleanupData, bool errHandled) {
  if (ASTChecker(nodes).async() || (this->state.insideAsync && ASTChecker(nodes).hasSyncBreaking())) {
    return this->_blockAsync(nodes, saveCleanUp, cleanupData, errHandled);
  }

  auto initialIndent = this->indent;
  auto initialCleanUp = this->state.cleanUp;
  auto jumpedBefore = false;
  auto code = std::string();

  if (saveCleanUp) {
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialCleanUp);

    if (!cleanupData.empty()) {
      this->state.cleanUp.add(cleanupData);
    }

    if (
      !nodes.empty() &&
      ASTChecker(nodes[0].parent).is<ASTNodeLoop>() &&
      ASTChecker(nodes).has<ASTNodeTry>() &&
      (ASTChecker(nodes).has<ASTNodeBreak>() || ASTChecker(nodes).has<ASTNodeContinue>())
    ) {
      this->state.cleanUp.add("");
    }
  }

  this->indent += 2;

  for (auto i = static_cast<std::size_t>(0); i < nodes.size(); i++) {
    auto node = nodes[i];
    auto nodeChecker = ASTChecker(node);

    auto throwWrapNode = std::holds_alternative<ASTNodeExpr>(*node.body) ||
      std::holds_alternative<ASTNodeVarDecl>(*node.body);

    if (i < nodes.size() - 1 && nodeChecker.hoistingFriendly() && ASTChecker(nodes[i + 1]).hoistingFriendly()) {
      for (auto j = i; j < nodes.size() && ASTChecker(nodes[j]).hoistingFriendly(); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC);
      }

      for (auto j = i; j < nodes.size() && ASTChecker(nodes[j]).hoistingFriendly(); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC_METHOD);
      }

      for (; i < nodes.size() && ASTChecker(nodes[i]).hoistingFriendly(); i++) {
        code += this->_node(nodes[i], true, CODEGEN_PHASE_INIT);
      }

      i--;
    } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
      code += this->_node(node, true, CODEGEN_PHASE_ALLOC);
      code += this->_node(node, true, CODEGEN_PHASE_ALLOC_METHOD);
      code += this->_node(node, true, CODEGEN_PHASE_INIT);
    } else if (this->throws && throwWrapNode) {
      if (!jumpedBefore) {
        code += std::string(this->indent, ' ') + this->_apiEval("if (_{setjmp}(_{err_state}.buf[_{err_state}.buf_idx++]) != 0) ");
        this->state.cleanUp.add(this->_apiEval("_{err_state}.buf_idx--;"));
      } else {
        code += std::string(this->indent, ' ') + this->_apiEval("if (_{setjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1]) != 0) ");
      }

      code += "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
      auto saveStateCleanUpJumpUsed = this->state.cleanUp.jumpUsed;

      this->state.cleanUp.jumpUsed = true;
      code += this->_node(node);
      this->state.cleanUp.jumpUsed = saveStateCleanUpJumpUsed;

      jumpedBefore = true;
    } else {
      code += this->_node(node);
    }
  }

  if (saveCleanUp) {
    code += this->state.cleanUp.gen(this->indent);
    auto nodesChecker = ASTChecker(nodes);
    auto nodesParentChecker = ASTChecker(nodes.empty() ? nullptr : nodes.begin()->parent);

    if (!nodesParentChecker.is<ASTNodeMain>() && this->throws && !this->state.cleanUp.empty() && !errHandled) {
      code += std::string(this->indent, ' ') + this->_apiEval("if (_{err_state}.id != -1) ");

      if (initialCleanUp.isClosestJump()) {
        code += this->_apiEval("_{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);" EOL);
      } else {
        code += "goto " + initialCleanUp.currentLabel() + ";" EOL;
      }
    }

    if (!nodesParentChecker.is<ASTNodeLoop>() && this->state.cleanUp.continueVarUsed && initialCleanUp.hasCleanUp(CODEGEN_CLEANUP_LOOP)) {
      code += std::string(this->indent, ' ') + "if (" + this->state.cleanUp.currentContinueVar() + " == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
    } else if (!nodesParentChecker.is<ASTNodeLoop>() && this->state.cleanUp.continueVarUsed) {
      code += std::string(this->indent, ' ') + "if (" + this->state.cleanUp.currentContinueVar() + " == 1) continue;" EOL;
    }

    if (!nodesParentChecker.is<ASTNodeLoop>() && this->state.cleanUp.breakVarUsed && initialCleanUp.hasCleanUp(CODEGEN_CLEANUP_LOOP)) {
      code += std::string(this->indent, ' ') + "if (" + this->state.cleanUp.currentBreakVar() + " == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
    } else if (this->state.cleanUp.breakVarUsed) {
      code += std::string(this->indent, ' ') + "if (" + this->state.cleanUp.currentBreakVar() + " == 1) break;" EOL;
    }

    if (this->state.cleanUp.returnVarUsed && !this->state.cleanUp.empty()) {
      code += std::string(this->indent, ' ') + "if (r == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
    }

    this->state.cleanUp = initialCleanUp;
  }

  this->indent = initialIndent;
  return code;
}

std::string Codegen::_blockAsync (const ASTBlock &nodes, bool saveCleanUp, const std::string &cleanupData, [[maybe_unused]] bool errHandled) {
  auto initialIndent = this->indent;
  auto initialCleanUp = this->state.cleanUp;
  auto code = std::string();

  if (saveCleanUp) {
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialCleanUp, true);

    if (!cleanupData.empty()) {
      this->state.cleanUp.add(cleanupData);
    }
  }

  this->indent += 2;

  for (auto i = static_cast<std::size_t>(0); i < nodes.size(); i++) {
    auto node = nodes[i];
    auto nodeChecker = ASTChecker(node);

    if (!this->state.cleanUp.empty() && nodeChecker.hasSyncBreaking()) {
      if (node.codegenAsyncCounter == nullptr) {
        node.codegenAsyncCounter = this->state.cleanUp.currentLabelAsync();
      } else {
        this->state.cleanUp.currentLabelAsync();
      }
    }

    if (i < nodes.size() - 1 && nodeChecker.hoistingFriendly() && ASTChecker(nodes[i + 1]).hoistingFriendly()) {
      for (auto j = i; j < nodes.size() && ASTChecker(nodes[j]).hoistingFriendly(); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC);
      }

      for (auto j = i; j < nodes.size() && ASTChecker(nodes[j]).hoistingFriendly(); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC_METHOD);
      }

      for (; i < nodes.size() && ASTChecker(nodes[i]).hoistingFriendly(); i++) {
        code += this->_node(nodes[i], true, CODEGEN_PHASE_INIT);
      }

      i--;
    } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
      code += this->_node(node, true, CODEGEN_PHASE_ALLOC);
      code += this->_node(node, true, CODEGEN_PHASE_ALLOC_METHOD);
      code += this->_node(node, true, CODEGEN_PHASE_INIT);
    } else if (!nodeChecker.hasSyncBreaking() && !nodeChecker.hasAwait()) {
      code += this->_node(node);
    } else {
      code += this->_nodeAsync(node);
    }
  }

  if (saveCleanUp) {
    auto nodesChecker = ASTChecker(nodes);
    auto nodesParentChecker = ASTChecker(nodes.empty() ? nullptr : nodes.begin()->parent);
    auto cleanUpCode = this->state.cleanUp.genAsync(this->indent, this->state.asyncCounter);

    if (!cleanUpCode.empty()) {
      this->state.asyncBuffer.push_back(CodegenAsyncBufferItem{this->state.asyncCounter, CodegenAsyncBufferItemCleanUp});
      code += cleanUpCode;

      if (!nodesParentChecker.is<ASTNodeLoop>() && this->state.cleanUp.continueVarUsed) {
        code += std::string(this->indent, ' ') + "if (*" + this->state.cleanUp.currentContinueVar() + " == 1) ";
        code += "return " + std::to_string(this->_findClosestAsyncBufferItem(CodegenAsyncBufferItemLoopContinue)) + ";" EOL;
      }

      if (this->state.cleanUp.breakVarUsed) {
        code += std::string(this->indent, ' ') + "if (*" + this->state.cleanUp.currentBreakVar() + " == 1) ";
        code += "return " + std::to_string(this->_findClosestAsyncBufferItem(CodegenAsyncBufferItemLoopBreak)) + ";" EOL;
      }

      // todo return
      // todo catch
    }

    this->state.cleanUp = initialCleanUp;
  }

  this->indent = initialIndent;
  return code;
}
