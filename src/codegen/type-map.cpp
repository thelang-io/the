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

std::string Codegen::_typeNameMap (Type *type) {
  auto typeName = Codegen::typeName(type->name);
  auto pairTypeName = Codegen::typeName("pair" + type->name.substr(3));

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto mapType = std::get<TypeBodyMap>(type->body);

  this->_apiEntity(pairTypeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += "struct " + pairTypeName + ";";
    def += "struct " + pairTypeName + " {" EOL;
    def += "  " + keyTypeInfo.typeCode + "f;" EOL;
    def += "  " + valueTypeInfo.typeCode + "s;" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  struct _{" + pairTypeName + "} *d;" EOL;
    def += "  _{size_t} c;" EOL;
    def += "  _{size_t} l;" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);
    auto keyVaArgCode = keyTypeInfo.type->vaArgCode(keyTypeInfo.typeCodeTrimmed);
    auto valueVaArgCode = valueTypeInfo.type->vaArgCode(valueTypeInfo.typeCodeTrimmed);

    decl += "struct _{" + typeName + "} " + typeName + "_alloc (int, ...);";
    def += "struct _{" + typeName + "} " + typeName + "_alloc (int x, ...) {" EOL;
    def += "  struct _{" + pairTypeName + "} *d = _{alloc}(x * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  _{va_list} args;" EOL;
    def += "  _{va_start}(args, x);" EOL;
    def += "  for (int i = 0; i < x; i++) d[i] = (struct _{" + pairTypeName + "}) ";
    def += "{_{va_arg}(args, " + keyVaArgCode + "), _{va_arg}(args, " + valueVaArgCode + ")};" EOL;
    def += "  _{va_end}(args);" EOL;
    def += "  return (struct _{" + typeName + "}) {d, x, x};" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "    " + this->_genFreeFn(mapType.keyType, "n.d[i].f").str() + ";" EOL;
    }

    if (mapType.valueType->shouldBeFreed()) {
      def += "    " + this->_genFreeFn(mapType.valueType, "n.d[i].s").str() + ";" EOL;
    }

    def += "  }" EOL;
    def += "  _{free}(n.d);" EOL;
    def += "}";

    return 0;
  });

  auto capFnEntityIdx = this->_apiEntity(typeName + "_cap", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_cap (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_cap (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} c = n.c;" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += "  return c;" EOL;
    def += "}";

    return freeFnEntityIdx;
  });

  auto clearFnEntityIdx = this->_apiEntity(typeName + "_clear", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *);";
    def += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *n) {" EOL;
    def += "  " + this->_genFreeFn(type, "*n").str() + ";" EOL;
    def += "  n->d = _{NULL};" EOL;
    def += "  n->l = 0;" EOL;
    def += "  return n;" EOL;
    def += "}";

    return capFnEntityIdx + 1;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  struct _{" + pairTypeName + "} *d = _{alloc}(n.c * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) d[i] = (struct _{" + pairTypeName + "}) ";
    def += "{" + this->_genCopyFn(mapType.keyType, "n.d[i].f").str() + ", " + this->_genCopyFn(mapType.valueType, "n.d[i].s").str() + "};" EOL;
    def += "  return (struct _{" + typeName + "}) {d, n.c, n.l};" EOL;
    def += "}";

    return clearFnEntityIdx + 1;
  });

  auto emptyFnEntityIdx = this->_apiEntity(typeName + "_empty", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_empty (struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_empty (struct _{" + typeName + "} n) {" EOL;
    def += "  _{bool} r = n.l == 0;" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l == n2.l;" EOL;
    def += "  if (r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(mapType.keyType, "n1.d[i].f", "n2.d[i].f", true).str() + " || ";
    def += this->_genEqFn(mapType.valueType, "n1.d[i].s", "n2.d[i].s", true).str() + ") {" EOL;
    def += "        r = _{false};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1").str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2").str() + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return emptyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_get", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += valueTypeInfo.typeCode + typeName + "_get (struct _{" + typeName + "}, " + keyTypeInfo.typeCodeTrimmed + ", int, int);";
    def += valueTypeInfo.typeCode + typeName + "_get (struct _{" + typeName + "} n, " + keyTypeInfo.typeCode + "k, int line, int col) {" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n.d[i].f", "k").str() + ") {" EOL;
    def += "      " + valueTypeInfo.typeCode + "r = " + this->_genCopyFn(mapType.valueType, "n.d[i].s").str() + ";" EOL;
    def += "      " + this->_genFreeFn(type, "n").str() + ";" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "k").str() + ";" EOL;
    }

    def += R"(      return r;)" EOL;
    def += R"(    })" EOL;
    def += R"(  })" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(mapType.keyType, "k").str() + ";" EOL;
    }

    def += R"(  _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("failed to get map value"), (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL;
    def += R"(  _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL;
    def += R"(})";

    return 0;
  });

  this->_apiEntity(typeName + "_has", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);

    decl += "_{bool} " + typeName + "_has (struct _{" + typeName + "}, " + keyTypeInfo.typeCodeTrimmed + ");";
    def += "_{bool} " + typeName + "_has (struct _{" + typeName + "} n, " + keyTypeInfo.typeCode + "k) {" EOL;
    def += "  _{bool} r = _{false};" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n.d[i].f", "k").str() + ") {" EOL;
    def += "      r = _{true};" EOL;
    def += "      break;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(mapType.keyType, "k").str() + ";" EOL;
    }

    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_keys", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto keysTypeInfo = this->_typeInfo(this->ast->typeMap.createArr(mapType.keyType));

    decl += keysTypeInfo.typeCode + typeName + "_keys (struct _{" + typeName + "});";
    def += keysTypeInfo.typeCode + typeName + "_keys (struct _{" + typeName + "} n) {" EOL;
    def += "  " + keyTypeInfo.typeCode + "*r = _{alloc}(n.l * sizeof(" + keyTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) r[i] = " + this->_genCopyFn(mapType.keyType, "n.d[i].f").str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += "  return (" + keysTypeInfo.typeCodeTrimmed + ") {r, n.l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_len", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_len (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_len (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} l = n.l;" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += "  return l;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_merge", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *n, struct _{" + typeName + "} m) {" EOL;
    def += "  if (n->l + m.l > n->c) n->d = _{re_alloc}(n->d, (n->c = n->l + m.l) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  for (_{size_t} i = 0; i < m.l; i++) {" EOL;
    def += "    for (_{size_t} j = 0; j < n->l; j++) {" EOL;
    def += "      if (" + this->_genEqFn(mapType.keyType, "n->d[j].f", "m.d[i].f").str() + ") {" EOL;

    if (mapType.valueType->shouldBeFreed()) {
      def += "        " + this->_genFreeFn(mapType.valueType, "n->d[j].s").str() + ";" EOL;
    }

    def += "        n->d[j].s = " + this->_genCopyFn(mapType.valueType, "m.d[i].s").str() + ";" EOL;
    def += "        goto " + typeName + "_merge_next;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "    n->d[n->l++] = (struct _{" + pairTypeName + "}) ";
    def += "{" + this->_genCopyFn(mapType.keyType, "m.d[i].f").str() + ", " + this->_genCopyFn(mapType.valueType, "m.d[i].s").str() + "};" EOL;
    def += typeName + "_merge_next:;" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "m").str() + ";" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l != n2.l;" EOL;
    def += "  if (!r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(mapType.keyType, "n1.d[i].f", "n2.d[i].f", true).str() + " || ";
    def += this->_genEqFn(mapType.valueType, "n1.d[i].s", "n2.d[i].s", true).str() + ") {" EOL;
    def += "        r = _{true};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1").str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2").str() + ";" EOL;
    def += "  return r;" EOL;
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

  this->_apiEntity(typeName + "_remove", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);

    decl += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *, " + keyTypeInfo.typeCodeTrimmed + ", int, int);";
    def += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *n, " + keyTypeInfo.typeCode + "k, int line, int col) {" EOL;
    def += "  for (_{size_t} i = 0; i < n->l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n->d[i].f", "k").str() + ") {" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "k").str() + ";" EOL;
    }

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "n->d[i].f").str() + ";" EOL;
    }

    if (mapType.valueType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.valueType, "n->d[i].s").str() + ";" EOL;
    }

    def += "      _{memmove}(&n->d[i], &n->d[i + 1], (--n->l - i) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += R"(      return n;)" EOL;
    def += R"(    })" EOL;
    def += R"(  })" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(mapType.keyType, "k").str() + ";" EOL;
    }

    def += R"(  _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("failed to remove map value"), (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL;
    def += R"(  _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL;
    def += R"(})";

    return 0;
  });

  this->_apiEntity(typeName + "_reserve", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_reserve (struct _{" + typeName + "} *, _{int32_t});";
    def += "struct _{" + typeName + "} *" + typeName + "_reserve (struct _{" + typeName + "} *n, _{int32_t} c) {" EOL;
    def += "  if (c > n->c) n->d = _{re_alloc}(n->d, (n->c = c) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_set", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += "struct _{" + typeName + "} *" + typeName + "_set (struct _{" + typeName + "} *";
    decl += ", " + keyTypeInfo.typeCodeTrimmed + ", " + valueTypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} *" + typeName + "_set (struct _{" + typeName + "} *n";
    def += ", " + keyTypeInfo.typeCode + "k, " + valueTypeInfo.typeCode + "v) {" EOL;
    def += "  for (_{size_t} i = 0; i < n->l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n->d[i].f", "k").str() + ") {" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "k").str() + ";" EOL;
    }

    if (mapType.valueType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.valueType, "n->d[i].s").str() + ";" EOL;
    }

    def += "      n->d[i].s = v;" EOL;
    def += "      return n;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  if (n->l + 1 > n->c) n->d = _{re_alloc}(n->d, (n->c = n->l + 1) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  n->d[n->l++] = (struct _{" + pairTypeName + "}) {k, v};" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_shrink", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += "struct _{" + typeName + "} *" + typeName + "_shrink (struct _{" + typeName + "} *);";
    def += "struct _{" + typeName + "} *" + typeName + "_shrink (struct _{" + typeName + "} *n) {" EOL;
    def += "  if (n->l != n->c && n->l == 0) {" EOL;
    def += "    _{free}(n->d);" EOL;
    def += "    n->d = _{NULL};" EOL;
    def += "    n->c = 0;" EOL;
    def += "  } else if (n->l != n->c) {" EOL;
    def += "    n->d = _{re_alloc}(n->d, (n->c = n->l) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  }" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += R"(  _{struct str} r = _{str_alloc}("{");)" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += R"(    if (i != 0) r = _{str_concat_cstr}(r, ", ");)" EOL;

    if (mapType.keyType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(mapType.keyType, "n.d[i].f").str() + ");" EOL;

    if (mapType.keyType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\": ");)" EOL;
    } else {
      def += R"(    r = _{str_concat_cstr}(r, ": ");)" EOL;
    }

    if (mapType.valueType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(mapType.valueType, "n.d[i].s").str() + ");" EOL;

    if (mapType.valueType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += R"(  return _{str_concat_cstr}(r, "}");)" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_values", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);
    auto valuesTypeInfo = this->_typeInfo(this->ast->typeMap.createArr(mapType.valueType));

    decl += valuesTypeInfo.typeCode + typeName + "_values (struct _{" + typeName + "});";
    def += valuesTypeInfo.typeCode + typeName + "_values (struct _{" + typeName + "} n) {" EOL;
    def += "  " + valueTypeInfo.typeCode + "*r = _{alloc}(n.l * sizeof(" + valueTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) r[i] = " + this->_genCopyFn(mapType.valueType, "n.d[i].s").str() + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n").str() + ";" EOL;
    def += "  return (" + valuesTypeInfo.typeCodeTrimmed + ") {r, n.l};" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}
