/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "VarMap.hpp"

Var &VarMap::add (const std::string &name, const std::string &codeName, const std::shared_ptr<Type> &type, bool mut, bool builtin) {
  this->_items.push_back(Var{name, codeName, type, mut, builtin, this->_frame});
  return this->_items.back();
}

const Var &VarMap::get (const std::string &name) const {
  for (auto it = this->_items.rbegin(); it != this->_items.rend(); it++) {
    if (it->name == name) {
      return *it;
    }
  }

  throw Error("Tried to access non existing variable map item");
}

bool VarMap::has (const std::string &name) const {
  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
    return it.name == name;
  });
}

std::string VarMap::name (const std::string &name) const {
  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto nameTest = name + "_" + std::to_string(idx);
    auto exists = false;

    for (const auto &item : this->_items) {
      if (item.codeName == nameTest) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      return nameTest;
    }
  }
}

void VarMap::restore () {
  for (auto it = this->_items.begin(); it != this->_items.end();) {
    if (it->_frame == this->_frame) {
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

std::vector<Var> VarMap::stack () const {
  auto stack = std::vector<Var>{};

  for (auto it = this->_items.rbegin(); it != this->_items.rend(); it++) {
    if (!it->builtin && !it->type->isObj()) {
      auto stackVar = std::find_if(stack.begin(), stack.end(), [&it] (const auto &it2) -> bool {
        return it2.name == it->name;
      });

      if (stackVar == stack.end()) {
        stack.push_back(*it);
      }
    }
  }

  return stack;
}
