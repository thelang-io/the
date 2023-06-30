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

void Codegen::_nodeFnDecl (CodegenASTStmt *c, const ASTNode &node, CodegenPhase phase) {
  auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);
  this->_fnDecl(c, nodeFnDecl.var, nodeFnDecl.stack, nodeFnDecl.params, nodeFnDecl.body, node, phase);
}
