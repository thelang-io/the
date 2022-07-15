/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "VarMap.hpp"
#include "Error.hpp"

std::shared_ptr<Var> VarMap::add (const std::string &name, const std::string &codeName, Type *type, bool mut) {
  this->_items.push_back(std::make_shared<Var>(Var{name, codeName, type, mut, false, this->_frame}));
  return this->_items.back();
}

std::shared_ptr<Var> VarMap::get (const std::string &name) {
  for (auto it = this->_items.rbegin(); it != this->_items.rend(); it++) {
    if ((*it)->name == name) {
      return *it;
    }
  }

  return nullptr;
}

bool VarMap::has (const std::string &name) const {
  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
    return it->name == name;
  });
}

void VarMap::init (TypeMap &typeMap) {
  this->_items.push_back(std::make_shared<Var>(Var{"print", "@print", typeMap.get("print"), false, true, this->_frame}));
}

std::string VarMap::name (const std::string &name) const {
  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto nameTest = name + "_" + std::to_string(idx);
    auto exists = false;

    for (const auto &item : this->_items) {
      if (item->codeName == nameTest) {
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
    if ((*it)->frame == this->_frame) {
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

VarStack VarMap::stack () const {
  auto stack = std::vector<std::shared_ptr<Var>>{};

  for (auto it = this->_items.rbegin(); it != this->_items.rend(); it++) {
    if (!(*it)->builtin) {
      auto stackVar = std::find_if(stack.begin(), stack.end(), [&it] (const auto &it2) -> bool {
        return it2->name == (*it)->name;
      });

      if (stackVar == stack.end()) {
        stack.push_back(*it);
      }
    }
  }

  return VarStack(stack);
}
