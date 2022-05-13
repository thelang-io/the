/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TYPE_MAP_HPP
#define SRC_TYPE_MAP_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Type;

struct TypeFnParam {
  std::string name;
  Type *type;
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

struct Type {
  std::string name;
  std::variant<TypeFn, TypeObj> body;
  bool builtin;

  static Type *largest (Type *, Type *); // todo test

  bool isAny () const;
  bool isBool () const;
  bool isByte () const;
  bool isChar () const;
  bool isF32 () const;
  bool isF64 () const;
  bool isFloat () const;
  bool isFn () const;
  bool isI8 () const;
  bool isI16 () const;
  bool isI32 () const;
  bool isI64 () const;
  bool isInt () const;
  bool isObj () const;
  bool isStr () const;
  bool isU8 () const;
  bool isU16 () const;
  bool isU32 () const;
  bool isU64 () const;
  bool isVoid () const;
  bool match (const Type *) const;
  std::string xml (std::size_t = 0) const;
};

class TypeMap {
 public:
  std::vector<std::string> stack;

  Type *add (const std::string &, const std::vector<TypeObjField> &);
  Type *add (const std::string &, const std::vector<TypeFnParam> &, Type *);
  Type *fn (const std::vector<TypeFnParam> &, Type *);
  Type *get (const std::string &);
  void init ();
  std::string name (const std::string &) const;

 private:
  std::vector<std::unique_ptr<Type>> _items;
};

#endif
