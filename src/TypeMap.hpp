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

  Type *add (const std::string &, const std::string &, const std::vector<TypeObjField> &);
  Type *add (const std::string &, const std::string &, const std::vector<TypeFnParam> &, Type *);
  Type *fn (const std::vector<TypeFnParam> &, Type *);
  Type *get (const std::string &);
  bool has (const std::string &);
  void init ();
  std::string name (const std::string &) const;

 private:
  std::vector<std::unique_ptr<Type>> _items;
};

#endif
