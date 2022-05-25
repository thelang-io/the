/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Type.hpp"
#include "Error.hpp"
#include "Var.hpp"
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

Type *Type::largest (Type *a, Type *b) {
  if (!a->isNumber() || !b->isNumber()) {
    throw Error("Error: tried to find largest type of non-number");
  }

  return (
    ((a->isF64() || a->isFloat()) && (b->isF64() || b->isFloat())) ||
    ((a->isF64() || a->isFloat()) && b->isF32()) ||
    ((a->isF64() || a->isFloat()) && b->isU32()) ||
    ((a->isF64() || a->isFloat()) && b->isU16()) ||
    ((a->isF64() || a->isFloat()) && b->isU8()) ||
    ((a->isF64() || a->isFloat()) && b->isI64()) ||
    ((a->isF64() || a->isFloat()) && (b->isI32() || b->isInt())) ||
    ((a->isF64() || a->isFloat()) && b->isI16()) ||
    ((a->isF64() || a->isFloat()) && b->isI8()) ||

    (a->isF32() && b->isF32()) ||
    (a->isF32() && b->isU16()) ||
    (a->isF32() && b->isU8()) ||
    (a->isF32() && (b->isI32() || b->isInt())) ||
    (a->isF32() && b->isI16()) ||
    (a->isF32() && b->isI8()) ||

    (a->isI64() && b->isU32()) ||
    (a->isI64() && b->isU16()) ||
    (a->isI64() && b->isU8()) ||
    (a->isI64() && b->isI64()) ||
    (a->isI64() && (b->isI32() || b->isInt())) ||
    (a->isI64() && b->isI16()) ||
    (a->isI64() && b->isI8()) ||
    (a->isU64() && b->isU64()) ||
    (a->isU64() && b->isU32()) ||
    (a->isU64() && b->isU16()) ||
    (a->isU64() && b->isU8()) ||

    ((a->isI32() || a->isInt()) && b->isU16()) ||
    ((a->isI32() || a->isInt()) && b->isU8()) ||
    ((a->isI32() || a->isInt()) && (b->isI32() || b->isInt())) ||
    ((a->isI32() || a->isInt()) && b->isI16()) ||
    ((a->isI32() || a->isInt()) && b->isI8()) ||
    (a->isU32() && b->isU32()) ||
    (a->isU32() && b->isU16()) ||
    (a->isU32() && b->isU8()) ||

    (a->isI16() && b->isU8()) ||
    (a->isI16() && b->isI16()) ||
    (a->isI16() && b->isI8()) ||
    (a->isU16() && b->isU16()) ||
    (a->isU16() && b->isU8()) ||

    (a->isI8() && b->isI8()) ||
    (a->isU8() && b->isU8())
  ) ? a : b;
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

bool Type::isFloatNumber () const {
  return this->isF32() || this->isF64() || this->isFloat();
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

bool Type::isNumber () const {
  return (
    this->isI8() || this->isI16() || this->isI32() || this->isInt() || this->isI64() ||
    this->isU8() || this->isU16() || this->isU32() || this->isU64() ||
    this->isFloatNumber()
  );
}

bool Type::isObj () const {
  return (
    !this->isAny() &&
    !this->isBool() &&
    !this->isByte() &&
    !this->isChar() &&
    !this->isNumber() &&
    !this->isFn() &&
    !this->isStr() &&
    !this->isVoid()
  );
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
        !lhsFnParam.var->type->match(rhsFnParam.var->type) ||
        lhsFnParam.required != rhsFnParam.required ||
        lhsFnParam.variadic != rhsFnParam.variadic
      ) {
        return false;
      }
    }

    return true;
  } else if (this->isObj()) {
    return type->isObj() && this->name == type->name;
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
    return std::string(indent, ' ') + "<BuiltinType" + (this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")") + " />";
  }

  auto result = std::string(indent, ' ') + "<Type";

  result += this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")";
  result += R"( type=")" + std::string(std::holds_alternative<TypeFn>(this->body) ? "fn" : "obj") + R"(">)" EOL;

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
        result += R"( required=")" + std::string(typeFnParam.required ? "true" : "false");
        result += R"(" variadic=")" + std::string(typeFnParam.variadic ? "true" : "false") + R"(">)" EOL;
        result += typeFnParam.var->xml(indent + 4) + EOL;
        result += std::string(indent + 2, ' ') + "</TypeFnParam>" EOL;
      }

      result += std::string(indent, ' ') + "</slot>" EOL;
    }

    if (!typeFn.stack.empty()) {
      result += std::string(indent, ' ') + R"(<slot name="stack">)" EOL;

      for (const auto &typeFnStackVar : typeFn.stack) {
        result += typeFnStackVar->xml(indent + 2) + EOL;
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
