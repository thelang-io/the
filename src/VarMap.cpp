/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "VarMap.hpp"

Var &VarMap::add (const std::string &name, const Type &type, bool mut, const std::optional<std::variant<std::shared_ptr<Fn>, std::shared_ptr<Type>>> &ref) {
  this->_items.push_back(Var{name, type, mut, ref, this->_frame});
  return this->_items.back();
}

const Var &VarMap::get (const std::string &name) const {
  for (auto it = this->_items.begin(); it != this->_items.end();) {
    auto item = *it;
    auto idx = static_cast<std::size_t>(std::distance(this->_items.begin(), it));

    if (item.name == name) {
      return this->_items[idx];
    }

    it++;
  }

  throw Error("Tried to access non existing variable map item");
}

void VarMap::restore () {
  for (auto it = this->_items.begin(); it != this->_items.end();) {
    auto item = *it;

    if (item.frame == this->_frame) {
      this->_items.erase(it);
      continue;
    }

    it++;
  }

  this->_frame--;
}

void VarMap::save () {
  this->_frame++;
}
