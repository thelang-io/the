/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_MAP_HPP
#define SRC_VAR_MAP_HPP

#include "TypeMap.hpp"
#include "VarStack.hpp"

class VarMap {
 public:
  std::shared_ptr<Var> add (const std::string &, const std::string &, Type *, bool = false);
  std::shared_ptr<Var> get (const std::string &);
  bool has (const std::string &) const;
  void init (TypeMap &);
  std::string name (const std::string &) const;
  void restore ();
  void save ();
  VarStack stack () const;

 private:
  std::size_t _frame = 0;
  std::vector<std::shared_ptr<Var>> _items;
};

#endif
