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

std::string Codegen::_typeNameAny (Type *type) {
  this->_saveStateBuiltinsEntities();
  auto typeInfoTypeName = this->_typeInfo(type).typeName;
  this->_restoreStateBuiltinsEntities();

  auto typeName = Codegen::typeName("any_" + (typeInfoTypeName.starts_with("__THE_1_") ? typeInfoTypeName.substr(8) : typeInfoTypeName));

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  " + typeInfo.typeCode + "d;" EOL;
    def += "};";

    return 0;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    auto cCopy = this->_genCopyFn(
      typeInfo.type,
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("o"), "d", true)
    );

    decl += "_{struct any} " + typeName + "_copy (const _{struct any});";
    def += "_{struct any} " + typeName + "_copy (const _{struct any} n) {" EOL;
    def += "  struct _{" + typeName + "} *o = n.d;" EOL;
    def += "  struct _{" + typeName + "} *r = _{alloc}(n.l);" EOL;
    def += "  r->d = " + cCopy->str() + ";" EOL;
    def += "  return (_{struct any}) {n.t, r, n.l, n._copy, n._free};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "void " + typeName + "_free (_{struct any});";
    def += "void " + typeName + "_free (_{struct any} _n) {" EOL;
    def += "  struct _{" + typeName + "} *n = _n.d;" EOL;

    if (typeInfo.type->shouldBeFreed()) {
      auto cFree = this->_genFreeFn(
        typeInfo.type,
        CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "d", true)
      );

      def += "  " + cFree->str() + ";" EOL;
    }

    def += "  _{free}(n);" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "_{struct any} " + typeName + "_alloc (" + typeInfo.typeCodeTrimmed + ");";
    def += "_{struct any} " + typeName + "_alloc (" + typeInfo.typeCode + "d) {" EOL;
    def += "  _{size_t} l = sizeof(struct _{" + typeName + "});" EOL;
    def += "  struct _{" + typeName + "} *r = _{alloc}(l);" EOL;
    def += "  r->d = d;" EOL;
    def += "  return (_{struct any}) {_{" + this->_typeDef(typeInfo.type) + "}, ";
    def += "r, l, &_{" + typeName + "_copy}, &_{" + typeName + "_free}};" EOL;
    def += "}";

    return copyFnEntityIdx;
  });

  return typeName;
}
