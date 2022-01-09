/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TYPED_MAP_HPP
#define SRC_TYPED_MAP_HPP

#include <algorithm>
#include <climits>
#include <map>
#include <memory>
#include <vector>
#include "Error.hpp"

struct Type {
  std::string name;
  std::optional<std::shared_ptr<Type>> parent = std::nullopt;
  std::map<std::string, std::shared_ptr<Type>> fields = {};

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
  std::shared_ptr<Type> type;
  // TODO use
  bool required;
};

struct Fn {
  std::string name;
  std::shared_ptr<Type> returnType;
  std::map<std::string, FnParam> params;
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

  inline const std::shared_ptr<T> &get (const std::string &name) const {
    for (const auto &item : this->_items) {
      if (item->name == name) {
        return item;
      }
    }

    throw Error("Tried to access non existing typed map item");
  }

  inline bool has (const std::string &name) const {
    return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
      return it.name == name;
    });
  }

  inline std::string name (const std::string &name) const {
    auto fullName = std::string();

    for (const auto &item : this->stack) {
      fullName += item + "SD";
    }

    fullName += name + "_";

    for (auto idx = static_cast<std::size_t>(0);; idx++) {
      auto fullNameTest = fullName + std::to_string(idx);
      auto exists = false;

      for (const auto &item : this->_items) {
        if (item->name == fullNameTest) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        fullName = fullNameTest;
        break;
      }
    }

    return fullName;
  }

 private:
  std::vector<std::shared_ptr<T>> _items;
};

using FnMap = TypedMap<Fn>;
using TypeMap = TypedMap<Type>;

#endif
