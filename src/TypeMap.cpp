/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "TypeMap.hpp"
#include <climits>
#include "Error.hpp"

bool numberTypeMatch (const std::string &src, const std::string &dest) {
  return (src == "i8" && dest == "i8") ||
    (src == "i16" && (dest == "i16" || dest == "u8" || numberTypeMatch("i8", dest))) ||
    ((src == "i32" || src == "int") && (dest == "i32" || dest == "int" || dest == "u16" || numberTypeMatch("i16", dest))) ||
    (src == "i64" && (dest == "i64" || dest == "u32" || numberTypeMatch("i32", dest))) ||
    (src == "u8" && dest == "u8") ||
    (src == "u16" && (dest == "u16" || numberTypeMatch("u8", dest))) ||
    (src == "u32" && (dest == "u32" || numberTypeMatch("u16", dest))) ||
    (src == "u64" && (dest == "u64" || numberTypeMatch("u32", dest))) ||
    (src == "f32" && (dest == "f32" || numberTypeMatch("i32", dest))) ||
    ((src == "f64" || src == "float") && (dest == "f32" || dest == "f64" || dest == "float" || numberTypeMatch("i64", dest)));
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

bool Type::isFloat () const {
  return this->name == "float";
}

bool Type::isFn () const {
  return std::holds_alternative<TypeFn>(this->body);
}

bool Type::isF32 () const {
  return this->name == "f32";
}

bool Type::isF64 () const {
  return this->name == "f64";
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
  return !this->isAny() &&
    !this->isBool() &&
    !this->isByte() &&
    !this->isChar() &&
    !this->isFloat() &&
    !this->isFn() &&
    !this->isF32() &&
    !this->isF64() &&
    !this->isInt() &&
    !this->isI8() &&
    !this->isI16() &&
    !this->isI32() &&
    !this->isI64() &&
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

bool Type::match (const Type &type) const {
  if (this->isObj() && type.isObj()) {
    return this->name == type.name;
  }

  return (this->name == "any") ||
    (this->name == "bool" && type.name == "bool") ||
    (this->name == "byte" && type.name == "byte") ||
    (this->name == "char" && type.name == "char") ||
    (this->name == "str" && (type.name == "ch" || type.name == "str")) ||
    numberTypeMatch(this->name, type.name);
}

std::string Type::xml (std::size_t indent) const {
  if (this->builtin) {
    auto nameAttr = this->name == "@" ? "" : R"( name=")" + this->name + R"(")";
    return std::string(indent, ' ') + "<BuiltinType" + nameAttr + " />";
  }

  auto result = std::string(indent, ' ') + R"(<Type builtin=")" + std::string(this->builtin ? "true" : "false");

  result += R"(" name=")" + this->name;
  result += R"(" type=")" + std::string(std::holds_alternative<TypeFn>(this->body) ? "fn" : "obj") + R"(">)" "\n";

  indent += 2;

  if (std::holds_alternative<TypeFn>(this->body)) {
    auto typeFn = std::get<TypeFn>(this->body);

    result += std::string(indent, ' ') + R"(<slot name="returnType">)" "\n";
    result += typeFn.returnType->xml(indent + 2) + "\n";
    result += std::string(indent, ' ') + "</slot>\n";

    if (!typeFn.params.empty()) {
      result += std::string(indent, ' ') + R"(<slot name="params">)" "\n";

      for (const auto &typeFnParam : typeFn.params) {
        result += std::string(indent + 2, ' ') + R"(<TypeFnParam name=")" + typeFnParam.name;
        result += R"(" required=")" + std::string(typeFnParam.required ? "true" : "false");
        result += R"(" variadic=")" + std::string(typeFnParam.variadic ? "true" : "false") + R"(">)" "\n";
        result += typeFnParam.type->xml(indent + 4) + "\n";
        result += std::string(indent + 2, ' ') + "</TypeFnParam>\n";
      }

      result += std::string(indent, ' ') + "</slot>\n";
    }
  } else {
    auto typeObj = std::get<TypeObj>(this->body);

    if (!typeObj.fields.empty()) {
      result += std::string(indent, ' ') + R"(<slot name="fields">)" "\n";

      for (const auto &typeObjField : typeObj.fields) {
        result += std::string(indent + 2, ' ') + R"(<TypeObjField name=")" + typeObjField.name + R"(">)" "\n";
        result += typeObjField.type->xml(indent + 4) + "\n";
        result += std::string(indent + 2, ' ') + "</TypeObjField>\n";
      }

      result += std::string(indent, ' ') + "</slot>\n";
    }
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Type>";
}

std::shared_ptr<Type> TypeMap::fn (const std::shared_ptr<Type> &returnType, const std::vector<TypeFnParam> &params) {
  auto typeFn = TypeFn{returnType, params};
  return std::make_shared<Type>(Type{"@", typeFn, false});
}

std::shared_ptr<Type> TypeMap::add (const std::string &name, const std::vector<TypeObjField> &fields) {
  auto obj = TypeObj{fields};

  this->_items.push_back(std::make_shared<Type>(Type{name, obj, false}));
  return this->_items.back();
}

std::shared_ptr<Type> TypeMap::add (const std::string &name, const std::vector<TypeFnParam> &params, const std::shared_ptr<Type> &returnType) {
  auto fn = TypeFn{returnType, params};

  this->_items.push_back(std::make_shared<Type>(Type{name, fn, false}));
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

void TypeMap::init () {
  this->stack.reserve(SHRT_MAX);

  auto anyType = std::make_shared<Type>(Type{"any", TypeObj{}, true});
  auto boolType = std::make_shared<Type>(Type{"bool", TypeObj{}, true});
  auto byteType = std::make_shared<Type>(Type{"byte", TypeObj{}, true});
  auto charType = std::make_shared<Type>(Type{"char", TypeObj{}, true});
  auto floatType = std::make_shared<Type>(Type{"float", TypeObj{}, true});
  auto f32Type = std::make_shared<Type>(Type{"f32", TypeObj{}, true});
  auto f64Type = std::make_shared<Type>(Type{"f64", TypeObj{}, true});
  auto intType = std::make_shared<Type>(Type{"int", TypeObj{}, true});
  auto i8Type = std::make_shared<Type>(Type{"i8", TypeObj{}, true});
  auto i16Type = std::make_shared<Type>(Type{"i16", TypeObj{}, true});
  auto i32Type = std::make_shared<Type>(Type{"i32", TypeObj{}, true});
  auto i64Type = std::make_shared<Type>(Type{"i64", TypeObj{}, true});
  auto strType = std::make_shared<Type>(Type{"str", TypeObj{}, true});
  auto u8Type = std::make_shared<Type>(Type{"u8", TypeObj{}, true});
  auto u16Type = std::make_shared<Type>(Type{"u16", TypeObj{}, true});
  auto u32Type = std::make_shared<Type>(Type{"u32", TypeObj{}, true});
  auto u64Type = std::make_shared<Type>(Type{"u64", TypeObj{}, true});
  auto voidType = std::make_shared<Type>(Type{"void", TypeObj{}, true});

  auto strTypeMethodConcat = TypeFn{strType, {
    {"src", strType, true, false}
  }};

  std::get<TypeObj>(strType->body).fields.push_back({"len", intType});
  std::get<TypeObj>(strType->body).fields.push_back({"concat", std::make_shared<Type>(Type{"@", strTypeMethodConcat, true})});

  std::get<TypeObj>(boolType->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(byteType->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(charType->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(floatType->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(f32Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(f64Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(intType->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(i8Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(i16Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(i32Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(i64Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(u8Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(u16Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(u32Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});
  std::get<TypeObj>(u64Type->body).fields.push_back({"str", std::make_shared<Type>(Type{"@", TypeFn{strType}, true})});

  this->_items.push_back(anyType);
  this->_items.push_back(boolType);
  this->_items.push_back(byteType);
  this->_items.push_back(charType);
  this->_items.push_back(floatType);
  this->_items.push_back(f32Type);
  this->_items.push_back(f64Type);
  this->_items.push_back(intType);
  this->_items.push_back(i8Type);
  this->_items.push_back(i16Type);
  this->_items.push_back(i32Type);
  this->_items.push_back(i64Type);
  this->_items.push_back(strType);
  this->_items.push_back(u8Type);
  this->_items.push_back(u16Type);
  this->_items.push_back(u32Type);
  this->_items.push_back(u64Type);
  this->_items.push_back(voidType);
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
