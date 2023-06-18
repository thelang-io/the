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

std::string Codegen::_nodeThrow (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeThrow = std::get<ASTNodeThrow>(*node.body);
  auto argTypeInfo = this->_typeInfo(nodeThrow.arg.type);
  auto argNodeExpr = this->_nodeExpr(nodeThrow.arg, argTypeInfo.type, node, decl);
  auto argNodeExprDef = this->_typeDef(argTypeInfo.type);
  auto line = std::to_string(node.start.line);
  auto col = std::to_string(node.start.col + 1);

  code += std::string(this->indent, ' ') + "_{error_assign}(&_{err_state}, _{" + argNodeExprDef + "}, ";
  code += "(void *) " + argNodeExpr + ", (void (*) (void *)) &_{" + argTypeInfo.typeName + "_free}, " + line + ", " + col + ");" EOL;

  if (this->state.cleanUp.isClosestJump()) {
    code += std::string(this->indent, ' ') + this->_apiEval("_{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);" EOL);
  } else {
    code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
  }

  code = this->_apiEval(code);
  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeThrowAsync (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  return this->_nodeThrow(node, root, phase, decl, code);
}
