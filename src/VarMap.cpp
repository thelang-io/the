/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "VarMap.hpp"

void VarMap::add (VarMapItemType type, const std::string &name) {
  this->items.push_back({type, name});
}

const VarMapItem &VarMap::get (const std::string &name) const {
  for (const auto &it : this->items) {
    if (it.name == name) {
      return it;
    }
  }

  throw Error("Tried to access non existing VarMap item");
}
