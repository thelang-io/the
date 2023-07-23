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
#include <utility>
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

// todo test
bool TypeCallInfo::empty () const {
  return this->codeName.empty() &&
    !this->isSelfFirst &&
    this->selfCodeName.empty() &&
    this->selfType == nullptr &&
    !this->isSelfMut &&
    !this->throws;
}

std::string TypeCallInfo::xml (std::size_t indent, std::set<std::string> parentTypes) const {
  auto result = std::string(indent, ' ') + "<TypeCallInfo";

  result += this->codeName[0] != '@' ? R"( codeName=")" + this->codeName + R"(")" : "";
  result += this->isSelfFirst ? R"( selfCodeName=")" + this->selfCodeName + R"(")" : "";
  result += this->isSelfFirst ? " selfFirst" : "";
  result += this->isSelfMut ? " selfMut" : "";

  if (!this->isSelfFirst) {
    return result + " />";
  }

  result += ">" EOL;
  result += this->selfType->xml(indent + 2, std::move(parentTypes)) + EOL;
  result += std::string(indent, ' ') + "</TypeCallInfo>";

  return result;
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

std::optional<TypeField> Type::fieldNth (std::size_t idx) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->fieldNth(idx);
  } else if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->fieldNth(idx);
  }

  auto result = std::vector<TypeField>{};

  for (const auto &field : this->fields) {
    if (field.builtin || field.type->isMethod()) {
      continue;
    }
    result.push_back(field);
  }

  return idx >= result.size() ? std::optional<TypeField>{} : result[idx];
}

Type *Type::getEnumerator (const std::string &memberName) const {
  if (!this->isEnum()) {
    throw Error("tried to get a member of non-enum");
  }

  auto enumType = std::get<TypeEnum>(this->body);

  auto typeMember = std::find_if(enumType.members.begin(), enumType.members.end(), [&memberName] (const auto &it) -> bool {
    return it->name == memberName;
  });

  if (typeMember == enumType.members.end()) {
    throw Error("tried to get non-existing enum member");
  }

  return *typeMember;
}

TypeField Type::getField (const std::string &fieldName) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->getField(fieldName);
  } else if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->getField(fieldName);
  }

  auto typeField = std::find_if(this->fields.begin(), this->fields.end(), [&fieldName] (const auto &it) -> bool {
    return it.name == fieldName;
  });

  if (typeField == this->fields.end()) {
    throw Error("tried to get non-existing field `" + fieldName + "`");
  }

  return *typeField;
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

  auto enumType = std::get<TypeEnum>(this->body);

  auto typeMember = std::find_if(enumType.members.begin(), enumType.members.end(), [&memberName] (const auto &it) -> bool {
    return it->name == memberName;
  });

  return typeMember != enumType.members.end();
}

bool Type::hasField (const std::string &fieldName) const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->hasField(fieldName);
  } else if (this->isRef()) {
    return std::get<TypeRef>(this->body).refType->hasField(fieldName);
  }

  auto typeField = std::find_if(this->fields.begin(), this->fields.end(), [&fieldName] (const auto &it) -> bool {
    return it.name == fieldName;
  });

  return typeField != this->fields.end();
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
    auto unionType = std::get<TypeUnion>(this->body);

    for (const auto &thisSubType : unionType.subTypes) {
      if (thisSubType->matchStrict(subType, true)) {
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

bool Type::isMap () const {
  return std::holds_alternative<TypeBodyMap>(this->body);
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
    !this->isMap() &&
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

bool Type::isRefOf (const Type *type) const {
  if (!this->isRef()) {
    return false;
  }

  auto refType = Type::actual(std::get<TypeRef>(this->body).refType);
  return refType->matchStrict(type) || refType->isRefOf(type);
}

bool Type::isSafeForTernaryAlt () const {
  if (this->isAlias()) {
    return std::get<TypeAlias>(this->body).type->isSafeForTernaryAlt();
  }

  return this->isAny() ||
    this->isArray() ||
    this->isFn() ||
    this->isMap() ||
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
  }

  if (this->isArray() || type->isArray()) {
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
  } else if (this->isMap() || type->isMap()) {
    if (!this->isMap() || !type->isMap()) {
      return false;
    }

    auto lhsMap = std::get<TypeBodyMap>(this->body);
    auto rhsMap = std::get<TypeBodyMap>(type->body);

    return lhsMap.keyType->matchNice(rhsMap.keyType) && lhsMap.valueType->matchNice(rhsMap.valueType);
  } else if (this->isFn()) {
    if (!type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (
      !lhsFn.returnType->matchNice(rhsFn.returnType) ||
      lhsFn.params.size() != rhsFn.params.size() ||
      lhsFn.async != rhsFn.async || // todo test
      lhsFn.isMethod != rhsFn.isMethod ||
      (lhsFn.callInfo.isSelfFirst != rhsFn.callInfo.isSelfFirst) ||
      (lhsFn.callInfo.isSelfFirst && !lhsFn.callInfo.selfType->matchNice(rhsFn.callInfo.selfType)) ||
      (lhsFn.callInfo.isSelfFirst && lhsFn.callInfo.selfType->isRef() && lhsFn.callInfo.isSelfMut != rhsFn.callInfo.isSelfMut)
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

    return lhsArray.elementType->matchStrict(rhsArray.elementType, exact);
  } else if (this->isFn() || type->isFn()) {
    if (!this->isFn() || !type->isFn()) {
      return false;
    }

    auto lhsFn = std::get<TypeFn>(this->body);
    auto rhsFn = std::get<TypeFn>(type->body);

    if (
      !lhsFn.returnType->matchStrict(rhsFn.returnType, exact) ||
      lhsFn.params.size() != rhsFn.params.size() ||
      lhsFn.async != rhsFn.async || // todo test
      lhsFn.isMethod != rhsFn.isMethod ||
      (exact && lhsFn.callInfo.codeName != rhsFn.callInfo.codeName) ||
      (lhsFn.callInfo.isSelfFirst != rhsFn.callInfo.isSelfFirst) ||
      (lhsFn.callInfo.isSelfFirst && lhsFn.callInfo.selfCodeName != rhsFn.callInfo.selfCodeName) ||
      (lhsFn.callInfo.isSelfFirst && !lhsFn.callInfo.selfType->matchStrict(rhsFn.callInfo.selfType, exact)) ||
      (lhsFn.callInfo.isSelfFirst && lhsFn.callInfo.isSelfMut != rhsFn.callInfo.isSelfMut)
    ) {
      return false;
    }

    for (auto i = static_cast<std::size_t>(0); i < lhsFn.params.size(); i++) {
      auto lhsFnParam = lhsFn.params[i];
      auto rhsFnParam = rhsFn.params[i];

      if (
        (exact && lhsFnParam.name != rhsFnParam.name) ||
        !lhsFnParam.type->matchStrict(rhsFnParam.type, exact) ||
        lhsFnParam.mut != rhsFnParam.mut ||
        lhsFnParam.required != rhsFnParam.required ||
        lhsFnParam.variadic != rhsFnParam.variadic
      ) {
        return false;
      }
    }

    return true;
  } else if (this->isMap() || type->isMap()) {
    if (!this->isMap() || !type->isMap()) {
      return false;
    }

    auto lhsMap = std::get<TypeBodyMap>(this->body);
    auto rhsMap = std::get<TypeBodyMap>(type->body);

    return lhsMap.keyType->matchStrict(rhsMap.keyType, exact) && lhsMap.valueType->matchStrict(rhsMap.valueType, exact);
  } else if (this->isOpt() || type->isOpt()) {
    if (!this->isOpt() || !type->isOpt()) {
      return false;
    }

    auto lhsOptional = std::get<TypeOptional>(this->body);
    auto rhsOptional = std::get<TypeOptional>(type->body);

    return lhsOptional.type->matchStrict(rhsOptional.type, exact);
  } else if (this->isRef() || type->isRef()) {
    if (!this->isRef() || !type->isRef()) {
      return false;
    }

    auto lhsRef = std::get<TypeRef>(this->body);
    auto rhsRef = std::get<TypeRef>(type->body);

    return lhsRef.refType->matchStrict(rhsRef.refType, exact);
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
    (this->isFn() && !this->builtin) ||
    this->isMap() ||
    this->isObj() ||
    this->isOpt() ||
    this->isStr();
}

std::string Type::vaArgCode (const std::string &code) const {
  return this->isSmallForVarArg() ? this->isF32() ? "double" : "int" : code;
}

std::string Type::xml (std::size_t indent, std::set<std::string> parentTypes) const {
  if (this->builtin) {
    return std::string(indent, ' ') + R"(<BuiltinType name=")" + this->name + R"(" />)";
  }

  auto typeName = std::string("Type");
  auto attrs = std::string();

  attrs += this->codeName[0] == '@' ? "" : R"( codeName=")" + this->codeName + R"(")";
  attrs += this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")";

  if (this->isAlias()) {
    typeName += "Alias";
  } else if (this->isArray()) {
    typeName += "Array";
  } else if (this->isEnum()) {
    typeName += "Enum";
  } else if (this->isEnumerator()) {
    typeName += "Enumerator";
  } else if (this->isFn()) {
    auto fnType = std::get<TypeFn>(this->body);

    typeName += this->isMethod() ? "Method" : "Fn";
    attrs += fnType.async ? " async" : ""; // todo test
    attrs += fnType.throws ? " throws" : "";
  } else if (this->isMap()) {
    typeName += "Map";
  } else if (this->isObj()) {
    typeName += "Obj";
  } else if (this->isOpt()) {
    typeName += "Optional";
  } else if (this->isRef()) {
    typeName += "Ref";
  } else if (this->isUnion()) {
    typeName += "Union";
  }

  auto result = std::string(indent, ' ') + "<" + typeName + attrs;

  if (this->isEnumerator() || (this->isObj() && parentTypes.contains(this->codeName))) {
    return result + " />";
  }

  result += ">" EOL;

  if (this->isAlias()) {
    auto aliasType = std::get<TypeAlias>(this->body);
    result += aliasType.type->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isArray()) {
    auto arrayType = std::get<TypeArray>(this->body);
    result += arrayType.elementType->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isEnum()) {
    auto enumType = std::get<TypeEnum>(this->body);

    for (const auto &member : enumType.members) {
      result += member->xml(indent + 2, parentTypes) + EOL;
    }
  } else if (this->isFn()) {
    auto fnType = std::get<TypeFn>(this->body);

    if (!fnType.callInfo.empty()) {
      result += fnType.callInfo.xml(indent + 2, parentTypes) + EOL;
    }

    if (!fnType.params.empty()) {
      result += std::string(indent + 2, ' ') + "<TypeFnParams>" EOL;

      for (const auto &typeFnParam : fnType.params) {
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
    result += fnType.returnType->xml(indent + 4, parentTypes) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeFnReturnType>" EOL;
  } else if (this->isMap()) {
    auto mapType = std::get<TypeBodyMap>(this->body);

    result += std::string(indent + 2, ' ') + "<TypeMapKeyType>" EOL;
    result += mapType.keyType->xml(indent + 4, parentTypes) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeMapKeyType>" EOL;
    result += std::string(indent + 2, ' ') + "<TypeMapValueType>" EOL;
    result += mapType.valueType->xml(indent + 4, parentTypes) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeMapValueType>" EOL;
  } else if (this->isObj()) {
    parentTypes.insert(this->codeName);

    for (const auto &field : this->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldAttrs = std::string();

      fieldAttrs += field.mut ? " mut" : "";
      fieldAttrs += R"( name=")" + field.name + R"(")";

      result += std::string(indent + 2, ' ') + "<TypeField" + fieldAttrs + ">" EOL;

      if (!field.callInfo.empty()) {
        result += field.callInfo.xml(indent + 4, parentTypes) + EOL;
      }

      result += field.type->xml(indent + 4, parentTypes) + EOL;
      result += std::string(indent + 2, ' ') + "</TypeField>" EOL;
    }
  } else if (this->isOpt()) {
    auto optType = std::get<TypeOptional>(this->body);
    result += optType.type->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isRef()) {
    auto refType = std::get<TypeRef>(this->body);
    result += refType.refType->xml(indent + 2, parentTypes) + EOL;
  } else if (this->isUnion()) {
    auto unionType = std::get<TypeUnion>(this->body);

    for (const auto &subType : unionType.subTypes) {
      result += subType->xml(indent + 2, parentTypes) + EOL;
    }
  }

  return result + std::string(indent, ' ') + "</" + typeName + ">";
}
