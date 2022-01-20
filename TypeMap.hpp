/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TYPE_MAP_HPP
#define TYPE_MAP_HPP

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "Error.hpp"

struct Type;

struct TypeFnParam {
  std::string name;
  std::shared_ptr<Type> type;
  bool required;
};

struct TypeFn {
  std::shared_ptr<Type> type;
  std::vector<TypeFnParam> params = {};
};

struct TypeObjField {
  std::string name;
  std::shared_ptr<Type> type;
};

struct TypeObj {
  std::vector<TypeObjField> fields = {};
};

struct Type {
  std::string name;
  std::optional<std::shared_ptr<Type>> parent;
  std::variant<TypeFn, TypeObj> body;

  bool isAny () const;
  bool isBool () const;
  bool isByte () const;
  bool isChar () const;
  bool isFloat () const;
  bool isF32 () const;
  bool isF64 () const;
  bool isFn () const;
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
};

class TypeMap {
 public:
  std::vector<std::string> stack;

  static std::shared_ptr<Type> fn (const std::shared_ptr<Type> &, const std::vector<TypeFnParam> & = {});
  static std::shared_ptr<Type> fn (const std::string &, const std::shared_ptr<Type> &, const std::vector<TypeFnParam> & = {});

  TypeMap ();

  std::shared_ptr<Type> add (const std::string &, const std::optional<std::shared_ptr<Type>> & = std::nullopt, const std::vector<TypeObjField> & = {});
  std::shared_ptr<Type> add (const std::string &, const std::shared_ptr<Type> & = {}, const std::vector<TypeFnParam> & = {});
  const std::shared_ptr<Type> &get (const std::string &) const;
  std::string name (const std::string &) const;
  std::shared_ptr<Type> obj () const;

 private:
  std::vector<std::shared_ptr<Type>> _items;
  std::shared_ptr<Type> _obj;
};

#endif
