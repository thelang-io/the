/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_MAP_HPP
#define SRC_VAR_MAP_HPP

#include <optional>
#include <variant>
#include "TypeMap.hpp"

class VarMap;

struct Var {
  std::string name;
  std::shared_ptr<Type> type;
  bool mut;
  std::size_t _frame;
};

class VarMap {
 public:
  Var &add (const std::string &, const std::shared_ptr<Type> &, bool = false);
  const Var &get (const std::string &) const;
  bool has (const std::string &) const;
  void restore ();
  void save ();

 private:
  std::size_t _frame = 0;
  std::vector<Var> _items;
};

#endif
