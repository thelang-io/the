/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <algorithm>
#include <climits>
#include "TypeMap.hpp"

bool Type::isAny () const {
  return this->name == "any";
}

bool Type::isBool () const {
  return this->name == "bool";
}

bool Type::isByte () const {
  return this->name == "byte";
}

bool Type::isChar () const {
  return this->name == "char";
}

bool Type::isFloat () const {
  return this->name == "float";
}

bool Type::isF32 () const {
  return this->name == "f32";
}

bool Type::isF64 () const {
  return this->name == "f64";
}

bool Type::isFn () const {
  return std::holds_alternative<TypeFn>(this->body);
}

bool Type::isInt () const {
  return this->name == "int";
}

bool Type::isI8 () const {
  return this->name == "i8";
}

bool Type::isI16 () const {
  return this->name == "i16";
}

bool Type::isI32 () const {
  return this->name == "i32";
}

bool Type::isI64 () const {
  return this->name == "i64";
}

bool Type::isObj () const {
  return this->parent != std::nullopt && (*this->parent)->name == "obj";
}

bool Type::isStr () const {
  return this->name == "str";
}

bool Type::isU8 () const {
  return this->name == "u8";
}

bool Type::isU16 () const {
  return this->name == "u16";
}

bool Type::isU32 () const {
  return this->name == "u32";
}

bool Type::isU64 () const {
  return this->name == "u64";
}

bool Type::isVoid () const {
  return this->name == "void";
}

std::shared_ptr<Type> TypeMap::fn (const std::shared_ptr<Type> &type, const std::vector<TypeFnParam> &params) {
  auto typeFn = TypeFn{type, params};
  return std::make_shared<Type>(Type{"$", std::nullopt, typeFn});
}

std::shared_ptr<Type> TypeMap::fn (const std::string &name, const std::shared_ptr<Type> &type, const std::vector<TypeFnParam> &params) {
  auto typeFn = TypeFn{type, params};
  return std::make_shared<Type>(Type{name, std::nullopt, typeFn});
}

TypeMap::TypeMap () {
  this->stack.reserve(SHRT_MAX);
  this->_obj = std::make_shared<Type>(Type{"obj", std::nullopt, TypeObj{}});
}

std::shared_ptr<Type> TypeMap::add (const std::string &name, const std::optional<std::shared_ptr<Type>> &parent, const std::vector<TypeObjField> &fields) {
  auto obj = TypeObj{fields};
  this->_items.push_back(std::make_shared<Type>(Type{name, parent, obj}));

  return this->_items.back();
}

std::shared_ptr<Type> TypeMap::add (const std::string &name, const std::shared_ptr<Type> &type, const std::vector<TypeFnParam> &params) {
  auto typeFn = TypeFn{type, params};

  this->_items.push_back(std::make_shared<Type>(Type{name, std::nullopt, typeFn}));
  return this->_items.back();
}

const std::shared_ptr<Type> &TypeMap::get (const std::string &name) const {
  for (const auto &item : this->_items) {
    if (item->name == name) {
      return item;
    }
  }

  throw Error("Tried to access non existing typed map item");
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
      fullName = fullNameTest;
      break;
    }
  }

  return fullName;
}

std::shared_ptr<Type> TypeMap::obj () const {
  return this->_obj;
}
