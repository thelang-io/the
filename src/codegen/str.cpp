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

// todo errors
const std::vector<std::string> codegenStr = {
  R"(_{struct str} cstr_concat_str (const char *r, _{struct str} s) {)" EOL
  R"(  _{size_t} l = s.l + _{strlen}(r);)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, r, l - s.l);)" EOL
  R"(  _{memcpy}(&d[l - s.l], s.d, s.l);)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{bool} cstr_eq_cstr (const char *c1, const char *c2) {)" EOL
  R"(  _{size_t} l = _{strlen}(c1);)" EOL
  R"(  return l == _{strlen}(c2) && _{memcmp}(c1, c2, l) == 0;)" EOL
  R"(})" EOL,

  R"(_{bool} cstr_eq_str (const char *c, _{struct str} s) {)" EOL
  R"(  _{bool} r = s.l == _{strlen}(c) && _{memcmp}(s.d, c, s.l) == 0;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{bool} cstr_ne_cstr (const char *c1, const char *c2) {)" EOL
  R"(  _{size_t} l = _{strlen}(c1);)" EOL
  R"(  return l != _{strlen}(c2) || _{memcmp}(c1, c2, l) != 0;)" EOL
  R"(})" EOL,

  R"(_{bool} cstr_ne_str (const char *d, _{struct str} s) {)" EOL
  R"(  _{bool} r = s.l != _{strlen}(d) || _{memcmp}(s.d, d, s.l) != 0;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_alloc (const char *r) {)" EOL
  R"(  _{size_t} l = _{strlen}(r);)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, r, l);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(char *str_at (_{struct str} s, _{int32_t} i) {)" EOL
  R"(  if ((i >= 0 && i >= s.l) || (i < 0 && i < -((_{int32_t}) s.l))) {)" EOL
  R"(    const char *fmt = "index %" _{PRId32} " out of string bounds";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, i);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, i);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, _{str_alloc}("")));)" EOL
  R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  })" EOL
  R"(  return i < 0 ? &s.d[s.l + i] : &s.d[i];)" EOL
  R"(})" EOL,

  R"(_{struct str} str_calloc (const char *r, _{size_t} l) {)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, r, l);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} str_concat_cstr (_{struct str} s, const char *r) {)" EOL
  R"(  _{size_t} l = s.l + _{strlen}(r);)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, s.d, s.l);)" EOL
  R"(  _{memcpy}(&d[s.l], r, l - s.l);)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} str_concat_str (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  _{size_t} l = s1.l + s2.l;)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, s1.d, s1.l);)" EOL
  R"(  _{memcpy}(&d[s1.l], s2.d, s2.l);)" EOL
  R"(  _{free}(s1.d);)" EOL
  R"(  _{free}(s2.d);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{bool} str_contains (_{struct str} self, _{struct str} n1) {)" EOL
  R"(  _{bool} r = n1.l == 0;)" EOL
  R"(  if (!r && self.l == n1.l) {)" EOL
  R"(    r = _{memcmp}(self.d, n1.d, n1.l) == 0;)" EOL
  R"(  } else if (!r && self.l > n1.l) {)" EOL
  R"(    for (_{size_t} i = 0; i < self.l - n1.l; i++) {)" EOL
  R"(      if (_{memcmp}(&self.d[i], n1.d, n1.l) == 0) {)" EOL
  R"(        r = _{true};)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  _{free}(n1.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_copy (const _{struct str} s) {)" EOL
  R"(  char *d = _{alloc}(s.l);)" EOL
  R"(  _{memcpy}(d, s.d, s.l);)" EOL
  R"(  return (_{struct str}) {d, s.l};)" EOL
  R"(})" EOL,

  R"(char *str_cstr (const _{struct str} s) {)" EOL
  R"(  char *d = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(d, s.d, s.l);)" EOL
  R"(  d[s.l] = '\0';)" EOL
  R"(  return d;)" EOL
  R"(})" EOL,

  R"(_{bool} str_empty (_{struct str} s) {)" EOL
  R"(  _{bool} r = s.l == 0;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{bool} str_eq_cstr (_{struct str} s, const char *r) {)" EOL
  R"(  _{bool} d = s.l == _{strlen}(r) && _{memcmp}(s.d, r, s.l) == 0;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return d;)" EOL
  R"(})" EOL,

  R"(_{bool} str_eq_str (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  _{bool} r = s1.l == s2.l && _{memcmp}(s1.d, s2.d, s1.l) == 0;)" EOL
  R"(  _{free}(s1.d);)" EOL
  R"(  _{free}(s2.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_escape (const _{struct str} s) {)" EOL
  R"(  char *d = _{alloc}(s.l);)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  for (_{size_t} i = 0; i < s.l; i++) {)" EOL
  R"(    char c = s.d[i];)" EOL
  R"(    if (c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '"') {)" EOL
  R"(      if (l + 2 > s.l) d = _{re_alloc}(d, l + 2);)" EOL
  R"(      d[l++] = '\\';)" EOL
  R"(      if (c == '\f') d[l++] = 'f';)" EOL
  R"(      else if (c == '\n') d[l++] = 'n';)" EOL
  R"(      else if (c == '\r') d[l++] = 'r';)" EOL
  R"(      else if (c == '\t') d[l++] = 't';)" EOL
  R"(      else if (c == '\v') d[l++] = 'v';)" EOL
  R"(      else if (c == '"') d[l++] = '"';)" EOL
  R"(      continue;)" EOL
  R"(    })" EOL
  R"(    if (l + 1 > s.l) d = _{re_alloc}(d, l + 1);)" EOL
  R"(    d[l++] = c;)" EOL
  R"(  })" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{int32_t} str_find (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  _{int32_t} r = -1;)" EOL
  R"(  for (_{size_t} i = 0; i < s1.l; i++) {)" EOL
  R"(    if (_{memcmp}(&s1.d[i], s2.d, s2.l) == 0) {)" EOL
  R"(      r = (_{int32_t}) i;)" EOL
  R"(      break;)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{free}(s1.d);)" EOL
  R"(  _{free}(s2.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void str_free (_{struct str} s) {)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(})" EOL,

  R"(_{size_t} str_len (_{struct str} s) {)" EOL
  R"(  _{size_t} l = s.l;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return l;)" EOL
  R"(})" EOL,

  R"(_{bool} str_ne_cstr (_{struct str} s, const char *c) {)" EOL
  R"(  _{bool} r = s.l != _{strlen}(c) || _{memcmp}(s.d, c, s.l) != 0;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{bool} str_ne_str (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  _{bool} r = s1.l != s2.l || _{memcmp}(s1.d, s2.d, s1.l) != 0;)" EOL
  R"(  _{free}(s1.d);)" EOL
  R"(  _{free}(s2.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_realloc (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  _{free}(s1.d);)" EOL
  R"(  return s2;)" EOL
  R"(})" EOL,

  R"(struct _{array_str} str_lines (_{struct str} s, unsigned char o1, _{bool} n1) {)" EOL
  R"(  _{bool} k = o1 == 0 ? _{false} : n1;)" EOL
  R"(  _{struct str} *r = _{NULL};)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  if (s.l != 0) {)" EOL
  R"(    char *d = _{alloc}(s.l);)" EOL
  R"(    _{size_t} i = 0;)" EOL
  R"(    for (_{size_t} j = 0; j < s.l; j++) {)" EOL
  R"(      char c = s.d[j];)" EOL
  R"(      if (c == '\r' || c == '\n') {)" EOL
  R"(        if (k) d[i++] = c;)" EOL
  R"(        if (c == '\r' && j + 1 < s.l && s.d[j + 1] == '\n') {)" EOL
  R"(          j++;)" EOL
  R"(          if (k) d[i++] = s.d[j];)" EOL
  R"(        })" EOL
  R"(        char *a = _{alloc}(i);)" EOL
  R"(        _{memcpy}(a, d, i);)" EOL
  R"(        r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(        r[l - 1] = (_{struct str}) {a, i};)" EOL
  R"(        i = 0;)" EOL
  R"(      } else {)" EOL
  R"(        d[i++] = c;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(    if (i != 0) {)" EOL
  R"(      char *a = _{alloc}(i);)" EOL
  R"(      _{memcpy}(a, d, i);)" EOL
  R"(      r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(      r[l - 1] = (_{struct str}) {a, i};)" EOL
  R"(    })" EOL
  R"(    _{free}(d);)" EOL
  R"(  })" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return (struct _{array_str}) {r, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} str_lower (_{struct str} s) {)" EOL
  R"(  if (s.l != 0) {)" EOL
  R"(    for (_{size_t} i = 0; i < s.l; i++) s.d[i] = (char) _{tolower}(s.d[i]);)" EOL
  R"(  })" EOL
  R"(  return s;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_lowerFirst (_{struct str} s) {)" EOL
  R"(  if (s.l != 0) s.d[0] = (char) _{tolower}(s.d[0]);)" EOL
  R"(  return s;)" EOL
  R"(})" EOL,

  R"(_{bool} str_not (_{struct str} s) {)" EOL
  R"(  _{bool} r = s.l == 0;)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_replace (_{struct str} self, _{struct str} n1, _{struct str} n2, unsigned char _o3, _{int32_t} n3) {)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  char *d = _{NULL};)" EOL
  R"(  _{int32_t} k = 0;)" EOL
  R"(  if (n1.l == 0 && n2.l > 0) {)" EOL
  R"(    l = self.l + (n3 > 0 && n3 <= self.l ? n3 : self.l + 1) * n2.l;)" EOL
  R"(    d = _{alloc}(l);)" EOL
  R"(    _{memcpy}(d, n2.d, n2.l);)" EOL
  R"(    _{size_t} j = n2.l;)" EOL
  R"(    for (_{size_t} i = 0; i < self.l; i++) {)" EOL
  R"(      d[j++] = self.d[i];)" EOL
  R"(      if (n3 <= 0 || ++k < n3) {)" EOL
  R"(        _{memcpy}(&d[j], n2.d, n2.l);)" EOL
  R"(        j += n2.l;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(  } else if (self.l == n1.l && n1.l > 0) {)" EOL
  R"(    if (_{memcmp}(self.d, n1.d, n1.l) != 0) {)" EOL
  R"(      l = self.l;)" EOL
  R"(      d = _{alloc}(l);)" EOL
  R"(      _{memcpy}(d, self.d, l);)" EOL
  R"(    } else if (n2.l > 0) {)" EOL
  R"(      l = n2.l;)" EOL
  R"(      d = _{alloc}(l);)" EOL
  R"(      _{memcpy}(d, n2.d, l);)" EOL
  R"(    })" EOL
  R"(  } else if (self.l > n1.l && n1.l > 0 && n2.l == 0) {)" EOL
  R"(    d = _{alloc}(self.l);)" EOL
  R"(    for (_{size_t} i = 0; i < self.l; i++) {)" EOL
  R"(      if (i <= self.l - n1.l && _{memcmp}(&self.d[i], n1.d, n1.l) == 0 && (n3 <= 0 || k++ < n3)) {)" EOL
  R"(        i += n1.l - 1;)" EOL
  R"(      } else {)" EOL
  R"(        d[l++] = self.d[i];)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(    if (l == 0) {)" EOL
  R"(      _{free}(d);)" EOL
  R"(      d = _{NULL};)" EOL
  R"(    } else if (l != self.l) {)" EOL
  R"(      d = _{re_alloc}(d, l);)" EOL
  R"(    })" EOL
  R"(  } else if (self.l > n1.l && n1.l > 0 && n2.l > 0) {)" EOL
  R"(    l = self.l;)" EOL
  R"(    d = _{alloc}(l);)" EOL
  R"(    _{size_t} j = 0;)" EOL
  R"(    for (_{size_t} i = 0; i < self.l; i++) {)" EOL
  R"(      if (i <= self.l - n1.l && _{memcmp}(&self.d[i], n1.d, n1.l) == 0 && (n3 <= 0 || k++ < n3)) {)" EOL
  R"(        if (n1.l < n2.l) {)" EOL
  R"(          l += n2.l - n1.l;)" EOL
  R"(          if (l > self.l) {)" EOL
  R"(            d = _{re_alloc}(d, l);)" EOL
  R"(          })" EOL
  R"(        } else if (n1.l > n2.l) {)" EOL
  R"(          l -= n1.l - n2.l;)" EOL
  R"(        })" EOL
  R"(        _{memcpy}(&d[j], n2.d, n2.l);)" EOL
  R"(        j += n2.l;)" EOL
  R"(        i += n1.l - 1;)" EOL
  R"(      } else {)" EOL
  R"(        d[j++] = self.d[i];)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(    d = _{re_alloc}(d, l);)" EOL
  R"(  } else if (self.l > 0) {)" EOL
  R"(    l = self.l;)" EOL
  R"(    d = _{alloc}(l);)" EOL
  R"(    _{memcpy}(d, self.d, l);)" EOL
  R"(  })" EOL
  R"(  _{free}(n2.d);)" EOL
  R"(  _{free}(n1.d);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} str_slice (_{struct str} s, unsigned char o1, _{int32_t} n1, unsigned char o2, _{int32_t} n2) {)" EOL
  R"(  _{int32_t} i1 = o1 == 0 ? 0 : (_{int32_t}) (n1 < 0 ? (n1 < -((_{int32_t}) s.l) ? 0 : n1 + s.l) : (n1 > s.l ? s.l : n1));)" EOL
  R"(  _{int32_t} i2 = o2 == 0 ? (_{int32_t}) s.l : (_{int32_t}) (n2 < 0 ? (n2 < -((_{int32_t}) s.l) ? 0 : n2 + s.l) : (n2 > s.l ? s.l : n2));)" EOL
  R"(  if (i1 >= i2 || i1 >= s.l) {)" EOL
  R"(    _{free}(s.d);)" EOL
  R"(    return _{str_alloc}("");)" EOL
  R"(  })" EOL
  R"(  _{size_t} l = i2 - i1;)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  for (_{size_t} i = 0; i1 < i2; i1++) d[i++] = s.d[i1];)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(struct _{array_str} str_split (_{struct str} self, unsigned char o1, _{struct str} n1) {)" EOL
  R"(  _{struct str} *r = _{NULL};)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  if (self.l > 0 && n1.l == 0) {)" EOL
  R"(    l = self.l;)" EOL
  R"(    r = _{alloc}(l * sizeof(_{struct str}));)" EOL
  R"(    for (_{size_t} i = 0; i < l; i++) {)" EOL
  R"(      r[i] = _{str_calloc}(&self.d[i], 1);)" EOL
  R"(    })" EOL
  R"(  } else if (self.l < n1.l) {)" EOL
  R"(    r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(    r[0] = _{str_calloc}(self.d, self.l);)" EOL
  R"(  } else if (n1.l > 0) {)" EOL
  R"(    _{size_t} i = 0;)" EOL
  R"(    for (_{size_t} j = 0; j <= self.l - n1.l; j++) {)" EOL
  R"(      if (_{memcmp}(&self.d[j], n1.d, n1.l) == 0) {)" EOL
  R"(        r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(        r[l - 1] = _{str_calloc}(&self.d[i], j - i);)" EOL
  R"(        j += n1.l;)" EOL
  R"(        i = j;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(    r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(    r[l - 1] = _{str_calloc}(&self.d[i], self.l - i);)" EOL
  R"(  })" EOL
  R"(  _{free}(n1.d);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (struct _{array_str}) {r, l};)" EOL
  R"(})" EOL,

  R"(_{struct buffer} str_toBuffer (_{struct str} s) {)" EOL
  R"(  return (_{struct buffer}) {(unsigned char *) s.d, s.l};)" EOL
  R"(})" EOL,

  R"(double str_toFloat (_{struct str} self) {)" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  double r = _{strtod}(c, &e);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < -_{DBL_MAX} || _{DBL_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(float str_toF32 (_{struct str} self) {)" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  float r = _{strtof}(c, &e);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < -_{FLT_MAX} || _{FLT_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(double str_toF64 (_{struct str} self) {)" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  double r = _{strtod}(c, &e);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < -_{DBL_MAX} || _{DBL_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{int32_t} str_toInt (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  long r = _{strtol}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < _{INT32_MIN} || _{INT32_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{int32_t}) r;)" EOL
  R"(})" EOL,

  R"(_{int8_t} str_toI8 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  long r = _{strtol}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < _{INT8_MIN} || _{INT8_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{int8_t}) r;)" EOL
  R"(})" EOL,

  R"(_{int16_t} str_toI16 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  long r = _{strtol}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < _{INT16_MIN} || _{INT16_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{int16_t}) r;)" EOL
  R"(})" EOL,

  R"(_{int32_t} str_toI32 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  long r = _{strtol}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < _{INT32_MIN} || _{INT32_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{int32_t}) r;)" EOL
  R"(})" EOL,

  R"(_{int64_t} str_toI64 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  long long r = _{strtoll}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || r < _{INT64_MIN} || _{INT64_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{int64_t}) r;)" EOL
  R"(})" EOL,

  R"(_{uint8_t} str_toU8 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  unsigned long r = _{strtoul}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || _{UINT8_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{uint8_t}) r;)" EOL
  R"(})" EOL,

  R"(_{uint16_t} str_toU16 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  unsigned long r = _{strtoul}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || _{UINT16_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{uint16_t}) r;)" EOL
  R"(})" EOL,

  R"(_{uint32_t} str_toU32 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  unsigned long r = _{strtoul}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || _{UINT32_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{uint32_t}) r;)" EOL
  R"(})" EOL,

  R"(_{uint64_t} str_toU64 (_{struct str} self, unsigned char o1, _{int32_t} n1) {)" EOL
  R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: radix %" _{PRId32} " is invalid, must be >= 2 and <= 36, or 0" _{THE_EOL}, n1);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char *c = _{str_cstr}(self);)" EOL
  R"(  char *e = _{NULL};)" EOL
  R"(  _{errno} = 0;)" EOL
  R"(  unsigned long long r = _{strtoull}(c, &e, o1 == 0 ? 10 : n1);)" EOL
  R"(  if (_{errno} == _{ERANGE} || _{UINT64_MAX} < r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` out of range" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (_{errno} != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: value `%s` has invalid syntax" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{free}(self.d);)" EOL
  R"(  return (_{uint64_t}) r;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_trim (_{struct str} s) {)" EOL
  R"(  if (s.l == 0) return s;)" EOL
  R"(  _{size_t} i = 0;)" EOL
  R"(  _{size_t} j = s.l;)" EOL
  R"(  while (i < s.l && _{isspace}(s.d[i])) i++;)" EOL
  R"(  while (j >= 0 && _{isspace}(s.d[j - 1])) {)" EOL
  R"(    j--;)" EOL
  R"(    if (j == 0) break;)" EOL
  R"(  })" EOL
  R"(  if (i >= j) {)" EOL
  R"(    _{free}(s.d);)" EOL
  R"(    return _{str_alloc}("");)" EOL
  R"(  })" EOL
  R"(  _{size_t} l = j - i;)" EOL
  R"(  char *r = _{alloc}(l);)" EOL
  R"(  for (_{size_t} k = 0; k < l;) r[k++] = s.d[i++];)" EOL
  R"(  _{free}(s.d);)" EOL
  R"(  return (_{struct str}) {r, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} str_trimEnd (_{struct str} s) {)" EOL
  R"(  if (s.l == 0) return s;)" EOL
  R"(  while (_{isspace}(s.d[s.l - 1])) {)" EOL
  R"(    s.l--;)" EOL
  R"(    if (s.l == 0) break;)" EOL
  R"(  })" EOL
  R"(  if (s.l == 0) {)" EOL
  R"(    _{free}(s.d);)" EOL
  R"(    s.d = _{NULL};)" EOL
  R"(  } else {)" EOL
  R"(    s.d = _{re_alloc}(s.d, s.l);)" EOL
  R"(  })" EOL
  R"(  return s;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_trimStart (_{struct str} s) {)" EOL
  R"(  if (s.l == 0) return s;)" EOL
  R"(  _{size_t} i = 0;)" EOL
  R"(  while (i < s.l && _{isspace}(s.d[i])) i++;)" EOL
  R"(  if (i >= s.l) {)" EOL
  R"(    _{free}(s.d);)" EOL
  R"(    s = (_{struct str}) {_{NULL}, 0};)" EOL
  R"(  } else {)" EOL
  R"(    _{memmove}(s.d, &s.d[i], s.l - i);)" EOL
  R"(    s.l -= i;)" EOL
  R"(    s.d = _{re_alloc}(s.d, s.l);)" EOL
  R"(  })" EOL
  R"(  return s;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_upper (_{struct str} s) {)" EOL
  R"(  if (s.l != 0) {)" EOL
  R"(    for (_{size_t} i = 0; i < s.l; i++) s.d[i] = (char) _{toupper}(s.d[i]);)" EOL
  R"(  })" EOL
  R"(  return s;)" EOL
  R"(})" EOL,

  R"(_{struct str} str_upperFirst (_{struct str} s) {)" EOL
  R"(  if (s.l != 0) s.d[0] = (char) _{toupper}(s.d[0]);)" EOL
  R"(  return s;)" EOL
  R"(})" EOL
};
