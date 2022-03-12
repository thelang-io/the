/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_MAP_HPP
#define SRC_VAR_MAP_HPP

#include "VarStack.hpp"

class VarMap;

class VarMap {
 public:
  std::shared_ptr<Var> add (const std::string &, const std::string &, const std::shared_ptr<Type> &, bool = false);
  std::shared_ptr<Var> get (const std::string &) const;
  bool has (const std::string &) const;
  void init (const TypeMap &);
  std::string name (const std::string &) const;
  void restore ();
  void save ();
  VarStack stack () const;

 private:
  std::size_t _frame = 0;
  std::vector<std::shared_ptr<Var>> _items;
};

#endif
