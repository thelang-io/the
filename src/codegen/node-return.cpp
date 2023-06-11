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

#include "../ASTChecker.hpp"
#include "../Codegen.hpp"
#include "../config.hpp"

std::string Codegen::_nodeReturn (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

  if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN) || this->state.cleanUp.returnVarUsed || this->state.insideAsync) {
    auto parentNotRoot = this->state.cleanUp.parent != nullptr && this->state.cleanUp.parent->type != CODEGEN_CLEANUP_ROOT;

    if (parentNotRoot && this->state.cleanUp.parent->hasCleanUp(CODEGEN_CLEANUP_FN)) {
      code += std::string(this->indent, ' ') + this->state.cleanUp.currentReturnVar() + " = 1;" EOL;
    }

    if (nodeReturn.body != std::nullopt) {
      code += std::string(this->indent, ' ') + (this->state.insideAsync ? "*" : "");
      code += this->state.cleanUp.currentValueVar() + " = ";
      code += this->_nodeExpr(*nodeReturn.body, this->state.returnType, node, decl) + ";" EOL;
    }

    auto nodeParentFunction = ASTChecker(node.parent).is<ASTNodeFnDecl>() || ASTChecker(node.parent).is<ASTNodeObjDecl>();
    auto nodeIsLast = node.parent != nullptr && ASTChecker(node).isLast();

    if ((!nodeParentFunction && this->state.cleanUp.empty()) || !nodeIsLast) {
      code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
    }
  } else if (nodeReturn.body != std::nullopt) {
    code = std::string(this->indent, ' ') + "return " + this->_nodeExpr(*nodeReturn.body, this->state.returnType, node, decl) + ";" EOL;
  } else {
    code = std::string(this->indent, ' ') + "return;" EOL;
  }

  return this->_wrapNode(node, root, phase, decl + code);
}
