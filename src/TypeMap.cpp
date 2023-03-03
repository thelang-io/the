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
  this->_fsModule(TYPE_MAP_DECL);
  this->_globalsModule(TYPE_MAP_DECL);
  this->_pathModule(TYPE_MAP_DECL);
  this->_processModule(TYPE_MAP_DECL);
  this->_requestModule(TYPE_MAP_DECL);
  this->_urlModule(TYPE_MAP_DECL);

  this->_anyType(TYPE_MAP_DEF);
  this->_boolType(TYPE_MAP_DEF);
  this->_byteType(TYPE_MAP_DEF);
  this->_charType(TYPE_MAP_DEF);
  this->_floatType(TYPE_MAP_DEF);
  this->_intType(TYPE_MAP_DEF);
  this->_strType(TYPE_MAP_DEF);
  this->_voidType(TYPE_MAP_DEF);
  this->_bufferModule(TYPE_MAP_DEF);
  this->_fsModule(TYPE_MAP_DEF);
  this->_globalsModule(TYPE_MAP_DEF);
  this->_pathModule(TYPE_MAP_DEF);
  this->_processModule(TYPE_MAP_DEF);
  this->_requestModule(TYPE_MAP_DEF);
  this->_urlModule(TYPE_MAP_DEF);
}

Type *TypeMap::createAlias (const std::string &name, Type *type) {
  this->_items.push_back(std::make_unique<Type>(Type{name, this->name(name), TypeAlias{type}}));
  return this->_items.back().get();
}

Type *TypeMap::createArr (Type *elementType) {
  if (elementType->isAlias()) {
    return this->createArr(std::get<TypeAlias>(elementType->body).type);
  }

  for (const auto &item : this->_items) {
    if (item->isArray() && std::get<TypeArray>(item->body).elementType->matchStrict(elementType)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"array_" + elementType->name, "@array_" + elementType->name, TypeArray{elementType}}));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);
  auto refElementType = this->createRef(elementType);

  selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, false, true});
  selfType->fields.push_back(TypeField{"first", refElementType, false, false, true});
  selfType->fields.push_back(TypeField{"last", refElementType, false, false, true});
  selfType->fields.push_back(TypeField{"len", this->get("int"), false, false, true});
  auto clearMethodInfo = TypeFnMethodInfo{selfType->name + "_clear", true, "self_0", refSelfType, true};
  auto clearTypeFn = TypeFn{refSelfType, {}, true, clearMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".clear", "@array.clear", clearTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"clear", this->_items.back().get(), false, true, true});
  auto containsMethodInfo = TypeFnMethodInfo{selfType->name + "_contains", true, "self_0", refSelfType, false};
  auto containsTypeFn = TypeFn{this->get("bool"), {
    TypeFnParam{"search", elementType, false, true, false}
  }, true, containsMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".contains", "@array.contains", containsTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"contains", this->_items.back().get(), false, true, true});
  auto filterMethodInfo = TypeFnMethodInfo{selfType->name + "_filter", true, "self_0", refSelfType, false};
  auto filterTypeFn = TypeFn{selfType, {
    TypeFnParam{"predicate", this->createFn({
      TypeFnParam{"it", elementType, false, true, false}
    }, this->get("bool")), false, true, false}
  }, true, filterMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".filter", "@array.filter", filterTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"filter", this->_items.back().get(), false, true, true});
  auto joinMethodInfo = TypeFnMethodInfo{selfType->name + "_join", true, "self_0", refSelfType, false};
  auto joinTypeFn = TypeFn{this->get("str"), {
    TypeFnParam{"separator", this->get("str"), false, false, false}
  }, true, joinMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".join", "@array.join", joinTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"join", this->_items.back().get(), false, true, true});
  auto mapMethodInfo = TypeFnMethodInfo{selfType->name + "_map", true, "self_0", refSelfType, false};
  auto mapTypeFn = TypeFn{this->createArr(this->get("any")), {
    TypeFnParam{"transform", this->createFn({
      TypeFnParam{"it", elementType, false, true, false}
    }, this->get("any")), false, true, false}
  }, true, mapMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".map", "@array.map", mapTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"map", this->_items.back().get(), false, true, true});
  auto mergeMethodInfo = TypeFnMethodInfo{selfType->name + "_merge", true, "self_0", refSelfType, true};
  auto mergeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, true, mergeMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".merge", "@array.merge", mergeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"merge", this->_items.back().get(), false, true, true});
  auto popMethodInfo = TypeFnMethodInfo{selfType->name + "_pop", true, "self_0", refSelfType, true};
  auto popTypeFn = TypeFn{elementType, {}, true, popMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".pop", "@array.pop", popTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"pop", this->_items.back().get(), false, true, true});
  auto pushMethodInfo = TypeFnMethodInfo{selfType->name + "_push", true, "self_0", refSelfType, true};
  auto pushTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"elements", elementType, false, false, true}
  }, true, pushMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".push", "@array.push", pushTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"push", this->_items.back().get(), false, true, true});
  auto removeMethodInfo = TypeFnMethodInfo{selfType->name + "_remove", true, "self_0", refSelfType, true};
  auto removeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"indexes", this->get("int"), false, false, true}
  }, true, removeMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".remove", "@array.remove", removeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"remove", this->_items.back().get(), false, true, true});
  auto reverseMethodInfo = TypeFnMethodInfo{selfType->name + "_reverse", true, "self_0", refSelfType, false};
  auto reverseTypeFn = TypeFn{selfType, {}, true, reverseMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".reverse", "@array.reverse", reverseTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"reverse", this->_items.back().get(), false, true, true});
  auto sliceMethodInfo = TypeFnMethodInfo{selfType->name + "_slice", true, "self_0", refSelfType, false};
  auto sliceTypeFn = TypeFn{selfType, {
    TypeFnParam{"start", this->get("int"), false, false, false},
    TypeFnParam{"end", this->get("int"), false, false, false}
  }, true, sliceMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".slice", "@array.slice", sliceTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true, true});
  auto sortMethodInfo = TypeFnMethodInfo{selfType->name + "_sort", true, "self_0", refSelfType, true};
  auto sortTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"comparator", this->createFn({
      TypeFnParam{"a", elementType, false, true, false},
      TypeFnParam{"b", elementType, false, true, false}
    }, this->get("int")), false, true, false}
  }, true, sortMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".sort", "@array.sort", sortTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"sort", this->_items.back().get(), false, true, true});
  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@array.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::createMap (Type *keyType, Type *valueType) {
  auto actualKeyType = Type::actual(keyType);
  auto actualValueType = Type::actual(valueType);
  auto newType = Type{"", "", TypeBodyMap{actualKeyType, actualValueType}};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isMap() && item->matchStrict(&newType)) {
      return item.get();
    }
  }

  newType.name = "map$" + std::to_string(this->_mapIdx++);
  newType.codeName = "@" + newType.name;

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, false, true});
  selfType->fields.push_back(TypeField{"cap", this->get("int"), false, false, true});
  selfType->fields.push_back(TypeField{"keys", this->createArr(actualKeyType), false, false, true});
  selfType->fields.push_back(TypeField{"len", this->get("int"), false, false, true});
  selfType->fields.push_back(TypeField{"values", this->createArr(actualValueType), false, false, true});
  auto clearMethodInfo = TypeFnMethodInfo{selfType->name + "_clear", true, "self_0", refSelfType, true};
  auto clearTypeFn = TypeFn{refSelfType, {}, true, clearMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".clear", "@map.clear", clearTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"clear", this->_items.back().get(), false, true, true});
  auto getMethodInfo = TypeFnMethodInfo{selfType->name + "_get", true, "self_0", refSelfType, false};
  auto getTypeFn = TypeFn{actualValueType, {
    TypeFnParam{"key", actualKeyType, false, true, false}
  }, true, getMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".get", "@map.get", getTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"get", this->_items.back().get(), false, true, true});
  auto hasMethodInfo = TypeFnMethodInfo{selfType->name + "_has", true, "self_0", refSelfType, false};
  auto hasTypeFn = TypeFn{this->get("bool"), {
    TypeFnParam{"key", actualKeyType, false, true, false}
  }, true, hasMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".has", "@map.has", hasTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"has", this->_items.back().get(), false, true, true});
  auto mergeMethodInfo = TypeFnMethodInfo{selfType->name + "_merge", true, "self_0", refSelfType, true};
  auto mergeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"other", selfType, false, true, false}
  }, true, mergeMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".merge", "@map.merge", mergeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"merge", this->_items.back().get(), false, true, true});
  auto removeMethodInfo = TypeFnMethodInfo{selfType->name + "_remove", true, "self_0", refSelfType, true};
  auto removeTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"key", actualKeyType, false, true, false}
  }, true, removeMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".remove", "@map.remove", removeTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"remove", this->_items.back().get(), false, true, true});
  auto reserveMethodInfo = TypeFnMethodInfo{selfType->name + "_reserve", true, "self_0", refSelfType, true};
  auto reserveTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"size", this->get("int"), false, true, false}
  }, true, reserveMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".reserve", "@map.reserve", reserveTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"reserve", this->_items.back().get(), false, true, true});
  auto setMethodInfo = TypeFnMethodInfo{selfType->name + "_set", true, "self_0", refSelfType, true};
  auto setTypeFn = TypeFn{refSelfType, {
    TypeFnParam{"key", actualKeyType, false, true, false},
    TypeFnParam{"value", actualValueType, false, true, false}
  }, true, setMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".set", "@map.set", setTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"set", this->_items.back().get(), false, true, true});
  auto shrinkMethodInfo = TypeFnMethodInfo{selfType->name + "_shrink", true, "self_0", refSelfType, true};
  auto shrinkTypeFn = TypeFn{refSelfType, {}, true, shrinkMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".shrink", "@map.shrink", shrinkTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"shrink", this->_items.back().get(), false, true, true});
  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@map.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::createEnum (const std::string &name, const std::string &codeName, const std::vector<Type *> &members) {
  auto typeBody = TypeEnum{members};
  auto newType = Type{name, codeName, typeBody};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->codeName == newType.codeName) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  selfType->fields.push_back(TypeField{"rawValue", this->get("str"), false, false, true});
  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@enum.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::createFn (const std::vector<TypeFnParam> &params, Type *returnType, const std::optional<TypeFnMethodInfo> &methodInfo) {
  auto typeBody = TypeFn{
    returnType,
    params,
    methodInfo != std::nullopt,
    methodInfo == std::nullopt ? TypeFnMethodInfo{} : *methodInfo
  };

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

  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@fn.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::createObj (const std::string &name, const std::string &codeName, const std::vector<TypeField> &fields) {
  this->_items.push_back(std::make_unique<Type>(Type{name, codeName, TypeObj{}, fields}));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@obj.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::createOpt (Type *type) {
  if (type->isAlias()) {
    return this->createOpt(std::get<TypeAlias>(type->body).type);
  }

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isOpt() && std::get<TypeOptional>(item->body).type->matchStrict(type)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"opt_" + type->name, "@opt_" + type->name, TypeOptional{type}}));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@opt.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::createRef (Type *refType) {
  if (refType->isAlias()) {
    return this->createRef(std::get<TypeAlias>(refType->body).type);
  }

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isRef() && std::get<TypeRef>(item->body).refType->matchStrict(refType)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"ref_" + refType->name, "@ref_" + refType->name, TypeRef{refType}}));
  return this->_items.back().get();
}

Type *TypeMap::createUnion (const std::vector<Type *> &subTypes) {
  auto newType = Type{"", "", TypeUnion{subTypes}};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->isUnion() && item->matchStrict(&newType)) {
      return item.get();
    }
  }

  newType.name = "union$" + std::to_string(this->_unIdx++);
  newType.codeName = "@" + newType.name;

  this->_items.push_back(std::make_unique<Type>(newType));
  auto selfType = this->_items.back().get();
  auto refSelfType = this->createRef(selfType);

  auto strMethodInfo = TypeFnMethodInfo{selfType->name + "_str", true, "self_0", refSelfType, false};
  auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
  this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@union.str", strTypeFn, {}, true}));
  selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});

  return selfType;
}

Type *TypeMap::enumerator (const std::string &enumeratorName, const std::string &enumeratorCodeName) {
  auto newType = Type{enumeratorName, enumeratorCodeName, TypeEnumerator{}};

  for (const auto &item : this->_items) {
    if (!item->builtin && item->codeName == newType.codeName) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  return this->_items.back().get();
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
    if (!subType->matchStrict(exceptType)) {
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
  this->_initType("any", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"any_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@any.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_boolType (TypeMapPhase phase) {
  this->_initType("bool", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"bool_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@bool.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_byteType (TypeMapPhase phase) {
  this->_initType("byte", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"byte_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@byte.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_charType (TypeMapPhase phase) {
  this->_initType("char", phase, [&] (auto selfType, auto refSelfType) {
    selfType->fields.push_back(TypeField{"byte", this->get("byte"), false, false, true});
    selfType->fields.push_back(TypeField{"isDigit", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"isLetter", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"isLetterOrDigit", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"isLower", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"isUpper", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"isWhitespace", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"lower", this->get("char"), false, false, true});
    selfType->fields.push_back(TypeField{"upper", this->get("char"), false, false, true});
    auto repeatMethodInfo = TypeFnMethodInfo{"char_repeat", true, "self_0", refSelfType, false};
    auto repeatTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"times", this->get("int"), false, true, false}
    }, true, repeatMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".repeat", "@char.repeat", repeatTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"repeat", this->_items.back().get(), false, true, true});
    auto strMethodInfo = TypeFnMethodInfo{"char_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@char.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_floatType (TypeMapPhase phase) {
  this->_initType("float", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"float_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@float.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("f32", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"f32_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@f32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("f64", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"f64_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@f64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_intType (TypeMapPhase phase) {
  this->_initType("int", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"int_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@int.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("i8", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"i8_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i8.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("i16", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"i16_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i16.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("i32", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"i32_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("i64", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"i64_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@i64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("u8", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"u8_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u8.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("u16", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"u16_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u16.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("u32", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"u32_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u32.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("u64", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"u64_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@u64.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_strType (TypeMapPhase phase) {
  this->_initType("str", phase, [&] (auto selfType, auto refSelfType) {
    selfType->fields.push_back(TypeField{"empty", this->get("bool"), false, false, true});
    selfType->fields.push_back(TypeField{"len", this->get("int"), false, false, true});
    selfType->fields.push_back(TypeField{"lower", selfType, false, false, true});
    selfType->fields.push_back(TypeField{"lowerFirst", selfType, false, false, true});
    selfType->fields.push_back(TypeField{"upper", selfType, false, false, true});
    selfType->fields.push_back(TypeField{"upperFirst", selfType, false, false, true});
    auto containsMethodInfo = TypeFnMethodInfo{"str_contains", true, "self_0", refSelfType, false};
    auto containsTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"search", selfType, false, true, false}
    }, true, containsMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".contains", "@str.contains", containsTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"contains", this->_items.back().get(), false, true, true});
    auto findMethodInfo = TypeFnMethodInfo{"str_find", true, "self_0", refSelfType, false};
    auto findTypeFn = TypeFn{selfType, {
      TypeFnParam{"search", selfType, false, true, false}
    }, true, findMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".find", "@str.find", findTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"find", this->_items.back().get(), false, true, true});
    auto linesMethodInfo = TypeFnMethodInfo{"str_lines", true, "self_0", refSelfType, false};
    auto linesTypeFn = TypeFn{this->createArr(selfType), {
      TypeFnParam{"keepLineBreaks", this->get("bool"), false, false, false}
    }, true, linesMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".lines", "@str.lines", linesTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"lines", this->_items.back().get(), false, true, true});
    auto replaceMethodInfo = TypeFnMethodInfo{"str_replace", true, "self_0", refSelfType, false};
    auto replaceTypeFn = TypeFn{selfType, {
      TypeFnParam{"search", selfType, false, true, false},
      TypeFnParam{"replacement", selfType, false, true, false},
      TypeFnParam{"all", this->get("bool"), false, false, false}
    }, true, replaceMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".replace", "@str.replace", replaceTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"replace", this->_items.back().get(), false, true, true});
    auto sliceMethodInfo = TypeFnMethodInfo{"str_slice", true, "self_0", refSelfType, false};
    auto sliceTypeFn = TypeFn{selfType, {
      TypeFnParam{"start", this->get("int"), false, false, false},
      TypeFnParam{"end", this->get("int"), false, false, false}
    }, true, sliceMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".slice", "@str.slice", sliceTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true, true});
    auto splitMethodInfo = TypeFnMethodInfo{"str_split", true, "self_0", refSelfType, false};
    auto splitTypeFn = TypeFn{this->createArr(selfType), {
      TypeFnParam{"delimiter", selfType, false, false, false}
    }, true, splitMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".split", "@str.split", splitTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"split", this->_items.back().get(), false, true, true});
    auto toBufferMethodInfo = TypeFnMethodInfo{"str_to_buffer", true, "self_0", refSelfType, false};
    auto toBufferTypeFn = TypeFn{this->get("buffer_Buffer"), {}, true, toBufferMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".toBuffer", "@str.toBuffer", toBufferTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"toBuffer", this->_items.back().get(), false, true, true});
    auto trimMethodInfo = TypeFnMethodInfo{"str_trim", true, "self_0", refSelfType, false};
    auto trimTypeFn = TypeFn{selfType, {}, true, trimMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".trim", "@str.trim", trimTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"trim", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_voidType (TypeMapPhase phase) {
  this->_initType("void", phase);
}

void TypeMap::_bufferModule (TypeMapPhase phase) {
  this->_initType("buffer_Buffer", phase, [&] (auto selfType, auto refSelfType) {
    auto strMethodInfo = TypeFnMethodInfo{"buffer_str", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@buffer_Buffer.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });
}

void TypeMap::_fsModule (TypeMapPhase phase) {
  this->_initType("fs_Stats", phase, [&] (auto selfType, auto refSelfType) {
    selfType->fields.push_back(TypeField{"dev", this->get("u64"), false, false, true});
    selfType->fields.push_back(TypeField{"mode", this->get("u32"), false, false, true});
    selfType->fields.push_back(TypeField{"nlink", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"ino", this->get("u64"), false, false, true});
    selfType->fields.push_back(TypeField{"uid", this->get("i32"), false, false, true});
    selfType->fields.push_back(TypeField{"gid", this->get("i32"), false, false, true});
    selfType->fields.push_back(TypeField{"rdev", this->get("u64"), false, false, true});
    selfType->fields.push_back(TypeField{"atime", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"atimeNs", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"mtime", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"mtimeNs", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"ctime", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"ctimeNs", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"btime", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"btimeNs", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"size", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"blocks", this->get("i64"), false, false, true});
    selfType->fields.push_back(TypeField{"blockSize", this->get("i64"), false, false, true});
    auto strMethodInfo = TypeFnMethodInfo{"fs_Stats__str_0", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@fs_Stats.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  if (phase == TYPE_MAP_DEF) {
    auto fs_chmodSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"mode", this->get("int"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_chmodSync", "@fs_chmodSync", fs_chmodSyncTypeFn, {}, true}));
    auto fs_chownSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"uid", this->get("int"), false, true, false},
      TypeFnParam{"gid", this->get("int"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_chownSync", "@fs_chownSync", fs_chownSyncTypeFn, {}, true}));
    auto fs_existsSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_existsSync", "@fs_existsSync", fs_existsSyncTypeFn, {}, true}));
    auto fs_isAbsoluteSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isAbsoluteSync", "@fs_isAbsoluteSync", fs_isAbsoluteSyncTypeFn, {}, true}));
    auto fs_isFileSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isFileSync", "@fs_isFileSync", fs_isFileSyncTypeFn, {}, true}));
    auto fs_isDirectorySyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isDirectorySync", "@fs_isDirectorySync", fs_isDirectorySyncTypeFn, {}, true}));
    auto fs_isSymbolicLinkSyncTypeFn = TypeFn{this->get("bool"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_isSymbolicLinkSync", "@fs_isSymbolicLinkSync", fs_isSymbolicLinkSyncTypeFn, {}, true}));
    auto fs_linkSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"sourcePath", this->get("str"), false, true, false},
      TypeFnParam{"linkPath", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_linkSync", "@fs_linkSync", fs_linkSyncTypeFn, {}, true}));
    auto fs_mkdirSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_mkdirSync", "@fs_mkdirSync", fs_mkdirSyncTypeFn, {}, true}));
    auto fs_readFileSyncTypeFn = TypeFn{this->get("buffer_Buffer"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_readFileSync", "@fs_readFileSync", fs_readFileSyncTypeFn, {}, true}));
    auto fs_realpathSyncTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_realpathSync", "@fs_realpathSync", fs_realpathSyncTypeFn, {}, true}));
    auto fs_rmSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_rmSync", "@fs_rmSync", fs_rmSyncTypeFn, {}, true}));
    auto fs_rmdirSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_rmdirSync", "@fs_rmdirSync", fs_rmdirSyncTypeFn, {}, true}));
    auto fs_scandirSyncTypeFn = TypeFn{this->createArr(this->get("str")), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_scandirSync", "@fs_scandirSync", fs_scandirSyncTypeFn, {}, true}));
    auto fs_statSyncTypeFn = TypeFn{this->get("fs_Stats"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_statSync", "@fs_statSync", fs_statSyncTypeFn, {}, true}));
    auto fs_unlinkSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_unlinkSync", "@fs_unlinkSync", fs_unlinkSyncTypeFn, {}, true}));
    auto fs_writeFileSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"path", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"fs_writeFileSync", "@fs_writeFileSync", fs_writeFileSyncTypeFn, {}, true}));
  }
}

void TypeMap::_globalsModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto exitTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"status", this->get("int"), false, false, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"exit", "@exit", exitTypeFn, {}, true}));
    auto printTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"items", this->get("any"), false, false, true},
      TypeFnParam{"separator", this->get("str"), false, false, false},
      TypeFnParam{"terminator", this->get("str"), false, false, false},
      TypeFnParam{"to", this->get("str"), false, false, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"print", "@print", printTypeFn, {}, true}));
    auto sleepSyncTypeFn = TypeFn{this->get("void"), {
      TypeFnParam{"milliseconds", this->get("int"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"sleepSync", "@sleepSync", sleepSyncTypeFn, {}, true}));
  }
}

void TypeMap::_pathModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto path_basenameTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"path_basename", "@path_basename", path_basenameTypeFn, {}, true}));
    auto path_dirnameTypeFn = TypeFn{this->get("str"), {
      TypeFnParam{"path", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"path_dirname", "@path_dirname", path_dirnameTypeFn, {}, true}));
  }
}

void TypeMap::_processModule (TypeMapPhase phase) {
  if (phase == TYPE_MAP_DEF) {
    auto process_runSyncTypeFn = TypeFn{this->get("buffer_Buffer"), {
      TypeFnParam{"command", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"process_runSync", "@process_runSync", process_runSyncTypeFn, {}, true}));
  }
}

void TypeMap::_requestModule (TypeMapPhase phase) {
  this->_initType("request_Header", phase, [&] (auto selfType, auto refSelfType) {
    selfType->fields.push_back(TypeField{"name", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"value", this->get("str"), false, false, true});
    auto strMethodInfo = TypeFnMethodInfo{"request_Header__str_0", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@request_Header.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("request_Request", phase, [&] (auto selfType, auto refSelfType) {
    auto closeMethodInfo = TypeFnMethodInfo{"request_close", true, "self_0", refSelfType, true};
    auto closeTypeFn = TypeFn{this->get("void"), {}, true, closeMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".close", "@request_Request.close", closeTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"close", this->_items.back().get(), false, true, true});
    auto readMethodInfo = TypeFnMethodInfo{"request_read", true, "self_0", refSelfType, true};
    auto readTypeFn = TypeFn{this->get("request_Response"), {}, true, readMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".read", "@request_Request.read", readTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"read", this->_items.back().get(), false, true, true});
    auto strMethodInfo = TypeFnMethodInfo{"request_Request__str_0", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@request_Request.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  this->_initType("request_Response", phase, [&] (auto selfType, auto refSelfType) {
    selfType->fields.push_back(TypeField{"data", this->get("buffer_Buffer"), false, false, true});
    selfType->fields.push_back(TypeField{"status", this->get("int"), false, false, true});
    selfType->fields.push_back(TypeField{"headers", this->createArr(this->get("request_Header")), false, false, true});
    auto strMethodInfo = TypeFnMethodInfo{"request_Response__str_0", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@request_Response.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  if (phase == TYPE_MAP_DEF) {
    auto request_openTypeFn = TypeFn{this->get("request_Request"), {
      TypeFnParam{"method", this->get("str"), false, true, false},
      TypeFnParam{"url", this->get("str"), false, true, false},
      TypeFnParam{"data", this->get("buffer_Buffer"), false, false, false},
      TypeFnParam{"headers", this->createArr(this->get("request_Header")), false, false, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"request_open", "@request_open", request_openTypeFn, {}, true}));
  }
}

void TypeMap::_urlModule (TypeMapPhase phase) {
  this->_initType("url_URL", phase, [&] (auto selfType, auto refSelfType) {
    selfType->fields.push_back(TypeField{"origin", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"protocol", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"host", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"hostname", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"port", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"path", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"pathname", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"search", this->get("str"), false, false, true});
    selfType->fields.push_back(TypeField{"hash", this->get("str"), false, false, true});
    auto strMethodInfo = TypeFnMethodInfo{"url_URL__str_0", true, "self_0", refSelfType, false};
    auto strTypeFn = TypeFn{this->get("str"), {}, true, strMethodInfo};
    this->_items.push_back(std::make_unique<Type>(Type{selfType->name + ".str", "@url_URL.str", strTypeFn, {}, true}));
    selfType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true, true});
  });

  if (phase == TYPE_MAP_DEF) {
    auto url_parseTypeFn = TypeFn{this->get("url_URL"), {
      TypeFnParam{"url", this->get("str"), false, true, false}
    }};
    this->_items.push_back(std::make_unique<Type>(Type{"url_parse", "@url_parse", url_parseTypeFn, {}, true}));
  }
}
