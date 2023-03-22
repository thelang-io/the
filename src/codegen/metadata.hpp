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

// (?<!_\{|[a-zA-Z0-9_])(EXIT_FAILURE|EXIT_SUCCESS|FILE|NULL|PRId8|PRId16|PRId32|PRId64|PRIu8|PRIu16|PRIu32|PRIu64|THE_EOL|THE_OS_WINDOWS|THE_PATH_SEP|alloc|bool|exit|false|fprintf|fputc|fputs|fread|free|fwrite|int8_t|int16_t|int32_t|int64_t|malloc|memcmp|memcpy|printf|re_alloc|realloc|size_t|snprintf|sprintf|stderr|stdout|str_alloc|str_free|strlen|struct any|struct buffer|struct str|true|uin8_t|uint16_t|uint32_t|uint64_t|va_arg|va_end|va_list|va_start)(?!}|[a-zA-Z0-9_])

extern const std::map<std::string, std::set<std::string>> codegenMetadata = {
  std::pair<std::string, std::set<std::string>>{"THE_EOL", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_OS_LINUX", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_OS_MACOS", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_OS_WINDOWS", {"definitions"}},
  std::pair<std::string, std::set<std::string>>{"THE_PATH_SEP", {"definitions"}},

  std::pair<std::string, std::set<std::string>>{"SystemFunction036", {"externSystemFunction036"}},

  std::pair<std::string, std::set<std::string>>{"struct any", {"typeAny"}},
  std::pair<std::string, std::set<std::string>>{"struct buffer", {"typeBuffer"}},
  std::pair<std::string, std::set<std::string>>{"struct request", {"typeRequest"}},
  std::pair<std::string, std::set<std::string>>{"struct str", {"typeStr"}},
  std::pair<std::string, std::set<std::string>>{"struct win_reparse_data_buffer", {"typeWinReparseDataBuffer"}},

  std::pair<std::string, std::set<std::string>>{"environ", {"varEnviron"}},
  std::pair<std::string, std::set<std::string>>{"lib_openssl_init", {"varLibOpensslInit"}},
  std::pair<std::string, std::set<std::string>>{"lib_ws2_init", {"varLibWs2Init"}},

  std::pair<std::string, std::set<std::string>>{"isalnum", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"isalpha", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"isdigit", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"islower", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"isspace", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"isupper", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"tolower", {"libCtype"}},
  std::pair<std::string, std::set<std::string>>{"toupper", {"libCtype"}},

  std::pair<std::string, std::set<std::string>>{"DIR", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"struct dirent", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"closedir", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"opendir", {"libDirent"}},
  std::pair<std::string, std::set<std::string>>{"readdir", {"libDirent"}},

  std::pair<std::string, std::set<std::string>>{"ERANGE", {"libErrno"}},
  std::pair<std::string, std::set<std::string>>{"errno", {"libErrno"}},

  std::pair<std::string, std::set<std::string>>{"DBL_MAX", {"libFloat"}},
  std::pair<std::string, std::set<std::string>>{"DBL_MIN", {"libFloat"}},
  std::pair<std::string, std::set<std::string>>{"FLT_MAX", {"libFloat"}},
  std::pair<std::string, std::set<std::string>>{"FLT_MIN", {"libFloat"}},

  std::pair<std::string, std::set<std::string>>{"PRId8", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRId16", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRId32", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRId64", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRIu8", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRIu16", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRIu32", {"libInttypes"}},
  std::pair<std::string, std::set<std::string>>{"PRIu64", {"libInttypes"}},

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

  std::pair<std::string, std::set<std::string>>{"getpwuid", {"libPwd"}},

  std::pair<std::string, std::set<std::string>>{"va_arg", {"libStdarg"}},
  std::pair<std::string, std::set<std::string>>{"va_end", {"libStdarg"}},
  std::pair<std::string, std::set<std::string>>{"va_list", {"libStdarg"}},
  std::pair<std::string, std::set<std::string>>{"va_start", {"libStdarg"}},

  std::pair<std::string, std::set<std::string>>{"bool", {"libStdbool"}},
  std::pair<std::string, std::set<std::string>>{"false", {"libStdbool"}},
  std::pair<std::string, std::set<std::string>>{"true", {"libStdbool"}},

  std::pair<std::string, std::set<std::string>>{"size_t", {"libStddef"}},
  std::pair<std::string, std::set<std::string>>{"NULL", {"libStddef"}},

  std::pair<std::string, std::set<std::string>>{"INT8_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT8_MIN", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT16_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT16_MIN", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT32_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT32_MIN", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT64_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"INT64_MIN", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"UINT8_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"UINT16_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"UINT32_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"UINT64_MAX", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int8_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int16_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int32_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"int64_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"uint8_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"uint16_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"uint32_t", {"libStdint"}},
  std::pair<std::string, std::set<std::string>>{"uint64_t", {"libStdint"}},

  std::pair<std::string, std::set<std::string>>{"FILE", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"TMP_MAX", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fclose", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fopen", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fprintf", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fputc", {"libStdio"}},
  std::pair<std::string, std::set<std::string>>{"fputs", {"libStdio"}},
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
  std::pair<std::string, std::set<std::string>>{"getenv", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"malloc", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"mkdtemp", {"libStdlib", "libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"mkstemp", {"libStdlib", "libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"realloc", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"realpath", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtod", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtof", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtol", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtoll", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtoul", {"libStdlib"}},
  std::pair<std::string, std::set<std::string>>{"strtoull", {"libStdlib"}},

  std::pair<std::string, std::set<std::string>>{"memcmp", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"memcpy", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"memmove", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"memset", {"libString"}},
  std::pair<std::string, std::set<std::string>>{"strchr", {"libString"}},
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

  std::pair<std::string, std::set<std::string>>{"struct utsname", {"libSysUtsname"}},
  std::pair<std::string, std::set<std::string>>{"uname", {"libSysUtsname"}},

  std::pair<std::string, std::set<std::string>>{"struct timespec", {"libTime"}},
  std::pair<std::string, std::set<std::string>>{"CLOCK_REALTIME", {"libTime"}},
  std::pair<std::string, std::set<std::string>>{"clock_gettime", {"libTime"}},

  std::pair<std::string, std::set<std::string>>{"access", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"chown", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"close", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"getcwd", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"getgid", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"getuid", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"rmdir", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"symlink", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"unlink", {"libUnistd"}},
  std::pair<std::string, std::set<std::string>>{"usleep", {"libUnistd"}},

  std::pair<std::string, std::set<std::string>>{"_getcwd", {"libWinDirect"}},

  std::pair<std::string, std::set<std::string>>{"_access", {"libWinIo"}},
  std::pair<std::string, std::set<std::string>>{"_chmod", {"libWinIo"}},

  std::pair<std::string, std::set<std::string>>{"CREATE_NEW", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"DWORD", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"ERROR_FILE_NOT_FOUND", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"ERROR_NO_MORE_FILES", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"FILETIME", {"libWindows"}},
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
  std::pair<std::string, std::set<std::string>>{"GetEnvironmentVariable", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetFileAttributes", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetFinalPathNameByHandle", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetLastError", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetSystemTimePreciseAsFileTime", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"GetTempPath", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"RemoveDirectory", {"libWindows"}},
  std::pair<std::string, std::set<std::string>>{"Sleep", {"libWindows"}},

  std::pair<std::string, std::set<std::string>>{"INVALID_SOCKET", {"libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"SOCKET", {"libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"SOCKET_ERROR", {"libWinsock2"}},
  std::pair<std::string, std::set<std::string>>{"closesocket", {"libWinsock2"}}
};

#endif
