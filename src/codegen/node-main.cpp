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

std::string Codegen::_nodeMain (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeMain = std::get<ASTNodeMain>(*node.body);
  auto initialIndent = this->indent;

  this->indent = 0;
  this->varMap.save();
  code = this->_block(nodeMain.body);
  this->varMap.restore();
  this->indent = initialIndent;

  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeMainAsync (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeMain = std::get<ASTNodeMain>(*node.body);
  auto returnType = this->ast->typeMap.get("void");
  auto asyncMainType = this->ast->typeMap.createFn({}, returnType, this->throws, this->async);
  auto asyncMainVar = std::make_shared<Var>(Var{"async_main", "async_main", asyncMainType, false, false, true, false, 0});

  code += this->_fnDecl(asyncMainVar, nodeMain.stack, {}, nodeMain.body, node, CODEGEN_PHASE_FULL);
  code += std::string(this->indent, ' ') + this->_apiEval("_{threadpool_add}(tp, async_main.f, async_main.x, _{NULL}, _{NULL}, _{NULL});" EOL);

  return this->_wrapNode(node, root, phase, decl + code);
}
