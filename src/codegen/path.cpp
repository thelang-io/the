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
  R"(_{struct str} path_basename (_{struct str} s) {)" EOL
  R"(  if (s.l == 0) return s;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    char e = '\\';)" EOL
  R"(  #else)" EOL
  R"(    char e = '/';)" EOL
  R"(  #endif)" EOL
  R"(  _{size_t} a = 0;)" EOL
  R"(  _{size_t} b = 0;)" EOL
  R"(  unsigned char k = 0;)" EOL
  R"(  for (_{size_t} i = s.l - 1;; i--) {)" EOL
  R"(    if (k == 1 && s.d[i] == e) {)" EOL
  R"(      a = i + 1;)" EOL
  R"(      break;)" EOL
  R"(    } else if (k == 0 && s.d[i] != e) {)" EOL
  R"(      k = 1;)" EOL
  R"(      b = i + 1;)" EOL
  R"(    })" EOL
  R"(    if (i == 0) break;)" EOL
  R"(  })" EOL
  R"(  if (k == 0) {)" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    return _{str_alloc}("");)" EOL
  R"(  })" EOL
  R"(  _{size_t} l = b - a;)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, &s.d[a], l);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} path_dirname (_{struct str} s) {)" EOL
  R"(  if (s.l == 0) {)" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    return _{str_alloc}(".");)" EOL
  R"(  })" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    char e = '\\';)" EOL
  R"(  #else)" EOL
  R"(    char e = '/';)" EOL
  R"(  #endif)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  unsigned char k = 0;)" EOL
  R"(  for (_{size_t} i = s.l - 1;; i--) {)" EOL
  R"(    if (k == 1 && s.d[i] == e) {)" EOL
  R"(      l = i;)" EOL
  R"(      break;)" EOL
  R"(    } else if (k == 0 && s.d[i] != e) {)" EOL
  R"(      k = 1;)" EOL
  R"(    })" EOL
  R"(    if (i == 0) break;)" EOL
  R"(  })" EOL
  R"(  if (l == 0) {)" EOL
  R"(    s.d = _{re_alloc}(s.d, 1);)" EOL
  R"(    if (s.d[0] != e) s.d[0] = '.';)" EOL
  R"(    return (_{struct str}) {s.d, 1};)" EOL
  R"(  })" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{memcpy}(d, s.d, l);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} path_tempDirectory () {)" EOL
  R"(  char *f;)" EOL
  R"(  if ()" EOL
  R"(    (f = _{getenv}("TMPDIR")) == _{NULL} &&)" EOL
  R"(    (f = _{getenv}("TMP")) == _{NULL} &&)" EOL
  R"(    (f = _{getenv}("TEMP")) == _{NULL} &&)" EOL
  R"(    (f = _{getenv}("TEMPDIR")) == _{NULL})" EOL
  R"(  ) {)" EOL
  R"(    f = "/tmp";)" EOL
  R"(  })" EOL
  R"(  char *fmt = f[_{strlen}(f) - 1] == '/' ? "%sXXXXXX" : "%s/XXXXXX";)" EOL
  R"(  _{size_t} l = _{snprintf}(_{NULL}, 0, fmt, f);)" EOL
  R"(  char *t = _{alloc}(l + 1);)" EOL
  R"(  _{sprintf}(t, fmt, f);)" EOL
  R"(  char *d = _{mkdtemp}(t);)" EOL
  R"(  if (d == _{NULL}) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create temporary directory" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{struct str} r = _{str_alloc}(t);)" EOL
  R"(  _{free}(t);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{struct str} path_tempFile () {)" EOL
  R"(  char *f;)" EOL
  R"(  if ()" EOL
  R"(    (f = _{getenv}("TMPDIR")) == _{NULL} &&)" EOL
  R"(    (f = _{getenv}("TMP")) == _{NULL} &&)" EOL
  R"(    (f = _{getenv}("TEMP")) == _{NULL} &&)" EOL
  R"(    (f = _{getenv}("TEMPDIR")) == _{NULL})" EOL
  R"(  ) {)" EOL
  R"(    f = "/tmp";)" EOL
  R"(  })" EOL
  R"(  char *fmt = f[_{strlen}(f) - 1] == '/' ? "%sXXXXXX" : "%s/XXXXXX";)" EOL
  R"(  _{size_t} l = _{snprintf}(_{NULL}, 0, fmt, f);)" EOL
  R"(  char *t = _{alloc}(l + 1);)" EOL
  R"(  _{sprintf}(t, fmt, f);)" EOL
  R"(  int fd = _{mkstemp}(t);)" EOL
  R"(  if (fd == -1) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create temporary file" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{close}(fd);)" EOL
  R"(  _{struct str} r = _{str_alloc}(t);)" EOL
  R"(  _{free}(t);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,
};
