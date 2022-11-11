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

#include "Type.hpp"
#include <algorithm>
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

Type *Type::real (Type *type) {
  return type->isRef() ? std::get<TypeRef>(type->body).refType : type;
}

Type *Type::largest (Type *a, Type *b) {
  if (!a->isNumber() || !b->isNumber()) {
    throw Error("tried to find largest type of non-number");
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

Type *Type::getProp (const std::string &propName) const {
  if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->getProp(propName);
  }

  auto typeField = std::find_if(this->fields.begin(), this->fields.end(), [&propName] (const auto &it) -> bool {
    return it.name == propName;
  });

  if (typeField == this->fields.end()) {
    throw Error("tried to get non-existing prop type");
  }

  return typeField->type;
}

bool Type::hasProp (const std::string &propName) const {
  if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->hasProp(propName);
  }

  auto typeField = std::find_if(this->fields.begin(), this->fields.end(), [&propName] (const auto &it) -> bool {
    return it.name == propName;
  });

  return typeField != this->fields.end();
}

bool Type::isAny () const {
  return this->name == "any";
}

bool Type::isArray () const {
  return std::holds_alternative<TypeArray>(this->body);
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

bool Type::isIntNumber () const {
  return (
    this->isI8() || this->isI16() || this->isI32() || this->isInt() || this->isI64() ||
    this->isU8() || this->isU16() || this->isU32() || this->isU64()
  );
}

bool Type::isNumber () const {
  return this->isIntNumber() || this->isFloatNumber();
}

bool Type::isObj () const {
  return (
    !this->isAny() &&
    !this->isArray() &&
    !this->isBool() &&
    !this->isByte() &&
    !this->isChar() &&
    !this->isNumber() &&
    !this->isFn() &&
    !this->isOpt() &&
    !this->isRef() &&
    !this->isStr() &&
    !this->isVoid()
  );
}

bool Type::isOpt () const {
  return std::holds_alternative<TypeOptional>(this->body);
}

bool Type::isRef () const {
  return std::holds_alternative<TypeRef>(this->body);
}

bool Type::isRefExt () const {
  if (this->isOpt()) {
    return std::get<TypeOptional>(this->body).type->isRef();
  }

  return std::holds_alternative<TypeRef>(this->body);
}

bool Type::isSmallForVarArg () const {
  return
    this->isBool() ||
    this->isByte() ||
    this->isChar() ||
    this->isF32() ||
    this->isI8() ||
    this->isI16() ||
    this->isU8() ||
    this->isU16();
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
  if (this->name == "any") {
    return true;
  } else if (this->isRef() || type->isRef()) {
    if (this->isRef() && type->isRef()) {
      auto lhsRef = std::get<TypeRef>(this->body);
      auto rhsRef = std::get<TypeRef>(type->body);

      return lhsRef.refType->match(rhsRef.refType);
    } else if (this->isRef() && !type->isRef()) {
      auto lhsRef = std::get<TypeRef>(this->body);
      return lhsRef.refType->match(type);
    } else {
      auto lhsRef = std::get<TypeRef>(type->body);
      return lhsRef.refType->match(this);
    }
  } else if (this->isArray() || type->isArray()) {
    if (!this->isArray() || !type->isArray()) {
      return false;
    }

    auto lhsArray = std::get<TypeArray>(this->body);
    auto rhsArray = std::get<TypeArray>(type->body);

    return lhsArray.elementType->match(rhsArray.elementType);
  } else if (this->isFn()) {
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
        (lhsFnParam.name != std::nullopt && lhsFnParam.name != rhsFnParam.name) ||
        !lhsFnParam.type->match(rhsFnParam.type) ||
        (lhsFnParam.type->isRef() && lhsFnParam.mut != rhsFnParam.mut) ||
        lhsFnParam.required != rhsFnParam.required ||
        lhsFnParam.variadic != rhsFnParam.variadic
      ) {
        return false;
      }
    }

    return true;
  } else if (this->isObj()) {
    return type->isObj() && this->name == type->name;
  } else if (this->isOpt()) {
    if (!type->isOpt()) {
      auto lhsOptional = std::get<TypeOptional>(this->body);
      return lhsOptional.type->match(type);
    }

    auto lhsOptional = std::get<TypeOptional>(this->body);
    auto rhsOptional = std::get<TypeOptional>(type->body);

    return lhsOptional.type->match(rhsOptional.type);
  }

  return (this->name == "bool" && type->name == "bool") ||
    (this->name == "byte" && (type->name == "byte" || type->isIntNumber())) ||
    (type->name == "byte" && (this->name == "byte" || this->isIntNumber())) ||
    (this->name == "char" && type->name == "char") ||
    (this->name == "str" && type->name == "str") ||
    (this->name == "void" && type->name == "void") ||
    numberTypeMatch(this->name, type->name);
}

bool Type::matchExact (const Type *type) const {
  if (this->isArray() || type->isArray()) {
    if (!this->isArray() || !type->isArray()) {
      return false;
    }

    auto lhsArray = std::get<TypeArray>(this->body);
    auto rhsArray = std::get<TypeArray>(type->body);

    return lhsArray.elementType->matchExact(rhsArray.elementType);
  } else if (this->isFn() || type->isFn()) {
    if (!this->isFn() || !type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (!lhsFn.returnType->matchExact(rhsFn.returnType) || lhsFn.params.size() != rhsFn.params.size()) {
      return false;
    }

    for (auto i = static_cast<std::size_t>(0); i < lhsFn.params.size(); i++) {
      auto lhsFnParam = lhsFn.params[i];
      auto rhsFnParam = rhsFn.params[i];

      if (
        lhsFnParam.name != rhsFnParam.name ||
        !lhsFnParam.type->matchExact(rhsFnParam.type) ||
        lhsFnParam.mut != rhsFnParam.mut ||
        lhsFnParam.required != rhsFnParam.required ||
        lhsFnParam.variadic != rhsFnParam.variadic
      ) {
        return false;
      }
    }

    return true;
  } else if (this->isOpt() || type->isOpt()) {
    if (!this->isOpt() || !type->isOpt()) {
      return false;
    }

    auto lhsOptional = std::get<TypeOptional>(this->body);
    auto rhsOptional = std::get<TypeOptional>(type->body);

    return lhsOptional.type->matchExact(rhsOptional.type);
  } else if (this->isRef() || type->isRef()) {
    if (!this->isRef() || !type->isRef()) {
      return false;
    }

    auto lhsRef = std::get<TypeRef>(this->body);
    auto rhsRef = std::get<TypeRef>(type->body);

    return lhsRef.refType->matchExact(rhsRef.refType);
  }

  return this->name == type->name;
}

bool Type::matchNice (const Type *type) const {
  if (this->isArray() || type->isArray()) {
    if (!this->isArray() || !type->isArray()) {
      return false;
    }

    auto lhsArray = std::get<TypeArray>(this->body);
    auto rhsArray = std::get<TypeArray>(type->body);

    return lhsArray.elementType->matchNice(rhsArray.elementType);
  } else if (this->isFn() || type->isFn()) {
    if (!this->isFn() || !type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (!lhsFn.returnType->matchNice(rhsFn.returnType) || lhsFn.params.size() != rhsFn.params.size()) {
      return false;
    }

    for (auto i = static_cast<std::size_t>(0); i < lhsFn.params.size(); i++) {
      auto lhsFnParam = lhsFn.params[i];
      auto rhsFnParam = rhsFn.params[i];

      if (
        (lhsFnParam.name != std::nullopt && rhsFnParam.name != std::nullopt && lhsFnParam.name != rhsFnParam.name) ||
        !lhsFnParam.type->matchNice(rhsFnParam.type) ||
        (lhsFnParam.mut && lhsFnParam.mut != rhsFnParam.mut) ||
        lhsFnParam.required != rhsFnParam.required ||
        lhsFnParam.variadic != rhsFnParam.variadic
      ) {
        return false;
      }
    }

    return true;
  } else if (this->isOpt() || type->isOpt()) {
    if (!this->isOpt() || !type->isOpt()) {
      return false;
    }

    auto lhsOptional = std::get<TypeOptional>(this->body);
    auto rhsOptional = std::get<TypeOptional>(type->body);

    return lhsOptional.type->matchNice(rhsOptional.type);
  } else if (this->isRef() || type->isRef()) {
    if (!this->isRef() || !type->isRef()) {
      return false;
    }

    auto lhsRef = std::get<TypeRef>(this->body);
    auto rhsRef = std::get<TypeRef>(type->body);

    return lhsRef.refType->matchNice(rhsRef.refType);
  }

  return this->name == type->name;
}

bool Type::shouldBeFreed () const {
  return this->isAny() || this->isArray() || this->isFn() || this->isObj() || this->isOpt() || this->isStr();
}

std::string Type::xml (std::size_t indent) const {
  if (this->builtin) {
    return std::string(indent, ' ') + R"(<BuiltinType name=")" + this->name + R"(" />)";
  }

  auto typeName = std::string("Type");

  if (this->isArray()) {
    typeName += "Array";
  } else if (this->isFn()) {
    typeName += "Fn";
  } else if (this->isObj()) {
    typeName += "Obj";
  } else if (this->isOpt()) {
    typeName += "Optional";
  } else if (this->isRef()) {
    typeName += "Ref";
  }

  auto result = std::string(indent, ' ') + "<" + typeName;

  result += this->codeName[0] == '@' ? "" : R"( codeName=")" + this->codeName + R"(")";
  result += this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")";
  result += ">" EOL;

  if (this->isArray()) {
    auto typeArray = std::get<TypeArray>(this->body);
    result += typeArray.elementType->xml(indent + 2) + EOL;
  } else if (this->isFn()) {
    auto typeFn = std::get<TypeFn>(this->body);

    if (!typeFn.params.empty()) {
      result += std::string(indent + 2, ' ') + "<TypeFnParams>" EOL;

      for (const auto &typeFnParam : typeFn.params) {
        auto paramAttrs = typeFnParam.name == std::nullopt ? "" : R"( name=")" + *typeFnParam.name + R"(")";

        paramAttrs += typeFnParam.mut ? " mut" : "";
        paramAttrs += typeFnParam.required ? " required" : "";
        paramAttrs += typeFnParam.variadic ? " variadic" : "";

        result += std::string(indent + 4, ' ') + "<TypeFnParam" + paramAttrs + ">" EOL;
        result += typeFnParam.type->xml(indent + 6) + EOL;
        result += std::string(indent + 4, ' ') + "</TypeFnParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</TypeFnParams>" EOL;
    }

    result += std::string(indent + 2, ' ') + "<TypeFnReturnType>" EOL;
    result += typeFn.returnType->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeFnReturnType>" EOL;
  } else if (this->isObj()) {
    for (const auto &typeField : this->fields) {
      if (typeField.builtin) {
        continue;
      }

      auto fieldAttrs = std::string();

      fieldAttrs += typeField.mut ? " mut" : "";
      fieldAttrs += R"( name=")" + typeField.name + R"(")";

      result += std::string(indent + 2, ' ') + "<TypeField" + fieldAttrs + ">" EOL;
      result += typeField.type->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</TypeField>" EOL;
    }
  } else if (this->isOpt()) {
    auto typeOptional = std::get<TypeOptional>(this->body);
    result += typeOptional.type->xml(indent + 2) + EOL;
  } else if (this->isRef()) {
    auto typeRef = std::get<TypeRef>(this->body);
    result += typeRef.refType->xml(indent + 2) + EOL;
  }

  return result + std::string(indent, ' ') + "</" + typeName + ">";
}
