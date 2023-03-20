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
  R"(_{struct buffer} buffer_copy (const _{struct buffer} o) {)" EOL
  R"(  unsigned char *d = _{alloc}(o.l);)" EOL
  R"(  _{memcpy}(d, o.d, o.l);)" EOL
  R"(  return (_{struct buffer}) {d, o.l};)" EOL
  R"(})" EOL,

  R"(_{bool} buffer_eq (_{struct buffer} o1, _{struct buffer} o2) {)" EOL
  R"(  _{bool} r = o1.l == o2.l && _{memcmp}(o1.d, o2.d, o1.l) == 0;)" EOL
  R"(  _{free}(o1.d);)" EOL
  R"(  _{free}(o2.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void buffer_free (_{struct buffer} o) {)" EOL
  R"(  _{free}(o.d);)" EOL
  R"(})" EOL,

  R"(_{bool} buffer_ne (_{struct buffer} o1, _{struct buffer} o2) {)" EOL
  R"(  _{bool} r = o1.l != o2.l || _{memcmp}(o1.d, o2.d, o1.l) != 0;)" EOL
  R"(  _{free}(o1.d);)" EOL
  R"(  _{free}(o2.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct buffer} buffer_realloc (_{struct buffer} o1, _{struct buffer} o2) {)" EOL
  R"(  _{free}(o1.d);)" EOL
  R"(  return o2;)" EOL
  R"(})" EOL,

  R"(_{struct str} buffer_str (_{struct buffer} b) {)" EOL
  R"(  _{size_t} l = 8 + (b.l * 3);)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, "[Buffer", 7);)" EOL
  R"(  for (_{size_t} i = 0; i < b.l; i++) _{sprintf}(d + 7 + (i * 3), " %02x", b.d[i]);)" EOL
  R"(  d[l - 1] = ']';)" EOL
  R"(  _{free}(b.d);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} buffer_to_str (_{struct buffer} b) {)" EOL
  R"(  return (_{struct str}) {(char *) b.d, b.l};)" EOL
  R"(})" EOL
};
