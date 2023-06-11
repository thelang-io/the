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

std::string Codegen::_exprArray (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprArray = std::get<ASTExprArray>(*nodeExpr.body);
  auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
  auto arrayType = std::get<TypeArray>(nodeTypeInfo.type->body);
  auto fieldsCode = std::to_string(exprArray.elements.size());

  for (const auto &element : exprArray.elements) {
    fieldsCode += ", " + this->_nodeExpr(element, arrayType.elementType, parent, decl);
  }

  auto code = this->_apiEval("_{" + nodeTypeInfo.typeName + "_alloc}(" + fieldsCode + ")", 1);
  code = !root ? code : this->_genFreeFn(nodeTypeInfo.type, code);

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
