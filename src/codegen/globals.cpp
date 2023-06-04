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

#include "globals.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenGlobals = {
  R"(void *alloc (_{size_t} n1) {)" EOL
  R"(  void *r = _{malloc}(n1);)" EOL
  R"(  if (r == _{NULL}) _{error_alloc}(&_{err_state}, n1);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void print (_{FILE} *stream, const char *fmt, ...) {)" EOL
  R"(  char *d = _{alloc}(1024);)" EOL
  R"(  _{size_t} cap = 1024;)" EOL
  R"(  _{size_t} len = 0;)" EOL
  R"(  _{size_t} y;)" EOL
  R"(  _{va_list} args;)" EOL
  R"(  _{va_start}(args, fmt);)" EOL
  R"(  while (*fmt) {)" EOL
  R"(    switch (*fmt++) {)" EOL
  R"(      case 't': {)" EOL
  R"(        int a = _{va_arg}(args, int);)" EOL
  R"(        y = a == 0 ? 5 : 4;)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{memcpy}(&d[len], a == 0 ? "false" : "true", y);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'b': {)" EOL
  R"(        unsigned int a = _{va_arg}(args, unsigned int);)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%u", a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%u", a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'c': {)" EOL
  R"(        char a = _{va_arg}(args, int);)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%c", a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%c", a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'e':)" EOL
  R"(      case 'f':)" EOL
  R"(      case 'g': {)" EOL
  R"(        double a = _{va_arg}(args, double);)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%f", a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%f", a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'h':)" EOL
  R"(      case 'j':)" EOL
  R"(      case 'v':)" EOL
  R"(      case 'w': {)" EOL
  R"(        int a = _{va_arg}(args, int);)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%d", a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%d", a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'i':)" EOL
  R"(      case 'k': {)" EOL
  R"(        _{int32_t} a = _{va_arg}(args, _{int32_t});)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%" _{PRId32}, a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%" _{PRId32}, a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'l': {)" EOL
  R"(        _{int64_t} a = _{va_arg}(args, _{int64_t});)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%" _{PRId64}, a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%" _{PRId64}, a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'p': {)" EOL
  R"(        void *a = _{va_arg}(args, void *);)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%p", a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%p", a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 's': {)" EOL
  R"(        _{struct str} a = _{va_arg}(args, _{struct str});)" EOL
  R"(        y = a.l;)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{memcpy}(&d[len], a.d, y);)" EOL
  R"(        _{free}(a.d);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'u': {)" EOL
  R"(        _{uint32_t} a = _{va_arg}(args, _{uint32_t});)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%" _{PRIu32}, a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%" _{PRIu32}, a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'y': {)" EOL
  R"(        _{uint64_t} a = _{va_arg}(args, _{uint64_t});)" EOL
  R"(        y = _{snprintf}(_{NULL}, 0, "%" _{PRIu64}, a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{sprintf}(&d[len], "%" _{PRIu64}, a);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(      case 'z': {)" EOL
  R"(        char *a = _{va_arg}(args, char *);)" EOL
  R"(        y = _{strlen}(a);)" EOL
  R"(        if (len + y >= cap) d = _{re_alloc}(d, cap += (y / 1024 + 1) * 1024);)" EOL
  R"(        _{memcpy}(&d[len], a, y);)" EOL
  R"(        break;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(    len += y;)" EOL
  R"(  })" EOL
  R"(  _{va_end}(args);)" EOL
  R"(  d[len] = '\0';)" EOL
  R"(  _{fputs}(d, stream);)" EOL
  R"(  _{fflush}(stream);)" EOL
  R"(  _{free}(d);)" EOL
  R"(})" EOL,

  R"(void *re_alloc (void *n1, _{size_t} n2) {)" EOL
  R"(  void *r = _{realloc}(n1, n2);)" EOL
  R"(  if (r == _{NULL}) _{error_alloc}(&_{err_state}, n2);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void *xalloc (void *n1, _{size_t} n2) {)" EOL
  R"(  void *r = _{malloc}(n2);)" EOL
  R"(  if (r == _{NULL}) _{error_alloc}(&_{err_state}, n2);)" EOL
  R"(  _{memcpy}(r, n1, n2);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL
};
