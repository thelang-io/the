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

#ifndef SRC_VAR_MAP_HPP
#define SRC_VAR_MAP_HPP

#include "TypeMap.hpp"
#include "VarStack.hpp"

class VarMap {
 public:
  std::shared_ptr<Var> add (const std::string &, const std::string &, Type *, bool = false);
  std::shared_ptr<Var> get (const std::string &);
  bool has (const std::string &) const;
  void init (TypeMap &);
  std::string name (const std::string &) const;
  void restore ();
  void save ();
  VarStack varStack () const;

 private:
  std::size_t _frame = 0;
  std::vector<std::shared_ptr<Var>> _items;
};

#endif
