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

#include <algorithm>
#include "../Codegen.hpp"

std::string Codegen::_exprObjDefaultField (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return this->_apiEval("(_{struct any}) {0, _{NULL}, 0, _{NULL}, _{NULL}}");
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    return this->_apiEval("_{" + typeInfo.typeName + "_alloc}(0)");
  } else if (typeInfo.type->isBool()) {
    return this->_apiEval("_{false}");
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried object expression default field on invalid type");
  } else if (typeInfo.type->isOpt()) {
    return this->_apiEval("_{NULL}");
  } else if (typeInfo.type->isObj() && typeInfo.type->builtin && typeInfo.type->codeName == "@buffer_Buffer") {
    return this->_apiEval("(_{struct buffer}) {}");
  } else if (typeInfo.type->isObj()) {
    auto fieldsCode = std::string();

    for (const auto &typeField : typeInfo.type->fields) {
      if (typeField.builtin || typeField.type->isMethod()) {
        continue;
      }

      auto fieldTypeInfo = this->_typeInfo(typeField.type);
      fieldsCode += ", " + this->_exprObjDefaultField(fieldTypeInfo);
    }

    return this->_apiEval("_{" + typeInfo.typeName + "_alloc}(" + (fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2)) + ")", 1);
  } else if (typeInfo.type->isStr()) {
    return this->_apiEval(R"(_{str_alloc}(""))");
  } else {
    return "0";
  }
}

std::string Codegen::_exprObj (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);
  auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
  auto fieldsCode = std::string();

  for (const auto &typeField : nodeTypeInfo.type->fields) {
    if (typeField.builtin || typeField.type->isMethod()) {
      continue;
    }

    auto fieldTypeInfo = this->_typeInfo(typeField.type);

    auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&typeField] (const auto &it) -> bool {
      return it.name == typeField.name;
    });

    fieldsCode += ", ";

    if (exprObjProp != exprObj.props.end()) {
      fieldsCode += this->_nodeExpr(exprObjProp->init, typeField.type, parent, decl);
    } else {
      fieldsCode += this->_exprObjDefaultField(fieldTypeInfo);
    }
  }

  this->_activateEntity(nodeTypeInfo.typeName + "_alloc");
  auto code = nodeTypeInfo.typeName + "_alloc(" + (fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2)) + ")";
  code = !root ? code : this->_genFreeFn(nodeTypeInfo.type, code);

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
