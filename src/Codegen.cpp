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

#include "Codegen.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "codegen/any.hpp"
#include "codegen/bool.hpp"
#include "codegen/buffer.hpp"
#include "codegen/byte.hpp"
#include "codegen/char.hpp"
#include "codegen/date.hpp"
#include "codegen/enum.hpp"
#include "codegen/float.hpp"
#include "codegen/fs.hpp"
#include "codegen/globals.hpp"
#include "codegen/int.hpp"
#include "codegen/metadata.hpp"
#include "codegen/os.hpp"
#include "codegen/path.hpp"
#include "codegen/process.hpp"
#include "codegen/random.hpp"
#include "codegen/request.hpp"
#include "codegen/str.hpp"
#include "codegen/thread.hpp"
#include "codegen/url.hpp"
#include "codegen/utils.hpp"
#include "ASTChecker.hpp"
#include "CParser.hpp"
#include "config.hpp"

const auto banner = std::string(
  "/*!" EOL
  " * Copyright (c) 2018 Aaron Delasy" EOL
  " *" EOL
  " * Unauthorized copying of this file, via any medium is strictly prohibited" EOL
  " * Proprietary and confidential" EOL
  " */" EOL
  EOL
);

bool isHoistingFriendlyNode (const ASTNode &node) {
  return std::holds_alternative<ASTNodeEnumDecl>(*node.body) ||
    std::holds_alternative<ASTNodeFnDecl>(*node.body) ||
    std::holds_alternative<ASTNodeObjDecl>(*node.body) ||
    std::holds_alternative<ASTNodeTypeDecl>(*node.body);
}

std::string getCompilerFromPlatform (const std::string &platform) {
  if (platform == "macos") {
    return "o64-clang";
  } else if (platform == "windows") {
    return "x86_64-w64-mingw32-gcc";
  }

  return "clang";
}

std::string getOSFromPlatform (const std::string &platform) {
  if (platform != "default") {
    return platform;
  }

  #if defined(OS_WINDOWS)
    return "windows";
  #elif defined(OS_MACOS)
    return "macos";
  #elif defined(OS_LINUX)
    return "linux";
  #else
    return "unknown";
  #endif
}

void Codegen::compile (
  const std::string &path,
  const std::tuple<std::string, std::vector<std::string>> &result,
  const std::string &platform,
  bool debug
) {
  auto code = std::get<0>(result);
  auto flags = std::get<1>(result);
  auto f = std::ofstream("build/output.c");

  f << code;
  f.close();

  auto depsDir = Codegen::getEnvVar("DEPS_DIR");
  auto compiler = getCompilerFromPlatform(platform);
  auto flagsStr = std::string();
  auto libraries = std::string();

  if (!depsDir.empty()) {
    flagsStr += " -I\"" + depsDir + "/include\"";
    flagsStr += " -L\"" + depsDir + "/lib\"";
  }

  auto targetOS = getOSFromPlatform(platform);

  for (const auto &flag : flags) {
    if (
      flag.starts_with("A:") ||
      (flag.starts_with("L:") && targetOS == "linux") ||
      (flag.starts_with("M:") && targetOS == "macos") ||
      (flag.starts_with("U:") && targetOS != "windows") ||
      (flag.starts_with("W:") && targetOS == "windows")
    ) {
      flagsStr += " " + flag.substr(2);
    }
  }

  auto cmd = compiler + " build/output.c " + libraries + "-w -o " + path + flagsStr + (debug ? " -g" : "");
  auto returnCode = std::system(cmd.c_str());

  std::filesystem::remove("build/output.c");

  if (returnCode != 0) {
    throw Error("failed to compile generated code");
  }
}

std::string Codegen::getEnvVar (const std::string &name) {
  #if defined(OS_WINDOWS)
    auto buf = static_cast<char *>(nullptr);
    auto size = static_cast<std::size_t>(0);

    if (_dupenv_s(&buf, &size, name.c_str()) != 0 || buf == nullptr) {
      return "";
    }

    auto result = std::string(buf);
    free(buf);
    return result;
  #else
    const char *result = std::getenv(name.c_str());
    return result == nullptr ? "" : result;
  #endif
}

std::string Codegen::name (const std::string &name) {
  return "__THE_0_" + name;
}

std::string Codegen::stringifyFlags (const std::vector<std::string> &flags) {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : flags) {
    if (!flag.empty()) {
      result += (idx++ == 0 ? "" : " ") + flag;
    }
  }

  return result;
}

std::string Codegen::typeName (const std::string &name) {
  return "__THE_1_" + name;
}

Codegen::Codegen (AST *a) {
  this->ast = a;
  this->reader = this->ast->reader;
}

std::tuple<std::string, std::vector<std::string>> Codegen::gen () {
  this->_apiDecl(codegenGlobals);
  this->_apiDecl(codegenAny);
  this->_apiDecl(codegenBool);
  this->_apiDecl(codegenBuffer);
  this->_apiDecl(codegenByte);
  this->_apiDecl(codegenChar);
  this->_apiDecl(codegenEnum);
  this->_apiDecl(codegenFloat);
  this->_apiDecl(codegenInt);
  this->_apiDecl(codegenStr);

  this->_typeNameObj(this->ast->typeMap.get("fs_Stats"));
  this->_typeNameObj(this->ast->typeMap.get("request_Header"));
  this->_typeNameObj(this->ast->typeMap.get("request_Request"));
  this->_typeNameObj(this->ast->typeMap.get("request_Response"));
  this->_typeNameObj(this->ast->typeMap.get("url_URL"));

  this->_apiDecl(codegenDate);
  this->_apiDecl(codegenFs);
  this->_apiDecl(codegenOS);
  this->_apiDecl(codegenPath);
  this->_apiDecl(codegenProcess);
  this->_apiDecl(codegenRandom);
  this->_apiDecl(codegenRequest);
  this->_apiDecl(codegenThread);
  this->_apiDecl(codegenURL);
  this->_apiDecl(codegenUtils);

  this->_apiDef(codegenGlobals);
  this->_apiDef(codegenAny);
  this->_apiDef(codegenBool);
  this->_apiDef(codegenBuffer);
  this->_apiDef(codegenByte);
  this->_apiDef(codegenChar);
  this->_apiDef(codegenEnum);
  this->_apiDef(codegenFloat);
  this->_apiDef(codegenInt);
  this->_apiDef(codegenStr);

  this->_typeNameObjDef(this->ast->typeMap.get("fs_Stats"));
  this->_typeNameObjDef(this->ast->typeMap.get("request_Header"));
  this->_typeNameObjDef(this->ast->typeMap.get("request_Request"), {
    std::pair<std::string, std::string>{"free", "  _{request_close}(&n);" EOL}
  });
  this->_typeNameObjDef(this->ast->typeMap.get("request_Response"));
  this->_typeNameObjDef(this->ast->typeMap.get("url_URL"));

  this->_apiDef(codegenDate);
  this->_apiDef(codegenFs);
  this->_apiDef(codegenOS);
  this->_apiDef(codegenPath);
  this->_apiDef(codegenProcess);
  this->_apiDef(codegenRandom);
  this->_apiDef(codegenRequest);
  this->_apiDef(codegenThread);
  this->_apiDef(codegenURL);
  this->_apiDef(codegenUtils);

  auto nodes = this->ast->gen();
  auto mainCode = std::string();

  mainCode += this->_block(nodes, false);
  mainCode += this->state.cleanUp.gen(2);

  auto defineCode = std::string();
  auto enumDeclCode = std::string();
  auto fnDeclCode = std::string();
  auto fnDefCode = std::string();
  auto structDeclCode = std::string();
  auto structDefCode = std::string();

  for (const auto &entity : this->entities) {
    if (!entity.active || entity.decl.empty()) {
      continue;
    }

    if (entity.type == CODEGEN_ENTITY_DEF) {
      defineCode += entity.decl + EOL;
    } else if (entity.type == CODEGEN_ENTITY_ENUM) {
      enumDeclCode += entity.decl + EOL;
    } else if (entity.type == CODEGEN_ENTITY_FN) {
      fnDeclCode += entity.decl + EOL;
      fnDefCode += entity.def + EOL;
    } else if (entity.type == CODEGEN_ENTITY_OBJ) {
      structDeclCode += entity.decl + EOL;
      structDefCode += entity.def + EOL;
    }
  }

  auto builtinDefineCode = std::string();
  auto builtinExternCode = std::string();
  auto builtinWinExternCode = std::string();
  auto builtinFnDeclCode = std::string();
  auto builtinFnDefCode = std::string();
  auto builtinStructDefCode = std::string();
  auto builtinVarCode = std::string();

  if (this->builtins.definitions) {
    builtinDefineCode += "#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)" EOL;
    builtinDefineCode += "  #define THE_OS_WINDOWS" EOL;
    builtinDefineCode += R"(  #define THE_EOL "\r\n")" EOL;
    builtinDefineCode += R"(  #define THE_PATH_SEP "\\")" EOL;
    builtinDefineCode += "#else" EOL;
    builtinDefineCode += "  #if defined(__APPLE__)" EOL;
    builtinDefineCode += "    #define THE_OS_MACOS" EOL;
    builtinDefineCode += "  #elif defined(__linux__)" EOL;
    builtinDefineCode += "    #define THE_OS_LINUX" EOL;
    builtinDefineCode += "  #endif" EOL;
    builtinDefineCode += R"(  #define THE_EOL "\n")" EOL;
    builtinDefineCode += R"(  #define THE_PATH_SEP "/")" EOL;
    builtinDefineCode += "#endif" EOL;
  }

  if (this->builtins.externSystemFunction036) {
    builtinWinExternCode += "  extern unsigned char NTAPI SystemFunction036 (void *, unsigned long);" EOL;
  }

  if (this->builtins.typeAny) {
    builtinStructDefCode += "struct any {" EOL;
    builtinStructDefCode += "  int t;" EOL;
    builtinStructDefCode += "  void *d;" EOL;
    builtinStructDefCode += "  size_t l;" EOL;
    builtinStructDefCode += "  struct any (*_copy) (const struct any);" EOL;
    builtinStructDefCode += "  void (*_free) (struct any);" EOL;
    builtinStructDefCode += "};" EOL;
  }

  if (this->builtins.typeBuffer) {
    builtinStructDefCode += "struct buffer {" EOL;
    builtinStructDefCode += "  unsigned char *d;" EOL;
    builtinStructDefCode += "  size_t l;" EOL;
    builtinStructDefCode += "};" EOL;
  }

  if (this->builtins.typeRequest) {
    builtinStructDefCode += "struct request {" EOL;
    builtinStructDefCode += "  #ifdef THE_OS_WINDOWS" EOL;
    builtinStructDefCode += "    SOCKET fd;" EOL;
    builtinStructDefCode += "  #else" EOL;
    builtinStructDefCode += "    int fd;" EOL;
    builtinStructDefCode += "  #endif" EOL;
    builtinStructDefCode += "  SSL_CTX *ctx;" EOL;
    builtinStructDefCode += "  SSL *ssl;" EOL;
    builtinStructDefCode += "};" EOL;
  }

  if (this->builtins.typeStr) {
    builtinStructDefCode += "struct str {" EOL;
    builtinStructDefCode += "  char *d;" EOL;
    builtinStructDefCode += "  size_t l;" EOL;
    builtinStructDefCode += "};" EOL;
  }

  if (this->builtins.typeWinReparseDataBuffer) {
    builtinStructDefCode += "#ifdef THE_OS_WINDOWS" EOL;
    builtinStructDefCode += "  struct win_reparse_data_buffer {" EOL;
    builtinStructDefCode += "    ULONG ReparseTag;" EOL;
    builtinStructDefCode += "    USHORT ReparseDataLength;" EOL;
    builtinStructDefCode += "    USHORT Reserved;" EOL;
    builtinStructDefCode += "    union {" EOL;
    builtinStructDefCode += "      struct {" EOL;
    builtinStructDefCode += "        USHORT SubstituteNameOffset;" EOL;
    builtinStructDefCode += "        USHORT SubstituteNameLength;" EOL;
    builtinStructDefCode += "        USHORT PrintNameOffset;" EOL;
    builtinStructDefCode += "        USHORT PrintNameLength;" EOL;
    builtinStructDefCode += "        ULONG Flags;" EOL;
    builtinStructDefCode += "        TCHAR PathBuffer[1];" EOL;
    builtinStructDefCode += "      } SymbolicLinkReparseBuffer;" EOL;
    builtinStructDefCode += "      struct {" EOL;
    builtinStructDefCode += "        USHORT SubstituteNameOffset;" EOL;
    builtinStructDefCode += "        USHORT SubstituteNameLength;" EOL;
    builtinStructDefCode += "        USHORT PrintNameOffset;" EOL;
    builtinStructDefCode += "        USHORT PrintNameLength;" EOL;
    builtinStructDefCode += "        TCHAR PathBuffer[1];" EOL;
    builtinStructDefCode += "      } MountPointReparseBuffer;" EOL;
    builtinStructDefCode += "      struct {" EOL;
    builtinStructDefCode += "        UCHAR DataBuffer[1];" EOL;
    builtinStructDefCode += "      } GenericReparseBuffer;" EOL;
    builtinStructDefCode += "      struct {" EOL;
    builtinStructDefCode += "        ULONG Version;" EOL;
    builtinStructDefCode += "        TCHAR StringList[1];" EOL;
    builtinStructDefCode += "      } AppExecLinkReparseBuffer;" EOL;
    builtinStructDefCode += "    };" EOL;
    builtinStructDefCode += "  };" EOL;
    builtinStructDefCode += "#endif" EOL;
  }

  if (this->needMainArgs) {
    builtinVarCode += "int argc = 0;" EOL;
    builtinVarCode += "char **argv = (void *) 0;" EOL;
  }

  if (this->builtins.varEnviron) {
    builtinExternCode += "extern char **environ;" EOL;
  }

  if (this->builtins.varLibOpensslInit) {
    builtinVarCode += "bool lib_openssl_init = false;" EOL;
  }

  if (this->builtins.varLibWs2Init) {
    builtinVarCode += "bool lib_ws2_init = false;" EOL;
  }

  for (const auto &[_, item] : this->api) {
    if (item.enabled) {
      builtinFnDeclCode += item.decl;
      builtinFnDefCode += item.def;
    }
  }

  fnDeclCode = builtinFnDeclCode + fnDeclCode;
  fnDefCode = builtinFnDefCode + fnDefCode;

  builtinDefineCode += builtinDefineCode.empty() ? "" : EOL;
  defineCode += defineCode.empty() ? "" : EOL;
  builtinExternCode += builtinExternCode.empty() ? "" : EOL;
  builtinVarCode += builtinVarCode.empty() ? "" : EOL;
  builtinStructDefCode += builtinStructDefCode.empty() ? "" : EOL;
  enumDeclCode += enumDeclCode.empty() ? "" : EOL;
  structDeclCode += structDeclCode.empty() ? "" : EOL;
  structDefCode += structDefCode.empty() ? "" : EOL;
  fnDeclCode += fnDeclCode.empty() ? "" : EOL;
  fnDefCode += fnDefCode.empty() ? "" : EOL;

  auto headers = std::string(this->builtins.libCtype ? "#include <ctype.h>" EOL : "");
  headers += this->builtins.libErrno ? "#include <errno.h>" EOL : "";
  headers += this->builtins.libFloat ? "#include <float.h>" EOL : "";
  headers += this->builtins.libInttypes ? "#include <inttypes.h>" EOL : "";
  headers += this->builtins.libMath ? "#include <math.h>" EOL : "";
  headers += this->builtins.libOpensslRand ? "#include <openssl/rand.h>" EOL : "";
  headers += this->builtins.libOpensslSsl ? "#include <openssl/ssl.h>" EOL : "";
  headers += this->builtins.libStdarg ? "#include <stdarg.h>" EOL : "";
  headers += this->builtins.libStdbool ? "#include <stdbool.h>" EOL : "";
  headers += this->builtins.libStddef && !this->builtins.libStdlib ? "#include <stddef.h>" EOL : "";
  headers += this->builtins.libStdint && !this->builtins.libInttypes ? "#include <stdint.h>" EOL : "";
  headers += this->builtins.libStdio ? "#include <stdio.h>" EOL : "";
  headers += this->builtins.libStdlib ? "#include <stdlib.h>" EOL : "";
  headers += this->builtins.libString ? "#include <string.h>" EOL : "";

  if (
    this->builtins.libSysTypes ||
    this->builtins.libSysSocket ||
    this->builtins.libSysStat ||
    this->builtins.libSysUtsname
  ) {
    headers += this->builtins.libSysTypes ? "#include <sys/types.h>" EOL : "";
  }

  headers += this->builtins.libSysStat ? "#include <sys/stat.h>" EOL : "";
  headers += this->builtins.libTime ? "#include <time.h>" EOL : "";

  if (
    this->builtins.libWinDirect ||
    this->builtins.libWinIo ||
    this->builtins.libWinWs2tcpip ||
    this->builtins.libWindows ||
    this->builtins.libWinsock2
  ) {
    headers += "#ifdef THE_OS_WINDOWS" EOL;
    headers += this->builtins.libWinDirect ? "  #include <direct.h>" EOL : "";
    headers += this->builtins.libWinIo ? "  #include <io.h>" EOL : "";
    headers += this->builtins.libWinsock2 ? "  #include <winsock2.h>" EOL : "";
    headers += this->builtins.libWinWs2tcpip ? "  #include <ws2tcpip.h>" EOL : "";
    headers += this->builtins.libWindows ? "  #include <windows.h>" EOL : "";
    headers += "#endif" EOL;
  }

  if (
    this->builtins.libArpaInet ||
    this->builtins.libFcntl ||
    this->builtins.libDirent ||
    this->builtins.libNetdb ||
    this->builtins.libNetinetIn ||
    this->builtins.libPwd ||
    this->builtins.libSysSocket ||
    this->builtins.libSysUtsname ||
    this->builtins.libUnistd
  ) {
    headers += "#ifndef THE_OS_WINDOWS" EOL;
    headers += this->builtins.libArpaInet ? "  #include <arpa/inet.h>" EOL : "";
    headers += this->builtins.libFcntl ? "  #include <fcntl.h>" EOL : "";
    headers += this->builtins.libDirent ? "  #include <dirent.h>" EOL : "";
    headers += this->builtins.libNetdb ? "  #include <netdb.h>" EOL : "";
    headers += this->builtins.libNetinetIn ? "  #include <netinet/in.h>" EOL : "";
    headers += this->builtins.libPwd ? "  #include <pwd.h>" EOL : "";
    headers += this->builtins.libSysSocket ? "  #include <sys/socket.h>" EOL : "";
    headers += this->builtins.libSysUtsname ? "  #include <sys/utsname.h>" EOL : "";
    headers += this->builtins.libUnistd ? "  #include <unistd.h>" EOL : "";
    headers += "#endif" EOL;
  }

  headers += headers.empty() ? "" : EOL;

  auto output = std::string();
  output += banner;
  output += builtinDefineCode;
  output += headers;
  output += builtinExternCode;

  if (!builtinWinExternCode.empty()) {
    output += "#ifdef THE_OS_WINDOWS" EOL;
    output += builtinWinExternCode;
    output += "#endif" EOL EOL;
  }

  output += defineCode;
  output += enumDeclCode;
  output += builtinVarCode;
  output += builtinStructDefCode;
  output += structDeclCode;
  output += structDefCode;
  output += fnDeclCode;
  output += fnDefCode;

  if (this->needMainArgs) {
    output += "int main (int _argc_, char *_argv_[]) {" EOL;
    output += "  argc = _argc_;" EOL;
    output += "  argv = _argv_;" EOL;
    output += mainCode;
    output += "}" EOL;
  } else {
    output += "int main () {" EOL;
    output += mainCode;
    output += "}" EOL;
  }

  return std::make_tuple(output, this->flags);
}

void Codegen::_activateBuiltin (const std::string &name, std::optional<std::vector<std::string> *> entityBuiltins) {
  if (entityBuiltins != std::nullopt || this->state.builtins != std::nullopt) {
    auto b = entityBuiltins == std::nullopt ? *this->state.builtins : *entityBuiltins;

    if (std::find(b->begin(), b->end(), name) == b->end()) {
      b->push_back(name);
    }

    return;
  }

  if (name == "definitions") {
    this->builtins.definitions = true;
  } else if (name == "externSystemFunction036") {
    this->builtins.externSystemFunction036 = true;
    this->_activateBuiltin("definitions");

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }
  } else if (name == "libArpaInet") {
    this->builtins.libArpaInet = true;
  } else if (name == "libCtype") {
    this->builtins.libCtype = true;
  } else if (name == "libDirent") {
    this->builtins.libDirent = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libErrno") {
    this->builtins.libErrno = true;
  } else if (name == "libFcntl") {
    this->builtins.libFcntl = true;
  } else if (name == "libFloat") {
    this->builtins.libFloat = true;
  } else if (name == "libInttypes") {
    this->builtins.libInttypes = true;
  } else if (name == "libMath") {
    this->builtins.libMath = true;

    if (std::find(this->flags.begin(), this->flags.end(), "U:-lm") == this->flags.end()) {
      this->flags.emplace_back("U:-lm");
    }
  } else if (name == "libNetdb") {
    this->builtins.libNetdb = true;
  } else if (name == "libNetinetIn") {
    this->builtins.libNetinetIn = true;
  } else if (name == "libOpensslRand") {
    this->builtins.libOpensslRand = true;

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lssl") == this->flags.end()) {
      this->flags.emplace_back("A:-lssl");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lcrypto") == this->flags.end()) {
      this->flags.emplace_back("A:-lcrypto");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lws2_32") == this->flags.end()) {
      this->flags.emplace_back("W:-lws2_32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lgdi32") == this->flags.end()) {
      this->flags.emplace_back("W:-lgdi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lcrypt32") == this->flags.end()) {
      this->flags.emplace_back("W:-lcrypt32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-luser32") == this->flags.end()) {
      this->flags.emplace_back("W:-luser32");
    }
  } else if (name == "libOpensslSsl") {
    this->builtins.libOpensslSsl = true;

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lssl") == this->flags.end()) {
      this->flags.emplace_back("A:-lssl");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lcrypto") == this->flags.end()) {
      this->flags.emplace_back("A:-lcrypto");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lws2_32") == this->flags.end()) {
      this->flags.emplace_back("W:-lws2_32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lgdi32") == this->flags.end()) {
      this->flags.emplace_back("W:-lgdi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lcrypt32") == this->flags.end()) {
      this->flags.emplace_back("W:-lcrypt32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-luser32") == this->flags.end()) {
      this->flags.emplace_back("W:-luser32");
    }
  } else if (name == "libPwd") {
    this->builtins.libPwd = true;
  } else if (name == "libStdarg") {
    this->builtins.libStdarg = true;
  } else if (name == "libStdbool") {
    this->builtins.libStdbool = true;
  } else if (name == "libStddef") {
    this->builtins.libStddef = true;
  } else if (name == "libStdint") {
    this->builtins.libStdint = true;
  } else if (name == "libStdio") {
    this->builtins.libStdio = true;
  } else if (name == "libStdlib") {
    this->builtins.libStdlib = true;
  } else if (name == "libString") {
    this->builtins.libString = true;
  } else if (name == "libSysSocket") {
    this->builtins.libSysSocket = true;
  } else if (name == "libSysStat") {
    this->builtins.libSysStat = true;
  } else if (name == "libSysTypes") {
    this->builtins.libSysTypes = true;
  } else if (name == "libSysUtsname") {
    this->builtins.libSysUtsname = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libTime") {
    this->builtins.libTime = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libUnistd") {
    this->builtins.libUnistd = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinDirect") {
    this->builtins.libWinDirect = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinIo") {
    this->builtins.libWinIo = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinWs2tcpip") {
    this->builtins.libWinWs2tcpip = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWindows") {
    this->builtins.libWindows = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinsock2") {
    this->builtins.libWinsock2 = true;
    this->_activateBuiltin("definitions");
  } else if (name == "typeAny") {
    this->builtins.typeAny = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeBuffer") {
    this->builtins.typeBuffer = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeRequest") {
    this->builtins.typeRequest = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libOpensslSsl");
    this->_activateBuiltin("libWinsock2");
  } else if (name == "typeStr") {
    this->builtins.typeStr = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeWinReparseDataBuffer") {
    this->builtins.typeWinReparseDataBuffer = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libWindows");
  } else if (name == "varEnviron") {
    this->builtins.varEnviron = true;
    this->_activateBuiltin("libUnistd");
  } else if (name == "varLibOpensslInit") {
    this->builtins.varLibOpensslInit = true;
    this->_activateBuiltin("libStdbool");
  } else if (name == "varLibWs2Init") {
    this->builtins.varLibWs2Init = true;
    this->_activateBuiltin("libStdbool");
  } else if (this->api.contains(name)) {
    this->api[name].enabled = true;

    for (const auto &dep : this->api[name].dependencies) {
      auto existsInEntities = false;

      for (const auto &entity : this->entities) {
        if (entity.name == dep) {
          existsInEntities = true;
          break;
        }
      }

      if (existsInEntities) {
        this->_activateEntity(dep);
      } else {
        this->_activateBuiltin(dep);
      }
    }
  } else {
    throw Error("tried activating unknown builtin `" + name + "`");
  }
}

void Codegen::_activateEntity (const std::string &name, std::optional<std::vector<std::string> *> entityEntities) {
  if (entityEntities != std::nullopt || this->state.entities != std::nullopt) {
    auto e = entityEntities == std::nullopt ? *this->state.entities : *entityEntities;

    if (std::find(e->begin(), e->end(), name) == e->end()) {
      e->push_back(name);
    }

    return;
  }

  for (auto &entity : this->entities) {
    if (entity.name != name) {
      continue;
    }

    if (!entity.active) {
      entity.active = true;

      for (const auto &builtin : entity.builtins) {
        this->_activateBuiltin(builtin);
      }

      for (const auto &child : entity.entities) {
        this->_activateEntity(child);
      }
    }

    return;
  }

  throw Error("tried activating unknown entity `" + name + "`");
}

int Codegen::_apiEntity (
  const std::string &name,
  CodegenEntityType type,
  const std::optional<std::function<int (std::string &, std::string &)>> &fn
) {
  auto initialStateBuiltins = this->state.builtins;
  auto initialStateEntities = this->state.entities;

  auto entity = CodegenEntity{name, type};
  this->state.builtins = &entity.builtins;
  this->state.entities = &entity.entities;
  auto newPos = fn == std::nullopt ? 0 : (*fn)(entity.decl, entity.def);
  entity.decl = entity.decl.empty() ? entity.decl : this->_apiEval(entity.decl);
  entity.def = entity.def.empty() ? entity.def : this->_apiEval(entity.def);

  this->state.builtins = initialStateBuiltins;
  this->state.entities = initialStateEntities;

  if (newPos == 0) {
    this->entities.push_back(entity);
    return static_cast<int>(this->entities.size()) - 1;
  } else {
    this->entities.insert(this->entities.begin() + newPos, entity);
    return newPos;
  }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string Codegen::_apiEval (
  const std::string &code,
  int limit,
  const std::string &selfName,
  const std::optional<std::set<std::string> *> &dependencies
) {
  auto name = std::string();
  auto isName = false;
  auto result = std::string();
  result.reserve(code.size());

  for (auto i = static_cast<std::size_t>(0); i < code.size(); i++) {
    auto ch = code[i];

    if (limit != -1 && ch == '_' && i + 1 < code.size() && code[i + 1] == '{') {
      isName = true;
      i += 1;
    } else if (isName && ch == '}') {
      auto sameAsSelfName = selfName == name;

      if (!sameAsSelfName && codegenMetadata.contains(name)) {
        for (const auto &dependency : codegenMetadata.at(name)) {
          if (dependencies == std::nullopt) {
            this->_activateBuiltin(dependency);
          } else if (!(*dependencies)->contains(dependency)) {
            (*dependencies)->emplace(dependency);
          }
        }
      } else if (!sameAsSelfName && this->api.contains(name)) {
        if (dependencies == std::nullopt) {
          this->_activateBuiltin(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName && name.starts_with("array_")) {
        if (name.ends_with("_free")) {
          name = this->_typeNameArray(this->ast->typeMap.createArr(this->ast->typeMap.get(name.substr(6, name.size() - 11)))) + "_free";
        } else {
          name = this->_typeNameArray(this->ast->typeMap.createArr(this->ast->typeMap.get(name.substr(6))));
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName && (name.starts_with("map_") || name.starts_with("pair_"))) {
        auto isPair = name.starts_with("pair_");
        auto nameSliced = isPair ? name.substr(5) : name.substr(4);
        auto keyType = this->ast->typeMap.get(nameSliced.substr(0, nameSliced.find('_')));
        auto valueType = this->ast->typeMap.get(nameSliced.substr(nameSliced.find('_') + 1));

        name = this->_typeNameMap(this->ast->typeMap.createMap(keyType, valueType));

        if (isPair) {
          name = Codegen::typeName("pair_" + name.substr(Codegen::typeName("map_").size()));
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName) {
        auto existsInEntities = false;

        for (const auto &entity : this->entities) {
          if (entity.name == name) {
            existsInEntities = true;
            break;
          }
        }

        if (!existsInEntities) {
          throw Error("can't find code generator entity `" + name + "`");
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      }

      result += name;
      isName = false;
      limit = limit == 1 ? -1 : limit == 0 ? 0 : limit - 1;
      name.clear();
    } else if (isName) {
      name += ch;
    } else {
      result += ch;
    }
  }

  return result;
}

void Codegen::_apiDecl (const std::vector<std::string> &items) {
  for (const auto &item : items) {
    auto cParser = cParse(item);
    auto apiItem = CodegenApiItem{.name = cParser.name, .decl = cParser.decl()};

    apiItem.decl = this->_apiEval(apiItem.decl, 0, apiItem.name, &apiItem.dependencies) + ";" EOL;
    this->api[apiItem.name] = apiItem;
  }
}

void Codegen::_apiDef (const std::vector<std::string> &items) {
  for (const auto &item : items) {
    auto cParser = cParse(item);
    this->api[cParser.name].def = this->_apiEval(item, 0, cParser.name, &this->api[cParser.name].dependencies);
  }
}

std::string Codegen::_block (const ASTBlock &nodes, bool saveCleanUp) {
  auto initialIndent = this->indent;
  auto initialCleanUp = this->state.cleanUp;
  auto code = std::string();

  if (saveCleanUp) {
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialCleanUp);
  }

  this->indent += 2;

  for (auto i = static_cast<std::size_t>(0); i < nodes.size(); i++) {
    auto node = nodes[i];

    if (i < nodes.size() - 1 && isHoistingFriendlyNode(node) && isHoistingFriendlyNode(nodes[i + 1])) {
      for (auto j = i; j < nodes.size() && isHoistingFriendlyNode(nodes[j]); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC);
      }

      for (auto j = i; j < nodes.size() && isHoistingFriendlyNode(nodes[j]); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC_METHOD);
      }

      for (; i < nodes.size() && isHoistingFriendlyNode(nodes[i]); i++) {
        code += this->_node(nodes[i], true, CODEGEN_PHASE_INIT);
      }

      i--;
    } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
      auto saveIndent = this->indent;

      this->indent = 0;
      code += this->_node(node);
      this->indent = saveIndent;
    } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
      code += this->_node(node, true, CODEGEN_PHASE_ALLOC);
      code += this->_node(node, true, CODEGEN_PHASE_ALLOC_METHOD);
      code += this->_node(node, true, CODEGEN_PHASE_INIT);
    } else {
      code += this->_node(node);
    }
  }

  if (saveCleanUp) {
    code += this->state.cleanUp.gen(this->indent);

    if (
      (!ASTChecker(nodes).endsWith<ASTNodeBreak>() || ASTChecker(nodes[0].parent).is<ASTNodeLoop>()) &&
      (!ASTChecker(nodes).endsWith<ASTNodeContinue>() || ASTChecker(nodes[0].parent).is<ASTNodeLoop>()) &&
      (!ASTChecker(nodes).endsWith<ASTNodeReturn>() || ASTChecker(nodes[0].parent).is<ASTNodeLoop>()) &&
      this->state.cleanUp.breakVarUsed
    ) {
      code += std::string(this->indent, ' ') + "if (" + initialCleanUp.currentBreakVar() + " == 1) break;" EOL;
    }

    if (!this->state.cleanUp.empty() && this->state.cleanUp.returnVarUsed) {
      code += std::string(this->indent, ' ') + "if (r == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
    }

    this->state.cleanUp = initialCleanUp;
  }

  this->indent = initialIndent;
  return code;
}

std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> Codegen::_evalTypeCasts (const ASTNodeExpr &nodeExpr) {
  auto initialTypeCasts = this->state.typeCasts;
  auto bodyTypeCasts = std::map<std::string, Type *>{};
  auto altTypeCasts = std::map<std::string, Type *>{};

  if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (exprBinary.op == AST_EXPR_BINARY_AND || exprBinary.op == AST_EXPR_BINARY_OR) {
      auto [leftBodyTypeCasts, leftAltTypeCasts] = this->_evalTypeCasts(exprBinary.left);

      if (exprBinary.op == AST_EXPR_BINARY_AND) {
        auto copyLeftBodyTypeCasts = leftBodyTypeCasts;
        copyLeftBodyTypeCasts.merge(this->state.typeCasts);
        copyLeftBodyTypeCasts.swap(this->state.typeCasts);
      } else {
        auto copyLeftAltTypeCasts = leftAltTypeCasts;
        copyLeftAltTypeCasts.merge(this->state.typeCasts);
        copyLeftAltTypeCasts.swap(this->state.typeCasts);
      }

      auto [rightBodyTypeCasts, rightAltTypeCasts] = this->_evalTypeCasts(exprBinary.right);

      if (exprBinary.op == AST_EXPR_BINARY_AND) {
        bodyTypeCasts.merge(rightBodyTypeCasts);
        bodyTypeCasts.merge(leftBodyTypeCasts);
        altTypeCasts.merge(rightAltTypeCasts);
        altTypeCasts.merge(leftAltTypeCasts);
      } else {
        for (const auto &[name, value] : rightBodyTypeCasts) {
          if (leftBodyTypeCasts.contains(name)) {
            bodyTypeCasts[name] = value;
          }
        }

        for (const auto &[name, value] : rightAltTypeCasts) {
          if (leftAltTypeCasts.contains(name)) {
            altTypeCasts[name] = value;
          }
        }
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) {
      if (
        (std::holds_alternative<ASTExprAccess>(*exprBinary.left.body) && std::holds_alternative<ASTExprLit>(*exprBinary.right.body)) ||
        (std::holds_alternative<ASTExprAccess>(*exprBinary.right.body) && std::holds_alternative<ASTExprLit>(*exprBinary.left.body))
      ) {
        auto exprBinaryLeft = std::holds_alternative<ASTExprAccess>(*exprBinary.left.body) ? exprBinary.left : exprBinary.right;
        auto exprBinaryRight = std::holds_alternative<ASTExprLit>(*exprBinary.right.body) ? exprBinary.right : exprBinary.left;
        auto exprBinaryRightLit = std::get<ASTExprLit>(*exprBinaryRight.body);

        if (exprBinaryLeft.type->isOpt() && exprBinaryRightLit.type == AST_EXPR_LIT_NIL) {
          auto exprBinaryLeftAccessCode = this->_nodeExpr(exprBinaryLeft, exprBinaryLeft.type, true);
          auto exprBinaryLeftAccessTypeOpt = std::get<TypeOptional>(exprBinaryLeft.type->body);

          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            altTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          } else {
            bodyTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          }
        }
      } else if (
        (std::holds_alternative<ASTExprAssign>(*exprBinary.left.body) && std::holds_alternative<ASTExprLit>(*exprBinary.right.body)) ||
        (std::holds_alternative<ASTExprAssign>(*exprBinary.right.body) && std::holds_alternative<ASTExprLit>(*exprBinary.left.body))
      ) {
        auto exprBinaryLeft = std::holds_alternative<ASTExprAssign>(*exprBinary.left.body) ? exprBinary.left : exprBinary.right;
        auto exprBinaryLeftAssign = std::get<ASTExprAssign>(*exprBinaryLeft.body);
        auto exprBinaryRight = std::holds_alternative<ASTExprLit>(*exprBinary.right.body) ? exprBinary.right : exprBinary.left;
        auto exprBinaryRightLit = std::get<ASTExprLit>(*exprBinaryRight.body);

        if (exprBinaryLeft.type->isOpt() && exprBinaryRightLit.type == AST_EXPR_LIT_NIL) {
          auto exprBinaryLeftAccessCode = this->_nodeExpr(exprBinaryLeftAssign.left, exprBinaryLeftAssign.left.type, true);
          auto exprBinaryLeftAccessTypeOpt = std::get<TypeOptional>(exprBinaryLeftAssign.left.type->body);

          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            altTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          } else {
            bodyTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          }
        }
      }
    }
  } else if (std::holds_alternative<ASTExprIs>(*nodeExpr.body)) {
    auto exprIs = std::get<ASTExprIs>(*nodeExpr.body);

    if (std::holds_alternative<ASTExprAccess>(*exprIs.expr.body)) {
      auto exprAccess = exprIs.expr;
      auto exprRealType = Type::real(exprAccess.type);

      if (exprRealType->isAny() || exprRealType->isOpt() || exprRealType->isUnion()) {
        auto exprBinaryLeftAccessCode = this->_nodeExpr(exprAccess, exprRealType, true);
        bodyTypeCasts[exprBinaryLeftAccessCode] = exprIs.type;

        if (exprRealType->isUnion()) {
          altTypeCasts[exprBinaryLeftAccessCode] = this->ast->typeMap.unionSub(exprRealType, exprIs.type);
        }
      }
    } else if (
      std::holds_alternative<ASTExprAssign>(*exprIs.expr.body) &&
      std::holds_alternative<ASTExprAccess>(*std::get<ASTExprAssign>(*exprIs.expr.body).left.body)
    ) {
      auto exprAccess = std::get<ASTExprAssign>(*exprIs.expr.body).left;
      auto exprRealType = Type::real(exprAccess.type);

      if (exprRealType->isAny() || exprRealType->isOpt() || exprRealType->isUnion()) {
        auto exprBinaryLeftAccessCode = this->_nodeExpr(exprAccess, exprRealType, true);
        bodyTypeCasts[exprBinaryLeftAccessCode] = exprIs.type;

        if (exprRealType->isUnion()) {
          altTypeCasts[exprBinaryLeftAccessCode] = this->ast->typeMap.unionSub(exprRealType, exprIs.type);
        }
      }
    }
  }

  this->state.typeCasts = initialTypeCasts;
  return std::make_tuple(bodyTypeCasts, altTypeCasts);
}

std::string Codegen::_exprCallDefaultArg (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return this->_apiEval("(_{struct any}) {}");
  } else if (
    typeInfo.type->isArray() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isUnion()
  ) {
    this->_activateEntity(typeInfo.typeName);
    return "(struct " + typeInfo.typeName + ") {}";
  } else if (typeInfo.type->isBool()) {
    return this->_apiEval("_{false}");
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isObj() && typeInfo.type->builtin && typeInfo.type->codeName == "@buffer_Buffer") {
    return this->_apiEval("(struct buffer) {}");
  } else if (typeInfo.type->isObj() || typeInfo.type->isOpt()) {
    return this->_apiEval("_{NULL}");
  } else if (typeInfo.type->isStr()) {
    return this->_apiEval("(_{struct str}) {}");
  } else {
    return "0";
  }
}

std::string Codegen::_exprCallPrintArg (const CodegenTypeInfo &typeInfo, const ASTNodeExpr &nodeExpr) {
  if (typeInfo.type->isStr() && nodeExpr.isLit()) {
    return nodeExpr.litBody();
  } else if (
    typeInfo.type->isAny() ||
    typeInfo.type->isArray() ||
    typeInfo.type->isEnum() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isObj() ||
    typeInfo.type->isOpt() ||
    typeInfo.type->isStr() ||
    typeInfo.type->isUnion()
  ) {
    return this->_genStrFn(typeInfo.type, this->_nodeExpr(nodeExpr, typeInfo.type), false, false);
  }

  return this->_nodeExpr(nodeExpr, typeInfo.type);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string Codegen::_exprCallPrintArgSign (const CodegenTypeInfo &typeInfo, const ASTNodeExpr &nodeExpr) {
  if (
    typeInfo.type->isAny() ||
    typeInfo.type->isArray() ||
    typeInfo.type->isEnum() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isObj() ||
    typeInfo.type->isOpt() ||
    typeInfo.type->isUnion()
  ) {
    return "s";
  } else if (typeInfo.type->isRef()) {
    return "p";
  } else if (typeInfo.type->isStr() && nodeExpr.isLit()) {
    return "z";
  } else {
    if (typeInfo.type->isBool()) return "t";
    else if (typeInfo.type->isByte()) return "b";
    else if (typeInfo.type->isChar()) return "c";
    else if (typeInfo.type->isF32()) return "e";
    else if (typeInfo.type->isF64()) return "g";
    else if (typeInfo.type->isFloat()) return "f";
    else if (typeInfo.type->isI8()) return "h";
    else if (typeInfo.type->isI16()) return "j";
    else if (typeInfo.type->isI32()) return "k";
    else if (typeInfo.type->isI64()) return "l";
    else if (typeInfo.type->isInt()) return "i";
    else if (typeInfo.type->isStr()) return "s";
    else if (typeInfo.type->isU8()) return "v";
    else if (typeInfo.type->isU16()) return "w";
    else if (typeInfo.type->isU32()) return "u";
    else if (typeInfo.type->isU64()) return "y";
  }

  throw Error("tried print unknown entity type");
}

std::string Codegen::_exprObjDefaultField (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return this->_apiEval("(_{struct any}) {0, _{NULL}, 0, _{NULL}, _{NULL}}");
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    this->_activateEntity(typeInfo.typeName + "_alloc");
    return typeInfo.typeName + "_alloc(0)";
  } else if (typeInfo.type->isBool()) {
    return this->_apiEval("_{false}");
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried object expression default field on invalid type");
  } else if (typeInfo.type->isOpt()) {
    return this->_apiEval("_{NULL}");
  } else if (typeInfo.type->isObj()) {
    auto fieldsCode = std::string();

    for (const auto &typeField : typeInfo.type->fields) {
      if (typeField.builtin || typeField.type->isMethod()) {
        continue;
      }

      auto fieldTypeInfo = this->_typeInfo(typeField.type);
      fieldsCode += ", " + this->_exprObjDefaultField(fieldTypeInfo);
    }

    this->_activateEntity(typeInfo.typeName + "_alloc");
    return typeInfo.typeName + "_alloc(" + (fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2)) + ")";
  } else if (typeInfo.type->isStr()) {
    return this->_apiEval(R"(_{str_alloc}(""))", 1);
  } else {
    return "0";
  }
}

std::string Codegen::_fnDecl (
  Type *type,
  const std::string &codeName,
  const std::vector<std::shared_ptr<Var>> &stack,
  const std::vector<ASTFnDeclParam> &params,
  const std::optional<ASTBlock> &body,
  CodegenPhase phase
) {
  if (body == std::nullopt) {
    return "";
  }

  auto typeName = Codegen::typeName(codeName);
  auto varTypeInfo = this->_typeInfo(type);
  auto fnType = std::get<TypeFn>(type->body);
  auto paramsName = varTypeInfo.typeName + "P";
  auto contextName = typeName + "X";
  auto code = std::string();

  if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
    auto initialIndent = this->indent;
    auto initialStateCleanUp = this->state.cleanUp;
    auto initialStateContextVars = this->state.contextVars;
    auto contextEntityIdx = 0;

    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_FN, &initialStateCleanUp);

    if (!stack.empty()) {
      contextEntityIdx = this->_apiEntity(contextName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
        decl += "struct " + contextName + ";";
        def += "struct " + contextName + " {" EOL;

        for (const auto &contextVar : stack) {
          auto contextVarName = Codegen::name(contextVar->codeName);
          auto contextVarTypeInfo = this->_typeInfo(contextVar->type);
          auto typeRefCode = (contextVar->mut ? contextVarTypeInfo.typeRefCode : contextVarTypeInfo.typeRefCodeConst);

          def += "  " + typeRefCode + contextVarName + ";" EOL;
        }

        def += "};";
        return 0;
      });
    }

    this->_apiEntity(typeName, CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
      this->varMap.save();
      this->indent = 2;

      auto bodyCode = std::string();

      if (!stack.empty()) {
        bodyCode += "  struct _{" + contextName + "} *x = px;" EOL;

        for (const auto &contextVar : stack) {
          auto contextVarName = Codegen::name(contextVar->codeName);
          auto contextVarTypeInfo = this->_typeInfo(contextVar->type);
          auto typeRefCode = (contextVar->mut ? contextVarTypeInfo.typeRefCode : contextVarTypeInfo.typeRefCodeConst);

          bodyCode += "  " + typeRefCode + contextVarName + " = x->" + contextVarName + ";" EOL;
          this->state.contextVars.insert(contextVarName);
        }
      }

      if (!params.empty()) {
        auto paramIdx = static_cast<std::size_t>(0);

        for (const auto &param : params) {
          auto paramName = Codegen::name(param.var->codeName);
          auto paramTypeInfo = this->_typeInfo(param.var->type);
          auto paramIdxStr = std::to_string(paramIdx);

          bodyCode += "  " + (param.var->mut ? paramTypeInfo.typeCode : paramTypeInfo.typeCodeConst) + paramName + " = ";

          if (param.init == std::nullopt) {
            bodyCode += "p.n" + paramIdxStr;
          } else {
            auto initCode = this->_nodeExpr(*param.init, paramTypeInfo.type);
            bodyCode += "p.o" + paramIdxStr + " == 1 ? p.n" + paramIdxStr + " : " + initCode;
          }

          bodyCode += ";" EOL;

          if (paramTypeInfo.type->shouldBeFreed()) {
            this->state.cleanUp.add(this->_genFreeFn(paramTypeInfo.type, paramName) + ";");
          }

          paramIdx++;
        }
      }

      if (fnType.isMethod && fnType.callInfo.isSelfFirst && fnType.callInfo.selfType->shouldBeFreed()) {
        this->state.cleanUp.add(this->_genFreeFn(fnType.callInfo.selfType, Codegen::name(fnType.callInfo.selfCodeName)) + ";");
      }

      auto returnTypeInfo = this->_typeInfo(fnType.returnType);

      this->indent = 0;
      this->state.returnType = returnTypeInfo.type;
      bodyCode += this->_block(*body, false);
      this->indent = 2;
      this->varMap.restore();

      if (!returnTypeInfo.type->isVoid() && this->state.cleanUp.valueVarUsed) {
        bodyCode.insert(0, std::string(this->indent, ' ') + returnTypeInfo.typeCode + "v;" EOL);
        bodyCode += this->state.cleanUp.gen(this->indent);
        bodyCode += std::string(this->indent, ' ') + "return v;" EOL;
      } else {
        bodyCode += this->state.cleanUp.gen(this->indent);
      }

      if (this->state.cleanUp.returnVarUsed) {
        bodyCode.insert(0, std::string(this->indent, ' ') + "unsigned char " + this->state.cleanUp.currentReturnVar() + " = 0;" EOL);
      }

      decl += returnTypeInfo.typeCode + typeName + " (void *";
      def += returnTypeInfo.typeCode + typeName + " (void *px";

      if (fnType.isMethod && fnType.callInfo.isSelfFirst) {
        auto selfTypeInfo = this->_typeInfo(fnType.callInfo.selfType);

        decl += ", " + (fnType.callInfo.isSelfMut ? selfTypeInfo.typeCodeTrimmed : selfTypeInfo.typeCodeConstTrimmed);
        def += ", " + (fnType.callInfo.isSelfMut ? selfTypeInfo.typeCode : selfTypeInfo.typeCodeConst) +
          Codegen::name(fnType.callInfo.selfCodeName);
      }

      if (!params.empty()) {
        decl += ", struct _{" + paramsName + "}";
        def += ", struct _{" + paramsName + "} p";
      }

      decl += ");";
      def += ") {" EOL + bodyCode + "}";

      return stack.empty() ? 0 : contextEntityIdx;
    });

    if (!stack.empty()) {
      this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
        decl += "void " + typeName + "_alloc (" + varTypeInfo.typeRefCode + ", struct _{" + contextName + "});";
        def += "void " + typeName + "_alloc (" + varTypeInfo.typeRefCode + "n, struct _{" + contextName + "} x) {" EOL;
        def += "  _{size_t} l = sizeof(struct _{" + contextName + "});" EOL;
        def += "  struct _{" + contextName + "} *r = _{alloc}(l);" EOL;
        def += "  _{memcpy}(r, &x, l);" EOL;
        def += "  n->f = &_{" + typeName + "};" EOL;
        def += "  n->x = r;" EOL;
        def += "  n->l = l;" EOL;
        def += "}";

        return 0;
      });
    }

    this->indent = initialIndent;
    this->state.cleanUp = initialStateCleanUp;
    this->state.contextVars = initialStateContextVars;
  }

  auto fnName = Codegen::name(codeName);

  if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
    this->_activateEntity(varTypeInfo.typeName);
    code += std::string(this->indent, ' ') + "const " + varTypeInfo.typeCode + fnName;
  }

  if ((phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) && stack.empty()) {
    code += this->_apiEval(" = (" + varTypeInfo.typeCodeTrimmed + ") {&_{" + typeName + "}, _{NULL}, 0};" EOL);
  } else if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
    code += ";" EOL;
  }

  if ((phase == CODEGEN_PHASE_INIT || phase == CODEGEN_PHASE_FULL) && !stack.empty()) {
    this->_activateEntity(typeName + "_alloc");
    code += std::string(this->indent, ' ') + typeName + "_alloc((" + varTypeInfo.typeRefCode + ") &" + fnName + ", ";

    this->_activateEntity(contextName);
    code += "(struct " + contextName + ") {";

    auto contextVarIdx = static_cast<std::size_t>(0);

    for (const auto &contextVar : stack) {
      auto contextVarName = Codegen::name(contextVar->codeName);

      code += contextVarIdx == 0 ? "" : ", ";
      code += (this->state.contextVars.contains(contextVarName) ? "" : "&") + contextVarName;

      contextVarIdx++;
    }

    code += "});" EOL;

    if (varTypeInfo.type->shouldBeFreed()) {
      this->state.cleanUp.add(this->_genFreeFn(varTypeInfo.type, fnName) + ";");
    }
  }

  return code;
}

std::string Codegen::_genCopyFn (Type *type, const std::string &code) {
  auto initialState = this->state;
  auto result = code;

  if (type->isAlias()) {
    result = this->_genCopyFn(std::get<TypeAlias>(type->body).type, code);
  } else if (type->isAny()) {
    result = this->_apiEval("_{any_copy}(" + result + ")", 1);
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = this->_apiEval("_{buffer_copy}(" + result + ")", 1);
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.realTypeName + "_copy");
    result = typeInfo.realTypeName + "_copy(" + result + ")";
  } else if (type->isStr()) {
    result = this->_apiEval("_{str_copy}(" + result + ")", 1);
  }

  return result;
}

std::string Codegen::_genEqFn (Type *type, const std::string &leftCode, const std::string &rightCode, bool reverse) {
  auto initialState = this->state;
  auto realType = Type::real(type);
  auto direction = std::string(reverse ? "ne" : "eq");
  auto code = std::string();

  if (realType->isObj() && realType->builtin && realType->codeName == "@buffer_Buffer") {
    code = this->_apiEval("_{buffer_" + direction + "}(" + this->_genCopyFn(realType, leftCode) + ", " + this->_genCopyFn(realType, rightCode) + ")", 1);
  } else if (
    realType->isArray() ||
    realType->isMap() ||
    realType->isObj() ||
    realType->isOpt() ||
    realType->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(realType);

    this->_activateEntity(typeInfo.realTypeName + "_" + direction);
    code = typeInfo.realTypeName + "_" + direction + "(";
    code += this->_genCopyFn(typeInfo.realType, leftCode) + ", ";
    code += this->_genCopyFn(typeInfo.realType, rightCode) + ")";
  } else if (realType->isStr()) {
    code = this->_apiEval("_{str_" + direction + "_str}(" + this->_genCopyFn(realType, leftCode) + ", " + this->_genCopyFn(realType, rightCode) + ")", 1);
  } else {
    code = leftCode + " " + (reverse ? "!=" : "==") + " " + rightCode;
  }

  return code;
}

std::string Codegen::_genFreeFn (Type *type, const std::string &code) {
  auto initialState = this->state;
  auto result = code;

  if (type->isAlias()) {
    result = this->_genFreeFn(std::get<TypeAlias>(type->body).type, code);
  } else if (type->isAny()) {
    result = this->_apiEval("_{any_free}((_{struct any}) " + result + ")", 2);
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = this->_apiEval("_{buffer_free}((_{struct buffer}) " + result + ")", 2);
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.realTypeName + "_free");
    result = typeInfo.realTypeName + "_free((" + typeInfo.realTypeCodeTrimmed + ") " + result + ")";
  } else if (type->isStr()) {
    result = this->_apiEval("_{str_free}((_{struct str}) " + result + ")", 2);
  }

  return result;
}

std::string Codegen::_genReallocFn (Type *type, const std::string &leftCode, const std::string &rightCode) {
  auto initialState = this->state;
  auto result = std::string();

  if (type->isAny()) {
    result = this->_apiEval(leftCode + " = _{any_realloc}(" + leftCode + ", " + rightCode + ")", 1);
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = this->_apiEval(leftCode + " = _{buffer_realloc}(" + leftCode + ", " + rightCode + ")", 1);
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.typeName + "_realloc");
    result = leftCode + " = " + typeInfo.typeName + "_realloc(" + leftCode + ", " + rightCode + ")";
  } else if (type->isStr()) {
    result = this->_apiEval(leftCode + " = _{str_realloc}(" + leftCode + ", " + rightCode + ")", 1);
  }

  return result;
}

std::string Codegen::_genStrFn (Type *type, const std::string &code, bool copy, bool escape) {
  auto initialState = this->state;
  auto realType = Type::real(type);
  auto result = code;

  if (realType->isAny()) {
    result = this->_apiEval("_{any_str}(" + (copy ? this->_genCopyFn(realType, result) : result) + ")", 1);
  } else if (realType->isObj() && realType->builtin && realType->codeName == "@buffer_Buffer") {
    result = this->_apiEval("_{buffer_str}(" + (copy ? this->_genCopyFn(realType, result) : result) + ")", 1);
  } else if (
    realType->isArray() ||
    realType->isFn() ||
    realType->isMap() ||
    realType->isObj() ||
    realType->isOpt() ||
    realType->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(realType);

    this->_activateEntity(typeInfo.realTypeName + "_str");
    result = typeInfo.realTypeName + "_str(" + (copy ? this->_genCopyFn(realType, result) : result) + ")";
  } else if (realType->isEnum()) {
    result = this->_apiEval("_{enum_str}(" + result + ")", 1);
  } else if (realType->isStr() && escape) {
    result = this->_apiEval("_{str_escape}(" + result + ")", 1);
  } else if (realType->isStr()) {
    result = copy ? this->_genCopyFn(realType, result) : result;
  } else {
    auto typeInfo = this->_typeInfo(realType);
    result = this->_apiEval("_{" + typeInfo.realTypeName + "_str}(" + result + ")", 1);
  }

  return result;
}

std::string Codegen::_node (const ASTNode &node, bool root, CodegenPhase phase) {
  auto code = std::string();

  if (std::holds_alternative<ASTNodeBreak>(*node.body)) {
    if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_LOOP)) {
      code = std::string(this->indent, ' ') + this->state.cleanUp.currentBreakVar() + " = 1;" EOL;

      if (!ASTChecker(node.parent).is<ASTNodeLoop>() || !ASTChecker(node).isLast()) {
        code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
      }
    } else {
      code = std::string(this->indent, ' ') + "break;" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeContinue>(*node.body)) {
    if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_LOOP)) {
      if (!ASTChecker(node.parent).is<ASTNodeLoop>() || !ASTChecker(node).isLast()) {
        code = std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
      }
    } else {
      code = std::string(this->indent, ' ') + "continue;" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeEnumDecl>(*node.body)) {
    auto nodeEnumDecl = std::get<ASTNodeEnumDecl>(*node.body);
    auto typeName = Codegen::typeName(nodeEnumDecl.type->codeName);
    auto enumType = std::get<TypeEnum>(nodeEnumDecl.type->body);

    if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
      this->_apiEntity(typeName, CODEGEN_ENTITY_ENUM, [&] (auto &decl, [[maybe_unused]] auto &def) {
        auto membersCode = std::string();

        for (auto i = static_cast<std::size_t>(0); i < enumType.members.size(); i++) {
          auto member = enumType.members[i];

          auto nodeEnumDeclMember = std::find_if(
            nodeEnumDecl.members.begin(),
            nodeEnumDecl.members.end(),
            [&member] (const auto &it) -> bool {
              return it.id == member->name;
            }
          );

          if (nodeEnumDeclMember != nodeEnumDecl.members.end()) {
            membersCode += "  " + Codegen::name(member->codeName);

            if (nodeEnumDeclMember->init != std::nullopt) {
              membersCode += " = " + this->_nodeExpr(*nodeEnumDeclMember->init, nodeEnumDeclMember->init->type, true);
            }

            membersCode += i == enumType.members.size() - 1 ? EOL : "," EOL;
          }
        }

        decl += "enum " + typeName + " {" EOL;
        decl += membersCode;
        decl += "};";

        return 0;
      });

      this->_apiEntity(typeName + "_rawValue", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
        auto typeInfo = this->_typeInfo(nodeEnumDecl.type);

        decl += "_{struct str} " + typeName + "_rawValue (" + typeInfo.typeCodeTrimmed + ");";
        def += "_{struct str} " + typeName + "_rawValue (" + typeInfo.typeCode + "n) {" EOL;

        for (const auto &member : enumType.members) {
          def += "  if (n == " + Codegen::name(member->codeName) + ")" R"( return _{str_alloc}(")" + member->name + R"(");)" EOL;
        }

        def += "}";

        return 0;
      });
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*node.body);
    code = this->_nodeExpr(nodeExpr, nodeExpr.type, true);

    if (root) {
      code = std::string(this->indent, ' ') + code + ";" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

    code += this->_fnDecl(
      nodeFnDecl.var->type,
      nodeFnDecl.var->codeName,
      nodeFnDecl.stack,
      nodeFnDecl.params,
      nodeFnDecl.body,
      phase
    );

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    auto nodeIf = std::get<ASTNodeIf>(*node.body);
    auto initialStateTypeCasts = this->state.typeCasts;
    auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(nodeIf.cond);

    code = std::string(this->indent, ' ') + "if (" + this->_nodeExpr(nodeIf.cond, this->ast->typeMap.get("bool")) + ") {" EOL;

    bodyTypeCasts.merge(this->state.typeCasts);
    bodyTypeCasts.swap(this->state.typeCasts);
    this->varMap.save();
    code += this->_block(nodeIf.body);
    this->varMap.restore();
    this->state.typeCasts = initialStateTypeCasts;

    if (nodeIf.alt != std::nullopt) {
      code += std::string(this->indent, ' ') + "} else ";
      altTypeCasts.merge(this->state.typeCasts);
      altTypeCasts.swap(this->state.typeCasts);

      if (std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
        this->varMap.save();
        code += "{" EOL + this->_block(std::get<ASTBlock>(*nodeIf.alt));
        code += std::string(this->indent, ' ') + "}" EOL;
        this->varMap.restore();
      } else if (std::holds_alternative<ASTNode>(*nodeIf.alt)) {
        auto elseIfCode = this->_node(std::get<ASTNode>(*nodeIf.alt));
        code += elseIfCode.substr(elseIfCode.find_first_not_of(' '));
      }

      this->state.typeCasts = initialStateTypeCasts;
    } else {
      code += std::string(this->indent, ' ') + "}" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(*node.body);

    auto initialCleanUp = this->state.cleanUp;
    auto initialIndent = this->indent;

    this->varMap.save();
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialCleanUp);
    this->state.cleanUp.breakVarIdx += 1;

    if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (1)";
    } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (" + this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), true) + ")";
    } else if (nodeLoop.init != std::nullopt) {
      this->indent += 2;
      auto initCode = this->_node(*nodeLoop.init);
      this->indent = initialIndent;

      if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_BLOCK)) {
        code = std::string(this->indent, ' ') + "{" EOL + initCode;
        code += std::string(this->indent + 2, ' ') + "for (;";

        this->indent += 2;
      } else {
        code = std::string(this->indent, ' ') + "for (" + this->_node(*nodeLoop.init, false) + ";";
      }

      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), true)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, true)) + ")";
    } else {
      code = std::string(this->indent, ' ') + "for (;";
      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), true)) + ";";
      code += " " + this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, true) + ")";
    }

    auto saveCleanUp = this->state.cleanUp;
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveCleanUp);

    auto bodyCode = this->_block(nodeLoop.body);
    code += " {" EOL;

    if (this->state.cleanUp.breakVarUsed) {
      code += std::string(this->indent + 2, ' ') + "unsigned char " + this->state.cleanUp.currentBreakVar() + " = 0;" EOL;
    }

    code += bodyCode + std::string(this->indent, ' ') + "}" EOL;

    if (nodeLoop.init != std::nullopt && !saveCleanUp.empty()) {
      code += saveCleanUp.gen(this->indent);

      if (saveCleanUp.returnVarUsed) {
        code += std::string(this->indent, ' ') + "if (r == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
      }

      this->indent = initialIndent;
      code += std::string(this->indent, ' ') + "}" EOL;
    }

    this->state.cleanUp.breakVarIdx -= 1;
    this->state.cleanUp = initialCleanUp;
    this->varMap.restore();

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(*node.body);

    this->varMap.save();
    code = this->_block(nodeMain.body);
    this->varMap.restore();

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);

    if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
      this->_typeNameObj(nodeObjDecl.type);
      this->_typeNameObjDef(nodeObjDecl.type);
    }

    if (phase == CODEGEN_PHASE_ALLOC_METHOD || phase == CODEGEN_PHASE_FULL) {
      for (const auto &nodeObjDeclMethod : nodeObjDecl.methods) {
        code += this->_fnDecl(
          nodeObjDeclMethod.var->type,
          nodeObjDeclMethod.var->codeName,
          nodeObjDeclMethod.stack,
          nodeObjDeclMethod.params,
          nodeObjDeclMethod.body,
          CODEGEN_PHASE_ALLOC
        );
      }
    }

    if (phase == CODEGEN_PHASE_INIT || phase == CODEGEN_PHASE_FULL) {
      for (const auto &nodeObjDeclMethod : nodeObjDecl.methods) {
        code += this->_fnDecl(
          nodeObjDeclMethod.var->type,
          nodeObjDeclMethod.var->codeName,
          nodeObjDeclMethod.stack,
          nodeObjDeclMethod.params,
          nodeObjDeclMethod.body,
          phase
        );
      }
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

    if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN) || this->state.cleanUp.returnVarUsed) {
      auto parentNotRoot = this->state.cleanUp.parent != nullptr && this->state.cleanUp.parent->type != CODEGEN_CLEANUP_ROOT;

      if (parentNotRoot && this->state.cleanUp.parent->hasCleanUp(CODEGEN_CLEANUP_FN)) {
        code += std::string(this->indent, ' ') + this->state.cleanUp.currentReturnVar() + " = 1;" EOL;
      }

      if (nodeReturn.body != std::nullopt) {
        code += std::string(this->indent, ' ') + this->state.cleanUp.currentValueVar() + " = ";
        code += this->_nodeExpr(*nodeReturn.body, this->state.returnType) + ";" EOL;
      }

      auto nodeParentFunction = ASTChecker(node.parent).is<ASTNodeFnDecl>() || ASTChecker(node.parent).is<ASTNodeObjDecl>();
      auto nodeIsLast = node.parent != nullptr && ASTChecker(node).isLast();

      if ((!nodeParentFunction && this->state.cleanUp.empty()) || !nodeIsLast) {
        code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
      }
    } else if (nodeReturn.body != std::nullopt) {
      code = std::string(this->indent, ' ') + "return " + this->_nodeExpr(*nodeReturn.body, this->state.returnType) + ";" EOL;
    } else {
      code = std::string(this->indent, ' ') + "return;" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeTypeDecl>(*node.body)) {
    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
    auto name = Codegen::name(nodeVarDecl.var->codeName);
    auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);
    auto initCode = std::string();

    if (nodeVarDecl.init != std::nullopt) {
      initCode = this->_nodeExpr(*nodeVarDecl.init, typeInfo.type);
    } else {
      initCode = this->_nodeVarDeclInit(typeInfo);
    }

    code = !root ? code : std::string(this->indent, ' ');
    code += (nodeVarDecl.var->mut ? typeInfo.typeCode : typeInfo.typeCodeConst) + name + " = " + initCode + (root ? ";" EOL : "");

    if (typeInfo.type->shouldBeFreed()) {
      this->state.cleanUp.add(this->_genFreeFn(typeInfo.type, name) + ";");
    }

    return this->_wrapNode(node, code);
  }

  throw Error("tried to generate code for unknown node");
}

std::string Codegen::_nodeExpr (const ASTNodeExpr &nodeExpr, Type *targetType, bool root) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
    auto code = std::string();

    if (exprAccess.expr != std::nullopt && std::holds_alternative<std::shared_ptr<Var>>(*exprAccess.expr)) {
      auto objVar = std::get<std::shared_ptr<Var>>(*exprAccess.expr);

      if (objVar->builtin && (objVar->codeName == "@math_MaxFloat" || objVar->codeName == "@math_MaxF64")) {
        code = this->_apiEval("_{DBL_MAX}");
      } else if (objVar->builtin && (objVar->codeName == "@math_MinFloat" || objVar->codeName == "@math_MinF64")) {
        code = this->_apiEval("-_{DBL_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxF32") {
        code = this->_apiEval("_{FLT_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MinF32") {
        code = this->_apiEval("-_{FLT_MAX}");
      } else if (objVar->builtin && (objVar->codeName == "@math_MaxInt" || objVar->codeName == "@math_MaxI32")) {
        code = this->_apiEval("_{INT32_MAX}");
      } else if (objVar->builtin && (objVar->codeName == "@math_MinInt" || objVar->codeName == "@math_MinI32")) {
        code = this->_apiEval("_{INT32_MIN}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxI8") {
        code = this->_apiEval("_{INT8_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MinI8") {
        code = this->_apiEval("_{INT8_MIN}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxI16") {
        code = this->_apiEval("_{INT16_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MinI16") {
        code = this->_apiEval("_{INT16_MIN}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxI64") {
        code = this->_apiEval("_{INT64_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MinI64") {
        code = this->_apiEval("_{INT64_MIN}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxU8") {
        code = this->_apiEval("_{UINT8_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxU16") {
        code = this->_apiEval("_{UINT16_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxU32") {
        code = this->_apiEval("_{UINT32_MAX}");
      } else if (objVar->builtin && objVar->codeName == "@math_MaxU64") {
        code = this->_apiEval("_{UINT64_MAX}");
      } else if (objVar->builtin && (
        objVar->codeName == "@math_MinU8" ||
        objVar->codeName == "@math_MinU16" ||
        objVar->codeName == "@math_MinU32" ||
        objVar->codeName == "@math_MinU64"
      )) {
        code = "0";
      } else if (objVar->builtin && objVar->codeName == "@os_EOL") {
        code = this->_apiEval("_{str_alloc}(_{THE_EOL})");
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@os_NAME") {
        code = this->_apiEval("_{os_name}()");
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@path_SEP") {
        code = this->_apiEval("_{str_alloc}(_{THE_PATH_SEP})");
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@process_args") {
        this->needMainArgs = true;
        code = this->_apiEval("_{process_args}()");
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@process_env") {
        code = this->_apiEval("_{process_env}()");
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@process_home") {
        code = this->_apiEval("_{process_home}()");
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else {
        auto objCode = Codegen::name(objVar->codeName);
        auto objType = this->state.typeCasts.contains(objCode) ? this->state.typeCasts[objCode] : objVar->type;

        code = objCode;

        if (objVar->type->isAny() && !objType->isAny()) {
          auto typeName = this->_typeNameAny(objType);
          this->_activateEntity(typeName);
          code = "((struct " + typeName + " *) " + code + ".d)->d";
        } else if (objVar->type->isOpt() && !objType->isOpt()) {
          code = "*" + code;
        } else if (objVar->type->isUnion() && (!objType->isUnion() || objVar->type->hasSubType(objType))) {
          code = code + ".v" + this->_typeDefIdx(objType);
        }

        if (this->state.contextVars.contains(objCode) && (nodeExpr.type->isRef() || !targetType->isRef())) {
          code = "*" + code;
        }

        if (
          !this->state.contextVars.contains(objCode) &&
          ((!nodeExpr.type->isRef() && targetType->isRef()) || targetType->isRefOf(nodeExpr.type))
        ) {
          code = "&" + code;
        } else if (nodeExpr.type->isRefOf(targetType)) {
          auto refNodeExprType = Type::actual(nodeExpr.type);

          while (refNodeExprType->isRefOf(targetType)) {
            code = "*" + code;
            refNodeExprType = Type::actual(std::get<TypeRef>(refNodeExprType->body).refType);
          }
        }

        auto nodeExprTypeRefAny = targetType->isAny() && !nodeExpr.type->isRefOf(targetType);
        auto shouldCopyVar = !objType->isRef() || (!targetType->isRef() && !targetType->hasSubType(objType) && !nodeExprTypeRefAny);

        if (!root && shouldCopyVar) {
          code = this->_genCopyFn(Type::real(objType), code);
        }
      }
    } else if (exprAccess.expr != std::nullopt && std::holds_alternative<ASTNodeExpr>(*exprAccess.expr)) {
      auto objNodeExpr = std::get<ASTNodeExpr>(*exprAccess.expr);
      auto objTypeInfo = this->_typeInfo(objNodeExpr.type);
      auto originalObjMemberType = nodeExpr.type;
      auto fieldTypeHasCallInfo = false;
      auto fieldType = static_cast<Type *>(nullptr);

      if (
        exprAccess.prop != std::nullopt &&
        objTypeInfo.realType->hasField(*exprAccess.prop) &&
        objTypeInfo.realType->getField(*exprAccess.prop).builtin &&
        !objTypeInfo.realType->getField(*exprAccess.prop).callInfo.empty()
      ) {
        auto typeField = objTypeInfo.realType->getField(*exprAccess.prop);

        if (typeField.callInfo.isSelfFirst) {
          auto objCode = this->_nodeExpr(objNodeExpr, typeField.callInfo.selfType, typeField.callInfo.isSelfMut);
          code = objNodeExpr.parenthesized ? objCode : "(" + objCode + ")";
        } else {
          code = "()";
        }

        code = this->_apiEval("_{" + typeField.callInfo.codeName + "}" + code, 1);
        fieldTypeHasCallInfo = true;
        fieldType = typeField.type;
      } else if (exprAccess.prop != std::nullopt && objTypeInfo.realType->isEnum()) {
        auto enumType = std::get<TypeEnum>(objTypeInfo.realType->body);

        for (const auto &member : enumType.members) {
          if (member->name == *exprAccess.prop) {
            code = Codegen::name(member->codeName);
            break;
          }
        }
      } else if (exprAccess.prop != std::nullopt) {
        auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, true);

        if (objCode.starts_with("*")) {
          objCode = "(" + objCode + ")";
        }

        code = objCode + "->" + Codegen::name(*exprAccess.prop);

        for (const auto &field : objTypeInfo.realType->fields) {
          if (field.name == *exprAccess.prop) {
            originalObjMemberType = field.type;
            break;
          }
        }
      } else if (exprAccess.elem != std::nullopt) {
        auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, true);
        auto objElemCode = this->_nodeExpr(*exprAccess.elem, this->ast->typeMap.get("int"));

        if (objTypeInfo.realType->isArray()) {
          code = this->_apiEval("_{" + objTypeInfo.realTypeName + "_at}(" + objCode + ", " + objElemCode + ")", 1);
        } else if (objTypeInfo.realType->isStr()) {
          code = this->_apiEval("_{str_at}(" + objCode + ", " + objElemCode + ")", 1);
        }
      }

      auto objMemberType = this->state.typeCasts.contains(code) ? this->state.typeCasts[code] : originalObjMemberType;

      if (originalObjMemberType->isOpt() && !objMemberType->isOpt()) {
        code = "*" + code;
      }

      if (!nodeExpr.type->isRef() && targetType->isRef()) {
        code = "&" + code;
      } else if (nodeExpr.type->isRef() && !targetType->isRef()) {
        code = "*" + code;
      }

      auto isExprAccessEnumField = objTypeInfo.realType->isEnum() && exprAccess.prop != std::nullopt;
      auto isExprAccessRef = nodeExpr.type->isRef() && targetType->isRef();

      if (!root && !isExprAccessEnumField && !isExprAccessRef && !(fieldTypeHasCallInfo && !fieldType->isRef())) {
        code = this->_genCopyFn(Type::real(nodeExpr.type), code);
      }

      if (
        root &&
        nodeExpr.type->shouldBeFreed() &&
        (exprAccess.prop == std::nullopt || (fieldType != nullptr && fieldType->shouldBeFreed()))
      ) {
        code = this->_genFreeFn(nodeExpr.type, code);
      }
    } else if (exprAccess.expr == std::nullopt && exprAccess.prop != std::nullopt) {
      auto memberName = *exprAccess.prop;
      auto enumType = std::get<TypeEnum>(nodeExpr.type->body);

      auto member = std::find_if(
        enumType.members.begin(),
        enumType.members.end(),
        [&memberName] (const auto &it) -> bool {
          return it->name == memberName;
        }
      );

      if (member != enumType.members.end()) {
        code = Codegen::name((*member)->codeName);
      }
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprArray>(*nodeExpr.body)) {
    auto exprArray = std::get<ASTExprArray>(*nodeExpr.body);
    auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
    auto arrayType = std::get<TypeArray>(nodeTypeInfo.type->body);
    auto fieldsCode = std::to_string(exprArray.elements.size());

    for (const auto &element : exprArray.elements) {
      fieldsCode += ", " + this->_nodeExpr(element, arrayType.elementType);
    }

    this->_activateEntity(nodeTypeInfo.typeName + "_alloc");
    auto code = nodeTypeInfo.typeName + "_alloc(" + fieldsCode + ")";
    code = !root ? code : this->_genFreeFn(nodeTypeInfo.type, code);

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto leftCode = this->_nodeExpr(exprAssign.left, nodeExpr.type, true);
    auto code = std::string();

    if (Type::actual(exprAssign.left.type)->isAny() || Type::actual(exprAssign.right.type)->isAny()) {
      code = this->_genReallocFn(this->ast->typeMap.get("any"), leftCode, this->_nodeExpr(exprAssign.right, nodeExpr.type));
      code = root ? code : this->_genCopyFn(this->ast->typeMap.get("any"), code);
    } else if (
      Type::actual(exprAssign.left.type)->isArray() ||
      Type::actual(exprAssign.left.type)->isFn() ||
      Type::actual(exprAssign.left.type)->isMap() ||
      Type::actual(exprAssign.left.type)->isObj() ||
      Type::actual(exprAssign.left.type)->isOpt() ||
      Type::actual(exprAssign.left.type)->isUnion()
    ) {
      code = this->_genReallocFn(Type::actual(exprAssign.left.type), leftCode, this->_nodeExpr(exprAssign.right, nodeExpr.type));
      code = root ? code : this->_genCopyFn(Type::actual(exprAssign.left.type), code);
    } else if (Type::actual(exprAssign.left.type)->isStr() || Type::actual(exprAssign.right.type)->isStr()) {
      auto rightCode = std::string();

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        if (exprAssign.right.isLit()) {
          rightCode = this->_apiEval("_{str_concat_cstr}(" + this->_genCopyFn(this->ast->typeMap.get("str"), leftCode) + ", " + exprAssign.right.litBody() + ")", 1);
        } else {
          rightCode = this->_apiEval("_{str_concat_str}(" + this->_genCopyFn(this->ast->typeMap.get("str"), leftCode) + ", " + this->_nodeExpr(exprAssign.right, nodeExpr.type) + ")", 1);
        }
      } else {
        rightCode = this->_nodeExpr(exprAssign.right, nodeExpr.type);
      }

      code = this->_genReallocFn(this->ast->typeMap.get("str"), leftCode, rightCode);
      code = root ? code : this->_genCopyFn(this->ast->typeMap.get("str"), code);
    } else {
      auto opCode = std::string(" = ");
      auto rightCode = this->_nodeExpr(exprAssign.right, nodeExpr.type);

      if (exprAssign.op == AST_EXPR_ASSIGN_AND) {
        rightCode = leftCode + " && " + rightCode;
      } else if (exprAssign.op == AST_EXPR_ASSIGN_OR) {
        rightCode = leftCode + " || " + rightCode;
      } else {
        if (exprAssign.op == AST_EXPR_ASSIGN_ADD) opCode = " += ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_AND) opCode = " &= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_OR) opCode = " |= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_XOR) opCode = " ^= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_DIV) opCode = " /= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_EQ) opCode = " = ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_LSHIFT) opCode = " <<= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_MOD) opCode = " %= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_MUL) opCode = " *= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_RSHIFT) opCode = " >>= ";
        else if (exprAssign.op == AST_EXPR_ASSIGN_SUB) opCode = " -= ";
      }

      code = leftCode + opCode + rightCode;
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);
    auto code = std::string();

    if (
      exprBinary.op == AST_EXPR_BINARY_EQ &&
      Type::real(exprBinary.left.type)->isObj() &&
      Type::real(exprBinary.right.type)->isObj() &&
      Type::real(exprBinary.left.type)->builtin &&
      Type::real(exprBinary.right.type)->builtin &&
      Type::real(exprBinary.left.type)->codeName == "@buffer_Buffer" &&
      Type::real(exprBinary.right.type)->codeName == "@buffer_Buffer"
    ) {
      auto leftCode = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type));
      auto rightCode = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type));
      code = this->_apiEval("_{buffer_eq}(" + leftCode + ", " + rightCode + ")", 1);
    } else if (
      exprBinary.op == AST_EXPR_BINARY_NE &&
      Type::real(exprBinary.left.type)->isObj() &&
      Type::real(exprBinary.right.type)->isObj() &&
      Type::real(exprBinary.left.type)->builtin &&
      Type::real(exprBinary.right.type)->builtin &&
      Type::real(exprBinary.left.type)->codeName == "@buffer_Buffer" &&
      Type::real(exprBinary.right.type)->codeName == "@buffer_Buffer"
    ) {
      auto leftCode = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type));
      auto rightCode = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type));
      code = this->_apiEval("_{buffer_ne}(" + leftCode + ", " + rightCode + ")", 1);
    } else if (exprBinary.op == AST_EXPR_BINARY_EQ && (
      (Type::real(exprBinary.left.type)->isArray() && Type::real(exprBinary.right.type)->isArray()) ||
      (Type::real(exprBinary.left.type)->isMap() && Type::real(exprBinary.right.type)->isMap()) ||
      (Type::real(exprBinary.left.type)->isObj() && Type::real(exprBinary.right.type)->isObj()) ||
      (Type::real(exprBinary.left.type)->isOpt() && Type::real(exprBinary.right.type)->isOpt())
    )) {
      auto typeInfo = this->_typeInfo(exprBinary.left.type);
      auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType);
      auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType);

      this->_activateEntity(typeInfo.realTypeName + "_eq");
      code = typeInfo.realTypeName + "_eq(" + leftCode + ", " + rightCode + ")";
    } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
      (Type::real(exprBinary.left.type)->isArray() && Type::real(exprBinary.right.type)->isArray()) ||
      (Type::real(exprBinary.left.type)->isMap() && Type::real(exprBinary.right.type)->isMap()) ||
      (Type::real(exprBinary.left.type)->isObj() && Type::real(exprBinary.right.type)->isObj()) ||
      (Type::real(exprBinary.left.type)->isOpt() && Type::real(exprBinary.right.type)->isOpt())
    )) {
      auto typeInfo = this->_typeInfo(exprBinary.left.type);
      auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType);
      auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType);

      this->_activateEntity(typeInfo.realTypeName + "_ne");
      code = typeInfo.realTypeName + "_ne(" + leftCode + ", " + rightCode + ")";
    } else if (exprBinary.op == AST_EXPR_BINARY_EQ && (
      Type::real(exprBinary.left.type)->isStr() &&
      Type::real(exprBinary.right.type)->isStr()
    )) {
      if (exprBinary.left.isLit() && exprBinary.right.isLit()) {
        code = this->_apiEval("_{cstr_eq_cstr}(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")", 1);
      } else if (exprBinary.left.isLit()) {
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));
        code = this->_apiEval("_{cstr_eq_str}(" + exprBinary.left.litBody() + ", " + rightCode + ")", 1);
      } else if (exprBinary.right.isLit()) {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        code = this->_apiEval("_{str_eq_cstr}(" + leftCode + ", " + exprBinary.right.litBody() + ")", 1);
      } else {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));
        code = this->_apiEval("_{str_eq_str}(" + leftCode + ", " + rightCode + ")", 1);
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
      Type::real(exprBinary.left.type)->isStr() &&
      Type::real(exprBinary.right.type)->isStr()
    )) {
      if (exprBinary.left.isLit() && exprBinary.right.isLit()) {
        code = this->_apiEval("_{cstr_ne_cstr}(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")", 1);
      } else if (exprBinary.left.isLit()) {
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));
        code = this->_apiEval("_{cstr_ne_str}(" + exprBinary.left.litBody() + ", " + rightCode + ")", 1);
      } else if (exprBinary.right.isLit()) {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        code = this->_apiEval("_{str_ne_cstr}(" + leftCode + ", " + exprBinary.right.litBody() + ")", 1);
      } else {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));
        code = this->_apiEval("_{str_ne_str}(" + leftCode + ", " + rightCode + ")", 1);
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_ADD && (
      Type::real(exprBinary.left.type)->isStr() &&
      Type::real(exprBinary.right.type)->isStr()
    )) {
      if (root && nodeExpr.isLit()) {
        return this->_wrapNodeExpr(nodeExpr, targetType, root, nodeExpr.litBody());
      } else if (nodeExpr.isLit()) {
        code = this->_apiEval("_{str_alloc}(" + nodeExpr.litBody() + ")", 1);
      } else if (exprBinary.left.isLit()) {
        code = this->_apiEval("_{cstr_concat_str}(" + exprBinary.left.litBody() + ", " + this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str")) + ")", 1);
      } else if (exprBinary.right.isLit()) {
        code = this->_apiEval("_{str_concat_cstr}(" + this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str")) + ", " + exprBinary.right.litBody() + ")", 1);
      } else {
        code = this->_apiEval("_{str_concat_str}(" + this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str")) + ", " + this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str")) + ")", 1);
      }

      code = !root ? code : this->_genFreeFn(Type::real(exprBinary.left.type), code);
    } else if (exprBinary.op == AST_EXPR_BINARY_EQ && (
      (Type::real(exprBinary.left.type)->isUnion() || Type::real(exprBinary.right.type)->isUnion())
    )) {
      auto unionType = Type::real(Type::real(exprBinary.left.type)->isUnion() ? exprBinary.left.type : exprBinary.right.type);
      auto typeInfo = this->_typeInfo(unionType);
      auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType);
      auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType);

      this->_activateEntity(typeInfo.realTypeName + "_eq");
      code = typeInfo.realTypeName + "_eq(" + leftCode + ", " + rightCode + ")";
    } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
      (Type::real(exprBinary.left.type)->isUnion() || Type::real(exprBinary.right.type)->isUnion())
    )) {
      auto unionType = Type::real(Type::real(exprBinary.left.type)->isUnion() ? exprBinary.left.type : exprBinary.right.type);
      auto typeInfo = this->_typeInfo(unionType);
      auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType);
      auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType);

      this->_activateEntity(typeInfo.realTypeName + "_ne");
      code = typeInfo.realTypeName + "_ne(" + leftCode + ", " + rightCode + ")";
    } else {
      auto leftCode = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type));
      auto rightCode = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type));
      auto opCode = std::string();

      if (exprBinary.op == AST_EXPR_BINARY_ADD) opCode = " + ";
      else if (exprBinary.op == AST_EXPR_BINARY_AND) opCode = " && ";
      else if (exprBinary.op == AST_EXPR_BINARY_BIT_AND) opCode = " & ";
      else if (exprBinary.op == AST_EXPR_BINARY_BIT_OR) opCode = " | ";
      else if (exprBinary.op == AST_EXPR_BINARY_BIT_XOR) opCode = " ^ ";
      else if (exprBinary.op == AST_EXPR_BINARY_DIV) opCode = " / ";
      else if (exprBinary.op == AST_EXPR_BINARY_EQ) opCode = " == ";
      else if (exprBinary.op == AST_EXPR_BINARY_GE) opCode = " >= ";
      else if (exprBinary.op == AST_EXPR_BINARY_GT) opCode = " > ";
      else if (exprBinary.op == AST_EXPR_BINARY_LSHIFT) opCode = " << ";
      else if (exprBinary.op == AST_EXPR_BINARY_LE) opCode = " <= ";
      else if (exprBinary.op == AST_EXPR_BINARY_LT) opCode = " < ";
      else if (exprBinary.op == AST_EXPR_BINARY_MOD) opCode = " % ";
      else if (exprBinary.op == AST_EXPR_BINARY_MUL) opCode = " * ";
      else if (exprBinary.op == AST_EXPR_BINARY_NE) opCode = " != ";
      else if (exprBinary.op == AST_EXPR_BINARY_OR) opCode = " || ";
      else if (exprBinary.op == AST_EXPR_BINARY_RSHIFT) opCode = " >> ";
      else if (exprBinary.op == AST_EXPR_BINARY_SUB) opCode = " - ";

      code = leftCode + opCode + rightCode;
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) {
    auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);
    auto calleeTypeInfo = this->_typeInfo(exprCall.callee.type);
    auto code = std::string();

    if (calleeTypeInfo.realType->builtin && calleeTypeInfo.realType->codeName == "@print") {
      auto separator = std::string(R"(" ")");
      auto isSeparatorLit = true;
      auto terminator = this->_apiEval("_{THE_EOL}");
      auto isTerminatorLit = true;
      auto to = this->_apiEval("_{stdout}");

      for (const auto &exprCallArg : exprCall.args) {
        if (exprCallArg.id != std::nullopt && exprCallArg.id == "separator") {
          if (exprCallArg.expr.isLit()) {
            separator = exprCallArg.expr.litBody();
          } else {
            separator = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"));
            isSeparatorLit = false;
          }
        } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "terminator") {
          if (exprCallArg.expr.isLit()) {
            terminator = exprCallArg.expr.litBody();
          } else {
            terminator = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"));
            isTerminatorLit = false;
          }
        } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "to") {
          if (exprCallArg.expr.isLit()) {
            to = this->_apiEval(exprCallArg.expr.litBody() == R"("stderr")" ? "_{stderr}" : "_{stdout}");
          } else {
            to = this->_apiEval(R"(_{cstr_eq_str}("stderr")");
            to += ", " + this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str")) + ") ? ";
            to += this->_apiEval("_{stderr} : _{stdout}");
          }
        }
      }

      code = "_{print}(" + to + R"(, ")";

      auto argsCode = std::string();
      auto argIdx = static_cast<std::size_t>(0);

      for (const auto &exprCallArg : exprCall.args) {
        if (exprCallArg.id != "items") {
          continue;
        }

        auto argTypeInfo = this->_typeInfo(exprCallArg.expr.type);

        if (argIdx != 0 && separator != R"("")") {
          code += isSeparatorLit ? "z" : "s";
          argsCode += separator + ", ";
        }

        code += this->_exprCallPrintArgSign(argTypeInfo, exprCallArg.expr);
        argsCode += this->_exprCallPrintArg(argTypeInfo, exprCallArg.expr) + ", ";
        argIdx++;
      }

      if (terminator == R"("")") {
        code += R"(", )" + argsCode.substr(0, argsCode.size() - 2);
      } else {
        code += std::string(isTerminatorLit ? "z" : "s") + R"(", )" + argsCode + terminator;
      }

      code += ")";
      code = this->_apiEval(code, 1);
      return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
    } else if (calleeTypeInfo.realType->builtin && calleeTypeInfo.realType->codeName == "@utils_swap") {
      auto argTypeInfo = this->_typeInfo(exprCall.args[0].expr.type);
      auto argRealTypeInfo = argTypeInfo;

      if (argTypeInfo.type->isRef()) {
        argRealTypeInfo = this->_typeInfo(std::get<TypeRef>(argTypeInfo.type->body).refType);
      }

      code = this->_apiEval(
        "_{utils_swap}(" + this->_nodeExpr(exprCall.args[0].expr, argTypeInfo.type) +
        ", " + this->_nodeExpr(exprCall.args[1].expr, argTypeInfo.type) + ", sizeof(" + argRealTypeInfo.typeCodeTrimmed + "))",
        1
      );

      return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
    }

    auto fnType = std::get<TypeFn>(calleeTypeInfo.realType->body);
    auto bodyCode = std::string();

    if (!fnType.params.empty() && !calleeTypeInfo.realType->builtin) {
      auto paramsName = calleeTypeInfo.realTypeName + "P";
      bodyCode += this->_apiEval("(struct _{" + paramsName + "}) {");
    }

    for (auto i = static_cast<std::size_t>(0); i < fnType.params.size(); i++) {
      auto param = fnType.params[i];
      auto paramTypeInfo = this->_typeInfo(param.type);
      auto foundArg = std::optional<ASTExprCallArg>{};
      auto foundArgIdx = static_cast<std::size_t>(0);

      if (param.name != std::nullopt) {
        for (auto j = static_cast<std::size_t>(0); j < exprCall.args.size(); j++) {
          if (exprCall.args[j].id == param.name) {
            foundArgIdx = j;
            foundArg = exprCall.args[foundArgIdx];
            break;
          }
        }
      } else if (i < exprCall.args.size()) {
        foundArgIdx = i;
        foundArg = exprCall.args[foundArgIdx];
      }

      if (!param.required && !param.variadic) {
        bodyCode += std::string(i == 0 ? "" : ", ") + (foundArg == std::nullopt ? "0" : "1");
      }

      bodyCode += i == 0 && (param.required || param.variadic) ? "" : ", ";

      if (param.variadic) {
        auto paramTypeElementType = std::get<TypeArray>(param.type->body).elementType;
        auto variadicArgs = std::vector<ASTExprCallArg>{};

        if (foundArg != std::nullopt) {
          variadicArgs.push_back(*foundArg);

          for (auto j = foundArgIdx + 1; j < exprCall.args.size(); j++) {
            auto exprCallArg = exprCall.args[j];

            if (exprCallArg.id != std::nullopt && *exprCallArg.id != param.name) {
              break;
            }

            variadicArgs.push_back(exprCallArg);
          }
        }

        bodyCode += this->_apiEval("_{" + paramTypeInfo.typeName + "_alloc}(") + std::to_string(variadicArgs.size());

        for (const auto &variadicArg : variadicArgs) {
          bodyCode += ", " + this->_nodeExpr(variadicArg.expr, paramTypeElementType);
        }

        bodyCode += ")";
      } else if (foundArg != std::nullopt) {
        bodyCode += this->_nodeExpr(foundArg->expr, paramTypeInfo.type);
      } else {
        bodyCode += this->_exprCallDefaultArg(paramTypeInfo);
      }
    }

    if (!fnType.params.empty() && !calleeTypeInfo.realType->builtin) {
      bodyCode += "}";
    }

    auto fnName = std::string();

    if (calleeTypeInfo.realType->builtin && !fnType.callInfo.empty()) {
      fnName = this->_apiEval("_{" + fnType.callInfo.codeName + "}");
    } else if (fnType.callInfo.empty()) {
      fnName = this->_nodeExpr(exprCall.callee, calleeTypeInfo.realType, true);
    } else {
      fnName = Codegen::name(fnType.callInfo.codeName);
    }

    if (
      fnType.isMethod &&
      this->state.contextVars.contains(fnName) &&
      (nodeExpr.type->isRef() || !targetType->isRef())
    ) {
      fnName = "*" + fnName;
    }

    if (fnName.starts_with("*")) {
      fnName = "(" + fnName + ")";
    }

    auto selfCode = std::string();
    auto isSelfParenthesized = false;

    if (fnType.isMethod && fnType.callInfo.isSelfFirst) {
      auto exprAccess = std::get<ASTExprAccess>(*exprCall.callee.body);
      auto nodeExprAccess = std::get<ASTNodeExpr>(*exprAccess.expr);

      selfCode += calleeTypeInfo.realType->builtin ? "" : ", ";
      isSelfParenthesized = nodeExprAccess.parenthesized;

      if (calleeTypeInfo.realType->builtin) {
        selfCode += this->_nodeExpr(nodeExprAccess, fnType.callInfo.selfType, fnType.callInfo.isSelfMut);
      } else {
        selfCode += this->_genCopyFn(fnType.callInfo.selfType, this->_nodeExpr(nodeExprAccess, fnType.callInfo.selfType, true));
      }
    }

    code = fnName;

    if (!calleeTypeInfo.realType->builtin || fnType.callInfo.empty()) {
      code += ".f(" + code + ".x";
    } else if (!isSelfParenthesized) {
      code += "(";
    }

    code += selfCode;

    if (!bodyCode.empty()) {
      code += !calleeTypeInfo.realType->builtin || !selfCode.empty() ? ", " : "";
      code += bodyCode;
    }

    if (!isSelfParenthesized) {
      code += ")";
    }

    if (!root && nodeExpr.type->isRef() && !targetType->isRef()) {
      code = this->_genCopyFn(targetType, "*" + code);
    }

    code = !root ? code : this->_genFreeFn(nodeExpr.type, code);
    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto initialStateTypeCasts = this->state.typeCasts;
    auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(exprCond.cond);
    auto condCode = this->_nodeExpr(exprCond.cond, this->ast->typeMap.get("bool"));

    bodyTypeCasts.merge(this->state.typeCasts);
    bodyTypeCasts.swap(this->state.typeCasts);
    auto bodyCode = this->_nodeExpr(exprCond.body, nodeExpr.type);
    this->state.typeCasts = initialStateTypeCasts;
    altTypeCasts.merge(this->state.typeCasts);
    altTypeCasts.swap(this->state.typeCasts);
    auto altCode = this->_nodeExpr(exprCond.alt, nodeExpr.type);
    this->state.typeCasts = initialStateTypeCasts;

    if (
      std::holds_alternative<ASTExprAssign>(*exprCond.alt.body) &&
      !exprCond.alt.parenthesized &&
      !exprCond.alt.type->isSafeForTernaryAlt()
    ) {
      altCode = "(" + altCode + ")";
    }

    auto code = condCode + " ? " + bodyCode + " : " + altCode;

    if (root && nodeExpr.type->shouldBeFreed()) {
      code = this->_genFreeFn(nodeExpr.type, "(" + code + ")");
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprIs>(*nodeExpr.body)) {
    auto exprIs = std::get<ASTExprIs>(*nodeExpr.body);
    auto exprIsExprCode = this->_nodeExpr(exprIs.expr, exprIs.expr.type, true);
    auto exprIsTypeDef = this->_typeDef(exprIs.type);

    this->_activateEntity(exprIsTypeDef);
    auto code = exprIsExprCode + ".t == " + exprIsTypeDef;

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprLit>(*nodeExpr.body)) {
    auto exprLit = std::get<ASTExprLit>(*nodeExpr.body);
    auto code = exprLit.body;

    if (exprLit.type == AST_EXPR_LIT_BOOL) {
      code = this->_apiEval("_{" + code + "}");
    } else if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
      auto val = std::stoull(code);

      if (val > 9223372036854775807) {
        code += "U";
      }
    } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      code.erase(std::remove(code.begin(), code.end(), 'O'), code.end());
      code.erase(std::remove(code.begin(), code.end(), 'o'), code.end());
    } else if (exprLit.type == AST_EXPR_LIT_NIL) {
      code = this->_apiEval("_{NULL}");
    } else if (!root && exprLit.type == AST_EXPR_LIT_STR) {
      code = this->_apiEval("_{str_alloc}(" + code + ")", 1);
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprMap>(*nodeExpr.body)) {
    auto exprMap = std::get<ASTExprMap>(*nodeExpr.body);
    auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
    auto mapType = std::get<TypeBodyMap>(nodeTypeInfo.type->body);
    auto fieldsCode = std::to_string(exprMap.props.size());

    for (const auto &prop : exprMap.props) {
      fieldsCode += this->_apiEval(R"(, _{str_alloc}(")" + prop.name + R"("), )" + this->_nodeExpr(prop.init, mapType.valueType), 1);
    }

    this->_activateEntity(nodeTypeInfo.typeName + "_alloc");
    auto code = nodeTypeInfo.typeName + "_alloc(" + fieldsCode + ")";
    code = !root ? code : this->_genFreeFn(nodeTypeInfo.type, code);

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
    auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);
    auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
    auto fieldsCode = std::string();

    for (const auto &typeField : nodeTypeInfo.type->fields) {
      if (typeField.builtin || typeField.type->isMethod()) {
        continue;
      }

      auto fieldTypeInfo = this->_typeInfo(typeField.type);

      auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&typeField] (const auto &it) -> bool {
        return it.name == typeField.name;
      });

      fieldsCode += ", ";

      if (exprObjProp != exprObj.props.end()) {
        fieldsCode += this->_nodeExpr(exprObjProp->init, typeField.type);
      } else {
        fieldsCode += this->_exprObjDefaultField(fieldTypeInfo);
      }
    }

    this->_activateEntity(nodeTypeInfo.typeName + "_alloc");
    auto code = nodeTypeInfo.typeName + "_alloc(" + (fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2)) + ")";
    code = !root ? code : this->_genFreeFn(nodeTypeInfo.type, code);

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprRef>(*nodeExpr.body)) {
    auto exprRef = std::get<ASTExprRef>(*nodeExpr.body);
    auto code = std::string();

    if (targetType->isAny()) {
      code = this->_nodeExpr(exprRef.expr, targetType, targetType->isRef());
      return this->_wrapNodeExpr(nodeExpr, targetType, true, code);
    } else if (targetType->isOpt()) {
      auto optTargetType = std::get<TypeOptional>(targetType->body).type;
      code = this->_nodeExpr(exprRef.expr, optTargetType, optTargetType->isRef());
    } else {
      code = this->_nodeExpr(exprRef.expr, targetType, targetType->isRef());
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);
    auto argCode = this->_nodeExpr(exprUnary.arg, nodeExpr.type);
    auto opCode = std::string();

    if (exprUnary.op == AST_EXPR_UNARY_BIT_NOT) opCode = "~";
    else if (exprUnary.op == AST_EXPR_UNARY_DECREMENT) opCode = "--";
    else if (exprUnary.op == AST_EXPR_UNARY_INCREMENT) opCode = "++";
    else if (exprUnary.op == AST_EXPR_UNARY_MINUS) opCode = "-";
    else if (exprUnary.op == AST_EXPR_UNARY_NOT) opCode = "!";
    else if (exprUnary.op == AST_EXPR_UNARY_PLUS) opCode = "+";

    if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isFloatNumber()) {
      argCode = this->_apiEval("((_{bool}) " + argCode + ")", 1);
    } else if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isStr()) {
      argCode = this->_apiEval("_{str_not}(" + argCode + ")", 1);
      opCode = "";
    } else if (argCode.starts_with("*")) {
      argCode = "(" + argCode + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, exprUnary.prefix ? opCode + argCode : argCode + opCode);
  }

  throw Error("tried to generate code for unknown expression");
}

std::string Codegen::_nodeVarDeclInit (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return this->_apiEval("{0, _{NULL}, 0, _{NULL}, _{NULL}}");
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    this->_activateEntity(typeInfo.typeName + "_alloc");
    return typeInfo.typeName + "_alloc(0)";
  } else if (typeInfo.type->isBool()) {
    return this->_apiEval("_{false}");
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried node variable declaration of invalid type");
  } else if (typeInfo.type->isObj()) {
    auto fieldsCode = std::string();

    for (const auto &typeField : typeInfo.type->fields) {
      if (!typeField.builtin && !typeField.type->isMethod()) {
        fieldsCode += ", " + this->_exprObjDefaultField(this->_typeInfo(typeField.type));
      }
    }

    auto allocCode = fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2);
    return this->_apiEval("_{" + typeInfo.typeName + "_alloc}(" + allocCode + ")", 1);
  } else if (typeInfo.type->isOpt()) {
    return this->_apiEval("_{NULL}");
  } else if (typeInfo.type->isStr()) {
    return this->_apiEval(R"(_{str_alloc}(""))");
  } else {
    return "0";
  }
}

std::string Codegen::_type (Type *type) {
  if (type->isAny()) {
    this->_activateBuiltin("typeAny");
    return "struct any ";
  } else if (type->isArray()) {
    auto typeName = this->_typeNameArray(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isByte()) {
    return "unsigned char ";
  } else if (type->isChar()) {
    return "char ";
  } else if (type->isEnum()) {
    auto typeName = Codegen::typeName(type->codeName);
    this->_activateEntity(typeName);
    return "enum " + typeName + " ";
  } else if (type->isF32()) {
    return "float ";
  } else if (type->isF64() || type->isFloat()) {
    return "double ";
  } else if (type->isBool()) {
    this->_activateBuiltin("libStdbool");
    return "bool ";
  } else if (type->isFn()) {
    if (type->builtin) {
      return type->name;
    }

    auto typeName = this->_typeNameFn(type);
    return this->_apiEval("struct _{" + typeName + "} ", 1);
  } else if (type->isI8()) {
    this->_activateBuiltin("libStdint");
    return "int8_t ";
  } else if (type->isI16()) {
    this->_activateBuiltin("libStdint");
    return "int16_t ";
  } else if (type->isI32() || type->isInt()) {
    this->_activateBuiltin("libStdint");
    return "int32_t ";
  } else if (type->isI64()) {
    this->_activateBuiltin("libStdint");
    return "int64_t ";
  } else if (type->isMap()) {
    auto typeName = this->_typeNameMap(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("typeBuffer");
    return "struct buffer ";
  } else if (type->isObj()) {
    auto typeName = type->builtin ? type->name : Codegen::typeName(type->codeName);
    this->_activateEntity(typeName);
    return "struct " + typeName + " *";
  } else if (type->isOpt()) {
    this->_typeNameOpt(type);
    auto optType = std::get<TypeOptional>(type->body);
    return this->_type(optType.type) + "*";
  } else if (type->isRef()) {
    auto refType = std::get<TypeRef>(type->body);
    return this->_type(refType.refType) + "*";
  } else if (type->isStr()) {
    this->_activateBuiltin("typeStr");
    return "struct str ";
  } else if (type->isU8()) {
    this->_activateBuiltin("libStdint");
    return "uint8_t ";
  } else if (type->isU16()) {
    this->_activateBuiltin("libStdint");
    return "uint16_t ";
  } else if (type->isU32()) {
    this->_activateBuiltin("libStdint");
    return "uint32_t ";
  } else if (type->isU64()) {
    this->_activateBuiltin("libStdint");
    return "uint64_t ";
  } else if (type->isUnion()) {
    auto typeName = this->_typeNameUnion(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isVoid()) {
    return "void ";
  }

  throw Error("tried generating unknown type");
}

std::string Codegen::_typeDef (Type *type) {
  auto initialStateBuiltins = this->state.builtins;
  auto initialStateEntities = this->state.entities;
  auto typeInfo = this->_typeInfo(type);
  this->state.builtins = initialStateBuiltins;
  this->state.entities = initialStateEntities;

  auto typeName = "TYPE_" + std::string(typeInfo.typeName.starts_with("__THE_1_") ? typeInfo.typeName.substr(8) : typeInfo.typeName);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  this->_apiEntity(typeName, CODEGEN_ENTITY_DEF, [&] (auto &decl, [[maybe_unused]] auto &def) {
    decl += "#define " + typeName + " " + std::to_string(this->lastTypeIdx++);
    return 0;
  });

  return typeName;
}

std::string Codegen::_typeDefIdx (Type *type) {
  auto typeName = this->_typeDef(type);
  auto result = std::string();

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      result = entity.decl.substr(entity.decl.find_last_of(' ') + 1);
      break;
    }
  }

  return result;
}

CodegenTypeInfo Codegen::_typeInfo (Type *type) {
  auto typeInfoItem = this->_typeInfoItem(type);

  if (!type->isRef()) {
    return CodegenTypeInfo{
      typeInfoItem.type,
      typeInfoItem.typeName,
      typeInfoItem.typeCode,
      typeInfoItem.typeCodeConst,
      typeInfoItem.typeCodeTrimmed,
      typeInfoItem.typeCodeConstTrimmed,
      typeInfoItem.typeRefCode,
      typeInfoItem.typeRefCodeConst,
      typeInfoItem.type,
      typeInfoItem.typeName,
      typeInfoItem.typeCode,
      typeInfoItem.typeCodeConst,
      typeInfoItem.typeCodeTrimmed,
      typeInfoItem.typeCodeConstTrimmed,
      typeInfoItem.typeRefCode,
      typeInfoItem.typeRefCodeConst
    };
  }

  auto realType = Type::real(type);
  auto realTypeInfoItem = this->_typeInfoItem(realType);

  return CodegenTypeInfo{
    typeInfoItem.type,
    typeInfoItem.typeName,
    typeInfoItem.typeCode,
    typeInfoItem.typeCodeConst,
    typeInfoItem.typeCodeTrimmed,
    typeInfoItem.typeCodeConstTrimmed,
    typeInfoItem.typeRefCode,
    typeInfoItem.typeRefCodeConst,
    realTypeInfoItem.type,
    realTypeInfoItem.typeName,
    realTypeInfoItem.typeCode,
    realTypeInfoItem.typeCodeConst,
    realTypeInfoItem.typeCodeTrimmed,
    realTypeInfoItem.typeCodeConstTrimmed,
    realTypeInfoItem.typeRefCode,
    realTypeInfoItem.typeRefCodeConst
  };
}

CodegenTypeInfoItem Codegen::_typeInfoItem (Type *type) {
  if (type->isAlias() && !type->builtin) {
    return this->_typeInfoItem(std::get<TypeAlias>(type->body).type);
  }

  auto typeCode = this->_type(type);
  auto typeCodeTrimmed = typeCode.substr(0, typeCode.find_last_not_of(' ') + 1);
  auto typeName = std::string();

  if (type->isArray() && !type->builtin) {
    typeName = this->_typeNameArray(type);
  } else if ((type->isEnum() || type->isObj()) && !type->builtin) {
    typeName = Codegen::typeName(type->codeName);
  } else if (type->isFn() && !type->builtin) {
    typeName = this->_typeNameFn(type);
  } else if (type->isMap() && !type->builtin) {
    typeName = this->_typeNameMap(type);
  } else if (type->isOpt() && !type->builtin) {
    typeName = this->_typeNameOpt(type);
  } else if (type->isUnion() && !type->builtin) {
    typeName = this->_typeNameUnion(type);
  } else {
    typeName = type->name;
  }

  return CodegenTypeInfoItem{
    type,
    typeName,
    typeCode,
    "const " + typeCode,
    typeCodeTrimmed,
    "const " + typeCodeTrimmed,
    typeCode + "*",
    "const " + typeCode + "*"
  };
}

std::string Codegen::_typeNameAny (Type *type) {
  auto initialStateBuiltins = this->state.builtins;
  auto initialStateEntities = this->state.entities;
  auto typeInfoTypeName = this->_typeInfo(type).typeName;
  this->state.builtins = initialStateBuiltins;
  this->state.entities = initialStateEntities;

  auto typeName = Codegen::typeName("any_" + (typeInfoTypeName.starts_with("__THE_1_") ? typeInfoTypeName.substr(8) : typeInfoTypeName));

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  " + typeInfo.typeCode + "d;" EOL;
    def += "};";

    return 0;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "_{struct any} " + typeName + "_copy (const _{struct any});";
    def += "_{struct any} " + typeName + "_copy (const _{struct any} n) {" EOL;
    def += "  struct _{" + typeName + "} *o = n.d;" EOL;
    def += "  struct _{" + typeName + "} *r = _{alloc}(n.l);" EOL;
    def += "  r->d = " + this->_genCopyFn(typeInfo.type, "o->d") + ";" EOL;
    def += "  return (_{struct any}) {n.t, r, n.l, n._copy, n._free};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "void " + typeName + "_free (_{struct any});";
    def += "void " + typeName + "_free (_{struct any} _n) {" EOL;
    def += "  struct _{" + typeName + "} *n = _n.d;" EOL;

    if (typeInfo.type->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(typeInfo.type, "n->d") + ";" EOL;
    }

    def += "  _{free}(n);" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto typeInfo = this->_typeInfo(type);

    decl += "_{struct any} " + typeName + "_alloc (" + typeInfo.typeCodeTrimmed + ");";
    def += "_{struct any} " + typeName + "_alloc (" + typeInfo.typeCode + "d) {" EOL;
    def += "  _{size_t} l = sizeof(struct _{" + typeName + "});" EOL;
    def += "  struct _{" + typeName + "} *r = _{alloc}(l);" EOL;
    def += "  r->d = d;" EOL;
    def += "  return (_{struct any}) {_{" + this->_typeDef(typeInfo.type) + "}, ";
    def += "r, l, &_{" + typeName + "_copy}, &_{" + typeName + "_free}};" EOL;
    def += "}";

    return copyFnEntityIdx;
  });

  return typeName;
}

std::string Codegen::_typeNameArray (Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto elementType = std::get<TypeArray>(type->body).elementType;
  auto elementRealTypeCode = elementType->isRef() ? "*n.d[i]" : "n.d[i]";

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d;" EOL;
    def += "  _{size_t} l;" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_alloc (_{size_t}, ...);";
    def += "struct _{" + typeName + "} " + typeName + "_alloc (_{size_t} x, ...) {" EOL;
    def += "  if (x == 0) return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(x * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{va_list} args;" EOL;
    def += "  _{va_start}(args, x);" EOL;
    def += "  for (_{size_t} i = 0; i < x; i++) ";
    def += "d[i] = _{va_arg}(args, " + elementType->vaArgCode(elementTypeInfo.typeCodeTrimmed) + ");" EOL;
    def += "  _{va_end}(args);" EOL;
    def += "  return (struct _{" + typeName + "}) {d, x};" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;

    if (elementTypeInfo.type->shouldBeFreed()) {
      def += "  for (_{size_t} i = 0; i < n.l; i++) " + this->_genFreeFn(elementTypeInfo.type, "n.d[i]") + ";" EOL;
    }

    def += "  _{free}(n.d);" EOL;
    def += "}";

    return 0;
  });

  auto atFnEntityIdx = this->_apiEntity(typeName + "_at", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeRefCode + typeName + "_at (struct _{" + typeName + "}, _{int32_t});";
    def += elementTypeInfo.typeRefCode + typeName + "_at (struct _{" + typeName + "} n, _{int32_t} i) {" EOL;
    def += "  if ((i >= 0 && i >= n.l) || (i < 0 && i < -((_{int32_t}) n.l))) {" EOL;
    def += R"(    _{fprintf}(_{stderr}, "Error: index %" _{PRId32} " out of array bounds" _{THE_EOL}, i);)" EOL;
    def += "    _{exit}(_{EXIT_FAILURE});" EOL;
    def += "  }" EOL;
    def += "  return i < 0 ? &n.d[n.l + i] : &n.d[i];" EOL;
    def += "}";

    return freeFnEntityIdx;
  });

  auto clearFnEntityIdx = this->_apiEntity(typeName + "_clear", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *);";
    def += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *self) {" EOL;
    def += "  " + this->_genFreeFn(type, "*self") + ";" EOL;
    def += "  self->d = _{NULL};" EOL;
    def += "  self->l = 0;" EOL;
    def += "  return self;" EOL;
    def += "}";

    return atFnEntityIdx + 1;
  });

  auto concatFnEntityIdx = this->_apiEntity(typeName + "_concat", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "concat";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_concat (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} " + typeName + "_concat (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{size_t} l = self.l + n1.l;" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{size_t} k = 0;" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) d[k++] = ";
    def += this->_genCopyFn(elementTypeInfo.type, "self.d[i]") + ";" EOL;
    def += "  for (_{size_t} i = 0; i < n1.l; i++) d[k++] = ";
    def += this->_genCopyFn(elementTypeInfo.type, "n1.d[i]") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "self") + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, l};" EOL;
    def += "}";

    return clearFnEntityIdx + 1;
  });

  auto containsFnEntityIdx = this->_apiEntity(typeName + "_contains", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "contains";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{bool} " + typeName + "_contains (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "_{bool} " + typeName + "_contains (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{bool} r = _{false};" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) {";
    def += "    if (" + this->_genEqFn(elementTypeInfo.type, "self.d[i]", "n1") + ") {" EOL;
    def += "      r = _{true};" EOL;
    def += "      break;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(param1TypeInfo.type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "self") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return concatFnEntityIdx + 1;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.l == 0) return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(n.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) d[i] = " + this->_genCopyFn(elementTypeInfo.type, "n.d[i]") + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, n.l};" EOL;
    def += "}";

    return containsFnEntityIdx + 1;
  });

  auto emptyFnEntityIdx = this->_apiEntity(typeName + "_empty", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_empty (struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_empty (struct _{" + typeName + "} n) {" EOL;
    def += "  _{bool} r = n.l == 0;" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l == n2.l;" EOL;
    def += "  if (r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(elementTypeInfo.type, "n1.d[i]", "n2.d[i]", true) + ") {" EOL;
    def += "        r = _{false};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return emptyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_filter", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "filter";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_filter (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} " + typeName + "_filter (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{size_t} l = 0;" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = alloc(self.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) {" EOL;
    def += "    if (n1.f(n1.x, (struct _{" + param1TypeInfo.typeName + "P}) ";
    def += "{" + this->_genCopyFn(elementTypeInfo.type, "self.d[i]") + "})) {" EOL;
    def += "      d[l++] = " + this->_genCopyFn(elementTypeInfo.type, "self.d[i]") + ";" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(param1TypeInfo.type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "self") + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_first", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeRefCode + typeName + "_first (struct _{" + typeName + "} *);";
    def += elementTypeInfo.typeRefCode + typeName + "_first (struct _{" + typeName + "} *self) {" EOL;
    def += "  if (self->l == 0) {" EOL;
    def += R"(    _{fprintf}(_{stderr}, "Error: tried getting first element of empty array" THE_EOL);)" EOL;
    def += "    _{exit}(_{EXIT_FAILURE});" EOL;
    def += "  }" EOL;
    def += "  return &self->d[0];" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_forEach", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "forEach";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "void " + typeName + "_forEach (struct _{" + typeName + "}, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "void " + typeName + "_forEach (struct _{" + typeName + "} self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  for (_{size_t} i = 0; i < self.l; i++) {" EOL;
    def += "    n1.f(n1.x, (struct _{" + param1TypeInfo.typeName + "P}) ";
    def += "{" + this->_genCopyFn(elementTypeInfo.type, "self.d[i]") + ", i});" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(param1TypeInfo.type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "self") + ";" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_join", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{struct str} " + typeName + "_join (struct _{" + typeName + "}, unsigned char, _{struct str});";
    def += "_{struct str} " + typeName + "_join (struct _{" + typeName + "} n, unsigned char o1, _{struct str} n1) {" EOL;
    def += R"(  _{struct str} x = o1 == 0 ? _{str_alloc}(",") : n1;)" EOL;
    def += R"(  _{struct str} r = _{str_alloc}("");)" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += "    if (i != 0) r = _{str_concat_str}(r, _{str_copy}(x));" EOL;
    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(elementTypeInfo.realType, elementRealTypeCode, true, false) + ");" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(this->ast->typeMap.get("str"), "x") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += R"(  return r;)" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_last", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeRefCode + typeName + "_last (struct _{" + typeName + "} *);";
    def += elementTypeInfo.typeRefCode + typeName + "_last (struct _{" + typeName + "} *self) {" EOL;
    def += "  if (self->l == 0) {" EOL;
    def += R"(    _{fprintf}(_{stderr}, "Error: tried getting last element of empty array" THE_EOL);)" EOL;
    def += "    _{exit}(_{EXIT_FAILURE});" EOL;
    def += "  }" EOL;
    def += "  return &self->d[self->l - 1];" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_len", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_len (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_len (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} l = n.l;" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return l;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_merge", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "merge";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  _{size_t} k = self->l;" EOL;
    def += "  self->l += n1.l;" EOL;
    def += "  self->d = _{re_alloc}(self->d, self->l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n1.l; i++) self->d[k++] = " + this->_genCopyFn(elementTypeInfo.type, "n1.d[i]") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  return self;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l != n2.l;" EOL;
    def += "  if (!r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(elementTypeInfo.type, "n1.d[i]", "n2.d[i]", true) + ") {" EOL;
    def += "        r = _{true};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_pop", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += elementTypeInfo.typeCode + typeName + "_pop (struct _{" + typeName + "} *);";
    def += elementTypeInfo.typeCode + typeName + "_pop (struct _{" + typeName + "} *n) {" EOL;
    def += "  n->l--;" EOL;
    def += "  return n->d[n->l];" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_push", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "void " + typeName + "_push (struct _{" + typeName + "} *, struct _{" + typeName + "});";
    def += "void " + typeName + "_push (struct _{" + typeName + "} *n, struct _{" + typeName + "} m) {" EOL;
    def += "  if (m.l == 0) return;" EOL;
    def += "  n->l += m.l;" EOL;
    def += "  n->d = _{re_alloc}(n->d, n->l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{size_t} k = 0;" EOL;
    def += "  for (_{size_t} i = n->l - m.l; i < n->l; i++) n->d[i] = m.d[k++];" EOL;
    def += "  _{free}(m.d);" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_remove", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "remove";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *, _{int32_t});";
    def += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *self, _{int32_t} n1) {" EOL;
    def += "  if ((n1 >= 0 && n1 >= self->l) || (n1 < 0 && n1 < -((_{int32_t}) self->l))) {" EOL;
    def += R"(    _{fprintf}(_{stderr}, "Error: index %" _{PRId32} " out of array bounds" _{THE_EOL}, n1);)" EOL;
    def += "    _{exit}(_{EXIT_FAILURE});" EOL;
    def += "  }" EOL;
    def += "  _{size_t} i = n1 < 0 ? n1 + self->l : n1;" EOL;

    if (elementTypeInfo.type->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(elementTypeInfo.type, "self->d[i]") + ";" EOL;
    }

    def += "  if (i != self->l - 1) {" EOL;
    def += "    _{memmove}(&self->d[i], &self->d[i + 1], (--self->l - i) * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  } else {" EOL;
    def += "    self->l--;" EOL;
    def += "  }" EOL;
    def += "  return self;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_reverse", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_reverse (struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_reverse (struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.l == 0) {" EOL;
    def += "    " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "    return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  }" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(n.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) d[i] = " + this->_genCopyFn(elementTypeInfo.type, "n.d[n.l - i - 1]") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, n.l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_slice", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} " + typeName + "_slice (struct _{" + typeName + "}, ";
    decl += "unsigned int, _{int32_t}, unsigned int, _{int32_t});";
    def += "struct _{" + typeName + "} " + typeName + "_slice (struct _{" + typeName + "} n, ";
    def += "unsigned int o1, _{int32_t} n1, unsigned int o2, _{int32_t} n2) {" EOL;
    def += "  _{int32_t} i1 = o1 == 0 ? 0 : (_{int32_t}) (n1 < 0 ? (n1 < -((_{int32_t}) n.l) ? 0 : n1 + n.l) ";
    def += ": (n1 > n.l ? n.l : n1));" EOL;
    def += "  _{int32_t} i2 = o2 == 0 ? (_{int32_t}) n.l : (_{int32_t}) (n2 < 0 ? (n2 < -((_{int32_t}) n.l) ? 0 : n2 + n.l) ";
    def += ": (n2 > n.l ? n.l : n2));" EOL;
    def += "  if (i1 > i2 || i1 >= n.l) {" EOL;
    def += "    " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "    return (struct _{" + typeName + "}) {_{NULL}, 0};" EOL;
    def += "  }" EOL;
    def += "  _{size_t} l = i2 - i1;" EOL;
    def += "  " + elementTypeInfo.typeRefCode + "d = _{alloc}(l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i1 < i2; i1++) d[i++] = " + this->_genCopyFn(elementTypeInfo.type, "n.d[i1]") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return (struct _{" + typeName + "}) {d, l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_sort", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto field = std::find_if(type->fields.begin(), type->fields.end(), [] (const auto &it) -> bool {
      return it.name == "sort";
    });

    auto fieldTypeFn = std::get<TypeFn>(field->type->body);
    auto param1TypeInfo = this->_typeInfo(fieldTypeFn.params[0].type);
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "struct _{" + typeName + "} *" + typeName + "_sort (struct _{" + typeName + "} *, " + param1TypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} *" + typeName + "_sort (struct _{" + typeName + "} *self, " + param1TypeInfo.typeCode + "n1) {" EOL;
    def += "  if (self->l < 2) return self;" EOL;
    def += "  while (1) {" EOL;
    def += "    unsigned char b = 0;" EOL;
    def += "    for (_{size_t} i = 1; i < self->l; i++) {" EOL;
    def += "      _{int32_t} c = n1.f(n1.x, (struct _{" + param1TypeInfo.typeName + "P}) {";
    def += this->_genCopyFn(elementTypeInfo.type, "self->d[i - 1]") + ", ";
    def += this->_genCopyFn(elementTypeInfo.type, "self->d[i]") + "});" EOL;
    def += "      if (c > 0) {" EOL;
    def += "        b = 1;" EOL;
    def += "        " + elementTypeInfo.typeCode + "t = self->d[i];" EOL;
    def += "        self->d[i] = self->d[i - 1];" EOL;
    def += "        self->d[i - 1] = t;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "    if (b == 0) break;" EOL;
    def += "  }" EOL;
    def += "  return self;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto elementTypeInfo = this->_typeInfo(elementType);

    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += R"(  _{struct str} r = _{str_alloc}("[");)" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += R"(    if (i != 0) r = _{str_concat_cstr}(r, ", ");)" EOL;

    if (elementTypeInfo.realType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(elementTypeInfo.realType, elementRealTypeCode) + ");" EOL;

    if (elementTypeInfo.realType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += R"(  return _{str_concat_cstr}(r, "]");)" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}

std::string Codegen::_typeNameFn (Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto fnType = std::get<TypeFn>(type->body);
  auto paramsName = typeName + "P";
  auto paramsEntityIdx = 0;

  if (!fnType.params.empty()) {
    paramsEntityIdx = this->_apiEntity(paramsName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
      auto paramsCode = std::string();
      auto paramIdx = static_cast<std::size_t>(0);

      for (const auto &param : fnType.params) {
        auto paramTypeInfo = this->_typeInfo(param.type);
        auto paramIdxStr = std::to_string(paramIdx);

        if (!param.required && !param.variadic) {
          paramsCode += "  unsigned char o" + paramIdxStr + ";" EOL;
        }

        paramsCode += "  " + paramTypeInfo.typeCode + "n" + paramIdxStr + ";" EOL;
        paramIdx++;
      }

      decl += "struct " + paramsName + ";";
      def += "struct " + paramsName + " {" EOL;
      def += paramsCode;
      def += "};";

      return 0;
    });
  }

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto returnTypeInfo = this->_typeInfo(fnType.returnType);
    auto functorArgs = std::string("void *");

    if (fnType.isMethod && fnType.callInfo.isSelfFirst) {
      auto selfTypeInfo = this->_typeInfo(fnType.callInfo.selfType);
      functorArgs += ", " + (fnType.callInfo.isSelfMut ? selfTypeInfo.typeCode : selfTypeInfo.typeCodeConst);
    }

    if (!fnType.params.empty()) {
      functorArgs += ", struct _{" + paramsName + "}";
    }

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  " + returnTypeInfo.typeCode + "(*f) (" + functorArgs + ");" EOL;
    def += "  void *x;" EOL;
    def += "  _{size_t} l;" EOL;
    def += "};";

    return fnType.params.empty() ? 0 : paramsEntityIdx;
  });

  this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.x == _{NULL}) return n;" EOL;
    def += "  void *x = _{alloc}(n.l);" EOL;
    def += "  _{memcpy}(x, n.x, n.l);" EOL;
    def += "  return (struct _{" + typeName + "}) {n.f, x, n.l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;
    def += "  if (n.x != _{NULL}) _{free}(n.x);" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += R"(  return _{str_alloc}("[Function]");)" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}

std::string Codegen::_typeNameMap (Type *type) {
  auto typeName = type->builtin ? type->name : Codegen::typeName(type->name);
  auto pairTypeName = Codegen::typeName("pair" + type->name.substr(3));

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto mapType = std::get<TypeBodyMap>(type->body);

  this->_apiEntity(pairTypeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += "struct " + pairTypeName + ";";
    def += "struct " + pairTypeName + " {" EOL;
    def += "  " + keyTypeInfo.typeCode + "f;" EOL;
    def += "  " + valueTypeInfo.typeCode + "s;" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  struct _{" + pairTypeName + "} *d;" EOL;
    def += "  _{size_t} c;" EOL;
    def += "  _{size_t} l;" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);
    auto keyVaArgCode = keyTypeInfo.type->vaArgCode(keyTypeInfo.typeCodeTrimmed);
    auto valueVaArgCode = valueTypeInfo.type->vaArgCode(valueTypeInfo.typeCodeTrimmed);

    decl += "struct _{" + typeName + "} " + typeName + "_alloc (int, ...);";
    def += "struct _{" + typeName + "} " + typeName + "_alloc (int x, ...) {" EOL;
    def += "  struct _{" + pairTypeName + "} *d = _{alloc}(x * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  _{va_list} args;" EOL;
    def += "  _{va_start}(args, x);" EOL;
    def += "  for (int i = 0; i < x; i++) d[i] = (struct _{" + pairTypeName + "}) ";
    def += "{_{va_arg}(args, " + keyVaArgCode + "), _{va_arg}(args, " + valueVaArgCode + ")};" EOL;
    def += "  _{va_end}(args);" EOL;
    def += "  return (struct _{" + typeName + "}) {d, x, x};" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "    " + this->_genFreeFn(mapType.keyType, "n.d[i].f") + ";" EOL;
    }

    if (mapType.valueType->shouldBeFreed()) {
      def += "    " + this->_genFreeFn(mapType.valueType, "n.d[i].s") + ";" EOL;
    }

    def += "  }" EOL;
    def += "  _{free}(n.d);" EOL;
    def += "}";

    return 0;
  });

  auto capFnEntityIdx = this->_apiEntity(typeName + "_cap", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_cap (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_cap (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} c = n.c;" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return c;" EOL;
    def += "}";

    return freeFnEntityIdx;
  });

  auto clearFnEntityIdx = this->_apiEntity(typeName + "_clear", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *);";
    def += "struct _{" + typeName + "} *" + typeName + "_clear (struct _{" + typeName + "} *n) {" EOL;
    def += "  " + this->_genFreeFn(type, "*n") + ";" EOL;
    def += "  n->d = _{NULL};" EOL;
    def += "  n->l = 0;" EOL;
    def += "  return n;" EOL;
    def += "}";

    return capFnEntityIdx + 1;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  struct _{" + pairTypeName + "} *d = _{alloc}(n.c * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) d[i] = (struct _{" + pairTypeName + "}) ";
    def += "{" + this->_genCopyFn(mapType.keyType, "n.d[i].f") + ", " + this->_genCopyFn(mapType.valueType, "n.d[i].s") + "};" EOL;
    def += "  return (struct _{" + typeName + "}) {d, n.c, n.l};" EOL;
    def += "}";

    return clearFnEntityIdx + 1;
  });

  auto emptyFnEntityIdx = this->_apiEntity(typeName + "_empty", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_empty (struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_empty (struct _{" + typeName + "} n) {" EOL;
    def += "  _{bool} r = n.l == 0;" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l == n2.l;" EOL;
    def += "  if (r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(mapType.keyType, "n1.d[i].f", "n2.d[i].f", true) + " || ";
    def += this->_genEqFn(mapType.valueType, "n1.d[i].s", "n2.d[i].s", true) + ") {" EOL;
    def += "        r = _{false};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return emptyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_get", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += valueTypeInfo.typeCode + typeName + "_get (struct _{" + typeName + "}, " + keyTypeInfo.typeCodeTrimmed + ");";
    def += valueTypeInfo.typeCode + typeName + "_get (struct _{" + typeName + "} n, " + keyTypeInfo.typeCode + "k) {" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n.d[i].f", "k") + ") {" EOL;
    def += "      " + valueTypeInfo.typeCode + "r = " + this->_genCopyFn(mapType.valueType, "n.d[i].s") + ";" EOL;
    def += "      " + this->_genFreeFn(type, "n") + ";" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "k") + ";" EOL;
    }

    def += "      return r;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += R"(  _{fprintf}(_{stderr}, "Error: failed to get map value" _{THE_EOL});)" EOL;
    def += "  _{exit}(_{EXIT_FAILURE});" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_has", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);

    decl += "_{bool} " + typeName + "_has (struct _{" + typeName + "}, " + keyTypeInfo.typeCodeTrimmed + ");";
    def += "_{bool} " + typeName + "_has (struct _{" + typeName + "} n, " + keyTypeInfo.typeCode + "k) {" EOL;
    def += "  _{bool} r = _{false};" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n.d[i].f", "k") + ") {" EOL;
    def += "      r = _{true};" EOL;
    def += "      break;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(mapType.keyType, "k") + ";" EOL;
    }

    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_keys", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto keysTypeInfo = this->_typeInfo(this->ast->typeMap.createArr(mapType.keyType));

    decl += keysTypeInfo.typeCode + typeName + "_keys (struct _{" + typeName + "});";
    def += keysTypeInfo.typeCode + typeName + "_keys (struct _{" + typeName + "} n) {" EOL;
    def += "  " + keyTypeInfo.typeCode + "*r = _{alloc}(n.l * sizeof(" + keyTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) r[i] = " + this->_genCopyFn(mapType.keyType, "n.d[i].f") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return (" + keysTypeInfo.typeCodeTrimmed + ") {r, n.l};" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_len", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_len (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_len (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} l = n.l;" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return l;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_merge", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} *" + typeName + "_merge (struct _{" + typeName + "} *n, struct _{" + typeName + "} m) {" EOL;
    def += "  if (n->l + m.l > n->c) n->d = _{re_alloc}(n->d, (n->c = n->l + m.l) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  for (_{size_t} i = 0; i < m.l; i++) {" EOL;
    def += "    for (_{size_t} j = 0; j < n->l; j++) {" EOL;
    def += "      if (" + this->_genEqFn(mapType.keyType, "n->d[j].f", "m.d[i].f") + ") {" EOL;

    if (mapType.valueType->shouldBeFreed()) {
      def += "        " + this->_genFreeFn(mapType.valueType, "n->d[j].s") + ";" EOL;
    }

    def += "        n->d[j].s = " + this->_genCopyFn(mapType.valueType, "m.d[i].s") + ";" EOL;
    def += "        goto " + typeName + "_merge_next;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "    n->d[n->l++] = (struct _{" + pairTypeName + "}) ";
    def += "{" + this->_genCopyFn(mapType.keyType, "m.d[i].f") + ", " + this->_genCopyFn(mapType.valueType, "m.d[i].s") + "};" EOL;
    def += typeName + "_merge_next:;" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "m") + ";" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.l != n2.l;" EOL;
    def += "  if (!r) {" EOL;
    def += "    for (_{size_t} i = 0; i < n1.l; i++) {" EOL;
    def += "      if (" + this->_genEqFn(mapType.keyType, "n1.d[i].f", "n2.d[i].f", true) + " || ";
    def += this->_genEqFn(mapType.valueType, "n1.d[i].s", "n2.d[i].s", true) + ") {" EOL;
    def += "        r = _{true};" EOL;
    def += "        break;" EOL;
    def += "      }" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_remove", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);

    decl += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *, " + keyTypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} *" + typeName + "_remove (struct _{" + typeName + "} *n, " + keyTypeInfo.typeCode + "k) {" EOL;
    def += "  for (_{size_t} i = 0; i < n->l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n->d[i].f", "k") + ") {" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "k") + ";" EOL;
    }

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "n->d[i].f") + ";" EOL;
    }

    if (mapType.valueType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.valueType, "n->d[i].s") + ";" EOL;
    }

    def += "      _{memmove}(&n->d[i], &n->d[i + 1], (--n->l - i) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "      return n;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += R"(  _{fprintf}(_{stderr}, "Error: failed to remove map value" _{THE_EOL});)" EOL;
    def += "  _{exit}(_{EXIT_FAILURE});" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_reserve", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} *" + typeName + "_reserve (struct _{" + typeName + "} *, _{int32_t});";
    def += "struct _{" + typeName + "} *" + typeName + "_reserve (struct _{" + typeName + "} *n, _{int32_t} c) {" EOL;
    def += "  if (c > n->c) n->d = _{re_alloc}(n->d, (n->c = c) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_set", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += "struct _{" + typeName + "} *" + typeName + "_set (struct _{" + typeName + "} *";
    decl += ", " + keyTypeInfo.typeCodeTrimmed + ", " + valueTypeInfo.typeCodeTrimmed + ");";
    def += "struct _{" + typeName + "} *" + typeName + "_set (struct _{" + typeName + "} *n";
    def += ", " + keyTypeInfo.typeCode + "k, " + valueTypeInfo.typeCode + "v) {" EOL;
    def += "  for (_{size_t} i = 0; i < n->l; i++) {" EOL;
    def += "    if (" + this->_genEqFn(mapType.keyType, "n->d[i].f", "k") + ") {" EOL;

    if (mapType.keyType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.keyType, "k") + ";" EOL;
    }

    if (mapType.valueType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.valueType, "n->d[i].s") + ";" EOL;
    }

    def += "      n->d[i].s = v;" EOL;
    def += "      return n;" EOL;
    def += "    }" EOL;
    def += "  }" EOL;
    def += "  if (n->l + 1 > n->c) n->d = _{re_alloc}(n->d, (n->c = n->l + 1) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  n->d[n->l++] = (struct _{" + pairTypeName + "}) {k, v};" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_shrink", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);

    decl += "struct _{" + typeName + "} *" + typeName + "_shrink (struct _{" + typeName + "} *);";
    def += "struct _{" + typeName + "} *" + typeName + "_shrink (struct _{" + typeName + "} *n) {" EOL;
    def += "  if (n->l != n->c && n->l == 0) {" EOL;
    def += "    _{free}(n->d);" EOL;
    def += "    n->d = _{NULL};" EOL;
    def += "    n->c = 0;" EOL;
    def += "  } else if (n->l != n->c) {" EOL;
    def += "    n->d = _{re_alloc}(n->d, (n->c = n->l) * sizeof(struct _{" + pairTypeName + "}));" EOL;
    def += "  }" EOL;
    def += "  return n;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += R"(  _{struct str} r = _{str_alloc}("{");)" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) {" EOL;
    def += R"(    if (i != 0) r = _{str_concat_cstr}(r, ", ");)" EOL;

    if (mapType.keyType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(mapType.keyType, "n.d[i].f") + ");" EOL;

    if (mapType.keyType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\": ");)" EOL;
    } else {
      def += R"(    r = _{str_concat_cstr}(r, ": ");)" EOL;
    }

    if (mapType.valueType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "    r = _{str_concat_str}(r, " + this->_genStrFn(mapType.valueType, "n.d[i].s") + ");" EOL;

    if (mapType.valueType->isStr()) {
      def += R"(    r = _{str_concat_cstr}(r, "\"");)" EOL;
    }

    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += R"(  return _{str_concat_cstr}(r, "}");)" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_values", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto valueTypeInfo = this->_typeInfo(mapType.valueType);
    auto valuesTypeInfo = this->_typeInfo(this->ast->typeMap.createArr(mapType.valueType));

    decl += valuesTypeInfo.typeCode + typeName + "_values (struct _{" + typeName + "});";
    def += valuesTypeInfo.typeCode + typeName + "_values (struct _{" + typeName + "} n) {" EOL;
    def += "  " + valueTypeInfo.typeCode + "*r = _{alloc}(n.l * sizeof(" + valueTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  for (_{size_t} i = 0; i < n.l; i++) r[i] = " + this->_genCopyFn(mapType.valueType, "n.d[i].s") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return (" + valuesTypeInfo.typeCodeTrimmed + ") {r, n.l};" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}

std::string Codegen::_typeNameObj (Type *type) {
  auto typeName = type->builtin ? type->name : Codegen::typeName(type->codeName);

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto objFields = std::string();

    for (const auto &field : type->fields) {
      if (field.builtin || field.type->isMethod()) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      objFields += "  " + (field.mut ? fieldTypeInfo.typeCode : fieldTypeInfo.typeCodeConst) + fieldName + ";" EOL;
    }

    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += objFields.empty() ? "  void *_;" EOL : objFields;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN);
  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN);

  return typeName;
}

std::string Codegen::_typeNameObjDef (Type *type, const std::map<std::string, std::string> &extra) {
  auto saveStateBuiltins = this->state.builtins;
  auto saveStateEntities = this->state.entities;
  auto typeName = type->builtin ? type->name : Codegen::typeName(type->codeName);

  for (auto &entity : this->entities) {
    if (entity.name == typeName + "_alloc") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      auto allocFnParamTypes = std::string();
      auto allocFnParams = std::string();
      auto allocFnCode = std::string();

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);
          auto fieldTypeInfo = this->_typeInfo(field.type);

          allocFnParamTypes += ", " + fieldTypeInfo.typeCodeTrimmed;
          allocFnParams += ", " + fieldTypeInfo.typeCode + fieldName;
          allocFnCode += ", " + fieldName;
        }
      }

      allocFnParamTypes = allocFnParamTypes.empty() ? allocFnParamTypes : allocFnParamTypes.substr(2);
      allocFnParams = allocFnParams.empty() ? allocFnParams : allocFnParams.substr(2);
      allocFnCode = allocFnCode.empty() ? "(void *) 0" : allocFnCode.substr(2);

      entity.decl += "struct _{" + typeName + "} *" + typeName + "_alloc (" + allocFnParamTypes + ");";
      entity.def += "struct _{" + typeName + "} *" + typeName + "_alloc (" + allocFnParams + ") {" EOL;
      entity.def += "  struct _{" + typeName + "} *" + "r = _{alloc}(sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  struct _{" + typeName + "} s = {" + allocFnCode + "};" EOL;
      entity.def += "  _{memcpy}(r, &s, sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_copy") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "struct _{" + typeName + "} *" + typeName + "_copy (const struct _{" + typeName + "} *);";
      entity.def += "struct _{" + typeName + "} *" + typeName + "_copy (const struct _{" + typeName + "} *n) {" EOL;
      entity.def += "  struct _{" + typeName + "} *r = _{alloc}(sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  struct _{" + typeName + "} s = {";

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);

          entity.def += (fieldIdx == 0 ? "" : ", ") + this->_genCopyFn(field.type, "n->" + fieldName);
          fieldIdx++;
        }
      }

      entity.def += "};" EOL;
      entity.def += "  _{memcpy}(r, &s, sizeof(struct _{" + typeName + "}));" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_eq") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} *, struct _{" + typeName + "} *);";
      entity.def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} *n1, struct _{" + typeName + "} *n2) {" EOL;
      entity.def += "  _{bool} r = ";

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);

          entity.def += (fieldIdx == 0 ? "" : " && ") + this->_genEqFn(field.type, "n1->" + fieldName, "n2->" + fieldName);
          fieldIdx++;
        }
      }

      entity.def += ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_free") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      entity.decl += "void " + typeName + "_free (struct _{" + typeName + "} *);";
      entity.def += "void " + typeName + "_free (struct _{" + typeName + "} *n) {" EOL;

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod() && field.type->shouldBeFreed()) {
          auto fieldName = Codegen::name(field.name);
          entity.def += "  " + this->_genFreeFn(field.type, "n->" + fieldName) + ";" EOL;
        }
      }

      entity.def += extra.contains("free") ? extra.at("free") : "";
      entity.def += "  _{free}(n);" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_ne") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} *, struct _{" + typeName + "} *);";
      entity.def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} *n1, struct _{" + typeName + "} *n2) {" EOL;
      entity.def += "  _{bool} r = ";

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);
          entity.def += (fieldIdx == 0 ? "" : " || ") + this->_genEqFn(field.type, "n1->" + fieldName, "n2->" + fieldName, true);
          fieldIdx++;
        }
      }

      entity.def += ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
      entity.def += "  return r;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_realloc") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      entity.decl += "struct _{" + typeName + "} *" + typeName + "_realloc ";
      entity.decl += "(struct _{" + typeName + "} *, struct _{" + typeName + "} *);";
      entity.def += "struct _{" + typeName + "} *" + typeName + "_realloc ";
      entity.def += "(struct _{" + typeName + "} *n1, struct _{" + typeName + "} *n2) {" EOL;
      entity.def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
      entity.def += "  return n2;" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    } else if (entity.name == typeName + "_str") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      auto fieldIdx = static_cast<std::size_t>(0);

      entity.decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} *);";
      entity.def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} *n) {" EOL;
      entity.def += R"(  _{struct str} r = _{str_alloc}(")" + type->name + R"({");)" EOL;

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.type->isMethod()) {
          auto fieldName = Codegen::name(field.name);
          auto fieldCode = std::string(field.type->isRef() ? "*" : "") + "n->" + fieldName;
          auto strCodeDelimiter = std::string(fieldIdx == 0 ? "" : ", ");

          if (field.type->isStr()) {
            entity.def += R"(  r = _{str_concat_cstr}(r, ")" + strCodeDelimiter + field.name + R"(: \"");)" EOL;
            entity.def += "  r = _{str_concat_str}(r, " + this->_genStrFn(field.type, fieldCode) + ");" EOL;
            entity.def += R"(  r = _{str_concat_cstr}(r, "\"");)" EOL;
          } else {
            entity.def += R"(  r = _{str_concat_cstr}(r, ")" + strCodeDelimiter + field.name + R"(: ");)" EOL;
            entity.def += "  r = _{str_concat_str}(r, " + this->_genStrFn(field.type, fieldCode) + ");" EOL;
          }

          fieldIdx++;
        }
      }

      entity.def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
      entity.def += R"(  return _{str_concat_cstr}(r, "}");)" EOL;
      entity.def += "}";
      entity.decl = this->_apiEval(entity.decl);
      entity.def = this->_apiEval(entity.def);
    }
  }

  this->state.builtins = saveStateBuiltins;
  this->state.entities = saveStateEntities;

  return typeName;
}

std::string Codegen::_typeNameOpt (Type *type) {
  auto typeName = Codegen::typeName(type->name);
  auto allocTypeName = typeName + "_alloc";

  for (const auto &entity : this->entities) {
    if (entity.name == allocTypeName) {
      return typeName;
    }
  }

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += underlyingTypeInfo.typeRefCode + typeName + "_alloc (" + underlyingTypeInfo.typeCodeTrimmed + ");";
    def += underlyingTypeInfo.typeRefCode + typeName + "_alloc (" + underlyingTypeInfo.typeCode + "n) {" EOL;
    def += "  " + underlyingTypeInfo.typeRefCode + "r = _{alloc}(sizeof(" + underlyingTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  *r = n;" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += "void " + typeName + "_free (" + underlyingTypeInfo.typeRefCode + ");";
    def += "void " + typeName + "_free (" + underlyingTypeInfo.typeRefCode + "n) {" EOL;
    def += "  if (n == _{NULL}) return;" EOL;

    if (underlyingTypeInfo.type->shouldBeFreed()) {
      def += "  " + this->_genFreeFn(underlyingTypeInfo.type, "*n") + ";" EOL;
    }

    def += "  _{free}(n);" EOL;
    def += "}";

    return 0;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += underlyingTypeInfo.typeRefCode + typeName + "_copy (const " + underlyingTypeInfo.typeRefCode + ");";
    def += underlyingTypeInfo.typeRefCode + typeName + "_copy (const " + underlyingTypeInfo.typeRefCode + "n) {" EOL;
    def += "  if (n == _{NULL}) return _{NULL};" EOL;
    def += "  " + underlyingTypeInfo.typeRefCode + "r = _{alloc}(sizeof(" + underlyingTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  *r = " + this->_genCopyFn(underlyingTypeInfo.type, "*n") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return freeFnEntityIdx;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += "_{bool} " + typeName + "_eq (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
    def += "_{bool} " + typeName + "_eq (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
    def += "  _{bool} r = (n1 == _{NULL} || n2 == _{NULL}) ? n1 == _{NULL} && n2 == _{NULL} ";
    def += ": " + this->_genEqFn(underlyingTypeInfo.type, "*n1", "*n2") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += "_{bool} " + typeName + "_ne (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
    def += "_{bool} " + typeName + "_ne (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
    def += "  _{bool} r = (n1 == _{NULL} || n2 == _{NULL}) ? n1 != _{NULL} || n2 != _{NULL} : ";
    def += this->_genEqFn(underlyingTypeInfo.type, "*n1", "*n2", true) + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += underlyingTypeInfo.typeRefCode + typeName + "_realloc (" + underlyingTypeInfo.typeRefCode;
    decl += ", " + underlyingTypeInfo.typeRefCode + ");";
    def += underlyingTypeInfo.typeRefCode + typeName + "_realloc (" + underlyingTypeInfo.typeRefCode + "n1";
    def += ", " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);

    decl += "_{struct str} " + typeName + "_str (" + underlyingTypeInfo.typeRefCode + ");";
    def += "_{struct str} " + typeName + "_str (" + underlyingTypeInfo.typeRefCode + "n) {" EOL;
    def += R"(  if (n == _{NULL}) return _{str_alloc}("nil");)" EOL;

    if (underlyingTypeInfo.realType->isStr()) {
      def += R"(  _{struct str} r = _{str_alloc}("\"");)" EOL;
      def += "  r = _{str_concat_str}(r, " + this->_genStrFn(underlyingTypeInfo.realType, "*n") + ");" EOL;
      def += R"(  r = _{str_concat_cstr}(r, "\"");)" EOL;
    } else {
      def += "  _{struct str} r = " + this->_genStrFn(underlyingTypeInfo.realType, "*n") + ";" EOL;
    }

    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}

std::string Codegen::_typeNameUnion (Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto subTypes = std::get<TypeUnion>(type->body).subTypes;

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    decl += "struct " + typeName + ";";
    def += "struct " + typeName + " {" EOL;
    def += "  int t;" EOL;
    def += "  union {" EOL;

    for (const auto &subType : subTypes) {
      auto subTypeInfo = this->_typeInfo(subType);
      def += "    " + subTypeInfo.typeCode + "v" + this->_typeDefIdx(subType) + ";" EOL;
    }

    def += "  };" EOL;
    def += "};";

    return 0;
  });

  this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_alloc (int, ...);";
    def += "struct _{" + typeName + "} " + typeName + "_alloc (int t, ...) {" EOL;
    def += "  struct _{" + typeName + "} r = {t};" EOL;
    def += "  _{va_list} args;" EOL;
    def += "  _{va_start}(args, t);" EOL;

    for (const auto &subType : subTypes) {
      auto subTypeInfo = this->_typeInfo(subType);

      def += "  if (t == _{" + this->_typeDef(subType) + "}) ";
      def += "r.v" + this->_typeDefIdx(subType) + " = _{va_arg}(args, " + subType->vaArgCode(subTypeInfo.typeCodeTrimmed) + ");" EOL;
    }

    def += "  _{va_end}(args);" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  auto freeFnEntityIdx = this->_apiEntity(typeName + "_free", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "void " + typeName + "_free (struct _{" + typeName + "});";
    def += "void " + typeName + "_free (struct _{" + typeName + "} n) {" EOL;

    for (const auto &subType : subTypes) {
      if (!subType->shouldBeFreed()) {
        continue;
      }

      def += "  if (n.t == _{" + this->_typeDef(subType) + "}) ";
      def += this->_genFreeFn(subType, "n.v" + this->_typeDefIdx(subType)) + ";" EOL;
    }

    def += "}";

    return 0;
  });

  auto copyFnEntityIdx = this->_apiEntity(typeName + "_copy", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_copy (const struct _{" + typeName + "} n) {" EOL;
    def += "  struct _{" + typeName + "} r = {n.t};" EOL;

    for (const auto &subType : subTypes) {
      auto subTypeProp = "v" + this->_typeDefIdx(subType);

      def += "  if (n.t == _{" + this->_typeDef(subType) + "}) r." + subTypeProp + " = ";
      def += this->_genCopyFn(subType, "n." + subTypeProp) + ";" EOL;
    }

    def += "  return r;" EOL;
    def += "}";

    return freeFnEntityIdx;
  });

  this->_apiEntity(typeName + "_eq", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_eq (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_eq (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.t == n2.t;" EOL;
    def += "  if (r) {" EOL;

    for (const auto &subType : subTypes) {
      auto subTypeProp = "v" + this->_typeDefIdx(subType);

      def += "    if (n1.t == _{" + this->_typeDef(subType) + "}) ";
      def += "r = " + this->_genEqFn(subType, "n1." + subTypeProp, "n2." + subTypeProp) + ";" EOL;
    }

    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return copyFnEntityIdx + 1;
  });

  this->_apiEntity(typeName + "_ne", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{bool} " + typeName + "_ne (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "_{bool} " + typeName + "_ne (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  _{bool} r = n1.t != n2.t;" EOL;
    def += "  if (!r) {" EOL;

    for (const auto &subType : subTypes) {
      auto subTypeProp = "v" + this->_typeDefIdx(subType);

      def += "    if (n1.t == _{" + this->_typeDef(subType) + "}) ";
      def += "r = " + this->_genEqFn(subType, "n1." + subTypeProp, "n2." + subTypeProp, true) + ";" EOL;
    }

    def += "  }" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  " + this->_genFreeFn(type, "n2") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_realloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "}, struct _{" + typeName + "});";
    def += "struct _{" + typeName + "} " + typeName + "_realloc (struct _{" + typeName + "} n1, struct _{" + typeName + "} n2) {" EOL;
    def += "  " + this->_genFreeFn(type, "n1") + ";" EOL;
    def += "  return n2;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_str", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{struct str} " + typeName + "_str (struct _{" + typeName + "});";
    def += "_{struct str} " + typeName + "_str (struct _{" + typeName + "} n) {" EOL;
    def += "  _{struct str} r;" EOL;

    for (const auto &subType : subTypes) {
      auto subTypeProp = "v" + this->_typeDefIdx(subType);

      def += "  if (n.t == _{" + this->_typeDef(subType) + "}) r = ";
      def += this->_genStrFn(subType, "n." + subTypeProp, true, false) + ";" EOL;
    }

    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  return typeName;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string Codegen::_wrapNode ([[maybe_unused]] const ASTNode &node, const std::string &code) {
  return code;
}

std::string Codegen::_wrapNodeExpr (const ASTNodeExpr &nodeExpr, Type *targetType, bool root, const std::string &code) {
  auto realTargetType = Type::real(targetType);
  auto realNodeExprType = Type::real(nodeExpr.type);
  auto result = code;

  if (!root && targetType->isAny() && !realNodeExprType->isAny()) {
    auto typeName = this->_typeNameAny(nodeExpr.type);
    this->_activateEntity(typeName + "_alloc");
    result = typeName + "_alloc(" + result + ")";
  } else if (!root && realTargetType->isOpt() && (!realNodeExprType->isOpt() || !realTargetType->matchStrict(realNodeExprType))) {
    auto targetTypeInfo = this->_typeInfo(realTargetType);
    auto optionalType = std::get<TypeOptional>(targetTypeInfo.type->body);

    if (Type::real(optionalType.type)->isAny() && !realNodeExprType->isAny()) {
      auto typeName = this->_typeNameAny(realNodeExprType);
      this->_activateEntity(typeName + "_alloc");
      result = typeName + "_alloc(" + result + ")";
    }

    this->_activateEntity(targetTypeInfo.typeName + "_alloc");
    result = targetTypeInfo.typeName + "_alloc(" + result + ")";
  } else if (!root && realTargetType->isUnion() && (!realNodeExprType->isUnion() || !realTargetType->matchStrict(realNodeExprType))) {
    auto typeName = this->_typeNameUnion(realTargetType);
    auto defName = this->_typeDef(nodeExpr.type);

    this->_activateEntity(defName);
    this->_activateEntity(typeName + "_alloc");
    result = typeName + "_alloc(" + defName + ", " + result + ")";
  }

  if (nodeExpr.parenthesized) {
    result = "(" + result + ")";
  }

  return result;
}
