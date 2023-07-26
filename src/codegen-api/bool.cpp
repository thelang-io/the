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

#include "bool.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenBool = {
  R"(_{struct str} bool_str (_{bool} t) {)" EOL
  R"(  return _{str_alloc}(t ? "true" : "false");)" EOL
  R"(})" EOL
};
