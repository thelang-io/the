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

std::string Codegen::_nodeIf (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeIf = std::get<ASTNodeIf>(*node.body);
  auto initialIndent = this->indent;
  auto initialStateTypeCasts = this->state.typeCasts;
  auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(nodeIf.cond, node);
  auto nodeIfCondCode = this->_nodeExpr(nodeIf.cond, this->ast->typeMap.get("bool"), node, decl);
  auto hasAwait = ASTChecker(node).hasExpr<ASTExprAwait>();

  if (hasAwait) {
    this->indent -= 2;
  }

  bodyTypeCasts.merge(this->state.typeCasts);
  bodyTypeCasts.swap(this->state.typeCasts);
  this->varMap.save();
  auto nodeIfBodyCode = this->_block(nodeIf.body);
  this->varMap.restore();
  this->state.typeCasts = initialStateTypeCasts;
  auto asyncAltCounter = static_cast<std::size_t>(0);
  auto nodeIfAltCode = std::string();

  if (nodeIf.alt != std::nullopt) {
    if (hasAwait) {
      asyncAltCounter = ++this->state.asyncCounter;
      this->indent = std::holds_alternative<ASTNode>(*nodeIf.alt) ? initialIndent : this->indent;
    }

    altTypeCasts.merge(this->state.typeCasts);
    altTypeCasts.swap(this->state.typeCasts);

    if (std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
      this->varMap.save();
      nodeIfAltCode += this->_block(std::get<ASTBlock>(*nodeIf.alt));
      this->varMap.restore();
    } else if (std::holds_alternative<ASTNode>(*nodeIf.alt)) {
      nodeIfAltCode += this->_node(std::get<ASTNode>(*nodeIf.alt));
    }
  }

  if (hasAwait) {
    this->indent = initialIndent - 2;

    if (nodeIf.alt != std::nullopt && std::holds_alternative<ASTBlock>(*nodeIf.alt) && !ASTChecker(node).isLast()) {
      nodeIfAltCode += std::string(this->indent, ' ') + "}" EOL;
      nodeIfAltCode += std::string(this->indent, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;
    }

    code = std::string(this->indent + 2, ' ') + "if (!(" + nodeIfCondCode + ")) {" EOL;
    code += std::string(this->indent + 4, ' ') + "return ";
    code += std::to_string(asyncAltCounter == 0 ? this->state.asyncCounter : asyncAltCounter) + ";" EOL;
    code += std::string(this->indent + 2, ' ') + "}" EOL;
    code += nodeIfBodyCode;

    if (asyncAltCounter != 0) {
      code += std::string(this->indent + 2, ' ') + "return " + std::to_string(this->state.asyncCounter) + ";" EOL;
      code += std::string(this->indent, ' ') + "}" EOL;
      code += std::string(this->indent, ' ') + "case " + std::to_string(asyncAltCounter) + ": {" EOL;
    }

    code += nodeIfAltCode;
  } else {
    code = std::string(this->indent, ' ') + "if (" + nodeIfCondCode + ") {" EOL;
    code += nodeIfBodyCode;

    if (nodeIf.alt != std::nullopt) {
      code += std::string(this->indent, ' ') + "} else ";

      if (std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
        code += "{" EOL + nodeIfAltCode;
        code += std::string(this->indent, ' ') + "}" EOL;
      } else if (std::holds_alternative<ASTNode>(*nodeIf.alt)) {
        code += nodeIfAltCode.substr(nodeIfAltCode.find_first_not_of(' '));
      }
    } else {
      code += std::string(this->indent, ' ') + "}" EOL;
    }
  }

  this->state.typeCasts = initialStateTypeCasts;
  this->indent = initialIndent;
  return this->_wrapNode(node, root, phase, decl + code);
}
