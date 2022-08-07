/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TYPE_MAP_HPP
#define SRC_TYPE_MAP_HPP

#include "Type.hpp"

class TypeMap {
 public:
  std::vector<std::string> stack;

  Type *arrayOf (Type *);
  Type *fn (const std::optional<std::string> &, const std::vector<TypeFnParam> &, Type *);
  Type *get (const std::string &);
  bool has (const std::string &);
  void init ();
  std::string name (const std::string &) const;
  Type *obj (const std::string &, const std::string &, const std::vector<TypeField> & = {});
  Type *ref (Type *);

 private:
  std::size_t _fnIdx = 0;
  std::vector<std::unique_ptr<Type>> _items;
};

#endif
