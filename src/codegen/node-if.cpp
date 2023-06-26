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

std::string Codegen::_nodeIf (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeIf = std::get<ASTNodeIf>(*node.body);
  auto initialIndent = this->indent;
  auto initialStateTypeCasts = this->state.typeCasts;
  auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(nodeIf.cond, node);
  auto nodeIfCondCode = this->_nodeExpr(nodeIf.cond, this->ast->typeMap.get("bool"), node, decl);

  bodyTypeCasts.merge(this->state.typeCasts);
  bodyTypeCasts.swap(this->state.typeCasts);
  this->varMap.save();
  auto nodeIfBodyCode = this->_block(nodeIf.body);
  this->varMap.restore();
  this->state.typeCasts = initialStateTypeCasts;
  auto nodeIfAltCode = std::string();

  if (nodeIf.alt != std::nullopt) {
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

  this->state.typeCasts = initialStateTypeCasts;
  this->indent = initialIndent;
  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeIfAsync (ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeIf = std::get<ASTNodeIf>(*node.body);
  auto initialIndent = this->indent;
  auto initialStateTypeCasts = this->state.typeCasts;
  auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(nodeIf.cond, node);
  auto condCode = this->_nodeExpr(nodeIf.cond, this->ast->typeMap.get("bool"), node, decl);

  bodyTypeCasts.merge(this->state.typeCasts);
  bodyTypeCasts.swap(this->state.typeCasts);
  this->indent -= 2;
  this->varMap.save();
  auto bodyCode = this->_block(nodeIf.body);
  this->varMap.restore();
  this->state.typeCasts = initialStateTypeCasts;

  auto afterBodyCode = std::string();
  afterBodyCode += std::string(this->indent, ' ') + "}" EOL;
  afterBodyCode += std::string(this->indent, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;

  auto afterBodyAsyncCounter = this->state.asyncCounter;
  auto altCode = std::string();

  if (nodeIf.alt != std::nullopt) {
    altTypeCasts.merge(this->state.typeCasts);
    altTypeCasts.swap(this->state.typeCasts);

    if (std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
      this->varMap.save();
      altCode += this->_block(std::get<ASTBlock>(*nodeIf.alt));
      this->varMap.restore();
    } else if (std::holds_alternative<ASTNode>(*nodeIf.alt)) {
      this->indent = initialIndent;
      altCode += this->_nodeAsync(std::get<ASTNode>(*nodeIf.alt));
      this->indent -= 2;
    }

    altCode += std::string(this->indent, ' ') + "}" EOL;
    altCode += std::string(this->indent, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;
  }

  code = std::string(this->indent + 2, ' ') + "if (!(" + condCode + ")) {" EOL;
  code += std::string(this->indent + 4, ' ') + "return " + std::to_string(afterBodyAsyncCounter) + ";" EOL;
  code += std::string(this->indent + 2, ' ') + "}" EOL;
  code += bodyCode;
  code += std::string(this->indent + 2, ' ') + "return ";
  code += std::to_string(this->state.asyncCounter) + ";" EOL;
  code += afterBodyCode;
  code += altCode;

  node.codegenAsyncCounter = std::make_shared<std::size_t>(this->state.asyncCounter);
  this->state.typeCasts = initialStateTypeCasts;
  this->indent = initialIndent;
  return this->_wrapNode(node, root, phase, decl + code);
}
