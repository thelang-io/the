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

const auto chmodSync = std::string(
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
  R"(})" EOL
);

const auto chownSync = std::string(
  R"(void fs_chownSync (_{struct str} s, _{int32_t} u, _{int32_t} g) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  if (_{chown}(c, u, g) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to change owner to %" _{PRId32} " and group to %" _{PRId32} " for file `%s`" _{THE_EOL}, u, g, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(})" EOL
);

// todo test
const auto rmSync = std::string(
  R"(void fs_rmSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  // todo test error
  R"(  if (_{remove}(c) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to remove file `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(})" EOL
);

// todo test
const auto statSync = std::string(
  R"(struct _{fs_Stats} *fs_statSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{struct stat} r;)" EOL
  // todo test error
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
  R"(})" EOL
);

// todo test
const auto writeFileSync = std::string(
  R"(void fs_writeFileSync (_{struct str} s, _{struct buffer} b) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "wb");)" EOL
  // todo test error
  R"(  if (f == NULL) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to open file `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  // todo test error
  R"(  if (_{fwrite}(b.d, b.l, 1, f) != 0) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to write to file `%s`" _{THE_EOL}, c);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{fclose}(f);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{buffer_free}(b);)" EOL
  R"(})" EOL
);

const std::vector<std::string> codegenFs = {
  chmodSync,
  chownSync,
  rmSync,
  statSync,
  writeFileSync
};
