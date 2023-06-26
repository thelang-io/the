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

std::string Codegen::_nodeContinue (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_LOOP)) {
    if (!ASTChecker(node.parent).is<ASTNodeLoop>()) {
      code = std::string(this->indent, ' ') + this->state.cleanUp.currentContinueVar() + " = 1;" EOL;
    }

    if (!ASTChecker(node).isLast()) {
      code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
    }
  } else {
    code = std::string(this->indent, ' ') + "continue;" EOL;
  }

  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeContinueAsync (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto asyncCounter = node.codegenAsyncCounter == nullptr || *node.codegenAsyncCounter == 0
    ? this->_findClosestAsyncBufferItem(CodegenAsyncBufferItemLoopContinue)
    : *node.codegenAsyncCounter;

  code = std::string(this->indent, ' ') + "*" + this->state.cleanUp.currentContinueVar() + " = 1;" EOL;

  if (asyncCounter != this->state.asyncCounter + 1 || !ASTChecker(node).isLast()) {
    code += std::string(this->indent, ' ') + "return " + std::to_string(asyncCounter) + ";" EOL;
  }

  return this->_wrapNode(node, root, phase, decl + code);
}
