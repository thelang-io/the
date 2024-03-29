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

#ifndef SRC_TYPE_HPP
#define SRC_TYPE_HPP

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <variant>
#include <vector>

struct Type;
struct TypeAlias;
struct TypeArray;
struct TypeEnum;
struct TypeEnumerator;
struct TypeFn;
struct TypeBodyMap;
struct TypeNamespace;
struct TypeObj;
struct TypeOptional;
struct TypeRef;
struct TypeUnion;

using TypeBody = std::variant<
  TypeAlias,
  TypeArray,
  TypeEnum,
  TypeEnumerator,
  TypeFn,
  TypeBodyMap,
  TypeNamespace,
  TypeObj,
  TypeOptional,
  TypeRef,
  TypeUnion
>;

struct TypeCallInfo {
  std::string codeName;
  bool isSelfFirst = false;
  std::string selfCodeName = "";
  Type *selfType = nullptr;
  bool isSelfMut = false;
  bool throws = false;

  bool empty () const;
  std::string xml (std::size_t = 0, std::set<std::string> = {}) const;
};

struct TypeField {
  std::string name;
  Type *type;
  bool mut;
  bool builtin = false;
  TypeCallInfo callInfo = {};
};

struct TypeAlias {
  Type *type;
};

struct TypeArray {
  Type *elementType;
};

struct TypeEnum {
  std::vector<Type *> members;
};

struct TypeEnumerator {
};

struct TypeFnParam {
  std::optional<std::string> name;
  Type *type;
  bool mut;
  bool required;
  bool variadic;
};

struct TypeFn {
  Type *returnType;
  std::vector<TypeFnParam> params = {};
  bool throws;
  bool isMethod;
  TypeCallInfo callInfo;
  bool async;
};

struct TypeBodyMap {
  Type *keyType;
  Type *valueType;
};

struct TypeNamespace {
};

struct TypeObj {
};

struct TypeOptional {
  Type *type;
};

struct TypeRef {
  Type *refType;
};

struct TypeUnion {
  std::vector<Type *> subTypes;
};

struct Type {
  std::string name;
  std::string codeName;
  TypeBody body;
  std::vector<TypeField> fields = {};
  bool builtin = false;

  static Type *actual (Type *);
  static Type *real (Type *);
  static Type *largest (Type *, Type *);

  bool canBeCast (Type *);
  std::optional<TypeField> fieldNth (std::size_t) const;
  Type *getEnumerator (const std::string &) const;
  TypeField getField (const std::string &) const;
  Type *getProp (const std::string &) const;
  bool hasEnumerator (const std::string &) const;
  bool hasField (const std::string &) const;
  bool hasProp (const std::string &) const;
  bool hasSubType (const Type *) const;
  bool isAlias () const;
  bool isAny () const;
  bool isArray () const;
  bool isBool () const;
  bool isByte () const;
  bool isChar () const;
  bool isEnum () const;
  bool isEnumerator () const;
  bool isF32 () const;
  bool isF64 () const;
  bool isFloat () const;
  bool isFloatNumber () const;
  bool isFn () const;
  bool isI8 () const;
  bool isI16 () const;
  bool isI32 () const;
  bool isI64 () const;
  bool isInt () const;
  bool isIntNumber () const;
  bool isMap () const;
  bool isMethod () const;
  bool isNamespace () const;
  bool isNumber () const;
  bool isObj () const;
  bool isOpt () const;
  bool isRef () const;
  bool isRefOf (const Type *) const;
  bool isSafeForTernaryAlt () const;
  bool isSmallForVarArg () const;
  bool isStr () const;
  bool isU8 () const;
  bool isU16 () const;
  bool isU32 () const;
  bool isU64 () const;
  bool isUnion () const;
  bool isVoid () const;
  bool matchNice (const Type *) const;
  bool matchStrict (const Type *, bool = false) const;
  bool shouldBeFreed () const;
  std::string vaArgCode (const std::string &) const;
  std::string xml (std::size_t = 0, std::set<std::string> = {}) const;
};

#endif
