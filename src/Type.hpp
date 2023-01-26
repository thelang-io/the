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
struct TypeArray;
struct TypeEnum;
struct TypeEnumerator;
struct TypeFn;
struct TypeObj;
struct TypeOptional;
struct TypeRef;
struct TypeUnion;

using TypeBody = std::variant<
  TypeArray,
  TypeEnum,
  TypeEnumerator,
  TypeFn,
  TypeObj,
  TypeOptional,
  TypeRef,
  TypeUnion
>;

struct TypeField {
  std::string name;
  Type *type;
  bool mut;
  bool method;
  bool builtin;
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

struct TypeFnMethodInfo {
  bool isSelfFirst = false;
  std::string selfCodeName = "";
  Type *selfType = nullptr;
  bool isSelfMut = false;
};

struct TypeFn {
  Type *returnType;
  std::vector<TypeFnParam> params = {};
  bool isMethod = false;
  TypeFnMethodInfo methodInfo = {};
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

  static Type *real (Type *);
  static Type *largest (Type *, Type *);

  Type *getEnumerator (const std::string &) const;
  Type *getProp (const std::string &) const;
  bool hasEnumerator (const std::string &) const;
  bool hasProp (const std::string &) const;
  bool hasSubType (const Type *) const;
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
  bool isMethod () const;
  bool isNumber () const;
  bool isObj () const;
  bool isOpt () const;
  bool isRef () const;
  bool isRefExt () const;
  bool isSmallForVarArg () const;
  bool isStr () const;
  bool isU8 () const;
  bool isU16 () const;
  bool isU32 () const;
  bool isU64 () const;
  bool isUnion () const;
  bool isVoid () const;
  bool match (const Type *) const;
  bool matchExact (const Type *) const;
  bool matchNice (const Type *) const;
  bool shouldBeFreed () const;
  std::string xml (std::size_t = 0, std::set<std::string> = {}) const;
};

#endif
