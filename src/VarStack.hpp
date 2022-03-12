/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_STACK_HPP
#define SRC_VAR_STACK_HPP

#include "TypeMap.hpp"

struct Var;
class VarStack;

struct Var {
  std::string name;
  std::string codeName;
  std::shared_ptr<Type> type;
  bool mut;
  bool builtin;
  std::size_t _frame;
};

class VarStack {
 public:
  explicit VarStack (const std::vector<std::shared_ptr<Var>> &);

  void mark (const std::shared_ptr<Var> &);
  std::vector<std::shared_ptr<Var>> snapshot () const;

 private:
  std::vector<std::tuple<std::shared_ptr<Var>, bool>> _items;
};

#endif
