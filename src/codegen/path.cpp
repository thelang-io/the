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

  R"(char *path_mktemp (unsigned char (*f) (char *)) {)" EOL
  R"(  static const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";)" EOL
  R"(  static const _{size_t} chars_len = 62;)" EOL
  R"(  static const _{size_t} x = 6;)" EOL
  R"(  char *d = _{path_tmpdir}();)" EOL
  R"(  _{size_t} l = _{strlen}(d) + x;)" EOL
  R"(  d = _{re_alloc}(d, l + 1);)" EOL
  R"(  _{memcpy}(&d[l - x], "XXXXXX", x + 1);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{size_t} j = _{TMP_MAX};)" EOL
  R"(    unsigned long long v;)" EOL
  R"(    do {)" EOL
  R"(      if (!_{SystemFunction036}((void *) &v, sizeof(v))) {)" EOL
  R"(        _{fprintf}(_{stderr}, "Error: failed to generate random with `SystemFunction036`" _{THE_EOL});)" EOL
  R"(        _{exit}(_{EXIT_FAILURE});)" EOL
  R"(      })" EOL
  R"(      char *p = &d[l - x];)" EOL
  R"(      for (_{size_t} i = 0; i < x; i++) {)" EOL
  R"(        *p++ = chars[v % chars_len];)" EOL
  R"(        v /= chars_len;)" EOL
  R"(      })" EOL
  R"(      if (f(d) == 0) return d;)" EOL
  R"(    } while (--j);)" EOL
  R"(    _{free}(d);)" EOL
  R"(    return _{NULL};)" EOL
  R"(  #else)" EOL
  R"(    if (f(d) == 1) {)" EOL
  R"(      _{free}(d);)" EOL
  R"(      return _{NULL};)" EOL
  R"(    })" EOL
  R"(    return d;)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(_{struct str} path_tempDirectory () {)" EOL
  R"(  char *d = _{path_mktemp}(_{path_tempDirectoryFunctor});)" EOL
  R"(  if (d == _{NULL}) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create temporary directory" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{struct str} r = _{str_alloc}(d);)" EOL
  R"(  _{free}(d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(unsigned char path_tempDirectoryFunctor (char *c) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    return _{CreateDirectory}(c, _{NULL}) ? 0 : 1;)" EOL
  R"(  #else)" EOL
  R"(    return _{mkdtemp}(c) == _{NULL} ? 1 : 0;)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(_{struct str} path_tempFile () {)" EOL
  R"(  char *d = _{path_mktemp}(_{path_tempFileFunctor});)" EOL
  R"(  if (d == _{NULL}) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create temporary file" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{struct str} r = _{str_alloc}(d);)" EOL
  R"(  _{free}(d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(unsigned char path_tempFileFunctor (char *c) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    return _{INVALID_HANDLE_VALUE} == _{CreateFile}(c, _{GENERIC_READ} | _{GENERIC_WRITE}, _{FILE_SHARE_READ} | _{FILE_SHARE_WRITE} | _{FILE_SHARE_DELETE}, _{NULL}, _{CREATE_NEW}, _{FILE_ATTRIBUTE_NORMAL}, _{NULL});)" EOL
  R"(  #else)" EOL
  R"(    int fd = _{mkstemp}(c);)" EOL
  R"(    if (fd != -1) _{close}(fd);)" EOL
  R"(    return fd == -1 ? 1 : 0;)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(char *path_tmpdir () {)" EOL
  R"(  char *d;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{size_t} l = _{GetTempPath}(0, _{NULL});)" EOL
  R"(    if (l == 0) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to get temporary path" _{THE_EOL});)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    l -= 1;)" EOL
  R"(    d = _{alloc}(l + 1);)" EOL
  R"(    if (_{GetTempPath}(l + 1, d) == 0) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to get temporary path" _{THE_EOL});)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    if (d[l - 1] != '\\' && d[l - 1] != '/') {)" EOL
  R"(      d[l] = '\\';)" EOL
  R"(      d = _{re_alloc}(d, ++l + 1);)" EOL
  R"(    })" EOL
  R"(    d[l] = '\0';)" EOL
  R"(  #else)" EOL
  R"(    char *f;)" EOL
  R"(    if ()" EOL
  R"(      (f = _{getenv}("TMPDIR")) == _{NULL} &&)" EOL
  R"(      (f = _{getenv}("TMP")) == _{NULL} &&)" EOL
  R"(      (f = _{getenv}("TEMP")) == _{NULL} &&)" EOL
  R"(      (f = _{getenv}("TEMPDIR")) == _{NULL})" EOL
  R"(    ) {)" EOL
  R"(      f = "/tmp/";)" EOL
  R"(    })" EOL
  R"(    _{size_t} l = _{strlen}(f);)" EOL
  R"(    d = _{alloc}(l + 1);)" EOL
  R"(    _{memcpy}(d, f, l);)" EOL
  R"(    if (f[l - 1] != '/') {)" EOL
  R"(      d[l] = '/';)" EOL
  R"(      d = _{re_alloc}(d, ++l + 1);)" EOL
  R"(    })" EOL
  R"(    d[l] = '\0';)" EOL
  R"(  #endif)" EOL
  R"(  return d;)" EOL
  R"(})" EOL
};
