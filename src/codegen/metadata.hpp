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
#include <set>
#include <string>

extern const std::map<std::string, std::set<std::string>> codegenMetadata = {
  std::pair<std::string, std::set<std::string>>{"THE_EOL", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_OS_LINUX", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_OS_MACOS", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_OS_WINDOWS", {"definitions"}},

  std::pair<std::string, std::set<std::string>>{"alloc", {"fnAlloc"}},
  std::pair<std::string, std::set<std::string>>{"any_copy", {"fnAnyCopy"}},
  std::pair<std::string, std::set<std::string>>{"any_free", {"fnAnyFree"}},
  std::pair<std::string, std::set<std::string>>{"any_realloc", {"fnAnyRealloc"}},
  std::pair<std::string, std::set<std::string>>{"any_str", {"fnAnyStr"}},
  std::pair<std::string, std::set<std::string>>{"bool_str", {"fnBoolStr"}},
  std::pair<std::string, std::set<std::string>>{"buffer_copy", {"fnBufferCopy"}},
  std::pair<std::string, std::set<std::string>>{"buffer_eq", {"fnBufferEq"}},
  std::pair<std::string, std::set<std::string>>{"buffer_free", {"fnBufferFree"}},
  std::pair<std::string, std::set<std::string>>{"buffer_ne", {"fnBufferNe"}},
  std::pair<std::string, std::set<std::string>>{"buffer_realloc", {"fnBufferRealloc"}},
  std::pair<std::string, std::set<std::string>>{"buffer_str", {"fnBufferStr"}},
  std::pair<std::string, std::set<std::string>>{"byte_str", {"fnByteStr"}},
  std::pair<std::string, std::set<std::string>>{"char_isDigit", {"fnCharIsDigit"}},
  std::pair<std::string, std::set<std::string>>{"char_isLetter", {"fnCharIsLetter"}},
  std::pair<std::string, std::set<std::string>>{"char_isLetterOrDigit", {"fnCharIsLetterOrDigit"}},
  std::pair<std::string, std::set<std::string>>{"char_isWhitespace", {"fnCharIsWhitespace"}},
  std::pair<std::string, std::set<std::string>>{"char_repeat", {"fnCharRepeat"}},
  std::pair<std::string, std::set<std::string>>{"char_str", {"fnCharStr"}},
  std::pair<std::string, std::set<std::string>>{"cstr_concat_str", {"fnCstrConcatStr"}},
  std::pair<std::string, std::set<std::string>>{"cstr_eq_cstr", {"fnCstrEqCstr"}},
  std::pair<std::string, std::set<std::string>>{"cstr_eq_str", {"fnCstrEqStr"}},
  std::pair<std::string, std::set<std::string>>{"cstr_ne_cstr", {"fnCstrNeCstr"}},
  std::pair<std::string, std::set<std::string>>{"cstr_ne_str", {"fnCstrNeStr"}},
  std::pair<std::string, std::set<std::string>>{"f32_str", {"fnF32Str"}},
  std::pair<std::string, std::set<std::string>>{"f64_str", {"fnF64Str"}},
  std::pair<std::string, std::set<std::string>>{"float_str", {"fnFloatStr"}},
  std::pair<std::string, std::set<std::string>>{"i8_str", {"fnI8Str"}},
  std::pair<std::string, std::set<std::string>>{"i16_str", {"fnI16Str"}},
  std::pair<std::string, std::set<std::string>>{"i32_str", {"fnI32Str"}},
  std::pair<std::string, std::set<std::string>>{"i64_str", {"fnI64Str"}},
  std::pair<std::string, std::set<std::string>>{"int_str", {"fnIntStr"}},
  std::pair<std::string, std::set<std::string>>{"os_name", {"fnOSName"}},
  std::pair<std::string, std::set<std::string>>{"path_basename", {"fnPathBasename"}},
  std::pair<std::string, std::set<std::string>>{"path_dirname", {"fnPathDirname"}},
  std::pair<std::string, std::set<std::string>>{"print", {"fnPrint"}},
  std::pair<std::string, std::set<std::string>>{"re_alloc", {"fnReAlloc"}},
  std::pair<std::string, std::set<std::string>>{"str_alloc", {"fnStrAlloc"}},
  std::pair<std::string, std::set<std::string>>{"str_at", {"fnStrAt"}},
  std::pair<std::string, std::set<std::string>>{"str_concat_cstr", {"fnStrConcatCstr"}},
  std::pair<std::string, std::set<std::string>>{"str_concat_str", {"fnStrConcatStr"}},
  std::pair<std::string, std::set<std::string>>{"str_copy", {"fnStrCopy"}},
  std::pair<std::string, std::set<std::string>>{"str_cstr", {"fnStrCstr"}},
  std::pair<std::string, std::set<std::string>>{"str_empty", {"fnStrEmpty"}},
  std::pair<std::string, std::set<std::string>>{"str_eq_cstr", {"fnStrEqCstr"}},
  std::pair<std::string, std::set<std::string>>{"str_eq_str", {"fnStrEqStr"}},
  std::pair<std::string, std::set<std::string>>{"str_escape", {"fnStrEscape"}},
  std::pair<std::string, std::set<std::string>>{"str_find", {"fnStrFind"}},
  std::pair<std::string, std::set<std::string>>{"str_free", {"fnStrFree"}},
  std::pair<std::string, std::set<std::string>>{"str_len", {"fnStrLen"}},
  std::pair<std::string, std::set<std::string>>{"str_lines", {"fnStrLines"}},
  std::pair<std::string, std::set<std::string>>{"str_lower", {"fnStrLower"}},
  std::pair<std::string, std::set<std::string>>{"str_lowerFirst", {"fnStrLowerFirst"}},
  std::pair<std::string, std::set<std::string>>{"str_ne_cstr", {"fnStrNeCstr"}},
  std::pair<std::string, std::set<std::string>>{"str_ne_str", {"fnStrNeStr"}},
  std::pair<std::string, std::set<std::string>>{"str_not", {"fnStrNot"}},
  std::pair<std::string, std::set<std::string>>{"str_realloc", {"fnStrRealloc"}},
  std::pair<std::string, std::set<std::string>>{"str_slice", {"fnStrSlice"}},
  std::pair<std::string, std::set<std::string>>{"str_to_buffer", {"fnStrToBuffer"}},
  std::pair<std::string, std::set<std::string>>{"str_trim", {"fnStrTrim"}},
  std::pair<std::string, std::set<std::string>>{"str_upper", {"fnStrUpper"}},
  std::pair<std::string, std::set<std::string>>{"str_upperFirst", {"fnStrUpperFirst"}},
  std::pair<std::string, std::set<std::string>>{"thread_sleep", {"fnThreadSleep"}},
  std::pair<std::string, std::set<std::string>>{"u8_str", {"fnU8Str"}},
  std::pair<std::string, std::set<std::string>>{"u16_str", {"fnU16Str"}},
  std::pair<std::string, std::set<std::string>>{"u32_str", {"fnU32Str"}},
  std::pair<std::string, std::set<std::string>>{"u64_str", {"fnU64Str"}},

  std::pair<std::string, std::set<std::string>>{"struct any", {"typeAny"}},
  std::pair<std::string, std::set<std::string>>{"struct buffer", {"typeBuffer"}},
  std::pair<std::string, std::set<std::string>>{"struct request", {"typeRequest"}},
  std::pair<std::string, std::set<std::string>>{"struct str", {"typeStr"}},
  std::pair<std::string, std::set<std::string>>{"struct win_reparse_data_buffer", {"typeWinReparseDataBuffer"}},

  std::pair<std::string, std::set<std::string>>{"lib_openssl_init", {"varLibOpensslInit"}},
  std::pair<std::string, std::set<std::string>>{"lib_ws2_init", {"varLibWs2Init"}},

  std::pair<std::string, std::set<std::string>>{"isdigit", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"isalnum", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"isalpha", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"tolower", {"libCtype"}},

  std::pair<std::string, std::set<std::string>>{"DIR", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"struct dirent", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"closedir", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"opendir", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"readdir", {"libDirent"}},

  std::pair<std::string, std::set<std::string>>{"PRId8", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRId16", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRId32", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRId64", {"libInttypes"}},

  std::pair<std::string, std::set<std::string>>{"struct addrinfo", {"libNetdb", "libWinWs2tcpip"}},
  std::pair<std::string, std::set<std::string>>{"freeaddrinfo", {"libNetdb", "libWinWs2tcpip"}},
  std::pair<std::string, std::set<std::string>>{"getaddrinfo", {"libNetdb", "libWinWs2tcpip"}},

  std::pair<std::string, std::set<std::string>>{"IPPROTO_TCP", {"libNetinetIn", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"WSADATA", {"libNetinetIn", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"WSAStartup", {"libNetinetIn", "libWinsock2"}},

  std::pair<std::string, std::set<std::string>>{"SSL_CTX_free", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_CTX_new", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_connect", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_free", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_library_init", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_new", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_read", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_set_fd", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_set_tlsext_host_name", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_shutdown", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"SSL_write", {"libOpensslSsl"}},
  std::pair<std::string, std::set<std::string>>{"TLS_client_method", {"libOpensslSsl"}},

  std::pair<std::string, std::set<std::string>>{"va_arg", {"libStdarg"}},
  std::pair<std::string, std::set<std::string>>{"va_end", {"libStdarg"}},
  std::pair<std::string, std::set<std::string>>{"va_list", {"libStdarg"}},
  std::pair<std::string, std::set<std::string>>{"va_start", {"libStdarg"}},

  std::pair<std::string, std::set<std::string>>{"bool", {"libStdbool"}},
  std::pair<std::string, std::set<std::string>>{"false", {"libStdbool"}},
  std::pair<std::string, std::set<std::string>>{"true", {"libStdbool"}},

  std::pair<std::string, std::set<std::string>>{"size_t", {"libStddef"}},
  std::pair<std::string, std::set<std::string>>{"NULL", {"libStddef"}},

  std::pair<std::string, std::set<std::string>>{"int8_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int16_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int32_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int64_t", {"libStdint"}},

  std::pair<std::string, std::set<std::string>>{"FILE", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fclose", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fopen", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fprintf", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fread", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fwrite", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"pclose", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"popen", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"remove", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"snprintf", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"sprintf", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"stderr", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"stdin", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"stdout", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"_pclose", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"_popen", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"_snprintf", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"_unlink", {"libStdio"}},

  std::pair<std::string, std::set<std::string>>{"EXIT_SUCCESS", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"EXIT_FAILURE", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"exit", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"free", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"realpath", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtoul", {"libStdlib"}},

  std::pair<std::string, std::set<std::string>>{"memcmp", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"memcpy", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"memmove", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"memset", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"strcmp", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"strlen", {"libString"}},

  std::pair<std::string, std::set<std::string>>{"struct stat", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"struct _stat", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"S_IFDIR", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"S_IFLNK", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"S_IFMT", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"S_IFREG", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"_S_IFDIR", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"_S_IFMT", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"_S_IFREG", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"chmod", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"lstat", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"mkdir", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"stat", {"libSysStat"}},
  std::pair<std::string, std::set<std::string>>{"_stat", {"libSysStat"}},

  std::pair<std::string, std::set<std::string>>{"AF_INET", {"libSysSocket", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"SOCK_STREAM", {"libSysSocket", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"connect", {"libSysSocket", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"recv", {"libSysSocket", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"send", {"libSysSocket", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"shutdown", {"libSysSocket", "libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"socket", {"libSysSocket", "libWinsock2"}},

  std::pair<std::string, std::set<std::string>>{"ssize_t", {"libSysTypes"}},

  std::pair<std::string, std::set<std::string>>{"access", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"chown", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"close", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"getcwd", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"getgid", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"getuid", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"rmdir", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"symlink", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"unlink", {"libUnistd"}},

  std::pair<std::string, std::set<std::string>>{"_getcwd", {"libWinDirect"}},

  std::pair<std::string, std::set<std::string>>{"_access", {"libWinIo"}},
  std::pair<std::string, std::set<std::string>>{"_chmod", {"libWinIo"}},

  std::pair<std::string, std::set<std::string>>{"DWORD", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"ERROR_FILE_NOT_FOUND", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"ERROR_NO_MORE_FILES", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_ATTRIBUTE_DIRECTORY", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_ATTRIBUTE_NORMAL", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_FLAG_BACKUP_SEMANTICS", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_FLAG_OPEN_REPARSE_POINT", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_SHARE_DELETE", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_SHARE_READ", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILE_SHARE_WRITE", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FSCTL_GET_REPARSE_POINT", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GENERIC_READ", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GENERIC_WRITE", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"HANDLE", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"INVALID_FILE_ATTRIBUTES", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"INVALID_HANDLE_VALUE", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"IO_REPARSE_TAG_APPEXECLINK", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"IO_REPARSE_TAG_MOUNT_POINT", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"IO_REPARSE_TAG_SYMLINK", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"MAKEWORD", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"MAXIMUM_REPARSE_DATA_BUFFER_SIZE", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"MAX_PATH", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"OPEN_EXISTING", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"SYMBOLIC_LINK_FLAG_DIRECTORY", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"VOLUME_NAME_DOS", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"WIN32_FIND_DATA", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"CloseHandle", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"CreateDirectory", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"CreateFile", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"CreateSymbolicLink", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"DeviceIoControl", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FindClose", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FindFirstFile", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FindNextFile", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetFileAttributes", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetFinalPathNameByHandle", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetLastError", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"RemoveDirectory", {"libWindows"}},

  std::pair<std::string, std::set<std::string>>{"INVALID_SOCKET", {"libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"SOCKET", {"libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"SOCKET_ERROR", {"libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"closesocket", {"libWinsock2"}}
};

#endif
