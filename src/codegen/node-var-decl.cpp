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

std::string Codegen::_nodeVarDeclInit (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return this->_apiEval("{0, _{NULL}, 0, _{NULL}, _{NULL}}");
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    return this->_apiEval("_{" + typeInfo.typeName + "_alloc}(0)", 1);
  } else if (typeInfo.type->isBool()) {
    return this->_apiEval("_{false}");
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried node variable declaration of invalid type");
  } else if (typeInfo.type->isObj()) {
    auto fieldsCode = std::string();

    for (const auto &typeField : typeInfo.type->fields) {
      if (!typeField.builtin && !typeField.type->isMethod()) {
        fieldsCode += ", " + this->_exprObjDefaultField(this->_typeInfo(typeField.type));
      }
    }

    auto allocCode = fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2);
    return this->_apiEval("_{" + typeInfo.typeName + "_alloc}(" + allocCode + ")", 1);
  } else if (typeInfo.type->isOpt()) {
    return this->_apiEval("_{NULL}");
  } else if (typeInfo.type->isStr()) {
    return this->_apiEval(R"(_{str_alloc}(""))");
  } else {
    return "0";
  }
}

std::string Codegen::_nodeVarDecl (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  if (this->state.insideAsync) {
    return this->_nodeVarDeclAsync(node, root, phase, decl, code);
  }

  auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
  auto name = Codegen::name(nodeVarDecl.var->codeName);
  auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);
  auto initCode = std::string();

  if (nodeVarDecl.init != std::nullopt) {
    initCode = this->_nodeExpr(*nodeVarDecl.init, typeInfo.type, node, decl);
  } else {
    initCode = this->_nodeVarDeclInit(typeInfo);
  }

  code = !root ? code : std::string(this->indent, ' ');
  code += nodeVarDecl.var->mut ? typeInfo.typeCode : typeInfo.typeCodeConst;
  code += name + " = " + initCode + (root ? ";" EOL : "");

  if (typeInfo.type->shouldBeFreed()) {
    this->state.cleanUp.add(this->_genFreeFn(typeInfo.type, name) + ";");
  }

  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeVarDeclAsync (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
  auto name = Codegen::name(nodeVarDecl.var->codeName);
  auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);
  auto initCode = std::string();

  if (nodeVarDecl.init != std::nullopt) {
    initCode = this->_nodeExpr(*nodeVarDecl.init, typeInfo.type, node, decl);
  } else {
    initCode = this->_nodeVarDeclInit(typeInfo);
  }

  code = !root ? code : std::string(this->indent, ' ') + "*" + name + " = " + initCode + (root ? ";" EOL : "");

  if (typeInfo.type->shouldBeFreed()) {
    this->state.cleanUp.add(this->_genFreeFn(typeInfo.type, "*" + name) + ";");
  }

  return this->_wrapNode(node, root, phase, decl + code);
}
