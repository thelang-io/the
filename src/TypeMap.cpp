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
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  return this->_items.back().get();
}

Type *TypeMap::createFn (
  const std::vector<TypeFnParam> &params,
  Type *returnType,
  const std::optional<TypeCallInfo> &callInfo
) {
  auto typeBody = TypeFn{returnType, params, false, callInfo == std::nullopt ? TypeCallInfo{} : *callInfo};
  auto newType = Type{"", "", typeBody};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchStrict(&newType, true)) {
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
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  this->_mapTypeDef(selfType, refSelfType, actualKeyType, actualValueType);
  return selfType;
}

Type *TypeMap::createMethod (
  const std::vector<TypeFnParam> &params,
  Type *returnType,
  const std::optional<TypeCallInfo> &callInfo
) {
  auto typeBody = TypeFn{returnType, params, true, callInfo == std::nullopt ? TypeCallInfo{} : *callInfo};
  auto newType = Type{"", "", typeBody};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchStrict(&newType, true)) {
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

Type *TypeMap::createObj (const std::string &name, const std::string &codeName, const std::vector<TypeField> &fields, bool builtin) {
  auto newType = Type{name, codeName, TypeObj{}, fields, builtin};

  for (const auto &item : this->_items) {
    if (item->codeName == newType.codeName) {
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
    return this->createOpt(std::get<TypeAlias>(type->body).type);
  }

  auto newType = Type{"opt_" + type->name, "@opt_" + type->name, TypeOptional{type}};

  for (const auto &item : this->_items) {
    if (
      item->isOpt() &&
      std::get<TypeOptional>(item->body).type->builtin == type->builtin &&
      item->matchStrict(&newType, true)
    ) {
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
    return this->self == std::nullopt ? nullptr : *this->self;
  }

  for (auto idx = this->_items.size() - 1;; idx--) {
    if (this->_items[idx]->name == name) {
      return this->_items[idx].get();
    } else if (idx == 0) {
      break;
    }
  }

  return nullptr;
}

bool TypeMap::has (const std::string &name) {
  if (name == "Self") {
    return this->self != std::nullopt;
  }

  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
    return it->name == name;
  });
}

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
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@any.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_boolType (TypeMapPhase phase) {
  this->_initType("bool", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@bool.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_byteType (TypeMapPhase phase) {
  this->_initType("byte", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@byte.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_charType (TypeMapPhase phase) {
  this->_initType("char", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto byteCallInfo = TypeCallInfo{selfType->name + "_byte", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"byte", this->get("byte"), false, true, byteCallInfo});
    auto isDigitCallInfo = TypeCallInfo{selfType->name + "_isDigit", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"isDigit", this->get("bool"), false, true, isDigitCallInfo});
    auto isLetterCallInfo = TypeCallInfo{selfType->name + "_isLetter", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"isLetter", this->get("bool"), false, true, isLetterCallInfo});
    auto isLetterOrDigitCallInfo = TypeCallInfo{selfType->name + "_isLetterOrDigit", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"isLetterOrDigit", this->get("bool"), false, true, isLetterOrDigitCallInfo});
    auto isLowerCallInfo = TypeCallInfo{selfType->name + "_isLower", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"isLower", this->get("bool"), false, true, isLowerCallInfo});
    auto isUpperCallInfo = TypeCallInfo{selfType->name + "_isUpper", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"isUpper", this->get("bool"), false, true, isUpperCallInfo});
    auto isWhitespaceCallInfo = TypeCallInfo{selfType->name + "_isWhitespace", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"isWhitespace", this->get("bool"), false, true, isWhitespaceCallInfo});
    auto lowerCallInfo = TypeCallInfo{selfType->name + "_lower", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"lower", this->get("char"), false, true, lowerCallInfo});
    auto upperCallInfo = TypeCallInfo{selfType->name + "_upper", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"upper", this->get("char"), false, true, upperCallInfo});
    auto repeatCallInfo = TypeCallInfo{selfType->name + "_repeat", true, "self_0", selfType, false};
    auto repeatTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"times", this->get("int"), false, true, false}
    }, true, repeatCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".repeat", "@char.repeat", repeatTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"repeat", this->_items.back().get(), false, true});
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@char.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_floatType (TypeMapPhase phase) {
  this->_initType("float", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@float.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("f32", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@f32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("f64", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@f64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_intType (TypeMapPhase phase) {
  this->_initType("int", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@int.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i8", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i8.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i16", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i16.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i32", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("i64", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u8", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u8.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u16", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u16.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u32", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
  this->_initType("u64", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{selfType->name + "_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_strType (TypeMapPhase phase) {
  this->_initType("str", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto emptyCallInfo = TypeCallInfo{selfType->name + "_empty", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, true, emptyCallInfo});
    auto lenCallInfo = TypeCallInfo{selfType->name + "_len", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"len", this->get("int"), false, true, lenCallInfo});
    auto lowerCallInfo = TypeCallInfo{selfType->name + "_lower", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"lower", selfType, false, true, lowerCallInfo});
    auto lowerFirstCallInfo = TypeCallInfo{selfType->name + "_lowerFirst", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"lowerFirst", selfType, false, true, lowerFirstCallInfo});
    auto upperCallInfo = TypeCallInfo{selfType->name + "_upper", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"upper", selfType, false, true, upperCallInfo});
    auto upperFirstCallInfo = TypeCallInfo{selfType->name + "_upperFirst", true, "self_0", selfType, false};
    selfType->fields.push_back(TypeField{"upperFirst", selfType, false, true, upperFirstCallInfo});
    auto containsCallInfo = TypeCallInfo{selfType->name + "_contains", true, "self_0", selfType, false};
    auto containsTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"search", selfType, false, true, false}
    }, true, containsCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".contains", "@str.contains", containsTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"contains", this->_items.back().get(), false, true});
    auto findCallInfo = TypeCallInfo{selfType->name + "_find", true, "self_0", selfType, false};
    auto findTypeFn = TypeFn{this->get("int"), {
      TypeFnParam{"search", selfType, false, true, false}
    }, true, findCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".find", "@str.find", findTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"find", this->_items.back().get(), false, true});
    auto linesCallInfo = TypeCallInfo{selfType->name + "_lines", true, "self_0", selfType, false};
    auto linesTypeFn = TypeFn{this->createArr(selfType), {
      TypeFnParam{"keepLineBreaks", this->get("bool"), false, false, false}
    }, true, linesCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".lines", "@str.lines", linesTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"lines", this->_items.back().get(), false, true});
    auto replaceCallInfo = TypeCallInfo{selfType->name + "_replace", true, "self_0", selfType, false};
    auto replaceTypeFn = TypeFn{selfType, {
      TypeFnParam{"search", selfType, false, true, false},
      TypeFnParam{"replacement", selfType, false, true, false},
      TypeFnParam{"count", this->get("int"), false, false, false}
    }, true, replaceCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".replace", "@str.replace", replaceTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"replace", this->_items.back().get(), false, true});
    auto sliceCallInfo = TypeCallInfo{selfType->name + "_slice", true, "self_0", selfType, false};
    auto sliceTypeFn = TypeFn{selfType, {
      TypeFnParam{"start", this->get("int"), false, false, false},
      TypeFnParam{"end", this->get("int"), false, false, false}
    }, true, sliceCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".slice", "@str.slice", sliceTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true});
    auto splitCallInfo = TypeCallInfo{selfType->name + "_split", true, "self_0", selfType, false};
    auto splitTypeFn = TypeFn{this->createArr(selfType), {
      TypeFnParam{"delimiter", this->get("str"), false, false, false}
    }, true, splitCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".split", "@str.split", splitTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"split", this->_items.back().get(), false, true});
    auto toBufferCallInfo = TypeCallInfo{selfType->name + "_toBuffer", true, "self_0", selfType, false};
    auto toBufferTypeFn = TypeFn{this->get("buffer_Buffer"), {}, true, toBufferCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toBuffer", "@str.toBuffer", toBufferTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toBuffer", this->_items.back().get(), false, true});
    auto toFloatCallInfo = TypeCallInfo{selfType->name + "_toFloat", true, "self_0", selfType, false};
    auto toFloatTypeFn = TypeFn{this->get("float"), {}, true, toFloatCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toFloat", "@str.toFloat", toFloatTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toFloat", this->_items.back().get(), false, true});
    auto toF32CallInfo = TypeCallInfo{selfType->name + "_toF32", true, "self_0", selfType, false};
    auto toF32TypeFn = TypeFn{this->get("f32"), {}, true, toF32CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toF32", "@str.toF32", toF32TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toF32", this->_items.back().get(), false, true});
    auto toF64CallInfo = TypeCallInfo{selfType->name + "_toF64", true, "self_0", selfType, false};
    auto toF64TypeFn = TypeFn{this->get("f64"), {}, true, toF64CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toF64", "@str.toF64", toF64TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toF64", this->_items.back().get(), false, true});
    auto toIntCallInfo = TypeCallInfo{selfType->name + "_toInt", true, "self_0", selfType, false};
    auto toIntTypeFn = TypeFn{this->get("int"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toIntCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toInt", "@str.toInt", toIntTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toInt", this->_items.back().get(), false, true});
    auto toI8CallInfo = TypeCallInfo{selfType->name + "_toI8", true, "self_0", selfType, false};
    auto toI8TypeFn = TypeFn{this->get("i8"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toI8CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI8", "@str.toI8", toI8TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI8", this->_items.back().get(), false, true});
    auto toI16CallInfo = TypeCallInfo{selfType->name + "_toI16", true, "self_0", selfType, false};
    auto toI16TypeFn = TypeFn{this->get("i16"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toI16CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI16", "@str.toI16", toI16TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI16", this->_items.back().get(), false, true});
    auto toI32CallInfo = TypeCallInfo{selfType->name + "_toI32", true, "self_0", selfType, false};
    auto toI32TypeFn = TypeFn{this->get("i32"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toI32CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI32", "@str.toI32", toI32TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI32", this->_items.back().get(), false, true});
    auto toI64CallInfo = TypeCallInfo{selfType->name + "_toI64", true, "self_0", selfType, false};
    auto toI64TypeFn = TypeFn{this->get("i64"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toI64CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toI64", "@str.toI64", toI64TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toI64", this->_items.back().get(), false, true});
    auto toU8CallInfo = TypeCallInfo{selfType->name + "_toU8", true, "self_0", selfType, false};
    auto toU8TypeFn = TypeFn{this->get("u8"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toU8CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU8", "@str.toU8", toU8TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU8", this->_items.back().get(), false, true});
    auto toU16CallInfo = TypeCallInfo{selfType->name + "_toU16", true, "self_0", selfType, false};
    auto toU16TypeFn = TypeFn{this->get("u16"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toU16CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU16", "@str.toU16", toU16TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU16", this->_items.back().get(), false, true});
    auto toU32CallInfo = TypeCallInfo{selfType->name + "_toU32", true, "self_0", selfType, false};
    auto toU32TypeFn = TypeFn{this->get("u32"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toU32CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU32", "@str.toU32", toU32TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU32", this->_items.back().get(), false, true});
    auto toU64CallInfo = TypeCallInfo{selfType->name + "_toU64", true, "self_0", selfType, false};
    auto toU64TypeFn = TypeFn{this->get("u64"), {
      TypeFnParam{"radix", this->get("int"), false, false, false}
    }, true, toU64CallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toU64", "@str.toU64", toU64TypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toU64", this->_items.back().get(), false, true});
    auto trimCallInfo = TypeCallInfo{selfType->name + "_trim", true, "self_0", selfType, false};
    auto trimTypeFn = TypeFn{selfType, {}, true, trimCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trim", "@str.trim", trimTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trim", this->_items.back().get(), false, true});
    auto trimEndCallInfo = TypeCallInfo{selfType->name + "_trimEnd", true, "self_0", selfType, false};
    auto trimEndTypeFn = TypeFn{selfType, {}, true, trimEndCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trimEnd", "@str.trimEnd", trimEndTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trimEnd", this->_items.back().get(), false, true});
    auto trimStartCallInfo = TypeCallInfo{selfType->name + "_trimStart", true, "self_0", selfType, false};
    auto trimStartTypeFn = TypeFn{selfType, {}, true, trimStartCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trimStart", "@str.trimStart", trimStartTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trimStart", this->_items.back().get(), false, true});
  });
}

void TypeMap::_voidType (TypeMapPhase phase) {
  this->_initType("void", phase);
}

void TypeMap::_bufferModule (TypeMapPhase phase) {
  this->_initType("buffer_Buffer", phase, [&] (auto selfType, [[maybe_unused]] auto refSelfType) {
    auto strCallInfo = TypeCallInfo{"buffer_to_str", true, "self_0", selfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@buffer_Buffer.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  });
}

void TypeMap::_dateModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto date_nowCallInfo = TypeCallInfo{"date_now"};
    auto date_nowTypeFn = TypeFn{this->get("u64"), {}, false, date_nowCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"date_now", "@date_now", date_nowTypeFn, {}, true}));
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
    auto fs_appendFileSyncCallInfo = TypeCallInfo{"fs_appendFileSync"};
    auto fs_appendFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, true, false}
    }, false, fs_appendFileSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_appendFileSync", "@fs_appendFileSync", fs_appendFileSyncTypeFn, {}, true}));
    auto fs_chmodSyncCallInfo = TypeCallInfo{"fs_chmodSync"};
    auto fs_chmodSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"mode", this->get("int"), false, true, false}
    }, false, fs_chmodSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_chmodSync", "@fs_chmodSync", fs_chmodSyncTypeFn, {}, true}));
    auto fs_chownSyncCallInfo = TypeCallInfo{"fs_chownSync"};
    auto fs_chownSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"uid", this->get("int"), false, true, false},
      TypeFnParam{"gid", this->get("int"), false, true, false}
    }, false, fs_chownSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_chownSync", "@fs_chownSync", fs_chownSyncTypeFn, {}, true}));
    auto fs_copyDirectorySyncCallInfo = TypeCallInfo{"fs_copyDirectorySync"};
    auto fs_copyDirectorySyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"source", this->get("str"), false, true, false},
      TypeFnParam{"destination", this->get("str"), false, true, false}
    }, false, fs_copyDirectorySyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_copyDirectorySync", "@fs_copyDirectorySync", fs_copyDirectorySyncTypeFn, {}, true}));
    auto fs_copyFileSyncCallInfo = TypeCallInfo{"fs_copyFileSync"};
    auto fs_copyFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"source", this->get("str"), false, true, false},
      TypeFnParam{"destination", this->get("str"), false, true, false}
    }, false, fs_copyFileSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_copyFileSync", "@fs_copyFileSync", fs_copyFileSyncTypeFn, {}, true}));
    auto fs_existsSyncCallInfo = TypeCallInfo{"fs_existsSync"};
    auto fs_existsSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_existsSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_existsSync", "@fs_existsSync", fs_existsSyncTypeFn, {}, true}));
    auto fs_isAbsoluteSyncCallInfo = TypeCallInfo{"fs_isAbsoluteSync"};
    auto fs_isAbsoluteSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_isAbsoluteSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isAbsoluteSync", "@fs_isAbsoluteSync", fs_isAbsoluteSyncTypeFn, {}, true}));
    auto fs_isDirectorySyncCallInfo = TypeCallInfo{"fs_isDirectorySync"};
    auto fs_isDirectorySyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_isDirectorySyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isDirectorySync", "@fs_isDirectorySync", fs_isDirectorySyncTypeFn, {}, true}));
    auto fs_isFileSyncCallInfo = TypeCallInfo{"fs_isFileSync"};
    auto fs_isFileSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_isFileSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isFileSync", "@fs_isFileSync", fs_isFileSyncTypeFn, {}, true}));
    auto fs_isSymbolicLinkSyncCallInfo = TypeCallInfo{"fs_isSymbolicLinkSync"};
    auto fs_isSymbolicLinkSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_isSymbolicLinkSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isSymbolicLinkSync", "@fs_isSymbolicLinkSync", fs_isSymbolicLinkSyncTypeFn, {}, true}));
    auto fs_linkSyncCallInfo = TypeCallInfo{"fs_linkSync"};
    auto fs_linkSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"sourcePath", this->get("str"), false, true, false},
      TypeFnParam{"linkPath", this->get("str"), false, true, false}
    }, false, fs_linkSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_linkSync", "@fs_linkSync", fs_linkSyncTypeFn, {}, true}));
    auto fs_mkdirSyncCallInfo = TypeCallInfo{"fs_mkdirSync"};
    auto fs_mkdirSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_mkdirSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_mkdirSync", "@fs_mkdirSync", fs_mkdirSyncTypeFn, {}, true}));
    auto fs_readFileSyncCallInfo = TypeCallInfo{"fs_readFileSync"};
    auto fs_readFileSyncTypeFn = TypeFn{this->get("buffer_Buffer"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_readFileSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_readFileSync", "@fs_readFileSync", fs_readFileSyncTypeFn, {}, true}));
    auto fs_realpathSyncCallInfo = TypeCallInfo{"fs_realpathSync"};
    auto fs_realpathSyncTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_realpathSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_realpathSync", "@fs_realpathSync", fs_realpathSyncTypeFn, {}, true}));
    auto fs_renameSyncCallInfo = TypeCallInfo{"fs_renameSync"};
    auto fs_renameSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"source", this->get("str"), false, true, false},
      TypeFnParam{"destination", this->get("str"), false, true, false}
    }, false, fs_renameSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_renameSync", "@fs_renameSync", fs_renameSyncTypeFn, {}, true}));
    auto fs_rmSyncCallInfo = TypeCallInfo{"fs_rmSync"};
    auto fs_rmSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_rmSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_rmSync", "@fs_rmSync", fs_rmSyncTypeFn, {}, true}));
    auto fs_rmdirSyncCallInfo = TypeCallInfo{"fs_rmdirSync"};
    auto fs_rmdirSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_rmdirSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_rmdirSync", "@fs_rmdirSync", fs_rmdirSyncTypeFn, {}, true}));
    auto fs_scandirSyncCallInfo = TypeCallInfo{"fs_scandirSync"};
    auto fs_scandirSyncTypeFn = TypeFn{this->createArr(this->get("str")), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_scandirSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_scandirSync", "@fs_scandirSync", fs_scandirSyncTypeFn, {}, true}));
    auto fs_statSyncCallInfo = TypeCallInfo{"fs_statSync"};
    auto fs_statSyncTypeFn = TypeFn{this->get("fs_Stats"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_statSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_statSync", "@fs_statSync", fs_statSyncTypeFn, {}, true}));
    auto fs_unlinkSyncCallInfo = TypeCallInfo{"fs_unlinkSync"};
    auto fs_unlinkSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, fs_unlinkSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_unlinkSync", "@fs_unlinkSync", fs_unlinkSyncTypeFn, {}, true}));
    auto fs_writeFileSyncCallInfo = TypeCallInfo{"fs_writeFileSync"};
    auto fs_writeFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, true, false}
    }, false, fs_writeFileSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_writeFileSync", "@fs_writeFileSync", fs_writeFileSyncTypeFn, {}, true}));
  }
}

void TypeMap::_globalsModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto printTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"items", this->createArr(this->get("any")), false, false, true},
      TypeFnParam{"separator", this->get("str"), false, false, false},
      TypeFnParam{"terminator", this->get("str"), false, false, false},
      TypeFnParam{"to", this->get("str"), false, false, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"print", "@print", printTypeFn, {}, true}));
  }
}

void TypeMap::_mathModule ([[maybe_unused]] TypeMapPhase phase) {
}

void TypeMap::_pathModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto path_basenameCallInfo = TypeCallInfo{"path_basename"};
    auto path_basenameTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, path_basenameCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"path_basename", "@path_basename", path_basenameTypeFn, {}, true}));
    auto path_dirnameCallInfo = TypeCallInfo{"path_dirname"};
    auto path_dirnameTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }, false, path_dirnameCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"path_dirname", "@path_dirname", path_dirnameTypeFn, {}, true}));
    auto path_tempDirectoryCallInfo = TypeCallInfo{"path_tempDirectory"};
    auto path_tempDirectoryTypeFn = TypeFn{this->get("str"), {}, false, path_tempDirectoryCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"path_tempDirectory", "@path_tempDirectory", path_tempDirectoryTypeFn, {}, true}));
    auto path_tempFileCallInfo = TypeCallInfo{"path_tempFile"};
    auto path_tempFileTypeFn = TypeFn{this->get("str"), {}, false, path_tempFileCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"path_tempFile", "@path_tempFile", path_tempFileTypeFn, {}, true}));
  }
}

void TypeMap::_processModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto process_cwdCallInfo = TypeCallInfo{"process_cwd"};
    auto process_cwdTypeFn = TypeFn{this->get("str"), {}, false, process_cwdCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"process_cwd", "@process_cwd", process_cwdTypeFn, {}, true}));
    auto process_exitCallInfo = TypeCallInfo{"process_exit"};
    auto process_exitTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"status", this->get("int"), false, false, false}
    }, false, process_exitCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"process_exit", "@process_exit", process_exitTypeFn, {}, true}));
    auto process_getgidCallInfo = TypeCallInfo{"process_getgid"};
    auto process_getgidTypeFn = TypeFn{this->get("int"), {}, false, process_getgidCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"process_getgid", "@process_getgid", process_getgidTypeFn, {}, true}));
    auto process_getuidCallInfo = TypeCallInfo{"process_getuid"};
    auto process_getuidTypeFn = TypeFn{this->get("int"), {}, false, process_getuidCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"process_getuid", "@process_getuid", process_getuidTypeFn, {}, true}));
    auto process_runSyncCallInfo = TypeCallInfo{"process_runSync"};
    auto process_runSyncTypeFn = TypeFn{this->get("buffer_Buffer"), {
      TypeFnParam{"command", this->get("str"), false, true, false}
    }, false, process_runSyncCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"process_runSync", "@process_runSync", process_runSyncTypeFn, {}, true}));
  }
}

void TypeMap::_randomModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto random_randomFloatCallInfo = TypeCallInfo{"random_randomFloat"};
    auto random_randomFloatTypeFn = TypeFn{this->get("float"), {
      TypeFnParam{"min", this->get("float"), false, false, false},
      TypeFnParam{"max", this->get("float"), false, false, false}
    }, false, random_randomFloatCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"random_randomFloat", "@random_randomFloat", random_randomFloatTypeFn, {}, true}));
    auto random_randomIntCallInfo = TypeCallInfo{"random_randomInt"};
    auto random_randomIntTypeFn = TypeFn{this->get("int"), {
      TypeFnParam{"min", this->get("int"), false, false, false},
      TypeFnParam{"max", this->get("int"), false, false, false}
    }, false, random_randomIntCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"random_randomInt", "@random_randomInt", random_randomIntTypeFn, {}, true}));
    auto random_randomStrCallInfo = TypeCallInfo{"random_randomStr"};
    auto random_randomStrTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"characters", this->get("str"), false, false, false},
      TypeFnParam{"len", this->get("int"), false, false, false}
    }, false, random_randomStrCallInfo};
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
      auto closeCallInfo = TypeCallInfo{"request_close", true, "self_0", refSelfType, true};
      auto closeTypeFn = TypeFn{this->get("void"), {}, true, closeCallInfo};
      this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".close", "@request_Request.close", closeTypeFn, {}, true}));
      selfType->fields.push_back(TypeField{"close", this->_items.back().get(), false, true});
      auto readCallInfo = TypeCallInfo{"request_read", true, "self_0", refSelfType, true};
      auto readTypeFn = TypeFn{this->get("request_Response"), {}, true, readCallInfo};
      this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".read", "@request_Request.read", readTypeFn, {}, true}));
      selfType->fields.push_back(TypeField{"read", this->_items.back().get(), false, true});
    });
    auto request_openCallInfo = TypeCallInfo{"request_open"};
    auto request_openTypeFn = TypeFn{this->get("request_Request"), {
      TypeFnParam{"method", this->get("str"), false, true, false},
      TypeFnParam{"url", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, false, false},
      TypeFnParam{"headers", this->createArr(this->get("request_Header")), false, false, false}
    }, false, request_openCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"request_open", "@request_open", request_openTypeFn, {}, true}));
  }
}

void TypeMap::_threadModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto thread_sleepCallInfo = TypeCallInfo{"thread_sleep"};
    auto thread_sleepTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"milliseconds", this->get("int"), false, true, false}
    }, false, thread_sleepCallInfo};
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
    auto url_parseCallInfo = TypeCallInfo{"url_parse"};
    auto url_parseTypeFn = TypeFn{this->get("url_URL"), {
      TypeFnParam{"url", this->get("str"), false, true, false}
    }, false, url_parseCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"url_parse", "@url_parse", url_parseTypeFn, {}, true}));
  }
}

void TypeMap::_utilsModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto utils_swapCallInfo = TypeCallInfo{"utils_swap"};
    auto utils_swapTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"a", this->createRef(this->get("any")), true, true, false},
      TypeFnParam{"b", this->createRef(this->get("any")), true, true, false}
    }, false, utils_swapCallInfo};
    this->_items.push_back(std::make_unique<Type>(Type{"utils_swap", "@utils_swap", utils_swapTypeFn, {}, true}));
  }
}

void TypeMap::_arrTypeDef (Type *selfType, Type *refSelfType, Type *elementType, Type *refElementType) {
  auto emptyCallInfo = TypeCallInfo{typeName(selfType->name + "_empty"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, true, emptyCallInfo});
  auto firstCallInfo = TypeCallInfo{typeName(selfType->name + "_first"), true, "self_0", refSelfType, true};
  selfType->fields.push_back(TypeField{"first", refElementType, false, true, firstCallInfo});
  auto lastCallInfo = TypeCallInfo{typeName(selfType->name + "_last"), true, "self_0", refSelfType, true};
  selfType->fields.push_back(TypeField{"last", refElementType, false, true, lastCallInfo});
  auto lenCallInfo = TypeCallInfo{typeName(selfType->name + "_len"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"len", this->get("int"), false, true, lenCallInfo});
  auto clearCallInfo = TypeCallInfo{typeName(selfType->name + "_clear"), true, "self_0", refSelfType, true};
  auto clearTypeFn = TypeFn{refSelfType, {}, true, clearCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".clear", "@array.clear", clearTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"clear", this->_items.back().get(), false, true});
  auto concatCallInfo = TypeCallInfo{typeName(selfType->name + "_concat"), true, "self_0", selfType, false};
  auto concatTypeFn = TypeFn{selfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, true, concatCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".concat", "@array.concat", concatTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"concat", this->_items.back().get(), false, true});
  auto containsCallInfo = TypeCallInfo{typeName(selfType->name + "_contains"), true, "self_0", selfType, false};
  auto containsTypeFn = TypeFn{this->get("bool"), {
    TypeFnParam{"search", elementType, false, true, false}
  }, true, containsCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".contains", "@array.contains", containsTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"contains", this->_items.back().get(), false, true});
  auto filterCallInfo = TypeCallInfo{typeName(selfType->name + "_filter"), true, "self_0", selfType, false};
  auto filterTypeFn = TypeFn{selfType, {
    TypeFnParam{"predicate", this->createFn({
      TypeFnParam{"it", elementType, false, true, false}
    }, this->get("bool"), std::nullopt), false, true, false}
  }, true, filterCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".filter", "@array.filter", filterTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"filter", this->_items.back().get(), false, true});
  auto forEachCallInfo = TypeCallInfo{typeName(selfType->name + "_forEach"), true, "self_0", selfType, false};
  auto forEachTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"iterator", this->createFn({
      TypeFnParam{"it", elementType, false, true, false},
      TypeFnParam{"idx", this->get("int"), false, true, false}
    }, this->get("void"), std::nullopt), false, true, false}
  }, true, forEachCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".forEach", "@array.forEach", forEachTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"forEach", this->_items.back().get(), false, true});
  auto joinCallInfo = TypeCallInfo{typeName(selfType->name + "_join"), true, "self_0", selfType, false};
  auto joinTypeFn = TypeFn{this->get("str"), {
    TypeFnParam{"separator", this->get("str"), false, false, false}
  }, true, joinCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".join", "@array.join", joinTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"join", this->_items.back().get(), false, true});
  auto mergeCallInfo = TypeCallInfo{typeName(selfType->name + "_merge"), true, "self_0", refSelfType, true};
  auto mergeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, true, mergeCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".merge", "@array.merge", mergeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"merge", this->_items.back().get(), false, true});
  auto popCallInfo = TypeCallInfo{typeName(selfType->name + "_pop"), true, "self_0", refSelfType, true};
  auto popTypeFn = TypeFn{elementType, {}, true, popCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".pop", "@array.pop", popTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"pop", this->_items.back().get(), false, true});
  auto pushCallInfo = TypeCallInfo{typeName(selfType->name + "_push"), true, "self_0", refSelfType, true};
  auto pushTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"elements", this->createArr(elementType), false, false, true}
  }, true, pushCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".push", "@array.push", pushTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"push", this->_items.back().get(), false, true});
  auto removeCallInfo = TypeCallInfo{typeName(selfType->name + "_remove"), true, "self_0", refSelfType, true};
  auto removeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"index", this->get("int"), false, true, false}
  }, true, removeCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".remove", "@array.remove", removeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"remove", this->_items.back().get(), false, true});
  auto reverseCallInfo = TypeCallInfo{typeName(selfType->name + "_reverse"), true, "self_0", selfType, false};
  auto reverseTypeFn = TypeFn{selfType, {}, true, reverseCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".reverse", "@array.reverse", reverseTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"reverse", this->_items.back().get(), false, true});
  auto sliceCallInfo = TypeCallInfo{typeName(selfType->name + "_slice"), true, "self_0", selfType, false};
  auto sliceTypeFn = TypeFn{selfType, {
    TypeFnParam{"start", this->get("int"), false, false, false},
    TypeFnParam{"end", this->get("int"), false, false, false}
  }, true, sliceCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".slice", "@array.slice", sliceTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true});
  auto sortCallInfo = TypeCallInfo{typeName(selfType->name + "_sort"), true, "self_0", refSelfType, true};
  auto sortTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"comparator", this->createFn({
      TypeFnParam{"a", elementType, false, true, false},
      TypeFnParam{"b", elementType, false, true, false}
    }, this->get("int"), std::nullopt), false, true, false}
  }, true, sortCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".sort", "@array.sort", sortTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"sort", this->_items.back().get(), false, true});
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@array.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_enumTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto rawValueCallInfo = TypeCallInfo{typeName(selfType->codeName + "_rawValue"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"rawValue", this->get("str"), false, true, rawValueCallInfo});
  auto strCallInfo = TypeCallInfo{"enum_str", true, "self_0", selfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->codeName + ".str", "@enum.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_fnTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@fn.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_mapTypeDef (Type *selfType, Type *refSelfType, Type *keyType, Type *valueType) {
  auto emptyCallInfo = TypeCallInfo{typeName(selfType->name + "_empty"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, true, emptyCallInfo});
  auto capCallInfo = TypeCallInfo{typeName(selfType->name + "_cap"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"cap", this->get("int"), false, true, capCallInfo});
  auto keysCallInfo = TypeCallInfo{typeName(selfType->name + "_keys"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"keys", this->createArr(keyType), false, true, keysCallInfo});
  auto lenCallInfo = TypeCallInfo{typeName(selfType->name + "_len"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"len", this->get("int"), false, true, lenCallInfo});
  auto valuesCallInfo = TypeCallInfo{typeName(selfType->name + "_values"), true, "self_0", selfType, false};
  selfType->fields.push_back(TypeField{"values", this->createArr(valueType), false, true, valuesCallInfo});
  auto clearCallInfo = TypeCallInfo{typeName(selfType->name + "_clear"), true, "self_0", refSelfType, true};
  auto clearTypeFn = TypeFn{refSelfType, {}, true, clearCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".clear", "@map.clear", clearTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"clear", this->_items.back().get(), false, true});
  auto getCallInfo = TypeCallInfo{typeName(selfType->name + "_get"), true, "self_0", selfType, false};
  auto getTypeFn = TypeFn{valueType, {
    TypeFnParam{"key", keyType, false, true, false}
  }, true, getCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".get", "@map.get", getTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"get", this->_items.back().get(), false, true});
  auto hasCallInfo = TypeCallInfo{typeName(selfType->name + "_has"), true, "self_0", selfType, false};
  auto hasTypeFn = TypeFn{this->get("bool"), {
    TypeFnParam{"key", keyType, false, true, false}
  }, true, hasCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".has", "@map.has", hasTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"has", this->_items.back().get(), false, true});
  auto mergeCallInfo = TypeCallInfo{typeName(selfType->name + "_merge"), true, "self_0", refSelfType, true};
  auto mergeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, true, mergeCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".merge", "@map.merge", mergeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"merge", this->_items.back().get(), false, true});
  auto removeCallInfo = TypeCallInfo{typeName(selfType->name + "_remove"), true, "self_0", refSelfType, true};
  auto removeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"key", keyType, false, true, false}
  }, true, removeCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".remove", "@map.remove", removeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"remove", this->_items.back().get(), false, true});
  auto reserveCallInfo = TypeCallInfo{typeName(selfType->name + "_reserve"), true, "self_0", refSelfType, true};
  auto reserveTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"size", this->get("int"), false, true, false}
  }, true, reserveCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".reserve", "@map.reserve", reserveTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"reserve", this->_items.back().get(), false, true});
  auto setCallInfo = TypeCallInfo{typeName(selfType->name + "_set"), true, "self_0", refSelfType, true};
  auto setTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"key", keyType, false, true, false},
    TypeFnParam{"value", valueType, false, true, false}
  }, true, setCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".set", "@map.set", setTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"set", this->_items.back().get(), false, true});
  auto shrinkCallInfo = TypeCallInfo{typeName(selfType->name + "_shrink"), true, "self_0", refSelfType, true};
  auto shrinkTypeFn = TypeFn{refSelfType, {}, true, shrinkCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".shrink", "@map.shrink", shrinkTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"shrink", this->_items.back().get(), false, true});
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@map.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_objTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{
    selfType->builtin ? (selfType->name + "_str") : typeName(selfType->codeName + "_str"),
    true,
    "self_0",
    selfType,
    false
  };
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@obj.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_optTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@opt.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}

void TypeMap::_unionTypeDef (Type *selfType, [[maybe_unused]] Type *refSelfType) {
  auto strCallInfo = TypeCallInfo{typeName(selfType->name + "_str"), true, "self_0", selfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strCallInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@union.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
}
