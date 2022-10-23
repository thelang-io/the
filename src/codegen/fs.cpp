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

// todo failed to create handle is when file doesn't exists

const std::vector<std::string> codegenFs = {
  R"(void fs_chmodSync (_{struct str} s, _{int32_t} m) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{_chmod}(c, m) == 0;)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{chmod}(c, m) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
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
  R"(    _{bool} r = _{_access}(c, 0) == 0;)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{access}(c, 0) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{bool} fs_isAbsoluteSync (_{struct str} s) {)" EOL
  R"(  _{bool} r = (s.l > 0 && s.d[0] == '/') || (s.l > 2 && _{isalpha}(s.d[0]) && s.d[1] == ':' && s.d[2] == '\\');)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(_{bool} fs_isDirectorySync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{bool} b = _{false};)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{struct _stat} sb;)" EOL
  R"(    if (_{_stat}(c, &sb) == 0) {)" EOL
  R"(      b = (sb.st_mode & _{_S_IFMT}) == _{_S_IFDIR};)" EOL
  R"(    })" EOL
  R"(  #else)" EOL
  R"(    _{struct stat} sb;)" EOL
  R"(    if (_{stat}(c, &sb) == 0) {)" EOL
  R"(      b = (sb.st_mode & _{S_IFMT}) == _{S_IFDIR};)" EOL
  R"(    })" EOL
  R"(  #endif)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return b;)" EOL
  R"(})" EOL,

  R"(_{bool} fs_isFileSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{bool} b = _{false};)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{struct _stat} sb;)" EOL
  R"(    if (_{_stat}(c, &sb) == 0) {)" EOL
  R"(      b = (sb.st_mode & _{_S_IFMT}) == _{_S_IFREG};)" EOL
  R"(    })" EOL
  R"(  #else)" EOL
  R"(    _{struct stat} sb;)" EOL
  R"(    if (_{stat}(c, &sb) == 0) {)" EOL
  R"(      b = (sb.st_mode & _{S_IFMT}) == _{S_IFREG};)" EOL
  R"(    })" EOL
  R"(  #endif)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return b;)" EOL
  R"(})" EOL,

  R"(_{bool} fs_isSymbolicLinkSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{bool} b = _{false};)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{HANDLE} h = _{CreateFile}(c, 0, _{FILE_SHARE_READ}, _{NULL}, _{OPEN_EXISTING}, 0, _{NULL});)" EOL
  R"(    if (h == _{INVALID_HANDLE_VALUE}) {)" EOL
  R"(      free(c);)" EOL
  R"(      return b;)" EOL
  R"(    })" EOL
  R"(    char d[_{MAXIMUM_REPARSE_DATA_BUFFER_SIZE}];)" EOL
  R"(    _{struct win_reparse_data_buffer} *r = (_{struct win_reparse_data_buffer} *) d;)" EOL
  R"(    _{DWORD} y;)" EOL
  R"(    if (_{DeviceIoControl}(h, _{FSCTL_GET_REPARSE_POINT}, _{NULL}, 0, &d, sizeof(d), &y, _{NULL})) {)" EOL
  R"(      if (r->ReparseTag == _{IO_REPARSE_TAG_SYMLINK}) {)" EOL
  R"(        b = _{true};)" EOL
  R"(      } else if (r->ReparseTag == _{IO_REPARSE_TAG_MOUNT_POINT}) {)" EOL
  R"(        char *t = r->MountPointReparseBuffer.PathBuffer + r->MountPointReparseBuffer.SubstituteNameOffset;)" EOL
  R"(        if ()" EOL
  R"(          r->MountPointReparseBuffer.SubstituteNameLength >= 6 &&)" EOL
  R"(          t[0] == '\\' && t[1] == '?' && t[2] == '?' && t[3] == '\\' &&)" EOL
  R"(          ((t[4] >= 'A' && t[4] <= 'Z') || (t[4] >= 'a' && t[4] <= 'z')) &&)" EOL
  R"(          t[5] == ':' && (r->MountPointReparseBuffer.SubstituteNameLength == 6 || t[6] == '\\'))" EOL
  R"(        ) {)" EOL
  R"(          b = _{true};)" EOL
  R"(        })" EOL
  R"(      } else if (r->ReparseTag == _{IO_REPARSE_TAG_APPEXECLINK} && r->AppExecLinkReparseBuffer.Version == 3) {)" EOL
  R"(        char *t = r->AppExecLinkReparseBuffer.StringList;)" EOL
  R"(        _{bool} f = _{false};)" EOL
  R"(        for (_{size_t} i = 0; i < 2; i++) {)" EOL
  R"(          size_t l = _{strlen}(t);)" EOL
  R"(          if (l == 0) {)" EOL
  R"(            f = _{true};)" EOL
  R"(            break;)" EOL
  R"(          })" EOL
  R"(          t += l + 1;)" EOL
  R"(        })" EOL
  R"(        if (!f) {)" EOL
  R"(          if (_{strlen}(t) >= 3 && ((t[0] >= 'a' && t[0] <= 'z') || (t[0] >= 'A' && t[0] <= 'Z')) && t[1] == ':' && t[2] == '\\') {)" EOL
  R"(            b = _{true};)" EOL
  R"(          })" EOL
  R"(        })" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(    _{CloseHandle}(h);)" EOL
  R"(  #else)" EOL
  R"(    _{struct stat} sb;)" EOL
  R"(    if (_{lstat}(c, &sb) == 0) {)" EOL
  R"(      b = (sb.st_mode & _{S_IFMT}) == _{S_IFLNK};)" EOL
  R"(    })" EOL
  R"(  #endif)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return b;)" EOL
  R"(})" EOL,

  // todo test link directory, file, and link
  R"(void fs_linkSync (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  char *c1 = _{alloc}(s1.l + 1);)" EOL
  R"(  _{memcpy}(c1, s1.d, s1.l);)" EOL
  R"(  c1[s1.l] = '\0';)" EOL
  R"(  char *c2 = _{alloc}(s2.l + 1);)" EOL
  R"(  _{memcpy}(c2, s2.d, s2.l);)" EOL
  R"(  c2[s2.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{CreateSymbolicLink}(c2, c1, 0);)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{symlink}(c1, c2) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to create symbolic link from `%s` to `%s`" _{THE_EOL}, c1, c2);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{str_free}(s1);)" EOL
  R"(  _{str_free}(s2);)" EOL
  R"(  _{free}(c1);)" EOL
  R"(  _{free}(c2);)" EOL
  R"(})" EOL,

  R"(void fs_mkdirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{CreateDirectory}(c, _{NULL}) == 0;)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{mkdir}(c, 0777) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
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

  R"(_{struct str} fs_realpathSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{HANDLE} h = _{CreateFile}(c, _{GENERIC_READ}, _{FILE_SHARE_READ}, _{NULL}, _{OPEN_EXISTING}, _{FILE_ATTRIBUTE_NORMAL}, _{NULL});)" EOL
  R"(    if (h == _{INVALID_HANDLE_VALUE}) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to create handle to get real path of file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    char *d = _{alloc}(_{MAX_PATH} + 1);)" EOL
  R"(    _{size_t} l = _{GetFinalPathNameByHandle}(h, d, _{MAX_PATH}, _{VOLUME_NAME_DOS});)" EOL
  R"(    if (l == 0) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to get real path of file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    if (_{strncmp}(d, "\\\\?\\UNC\\", 8) == 0) {)" EOL
  R"(      d += 6;)" EOL
  R"(      d[0] = '\\';)" EOL
  R"(      l -= 6;)" EOL
  R"(    } else if (_{strncmp}(d, "\\\\?\\", 4) == 0) {)" EOL
  R"(      d += 4;)" EOL
  R"(      l -= 4;)" EOL
  R"(    } else {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to get real path of file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    _{CloseHandle}(h);)" EOL
  R"(  #else)" EOL
  R"(    char *d = _{realpath}(c, _{NULL});)" EOL
  R"(    if (d == _{NULL}) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to get real path of file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    _{size_t} l = _{strlen}(d);)" EOL
  R"(  #endif)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}((_{struct str}) s);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
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

  R"(void fs_rmdirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{_rmdir}(c) == 0;)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{rmdir}(c) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
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

  R"(struct _{fs_Stats} *fs_statSync (_{struct str} s) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  struct _{fs_Stats} *r;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{struct _stat} sb;)" EOL
  R"(    if (_{_stat}(c, &sb) != 0) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to stat file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    r = _{fs_Stats_alloc}(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atime, 0, sb.st_mtime, 0, sb.st_ctime, 0, sb.st_ctime, 0, sb.st_size, 0, 4096);)" EOL
  R"(  #else)" EOL
  R"(    _{struct stat} sb;)" EOL
  R"(    if (_{stat}(c, &sb) != 0) {)" EOL
  R"(      _{fprintf}(_{stderr}, "Error: failed to stat file `%s`" _{THE_EOL}, c);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    #ifdef _{THE_OS_MACOS})" EOL
  R"(      r = _{fs_Stats_alloc}(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atimespec.tv_sec, sb.st_atimespec.tv_nsec, sb.st_mtimespec.tv_sec, sb.st_mtimespec.tv_nsec, sb.st_ctimespec.tv_sec, sb.st_ctimespec.tv_nsec, sb.st_birthtimespec.tv_sec, sb.st_birthtimespec.tv_nsec, sb.st_size, sb.st_blocks, sb.st_blksize);)" EOL
  R"(    #else)" EOL
  R"(      r = _{fs_Stats_alloc}(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atim.tv_sec, sb.st_atim.tv_nsec, sb.st_mtim.tv_sec, sb.st_mtim.tv_nsec, sb.st_ctim.tv_sec, sb.st_ctim.tv_nsec, sb.st_ctim.tv_sec, sb.st_ctim.tv_nsec, sb.st_size, sb.st_blocks, sb.st_blksize);)" EOL
  R"(    #endif)" EOL
  R"(  #endif)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void fs_writeFileSync (_{struct str} s, _{struct buffer} b) {)" EOL
  R"(  char *c = _{alloc}(s.l + 1);)" EOL
  R"(  _{memcpy}(c, s.d, s.l);)" EOL
  R"(  c[s.l] = '\0';)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "wb");)" EOL
  R"(  if (f == _{NULL}) {)" EOL
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
