/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "TypeMap.hpp"
#include <limits>

Type *TypeMap::fn (const std::optional<std::string> &codeName, const std::vector<TypeFnParam> &params, Type *returnType) {
  auto newType = Type{"", codeName == std::nullopt ? "@" : *codeName, TypeFn{returnType, params}, false};

  for (auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->codeName == newType.codeName && item->matchExact(&newType)) {
      return item.get();
    }
  }

  for (auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchNice(&newType)) {
      newType.name = item->name;
      break;
    }
  }

  if (newType.name.empty()) {
    newType.name = "fn$" + std::to_string(this->_fnIdx++);
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  return this->_items.back().get();
}

Type *TypeMap::get (const std::string &name) {
  for (const auto &item : this->_items) {
    if (item->name == name) {
      return item.get();
    }
  }

  return nullptr;
}

bool TypeMap::has (const std::string &name) {
  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
    return it->name == name;
  });
}

void TypeMap::init () {
  this->stack.reserve(std::numeric_limits<short>::max());

  this->_items.push_back(std::make_unique<Type>(Type{"any", "@any", TypeObj{}, true}));
  auto anyType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"bool", "@bool", TypeObj{}, true}));
  auto boolType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"byte", "@byte", TypeObj{}, true}));
  auto byteType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"char", "@char", TypeObj{}, true}));
  auto charType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f32", "@f32", TypeObj{}, true}));
  auto f32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f64", "@f64", TypeObj{}, true}));
  auto f64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"float", "@float", TypeObj{}, true}));
  auto floatType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i8", "@i8", TypeObj{}, true}));
  auto i8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i16", "@i16", TypeObj{}, true}));
  auto i16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i32", "@i32", TypeObj{}, true}));
  auto i32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i64", "@i64", TypeObj{}, true}));
  auto i64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"int", "@int", TypeObj{}, true}));
  auto intType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"str", "@str", TypeObj{}, true}));
  auto strType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u8", "@u8", TypeObj{}, true}));
  auto u8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u16", "@u16", TypeObj{}, true}));
  auto u16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u32", "@u32", TypeObj{}, true}));
  auto u32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u64", "@u64", TypeObj{}, true}));
  auto u64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"void", "@void", TypeObj{}, true}));
  auto voidType = this->_items.back().get();

  auto strConcatTypeFn = TypeFn{strType, {
    TypeFnParam{"src", strType, false, true, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"bool.str", "@bool.str", TypeFn{strType}, true}));
  std::get<TypeObj>(boolType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"byte.str", "@byte.str", TypeFn{strType}, true}));
  std::get<TypeObj>(byteType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"char.str", "@char.str", TypeFn{strType}, true}));
  std::get<TypeObj>(charType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"f32.str", "@f32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(f32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"f64.str", "@f64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(f64Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"float.str", "@float.str", TypeFn{strType}, true}));
  std::get<TypeObj>(floatType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"i8.str", "@i8.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i8Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"i16.str", "@i16.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i16Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"i32.str", "@i32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"i64.str", "@i64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i64Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"int.str", "@int.str", TypeFn{strType}, true}));
  std::get<TypeObj>(intType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"str.concat", "@str.concat", strConcatTypeFn, true}));
  std::get<TypeObj>(strType->body).fields.push_back({"concat", this->_items.back().get()});
  std::get<TypeObj>(strType->body).fields.push_back({"len", intType});
  this->_items.push_back(std::make_unique<Type>(Type{"u8.str", "@u8.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u8Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"u16.str", "@u16.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u16Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"u32.str", "@u32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"u64.str", "@u64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u64Type->body).fields.push_back({"str", this->_items.back().get()});

  auto printTypeFn = TypeFn{voidType, {
    TypeFnParam{"items", anyType, false, false, true},
    TypeFnParam{"separator", strType, false, false, false},
    TypeFnParam{"terminator", strType, false, false, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"print", "@print", printTypeFn, true}));
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

Type *TypeMap::obj (const std::string &name, const std::string &codeName, const std::vector<TypeObjField> &fields) {
  this->_items.push_back(std::make_unique<Type>(Type{name, codeName, TypeObj{fields}, false}));
  return this->_items.back().get();
}

Type *TypeMap::ref (Type *type) {
  for (const auto &item : this->_items) {
    if (item->isRef()) {
      auto typeRef = std::get<TypeRef>(item->body);

      if (typeRef.type->matchExact(type)) {
        return item.get();
      }
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"@", "@", TypeRef{type}, false}));
  return this->_items.back().get();
}
