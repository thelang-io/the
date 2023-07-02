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
#include "../config.hpp"

std::string Codegen::_typeNameArray (Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto elementType = std::get<TypeArray>(type->body).elementType;

  auto cElementRealTypeExpr = CodegenASTExprAccess::create(
    CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "d"),
    CodegenASTExprAccess::create("i")
  );

  auto cElementRealType = elementType->isRef()
    ? CodegenASTExprUnary::create("*", cElementRealTypeExpr)
    : cElementRealTypeExpr;

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d;" EOL;
    def += "  _{size_t} l;" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_alloc (_{size_t}, ...);";
    def += "struct _{" + typeName + "} " + typeName + "_alloc (_{size_t} x, ...) {" EOL;
    def += "  if (x == 0) return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(x * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{va_list} args;" EOL;
    def += "  _{va_start}(args, x);" EOL;
    def += "  for (_{size_t} i = 0; i < x; i++) ";
    def += "d[i] = _{va_arg}(args, " + elementType->vaArgCode(elementTypeInfo.typeCodeTrimmed) + ");" EOL;
    def += "  _{va_end}(args);" EOL;
    def += "  return (struct _{" + typeName + "}) {d, x};" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cFree = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;

    if (elementTypeInfo.type->shouldBeFreed()) {
      def += "  for (_{size_t} i = 0; i < n.l; i++) " + this->_genFreeFn(elementTypeInfo.type, cFree)->str() + ";" EOL;
    }

    def += "  _{free}(n.d);" EOL;
    def += "}";

    return 0;
  });

  auto atFnEntityIdx = this->_apiEntity(typeName + "_at", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeRefCode + typeName + "_at (struct _{" + typeName + "}, _{int32_t}, int, int);";
    def += elementTypeInfo.typeRefCode + typeName + "_at (struct _{" + typeName + "} n, _{int32_t} i, int line, int col) {" EOL;
    def += R"(  if ((i >= 0 && i >= n.l) || (i < 0 && i < -((_{int32_t}) n.l))) {)" EOL;
    def += R"(    const char *fmt = "index %" _{PRId32} " out of array bounds";)" EOL;
    def += R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, i);)" EOL;
    def += R"(    char *d = _{alloc}(z + 1);)" EOL;
    def += R"(    _{sprintf}(d, fmt, i);)" EOL;
    def += R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL;
    def += R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL;
    def += R"(  })" EOL;
    def += R"(  return i < 0 ? &n.d[n.l + i] : &n.d[i];)" EOL;
    def += R"(})";

    return freeFnEntityIdx;
  });

  auto clearFnEntityIdx = this->_apiEntity(typeName + "_clear", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto cFree = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("self"));

    decl += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *);";
    def += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *self) {" EOL;
    def += "  " + this->_genFreeFn(type, cFree)->str() + ";" EOL;
    def += "  self->d = _{NULL};" EOL;
    def += "  self->l = 0;" EOL;
    def += "  return self;" EOL;
    def += "}";

    return atFnEntityIdx + 1;
  });

  auto concatFnEntityIdx = this->_apiEntity(typeName + "_concat", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "concat";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy1 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d"),
      CodegenASTExprAccess::create("i")
    );

    auto cCopy2 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n1"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "struct _{" + typeName + "} " + typeName + "_concat (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} " + typeName + "_concat (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{size_t} l = self.l + n1.l;" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{size_t} k = 0;" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) d[k++] = ";
    def += this->_genCopyFn(elementTypeInfo.type, cCopy1)->str() + ";" EOL;
    def += "  for (_{size_t} i = 0; i < n1.l; i++) d[k++] = ";
    def += this->_genCopyFn(elementTypeInfo.type, cCopy2)->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("self"))->str() + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, l};" EOL;
    def += "}";

    return clearFnEntityIdx + 1;
  });

  auto containsFnEntityIdx = this->_apiEntity(typeName + "_contains", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "contains";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cEq1 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d"),
      CodegenASTExprAccess::create("i")
    );

    auto cEq2 = CodegenASTExprAccess::create("n1");

    decl += "_{bool} " + typeName + "_contains (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "_{bool} " + typeName + "_contains (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{bool} r = _{false};" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(elementTypeInfo.type, cEq1, cEq2)->str() + ") {" EOL;
    def += "      r = _{true};" EOL;
    def += "      break;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;

    if (param1TypeInfo.type->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(param1TypeInfo.type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    }

    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("self"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return concatFnEntityIdx + 1;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.l == 0) return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(n.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) d[i] = " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, n.l};" EOL;
    def += "}";

    return containsFnEntityIdx + 1;
  });

  auto emptyFnEntityIdx = this->_apiEntity(typeName + "_empty", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_empty (struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_empty (struct _{" + typeName + "} n) {" EOL;
    def += "  _{bool} r = n.l == 0;" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cEq1 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n1"), "d"),
      CodegenASTExprAccess::create("i")
    );

    auto cEq2 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n2"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l == n2.l;" EOL;
    def += "  if (r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(elementTypeInfo.type, cEq1, cEq2, true)->str() + ") {" EOL;
    def += "        r = _{false};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n2"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return emptyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_filter", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "filter";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "struct _{" + typeName + "} " + typeName + "_filter (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ", int, int);";
    def += "struct _{" + typeName + "} " + typeName + "_filter (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1, int line, int col) {" EOL;
    def += "  _{size_t} l = 0;" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(self.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) {" EOL;
    def += "    if (n1.f(n1.x, _{xalloc}(&(struct _{" + param1TypeInfo.typeName + "P}) ";
    def += "{line, col, " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + "}, sizeof(struct _{" + param1TypeInfo.typeName + "P})))) {" EOL;
    def += "      d[l++] = " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + ";" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(param1TypeInfo.type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("self"))->str() + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_first", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeRefCode + typeName + "_first (struct _{" + typeName + "} *, int, int);";
    def += elementTypeInfo.typeRefCode + typeName + "_first (struct _{" + typeName + "} *self, int line, int col) {" EOL;
    def += R"(  if (self->l == 0) {)" EOL;
    def += R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("tried getting first element of empty array"), (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL;
    def += R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL;
    def += R"(  })" EOL;
    def += R"(  return &self->d[0];)" EOL;
    def += R"(})";

    return 0;
  });

  this->_apiEntity(typeName + "_forEach", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "forEach";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "void " + typeName + "_forEach (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ", int, int);";
    def += "void " + typeName + "_forEach (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1, int line, int col) {" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) {" EOL;
    def += "    n1.f(n1.x, _{xalloc}(&(struct _{" + param1TypeInfo.typeName + "P}) ";
    def += "{line, col, " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + ", i}, ";
    def += "sizeof(struct _{" + param1TypeInfo.typeName + "P})));" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(param1TypeInfo.type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("self"))->str() + ";" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_join", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{struct str} " + typeName + "_join (struct _{" + typeName + "}, unsigned char, _{struct str});";
    def += "_{struct str} " + typeName + "_join (struct _{" + typeName + "} n, unsigned char o1, _{struct str} n1) {" EOL;
    def += R"(  _{struct str} x = o1 == 0 ? _{str_alloc}(",") : n1;)" EOL;
    def += R"(  _{struct str} r = _{str_alloc}("");)" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += "    if (i != 0) r = _{str_concat_str}(r, _{str_copy}(x));" EOL;
    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(elementTypeInfo.realType, cElementRealType, true, false)->str() + ");" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(this->ast->typeMap.get("str"), CodegenASTExprAccess::create("x"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += R"(  return r;)" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_last", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeRefCode + typeName + "_last (struct _{" + typeName + "} *, int, int);";
    def += elementTypeInfo.typeRefCode + typeName + "_last (struct _{" + typeName + "} *self, int line, int col) {" EOL;
    def += "  if (self->l == 0) {" EOL;
    def += R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("tried getting last element of empty array"), (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL;
    def += R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL;
    def += "  }" EOL;
    def += "  return &self->d[self->l - 1];" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_len", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_len (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_len (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} l = n.l;" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "  return l;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_merge", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "merge";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n1"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{size_t} k = self->l;" EOL;
    def += "  self->l += n1.l;" EOL;
    def += "  self->d = _{re_alloc}(self->d, self->l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n1.l; i++) self->d[k++] = " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  return self;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cEq1 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n1"), "d"),
      CodegenASTExprAccess::create("i")
    );

    auto cEq2 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n2"), "d"),
      CodegenASTExprAccess::create("i")
    );

    decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l != n2.l;" EOL;
    def += "  if (!r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(elementTypeInfo.type, cEq1, cEq2, true)->str() + ") {" EOL;
    def += "        r = _{true};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n2"))->str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_pop", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeCode + typeName + "_pop (struct _{" + typeName + "} *);";
    def += elementTypeInfo.typeCode + typeName + "_pop (struct _{" + typeName + "} *n) {" EOL;
    def += "  n->l--;" EOL;
    def += "  return n->d[n->l];" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_push", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "void " + typeName + "_push (struct _{" + typeName + "} *, struct _{" + typeName + "});";
    def += "void " + typeName + "_push (struct _{" + typeName + "} *n, struct _{" + typeName + "} m) {" EOL;
    def += "  if (m.l == 0) return;" EOL;
    def += "  n->l += m.l;" EOL;
    def += "  n->d = _{re_alloc}(n->d, n->l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{size_t} k = 0;" EOL;
    def += "  for (_{size_t} i = n->l - m.l; i < n->l; i++) n->d[i] = m.d[k++];" EOL;
    def += "  _{free}(m.d);" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_remove", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cFree = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d", true),
      CodegenASTExprAccess::create("i")
    );

    decl += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *, _{int32_t}, int, int);";
    def += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *self, _{int32_t} n1, int line, int col) {" EOL;
    def += R"(  if ((n1 >= 0 && n1 >= self->l) || (n1 < 0 && n1 < -((_{int32_t}) self->l))) {)" EOL;
    def += R"(    const char *fmt = "index %" _{PRId32} " out of array bounds";)" EOL;
    def += R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, n1);)" EOL;
    def += R"(    char *d = _{alloc}(z + 1);)" EOL;
    def += R"(    _{sprintf}(d, fmt, n1);)" EOL;
    def += R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL;
    def += R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL;
    def += R"(  })" EOL;
    def += R"(  _{size_t} i = n1 < 0 ? n1 + self->l : n1;)" EOL;

    if (elementTypeInfo.type->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(elementTypeInfo.type, cFree)->str() + ";" EOL;
    }

    def += "  if (i != self->l - 1) {" EOL;
    def += "    _{memmove}(&self->d[i], &self->d[i + 1], (--self->l - i) * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  } else {" EOL;
    def += "    self->l--;" EOL;
    def += "  }" EOL;
    def += "  return self;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_reverse", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "d"),
      CodegenASTExprBinary::create(
        CodegenASTExprBinary::create(
          CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "l"),
          "-",
          CodegenASTExprAccess::create("i")
        ),
        "-",
        CodegenASTExprLiteral::create("1")
      )
    );

    decl += "struct _{" + typeName + "} " + typeName + "_reverse (struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_reverse (struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.l == 0) {" EOL;
    def += "    " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "    return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  }" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(n.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) d[i] = " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, n.l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_slice", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("n"), "d"),
      CodegenASTExprAccess::create("i1")
    );

    decl += "struct _{" + typeName + "} " + typeName + "_slice (struct _{" + typeName + "}, ";
    decl += "unsigned int, _{int32_t}, unsigned int, _{int32_t});";
    def += "struct _{" + typeName + "} " + typeName + "_slice (struct _{" + typeName + "} n, ";
    def += "unsigned int o1, _{int32_t} n1, unsigned int o2, _{int32_t} n2) {" EOL;
    def += "  _{int32_t} i1 = o1 == 0 ? 0 : (_{int32_t}) (n1 < 0 ? (n1 < -((_{int32_t}) n.l) ? 0 : n1 + n.l) ";
    def += ": (n1 > n.l ? n.l : n1));" EOL;
    def += "  _{int32_t} i2 = o2 == 0 ? (_{int32_t}) n.l : (_{int32_t}) (n2 < 0 ? (n2 < -((_{int32_t}) n.l) ? 0 : n2 + n.l) ";
    def += ": (n2 > n.l ? n.l : n2));" EOL;
    def += "  if (i1 > i2 || i1 >= n.l) {" EOL;
    def += "    " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "    return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  }" EOL;
    def += "  _{size_t} l = i2 - i1;" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i1 < i2; i1++) d[i++] = " + this->_genCopyFn(elementTypeInfo.type, cCopy)->str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_sort", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "sort";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    auto cCopy1 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d", true),
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create("i"),
        "-",
        CodegenASTExprLiteral::create("1")
      )
    );

    auto cCopy2 = CodegenASTExprAccess::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create("self"), "d", true),
      CodegenASTExprAccess::create("i")
    );

    decl += "struct _{" + typeName + "} *" + typeName + "_sort (struct _{" + typeName + "} *, " + param1TypeInfo.typeCodeTrimmed + ", int, int);";
    def += "struct _{" + typeName + "} *" + typeName + "_sort (struct _{" + typeName + "} *self, " + param1TypeInfo.typeCode + "n1, int line, int col) {" EOL;
    def += "  if (self->l > 1) {" EOL;
    def += "    while (1) {" EOL;
    def += "      unsigned char b = 0;" EOL;
    def += "      for (_{size_t} i = 1; i < self->l; i++) {" EOL;
    def += "        _{int32_t} c = n1.f(n1.x, _{xalloc}(&(struct _{" + param1TypeInfo.typeName + "P}) {line, col, ";
    def += this->_genCopyFn(elementTypeInfo.type, cCopy1)->str() + ", ";
    def += this->_genCopyFn(elementTypeInfo.type, cCopy2)->str() + "}, sizeof(struct _{" + param1TypeInfo.typeName + "P})));" EOL;
    def += "        if (c > 0) {" EOL;
    def += "          b = 1;" EOL;
    def += "          " + elementTypeInfo.typeCode + "t = self->d[i];" EOL;
    def += "          self->d[i] = self->d[i - 1];" EOL;
    def += "          self->d[i - 1] = t;" EOL;
    def += "        }" EOL;
    def += "      }" EOL;
    def += "      if (b == 0) break;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(param1TypeInfo.type, CodegenASTExprAccess::create("n1"))->str() + ";" EOL;
    def += "  return self;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += R"(  _{struct str} r = _{str_alloc}("[");)" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += R"(    if (i != 0) r = _{str_concat_cstr}(r, ", ");)" EOL;

    if (elementTypeInfo.realType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(elementTypeInfo.realType, cElementRealType)->str() + ");" EOL;

    if (elementTypeInfo.realType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, CodegenASTExprAccess::create("n"))->str() + ";" EOL;
    def += R"(  return _{str_concat_cstr}(r, "]");)" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}
