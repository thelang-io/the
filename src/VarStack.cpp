/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "VarStack.hpp"

VarStack::VarStack (const std::vector<std::shared_ptr<Var>> &items) {
  this->_items.reserve(items.size());

  for (const auto &item : items) {
    this->_items.emplace_back(std::make_tuple(item, false));
  }
}

void VarStack::mark (const std::shared_ptr<Var> &var) {
  for (auto &item : this->_items) {
    if (std::get<0>(item)->codeName == var->codeName) {
      std::get<1>(item) = false;
    }
  }
}

std::vector<std::shared_ptr<Var>> VarStack::snapshot () const {
  auto result = std::vector<std::shared_ptr<Var>>{};

  for (auto &item : this->_items) {
    if (std::get<1>(item)) {
      result.push_back(std::get<0>(item));
    }
  }

  return result;
}
