/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "VarMap.hpp"

Var &VarMap::add (const std::string &name, const std::shared_ptr<Type> &type, bool mut, const std::optional<std::variant<std::shared_ptr<Fn>, std::shared_ptr<Type>>> &ref) {
  this->_items.push_back(Var{name, type, mut, ref, this->_frame});
  return this->_items.back();
}

const Var &VarMap::get (const std::string &name) const {
  for (const auto &item : this->_items) {
    if (item.name == name) {
      return item;
    }
  }

  throw Error("Tried to access non existing variable map item");
}

bool VarMap::has (const std::string &name) const {
  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (auto it) -> bool {
    return it.name == name;
  });
}

void VarMap::restore () {
  for (auto it = this->_items.begin(); it != this->_items.end();) {
    if (it->frame == this->_frame) {
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
