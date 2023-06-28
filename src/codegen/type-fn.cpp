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

std::string Codegen::_typeNameFn (Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto fnType = std::get<TypeFn>(type->body);
  auto hasSelfParam = fnType.isMethod && fnType.callInfo.isSelfFirst;
  auto hasParams = this->throws || !fnType.params.empty() || hasSelfParam;
  auto paramsName = typeName + "P";

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto returnTypeInfo = this->_typeInfo(fnType.returnType);

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  " + (fnType.async ? "int " : returnTypeInfo.typeCode) + "(*f) (";
    def += fnType.async ? "_{threadpool_t} *, _{threadpool_job_t} *, " : "";
    def += "void *" + std::string(hasParams || fnType.async ? ", void *" : "");
    def += fnType.async ? ", void *, int" : "";
    def += ");" EOL;
    def += "  void *x;" EOL;
    def += "  _{size_t} l;" EOL;
    def += "};";

    return 0;
  });

  if (hasParams) {
    this->_apiEntity(paramsName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
      auto paramsCode = std::string();
      auto paramIdx = static_cast<std::size_t>(0);

      if (this->throws) {
        paramsCode += "  int line;" EOL;
        paramsCode += "  int col;" EOL;
      }

      if (hasSelfParam) {
        auto selfTypeInfo = this->_typeInfo(fnType.callInfo.selfType);
        paramsCode += "  " + (fnType.callInfo.isSelfMut ? selfTypeInfo.typeCode : selfTypeInfo.typeCodeConst) + "self;" EOL;
      }

      for (const auto &param : fnType.params) {
        auto paramTypeInfo = this->_typeInfo(param.type);
        auto paramIdxStr = std::to_string(paramIdx);

        if (!param.required && !param.variadic) {
          paramsCode += "  unsigned char o" + paramIdxStr + ";" EOL;
        }

        paramsCode += "  " + paramTypeInfo.typeCode + "n" + paramIdxStr + ";" EOL;
        paramIdx++;
      }

      decl += "struct " + paramsName + ";";
      def += "struct " + paramsName + " {" EOL;
      def += paramsCode;
      def += "};";

      return 0;
    });
  }

  this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.x == _{NULL}) return n;" EOL;
    def += "  void *x = _{alloc}(n.l);" EOL;
    def += "  _{memcpy}(x, n.x, n.l);" EOL;
    def += "  return (struct _{" + typeName + "}) {n.f, x, n.l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.x != _{NULL}) _{free}(n.x);" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  " + this->_genFreeFn(type, "n1").str() + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += R"(  return _{str_alloc}("[Function]");)" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}
