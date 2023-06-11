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

std::string Codegen::_exprAwait (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprAwait = std::get<ASTExprAwait>(*nodeExpr.body);
  auto code = std::string();

  if (!nodeExpr.type->isVoid() && !root) {
    code += "*t" + std::to_string(exprAwait.id);
  }

  decl += std::string(this->indent, ' ') + this->_nodeExpr(exprAwait.arg, exprAwait.arg.type, parent, decl, false, exprAwait.id) + ";" EOL;
  decl += std::string(this->indent, ' ') + "return " + std::to_string(this->state.asyncCounter + 1) + ";" EOL;
  decl += std::string(this->indent - 2, ' ') + "}" EOL;
  decl += std::string(this->indent - 2, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
