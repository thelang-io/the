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
#include "TypedMap.hpp"

struct Var {
  std::string name;
  Type type;
  bool mut;
  std::optional<std::variant<std::shared_ptr<Fn>, std::shared_ptr<Type>>> ref;
  std::size_t frame;
};

class VarMap {
 public:
  Var &add (const std::string &, const Type &, bool, const std::optional<std::variant<std::shared_ptr<Fn>, std::shared_ptr<Type>>> & = std::nullopt);
  const Var &get (const std::string &) const;
  void restore ();
  void save ();

 private:
  std::size_t _frame = 0;
  std::vector<Var> _items;
};

#endif
