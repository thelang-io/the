/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "TypeMap.hpp"
#include <limits>

Type *TypeMap::add (const std::string &name, const std::vector<TypeObjField> &fields) {
  this->_items.push_back(std::make_unique<Type>(Type{name, TypeObj{fields}, false}));
  return this->_items.back().get();
}

Type *TypeMap::add (const std::string &name, const std::vector<TypeFnParam> &params, Type *returnType) {
  this->_items.push_back(std::make_unique<Type>(Type{name, TypeFn{returnType, params}, false}));
  return this->_items.back().get();
}

Type *TypeMap::fn (const std::vector<TypeFnParam> &params, Type *returnType) {
  this->_items.push_back(std::make_unique<Type>(Type{"@", TypeFn{returnType, params}, false}));
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

void TypeMap::init () {
  this->stack.reserve(std::numeric_limits<short>::max());

  this->_items.push_back(std::make_unique<Type>(Type{"any", TypeObj{}, true}));
  auto anyType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"bool", TypeObj{}, true}));
  auto boolType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"byte", TypeObj{}, true}));
  auto byteType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"char", TypeObj{}, true}));
  auto charType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f32", TypeObj{}, true}));
  auto f32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f64", TypeObj{}, true}));
  auto f64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"float", TypeObj{}, true}));
  auto floatType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i8", TypeObj{}, true}));
  auto i8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i16", TypeObj{}, true}));
  auto i16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i32", TypeObj{}, true}));
  auto i32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i64", TypeObj{}, true}));
  auto i64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"int", TypeObj{}, true}));
  auto intType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"str", TypeObj{}, true}));
  auto strType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u8", TypeObj{}, true}));
  auto u8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u16", TypeObj{}, true}));
  auto u16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u32", TypeObj{}, true}));
  auto u32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u64", TypeObj{}, true}));
  auto u64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"void", TypeObj{}, true}));
  auto voidType = this->_items.back().get();

  auto strConcatTypeFn = TypeFn{strType, {
    {"src", strType, true, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"@bool.str", TypeFn{strType}, true}));
  std::get<TypeObj>(boolType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@byte.str", TypeFn{strType}, true}));
  std::get<TypeObj>(byteType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@char.str", TypeFn{strType}, true}));
  std::get<TypeObj>(charType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@f32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(f32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@f64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(f64Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@float.str", TypeFn{strType}, true}));
  std::get<TypeObj>(floatType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@i8.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i8Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@i16.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i16Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@i32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@i64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(i64Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@int.str", TypeFn{strType}, true}));
  std::get<TypeObj>(intType->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@str.concat", strConcatTypeFn, true}));
  std::get<TypeObj>(strType->body).fields.push_back({"concat", this->_items.back().get()});
  std::get<TypeObj>(strType->body).fields.push_back({"len", intType});
  this->_items.push_back(std::make_unique<Type>(Type{"@u8.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u8Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@u16.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u16Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@u32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@u64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u64Type->body).fields.push_back({"str", this->_items.back().get()});

  auto printTypeFn = TypeFn{voidType, {
    {"items", anyType, false, true},
    {"separator", strType, false, false},
    {"terminator", strType, false, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"@print", printTypeFn, true}));
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
      if (item->name == fullNameTest) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      return fullNameTest;
    }
  }
}
