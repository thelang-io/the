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

#include "any.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenAny = {
  "_{struct any} any_copy (const _{struct any} n) {" EOL
  "  return n.d == _{NULL} ? n : n._copy(n);" EOL
  "}" EOL,

  "void any_free (_{struct any} n) {" EOL
  "  if (n.d != _{NULL}) n._free(n);" EOL
  "}" EOL,

  "_{struct any} any_realloc (_{struct any} n1, _{struct any} n2) {" EOL
  "  if (n1.d != _{NULL}) n1._free(n1);" EOL
  "  return n2;" EOL
  "}" EOL,

  "_{struct str} any_str (_{struct any} n) {" EOL
  "  if (n.d != _{NULL}) n._free(n);" EOL
  R"(  return _{str_alloc}("any");)" EOL
  "}" EOL
};
