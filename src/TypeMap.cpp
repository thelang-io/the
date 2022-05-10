/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "TypeMap.hpp"
#include <limits>
#include "Error.hpp"
#include "config.hpp"

bool numberTypeMatch (const std::string &lhs, const std::string &rhs) {
  return (lhs == "i8" && rhs == "i8") ||
    (lhs == "i16" && (rhs == "i16" || rhs == "u8" || numberTypeMatch("i8", rhs))) ||
    ((lhs == "i32" || lhs == "int") && (rhs == "i32" || rhs == "int" || rhs == "u16" || numberTypeMatch("i16", rhs))) ||
    (lhs == "i64" && (rhs == "i64" || rhs == "u32" || numberTypeMatch("i32", rhs))) ||
    (lhs == "u8" && rhs == "u8") ||
    (lhs == "u16" && (rhs == "u16" || numberTypeMatch("u8", rhs))) ||
    (lhs == "u32" && (rhs == "u32" || numberTypeMatch("u16", rhs))) ||
    (lhs == "u64" && (rhs == "u64" || numberTypeMatch("u32", rhs))) ||
    (lhs == "f32" && (rhs == "f32" || numberTypeMatch("i32", rhs))) ||
    ((lhs == "f64" || lhs == "float") && (rhs == "f32" || rhs == "f64" || rhs == "float" || numberTypeMatch("i64", rhs)));
}

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

bool Type::isF32 () const {
  return this->name == "f32";
}

bool Type::isF64 () const {
  return this->name == "f64";
}

bool Type::isFloat () const {
  return this->name == "float";
}

bool Type::isFn () const {
  return std::holds_alternative<TypeFn>(this->body);
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

bool Type::isInt () const {
  return this->name == "int";
}

bool Type::isObj () const {
  return !this->isAny() &&
    !this->isBool() &&
    !this->isByte() &&
    !this->isChar() &&
    !this->isF32() &&
    !this->isF64() &&
    !this->isFloat() &&
    !this->isFn() &&
    !this->isI8() &&
    !this->isI16() &&
    !this->isI32() &&
    !this->isI64() &&
    !this->isInt() &&
    !this->isStr() &&
    !this->isU8() &&
    !this->isU16() &&
    !this->isU32() &&
    !this->isU64() &&
    !this->isVoid();
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

bool Type::match (const Type *type) const {
  if (this->isFn()) {
    if (!type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (!lhsFn.returnType->match(rhsFn.returnType) || lhsFn.params.size() != rhsFn.params.size()) {
      return false;
    }

    for (auto i = static_cast<std::size_t>(0); i < lhsFn.params.size(); i++) {
      auto lhsFnParam = lhsFn.params[i];
      auto rhsFnParam = rhsFn.params[i];

      if (
        !lhsFnParam.type->match(rhsFnParam.type) ||
        lhsFnParam.required != rhsFnParam.required ||
        lhsFnParam.variadic != rhsFnParam.variadic
      ) {
        return false;
      }
    }

    return true;
  } else if (this->isObj()) {
    if (!type->isObj()) {
      return false;
    }

    return this->name == type->name;
  }

  return (this->name == "any") ||
    (this->name == "bool" && type->name == "bool") ||
    (this->name == "byte" && type->name == "byte") ||
    (this->name == "char" && type->name == "char") ||
    (this->name == "void" && type->name == "void") ||
    (this->name == "str" && type->name == "str") ||
    numberTypeMatch(this->name, type->name);
}

std::string Type::xml (std::size_t indent) const {
  if (this->builtin) {
    auto nameAttr = this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")";
    return std::string(indent, ' ') + "<BuiltinType" + nameAttr + " />";
  }

  auto result = std::string(indent, ' ') + R"(<Type builtin=")" + std::string(this->builtin ? "true" : "false");

  if (this->name != "@") {
    result += R"(" name=")" + this->name;
  }

  result += R"(" type=")" + std::string(std::holds_alternative<TypeFn>(this->body) ? "fn" : "obj") + R"(">)" EOL;
  indent += 2;

  if (std::holds_alternative<TypeFn>(this->body)) {
    auto typeFn = std::get<TypeFn>(this->body);

    result += std::string(indent, ' ') + R"(<slot name="returnType">)" EOL;
    result += typeFn.returnType->xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</slot>" EOL;

    if (!typeFn.params.empty()) {
      result += std::string(indent, ' ') + R"(<slot name="params">)" EOL;

      for (const auto &typeFnParam : typeFn.params) {
        result += std::string(indent + 2, ' ') + "<TypeFnParam";

        if (this->name != "@") {
          result += R"( name=")" + typeFnParam.name + R"(")";
        }

        result += R"( required=")" + std::string(typeFnParam.required ? "true" : "false");
        result += R"(" variadic=")" + std::string(typeFnParam.variadic ? "true" : "false") + R"(">)" EOL;
        result += typeFnParam.type->xml(indent + 4) + EOL;
        result += std::string(indent + 2, ' ') + "</TypeFnParam>" EOL;
      }

      result += std::string(indent, ' ') + "</slot>" EOL;
    }
  } else {
    auto typeObj = std::get<TypeObj>(this->body);

    if (!typeObj.fields.empty()) {
      result += std::string(indent, ' ') + R"(<slot name="fields">)" EOL;

      for (const auto &typeObjField : typeObj.fields) {
        result += std::string(indent + 2, ' ') + R"(<TypeObjField name=")" + typeObjField.name + R"(">)" EOL;
        result += typeObjField.type->xml(indent + 4) + EOL;
        result += std::string(indent + 2, ' ') + "</TypeObjField>" EOL;
      }

      result += std::string(indent, ' ') + "</slot>" EOL;
    }
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Type>";
}

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

  std::get<TypeObj>(strType->body).fields.push_back({"len", intType});

  this->_items.push_back(std::make_unique<Type>(Type{"@str.concat", TypeFn{strType, {
    {"src", strType, true, false}
  }}, true}));

  std::get<TypeObj>(strType->body).fields.push_back({"concat", this->_items.back().get()});

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
  this->_items.push_back(std::make_unique<Type>(Type{"@u8.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u8Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@u16.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u16Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@u32.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u32Type->body).fields.push_back({"str", this->_items.back().get()});
  this->_items.push_back(std::make_unique<Type>(Type{"@u64.str", TypeFn{strType}, true}));
  std::get<TypeObj>(u64Type->body).fields.push_back({"str", this->_items.back().get()});

  auto printFnTypeFn = TypeFn{voidType, {
    TypeFnParam{"items", anyType, false, true},
    TypeFnParam{"separator", strType, false, false},
    TypeFnParam{"terminator", strType, false, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"@print", printFnTypeFn, true}));
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
