/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TYPE_MAP_HPP
#define SRC_TYPE_MAP_HPP

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Type;
struct TypeFn;
struct TypeFnParam;
struct TypeObj;
struct TypeObjField;

struct TypeFn {
  std::shared_ptr<Type> returnType;
  std::vector<TypeFnParam> params = {};
};

struct TypeFnParam {
  std::string name;
  std::shared_ptr<Type> type;
  bool required;
  bool variadic;
};

struct TypeObj {
  std::vector<TypeObjField> fields = {};
};

struct TypeObjField {
  std::string name;
  std::shared_ptr<Type> type;
};

struct Type {
  std::string name;
  std::variant<TypeFn, TypeObj> body;
  bool builtin;

  bool isAny () const;
  bool isBool () const;
  bool isByte () const;
  bool isChar () const;
  bool isFloat () const;
  bool isFn () const;
  bool isF32 () const;
  bool isF64 () const;
  bool isInt () const;
  bool isI8 () const;
  bool isI16 () const;
  bool isI32 () const;
  bool isI64 () const;
  bool isObj () const;
  bool isStr () const;
  bool isU8 () const;
  bool isU16 () const;
  bool isU32 () const;
  bool isU64 () const;
  bool isVoid () const;
  bool match (const std::shared_ptr<Type> &) const;
  std::string xml (std::size_t = 0) const;
};

class TypeMap {
 public:
  std::vector<std::string> stack;

  static std::shared_ptr<Type> fn (const std::shared_ptr<Type> &, const std::vector<TypeFnParam> & = {});

  std::shared_ptr<Type> add (const std::string &, const std::vector<TypeObjField> &);
  std::shared_ptr<Type> add (const std::string &, const std::vector<TypeFnParam> &, const std::shared_ptr<Type> &);
  std::shared_ptr<Type> get (const std::string &) const;
  void init ();
  std::string name (const std::string &) const;

 private:
  std::vector<std::shared_ptr<Type>> _items;
};

#endif
