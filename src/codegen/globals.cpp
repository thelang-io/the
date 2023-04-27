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
  R"(void *alloc (_{size_t} l) {)" EOL
  R"(  void *r = _{malloc}(l);)" EOL
  R"(  if (r == _{NULL}) {)" EOL
  R"(    const char *fmt = "failed to allocate %zu bytes";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, l);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, l);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  })" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void print (_{FILE} *stream, const char *fmt, ...) {)" EOL
  R"(  _{va_list} args;)" EOL
  R"(  _{va_start}(args, fmt);)" EOL
  R"(  char buf[512];)" EOL
  R"(  _{struct str} s;)" EOL
  R"(  while (*fmt) {)" EOL
  R"(    switch (*fmt++) {)" EOL
  R"(      case 't': _{fputs}(_{va_arg}(args, int) ? "true" : "false", stream); break;)" EOL
  R"(      case 'b': _{sprintf}(buf, "%u", _{va_arg}(args, unsigned)); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'c': _{fputc}(_{va_arg}(args, int), stream); break;)" EOL
  R"(      case 'e':)" EOL
  R"(      case 'f':)" EOL
  R"(      case 'g': _{snprintf}(buf, 512, "%f", _{va_arg}(args, double)); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'h':)" EOL
  R"(      case 'j':)" EOL
  R"(      case 'v':)" EOL
  R"(      case 'w': _{sprintf}(buf, "%d", _{va_arg}(args, int)); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'i':)" EOL
  R"(      case 'k': _{sprintf}(buf, "%" _{PRId32}, _{va_arg}(args, _{int32_t})); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'l': _{sprintf}(buf, "%" _{PRId64}, _{va_arg}(args, _{int64_t})); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'p': _{sprintf}(buf, "%p", _{va_arg}(args, void *)); _{fputs}(buf, stream); break;)" EOL
  R"(      case 's': s = _{va_arg}(args, _{struct str}); _{fwrite}(s.d, 1, s.l, stream); _{str_free}(s); break;)" EOL
  R"(      case 'u': _{sprintf}(buf, "%" _{PRIu32}, _{va_arg}(args, _{uint32_t})); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'y': _{sprintf}(buf, "%" _{PRIu64}, _{va_arg}(args, _{uint64_t})); _{fputs}(buf, stream); break;)" EOL
  R"(      case 'z': _{fputs}(_{va_arg}(args, char *), stream); break;)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{va_end}(args);)" EOL
  R"(})" EOL,

  R"(void *re_alloc (void *d, _{size_t} l) {)" EOL
  R"(  void *r = _{realloc}(d, l);)" EOL
  R"(  if (r == _{NULL}) {)" EOL
  R"(    const char *fmt = "failed to reallocate %zu bytes";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, l);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, l);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  })" EOL
  R"(  return r;)" EOL
  R"(})" EOL
};
