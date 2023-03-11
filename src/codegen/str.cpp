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

#include "str.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenStr = {
  "_{struct str} str_calloc (const char *r, _{size_t} l) {" EOL
  "  char *d = _{alloc}(l);" EOL
  "  _{memcpy}(d, r, l);" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL,

  "char *str_at (_{struct str} s, _{int32_t} i) {" EOL
  "  if ((i >= 0 && i >= s.l) || (i < 0 && i < -((_{int32_t}) s.l))) {" EOL
  R"(    _{fprintf}(_{stderr}, "Error: index %" _{PRId32} " out of string bounds" _{THE_EOL}, i);)" EOL
  "    _{exit}(_{EXIT_FAILURE});" EOL
  "  }" EOL
  "  return i < 0 ? &s.d[s.l + i] : &s.d[i];" EOL
  "}" EOL,

  "_{bool} str_contains (_{struct str} self, _{struct str} n1) {" EOL
  "  _{bool} r = n1.l == 0;" EOL
  "  if (!r && self.l == n1.l) {" EOL
  "    r = _{memcmp}(self.d, n1.d, n1.l) == 0;" EOL
  "  } else if (!r && self.l > n1.l) {" EOL
  "    for (_{size_t} i = 0; i < self.l - n1.l; i++) {" EOL
  "      if (_{memcmp}(&self.d[i], n1.d, n1.l) == 0) {" EOL
  "        r = _{true};" EOL
  "        break;" EOL
  "      }" EOL
  "    }" EOL
  "  }" EOL
  "  _{free}(self.d);" EOL
  "  _{free}(n1.d);" EOL
  "  return r;" EOL
  "}" EOL,

  "_{bool} str_empty (_{struct str} s) {" EOL
  "  _{bool} r = s.l == 0;" EOL
  "  _{free}(s.d);" EOL
  "  return r;" EOL
  "}" EOL,

  "_{int32_t} str_find (_{struct str} s1, _{struct str} s2) {" EOL
  "  _{int32_t} r = -1;" EOL
  "  for (_{size_t} i = 0; i < s1.l; i++) {" EOL
  "    if (_{memcmp}(&s1.d[i], s2.d, s2.l) == 0) {" EOL
  "      r = (_{int32_t}) i;" EOL
  "      break;" EOL
  "    }" EOL
  "  }" EOL
  "  _{free}(s1.d);" EOL
  "  _{free}(s2.d);" EOL
  "  return r;" EOL
  "}" EOL,

  "_{size_t} str_len (_{struct str} s) {" EOL
  "  _{size_t} l = s.l;" EOL
  "  _{free}(s.d);" EOL
  "  return l;" EOL
  "}" EOL,

  "struct _{array_str} str_lines (_{struct str} s, unsigned char o1, _{bool} n1) {" EOL
  "  _{bool} k = o1 == 0 ? _{false} : n1;" EOL
  "  _{struct str} *r = _{NULL};" EOL
  "  _{size_t} l = 0;" EOL
  "  if (s.l != 0) {" EOL
  "    char *d = _{alloc}(s.l);" EOL
  "    _{size_t} i = 0;" EOL
  "    for (_{size_t} j = 0; j < s.l; j++) {" EOL
  "      char c = s.d[j];" EOL
  R"(      if (c == '\r' || c == '\n') {)" EOL
  "        if (k) d[i++] = c;" EOL
  R"(        if (c == '\r' && j + 1 < s.l && s.d[j + 1] == '\n') {)" EOL
  "          j++;" EOL
  "          if (k) d[i++] = s.d[j];" EOL
  "        }" EOL
  "        char *a = _{alloc}(i);" EOL
  "        _{memcpy}(a, d, i);" EOL
  "        r = _{re_alloc}(r, ++l * sizeof(_{struct str}));" EOL
  "        r[l - 1] = (_{struct str}) {a, i};" EOL
  "        i = 0;" EOL
  "      } else {" EOL
  "        d[i++] = c;" EOL
  "      }" EOL
  "    }" EOL
  "    if (i != 0) {" EOL
  "      char *a = _{alloc}(i);" EOL
  "      _{memcpy}(a, d, i);" EOL
  "      r = _{re_alloc}(r, ++l * sizeof(_{struct str}));" EOL
  "      r[l - 1] = (_{struct str}) {a, i};" EOL
  "    }" EOL
  "    _{free}(d);" EOL
  "  }" EOL
  "  _{free}(s.d);" EOL
  "  return (struct _{array_str}) {r, l};" EOL
  "}" EOL,

  "_{struct str} str_lower (_{struct str} s) {" EOL
  "  if (s.l != 0) {" EOL
  "    for (_{size_t} i = 0; i < s.l; i++) s.d[i] = (char) _{tolower}(s.d[i]);" EOL
  "  }" EOL
  "  return s;" EOL
  "}" EOL,

  "_{struct str} str_lowerFirst (_{struct str} s) {" EOL
  "  if (s.l != 0) s.d[0] = (char) _{tolower}(s.d[0]);" EOL
  "  return s;" EOL
  "}" EOL,

  "_{bool} str_not (_{struct str} s) {" EOL
  "  _{bool} r = s.l == 0;" EOL
  "  _{free}(s.d);" EOL
  "  return r;" EOL
  "}" EOL,

  "_{struct str} str_replace (_{struct str} self, _{struct str} n1, _{struct str} n2, unsigned char _o3, _{int32_t} n3) {" EOL
  "  _{size_t} l = 0;" EOL
  "  char *d = _{NULL};" EOL
  "  _{int32_t} k = 0;" EOL
  "  if (n1.l == 0 && n2.l > 0) {" EOL
  "    l = self.l + (n3 > 0 && n3 <= self.l ? n3 : self.l + 1) * n2.l;" EOL
  "    d = _{alloc}(l);" EOL
  "    _{memcpy}(d, n2.d, n2.l);" EOL
  "    _{size_t} j = n2.l;" EOL
  "    for (_{size_t} i = 0; i < self.l; i++) {" EOL
  "      d[j++] = self.d[i];" EOL
  "      if (n3 <= 0 || ++k < n3) {" EOL
  "        _{memcpy}(&d[j], n2.d, n2.l);" EOL
  "        j += n2.l;" EOL
  "      }" EOL
  "    }" EOL
  "  } else if (self.l == n1.l && n1.l > 0) {" EOL
  "    if (_{memcmp}(self.d, n1.d, n1.l) != 0) {" EOL
  "      l = self.l;" EOL
  "      d = _{alloc}(l);" EOL
  "      _{memcpy}(d, self.d, l);" EOL
  "    } else if (n2.l > 0) {" EOL
  "      l = n2.l;" EOL
  "      d = _{alloc}(l);" EOL
  "      _{memcpy}(d, n2.d, l);" EOL
  "    }" EOL
  "  } else if (self.l > n1.l && n1.l > 0 && n2.l == 0) {" EOL
  "    d = _{alloc}(self.l);" EOL
  "    for (_{size_t} i = 0; i < self.l; i++) {" EOL
  "      if (i <= self.l - n1.l && _{memcmp}(&self.d[i], n1.d, n1.l) == 0 && (n3 <= 0 || k++ < n3)) {" EOL
  "        i += n1.l - 1;" EOL
  "      } else {" EOL
  "        d[l++] = self.d[i];" EOL
  "      }" EOL
  "    }" EOL
  "    if (l == 0) {" EOL
  "      _{free}(d);" EOL
  "      d = _{NULL};" EOL
  "    } else if (l != self.l) {" EOL
  "      d = _{re_alloc}(d, l);" EOL
  "    }" EOL
  "  } else if (self.l > n1.l && n1.l > 0 && n2.l > 0) {" EOL
  "    l = self.l;" EOL
  "    d = _{alloc}(l);" EOL
  "    _{size_t} j = 0;" EOL
  "    for (_{size_t} i = 0; i < self.l; i++) {" EOL
  "      if (i <= self.l - n1.l && _{memcmp}(&self.d[i], n1.d, n1.l) == 0 && (n3 <= 0 || k++ < n3)) {" EOL
  "        if (n1.l < n2.l) {" EOL
  "          l += n2.l - n1.l;" EOL
  "          if (l > self.l) {" EOL
  "            d = _{re_alloc}(d, l);" EOL
  "          }" EOL
  "        } else if (n1.l > n2.l) {" EOL
  "          l -= n1.l - n2.l;" EOL
  "        }" EOL
  "        _{memcpy}(&d[j], n2.d, n2.l);" EOL
  "        j += n2.l;" EOL
  "        i += n1.l - 1;" EOL
  "      } else {" EOL
  "        d[j++] = self.d[i];" EOL
  "      }" EOL
  "    }" EOL
  "    d = _{re_alloc}(d, l);" EOL
  "  } else if (self.l > 0) {" EOL
  "    l = self.l;" EOL
  "    d = _{alloc}(l);" EOL
  "    _{memcpy}(d, self.d, l);" EOL
  "  }" EOL
  "  _{free}(n2.d);" EOL
  "  _{free}(n1.d);" EOL
  "  _{free}(self.d);" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL,

  "_{struct str} str_slice (_{struct str} s, unsigned char o1, _{int32_t} n1, unsigned char o2, _{int32_t} n2) {" EOL
  "  _{int32_t} i1 = o1 == 0 ? 0 : (_{int32_t}) (n1 < 0 ? (n1 < -((_{int32_t}) s.l) ? 0 : n1 + s.l) : (n1 > s.l ? s.l : n1));" EOL
  "  _{int32_t} i2 = o2 == 0 ? (_{int32_t}) s.l : (_{int32_t}) (n2 < 0 ? (n2 < -((_{int32_t}) s.l) ? 0 : n2 + s.l) : (n2 > s.l ? s.l : n2));" EOL
  "  if (i1 >= i2 || i1 >= s.l) {" EOL
  "    _{free}(s.d);" EOL
  R"(    return _{str_alloc}("");)" EOL
  "  }" EOL
  "  _{size_t} l = i2 - i1;" EOL
  "  char *d = _{alloc}(l);" EOL
  "  for (_{size_t} i = 0; i1 < i2; i1++) d[i++] = s.d[i1];" EOL
  "  _{free}(s.d);" EOL
  "  return (_{struct str}) {d, l};" EOL
  "}" EOL,

  "struct _{array_str} str_split (_{struct str} self, unsigned char o1, _{struct str} n1) {" EOL
  "  _{struct str} *r = _{NULL};" EOL
  "  _{size_t} l = 0;" EOL
  "  if (self.l > 0 && n1.l == 0) {" EOL
  "    l = self.l;" EOL
  "    r = _{alloc}(l * sizeof(_{struct str}));" EOL
  "    for (_{size_t} i = 0; i < l; i++) {" EOL
  R"(      r[i] = _{str_calloc}(&self.d[i], 1);)" EOL
  "    }" EOL
  "  } else if (self.l < n1.l) {" EOL
  "    r = _{re_alloc}(r, ++l * sizeof(_{struct str}));" EOL
  "    r[0] = _{str_calloc}(self.d, self.l);" EOL
  "  } else if (n1.l > 0) {" EOL
  "    _{size_t} i = 0;" EOL
  "    for (_{size_t} j = 0; j <= self.l - n1.l; j++) {" EOL
  "      if (_{memcmp}(&self.d[j], n1.d, n1.l) == 0) {" EOL
  "        r = _{re_alloc}(r, ++l * sizeof(_{struct str}));" EOL
  "        r[l - 1] = _{str_calloc}(&self.d[i], j - i);" EOL
  "        j += n1.l;" EOL
  "        i = j;" EOL
  "      }" EOL
  "    }" EOL
  "    r = _{re_alloc}(r, ++l * sizeof(_{struct str}));" EOL
  "    r[l - 1] = _{str_calloc}(&self.d[i], self.l - i);" EOL
  "  }" EOL
  "  _{free}(n1.d);" EOL
  "  _{free}(self.d);" EOL
  "  return (struct _{array_str}) {r, l};" EOL
  "}" EOL,

  "struct buffer str_toBuffer (_{struct str} s) {" EOL
  "  return (_{struct buffer}) {(unsigned char *) s.d, s.l};" EOL
  "}" EOL,

  "_{struct str} str_trim (_{struct str} s) {" EOL
  "  if (s.l == 0) return s;" EOL
  "  _{size_t} i = 0;" EOL
  "  _{size_t} j = s.l;" EOL
  "  while (i < s.l && _{isspace}(s.d[i])) i++;" EOL
  "  while (j >= 0 && _{isspace}(s.d[j - 1])) {" EOL
  "    j--;" EOL
  "    if (j == 0) break;" EOL
  "  }" EOL
  "  if (i >= j) {" EOL
  "    _{free}(s.d);" EOL
  R"(    return _{str_alloc}("");)" EOL
  "  }" EOL
  "  _{size_t} l = j - i;" EOL
  "  char *r = _{alloc}(l);" EOL
  "  for (_{size_t} k = 0; k < l;) r[k++] = s.d[i++];" EOL
  "  _{free}(s.d);" EOL
  "  return (_{struct str}) {r, l};" EOL
  "}" EOL,

  "_{int32_t} str_toInt (_{struct str} self, unsigned char o1, _{int32_t} n1) {" EOL
  "  char *c = _{str_cstr}(self);" EOL
  "  char *e = _{NULL};" EOL
  "  _{errno} = 0;" EOL
  "  long r = _{strtol}(c, &e, o1 == 0 ? 10 : n1);" EOL
  "  if (_{errno} != 0 || *e != 0) {" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  "    _{exit}(_{EXIT_FAILURE});" EOL
  "  } else if (_{errno} == _{ERANGE} || r < _{INT32_MIN} || _{INT32_MAX} < r) {" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  "    _{exit}(_{EXIT_FAILURE});" EOL
  "  }" EOL
  "  _{free}(c);" EOL
  "  _{free}(self.d);" EOL
  "  return (_{int32_t}) r;" EOL
  "}" EOL,

  "_{struct str} str_trimEnd (_{struct str} s) {" EOL
  "  if (s.l == 0) return s;" EOL
  "  while (_{isspace}(s.d[s.l - 1])) {" EOL
  "    s.l--;" EOL
  "    if (s.l == 0) break;" EOL
  "  }" EOL
  "  if (s.l == 0) {" EOL
  "    _{free}(s.d);" EOL
  "    s.d = _{NULL};" EOL
  "  } else {" EOL
  "    s.d = _{re_alloc}(s.d, s.l);" EOL
  "  }" EOL
  "  return s;" EOL
  "}" EOL,

  "_{struct str} str_trimStart (_{struct str} s) {" EOL
  "  if (s.l == 0) return s;" EOL
  "  _{size_t} i = 0;" EOL
  "  while (i < s.l && _{isspace}(s.d[i])) i++;" EOL
  "  if (i >= s.l) {" EOL
  "    _{free}(s.d);" EOL
  "    s = (_{struct str}) {_{NULL}, 0};" EOL
  "  } else {" EOL
  "    _{memmove}(s.d, &s.d[i], s.l - i);" EOL
  "    s.l -= i;" EOL
  "    s.d = _{re_alloc}(s.d, s.l);" EOL
  "  }" EOL
  "  return s;" EOL
  "}" EOL,

  "_{struct str} str_upper (_{struct str} s) {" EOL
  "  if (s.l != 0) {" EOL
  "    for (_{size_t} i = 0; i < s.l; i++) s.d[i] = (char) _{toupper}(s.d[i]);" EOL
  "  }" EOL
  "  return s;" EOL
  "}" EOL,

  "_{struct str} str_upperFirst (_{struct str} s) {" EOL
  "  if (s.l != 0) s.d[0] = (char) _{toupper}(s.d[0]);" EOL
  "  return s;" EOL
  "}" EOL
};
