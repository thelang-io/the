/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TYPE_HPP
#define SRC_TYPE_HPP

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Type;

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
};

struct TypeObjField {
  std::string name;
  Type *type;
};

struct TypeObj {
  std::vector<TypeObjField> fields = {};
};

struct TypeRef {
  Type *type;
};

struct Type {
  std::string name;
  std::string codeName;
  std::variant<TypeFn, TypeObj, TypeRef> body;
  bool builtin;

  static Type *real (Type *);
  static Type *largest (Type *, Type *);

  bool hasProp (const std::string &) const;
  Type *getProp (const std::string &) const;
  bool isAny () const;
  bool isBool () const;
  bool isByte () const;
  bool isChar () const;
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
  bool isNumber () const;
  bool isObj () const;
  bool isRef () const;
  bool isStr () const;
  bool isU8 () const;
  bool isU16 () const;
  bool isU32 () const;
  bool isU64 () const;
  bool isVoid () const;
  bool match (const Type *) const;
  bool matchExact (const Type *) const;
  bool matchNice (const Type *) const;
  std::string xml (std::size_t = 0) const;
};

#endif
