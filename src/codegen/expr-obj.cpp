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

std::shared_ptr<CodegenASTExpr> Codegen::_exprObjDefaultField (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return CodegenASTExprCast::create(
      CodegenASTType::create(this->_("struct any")),
      CodegenASTExprInitList::create({
        CodegenASTExprLiteral::create("0"),
        CodegenASTExprAccess::create(this->_("NULL")),
        CodegenASTExprLiteral::create("0"),
        CodegenASTExprAccess::create(this->_("NULL")),
        CodegenASTExprAccess::create(this->_("NULL"))
      })
    );
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.typeName + "_alloc")),
      {CodegenASTExprLiteral::create("0")}
    );
  } else if (typeInfo.type->isBool()) {
    return CodegenASTExprAccess::create(this->_("false"));
  } else if (typeInfo.type->isChar()) {
    return CodegenASTExprLiteral::create(R"('\0')");
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried object expression default field on invalid type");
  } else if (typeInfo.type->isOpt()) {
    return CodegenASTExprAccess::create(this->_("NULL"));
  } else if (typeInfo.type->isObj() && typeInfo.type->builtin && typeInfo.type->codeName == "@buffer_Buffer") {
    return CodegenASTExprCast::create(
      CodegenASTType::create(this->_("struct buffer")),
      CodegenASTExprInitList::create()
    );
  } else if (typeInfo.type->isObj()) {
    auto cFields = std::vector<std::shared_ptr<CodegenASTExpr>>{};

    for (const auto &typeField : typeInfo.type->fields) {
      if (typeField.builtin || typeField.type->isMethod()) {
        continue;
      }

      auto fieldTypeInfo = this->_typeInfo(typeField.type);
      cFields.push_back(this->_exprObjDefaultField(fieldTypeInfo));
    }

    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.typeName + "_alloc")),
      cFields
    );
  } else if (typeInfo.type->isStr()) {
    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("str_alloc")),
      {CodegenASTExprLiteral::create(R"("")")}
    );
  } else {
    return CodegenASTExprLiteral::create("0");
  }
}

std::shared_ptr<CodegenASTExpr> Codegen::_exprObj (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);
  auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
  auto cFields = std::vector<std::shared_ptr<CodegenASTExpr>>{};

  for (const auto &typeField : nodeTypeInfo.type->fields) {
    if (typeField.builtin || typeField.type->isMethod()) {
      continue;
    }

    auto fieldTypeInfo = this->_typeInfo(typeField.type);

    auto prop = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&] (const auto &it) -> bool {
      return it.name == typeField.name;
    });

    cFields.push_back(
      prop != exprObj.props.end()
        ? this->_nodeExpr(prop->init, typeField.type, parent, c)
        : this->_exprObjDefaultField(fieldTypeInfo)
    );
  }

  auto expr = CodegenASTExprCall::create(
    CodegenASTExprAccess::create(this->_(nodeTypeInfo.typeName + "_alloc")),
    cFields
  );

  if (root) {
    expr = this->_genFreeFn(nodeTypeInfo.type, expr);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
