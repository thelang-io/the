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

#include "fs.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenFs = {
  // todo test windows
  R"(void fs_chmodSync (_{struct str} s, _{int32_t} m) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  if (_{chmod}(c, m) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to change mode to %" _{PRId32} " for file `%s`" _{THE_EOL}, m, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(})" EOL,

  R"(void fs_chownSync (_{struct str} s, _{int32_t} u, _{int32_t} g) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    return;)" EOL
  R"(  #endif)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  if (_{chown}(c, u, g) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to change owner to %" _{PRId32} " and group to %" _{PRId32} " for file `%s`" _{THE_EOL}, u, g, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(})" EOL,

  R"(_{bool} fs_existsSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    int r = _{_access}(c, 0);)" EOL
  R"(  #else)" EOL
  R"(    int r = _{access}(c, 0);)" EOL
  R"(  #endif)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  return r == 0 ? true : false;)" EOL
  R"(})" EOL,

  R"(_{bool} fs_isAbsoluteSync (_{struct str} s) {)" EOL
  R"(  _{bool} r = (s.l > 0 && s.d[0] == '/') || (s.l > 2 && _{isalpha}(s.d[0]) && s.d[1] == ':' && s.d[2] == '\\');)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  // todo test windows
  R"(_{bool} fs_isDirectorySync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{struct stat} r;)" EOL
  R"(  _{bool} b = false;)" EOL
  R"(  if (_{stat}(c, &r) == 0) {)" EOL
  R"(    b = _{S_ISDIR}(r.st_mode);)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return b;)" EOL
  R"(})" EOL,

  // todo test windows
  R"(_{bool} fs_isFileSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{struct stat} r;)" EOL
  R"(  _{bool} b = false;)" EOL
  R"(  if (_{stat}(c, &r) == 0) {)" EOL
  R"(    b = _{S_ISREG}(r.st_mode);)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return b;)" EOL
  R"(})" EOL,

  // todo test windows
  R"(_{bool} fs_isSymbolicLinkSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{struct stat} r;)" EOL
  R"(  _{bool} b = false;)" EOL
  R"(  if (_{lstat}(c, &r) == 0) {)" EOL
  R"(    b = _{S_ISLNK}(r.st_mode);)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return b;)" EOL
  R"(})" EOL,

  // todo test windows
  R"(void fs_linkSync (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  char *c1 = _{alloc}(s1.l + 1);)" EOL
  R"(  _{memcpy}(c1, s1.d, s1.l);)" EOL
  R"(  c1[s1.l] = '\0';)" EOL
  R"(  char *c2 = _{alloc}(s2.l + 1);)" EOL
  R"(  _{memcpy}(c2, s2.d, s2.l);)" EOL
  R"(  c2[s2.l] = '\0';)" EOL
  R"(  if (_{symlink}(c1, c2) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create symbolic link from `%s` to `%s`" _{THE_EOL}, c1, c2);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{str_free}(s1);)" EOL
  R"(  _{str_free}(s2);)" EOL
  R"(  _{free}(c1);)" EOL
  R"(  _{free}(c2);)" EOL
  R"(})" EOL,

  // todo test windows
  R"(void fs_mkdirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    int r = _{CreateDirectoryW}(c, NULL);)" EOL
  R"(  #else)" EOL
  R"(    int r = _{mkdir}(c, 0777);)" EOL
  R"(  #endif)" EOL
  R"(  if (r != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create directory `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(})" EOL,

  R"(_{struct buffer} fs_readFileSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "rb");)" EOL
  R"(  if (f == _{NULL}) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to open file `%s` for reading" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  unsigned char *d = _{NULL};)" EOL
  R"(  unsigned char b[4096];)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  _{size_t} y;)" EOL
  R"(  while ((y = _{fread}(b, 1, sizeof(b), f)) > 0) {)" EOL
  R"(    d = _{re_alloc}(d, l + y);)" EOL
  R"(    _{memcpy}(&d[l], b, y);)" EOL
  R"(    l += y;)" EOL
  R"(  })" EOL
  R"(  _{fclose}(f);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}((_{struct str}) s);)" EOL
  R"(  return (_{struct buffer}) {d, l};)" EOL
  R"(})" EOL,

  // todo test windows
  R"(_{struct str} fs_realpathSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  char *d = _{realpath}(c, _{NULL});)" EOL
  R"(  if (d == _{NULL}) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to get real path of file `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}((_{struct str}) s);)" EOL
  R"(  return (_{struct str}) {d, _{strlen}(d)};)" EOL
  R"(})" EOL,

  R"(void fs_rmSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  if (_{remove}(c) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to remove file `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(})" EOL,

  // todo test windows
  R"(void fs_rmdirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    int r = _{_wrmdir}(c);)" EOL
  R"(  #else)" EOL
  R"(    int r = _{rmdir}(c);)" EOL
  R"(  #endif)" EOL
  R"(  if (r != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to remove directory `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(})" EOL,

  // todo test windows
  R"(struct _{array_str} fs_scandirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{DIR} *f = _{opendir}(c);)" EOL
  R"(  if (f == _{NULL}) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to open directory `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{struct dirent} *a;)" EOL
  R"(  _{struct str} *r = _{NULL};)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  while ((a = _{readdir}(f)) != _{NULL}) {)" EOL
  R"(    if (_{strcmp}(a->d_name, ".") == 0 || _{strcmp}(a->d_name, "..") == 0) continue;)" EOL
  R"(    r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(    r[l - 1] = _{str_alloc}(a->d_name);)" EOL
  R"(  })" EOL
  R"(  _{closedir}(f);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return (struct _{array_str}) {r, l};)" EOL
  R"(})" EOL,

  // todo test windows
  R"(struct _{fs_Stats} *fs_statSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{struct stat} r;)" EOL
  R"(  if (_{stat}(c, &r) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to stat file `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  #ifdef _{THE_OS_MACOS})" EOL
  R"(    return _{fs_Stats_alloc}(r.st_dev, r.st_mode, r.st_nlink, r.st_ino, r.st_uid, r.st_gid, r.st_rdev, r.st_atimespec.tv_sec, r.st_atimespec.tv_nsec, r.st_mtimespec.tv_sec, r.st_mtimespec.tv_nsec, r.st_ctimespec.tv_sec, r.st_ctimespec.tv_nsec, r.st_birthtimespec.tv_sec, r.st_birthtimespec.tv_nsec, r.st_size, r.st_blocks, r.st_blksize);)" EOL
  R"(  #else)" EOL
  R"(    return _{fs_Stats_alloc}(r.st_dev, r.st_mode, r.st_nlink, r.st_ino, r.st_uid, r.st_gid, r.st_rdev, r.st_atim.tv_sec, r.st_atim.tv_nsec, r.st_mtim.tv_sec, r.st_mtim.tv_nsec, r.st_ctim.tv_sec, r.st_ctim.tv_nsec, r.st_ctim.tv_sec, r.st_ctim.tv_nsec, r.st_size, r.st_blocks, r.st_blksize);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(void fs_writeFileSync (_{struct str} s, _{struct buffer} b) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "wb");)" EOL
  R"(  if (f == NULL) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to open file `%s` for writing" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  if (b.l != 0) {)" EOL
  R"(    if (_{fwrite}(b.d, b.l, 1, f) != 1) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to write file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{fclose}(f);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{buffer_free}(b);)" EOL
  R"(})" EOL
};
