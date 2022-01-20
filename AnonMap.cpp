/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AnonMap.hpp"

std::string AnonMap::add (const std::string &name) {
  auto fullName = std::string();

  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto fullNameTest = name + std::to_string(idx);
    auto exists = false;

    for (const auto &item : this->_items) {
      if (item.name == fullNameTest) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      fullName = fullNameTest;
      break;
    }
  }

  this->_items.push_back(AnonMapItem{fullName});
  return fullName;
}
