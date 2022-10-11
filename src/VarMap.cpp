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

#include "VarMap.hpp"
#include <algorithm>
#include "Error.hpp"

std::shared_ptr<Var> VarMap::add (const std::string &name, const std::string &codeName, Type *type, bool mut) {
  this->_items.push_back(std::make_shared<Var>(Var{name, codeName, type, mut, false, this->_frame}));
  return this->_items.back();
}

std::shared_ptr<Var> VarMap::get (const std::string &name) {
  if (this->_items.empty()) {
    return nullptr;
  }

  for (auto idx = this->_items.size() - 1;; idx--) {
    auto item = this->_items[idx];

    if (item->name == name) {
      return item;
    }

    if (idx == 0) {
      break;
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
  this->_items.push_back(std::make_shared<Var>(Var{"exit", "@exit", typeMap.get("exit"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"os_EOL", "@os_EOL", typeMap.get("str"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"os_name", "@os_name", typeMap.get("os_name"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_basename", "@path_basename", typeMap.get("path_basename"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_dirname", "@path_dirname", typeMap.get("path_dirname"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"print", "@print", typeMap.get("print"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_args", "@process_args", typeMap.arrayOf(typeMap.get("str")), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_cwd", "@process_cwd", typeMap.get("process_cwd"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_runSync", "@process_runSync", typeMap.get("process_runSync"), false, true, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"sleepSync", "@sleepSync", typeMap.get("sleepSync"), false, true, this->_frame}));
}

std::string VarMap::name (const std::string &name) const {
  auto fullName = name + "_";

  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto fullNameTest = fullName + std::to_string(idx);
    auto exists = false;

    for (const auto &item : this->_items) {
      if (item->codeName == fullNameTest) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      return fullNameTest;
    }
  }
}

void VarMap::restore () {
  for (auto idx = static_cast<std::size_t>(0); idx < this->_items.size();) {
    auto item = this->_items[idx];

    if (item->frame == this->_frame) {
      this->_items.erase(this->_items.begin() + static_cast<std::ptrdiff_t>(idx));
      continue;
    }

    idx++;
  }

  this->_frame--;
}

void VarMap::save () {
  this->_frame++;
}

VarStack VarMap::varStack () const {
  if (this->_items.empty()) {
    return VarStack({});
  }

  auto result = std::vector<std::shared_ptr<Var>>{};

  for (auto idx = this->_items.size() - 1;; idx--) {
    auto item = this->_items[idx];

    if (!item->builtin) {
      auto stackVar = std::find_if(result.begin(), result.end(), [&item] (const auto &it2) -> bool {
        return it2->name == item->name;
      });

      if (stackVar == result.end()) {
        result.push_back(item);
      }
    }

    if (idx == 0) {
      break;
    }
  }

  return VarStack(result);
}
