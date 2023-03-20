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

#include "buffer.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenBuffer = {
  "_{struct buffer} buffer_copy (const _{struct buffer} o) {" EOL
  "  unsigned char *d = _{alloc}(o.l);" EOL
  "  _{memcpy}(d, o.d, o.l);" EOL
  "  return (_{struct buffer}) {d, o.l};" EOL
  "}" EOL,

  "_{bool} buffer_eq (_{struct buffer} o1, _{struct buffer} o2) {" EOL
  "  _{bool} r = o1.l == o2.l && _{memcmp}(o1.d, o2.d, o1.l) == 0;" EOL
  "  _{free}(o1.d);" EOL
  "  _{free}(o2.d);" EOL
  "  return r;" EOL
  "}" EOL,

  "void buffer_free (_{struct buffer} o) {" EOL
  "  _{free}(o.d);" EOL
  "}" EOL,

  "_{bool} buffer_ne (_{struct buffer} o1, _{struct buffer} o2) {" EOL
  "  _{bool} r = o1.l != o2.l || _{memcmp}(o1.d, o2.d, o1.l) != 0;" EOL
  "  _{free}(o1.d);" EOL
  "  _{free}(o2.d);" EOL
  "  return r;" EOL
  "}" EOL,

  "_{struct buffer} buffer_realloc (_{struct buffer} o1, _{struct buffer} o2) {" EOL
  "  _{free}(o1.d);" EOL
  "  return o2;" EOL
  "}" EOL,

  "_{struct str} buffer_str (_{struct buffer} b) {" EOL
  "  _{size_t} l = 8 + (b.l * 3);" EOL
  "  char *d = _{alloc}(l);" EOL
  R"(  _{memcpy}(d, "[Buffer", 7);)" EOL
  R"(  for (_{size_t} i = 0; i < b.l; i++) _{sprintf}(d + 7 + (i * 3), " %02x", b.d[i]);)" EOL
  "  d[l - 1] = ']';" EOL
  "  _{free}(b.d);" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL,

  "_{struct str} buffer_to_str (_{struct buffer} b) {" EOL
  "  return (_{struct str}) {(char *) b.d, b.l};" EOL
  "}" EOL
};
