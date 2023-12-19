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

std::string Codegen::_typeNameOpt (Type *type) {
  auto typeName = Codegen::typeName(type->name);
  auto allocTypeName = typeName + "_alloc";

  for (const auto &entity : this->entities) {
    if (entity.name == allocTypeName) {
      return typeName;
    }
  }

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += underlyingTypeInfo.typeRefCode + typeName + "_alloc (" + underlyingTypeInfo.typeCodeTrimmed + ");";
    def += underlyingTypeInfo.typeRefCode + typeName + "_alloc (" + underlyingTypeInfo.typeCode + "n) {" EOL;
    def += "  " + underlyingTypeInfo.typeRefCode + "r = _{alloc}(sizeof(" + underlyingTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  *r = n;" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);
    auto cFree = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n"));

    decl += "void " + typeName + "_free (" + underlyingTypeInfo.typeRefCode + ");";
    def += "void " + typeName + "_free (" + underlyingTypeInfo.typeRefCode + "n) {" EOL;
    def += "  if (n == _{NULL}) return;" EOL;

    if (underlyingTypeInfo.type->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(underlyingTypeInfo.type, cFree)->str() + ";" EOL;
    }

    def += "  _{free}(n);" EOL;
    def += "}";

    return 0;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);
    auto cCopy = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n"));

    decl += underlyingTypeInfo.typeRefCode + typeName + "_copy (const " + underlyingTypeInfo.typeRefCode + ");";
    def += underlyingTypeInfo.typeRefCode + typeName + "_copy (const " + underlyingTypeInfo.typeRefCode + "n) {" EOL;
    def += "  if (n == _{NULL}) return _{NULL};" EOL;
    def += "  " + underlyingTypeInfo.typeRefCode + "r = _{alloc}(sizeof(" + underlyingTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  *r = " + this->_genCopyFn(underlyingTypeInfo.type, cCopy)->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return freeFnEntityIdx;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);
    auto cEq1 = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n1"));
    auto cEq2 = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n2"));

    decl += "_{bool} " + typeName + "_eq (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
    def += "_{bool} " + typeName + "_eq (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
    def += "  _{bool} r = (n1 == _{NULL} || n2 == _{NULL}) ? n1 == _{NULL} && n2 == _{NULL} : ";

    if (underlyingTypeInfo.type->isAny()) {
      def += this->_genEqFn(underlyingTypeInfo.type, CodegenASTExprAccess::create("n1"), CodegenASTExprAccess::create("n2"))->str();
    } else {
      def += this->_genEqFn(underlyingTypeInfo.type, cEq1, cEq2)->str();
    }

    def += ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n2"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);
    auto cEq1 = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n1"));
    auto cEq2 = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n2"));

    decl += "_{bool} " + typeName + "_ne (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
    def += "_{bool} " + typeName + "_ne (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
    def += "  _{bool} r = (n1 == _{NULL} || n2 == _{NULL}) ? n1 != _{NULL} || n2 != _{NULL} : ";

    if (underlyingTypeInfo.type->isAny()) {
      def += this->_genEqFn(underlyingTypeInfo.type, CodegenASTExprAccess::create("n1"), CodegenASTExprAccess::create("n2"), true)->str();
    } else {
      def += this->_genEqFn(underlyingTypeInfo.type, cEq1, cEq2, true)->str();
    }

    def += ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n2"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += underlyingTypeInfo.typeRefCode + typeName + "_realloc (" + underlyingTypeInfo.typeRefCode;
    decl += ", " + underlyingTypeInfo.typeRefCode + ");";
    def += underlyingTypeInfo.typeRefCode + typeName + "_realloc (" + underlyingTypeInfo.typeRefCode + "n1";
    def += ", " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);
    auto cStr = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("n"));

    decl += "_{struct str} " + typeName + "_str (" + underlyingTypeInfo.typeRefCode + ");";
    def += "_{struct str} " + typeName + "_str (" + underlyingTypeInfo.typeRefCode + "n) {" EOL;
    def += R"(  if (n == _{NULL}) return _{str_alloc}("nil");)" EOL;

    if (underlyingTypeInfo.realType->isStr()) {
      def += R"(  _{struct str} r = _{str_alloc}("\"");)" EOL;
      def += "  r = _{str_concat_str}(r, " + this->_genStrFn(underlyingTypeInfo.realType, cStr)->str() + ");" EOL;
      def += R"(  r = _{str_concat_cstr}(r, "\"");)" EOL;
    } else {
      def += "  _{struct str} r = " + this->_genStrFn(underlyingTypeInfo.realType, cStr)->str() + ";" EOL;
    }

    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}
