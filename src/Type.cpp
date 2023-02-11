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

Type *Type::actual (Type *type) {
  if (type->isAlias()) {
    return Type::actual(std::get<TypeAlias>(type->body).type);
  }

  return type;
}

Type *Type::real (Type *type) {
  if (type->isAlias()) {
    return Type::real(std::get<TypeAlias>(type->body).type);
  } else if (type->isRef()) {
    return Type::real(std::get<TypeRef>(type->body).refType);
  } else {
    return type;
  }
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

Type *Type::getEnumerator (const std::string &memberName) const {
  if (!this->isEnum()) {
    throw Error("tried to get a member of non-enum");
  }

  auto typeEnum = std::get<TypeEnum>(this->body);

  auto typeMember = std::find_if(typeEnum.members.begin(), typeEnum.members.end(), [&memberName] (const auto &it) -> bool {
    return it->name == memberName;
  });

  if (typeMember == typeEnum.members.end()) {
    throw Error("tried to get non-existing enum member");
  }

  return *typeMember;
}

Type *Type::getProp (const std::string &propName) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->getProp(propName);
  } else if (this->isRef()) {
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

bool Type::hasEnumerator (const std::string &memberName) const {
  if (!this->isEnum()) {
    return false;
  }

  auto typeEnum = std::get<TypeEnum>(this->body);

  auto typeMember = std::find_if(typeEnum.members.begin(), typeEnum.members.end(), [&memberName] (const auto &it) -> bool {
    return it->name == memberName;
  });

  return typeMember != typeEnum.members.end();
}

bool Type::hasProp (const std::string &propName) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->hasProp(propName);
  } else if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->hasProp(propName);
  }

  auto typeField = std::find_if(this->fields.begin(), this->fields.end(), [&propName] (const auto &it) -> bool {
    return it.name == propName;
  });

  return typeField != this->fields.end();
}

bool Type::hasSubType (const Type *subType) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->hasSubType(subType);
  } else if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->hasSubType(subType);
  } else if (this->isUnion()) {
    auto typeUnion = std::get<TypeUnion>(this->body);

    for (const auto &thisSubType : typeUnion.subTypes) {
      if (thisSubType == subType) {
        return true;
      }
    }
  }

  return false;
}

bool Type::isAlias () const {
  return std::holds_alternative<TypeAlias>(this->body);
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

bool Type::isEnum () const {
  return std::holds_alternative<TypeEnum>(this->body);
}

bool Type::isEnumerator () const {
  return std::holds_alternative<TypeEnumerator>(this->body);
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

bool Type::isMethod () const {
  return std::holds_alternative<TypeFn>(this->body) && std::get<TypeFn>(this->body).isMethod;
}

bool Type::isNumber () const {
  return this->isIntNumber() || this->isFloatNumber();
}

bool Type::isObj () const {
  return (
    !this->isAlias() &&
    !this->isAny() &&
    !this->isArray() &&
    !this->isBool() &&
    !this->isByte() &&
    !this->isChar() &&
    !this->isEnum() &&
    !this->isNumber() &&
    !this->isFn() &&
    !this->isOpt() &&
    !this->isRef() &&
    !this->isStr() &&
    !this->isUnion() &&
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

bool Type::isRefOf (const Type *type) const {
  if (this->isOpt()) {
    return std::get<TypeOptional>(this->body).type->isRefOf(type);
  } else if (!this->isRef()) {
    return false;
  }

  return std::get<TypeRef>(this->body).refType->matchStrict(type, true);
}

bool Type::isSafeForTernaryAlt () const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->isSafeForTernaryAlt();
  }

  return this->isAny() ||
    this->isArray() ||
    this->isObj() ||
    this->isOpt() ||
    this->isStr() ||
    this->isUnion();
}

bool Type::isSmallForVarArg () const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->isSmallForVarArg();
  }

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

bool Type::isUnion () const {
  return std::holds_alternative<TypeUnion>(this->body);
}

bool Type::isVoid () const {
  return this->name == "void";
}

bool Type::matchNice (const Type *type) const {
  if (this->isAny()) {
    return true;
  } else if (this->isAlias() || type->isAlias()) {
    if (this->isAlias() && type->isAlias()) {
      auto lhsAlias = std::get<TypeAlias>(this->body);
      auto rhsAlias = std::get<TypeAlias>(type->body);

      return lhsAlias.type->matchNice(rhsAlias.type);
    } else if (this->isAlias()) {
      auto lhsAlias = std::get<TypeAlias>(this->body);
      return lhsAlias.type->matchNice(type);
    } else {
      auto lhsAlias = std::get<TypeAlias>(type->body);
      return lhsAlias.type->matchNice(this);
    }
  } else if (this->isRef() || type->isRef()) {
    if (this->isRef() && type->isRef()) {
      auto lhsRef = std::get<TypeRef>(this->body);
      auto rhsRef = std::get<TypeRef>(type->body);

      return lhsRef.refType->matchNice(rhsRef.refType);
    } else if (this->isRef()) {
      auto lhsRef = std::get<TypeRef>(this->body);
      return lhsRef.refType->matchNice(type);
    } else {
      auto lhsRef = std::get<TypeRef>(type->body);
      return lhsRef.refType->matchNice(this);
    }
  } else if (this->isArray() || type->isArray()) {
    if (!this->isArray() || !type->isArray()) {
      return false;
    }

    auto lhsArray = std::get<TypeArray>(this->body);
    auto rhsArray = std::get<TypeArray>(type->body);

    return lhsArray.elementType->matchNice(rhsArray.elementType);
  } else if (this->isEnum() || type->isEnum()) {
    if (this->isEnum() && type->isEnum()) {
      return type->name == this->name;
    }

    return this->isInt() || type->isInt();
  } else if (this->isFn()) {
    if (!type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (
      !lhsFn.returnType->matchNice(rhsFn.returnType) ||
      lhsFn.params.size() != rhsFn.params.size() ||
      lhsFn.isMethod != rhsFn.isMethod ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst != rhsFn.methodInfo.isSelfFirst) ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst && !lhsFn.methodInfo.selfType->matchNice(rhsFn.methodInfo.selfType)) ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst && lhsFn.methodInfo.selfType->isRef() && lhsFn.methodInfo.isSelfMut != rhsFn.methodInfo.isSelfMut)
    ) {
      return false;
    }

    for (auto i = static_cast<std::size_t>(0); i < lhsFn.params.size(); i++) {
      auto lhsFnParam = lhsFn.params[i];
      auto rhsFnParam = rhsFn.params[i];

      if (
        (lhsFnParam.name != std::nullopt && lhsFnParam.name != rhsFnParam.name) ||
        !lhsFnParam.type->matchNice(rhsFnParam.type) ||
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
      return lhsOptional.type->matchNice(type);
    }

    auto lhsOptional = std::get<TypeOptional>(this->body);
    auto rhsOptional = std::get<TypeOptional>(type->body);

    return lhsOptional.type->matchNice(rhsOptional.type);
  } else if (this->isUnion()) {
    if (!type->isUnion()) {
      return this->hasSubType(type);
    }

    auto lhsUnion = std::get<TypeUnion>(this->body);
    auto rhsUnion = std::get<TypeUnion>(type->body);

    if (lhsUnion.subTypes.size() != rhsUnion.subTypes.size()) {
      return false;
    }

    return std::all_of(lhsUnion.subTypes.begin(), lhsUnion.subTypes.end(), [&type] (const auto &it) -> bool {
      return type->hasSubType(it);
    });
  }

  return (this->name == "bool" && type->name == "bool") ||
    (this->name == "byte" && (type->name == "byte" || type->isIntNumber())) ||
    (type->name == "byte" && (this->name == "byte" || this->isIntNumber())) ||
    (this->name == "char" && type->name == "char") ||
    (this->name == "str" && type->name == "str") ||
    (this->name == "void" && type->name == "void") ||
    numberTypeMatch(this->name, type->name);
}

bool Type::matchStrict (const Type *type, bool exact) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->matchStrict(type, exact);
  } else if (type->isAlias()) {
    type = Type::actual(std::get<TypeAlias>(type->body).type);
  }

  if (this->isArray() || type->isArray()) {
    if (!this->isArray() || !type->isArray()) {
      return false;
    }

    auto lhsArray = std::get<TypeArray>(this->body);
    auto rhsArray = std::get<TypeArray>(type->body);

    return lhsArray.elementType->matchStrict(rhsArray.elementType);
  } else if (this->isFn() || type->isFn()) {
    if (!this->isFn() || !type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (
      !lhsFn.returnType->matchStrict(rhsFn.returnType) ||
      lhsFn.params.size() != rhsFn.params.size() ||
      lhsFn.isMethod != rhsFn.isMethod ||
      (exact && lhsFn.isMethod && lhsFn.methodInfo.codeName != rhsFn.methodInfo.codeName) ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst != rhsFn.methodInfo.isSelfFirst) ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst && lhsFn.methodInfo.selfCodeName != rhsFn.methodInfo.selfCodeName) ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst && !lhsFn.methodInfo.selfType->matchStrict(rhsFn.methodInfo.selfType)) ||
      (lhsFn.isMethod && lhsFn.methodInfo.isSelfFirst && lhsFn.methodInfo.isSelfMut != rhsFn.methodInfo.isSelfMut)
    ) {
      return false;
    }

    for (auto i = static_cast<std::size_t>(0); i < lhsFn.params.size(); i++) {
      auto lhsFnParam = lhsFn.params[i];
      auto rhsFnParam = rhsFn.params[i];

      if (
        (exact && lhsFnParam.name != rhsFnParam.name) ||
        !lhsFnParam.type->matchStrict(rhsFnParam.type) ||
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

    return lhsOptional.type->matchStrict(rhsOptional.type);
  } else if (this->isRef() || type->isRef()) {
    if (!this->isRef() || !type->isRef()) {
      return false;
    }

    auto lhsRef = std::get<TypeRef>(this->body);
    auto rhsRef = std::get<TypeRef>(type->body);

    return lhsRef.refType->matchStrict(rhsRef.refType);
  } else if (this->isUnion() || type->isUnion()) {
    if (!this->isUnion() || !type->isUnion()) {
      return false;
    }

    auto lhsUnion = std::get<TypeUnion>(this->body);
    auto rhsUnion = std::get<TypeUnion>(type->body);

    if (lhsUnion.subTypes.size() != rhsUnion.subTypes.size()) {
      return false;
    }

    return std::all_of(lhsUnion.subTypes.begin(), lhsUnion.subTypes.end(), [&type] (const auto &it) -> bool {
      return type->hasSubType(it);
    });
  }

  return this->name == type->name;
}

bool Type::shouldBeFreed () const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->shouldBeFreed();
  } else if (this->isUnion()) {
    auto subTypes = std::get<TypeUnion>(this->body).subTypes;
    bool result = false;

    for (const auto &subType : subTypes) {
      if (subType->shouldBeFreed()) {
        result = true;
        break;
      }
    }

    return result;
  }

  return
    this->isAny() ||
    this->isArray() ||
    this->isFn() ||
    this->isObj() ||
    this->isOpt() ||
    this->isStr();
}

std::string Type::xml (std::size_t indent, std::set<std::string> parentTypes) const {
  if (this->builtin) {
    return std::string(indent, ' ') + R"(<BuiltinType name=")" + this->name + R"(" />)";
  }

  auto typeName = std::string("Type");

  if (this->isAlias()) {
    typeName += "Alias";
  } else if (this->isArray()) {
    typeName += "Array";
  } else if (this->isEnum()) {
    typeName += "Enum";
  } else if (this->isEnumerator()) {
    typeName += "Enumerator";
  } else if (this->isFn()) {
    typeName += "Fn";
  } else if (this->isObj()) {
    typeName += "Obj";
  } else if (this->isOpt()) {
    typeName += "Optional";
  } else if (this->isRef()) {
    typeName += "Ref";
  } else if (this->isUnion()) {
    typeName += "Union";
  }

  auto result = std::string(indent, ' ') + "<" + typeName;

  result += this->codeName[0] == '@' ? "" : R"( codeName=")" + this->codeName + R"(")";
  result += this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")";

  if (this->isEnumerator() || (this->isObj() && parentTypes.contains(this->codeName))) {
    return result + " />";
  }

  result += ">" EOL;

  if (this->isAlias()) {
    auto typeAlias = std::get<TypeAlias>(this->body);
    result += typeAlias.type->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isArray()) {
    auto typeArray = std::get<TypeArray>(this->body);
    result += typeArray.elementType->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isEnum()) {
    auto typeEnum = std::get<TypeEnum>(this->body);

    for (const auto &member : typeEnum.members) {
      result += member->xml(indent + 2, parentTypes) + EOL;
    }
  } else if (this->isFn()) {
    auto typeFn = std::get<TypeFn>(this->body);

    if (typeFn.isMethod) {
      auto methodAttrs = std::string();

      methodAttrs += typeFn.methodInfo.codeName[0] != '@' ? R"( codeName=")" + typeFn.methodInfo.codeName + R"(")" : "";
      methodAttrs += typeFn.methodInfo.isSelfFirst ? R"( selfCodeName=")" + typeFn.methodInfo.selfCodeName + R"(")" : "";
      methodAttrs += typeFn.methodInfo.isSelfFirst ? " selfFirst" : "";
      methodAttrs += typeFn.methodInfo.isSelfMut ? " selfMut" : "";

      if (typeFn.methodInfo.isSelfFirst) {
        result += std::string(indent + 2, ' ') + "<TypeFnMethodInfo" + methodAttrs + ">" EOL;
        result += typeFn.methodInfo.selfType->xml(indent + 4, parentTypes) + EOL;
        result += std::string(indent + 2, ' ') + "</TypeFnMethodInfo>" EOL;
      } else {
        result += std::string(indent + 2, ' ') + "<TypeFnMethodInfo" + methodAttrs + " />" EOL;
      }
    }

    if (!typeFn.params.empty()) {
      result += std::string(indent + 2, ' ') + "<TypeFnParams>" EOL;

      for (const auto &typeFnParam : typeFn.params) {
        auto paramAttrs = typeFnParam.name == std::nullopt ? "" : R"( name=")" + *typeFnParam.name + R"(")";

        paramAttrs += typeFnParam.mut ? " mut" : "";
        paramAttrs += typeFnParam.required ? " required" : "";
        paramAttrs += typeFnParam.variadic ? " variadic" : "";

        result += std::string(indent + 4, ' ') + "<TypeFnParam" + paramAttrs + ">" EOL;
        result += typeFnParam.type->xml(indent + 6, parentTypes) + EOL;
        result += std::string(indent + 4, ' ') + "</TypeFnParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</TypeFnParams>" EOL;
    }

    result += std::string(indent + 2, ' ') + "<TypeFnReturnType>" EOL;
    result += typeFn.returnType->xml(indent + 4, parentTypes) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeFnReturnType>" EOL;
  } else if (this->isObj()) {
    parentTypes.insert(this->codeName);

    for (const auto &field : this->fields) {
      if (field.builtin) {
        continue;
      }

      auto tagName = std::string(field.method ? "TypeMethod" : "TypeField");
      auto fieldAttrs = std::string();

      fieldAttrs += field.mut ? " mut" : "";
      fieldAttrs += R"( name=")" + field.name + R"(")";

      result += std::string(indent + 2, ' ') + "<" + tagName + fieldAttrs + ">" EOL;
      result += field.type->xml(indent + 4, parentTypes) + EOL;
      result += std::string(indent + 2, ' ') + "</" + tagName + ">" EOL;
    }
  } else if (this->isOpt()) {
    auto typeOptional = std::get<TypeOptional>(this->body);
    result += typeOptional.type->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isRef()) {
    auto typeRef = std::get<TypeRef>(this->body);
    result += typeRef.refType->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isUnion()) {
    auto typeUnion = std::get<TypeUnion>(this->body);

    for (const auto &subType : typeUnion.subTypes) {
      result += subType->xml(indent + 2, parentTypes) + EOL;
    }
  }

  return result + std::string(indent, ' ') + "</" + typeName + ">";
}
