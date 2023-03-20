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

#include "path.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenPath = {
  "_{struct str} path_basename (_{struct str} s) {" EOL
  "  if (s.l == 0) return s;" EOL
  "  #ifdef _{THE_OS_WINDOWS}" EOL
  R"(    char e = '\\';)" EOL
  "  #else" EOL
  "    char e = '/';" EOL
  "  #endif" EOL
  "  _{size_t} a = 0;" EOL
  "  _{size_t} b = 0;" EOL
  "  unsigned char k = 0;" EOL
  "  for (_{size_t} i = s.l - 1;; i--) {" EOL
  "    if (k == 1 && s.d[i] == e) {" EOL
  "      a = i + 1;" EOL
  "      break;" EOL
  "    } else if (k == 0 && s.d[i] != e) {" EOL
  "      k = 1;" EOL
  "      b = i + 1;" EOL
  "    }" EOL
  "    if (i == 0) break;" EOL
  "  }" EOL
  "  if (k == 0) {" EOL
  "    _{str_free}(s);" EOL
  R"(    return _{str_alloc}("");)" EOL
  "  }" EOL
  "  _{size_t} l = b - a;" EOL
  "  char *d = _{alloc}(l);" EOL
  "  _{memcpy}(d, &s.d[a], l);" EOL
  "  _{str_free}(s);" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL,

  "_{struct str} path_dirname (_{struct str} s) {" EOL
  "  if (s.l == 0) {" EOL
  "    _{str_free}(s);" EOL
  R"(    return _{str_alloc}(".");)" EOL
  "  }" EOL
  "  #ifdef _{THE_OS_WINDOWS}" EOL
  R"(    char e = '\\';)" EOL
  "  #else" EOL
  "    char e = '/';" EOL
  "  #endif" EOL
  "  _{size_t} l = 0;" EOL
  "  unsigned char k = 0;" EOL
  "  for (_{size_t} i = s.l - 1;; i--) {" EOL
  "    if (k == 1 && s.d[i] == e) {" EOL
  "      l = i;" EOL
  "      break;" EOL
  "    } else if (k == 0 && s.d[i] != e) {" EOL
  "      k = 1;" EOL
  "    }" EOL
  "    if (i == 0) break;" EOL
  "  }" EOL
  "  if (l == 0) {" EOL
  "    s.d = _{re_alloc}(s.d, 1);" EOL
  "    if (s.d[0] != e) s.d[0] = '.';" EOL
  "    return (_{struct str}) {s.d, 1};" EOL
  "  }" EOL
  "  char *d = _{alloc}(l);" EOL
  "  _{memcpy}(d, s.d, l);" EOL
  "  _{str_free}(s);" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL
};
