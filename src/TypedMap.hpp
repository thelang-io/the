/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TYPED_MAP_HPP
#define SRC_TYPED_MAP_HPP

#include <climits>
#include <memory>
#include <vector>
#include "Error.hpp"

struct Type {
  std::string name;

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

struct FnParam {
  std::string name;
  Type type;
  bool required = false; // TODO Use
};

struct Fn {
  std::string name;
  Type returnType;
  std::vector<FnParam> params;
  bool builtin = false;
};

template <class T>
class TypedMap {
 public:
  std::vector<std::string> stack;

  inline TypedMap () {
    this->stack.reserve(SHRT_MAX);
  }

  inline std::shared_ptr<T> add (const T &n) {
    this->_items.push_back(std::make_shared<T>(n));
    return this->_items.back();
  }

  inline const T &get (const std::string &name) const {
    for (auto it : this->_items) {
      if (it->name == name) {
        return *it;
      }
    }

    throw Error("Tried to access non existing variable map item");
  }

  inline std::string name (const std::string &name) const {
    auto result = std::string();

    for (auto it : this->stack) {
      result += it + "SD";
    }

    result = result + name + "_";

    for (auto idx = static_cast<std::size_t>(0);; idx++) {
      auto fullName = result + std::to_string(idx);
      auto exists = false;

      for (auto it : this->_items) {
        if (it->name == fullName) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        result = fullName;
        break;
      }
    }

    return result;
  }

 private:
  std::vector<std::shared_ptr<T>> _items;
};

using FnMap = TypedMap<Fn>;
using TypeMap = TypedMap<Type>;

#endif
