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

std::string Codegen::_typeNameObj (Type *type) {
  auto typeName = type->builtin ? type->name : Codegen::typeName(type->codeName);

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto objFields = std::string();

    for (const auto &field : type->fields) {
      if (field.builtin || field.type->isMethod()) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      objFields += "  " + (field.mut ? fieldTypeInfo.typeCode : fieldTypeInfo.typeCodeConst) + fieldName + ";" EOL;
    }

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += objFields.empty() ? "  void *_;" EOL : objFields;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN);

  return typeName;
}

std::string Codegen::_typeNameObjDef (Type *type, const std::map<std::string, std::string> &extra) {
  this->_saveStateBuiltinsEntities();
  auto typeName = type->builtin ? type->name : Codegen::typeName(type->codeName);

  for (auto &entity : this->entities) {
    if (entity.name == typeName + "_alloc") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      auto allocFnParamTypes = std::string();
      auto allocFnParams = std::string();
      auto allocFnCode = std::string();

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);
          auto fieldTypeInfo = this->_typeInfo(field.type);

          allocFnParamTypes += ", " + fieldTypeInfo.typeCodeTrimmed;
          allocFnParams += ", " + fieldTypeInfo.typeCode + fieldName;
          allocFnCode += ", " + fieldName;
        }
      }

      allocFnParamTypes = allocFnParamTypes.empty() ? allocFnParamTypes : allocFnParamTypes.substr(2);
      allocFnParams = allocFnParams.empty() ? allocFnParams : allocFnParams.substr(2);
      allocFnCode = allocFnCode.empty() ? "(void *) 0" : allocFnCode.substr(2);

      entity.decl += "struct _{" + typeName + "} *" + typeName + "_alloc (" + allocFnParamTypes + ");";
      entity.def += "struct _{" + typeName + "} *" + typeName + "_alloc (" + allocFnParams + ") {" EOL;
      entity.def += "  struct _{" + typeName + "} *" + "r = _{alloc}(sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  struct _{" + typeName + "} s = {" + allocFnCode + "};" EOL;
      entity.def += "  _{memcpy}(r, &s, sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_copy") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "struct _{" + typeName + "} *" + typeName + "_copy (const struct _{" + typeName + "} *);";
      entity.def += "struct _{" + typeName + "} *" + typeName + "_copy (const struct _{" + typeName + "} *n) {" EOL;
      entity.def += "  struct _{" + typeName + "} *r = _{alloc}(sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  struct _{" + typeName + "} s = {";

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);

          entity.def += (fieldIdx == 0 ? "" : ", ") + this->_genCopyFn(field.type, "n->" + fieldName).str();
          fieldIdx++;
        }
      }

      entity.def += "};" EOL;
      entity.def += "  _{memcpy}(r, &s, sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_eq") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} *, struct _{" + typeName + "} *);";
      entity.def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} *n1, struct _{" + typeName + "} *n2) {" EOL;
      entity.def += "  _{bool} r = ";

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);

          entity.def += (fieldIdx == 0 ? "" : " && ") + this->_genEqFn(field.type, "n1->" + fieldName, "n2->" + fieldName).str();
          fieldIdx++;
        }
      }

      entity.def += ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n1").str() + ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n2").str() + ";" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_free") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      entity.decl += "void " + typeName + "_free (struct _{" + typeName + "} *);";
      entity.def += "void " + typeName + "_free (struct _{" + typeName + "} *n) {" EOL;

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod() && field.type->shouldBeFreed()) {
          auto fieldName = Codegen::name(field.name);
          entity.def += "  " + this->_genFreeFn(field.type, "n->" + fieldName).str() + ";" EOL;
        }
      }

      entity.def += extra.contains("free") ? extra.at("free") : "";
      entity.def += "  _{free}(n);" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_ne") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} *, struct _{" + typeName + "} *);";
      entity.def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} *n1, struct _{" + typeName + "} *n2) {" EOL;
      entity.def += "  _{bool} r = ";

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);
          entity.def += (fieldIdx == 0 ? "" : " || ") + this->_genEqFn(field.type, "n1->" + fieldName, "n2->" + fieldName, true).str();
          fieldIdx++;
        }
      }

      entity.def += ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n1").str() + ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n2").str() + ";" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_realloc") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      entity.decl += "struct _{" + typeName + "} *" + typeName + "_realloc ";
      entity.decl += "(struct _{" + typeName + "} *, struct _{" + typeName + "} *);";
      entity.def += "struct _{" + typeName + "} *" + typeName + "_realloc ";
      entity.def += "(struct _{" + typeName + "} *n1, struct _{" + typeName + "} *n2) {" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n1").str() + ";" EOL;
      entity.def += "  return n2;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_str") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} *);";
      entity.def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} *n) {" EOL;
      entity.def += R"(  _{struct str} r = _{str_alloc}(")" + type->name + R"({");)" EOL;

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);
          auto fieldCode = std::string(field.type->isRef() ? "*" : "") + "n->" + fieldName;
          auto strCodeDelimiter = std::string(fieldIdx == 0 ? "" : ", ");

          if (field.type->isStr()) {
            entity.def += R"(  r = _{str_concat_cstr}(r, ")" + strCodeDelimiter + field.name + R"(: \"");)" EOL;
            entity.def += "  r = _{str_concat_str}(r, " + this->_genStrFn(field.type, fieldCode).str() + ");" EOL;
            entity.def += R"(  r = _{str_concat_cstr}(r, "\"");)" EOL;
          } else {
            entity.def += R"(  r = _{str_concat_cstr}(r, ")" + strCodeDelimiter + field.name + R"(: ");)" EOL;
            entity.def += "  r = _{str_concat_str}(r, " + this->_genStrFn(field.type, fieldCode).str() + ");" EOL;
          }

          fieldIdx++;
        }
      }

      entity.def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
      entity.def += R"(  return _{str_concat_cstr}(r, "}");)" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    }
  }

  this->_restoreStateBuiltinsEntities();
  return typeName;
}
