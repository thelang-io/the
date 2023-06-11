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

std::string Codegen::_nodeFnDecl (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

  code += this->_fnDecl(
    nodeFnDecl.var,
    nodeFnDecl.stack,
    nodeFnDecl.params,
    nodeFnDecl.body,
    node,
    phase
  );

  return this->_wrapNode(node, root, phase, decl + code);
}
