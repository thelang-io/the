/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VarStack.hpp"

VarStack::VarStack (const std::vector<std::shared_ptr<Var>> &items) {
  this->_items.reserve(items.size());

  for (const auto &it : items) {
    this->_items.emplace_back(it, false);
  }
}

void VarStack::mark (const std::string &codeName) {
  for (auto &it : this->_items) {
    if (std::get<0>(it)->codeName == codeName) {
      std::get<1>(it) = true;
      break;
    }
  }
}

void VarStack::mark (const std::shared_ptr<Var> &var) {
  for (auto &it : this->_items) {
    if (std::get<0>(it)->codeName == var->codeName) {
      std::get<1>(it) = true;
      break;
    }
  }
}

void VarStack::mark (const std::vector<std::shared_ptr<Var>> &vars) {
  for (const auto &var : vars) {
    this->mark(var);
  }
}

std::vector<std::shared_ptr<Var>> VarStack::snapshot () const {
  auto result = std::vector<std::shared_ptr<Var>>{};

  for (const auto &it : this->_items) {
    if (std::get<1>(it)) {
      result.push_back(std::get<0>(it));
    }
  }

  return result;
}
