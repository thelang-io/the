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

#include "TypeMap.hpp"
#include <algorithm>
#include <limits>
#include <utility>
#include "codegen-api.hpp"

std::string typeName (const std::string &name) {
  return "__THE_1_" + name;
}

void TypeMap::init () {
  this->stack.reserve(std::numeric_limits<short>::max());
  this->_anyType(TYPE_MAP_DECL);
  this->_boolType(TYPE_MAP_DECL);
  this->_byteType(TYPE_MAP_DECL);
  this->_charType(TYPE_MAP_DECL);
  this->_floatType(TYPE_MAP_DECL);
  this->_intType(TYPE_MAP_DECL);
  this->_strType(TYPE_MAP_DECL);
  this->_voidType(TYPE_MAP_DECL);
  this->_bufferModule(TYPE_MAP_DECL);
  this->_dateModule(TYPE_MAP_DECL);
  this->_errorModule(TYPE_MAP_DECL);
  this->_fsModule(TYPE_MAP_DECL);
  this->_globalsModule(TYPE_MAP_DECL);
  this->_mathModule(TYPE_MAP_DECL);
  this->_pathModule(TYPE_MAP_DECL);
  this->_processModule(TYPE_MAP_DECL);
  this->_randomModule(TYPE_MAP_DECL);
  this->_requestModule(TYPE_MAP_DECL);
  this->_threadModule(TYPE_MAP_DECL);
  this->_urlModule(TYPE_MAP_DECL);
  this->_utilsModule(TYPE_MAP_DECL);

  this->_anyType(TYPE_MAP_DEF);
  this->_boolType(TYPE_MAP_DEF);
  this->_byteType(TYPE_MAP_DEF);
  this->_charType(TYPE_MAP_DEF);
  this->_floatType(TYPE_MAP_DEF);
  this->_intType(TYPE_MAP_DEF);
  this->_strType(TYPE_MAP_DEF);
  this->_voidType(TYPE_MAP_DEF);
  this->_bufferModule(TYPE_MAP_DEF);
  this->_dateModule(TYPE_MAP_DEF);
  this->_errorModule(TYPE_MAP_DEF);
  this->_fsModule(TYPE_MAP_DEF);
  this->_globalsModule(TYPE_MAP_DEF);
  this->_mathModule(TYPE_MAP_DEF);
  this->_pathModule(TYPE_MAP_DEF);
  this->_processModule(TYPE_MAP_DEF);
  this->_randomModule(TYPE_MAP_DEF);
  this->_requestModule(TYPE_MAP_DEF);
  this->_threadModule(TYPE_MAP_DEF);
  this->_urlModule(TYPE_MAP_DEF);
  this->_utilsModule(TYPE_MAP_DEF);

  this->_fnIdx = 1000;
}

Type *TypeMap::createAlias (const std::string &name, Type *type) {
  auto newType = Type{name, this->name(name), TypeAlias{type}};
  this->_items.push_back(std::make_unique<Type>(newType));
  return this->_items.back().get();
}

Type *TypeMap::createArr (Type *elementType) {
  if (elementType->isAlias()) {
    // todo test
    return this->createArr(std::get<TypeAlias>(elementType->body).type);
  }

  auto newType = Type{"array_" + elementType->name, "@array_" + elementType->name, TypeArray{elementType}};

  for (const auto &item : this->_items) {
    if (
      item->isArray() &&
      std::get<TypeArray>(item->body).elementType->builtin == elementType->builtin &&
      item->matchStrict(&newType, true)
    ) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);
  auto refElementType = this->createRef(elementType);

  this->_arrTypeDef(selfType, refSelfType, elementType, refElementType);
  return selfType;
}

Type *TypeMap::createEnum (const std::string &name, const std::string &codeName, const std::vector<Type *> &members) {
  auto newType = Type{name, codeName, TypeEnum{members}};

  for (const auto &item : this->_items) {
    if (item->isEnum() && item->codeName == newType.codeName) {
      // todo test
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_enumTypeDef(selfType, refSelfType);
  return selfType;
}

Type *TypeMap::createEnumerator (const std::string &enumeratorName, const std::string &enumeratorCodeName) {
  auto newType = Type{enumeratorName, enumeratorCodeName, TypeEnumerator{}};

  for (const auto &item : this->_items) {
    if (item->isEnumerator() && item->codeName == newType.codeName) {
      // todo test
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  return this->_items.back().get();
}

Type *TypeMap::createFn (
  const std::vector<TypeFnParam> &params,
  Type *returnType,
  bool throws,
  bool async,
  const std::optional<TypeCallInfo> &callInfo
) {
  // todo test
  auto typeBody = TypeFn{returnType, params, throws, false, callInfo == std::nullopt ? TypeCallInfo{} : *callInfo, async};
  auto newType = Type{"", "", typeBody};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchStrict(&newType, true)) {
      // todo test
      return item.get();
    }
  }

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchStrict(&newType)) {
      newType.name = item->name;
      newType.codeName = item->codeName;
      break;
    }
  }

  if (newType.name.empty()) {
    newType.name = "fn$" + std::to_string(this->_fnIdx++);
    newType.codeName = "@" + newType.name;
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_fnTypeDef(selfType, refSelfType);
  return selfType;
}

Type *TypeMap::createMap (Type *keyType, Type *valueType) {
  auto actualKeyType = Type::actual(keyType);
  auto actualValueType = Type::actual(valueType);

  auto name = "map_" + actualKeyType->name + "MS" + actualValueType->name + "ME";
  auto codeName = "@map_" + actualKeyType->codeName + "MS" + actualValueType->codeName + "ME";
  auto newType = Type{name, codeName, TypeBodyMap{actualKeyType, actualValueType}};

  for (const auto &item : this->_items) {
    if (item->codeName == newType.codeName) {
      // todo test
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_mapTypeDef(selfType, refSelfType, actualKeyType, actualValueType);
  return selfType;
}

Type *TypeMap::createMethod (const std::vector<TypeFnParam> &params, Type *returnType, bool throws, bool async, TypeCallInfo callInfo) {
  // todo test
  auto typeBody = TypeFn{returnType, params, throws, true, std::move(callInfo), async};
  auto newType = Type{"", "", typeBody};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchStrict(&newType, true)) {
      // todo test
      return item.get();
    }
  }

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchStrict(&newType)) {
      // todo test
      newType.name = item->name;
      newType.codeName = item->codeName;
      break;
    }
  }

  if (newType.name.empty()) {
    newType.name = "fn$" + std::to_string(this->_fnIdx++);
    newType.codeName = "@" + newType.name;
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_fnTypeDef(selfType, refSelfType);
  return selfType;
}

Type *TypeMap::createObj (const std::string &name, const std::string &codeName, const std::vector<TypeField> &fields, bool builtin) {
  auto newType = Type{name, codeName, TypeObj{}, fields, builtin};

  for (const auto &item : this->_items) {
    if (item->codeName == newType.codeName) {
      // todo test
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_objTypeDef(selfType, refSelfType);
  return selfType;
}

Type *TypeMap::createOpt (Type *type) {
  if (type->isAlias()) {
    // todo test
    return this->createOpt(std::get<TypeAlias>(type->body).type);
  }

  auto newType = Type{"opt_" + type->name, "@opt_" + type->name, TypeOptional{type}};

  for (const auto &item : this->_items) {
    if (
      item->isOpt() &&
      std::get<TypeOptional>(item->body).type->builtin == type->builtin && // todo test
      item->matchStrict(&newType, true) // todo test
    ) {
      // todo test
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_optTypeDef(selfType, refSelfType);
  return selfType;
}

Type *TypeMap::createRef (Type *refType) {
  if (refType->isAlias()) {
    return this->createRef(std::get<TypeAlias>(refType->body).type);
  }

  auto newType = Type{"ref_" + refType->name, "@ref_" + refType->name, TypeRef{refType}};

  for (const auto &item : this->_items) {
    if (
      item->isRef() &&
      std::get<TypeRef>(item->body).refType->builtin == refType->builtin &&
      item->matchStrict(&newType, true)
    ) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  return this->_items.back().get();
}

Type *TypeMap::createUnion (const std::vector<Type *> &subTypes) {
  auto actualSubTypes = std::vector<Type *>{};
  auto name = std::string("union_");
  auto codeName = std::string("@union_");

  for (const auto &subType : subTypes) {
    actualSubTypes.push_back(Type::actual(subType));
    name += actualSubTypes.back()->name + "US";
    codeName += actualSubTypes.back()->codeName + "US";
  }

  name = name.substr(0, name.size() - 2) + "UE";
  codeName = codeName.substr(0, codeName.size() - 2) + "UE";
  auto newType = Type{name, codeName, TypeUnion{actualSubTypes}};

  for (const auto &item : this->_items) {
    if (item->codeName == newType.codeName) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_unionTypeDef(selfType, refSelfType);
  return selfType;
}

Type *TypeMap::get (const std::string &name) {
  if (name == "Self") {
    // todo test
    return this->self == std::nullopt ? nullptr : *this->self;
  }

  for (auto idx = this->_items.size() - 1;; idx--) {
    if (this->_items[idx]->name == name) {
      return this->_items[idx].get();
    } else if (idx == 0) {
      // todo test
      break;
    }
  }

  // todo test
  return nullptr;
}

// todo test
bool TypeMap::has (const std::string &name) {
  if (name == "Self") {
    return this->self != std::nullopt;
  }

  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
    return it->name == name;
  });
}

// todo test
bool TypeMap::isSelf (Type *type) {
  return this->self != std::nullopt && Type::real(type)->codeName == (*this->self)->codeName;
}

std::string TypeMap::name (const std::string &name) const {
  auto fullName = std::string();

  for (const auto &item : this->stack) {
    fullName += item + "SD";
  }

  fullName += name + "_";

  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto fullNameTest = fullName + std::to_string(idx);
    auto exists = false;

    for (const auto &item : this->_items) {
      if (item->codeName == fullNameTest) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      return fullNameTest;
    }
  }
}

// todo test
Type *TypeMap::unionAdd (Type *type, Type *subType) {
  auto subTypes = std::vector<Type *>{};

  if (type->isUnion()) {
    auto typeSubTypes = std::get<TypeUnion>(type->body).subTypes;
    subTypes.insert(subTypes.end(), typeSubTypes.begin(), typeSubTypes.end());
  } else {
    subTypes.push_back(type);
  }

  subTypes.push_back(subType);
  return this->createUnion(subTypes);
}

// todo test
Type *TypeMap::unionSub (const Type *type, const Type *exceptType) {
  auto subTypes = std::get<TypeUnion>(type->body).subTypes;
  auto newSubTypes = std::vector<Type *>{};

  for (const auto &subType : subTypes) {
    if (!subType->matchStrict(exceptType, true)) {
      newSubTypes.push_back(subType);
    }
  }

  if (newSubTypes.empty()) {
    return subTypes[0];
  } else if (newSubTypes.size() == 1) {
    return newSubTypes[0];
  }

  return this->createUnion(newSubTypes);
}

void TypeMap::_initType (
  const std::string &name,
  TypeMapPhase phase,
  const std::optional<std::function<void (Type *, Type *)>> &def
) {
  if (phase == TYPE_MAP_DECL) {
    this->_items.push_back(std::make_unique<Type>(Type{name, "@" + name, TypeObj{}, {}, true}));
  } else if (phase == TYPE_MAP_DEF && def != std::nullopt) {
    auto selfType = this->get(name);
    (*def)(selfType, this->createRef(selfType));
  }
}

void TypeMap::_anyType (TypeMapPhase phase) {
  this->_initType("any", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"any_str", true, "self_0", selfType, false, codegenAPI.at("any_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@any.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_boolType (TypeMapPhase phase) {
  this->_initType("bool", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"bool_str", true, "self_0", selfType, false, codegenAPI.at("bool_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@bool.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_byteType (TypeMapPhase phase) {
  this->_initType("byte", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"byte_str", true, "self_0", selfType, false, codegenAPI.at("byte_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@byte.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_charType (TypeMapPhase phase) {
  this->_initType("char", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto byteCallInfo = TypeCallInfo{"char_byte", true, "self_0", selfType, false, codegenAPI.at("char_byte").throws};
    selfType->fields.push_back(TypeField{"byte", this->get("byte"), false, true, byteCallInfo});
    auto isDigitCallInfo = TypeCallInfo{"char_isDigit", true, "self_0", selfType, false, codegenAPI.at("char_isDigit").throws};
    selfType->fields.push_back(TypeField{"isDigit", this->get("bool"), false, true, isDigitCallInfo});
    auto isLetterCallInfo = TypeCallInfo{"char_isLetter", true, "self_0", selfType, false, codegenAPI.at("char_isLetter").throws};
    selfType->fields.push_back(TypeField{"isLetter", this->get("bool"), false, true, isLetterCallInfo});
    auto isLetterOrDigitCallInfo = TypeCallInfo{"char_isLetterOrDigit", true, "self_0", selfType, false, codegenAPI.at("char_isLetterOrDigit").throws};
    selfType->fields.push_back(TypeField{"isLetterOrDigit", this->get("bool"), false, true, isLetterOrDigitCallInfo});
    auto isLowerCallInfo = TypeCallInfo{"char_isLower", true, "self_0", selfType, false, codegenAPI.at("char_isLower").throws};
    selfType->fields.push_back(TypeField{"isLower", this->get("bool"), false, true, isLowerCallInfo});
    auto isUpperCallInfo = TypeCallInfo{"char_isUpper", true, "self_0", selfType, false, codegenAPI.at("char_isUpper").throws};
    selfType->fields.push_back(TypeField{"isUpper", this->get("bool"), false, true, isUpperCallInfo});
    auto isWhitespaceCallInfo = TypeCallInfo{"char_isWhitespace", true, "self_0", selfType, false, codegenAPI.at("char_isWhitespace").throws};
    selfType->fields.push_back(TypeField{"isWhitespace", this->get("bool"), false, true, isWhitespaceCallInfo});
    auto lowerCallInfo = TypeCallInfo{"char_lower", true, "self_0", selfType, false, codegenAPI.at("char_lower").throws};
    selfType->fields.push_back(TypeField{"lower", this->get("char"), false, true, lowerCallInfo});
    auto upperCallInfo = TypeCallInfo{"char_upper", true, "self_0", selfType, false, codegenAPI.at("char_upper").throws};
    selfType->fields.push_back(TypeField{"upper", this->get("char"), false, true, upperCallInfo});
    auto repeatCallInfo = TypeCallInfo{"char_repeat", true, "self_0", selfType, false, codegenAPI.at("char_repeat").throws};
    auto repeatTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"times", this->get("int"), false, true, false}
    }, repeatCallInfo.throws, true, repeatCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".repeat", "@char.repeat", repeatTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"repeat", this->_items.back().get(), false, true});
    auto strCallInfo = TypeCallInfo{"char_str", true, "self_0", selfType, false, codegenAPI.at("char_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@char.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_floatType (TypeMapPhase phase) {
  this->_initType("float", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"float_str", true, "self_0", selfType, false, codegenAPI.at("float_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@float.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("f32", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"f32_str", true, "self_0", selfType, false, codegenAPI.at("f32_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@f32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("f64", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"f64_str", true, "self_0", selfType, false, codegenAPI.at("f64_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@f64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_intType (TypeMapPhase phase) {
  this->_initType("int", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"int_str", true, "self_0", selfType, false, codegenAPI.at("int_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@int.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i8", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"i8_str", true, "self_0", selfType, false, codegenAPI.at("i8_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i8.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i16", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"i16_str", true, "self_0", selfType, false, codegenAPI.at("i16_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i16.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i32", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"i32_str", true, "self_0", selfType, false, codegenAPI.at("i32_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i64", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"i64_str", true, "self_0", selfType, false, codegenAPI.at("i64_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u8", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"u8_str", true, "self_0", selfType, false, codegenAPI.at("u8_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u8.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u16", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"u16_str", true, "self_0", selfType, false, codegenAPI.at("u16_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u16.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u32", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"u32_str", true, "self_0", selfType, false, codegenAPI.at("u32_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u64", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"u64_str", true, "self_0", selfType, false, codegenAPI.at("u64_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_strType (TypeMapPhase phase) {
  this->_initType("str", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto emptyCallInfo = TypeCallInfo{"str_empty", true, "self_0", selfType, false, codegenAPI.at("str_empty").throws};
    selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, true, emptyCallInfo});
    auto lenCallInfo = TypeCallInfo{"str_len", true, "self_0", selfType, false, codegenAPI.at("str_len").throws};
    selfType->fields.push_back(TypeField{"len", this->get("int"), false, true, lenCallInfo});
    auto lowerCallInfo = TypeCallInfo{"str_lower", true, "self_0", selfType, false, codegenAPI.at("str_lower").throws};
    selfType->fields.push_back(TypeField{"lower", selfType, false, true, lowerCallInfo});
    auto lowerFirstCallInfo = TypeCallInfo{"str_lowerFirst", true, "self_0", selfType, false, codegenAPI.at("str_lowerFirst").throws};
    selfType->fields.push_back(TypeField{"lowerFirst", selfType, false, true, lowerFirstCallInfo});
    auto upperCallInfo = TypeCallInfo{"str_upper", true, "self_0", selfType, false, codegenAPI.at("str_upper").throws};
    selfType->fields.push_back(TypeField{"upper", selfType, false, true, upperCallInfo});
    auto upperFirstCallInfo = TypeCallInfo{"str_upperFirst", true, "self_0", selfType, false, codegenAPI.at("str_upperFirst").throws};
    selfType->fields.push_back(TypeField{"upperFirst", selfType, false, true, upperFirstCallInfo});
    auto containsCallInfo = TypeCallInfo{"str_contains", true, "self_0", selfType, false, codegenAPI.at("str_contains").throws};
    auto containsTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"search", selfType, false, true, false}
    }, containsCallInfo.throws, true, containsCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".contains", "@str.contains", containsTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"contains", this->_items.back().get(), false, true});
    auto findCallInfo = TypeCallInfo{"str_find", true, "self_0", selfType, false, codegenAPI.at("str_find").throws};
    auto findTypeFn = TypeFn{this->get("int"), {
      TypeFnParam{"search", selfType, false, true, false}
    }, findCallInfo.throws, true, findCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".find", "@str.find", findTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"find", this->_items.back().get(), false, true});
    auto linesCallInfo = TypeCallInfo{"str_lines", true, "self_0", selfType, false, codegenAPI.at("str_lines").throws};
    auto linesTypeFn = TypeFn{this->createArr(selfType), {
      TypeFnParam{"keepLineBreaks", this->get("bool"), false, false, false}
    }, linesCallInfo.throws, true, linesCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".lines", "@str.lines", linesTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"lines", this->_items.back().get(), false, true});
    auto replaceCallInfo = TypeCallInfo{"str_replace", true, "self_0", selfType, false, codegenAPI.at("str_replace").throws};
    auto replaceTypeFn = TypeFn{selfType, {
      TypeFnParam{"search", selfType, false, true, false},
      TypeFnParam{"replacement", selfType, false, true, false},
      TypeFnParam{"count", this->get("int"), false, false, false}
    }, replaceCallInfo.throws, true, replaceCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".replace", "@str.replace", replaceTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"replace", this->_items.back().get(), false, true});
    auto sliceCallInfo = TypeCallInfo{"str_slice", true, "self_0", selfType, false, codegenAPI.at("str_slice").throws};
    auto sliceTypeFn = TypeFn{selfType, {
      TypeFnParam{"start", this->get("int"), false, false, false},
      TypeFnParam{"end", this->get("int"), false, false, false}
    }, sliceCallInfo.throws, true, sliceCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".slice", "@str.slice", sliceTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true});
    auto splitCallInfo = TypeCallInfo{"str_split", true, "self_0", selfType, false, codegenAPI.at("str_split").throws};
    auto splitTypeFn = TypeFn{this->createArr(selfType), {
      TypeFnParam{"delimiter", this->get("str"), false, false, false}
    }, splitCallInfo.throws, true, splitCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".split", "@str.split", splitTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"split", this->_items.back().get(), false, true});
    auto toBufferCallInfo = TypeCallInfo{"str_toBuffer", true, "self_0", selfType, false, codegenAPI.at("str_toBuffer").throws};
    auto toBufferTypeFn = TypeFn{this->get("buffer_Buffer"), {}, toBufferCallInfo.throws, true, toBufferCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toBuffer", "@str.toBuffer", toBufferTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toBuffer", this->_items.back().get(), false, true});
    auto toFloatCallInfo = TypeCallInfo{"str_toFloat", true, "self_0", selfType, false, codegenAPI.at("str_toFloat").throws};
    auto toFloatTypeFn = TypeFn{this->get("float"), {}, toFloatCallInfo.throws, true, toFloatCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toFloat", "@str.toFloat", toFloatTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toFloat", this->_items.back().get(), false, true});
    auto toF32CallInfo = TypeCallInfo{"str_toF32", true, "self_0", selfType, false, codegenAPI.at("str_toF32").throws};
    auto toF32TypeFn = TypeFn{this->get("f32"), {}, toF32CallInfo.throws, true, toF32CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toF32", "@str.toF32", toF32TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toF32", this->_items.back().get(), false, true});
    auto toF64CallInfo = TypeCallInfo{"str_toF64", true, "self_0", selfType, false, codegenAPI.at("str_toF64").throws};
    auto toF64TypeFn = TypeFn{this->get("f64"), {}, toF64CallInfo.throws, true, toF64CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toF64", "@str.toF64", toF64TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toF64", this->_items.back().get(), false, true});
    auto toIntCallInfo = TypeCallInfo{"str_toInt", true, "self_0", selfType, false, codegenAPI.at("str_toInt").throws};
    auto toIntTypeFn = TypeFn{this->get("int"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toIntCallInfo.throws, true, toIntCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toInt", "@str.toInt", toIntTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toInt", this->_items.back().get(), false, true});
    auto toI8CallInfo = TypeCallInfo{"str_toI8", true, "self_0", selfType, false, codegenAPI.at("str_toI8").throws};
    auto toI8TypeFn = TypeFn{this->get("i8"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toI8CallInfo.throws, true, toI8CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI8", "@str.toI8", toI8TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI8", this->_items.back().get(), false, true});
    auto toI16CallInfo = TypeCallInfo{"str_toI16", true, "self_0", selfType, false, codegenAPI.at("str_toI16").throws};
    auto toI16TypeFn = TypeFn{this->get("i16"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toI16CallInfo.throws, true, toI16CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI16", "@str.toI16", toI16TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI16", this->_items.back().get(), false, true});
    auto toI32CallInfo = TypeCallInfo{"str_toI32", true, "self_0", selfType, false, codegenAPI.at("str_toI32").throws};
    auto toI32TypeFn = TypeFn{this->get("i32"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toI32CallInfo.throws, true, toI32CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI32", "@str.toI32", toI32TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI32", this->_items.back().get(), false, true});
    auto toI64CallInfo = TypeCallInfo{"str_toI64", true, "self_0", selfType, false, codegenAPI.at("str_toI64").throws};
    auto toI64TypeFn = TypeFn{this->get("i64"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toI64CallInfo.throws, true, toI64CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI64", "@str.toI64", toI64TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI64", this->_items.back().get(), false, true});
    auto toU8CallInfo = TypeCallInfo{"str_toU8", true, "self_0", selfType, false, codegenAPI.at("str_toU8").throws};
    auto toU8TypeFn = TypeFn{this->get("u8"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toU8CallInfo.throws, true, toU8CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU8", "@str.toU8", toU8TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU8", this->_items.back().get(), false, true});
    auto toU16CallInfo = TypeCallInfo{"str_toU16", true, "self_0", selfType, false, codegenAPI.at("str_toU16").throws};
    auto toU16TypeFn = TypeFn{this->get("u16"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toU16CallInfo.throws, true, toU16CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU16", "@str.toU16", toU16TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU16", this->_items.back().get(), false, true});
    auto toU32CallInfo = TypeCallInfo{"str_toU32", true, "self_0", selfType, false, codegenAPI.at("str_toU32").throws};
    auto toU32TypeFn = TypeFn{this->get("u32"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toU32CallInfo.throws, true, toU32CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU32", "@str.toU32", toU32TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU32", this->_items.back().get(), false, true});
    auto toU64CallInfo = TypeCallInfo{"str_toU64", true, "self_0", selfType, false, codegenAPI.at("str_toU64").throws};
    auto toU64TypeFn = TypeFn{this->get("u64"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, toU64CallInfo.throws, true, toU64CallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU64", "@str.toU64", toU64TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU64", this->_items.back().get(), false, true});
    auto trimCallInfo = TypeCallInfo{"str_trim", true, "self_0", selfType, false, codegenAPI.at("str_trim").throws};
    auto trimTypeFn = TypeFn{selfType, {}, trimCallInfo.throws, true, trimCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trim", "@str.trim", trimTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trim", this->_items.back().get(), false, true});
    auto trimEndCallInfo = TypeCallInfo{"str_trimEnd", true, "self_0", selfType, false, codegenAPI.at("str_trimEnd").throws};
    auto trimEndTypeFn = TypeFn{selfType, {}, trimEndCallInfo.throws, true, trimEndCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trimEnd", "@str.trimEnd", trimEndTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trimEnd", this->_items.back().get(), false, true});
    auto trimStartCallInfo = TypeCallInfo{"str_trimStart", true, "self_0", selfType, false, codegenAPI.at("str_trimStart").throws};
    auto trimStartTypeFn = TypeFn{selfType, {}, trimStartCallInfo.throws, true, trimStartCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trimStart", "@str.trimStart", trimStartTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trimStart", this->_items.back().get(), false, true});
  });
}

void TypeMap::_voidType (TypeMapPhase phase) {
  this->_initType("void", phase);
}

void TypeMap::_bufferModule (TypeMapPhase phase) {
  this->_initType("buffer_Buffer", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"buffer_to_str", true, "self_0", selfType, false, codegenAPI.at("buffer_to_str").throws};
    auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@buffer_Buffer.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_dateModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto date_nowCallInfo = TypeCallInfo{.codeName = "date_now", .throws = codegenAPI.at("date_now").throws};
    auto date_nowTypeFn = TypeFn{this->get("u64"), {}, date_nowCallInfo.throws, false, date_nowCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"date_now", "@date_now", date_nowTypeFn, {}, true}));
  }
}

void TypeMap::_errorModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    this->createObj("error_Error", this->name("error_Error"), {
      TypeField{"message", this->get("str"), false, false},
      TypeField{"stack", this->get("str"), false, false}
    }, true);
    auto error_NewErrorCallInfo = TypeCallInfo{.codeName = "new_error", .throws = codegenAPI.at("new_error").throws};
    auto error_NewErrorTypeFn = TypeFn{this->get("error_Error"), {
      TypeFnParam{"message", this->get("str"), false, true, false}
    }, error_NewErrorCallInfo.throws, false, error_NewErrorCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"error_NewError", "@error_NewError", error_NewErrorTypeFn, {}, true}));
  }
}

void TypeMap::_fsModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    this->createObj("fs_Stats", this->name("fs_Stats"), {
      TypeField{"dev", this->get("u64"), false, false},
      TypeField{"mode", this->get("u32"), false, false},
      TypeField{"nlink", this->get("i64"), false, false},
      TypeField{"ino", this->get("u64"), false, false},
      TypeField{"uid", this->get("i32"), false, false},
      TypeField{"gid", this->get("i32"), false, false},
      TypeField{"rdev", this->get("u64"), false, false},
      TypeField{"atime", this->get("i64"), false, false},
      TypeField{"atimeNs", this->get("i64"), false, false},
      TypeField{"mtime", this->get("i64"), false, false},
      TypeField{"mtimeNs", this->get("i64"), false, false},
      TypeField{"ctime", this->get("i64"), false, false},
      TypeField{"ctimeNs", this->get("i64"), false, false},
      TypeField{"btime", this->get("i64"), false, false},
      TypeField{"btimeNs", this->get("i64"), false, false},
      TypeField{"size", this->get("i64"), false, false},
      TypeField{"blocks", this->get("i64"), false, false},
      TypeField{"blockSize", this->get("i64"), false, false}
    }, true);
    auto fs_appendFileSyncCallInfo = TypeCallInfo{.codeName = "fs_appendFileSync", .throws = codegenAPI.at("fs_appendFileSync").throws};
    auto fs_appendFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, true, false}
    }, fs_appendFileSyncCallInfo.throws, false, fs_appendFileSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_appendFileSync", "@fs_appendFileSync", fs_appendFileSyncTypeFn, {}, true}));
    auto fs_chmodSyncCallInfo = TypeCallInfo{.codeName = "fs_chmodSync", .throws = codegenAPI.at("fs_chmodSync").throws};
    auto fs_chmodSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"mode", this->get("int"), false, true, false}
    }, fs_chmodSyncCallInfo.throws, false, fs_chmodSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_chmodSync", "@fs_chmodSync", fs_chmodSyncTypeFn, {}, true}));
    auto fs_chownSyncCallInfo = TypeCallInfo{.codeName = "fs_chownSync", .throws = codegenAPI.at("fs_chownSync").throws};
    auto fs_chownSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"uid", this->get("int"), false, true, false},
      TypeFnParam{"gid", this->get("int"), false, true, false}
    }, fs_chownSyncCallInfo.throws, false, fs_chownSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_chownSync", "@fs_chownSync", fs_chownSyncTypeFn, {}, true}));
    auto fs_copyDirectorySyncCallInfo = TypeCallInfo{.codeName = "fs_copyDirectorySync", .throws = codegenAPI.at("fs_copyDirectorySync").throws};
    auto fs_copyDirectorySyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"source", this->get("str"), false, true, false},
      TypeFnParam{"destination", this->get("str"), false, true, false}
    }, fs_copyDirectorySyncCallInfo.throws, false, fs_copyDirectorySyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_copyDirectorySync", "@fs_copyDirectorySync", fs_copyDirectorySyncTypeFn, {}, true}));
    auto fs_copyFileSyncCallInfo = TypeCallInfo{.codeName = "fs_copyFileSync", .throws = codegenAPI.at("fs_copyFileSync").throws};
    auto fs_copyFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"source", this->get("str"), false, true, false},
      TypeFnParam{"destination", this->get("str"), false, true, false}
    }, fs_copyFileSyncCallInfo.throws, false, fs_copyFileSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_copyFileSync", "@fs_copyFileSync", fs_copyFileSyncTypeFn, {}, true}));
    auto fs_existsSyncCallInfo = TypeCallInfo{.codeName = "fs_existsSync", .throws = codegenAPI.at("fs_existsSync").throws};
    auto fs_existsSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_existsSyncCallInfo.throws, false, fs_existsSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_existsSync", "@fs_existsSync", fs_existsSyncTypeFn, {}, true}));
    auto fs_isAbsoluteSyncCallInfo = TypeCallInfo{.codeName = "fs_isAbsoluteSync", .throws = codegenAPI.at("fs_isAbsoluteSync").throws};
    auto fs_isAbsoluteSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_isAbsoluteSyncCallInfo.throws, false, fs_isAbsoluteSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isAbsoluteSync", "@fs_isAbsoluteSync", fs_isAbsoluteSyncTypeFn, {}, true}));
    auto fs_isDirectorySyncCallInfo = TypeCallInfo{.codeName = "fs_isDirectorySync", .throws = codegenAPI.at("fs_isDirectorySync").throws};
    auto fs_isDirectorySyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_isDirectorySyncCallInfo.throws, false, fs_isDirectorySyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isDirectorySync", "@fs_isDirectorySync", fs_isDirectorySyncTypeFn, {}, true}));
    auto fs_isFileSyncCallInfo = TypeCallInfo{.codeName = "fs_isFileSync", .throws = codegenAPI.at("fs_isFileSync").throws};
    auto fs_isFileSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_isFileSyncCallInfo.throws, false, fs_isFileSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isFileSync", "@fs_isFileSync", fs_isFileSyncTypeFn, {}, true}));
    auto fs_isSymbolicLinkSyncCallInfo = TypeCallInfo{.codeName = "fs_isSymbolicLinkSync", .throws = codegenAPI.at("fs_isSymbolicLinkSync").throws};
    auto fs_isSymbolicLinkSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_isSymbolicLinkSyncCallInfo.throws, false, fs_isSymbolicLinkSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isSymbolicLinkSync", "@fs_isSymbolicLinkSync", fs_isSymbolicLinkSyncTypeFn, {}, true}));
    auto fs_linkSyncCallInfo = TypeCallInfo{.codeName = "fs_linkSync", .throws = codegenAPI.at("fs_linkSync").throws};
    auto fs_linkSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"sourcePath", this->get("str"), false, true, false},
      TypeFnParam{"linkPath", this->get("str"), false, true, false}
    }, fs_linkSyncCallInfo.throws, false, fs_linkSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_linkSync", "@fs_linkSync", fs_linkSyncTypeFn, {}, true}));
    auto fs_mkdirSyncCallInfo = TypeCallInfo{.codeName = "fs_mkdirSync", .throws = codegenAPI.at("fs_mkdirSync").throws};
    auto fs_mkdirSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_mkdirSyncCallInfo.throws, false, fs_mkdirSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_mkdirSync", "@fs_mkdirSync", fs_mkdirSyncTypeFn, {}, true}));
    auto fs_readFileSyncCallInfo = TypeCallInfo{.codeName = "fs_readFileSync", .throws = codegenAPI.at("fs_readFileSync").throws};
    auto fs_readFileSyncTypeFn = TypeFn{this->get("buffer_Buffer"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_readFileSyncCallInfo.throws, false, fs_readFileSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_readFileSync", "@fs_readFileSync", fs_readFileSyncTypeFn, {}, true}));
    auto fs_realpathSyncCallInfo = TypeCallInfo{.codeName = "fs_realpathSync", .throws = codegenAPI.at("fs_realpathSync").throws};
    auto fs_realpathSyncTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_realpathSyncCallInfo.throws, false, fs_realpathSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_realpathSync", "@fs_realpathSync", fs_realpathSyncTypeFn, {}, true}));
    auto fs_renameSyncCallInfo = TypeCallInfo{.codeName = "fs_renameSync", .throws = codegenAPI.at("fs_renameSync").throws};
    auto fs_renameSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"source", this->get("str"), false, true, false},
      TypeFnParam{"destination", this->get("str"), false, true, false}
    }, fs_renameSyncCallInfo.throws, false, fs_renameSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_renameSync", "@fs_renameSync", fs_renameSyncTypeFn, {}, true}));
    auto fs_rmSyncCallInfo = TypeCallInfo{.codeName = "fs_rmSync", .throws = codegenAPI.at("fs_rmSync").throws};
    auto fs_rmSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_rmSyncCallInfo.throws, false, fs_rmSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_rmSync", "@fs_rmSync", fs_rmSyncTypeFn, {}, true}));
    auto fs_rmdirSyncCallInfo = TypeCallInfo{.codeName = "fs_rmdirSync", .throws = codegenAPI.at("fs_rmdirSync").throws};
    auto fs_rmdirSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_rmdirSyncCallInfo.throws, false, fs_rmdirSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_rmdirSync", "@fs_rmdirSync", fs_rmdirSyncTypeFn, {}, true}));
    auto fs_scandirSyncCallInfo = TypeCallInfo{.codeName = "fs_scandirSync", .throws = codegenAPI.at("fs_scandirSync").throws};
    auto fs_scandirSyncTypeFn = TypeFn{this->createArr(this->get("str")), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_scandirSyncCallInfo.throws, false, fs_scandirSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_scandirSync", "@fs_scandirSync", fs_scandirSyncTypeFn, {}, true}));
    auto fs_statSyncCallInfo = TypeCallInfo{.codeName = "fs_statSync", .throws = codegenAPI.at("fs_statSync").throws};
    auto fs_statSyncTypeFn = TypeFn{this->get("fs_Stats"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_statSyncCallInfo.throws, false, fs_statSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_statSync", "@fs_statSync", fs_statSyncTypeFn, {}, true}));
    auto fs_unlinkSyncCallInfo = TypeCallInfo{.codeName = "fs_unlinkSync", .throws = codegenAPI.at("fs_unlinkSync").throws};
    auto fs_unlinkSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, fs_unlinkSyncCallInfo.throws, false, fs_unlinkSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_unlinkSync", "@fs_unlinkSync", fs_unlinkSyncTypeFn, {}, true}));
    auto fs_writeFileSyncCallInfo = TypeCallInfo{.codeName = "fs_writeFileSync", .throws = codegenAPI.at("fs_writeFileSync").throws};
    auto fs_writeFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, true, false}
    }, fs_writeFileSyncCallInfo.throws, false, fs_writeFileSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_writeFileSync", "@fs_writeFileSync", fs_writeFileSyncTypeFn, {}, true}));
  }
}

void TypeMap::_globalsModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto printCallInfo = TypeCallInfo{.codeName = "print", .throws = codegenAPI.at("print").throws};
    auto printTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"items", this->createArr(this->get("any")), false, false, true},
      TypeFnParam{"separator", this->get("str"), false, false, false},
      TypeFnParam{"terminator", this->get("str"), false, false, false},
      TypeFnParam{"to", this->get("str"), false, false, false}
    }, printCallInfo.throws, false, printCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"print", "@print", printTypeFn, {}, true}));
  }
}

void TypeMap::_mathModule ([[maybe_unused]] TypeMapPhase phase) {
}

void TypeMap::_pathModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto path_basenameCallInfo = TypeCallInfo{.codeName = "path_basename", .throws = codegenAPI.at("path_basename").throws};
    auto path_basenameTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, path_basenameCallInfo.throws, false, path_basenameCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"path_basename", "@path_basename", path_basenameTypeFn, {}, true}));
    auto path_dirnameCallInfo = TypeCallInfo{.codeName = "path_dirname", .throws = codegenAPI.at("path_dirname").throws};
    auto path_dirnameTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, path_dirnameCallInfo.throws, false, path_dirnameCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"path_dirname", "@path_dirname", path_dirnameTypeFn, {}, true}));
    auto path_tempDirectoryCallInfo = TypeCallInfo{.codeName = "path_tempDirectory", .throws = codegenAPI.at("path_tempDirectory").throws};
    auto path_tempDirectoryTypeFn = TypeFn{this->get("str"), {}, path_tempDirectoryCallInfo.throws, false, path_tempDirectoryCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"path_tempDirectory", "@path_tempDirectory", path_tempDirectoryTypeFn, {}, true}));
    auto path_tempFileCallInfo = TypeCallInfo{.codeName = "path_tempFile", .throws = codegenAPI.at("path_tempFile").throws};
    auto path_tempFileTypeFn = TypeFn{this->get("str"), {}, path_tempDirectoryCallInfo.throws, false, path_tempFileCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"path_tempFile", "@path_tempFile", path_tempFileTypeFn, {}, true}));
  }
}

void TypeMap::_processModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto process_cwdCallInfo = TypeCallInfo{.codeName = "process_cwd", .throws = codegenAPI.at("process_cwd").throws};
    auto process_cwdTypeFn = TypeFn{this->get("str"), {}, process_cwdCallInfo.throws, false, process_cwdCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"process_cwd", "@process_cwd", process_cwdTypeFn, {}, true}));
    auto process_exitCallInfo = TypeCallInfo{.codeName = "process_exit", .throws = codegenAPI.at("process_exit").throws};
    auto process_exitTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"status", this->get("int"), false, false, false}
    }, process_exitCallInfo.throws, false, process_exitCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"process_exit", "@process_exit", process_exitTypeFn, {}, true}));
    auto process_getgidCallInfo = TypeCallInfo{.codeName = "process_getgid", .throws = codegenAPI.at("process_getgid").throws};
    auto process_getgidTypeFn = TypeFn{this->get("int"), {}, process_getgidCallInfo.throws, false, process_getgidCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"process_getgid", "@process_getgid", process_getgidTypeFn, {}, true}));
    auto process_getuidCallInfo = TypeCallInfo{.codeName = "process_getuid", .throws = codegenAPI.at("process_getuid").throws};
    auto process_getuidTypeFn = TypeFn{this->get("int"), {}, process_getuidCallInfo.throws, false, process_getuidCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"process_getuid", "@process_getuid", process_getuidTypeFn, {}, true}));
    auto process_runSyncCallInfo = TypeCallInfo{.codeName = "process_runSync", .throws = codegenAPI.at("process_runSync").throws};
    auto process_runSyncTypeFn = TypeFn{this->get("buffer_Buffer"), {
      TypeFnParam{"command", this->get("str"), false, true, false}
    }, process_runSyncCallInfo.throws, false, process_runSyncCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"process_runSync", "@process_runSync", process_runSyncTypeFn, {}, true}));
  }
}

void TypeMap::_randomModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto random_randomFloatCallInfo = TypeCallInfo{.codeName = "random_randomFloat", .throws = codegenAPI.at("random_randomFloat").throws};
    auto random_randomFloatTypeFn = TypeFn{this->get("float"), {
      TypeFnParam{"min", this->get("float"), false, false, false},
      TypeFnParam{"max", this->get("float"), false, false, false}
    }, random_randomFloatCallInfo.throws, false, random_randomFloatCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"random_randomFloat", "@random_randomFloat", random_randomFloatTypeFn, {}, true}));
    auto random_randomIntCallInfo = TypeCallInfo{.codeName = "random_randomInt", .throws = codegenAPI.at("random_randomInt").throws};
    auto random_randomIntTypeFn = TypeFn{this->get("int"), {
      TypeFnParam{"min", this->get("int"), false, false, false},
      TypeFnParam{"max", this->get("int"), false, false, false}
    }, random_randomIntCallInfo.throws, false, random_randomIntCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"random_randomInt", "@random_randomInt", random_randomIntTypeFn, {}, true}));
    auto random_randomStrCallInfo = TypeCallInfo{.codeName = "random_randomStr", .throws = codegenAPI.at("random_randomStr").throws};
    auto random_randomStrTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"characters", this->get("str"), false, false, false},
      TypeFnParam{"len", this->get("int"), false, false, false}
    }, random_randomStrCallInfo.throws, false, random_randomStrCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"random_randomStr", "@random_randomStr", random_randomStrTypeFn, {}, true}));
  }
}

void TypeMap::_requestModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    this->createObj("request_Header", this->name("request_Header"), {
      TypeField{"name", this->get("str"), false, false},
      TypeField{"value", this->get("str"), false, false}
    }, true);
    this->createObj("request_Request", this->name("request_Request"), {}, true);
    this->createObj("request_Response", this->name("request_Response"), {
      TypeField{"data", this->get("buffer_Buffer"), false, false},
      TypeField{"status", this->get("int"), false, false},
      TypeField{"headers", this->createArr(this->get("request_Header")), false, false}
    }, true);
    this->_initType("request_Request", phase, [&] (auto selfType, auto refSelfType) {
      auto closeCallInfo = TypeCallInfo{"request_close", true, "self_0", refSelfType, true, codegenAPI.at("request_close").throws};
      auto closeTypeFn = TypeFn{this->get("void"), {}, closeCallInfo.throws, true, closeCallInfo, false};
      this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".close", "@request_Request.close", closeTypeFn, {}, true}));
      selfType->fields.push_back(TypeField{"close", this->_items.back().get(), false, true});
      auto readCallInfo = TypeCallInfo{"request_read", true, "self_0", refSelfType, true, codegenAPI.at("request_read").throws};
      auto readTypeFn = TypeFn{this->get("request_Response"), {}, readCallInfo.throws, true, readCallInfo, false};
      this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".read", "@request_Request.read", readTypeFn, {}, true}));
      selfType->fields.push_back(TypeField{"read", this->_items.back().get(), false, true});
    });
    auto request_openCallInfo = TypeCallInfo{.codeName = "request_open", .throws = codegenAPI.at("request_open").throws};
    auto request_openTypeFn = TypeFn{this->get("request_Request"), {
      TypeFnParam{"method", this->get("str"), false, true, false},
      TypeFnParam{"url", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, false, false},
      TypeFnParam{"headers", this->createArr(this->get("request_Header")), false, false, false}
    }, request_openCallInfo.throws, false, request_openCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"request_open", "@request_open", request_openTypeFn, {}, true}));
  }
}

void TypeMap::_threadModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto thread_idCallInfo = TypeCallInfo{.codeName = "thread_id", .throws = codegenAPI.at("thread_id").throws};
    auto thread_idTypeFn = TypeFn{this->get("int"), {}, thread_idCallInfo.throws, false, thread_idCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"thread_id", "@thread_id", thread_idTypeFn, {}, true}));
    auto thread_sleepCallInfo = TypeCallInfo{.codeName = "thread_sleep", .throws = codegenAPI.at("thread_sleep").throws};
    auto thread_sleepTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"milliseconds", this->get("int"), false, true, false}
    }, thread_sleepCallInfo.throws, false, thread_sleepCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"thread_sleep", "@thread_sleep", thread_sleepTypeFn, {}, true}));
  }
}

void TypeMap::_urlModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    this->createObj("url_URL", this->name("url_URL"), {
      TypeField{"origin", this->get("str"), false, false},
      TypeField{"protocol", this->get("str"), false, false},
      TypeField{"host", this->get("str"), false, false},
      TypeField{"hostname", this->get("str"), false, false},
      TypeField{"port", this->get("str"), false, false},
      TypeField{"path", this->get("str"), false, false},
      TypeField{"pathname", this->get("str"), false, false},
      TypeField{"search", this->get("str"), false, false},
      TypeField{"hash", this->get("str"), false, false}
    }, true);
    auto url_parseCallInfo = TypeCallInfo{.codeName = "url_parse", .throws = codegenAPI.at("url_parse").throws};
    auto url_parseTypeFn = TypeFn{this->get("url_URL"), {
      TypeFnParam{"url", this->get("str"), false, true, false}
    }, url_parseCallInfo.throws, false, url_parseCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"url_parse", "@url_parse", url_parseTypeFn, {}, true}));
  }
}

void TypeMap::_utilsModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto utils_swapCallInfo = TypeCallInfo{.codeName = "utils_swap", .throws = codegenAPI.at("utils_swap").throws};
    auto utils_swapTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"a", this->createRef(this->get("any")), true, true, false},
      TypeFnParam{"b", this->createRef(this->get("any")), true, true, false}
    }, utils_swapCallInfo.throws, false, utils_swapCallInfo, false};
    this->_items.push_back(std::make_unique<Type>(Type{"utils_swap", "@utils_swap", utils_swapTypeFn, {}, true}));
  }
}

void TypeMap::_arrTypeDef (Type *selfType, Type *refSelfType, Type *elementType, Type *refElementType) {
  auto emptyCallInfo = TypeCallInfo{typeName(selfType->name + "_empty"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, true, emptyCallInfo});
  auto firstCallInfo = TypeCallInfo{typeName(selfType->name + "_first"), true, "self_0", refSelfType, true, true};
  selfType->fields.push_back(TypeField{"first", refElementType, false, true, firstCallInfo});
  auto lastCallInfo = TypeCallInfo{typeName(selfType->name + "_last"), true, "self_0", refSelfType, true, true};
  selfType->fields.push_back(TypeField{"last", refElementType, false, true, lastCallInfo});
  auto lenCallInfo = TypeCallInfo{typeName(selfType->name + "_len"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"len", this->get("int"), false, true, lenCallInfo});
  auto clearCallInfo = TypeCallInfo{typeName(selfType->name + "_clear"), true, "self_0", refSelfType, true, false};
  auto clearTypeFn = TypeFn{refSelfType, {}, clearCallInfo.throws, true, clearCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".clear", "@array.clear", clearTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"clear", this->_items.back().get(), false, true});
  auto concatCallInfo = TypeCallInfo{typeName(selfType->name + "_concat"), true, "self_0", selfType, false, false};
  auto concatTypeFn = TypeFn{selfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, concatCallInfo.throws, true, concatCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".concat", "@array.concat", concatTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"concat", this->_items.back().get(), false, true});
  auto containsCallInfo = TypeCallInfo{typeName(selfType->name + "_contains"), true, "self_0", selfType, false, false};
  auto containsTypeFn = TypeFn{this->get("bool"), {
    TypeFnParam{"search", elementType, false, true, false}
  }, containsCallInfo.throws, true, containsCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".contains", "@array.contains", containsTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"contains", this->_items.back().get(), false, true});
  auto filterCallInfo = TypeCallInfo{typeName(selfType->name + "_filter"), true, "self_0", selfType, false, true};
  auto filterTypeFn = TypeFn{selfType, {
    TypeFnParam{"predicate", this->createFn({
      TypeFnParam{"it", elementType, false, true, false}
    }, this->get("bool"), false, false), false, true, false}
  }, filterCallInfo.throws, true, filterCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".filter", "@array.filter", filterTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"filter", this->_items.back().get(), false, true});
  auto forEachCallInfo = TypeCallInfo{typeName(selfType->name + "_forEach"), true, "self_0", selfType, false, true};
  auto forEachTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"iterator", this->createFn({
      TypeFnParam{"it", elementType, false, true, false},
      TypeFnParam{"idx", this->get("int"), false, true, false}
    }, this->get("void"), false, false), false, true, false}
  }, forEachCallInfo.throws, true, forEachCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".forEach", "@array.forEach", forEachTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"forEach", this->_items.back().get(), false, true});
  auto joinCallInfo = TypeCallInfo{typeName(selfType->name + "_join"), true, "self_0", selfType, false, false};
  auto joinTypeFn = TypeFn{this->get("str"), {
    TypeFnParam{"separator", this->get("str"), false, false, false}
  }, joinCallInfo.throws, true, joinCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".join", "@array.join", joinTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"join", this->_items.back().get(), false, true});
  auto mergeCallInfo = TypeCallInfo{typeName(selfType->name + "_merge"), true, "self_0", refSelfType, true, false};
  auto mergeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, mergeCallInfo.throws, true, mergeCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".merge", "@array.merge", mergeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"merge", this->_items.back().get(), false, true});
  auto popCallInfo = TypeCallInfo{typeName(selfType->name + "_pop"), true, "self_0", refSelfType, true, false};
  auto popTypeFn = TypeFn{elementType, {}, popCallInfo.throws, true, popCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".pop", "@array.pop", popTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"pop", this->_items.back().get(), false, true});
  auto pushCallInfo = TypeCallInfo{typeName(selfType->name + "_push"), true, "self_0", refSelfType, true, false};
  auto pushTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"elements", this->createArr(elementType), false, false, true}
  }, pushCallInfo.throws, true, pushCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".push", "@array.push", pushTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"push", this->_items.back().get(), false, true});
  auto removeCallInfo = TypeCallInfo{typeName(selfType->name + "_remove"), true, "self_0", refSelfType, true, true};
  auto removeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"index", this->get("int"), false, true, false}
  }, removeCallInfo.throws, true, removeCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".remove", "@array.remove", removeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"remove", this->_items.back().get(), false, true});
  auto reverseCallInfo = TypeCallInfo{typeName(selfType->name + "_reverse"), true, "self_0", selfType, false, false};
  auto reverseTypeFn = TypeFn{selfType, {}, reverseCallInfo.throws, true, reverseCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".reverse", "@array.reverse", reverseTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"reverse", this->_items.back().get(), false, true});
  auto sliceCallInfo = TypeCallInfo{typeName(selfType->name + "_slice"), true, "self_0", selfType, false, false};
  auto sliceTypeFn = TypeFn{selfType, {
    TypeFnParam{"start", this->get("int"), false, false, false},
    TypeFnParam{"end", this->get("int"), false, false, false}
  }, sliceCallInfo.throws, true, sliceCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".slice", "@array.slice", sliceTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true});
  auto sortCallInfo = TypeCallInfo{typeName(selfType->name + "_sort"), true, "self_0", refSelfType, true, true};
  auto sortTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"comparator", this->createFn({
      TypeFnParam{"a", elementType, false, true, false},
      TypeFnParam{"b", elementType, false, true, false}
    }, this->get("int"), false, false), false, true, false}
  }, sortCallInfo.throws, true, sortCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".sort", "@array.sort", sortTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"sort", this->_items.back().get(), false, true});
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@array.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_enumTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto rawValueCallInfo = TypeCallInfo{typeName(selfType->codeName + "_rawValue"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"rawValue", this->get("str"), false, true, rawValueCallInfo});
  auto strCallInfo = TypeCallInfo{"enum_str", true, "self_0", selfType, false, codegenAPI.at("enum_str").throws};
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->codeName + ".str", "@enum.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_fnTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@fn.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_mapTypeDef (Type *selfType, Type *refSelfType, Type *keyType, Type *valueType) {
  auto emptyCallInfo = TypeCallInfo{typeName(selfType->name + "_empty"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, true, emptyCallInfo});
  auto capCallInfo = TypeCallInfo{typeName(selfType->name + "_cap"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"cap", this->get("int"), false, true, capCallInfo});
  auto keysCallInfo = TypeCallInfo{typeName(selfType->name + "_keys"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"keys", this->createArr(keyType), false, true, keysCallInfo});
  auto lenCallInfo = TypeCallInfo{typeName(selfType->name + "_len"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"len", this->get("int"), false, true, lenCallInfo});
  auto valuesCallInfo = TypeCallInfo{typeName(selfType->name + "_values"), true, "self_0", selfType, false, false};
  selfType->fields.push_back(TypeField{"values", this->createArr(valueType), false, true, valuesCallInfo});
  auto clearCallInfo = TypeCallInfo{typeName(selfType->name + "_clear"), true, "self_0", refSelfType, true, false};
  auto clearTypeFn = TypeFn{refSelfType, {}, clearCallInfo.throws, true, clearCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".clear", "@map.clear", clearTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"clear", this->_items.back().get(), false, true});
  auto getCallInfo = TypeCallInfo{typeName(selfType->name + "_get"), true, "self_0", selfType, false, true};
  auto getTypeFn = TypeFn{valueType, {
    TypeFnParam{"key", keyType, false, true, false}
  }, getCallInfo.throws, true, getCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".get", "@map.get", getTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"get", this->_items.back().get(), false, true});
  auto hasCallInfo = TypeCallInfo{typeName(selfType->name + "_has"), true, "self_0", selfType, false, false};
  auto hasTypeFn = TypeFn{this->get("bool"), {
    TypeFnParam{"key", keyType, false, true, false}
  }, hasCallInfo.throws, true, hasCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".has", "@map.has", hasTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"has", this->_items.back().get(), false, true});
  auto mergeCallInfo = TypeCallInfo{typeName(selfType->name + "_merge"), true, "self_0", refSelfType, true, false};
  auto mergeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, mergeCallInfo.throws, true, mergeCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".merge", "@map.merge", mergeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"merge", this->_items.back().get(), false, true});
  auto removeCallInfo = TypeCallInfo{typeName(selfType->name + "_remove"), true, "self_0", refSelfType, true, true};
  auto removeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"key", keyType, false, true, false}
  }, removeCallInfo.throws, true, removeCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".remove", "@map.remove", removeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"remove", this->_items.back().get(), false, true});
  auto reserveCallInfo = TypeCallInfo{typeName(selfType->name + "_reserve"), true, "self_0", refSelfType, true, false};
  auto reserveTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"size", this->get("int"), false, true, false}
  }, reserveCallInfo.throws, true, reserveCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".reserve", "@map.reserve", reserveTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"reserve", this->_items.back().get(), false, true});
  auto setCallInfo = TypeCallInfo{typeName(selfType->name + "_set"), true, "self_0", refSelfType, true, false};
  auto setTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"key", keyType, false, true, false},
    TypeFnParam{"value", valueType, false, true, false}
  }, setCallInfo.throws, true, setCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".set", "@map.set", setTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"set", this->_items.back().get(), false, true});
  auto shrinkCallInfo = TypeCallInfo{typeName(selfType->name + "_shrink"), true, "self_0", refSelfType, true, false};
  auto shrinkTypeFn = TypeFn{refSelfType, {}, shrinkCallInfo.throws, true, shrinkCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".shrink", "@map.shrink", shrinkTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"shrink", this->_items.back().get(), false, true});
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@map.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_objTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{
    selfType->builtin ? (selfType->name + "_str") : typeName(selfType->codeName + "_str"),
    true,
    "self_0",
    selfType,
    false,
    false
  };
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@obj.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_optTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@opt.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_unionTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, strCallInfo.throws, true, strCallInfo, false};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@union.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}
