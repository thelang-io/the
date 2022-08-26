/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "TypeMap.hpp"
#include <limits>

Type *TypeMap::arrayOf (Type *elementType) {
  for (const auto &item : this->_items) {
    if (item->isArray() && std::get<TypeArray>(item->body).elementType->matchExact(elementType)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"array_" + elementType->name, "@array_" + elementType->name, TypeArray{elementType}}));
  auto self = this->_items.back().get();

  auto joinTypeFn = TypeFn{this->get("str"), {
    TypeFnParam{"separator", this->get("str"), false, false, false}
  }};

  auto popTypeFn = TypeFn{this->get("void")};

  auto pushTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"element", elementType, false, false, true}
  }};

  auto reverseTypeFn = TypeFn{self};

  auto sliceTypeFn = TypeFn{self, {
    TypeFnParam{"start", this->get("i64"), false, false, false},
    TypeFnParam{"end", this->get("i64"), false, false, false}
  }};

  auto strTypeFn = TypeFn{this->get("str")};

  self->fields.push_back(TypeField{"len", this->get("int"), true});

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".join", "@array.join", joinTypeFn, {}, true}));
  self->fields.push_back(TypeField{"join", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".pop", "@array.pop", popTypeFn, {}, true}));
  self->fields.push_back(TypeField{"pop", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".push", "@array.push", pushTypeFn, {}, true}));
  self->fields.push_back(TypeField{"push", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".reverse", "@array.reverse", reverseTypeFn, {}, true}));
  self->fields.push_back(TypeField{"reverse", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".slice", "@array.slice", sliceTypeFn, {}, true}));
  self->fields.push_back(TypeField{"slice", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@array.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), true});

  return self;
}

Type *TypeMap::fn (const std::optional<std::string> &codeName, const std::vector<TypeFnParam> &params, Type *returnType) {
  auto newType = Type{"", codeName == std::nullopt ? "@" : *codeName, TypeFn{returnType, params}};

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
  auto self = this->_items.back().get();

  auto strTypeFn = TypeFn{this->get("str")};

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@fn.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), true});

  return self;
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

  this->_items.push_back(std::make_unique<Type>(Type{"any", "@any", TypeObj{}, {}, true}));
  auto anyType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"bool", "@bool", TypeObj{}, {}, true}));
  auto boolType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"byte", "@byte", TypeObj{}, {}, true}));
  auto byteType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"char", "@char", TypeObj{}, {}, true}));
  auto charType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f32", "@f32", TypeObj{}, {}, true}));
  auto f32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f64", "@f64", TypeObj{}, {}, true}));
  auto f64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"float", "@float", TypeObj{}, {}, true}));
  auto floatType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i8", "@i8", TypeObj{}, {}, true}));
  auto i8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i16", "@i16", TypeObj{}, {}, true}));
  auto i16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i32", "@i32", TypeObj{}, {}, true}));
  auto i32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i64", "@i64", TypeObj{}, {}, true}));
  auto i64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"int", "@int", TypeObj{}, {}, true}));
  auto intType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"str", "@str", TypeObj{}, {}, true}));
  auto strType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u8", "@u8", TypeObj{}, {}, true}));
  auto u8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u16", "@u16", TypeObj{}, {}, true}));
  auto u16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u32", "@u32", TypeObj{}, {}, true}));
  auto u32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u64", "@u64", TypeObj{}, {}, true}));
  auto u64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"void", "@void", TypeObj{}, {}, true}));
  auto voidType = this->_items.back().get();

  this->_items.push_back(std::make_unique<Type>(Type{"any.str", "@any.str", TypeFn{strType}, {}, true}));
  anyType->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"bool.str", "@bool.str", TypeFn{strType}, {}, true}));
  boolType->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"byte.str", "@byte.str", TypeFn{strType}, {}, true}));
  byteType->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"char.str", "@char.str", TypeFn{strType}, {}, true}));
  charType->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"f32.str", "@f32.str", TypeFn{strType}, {}, true}));
  f32Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"f64.str", "@f64.str", TypeFn{strType}, {}, true}));
  f64Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"float.str", "@float.str", TypeFn{strType}, {}, true}));
  floatType->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"i8.str", "@i8.str", TypeFn{strType}, {}, true}));
  i8Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"i16.str", "@i16.str", TypeFn{strType}, {}, true}));
  i16Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"i32.str", "@i32.str", TypeFn{strType}, {}, true}));
  i32Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"i64.str", "@i64.str", TypeFn{strType}, {}, true}));
  i64Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"int.str", "@int.str", TypeFn{strType}, {}, true}));
  intType->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  strType->fields.push_back(TypeField{"len", intType, true});

  auto sliceStrTypeFn = TypeFn{strType, {
    TypeFnParam{"start", i64Type, false, false, false},
    TypeFnParam{"end", i64Type, false, false, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"str.slice", "@str.slice", sliceStrTypeFn, {}, true}));
  strType->fields.push_back(TypeField{"slice", this->_items.back().get(), true});

  this->_items.push_back(std::make_unique<Type>(Type{"u8.str", "@u8.str", TypeFn{strType}, {}, true}));
  u8Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"u16.str", "@u16.str", TypeFn{strType}, {}, true}));
  u16Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"u32.str", "@u32.str", TypeFn{strType}, {}, true}));
  u32Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});
  this->_items.push_back(std::make_unique<Type>(Type{"u64.str", "@u64.str", TypeFn{strType}, {}, true}));
  u64Type->fields.push_back(TypeField{"str", this->_items.back().get(), true});

  auto exitTypeFn = TypeFn{voidType, {
    TypeFnParam{"status", intType, false, false, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"exit", "@exit", exitTypeFn, {}, true}));

  auto printTypeFn = TypeFn{voidType, {
    TypeFnParam{"items", anyType, false, false, true},
    TypeFnParam{"separator", strType, false, false, false},
    TypeFnParam{"terminator", strType, false, false, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"print", "@print", printTypeFn, {}, true}));

  auto sleepTypeFn = TypeFn{voidType, {
    TypeFnParam{"milliseconds", u64Type, false, true, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"sleep", "@sleep", sleepTypeFn, {}, true}));
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

Type *TypeMap::obj (const std::string &name, const std::string &codeName, const std::vector<TypeField> &fields) {
  this->_items.push_back(std::make_unique<Type>(Type{name, codeName, TypeObj{}, fields}));
  auto self = this->_items.back().get();

  auto strTypeFn = TypeFn{this->get("str")};

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@opt.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), true});

  return self;
}

Type *TypeMap::opt (Type *type) {
  for (const auto &item : this->_items) {
    if (item->isOpt() && std::get<TypeOptional>(item->body).type->matchExact(type)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"opt_" + type->name, "@opt_" + type->name, TypeOptional{type}}));
  auto self = this->_items.back().get();

  auto strTypeFn = TypeFn{this->get("str")};

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@opt.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), true});

  return self;
}

Type *TypeMap::ref (Type *refType) {
  for (const auto &item : this->_items) {
    if (item->isRef() && std::get<TypeRef>(item->body).refType->matchExact(refType)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"ref_" + refType->name, "@ref_" + refType->name, TypeRef{refType}}));
  return this->_items.back().get();
}
