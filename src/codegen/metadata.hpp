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

#ifndef SRC_CODEGEN_METADATA_HPP
#define SRC_CODEGEN_METADATA_HPP

#include <map>
#include <string>

extern const std::map<std::string, std::string> codegenMetadata = {
  std::pair<std::string, std::string>{"THE_EOL", "definitions"},
  std::pair<std::string, std::string>{"THE_OS_LINUX", "definitions"},
  std::pair<std::string, std::string>{"THE_OS_MACOS", "definitions"},
  std::pair<std::string, std::string>{"THE_OS_WINDOWS", "definitions"},

  std::pair<std::string, std::string>{"alloc", "fnAlloc"},
  std::pair<std::string, std::string>{"any_copy", "fnAnyCopy"},
  std::pair<std::string, std::string>{"any_free", "fnAnyFree"},
  std::pair<std::string, std::string>{"any_realloc", "fnAnyRealloc"},
  std::pair<std::string, std::string>{"any_str", "fnAnyStr"},
  std::pair<std::string, std::string>{"bool_str", "fnBoolStr"},
  std::pair<std::string, std::string>{"buffer_copy", "fnBufferCopy"},
  std::pair<std::string, std::string>{"buffer_eq", "fnBufferEq"},
  std::pair<std::string, std::string>{"buffer_free", "fnBufferFree"},
  std::pair<std::string, std::string>{"buffer_ne", "fnBufferNe"},
  std::pair<std::string, std::string>{"buffer_realloc", "fnBufferRealloc"},
  std::pair<std::string, std::string>{"buffer_str", "fnBufferStr"},
  std::pair<std::string, std::string>{"byte_str", "fnByteStr"},
  std::pair<std::string, std::string>{"char_is_alpha", "fnCharIsAlpha"},
  std::pair<std::string, std::string>{"char_is_alpha_num", "fnCharIsAlphaNum"},
  std::pair<std::string, std::string>{"char_is_digit", "fnCharIsDigit"},
  std::pair<std::string, std::string>{"char_is_space", "fnCharIsSpace"},
  std::pair<std::string, std::string>{"char_repeat", "fnCharRepeat"},
  std::pair<std::string, std::string>{"char_str", "fnCharStr"},
  std::pair<std::string, std::string>{"cstr_concat_str", "fnCstrConcatStr"},
  std::pair<std::string, std::string>{"cstr_eq_cstr", "fnCstrEqCstr"},
  std::pair<std::string, std::string>{"cstr_eq_str", "fnCstrEqStr"},
  std::pair<std::string, std::string>{"cstr_ne_cstr", "fnCstrNeCstr"},
  std::pair<std::string, std::string>{"cstr_ne_str", "fnCstrNeStr"},
  std::pair<std::string, std::string>{"f32_str", "fnF32Str"},
  std::pair<std::string, std::string>{"f64_str", "fnF64Str"},
  std::pair<std::string, std::string>{"float_str", "fnFloatStr"},
  std::pair<std::string, std::string>{"i8_str", "fnI8Str"},
  std::pair<std::string, std::string>{"i16_str", "fnI16Str"},
  std::pair<std::string, std::string>{"i32_str", "fnI32Str"},
  std::pair<std::string, std::string>{"i64_str", "fnI64Str"},
  std::pair<std::string, std::string>{"int_str", "fnIntStr"},
  std::pair<std::string, std::string>{"os_name", "fnOSName"},
  std::pair<std::string, std::string>{"path_basename", "fnPathBasename"},
  std::pair<std::string, std::string>{"path_dirname", "fnPathDirname"},
  std::pair<std::string, std::string>{"print", "fnPrint"},
  std::pair<std::string, std::string>{"re_alloc", "fnReAlloc"},
  std::pair<std::string, std::string>{"sleep_sync", "fnSleepSync"},
  std::pair<std::string, std::string>{"str_alloc", "fnStrAlloc"},
  std::pair<std::string, std::string>{"str_at", "fnStrAt"},
  std::pair<std::string, std::string>{"str_concat_cstr", "fnStrConcatCstr"},
  std::pair<std::string, std::string>{"str_concat_str", "fnStrConcatStr"},
  std::pair<std::string, std::string>{"str_copy", "fnStrCopy"},
  std::pair<std::string, std::string>{"str_eq_cstr", "fnStrEqCstr"},
  std::pair<std::string, std::string>{"str_eq_str", "fnStrEqStr"},
  std::pair<std::string, std::string>{"str_escape", "fnStrEscape"},
  std::pair<std::string, std::string>{"str_find", "fnStrFind"},
  std::pair<std::string, std::string>{"str_free", "fnStrFree"},
  std::pair<std::string, std::string>{"str_len", "fnStrLen"},
  std::pair<std::string, std::string>{"str_lines", "fnStrLines"},
  std::pair<std::string, std::string>{"str_lower", "fnStrLower"},
  std::pair<std::string, std::string>{"str_lower_first", "fnStrLowerFirst"},
  std::pair<std::string, std::string>{"str_ne_cstr", "fnStrNeCstr"},
  std::pair<std::string, std::string>{"str_ne_str", "fnStrNeStr"},
  std::pair<std::string, std::string>{"str_not", "fnStrNot"},
  std::pair<std::string, std::string>{"str_realloc", "fnStrRealloc"},
  std::pair<std::string, std::string>{"str_slice", "fnStrSlice"},
  std::pair<std::string, std::string>{"str_to_buffer", "fnStrToBuffer"},
  std::pair<std::string, std::string>{"str_trim", "fnStrTrim"},
  std::pair<std::string, std::string>{"str_upper", "fnStrUpper"},
  std::pair<std::string, std::string>{"str_upper_first", "fnStrUpperFirst"},
  std::pair<std::string, std::string>{"u8_str", "fnU8Str"},
  std::pair<std::string, std::string>{"u16_str", "fnU16Str"},
  std::pair<std::string, std::string>{"u32_str", "fnU32Str"},
  std::pair<std::string, std::string>{"u64_str", "fnU64Str"},

  std::pair<std::string, std::string>{"struct any", "typeAny"},
  std::pair<std::string, std::string>{"struct buffer", "typeBuffer"},
  std::pair<std::string, std::string>{"struct str", "typeStr"},
  std::pair<std::string, std::string>{"struct win_reparse_data_buffer", "typeWinReparseDataBuffer"},

  std::pair<std::string, std::string>{"isalpha", "libCtype"},

  std::pair<std::string, std::string>{"DIR", "libDirent"},
  std::pair<std::string, std::string>{"struct dirent", "libDirent"},
  std::pair<std::string, std::string>{"closedir", "libDirent"},
  std::pair<std::string, std::string>{"opendir", "libDirent"},
  std::pair<std::string, std::string>{"readdir", "libDirent"},

  std::pair<std::string, std::string>{"PRId8", "libInttypes"},
  std::pair<std::string, std::string>{"PRId16", "libInttypes"},
  std::pair<std::string, std::string>{"PRId32", "libInttypes"},
  std::pair<std::string, std::string>{"PRId64", "libInttypes"},

  std::pair<std::string, std::string>{"bool", "libStdbool"},
  std::pair<std::string, std::string>{"true", "libStdbool"},
  std::pair<std::string, std::string>{"false", "libStdbool"},

  std::pair<std::string, std::string>{"size_t", "libStddef"},
  std::pair<std::string, std::string>{"NULL", "libStddef"},

  std::pair<std::string, std::string>{"int8_t", "libStdint"},
  std::pair<std::string, std::string>{"int16_t", "libStdint"},
  std::pair<std::string, std::string>{"int32_t", "libStdint"},
  std::pair<std::string, std::string>{"int64_t", "libStdint"},

  std::pair<std::string, std::string>{"FILE", "libStdio"},
  std::pair<std::string, std::string>{"fclose", "libStdio"},
  std::pair<std::string, std::string>{"fopen", "libStdio"},
  std::pair<std::string, std::string>{"fprintf", "libStdio"},
  std::pair<std::string, std::string>{"fread", "libStdio"},
  std::pair<std::string, std::string>{"fwrite", "libStdio"},
  std::pair<std::string, std::string>{"pclose", "libStdio"},
  std::pair<std::string, std::string>{"popen", "libStdio"},
  std::pair<std::string, std::string>{"remove", "libStdio"},
  std::pair<std::string, std::string>{"stderr", "libStdio"},
  std::pair<std::string, std::string>{"stdin", "libStdio"},
  std::pair<std::string, std::string>{"stdout", "libStdio"},
  std::pair<std::string, std::string>{"_pclose", "libStdio"},
  std::pair<std::string, std::string>{"_popen", "libStdio"},

  std::pair<std::string, std::string>{"EXIT_SUCCESS", "libStdlib"},
  std::pair<std::string, std::string>{"EXIT_FAILURE", "libStdlib"},
  std::pair<std::string, std::string>{"exit", "libStdlib"},
  std::pair<std::string, std::string>{"free", "libStdlib"},
  std::pair<std::string, std::string>{"realpath", "libStdlib"},

  std::pair<std::string, std::string>{"memcpy", "libString"},
  std::pair<std::string, std::string>{"strcmp", "libString"},
  std::pair<std::string, std::string>{"strlen", "libString"},
  std::pair<std::string, std::string>{"strncmp", "libString"},

  std::pair<std::string, std::string>{"struct stat", "libSysStat"},
  std::pair<std::string, std::string>{"struct _stat", "libSysStat"},
  std::pair<std::string, std::string>{"S_IFDIR", "libSysStat"},
  std::pair<std::string, std::string>{"S_IFLNK", "libSysStat"},
  std::pair<std::string, std::string>{"S_IFMT", "libSysStat"},
  std::pair<std::string, std::string>{"S_IFREG", "libSysStat"},
  std::pair<std::string, std::string>{"chmod", "libSysStat"},
  std::pair<std::string, std::string>{"lstat", "libSysStat"},
  std::pair<std::string, std::string>{"mkdir", "libSysStat"},
  std::pair<std::string, std::string>{"stat", "libSysStat"},
  std::pair<std::string, std::string>{"_S_IFDIR", "libSysStat"},
  std::pair<std::string, std::string>{"_S_IFMT", "libSysStat"},
  std::pair<std::string, std::string>{"_S_IFREG", "libSysStat"},
  std::pair<std::string, std::string>{"_lstat", "libSysStat"},
  std::pair<std::string, std::string>{"_stat", "libSysStat"},

  std::pair<std::string, std::string>{"access", "libUnistd"},
  std::pair<std::string, std::string>{"chown", "libUnistd"},
  std::pair<std::string, std::string>{"getcwd", "libUnistd"},
  std::pair<std::string, std::string>{"getgid", "libUnistd"},
  std::pair<std::string, std::string>{"getuid", "libUnistd"},
  std::pair<std::string, std::string>{"rmdir", "libUnistd"},
  std::pair<std::string, std::string>{"symlink", "libUnistd"},

  std::pair<std::string, std::string>{"_getcwd", "libWinDirect"},
  std::pair<std::string, std::string>{"_rmdir", "libWinDirect"},

  std::pair<std::string, std::string>{"_access", "libWinIo"},
  std::pair<std::string, std::string>{"_chmod", "libWinIo"},

  std::pair<std::string, std::string>{"DWORD", "libWindows"},
  std::pair<std::string, std::string>{"FILE_ATTRIBUTE_NORMAL", "libWindows"},
  std::pair<std::string, std::string>{"FILE_SHARE_READ", "libWindows"},
  std::pair<std::string, std::string>{"FILE_SHARE_WRITE", "libWindows"},
  std::pair<std::string, std::string>{"FSCTL_GET_REPARSE_POINT", "libWindows"},
  std::pair<std::string, std::string>{"GENERIC_READ", "libWindows"},
  std::pair<std::string, std::string>{"HANDLE", "libWindows"},
  std::pair<std::string, std::string>{"INVALID_HANDLE_VALUE", "libWindows"},
  std::pair<std::string, std::string>{"IO_REPARSE_TAG_APPEXECLINK", "libWindows"},
  std::pair<std::string, std::string>{"IO_REPARSE_TAG_MOUNT_POINT", "libWindows"},
  std::pair<std::string, std::string>{"IO_REPARSE_TAG_SYMLINK", "libWindows"},
  std::pair<std::string, std::string>{"MAXIMUM_REPARSE_DATA_BUFFER_SIZE", "libWindows"},
  std::pair<std::string, std::string>{"MAX_PATH", "libWindows"},
  std::pair<std::string, std::string>{"OPEN_EXISTING", "libWindows"},
  std::pair<std::string, std::string>{"VOLUME_NAME_DOS", "libWindows"},
  std::pair<std::string, std::string>{"CloseHandle", "libWindows"},
  std::pair<std::string, std::string>{"CreateDirectory", "libWindows"},
  std::pair<std::string, std::string>{"CreateFile", "libWindows"},
  std::pair<std::string, std::string>{"CreateSymbolicLink", "libWindows"},
  std::pair<std::string, std::string>{"DeviceIoControl", "libWindows"},
  std::pair<std::string, std::string>{"GetFinalPathNameByHandle", "libWindows"}
};

#endif
