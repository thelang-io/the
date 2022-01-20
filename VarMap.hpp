/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef VAR_MAP_HPP
#define VAR_MAP_HPP

#include "TypeMap.hpp"

class VarMap;

struct Var {
  std::string name;
  std::string codeName;
  std::shared_ptr<Type> type;
  bool mut;
  bool builtin;
  std::size_t _frame;
};

class VarMap {
 public:
  Var &add (const std::string &, const std::string &, const std::shared_ptr<Type> &, bool = false, bool = false);
  const Var &get (const std::string &) const;
  bool has (const std::string &) const;
  std::string name (const std::string &) const;
  void restore ();
  void save ();
  std::vector<Var> stack () const;

 private:
  std::size_t _frame = 0;
  std::vector<Var> _items;
};

#endif
