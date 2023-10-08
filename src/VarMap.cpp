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

std::shared_ptr<Var> VarMap::add (const std::string &name, const std::string &codeName, Type *type, bool mut, bool constant, bool ctxIgnored) {
  this->_items.push_back(std::make_shared<Var>(Var{name, codeName, type, mut, constant, false, ctxIgnored, this->_frame}));
  return this->_items.back();
}

std::shared_ptr<Var> VarMap::addNamespace (const std::string &name, Type *type) {
  this->_items.push_back(std::make_shared<Var>(Var{name, this->name(name), type, false, false, false, true, this->_frame}));
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
  return std::any_of(this->_items.begin(), this->_items.end(), [&] (const auto &it) -> bool {
    return it->name == name;
  });
}

void VarMap::init (TypeMap &typeMap) {
  this->_items.push_back(std::make_shared<Var>(Var{"date_now", "@date_now", typeMap.get("date_now"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"error_NewError", "@error_NewError", typeMap.get("error_NewError"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_appendFileSync", "@fs_appendFileSync", typeMap.get("fs_appendFileSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_chmodSync", "@fs_chmodSync", typeMap.get("fs_chmodSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_chownSync", "@fs_chownSync", typeMap.get("fs_chownSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_copyDirectorySync", "@fs_copyDirectorySync", typeMap.get("fs_copyDirectorySync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_copyFileSync", "@fs_copyFileSync", typeMap.get("fs_copyFileSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_existsSync", "@fs_existsSync", typeMap.get("fs_existsSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_isAbsoluteSync", "@fs_isAbsoluteSync", typeMap.get("fs_isAbsoluteSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_isDirectorySync", "@fs_isDirectorySync", typeMap.get("fs_isDirectorySync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_isFileSync", "@fs_isFileSync", typeMap.get("fs_isFileSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_isSymbolicLinkSync", "@fs_isSymbolicLinkSync", typeMap.get("fs_isSymbolicLinkSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_linkSync", "@fs_linkSync", typeMap.get("fs_linkSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_mkdirSync", "@fs_mkdirSync", typeMap.get("fs_mkdirSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_readFileSync", "@fs_readFileSync", typeMap.get("fs_readFileSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_realpathSync", "@fs_realpathSync", typeMap.get("fs_realpathSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_renameSync", "@fs_renameSync", typeMap.get("fs_renameSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_rmSync", "@fs_rmSync", typeMap.get("fs_rmSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_rmdirSync", "@fs_rmdirSync", typeMap.get("fs_rmdirSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_scandirSync", "@fs_scandirSync", typeMap.get("fs_scandirSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_statSync", "@fs_statSync", typeMap.get("fs_statSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_unlinkSync", "@fs_unlinkSync", typeMap.get("fs_unlinkSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"fs_writeFileSync", "@fs_writeFileSync", typeMap.get("fs_writeFileSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxFloat", "@math_MaxFloat", typeMap.get("float"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinFloat", "@math_MinFloat", typeMap.get("float"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxF32", "@math_MaxF32", typeMap.get("f32"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinF32", "@math_MinF32", typeMap.get("f32"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxF64", "@math_MaxF64", typeMap.get("f64"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinF64", "@math_MinF64", typeMap.get("f64"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxInt", "@math_MaxInt", typeMap.get("int"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinInt", "@math_MinInt", typeMap.get("int"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxI8", "@math_MaxI8", typeMap.get("i8"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinI8", "@math_MinI8", typeMap.get("i8"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxI16", "@math_MaxI16", typeMap.get("i16"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinI16", "@math_MinI16", typeMap.get("i16"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxI32", "@math_MaxI32", typeMap.get("i32"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinI32", "@math_MinI32", typeMap.get("i32"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxI64", "@math_MaxI64", typeMap.get("i64"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinI64", "@math_MinI64", typeMap.get("i64"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxU8", "@math_MaxU8", typeMap.get("u8"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinU8", "@math_MinU8", typeMap.get("u8"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxU16", "@math_MaxU16", typeMap.get("u16"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinU16", "@math_MinU16", typeMap.get("u16"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxU32", "@math_MaxU32", typeMap.get("u32"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinU32", "@math_MinU32", typeMap.get("u32"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MaxU64", "@math_MaxU64", typeMap.get("u64"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"math_MinU64", "@math_MinU64", typeMap.get("u64"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"os_ARCH", "@os_ARCH", typeMap.get("str"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"os_EOL", "@os_EOL", typeMap.get("str"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"os_NAME", "@os_NAME", typeMap.get("str"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_SEP", "@path_SEP", typeMap.get("str"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_basename", "@path_basename", typeMap.get("path_basename"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_dirname", "@path_dirname", typeMap.get("path_dirname"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_tempDirectory", "@path_tempDirectory", typeMap.get("path_tempDirectory"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"path_tempFile", "@path_tempFile", typeMap.get("path_tempFile"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"print", "@print", typeMap.get("print"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_args", "@process_args", typeMap.createArr(typeMap.get("str")), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_cwd", "@process_cwd", typeMap.get("process_cwd"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_env", "@process_env", typeMap.createMap(typeMap.get("str"), typeMap.get("str")), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_exit", "@process_exit", typeMap.get("process_exit"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_getgid", "@process_getgid", typeMap.get("process_getgid"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_getuid", "@process_getuid", typeMap.get("process_getuid"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_home", "@process_home", typeMap.get("str"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"process_runSync", "@process_runSync", typeMap.get("process_runSync"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"random_randomFloat", "@random_randomFloat", typeMap.get("random_randomFloat"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"random_randomInt", "@random_randomInt", typeMap.get("random_randomInt"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"random_randomStr", "@random_randomStr", typeMap.get("random_randomStr"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"request_open", "@request_open", typeMap.get("request_open"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"thread_id", "@thread_id", typeMap.get("thread_id"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"thread_sleep", "@thread_sleep", typeMap.get("thread_sleep"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"url_parse", "@url_parse", typeMap.get("url_parse"), false, false, true, false, this->_frame}));
  this->_items.push_back(std::make_shared<Var>(Var{"utils_swap", "@utils_swap", typeMap.get("utils_swap"), false, false, true, false, this->_frame}));
}

std::shared_ptr<Var> VarMap::insert (const std::shared_ptr<Var> &var) {
  this->_items.push_back(std::make_shared<Var>(*var));
  return this->_items.back();
}

std::vector<std::shared_ptr<Var>> VarMap::methods () const {
  auto result = std::vector<std::shared_ptr<Var>>{};

  for (const auto &it : this->_items) {
    if (it->type->isMethod()) {
      result.push_back(it);
    }
  }

  return result;
}

std::string VarMap::name (const std::string &name) const {
  auto fullName = name + "_";

  for (auto i = static_cast<std::size_t>(0);; i++) {
    auto fullNameTest = fullName + std::to_string(i);
    auto exists = false;

    for (const auto &it : this->_items) {
      if (it->codeName == fullNameTest) {
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
  for (auto i = static_cast<std::size_t>(0); i < this->_items.size();) {
    auto item = this->_items[i];

    if (item->frame == this->_frame) {
      this->_items.erase(this->_items.begin() + static_cast<std::ptrdiff_t>(i));
      continue;
    }

    i++;
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

  for (auto it = this->_items.rbegin(); it != this->_items.rend(); it++) {
    if (!(*it)->builtin && !(*it)->ctxIgnored) {
      auto stackVar = std::find_if(result.begin(), result.end(), [&] (const auto &it2) -> bool {
        return it2->codeName == (*it)->codeName;
      });

      if (stackVar == result.end()) {
        result.push_back(*it);
      }
    }
  }

  return VarStack(result);
}
