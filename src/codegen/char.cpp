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

#include "char.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenChar = {
  "_{bool} char_isDigit (char c) {" EOL
  "  return isdigit(c);" EOL
  "}" EOL,

  "_{bool} char_isLetter (char c) {" EOL
  "  return isalpha(c);" EOL
  "}" EOL,

  "_{bool} char_isLetterOrDigit (char c) {" EOL
  "  return isalnum(c);" EOL
  "}" EOL,

  "_{bool} char_isWhitespace (char c) {" EOL
  "  return isspace(c);" EOL
  "}" EOL,

  "_{struct str} char_repeat (char c, _{int32_t} k) {" EOL
  R"(  if (k <= 0) return _{str_alloc}("");)" EOL
  "  _{size_t} l = (_{size_t}) k;" EOL
  "  char *d = _{alloc}(l);" EOL
  "  _{size_t} i = 0;" EOL
  "  while (i < l) d[i++] = c;" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL,

  "_{struct str} char_str (char c) {" EOL
  "  char buf[2] = {c, '\\0'};" EOL
  "  return _{str_alloc}(buf);" EOL
  "}" EOL
};
