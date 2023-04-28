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
  R"(void fs_appendFileSync (_{struct str} s, _{struct buffer} b) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "ab");)" EOL
  R"(  if (f == _{NULL}) {)" EOL
  R"(    const char *fmt = "failed to open file `%s` for writing";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_appendFileSync_cleanup1;)" EOL
  R"(  })" EOL
  R"(  if (b.l != 0) {)" EOL
  R"(    if (_{fwrite}(b.d, b.l, 1, f) != 1) {)" EOL
  R"(      const char *fmt = "failed appending to file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_appendFileSync_cleanup2;)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(fs_appendFileSync_cleanup2:)" EOL
  R"(  _{fclose}(f);)" EOL
  R"(fs_appendFileSync_cleanup1:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{buffer_free}(b);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_chmodSync (_{struct str} s, _{int32_t} m) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{_chmod}(c, m) == 0;)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{chmod}(c, m) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
  R"(    const char *fmt = "failed to change mode to %" _{PRId32} " for file `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, m, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, m, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_chmodSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_chmodSync_cleanup:)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_chownSync (_{struct str} s, _{int32_t} u, _{int32_t} g) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    return;)" EOL
  R"(  #endif)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  if (_{chown}(c, u, g) != 0) {)" EOL
  R"(    const char *fmt = "failed to change owner to %" _{PRId32} " and group to %" _{PRId32} " for file `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, u, g, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, u, g, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_chownSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_chownSync_cleanup:)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{free}(c);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_copyDirectorySync (_{struct str} n1, _{struct str} n2) {)" EOL
  R"(  if (_{fs_existsSync}(_{str_copy}(n2))) {)" EOL
  R"(    if (_{fs_isDirectorySync}(_{str_copy}(n2))) {)" EOL
  R"(      _{fs_rmdirSync}(_{str_copy}(n2));)" EOL
  R"(    } else {)" EOL
  R"(      _{fs_rmSync}(_{str_copy}(n2));)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  if (n1.l > 0 && n1.d[n1.l - 1] != (_{THE_PATH_SEP})[0] && n1.d[n1.l - 1] != '/') {)" EOL
  R"(    n1.d = _{re_alloc}(n1.d, ++n1.l);)" EOL
  R"(    n1.d[n1.l - 1] = (_{THE_PATH_SEP})[0];)" EOL
  R"(  })" EOL
  R"(  if (n2.l > 0 && n2.d[n2.l - 1] != (_{THE_PATH_SEP})[0] && n2.d[n2.l - 1] != '/') {)" EOL
  R"(    n2.d = _{re_alloc}(n2.d, ++n2.l);)" EOL
  R"(    n2.d[n2.l - 1] = (_{THE_PATH_SEP})[0];)" EOL
  R"(  })" EOL
  R"(  struct _{array_str} files = _{fs_scandirSync}(_{str_copy}(n1));)" EOL
  R"(  _{fs_mkdirSync}(_{str_copy}(n2));)" EOL
  R"(  for (_{size_t} i = 0; i < files.l; i++) {)" EOL
  R"(    _{struct str} file = _{str_concat_str}(_{str_copy}(n1), _{str_copy}(files.d[i]));)" EOL
  R"(    if (_{fs_isDirectorySync}(_{str_copy}(file))) {)" EOL
  R"(      _{fs_copyDirectorySync}(file, _{str_concat_str}(_{str_copy}(n2), _{str_copy}(files.d[i])));)" EOL
  R"(    } else {)" EOL
  R"(      _{fs_copyFileSync}(file, _{str_concat_str}(_{str_copy}(n2), _{str_copy}(files.d[i])));)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{array_str_free}(files);)" EOL
  R"(  _{str_free}(n2);)" EOL
  R"(  _{str_free}(n1);)" EOL
  R"(})" EOL,

  R"(void fs_copyFileSync (_{struct str} n1, _{struct str} n2) {)" EOL
  R"(  char *c1 = _{str_cstr}(n1);)" EOL
  R"(  char *c2 = _{str_cstr}(n2);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    if (_{CopyFile}(c1, c2, 0) == 0) {)" EOL
  R"(      const char *fmt = "failed to copy file from `%s` to `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1, c2);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c1, c2);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(  #else)" EOL
  R"(    int fd1 = _{open}(c1, _{O_RDONLY});)" EOL
  R"(    if (fd1 < 0) {)" EOL
  R"(      const char *fmt = "failed to open file descriptor of `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c1);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(    _{struct stat} sb1;)" EOL
  R"(    if (_{fstat}(fd1, &sb1) != 0) {)" EOL
  R"(      const char *fmt = "failed to stat file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c1);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup2;)" EOL
  R"(    })" EOL
  R"(    if ((sb1.st_mode & _{S_IFMT}) != _{S_IFREG}) {)" EOL
  R"(      const char *fmt = "cannot copy non-file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c1);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup2;)" EOL
  R"(    })" EOL
  R"(    int fd2 = _{open}(c2, _{O_WRONLY} | _{O_CREAT});)" EOL
  R"(    if (fd2 < 0) {)" EOL
  R"(      const char *fmt = "failed to open file descriptor of `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c2);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c2);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup2;)" EOL
  R"(    })" EOL
  R"(    _{struct stat} sb2;)" EOL
  R"(    if (_{fstat}(fd2, &sb2) != 0) {)" EOL
  R"(      const char *fmt = "failed to stat file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c2);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c2);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup3;)" EOL
  R"(    })" EOL
  R"(    if (sb1.st_dev == sb2.st_dev && sb1.st_ino == sb2.st_ino) {)" EOL
  R"(      const char *fmt = "can't copy same file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c2);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c2);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup3;)" EOL
  R"(    })" EOL
  R"(    if (sb2.st_size > 0 && _{ftruncate}(fd2, 0) != 0) {)" EOL
  R"(      const char *fmt = "failed to truncate file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c2);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c2);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup3;)" EOL
  R"(    })" EOL
  R"(    if (_{fchmod}(fd2, sb1.st_mode) != 0) {)" EOL
  R"(      const char *fmt = "failed to chmod file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c2);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c2);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_copyFileSync_cleanup3;)" EOL
  R"(    })" EOL
  R"(    _{size_t} bytes = sb1.st_size;)" EOL
  R"(    _{size_t} buf_len = 8192;)" EOL
  R"(    char buf[buf_len];)" EOL
  R"(    while (bytes != 0) {)" EOL
  R"(      _{ssize_t} read_bytes_raw = _{read}(fd1, buf, bytes > buf_len ? buf_len : bytes);)" EOL
  R"(      if (read_bytes_raw <= 0) {)" EOL
  R"(        const char *fmt = "failed to read data from file `%s`";)" EOL
  R"(        _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1);)" EOL
  R"(        char *d = _{alloc}(z);)" EOL
  R"(        _{sprintf}(d, fmt, c1);)" EOL
  R"(        _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(        goto fs_copyFileSync_cleanup3;)" EOL
  R"(      })" EOL
  R"(      _{size_t} read_bytes = read_bytes_raw;)" EOL
  R"(      _{size_t} written_bytes = 0;)" EOL
  R"(      while (written_bytes != read_bytes) {)" EOL
  R"(        _{ssize_t} written_bytes_raw = _{write}(fd2, &buf[written_bytes], read_bytes - written_bytes);)" EOL
  R"(        if (written_bytes_raw <= 0) {)" EOL
  R"(          const char *fmt = "failed to write data to file `%s`";)" EOL
  R"(          _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c2);)" EOL
  R"(          char *d = _{alloc}(z);)" EOL
  R"(          _{sprintf}(d, fmt, c2);)" EOL
  R"(          _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(          goto fs_copyFileSync_cleanup3;)" EOL
  R"(        })" EOL
  R"(        written_bytes += (_{size_t}) written_bytes_raw;)" EOL
  R"(      })" EOL
  R"(      bytes -= written_bytes;)" EOL
  R"(    })" EOL
  R"(fs_copyFileSync_cleanup3:)" EOL
  R"(    _{close}(fd2);)" EOL
  R"(fs_copyFileSync_cleanup2:)" EOL
  R"(    _{close}(fd1);)" EOL
  R"(  #endif)" EOL
  R"(fs_copyFileSync_cleanup1:)" EOL
  R"(  _{free}(c2);)" EOL
  R"(  _{free}(c1);)" EOL
  R"(  _{str_free}(n2);)" EOL
  R"(  _{str_free}(n1);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(_{bool} fs_existsSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
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
  R"(  char *c = _{str_cstr}(s);)" EOL
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
  R"(  char *c = _{str_cstr}(s);)" EOL
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
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  _{bool} b = _{false};)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{HANDLE} h = _{CreateFile}(c, 0, _{FILE_SHARE_READ}, _{NULL}, _{OPEN_EXISTING}, _{FILE_FLAG_BACKUP_SEMANTICS} | _{FILE_FLAG_OPEN_REPARSE_POINT}, _{NULL});)" EOL
  R"(    if (h == _{INVALID_HANDLE_VALUE}) {)" EOL
  R"(      _{free}(c);)" EOL
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
  R"(          _{size_t} l = _{strlen}(t);)" EOL
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

  R"(void fs_linkSync (_{struct str} s1, _{struct str} s2) {)" EOL
  R"(  char *c1 = _{str_cstr}(s1);)" EOL
  R"(  char *c2 = _{str_cstr}(s2);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    for (_{size_t} i = 0; i < s1.l; i++) c1[i] = (c1[i] == '/' ? '\\' : c1[i]);)" EOL
  R"(    for (_{size_t} i = 0; i < s2.l; i++) c2[i] = (c2[i] == '/' ? '\\' : c2[i]);)" EOL
  R"(    _{DWORD} flags = 0;)" EOL
  R"(    if (_{GetFileAttributes}(c1) & _{FILE_ATTRIBUTE_DIRECTORY}) flags |= _{SYMBOLIC_LINK_FLAG_DIRECTORY};)" EOL
  R"(    _{bool} r = _{CreateSymbolicLink}(c2, c1, flags);)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{symlink}(c1, c2) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
  R"(    const char *fmt = "failed to create symbolic link from `%s` to `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1, c2);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c1, c2);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_linkSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_linkSync_cleanup:)" EOL
  R"(  _{str_free}(s1);)" EOL
  R"(  _{str_free}(s2);)" EOL
  R"(  _{free}(c1);)" EOL
  R"(  _{free}(c2);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_mkdirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{CreateDirectory}(c, _{NULL});)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{mkdir}(c, 0777) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
  R"(    const char *fmt = "failed to create directory `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_mkdirSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_mkdirSync_cleanup:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(_{struct buffer} fs_readFileSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "rb");)" EOL
  R"(  if (f == _{NULL}) {)" EOL
  R"(    const char *fmt = "failed to open file `%s` for reading";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_readFileSync_cleanup;)" EOL
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
  R"(fs_readFileSync_cleanup:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}((_{struct str}) s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  return (_{struct buffer}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} fs_realpathSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  char *d = _{NULL};)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{HANDLE} h = _{CreateFile}(c, 0, 0, _{NULL}, _{OPEN_EXISTING}, _{FILE_ATTRIBUTE_NORMAL} | _{FILE_FLAG_BACKUP_SEMANTICS}, _{NULL});)" EOL
  R"(    if (h == _{INVALID_HANDLE_VALUE}) {)" EOL
  R"(      const char *fmt = "failed to create handle to get real path of file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_realpathSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(    _{size_t} l = _{GetFinalPathNameByHandle}(h, _{NULL}, 0, _{VOLUME_NAME_DOS});)" EOL
  R"(    if (l == 0) {)" EOL
  R"(      const char *fmt = "not enough memory to get real path of file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_realpathSync_cleanup2;)" EOL
  R"(    })" EOL
  R"(    char *r = _{alloc}(l + 1);)" EOL
  R"(    if (_{GetFinalPathNameByHandle}(h, r, _{MAX_PATH}, _{VOLUME_NAME_DOS}) == 0) {)" EOL
  R"(      const char *fmt = "failed to get real path by handle of file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_realpathSync_cleanup3;)" EOL
  R"(    })" EOL
  R"(    if (_{memcmp}(r, "\\\\?\\UNC\\", 8) == 0) {)" EOL
  R"(      l -= 6;)" EOL
  R"(      d = _{alloc}(l);)" EOL
  R"(      _{memcpy}(d, &r[6], l);)" EOL
  R"(      d[0] = '\\';)" EOL
  R"(    } else if (_{memcmp}(r, "\\\\?\\", 4) == 0) {)" EOL
  R"(      l -= 4;)" EOL
  R"(      d = _{alloc}(l);)" EOL
  R"(      _{memcpy}(d, &r[4], l);)" EOL
  R"(    })" EOL
  R"(fs_realpathSync_cleanup3:)" EOL
  R"(    _{free}(r);)" EOL
  R"(fs_realpathSync_cleanup2:)" EOL
  R"(    _{CloseHandle}(h);)" EOL
  R"(    if (_{err_state}.id != -1) goto fs_realpathSync_cleanup1;)" EOL
  R"(  #else)" EOL
  R"(    d = _{realpath}(c, _{NULL});)" EOL
  R"(    _{size_t} l = d == _{NULL} ? 0 : _{strlen}(d);)" EOL
  R"(  #endif)" EOL
  R"(  if (d == _{NULL}) {)" EOL
  R"(    const char *fmt = "failed to get real path of file `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_realpathSync_cleanup1;)" EOL
  R"(  })" EOL
  R"(fs_realpathSync_cleanup1:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}((_{struct str}) s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(void fs_renameSync (_{struct str} n1, _{struct str} n2) {)" EOL
  R"(  char *c1 = _{str_cstr}(n1);)" EOL
  R"(  char *c2 = _{str_cstr}(n2);)" EOL
  R"(  if (_{rename}(c1, c2) != 0) {)" EOL
  R"(    const char *fmt = "failed to rename from `%s` to `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c1, c2);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c1, c2);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_renameSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_renameSync_cleanup:)" EOL
  R"(  _{free}(c2);)" EOL
  R"(  _{free}(c1);)" EOL
  R"(  _{str_free}(n2);)" EOL
  R"(  _{str_free}(n1);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_rmSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  if (_{remove}(c) != 0) {)" EOL
  R"(    const char *fmt = "failed to remove file `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_rmSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_rmSync_cleanup:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_rmdirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} r = _{RemoveDirectory}(c);)" EOL
  R"(  #else)" EOL
  R"(    _{bool} r = _{rmdir}(c) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
  R"(    const char *fmt = "failed to remove directory `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_rmdirSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_rmdirSync_cleanup:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(struct _{array_str} fs_scandirSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  _{struct str} *r = _{NULL};)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    if (_{GetFileAttributes}(c) == _{INVALID_FILE_ATTRIBUTES}) {)" EOL
  R"(      const char *fmt = "directory `%s` doesn't exist";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    } else if (!(_{GetFileAttributes}(c) & _{FILE_ATTRIBUTE_DIRECTORY})) {)" EOL
  R"(      const char *fmt = "failed to scan non-directory `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(    const char *fmt = s.l == 0 ? "./*" : ((s.d[s.l - 1] == '/' || s.d[s.l - 1] == '\\') ? "%s*" : "%s\\*");)" EOL
  R"(    char *b = _{alloc}(s.l + 4);)" EOL
  R"(    _{snprintf}(b, s.l + 3, fmt, c);)" EOL
  R"(    _{WIN32_FIND_DATA} m;)" EOL
  R"(    _{HANDLE} h = _{FindFirstFile}(b, &m);)" EOL
  R"(    _{free}(b);)" EOL
  R"(    if (h == _{INVALID_HANDLE_VALUE} && _{GetLastError}() != _{ERROR_FILE_NOT_FOUND}) {)" EOL
  R"(      const char *fmt = "failed to open directory `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    } else if (h == _{INVALID_HANDLE_VALUE}) {)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(    _{WIN32_FIND_DATA} *n = &m;)" EOL
  R"(    while (_{true}) {)" EOL
  R"(      if (_{strcmp}(n->cFileName, ".") != 0 && _{strcmp}(n->cFileName, "..") != 0) {)" EOL
  R"(        r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(        r[l - 1] = _{str_alloc}(n->cFileName);)" EOL
  R"(      })" EOL
  R"(      _{bool} g = _{FindNextFile}(h, n);)" EOL
  R"(      if (!g && _{GetLastError}() == _{ERROR_NO_MORE_FILES}) {)" EOL
  R"(        break;)" EOL
  R"(      } else if (!g) {)" EOL
  R"(        const char *fmt = "failed to scan next file of directory `%s`";)" EOL
  R"(        _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(        char *d = _{alloc}(z);)" EOL
  R"(        _{sprintf}(d, fmt, c);)" EOL
  R"(        _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(        goto fs_scandirSync_cleanup2;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(fs_scandirSync_cleanup2:)" EOL
  R"(    _{FindClose}(h);)" EOL
  R"(  #else)" EOL
  R"(    _{struct stat} sb;)" EOL
  R"(    if (_{stat}(c, &sb) != 0) {)" EOL
  R"(      const char *fmt = "directory `%s` doesn't exist";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    } else if ((sb.st_mode & _{S_IFMT}) != _{S_IFDIR}) {)" EOL
  R"(      const char *fmt = "failed to scan non-directory `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(    _{DIR} *f = _{opendir}(c);)" EOL
  R"(    if (f == _{NULL}) {)" EOL
  R"(      const char *fmt = "failed to open directory `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_scandirSync_cleanup1;)" EOL
  R"(    })" EOL
  R"(    _{struct dirent} *a;)" EOL
  R"(    while ((a = _{readdir}(f)) != _{NULL}) {)" EOL
  R"(      if (_{strcmp}(a->d_name, ".") == 0 || _{strcmp}(a->d_name, "..") == 0) continue;)" EOL
  R"(      r = _{re_alloc}(r, ++l * sizeof(_{struct str}));)" EOL
  R"(      r[l - 1] = _{str_alloc}(a->d_name);)" EOL
  R"(    })" EOL
  R"(    _{closedir}(f);)" EOL
  R"(  #endif)" EOL
  R"(  for (_{size_t} j = 0; j < l; j++) {)" EOL
  R"(    for (_{size_t} k = j + 1; k < l; k++) {)" EOL
  R"(      if (_{memcmp}(r[j].d, r[k].d, r[j].l > r[k].l ? r[k].l : r[j].l) > 0) {)" EOL
  R"(        _{struct str} t = r[j];)" EOL
  R"(        r[j] = r[k];)" EOL
  R"(        r[k] = t;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(fs_scandirSync_cleanup1:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  return (struct _{array_str}) {r, l};)" EOL
  R"(})" EOL,

  R"(struct _{fs_Stats} *fs_statSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  struct _{fs_Stats} *r;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{struct _stat} sb;)" EOL
  R"(    if (_{_stat}(c, &sb) != 0) {)" EOL
  R"(      const char *fmt = "failed to stat file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_statSync_cleanup;)" EOL
  R"(    })" EOL
  R"(    r = _{fs_Stats_alloc}(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atime, 0, sb.st_mtime, 0, sb.st_ctime, 0, sb.st_ctime, 0, sb.st_size, 0, 4096);)" EOL
  R"(  #else)" EOL
  R"(    _{struct stat} sb;)" EOL
  R"(    if (_{stat}(c, &sb) != 0) {)" EOL
  R"(      const char *fmt = "failed to stat file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_statSync_cleanup;)" EOL
  R"(    })" EOL
  R"(    #ifdef _{THE_OS_MACOS})" EOL
  R"(      r = _{fs_Stats_alloc}(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atimespec.tv_sec, sb.st_atimespec.tv_nsec, sb.st_mtimespec.tv_sec, sb.st_mtimespec.tv_nsec, sb.st_ctimespec.tv_sec, sb.st_ctimespec.tv_nsec, sb.st_birthtimespec.tv_sec, sb.st_birthtimespec.tv_nsec, sb.st_size, sb.st_blocks, sb.st_blksize);)" EOL
  R"(    #else)" EOL
  R"(      r = _{fs_Stats_alloc}(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atim.tv_sec, sb.st_atim.tv_nsec, sb.st_mtim.tv_sec, sb.st_mtim.tv_nsec, sb.st_ctim.tv_sec, sb.st_ctim.tv_nsec, sb.st_ctim.tv_sec, sb.st_ctim.tv_nsec, sb.st_size, sb.st_blocks, sb.st_blksize);)" EOL
  R"(    #endif)" EOL
  R"(  #endif)" EOL
  R"(fs_statSync_cleanup:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL,

  R"(void fs_unlinkSync (_{struct str} s) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  _{bool} r;)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    if (_{GetFileAttributes}(c) & _{FILE_ATTRIBUTE_DIRECTORY}) {)" EOL
  R"(      r = _{RemoveDirectory}(c);)" EOL
  R"(    } else {)" EOL
  R"(      r = _{_unlink}(c) == 0;)" EOL
  R"(    })" EOL
  R"(  #else)" EOL
  R"(    r = _{unlink}(c) == 0;)" EOL
  R"(  #endif)" EOL
  R"(  if (!r) {)" EOL
  R"(    const char *fmt = "failed to unlink path `%s`";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_unlinkSync_cleanup;)" EOL
  R"(  })" EOL
  R"(fs_unlinkSync_cleanup:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL,

  R"(void fs_writeFileSync (_{struct str} s, _{struct buffer} b) {)" EOL
  R"(  char *c = _{str_cstr}(s);)" EOL
  R"(  _{FILE} *f = _{fopen}(c, "wb");)" EOL
  R"(  if (f == _{NULL}) {)" EOL
  R"(    const char *fmt = "failed to open file `%s` for writing";)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(    char *d = _{alloc}(z);)" EOL
  R"(    _{sprintf}(d, fmt, c);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    goto fs_writeFileSync_cleanup1;)" EOL
  R"(  })" EOL
  R"(  if (b.l != 0) {)" EOL
  R"(    if (_{fwrite}(b.d, b.l, 1, f) != 1) {)" EOL
  R"(      const char *fmt = "failed to write file `%s`";)" EOL
  R"(      _{size_t} z = _{snprintf}(_{NULL}, 0, fmt, c);)" EOL
  R"(      char *d = _{alloc}(z);)" EOL
  R"(      _{sprintf}(d, fmt, c);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}((_{struct str}) {d, z}, (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      goto fs_writeFileSync_cleanup2;)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(fs_writeFileSync_cleanup2:)" EOL
  R"(  _{fclose}(f);)" EOL
  R"(fs_writeFileSync_cleanup1:)" EOL
  R"(  _{free}(c);)" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  _{buffer_free}(b);)" EOL
  R"(  if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(})" EOL
};
