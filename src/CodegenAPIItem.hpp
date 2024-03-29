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

#ifndef SRC_CODEGEN_API_ITEM_HPP
#define SRC_CODEGEN_API_ITEM_HPP

#include <set>
#include <string>

struct CodegenAPIItem {
  bool enabled;
  std::string name;
  std::string decl;
  std::string def;
  std::set<std::string> dependencies;
  std::set<std::string> entityDependencies;
  bool throws;
};

#endif
