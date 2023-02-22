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
#include "ASTChecker.hpp"
#include "config.hpp"
#include "codegen/fs.hpp"
#include "codegen/metadata.hpp"
#include "codegen/process.hpp"
#include "codegen/request.hpp"
#include "codegen/url.hpp"

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
  this->_typeNameObj(this->ast->typeMap.get("fs_Stats"), true);
  this->_typeNameObj(this->ast->typeMap.get("request_Header"), true);
  this->_typeNameObj(this->ast->typeMap.get("request_Request"), true);
  this->_typeNameObj(this->ast->typeMap.get("request_Response"), true);
  this->_typeNameObj(this->ast->typeMap.get("url_URL"), true);

  this->_apiPreLoad(codegenFs);
  this->_apiPreLoad(codegenProcess);
  this->_apiPreLoad(codegenRequest);
  this->_apiPreLoad(codegenURL);

  this->_typeNameObjDef(this->ast->typeMap.get("fs_Stats"), {}, true);
  this->_typeNameObjDef(this->ast->typeMap.get("request_Header"), {}, true);
  this->_typeNameObjDef(this->ast->typeMap.get("request_Request"), {
    std::pair<std::string, std::string>{"free", "  _{request_close}(&n);" EOL}
  }, true);
  this->_typeNameObjDef(this->ast->typeMap.get("request_Response"), {}, true);
  this->_typeNameObjDef(this->ast->typeMap.get("url_URL"), {}, true);

  this->_apiLoad(codegenFs);
  this->_apiLoad(codegenProcess);
  this->_apiLoad(codegenRequest);
  this->_apiLoad(codegenURL);

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

  if (this->builtins.varLibOpensslInit) {
    builtinVarCode += "bool lib_openssl_init = false;" EOL;
  }

  if (this->builtins.varLibWs2Init) {
    builtinVarCode += "bool lib_ws2_init = false;" EOL;
  }

  if (this->builtins.fnAlloc) {
    builtinFnDeclCode += "void *alloc (size_t);" EOL;
    builtinFnDefCode += "void *alloc (size_t l) {" EOL;
    builtinFnDefCode += "  void *r = malloc(l);" EOL;
    builtinFnDefCode += "  if (r == NULL) {" EOL;
    builtinFnDefCode += R"(    fprintf(stderr, "Error: failed to allocate %zu bytes" THE_EOL, l);)" EOL;
    builtinFnDefCode += "    exit(EXIT_FAILURE);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnReAlloc) {
    builtinFnDeclCode += "void *re_alloc (void *, size_t);" EOL;
    builtinFnDefCode += "void *re_alloc (void *d, size_t l) {" EOL;
    builtinFnDefCode += "  void *r = realloc(d, l);" EOL;
    builtinFnDefCode += "  if (r == NULL) {" EOL;
    builtinFnDefCode += R"(    fprintf(stderr, "Error: failed to reallocate %zu bytes" THE_EOL, l);)" EOL;
    builtinFnDefCode += "    exit(EXIT_FAILURE);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnAnyCopy) {
    builtinFnDeclCode += "struct any any_copy (const struct any);" EOL;
    builtinFnDefCode += "struct any any_copy (const struct any n) {" EOL;
    builtinFnDefCode += "  return n.d == NULL ? n : n._copy(n);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnAnyFree) {
    builtinFnDeclCode += "void any_free (struct any);" EOL;
    builtinFnDefCode += "void any_free (struct any n) {" EOL;
    builtinFnDefCode += "  if (n.d != NULL) n._free(n);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnAnyRealloc) {
    builtinFnDeclCode += "struct any any_realloc (struct any, struct any);" EOL;
    builtinFnDefCode += "struct any any_realloc (struct any n1, struct any n2) {" EOL;
    builtinFnDefCode += "  if (n1.d != NULL) n1._free(n1);" EOL;
    builtinFnDefCode += "  return n2;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnAnyStr) {
    builtinFnDeclCode += "struct str any_str (struct any);" EOL;
    builtinFnDefCode += "struct str any_str (struct any n) {" EOL;
    builtinFnDefCode += "  if (n.d != NULL) n._free(n);" EOL;
    builtinFnDefCode += R"(  return str_alloc("any");)" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBoolStr) {
    builtinFnDeclCode += "struct str bool_str (bool);" EOL;
    builtinFnDefCode += "struct str bool_str (bool t) {" EOL;
    builtinFnDefCode += R"(  return str_alloc(t ? "true" : "false");)" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferCopy) {
    builtinFnDeclCode += "struct buffer buffer_copy (const struct buffer);" EOL;
    builtinFnDefCode += "struct buffer buffer_copy (const struct buffer o) {" EOL;
    builtinFnDefCode += "  unsigned char *d = alloc(o.l);" EOL;
    builtinFnDefCode += "  memcpy(d, o.d, o.l);" EOL;
    builtinFnDefCode += "  return (struct buffer) {d, o.l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferEq) {
    builtinFnDeclCode += "bool buffer_eq (struct buffer, struct buffer);" EOL;
    builtinFnDefCode += "bool buffer_eq (struct buffer o1, struct buffer o2) {" EOL;
    builtinFnDefCode += "  bool r = o1.l == o2.l && memcmp(o1.d, o2.d, o1.l) == 0;" EOL;
    builtinFnDefCode += "  free(o1.d);" EOL;
    builtinFnDefCode += "  free(o2.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferFree) {
    builtinFnDeclCode += "void buffer_free (struct buffer);" EOL;
    builtinFnDefCode += "void buffer_free (struct buffer o) {" EOL;
    builtinFnDefCode += "  free(o.d);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferNe) {
    builtinFnDeclCode += "bool buffer_ne (struct buffer, struct buffer);" EOL;
    builtinFnDefCode += "bool buffer_ne (struct buffer o1, struct buffer o2) {" EOL;
    builtinFnDefCode += "  bool r = o1.l != o2.l || memcmp(o1.d, o2.d, o1.l) != 0;" EOL;
    builtinFnDefCode += "  free(o1.d);" EOL;
    builtinFnDefCode += "  free(o2.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferRealloc) {
    builtinFnDeclCode += "struct buffer buffer_realloc (struct buffer, struct buffer);" EOL;
    builtinFnDefCode += "struct buffer buffer_realloc (struct buffer o1, struct buffer o2) {" EOL;
    builtinFnDefCode += "  free(o1.d);" EOL;
    builtinFnDefCode += "  return o2;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferStr) {
    builtinFnDeclCode += "struct str buffer_str (struct buffer);" EOL;
    builtinFnDefCode += "struct str buffer_str (struct buffer b) {" EOL;
    builtinFnDefCode += "  size_t l = 8 + (b.l * 3);" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += R"(  memcpy(d, "[Buffer", 7);)" EOL;
    builtinFnDefCode += R"(  for (size_t i = 0; i < b.l; i++) sprintf(d + 7 + (i * 3), " %02x", b.d[i]);)" EOL;
    builtinFnDefCode += "  d[l - 1] = ']';" EOL;
    builtinFnDefCode += "  free(b.d);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnBufferToStr) {
    builtinFnDeclCode += "struct str buffer_to_str (struct buffer);" EOL;
    builtinFnDefCode += "struct str buffer_to_str (struct buffer b) {" EOL;
    builtinFnDefCode += "  return (struct str) {(char *) b.d, b.l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnByteStr) {
    builtinFnDeclCode += "struct str byte_str (unsigned char);" EOL;
    builtinFnDefCode += "struct str byte_str (unsigned char x) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%u", x);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharIsAlpha) {
    builtinFnDeclCode += "bool char_is_alpha (char);" EOL;
    builtinFnDefCode += "bool char_is_alpha (char c) {" EOL;
    builtinFnDefCode += "  return isalpha(c);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharIsAlphaNum) {
    builtinFnDeclCode += "bool char_is_alpha_num (char);" EOL;
    builtinFnDefCode += "bool char_is_alpha_num (char c) {" EOL;
    builtinFnDefCode += "  return isalnum(c);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharIsDigit) {
    builtinFnDeclCode += "bool char_is_digit (char);" EOL;
    builtinFnDefCode += "bool char_is_digit (char c) {" EOL;
    builtinFnDefCode += "  return isdigit(c);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharIsSpace) {
    builtinFnDeclCode += "bool char_is_space (char);" EOL;
    builtinFnDefCode += "bool char_is_space (char c) {" EOL;
    builtinFnDefCode += "  return isspace(c);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharRepeat) {
    builtinFnDeclCode += "struct str char_repeat (char, int32_t);" EOL;
    builtinFnDefCode += "struct str char_repeat (char c, int32_t k) {" EOL;
    builtinFnDefCode += R"(  if (k <= 0) return str_alloc("");)" EOL;
    builtinFnDefCode += "  size_t l = (size_t) k;" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  size_t i = 0;" EOL;
    builtinFnDefCode += "  while (i < l) d[i++] = c;" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharStr) {
    builtinFnDeclCode += "struct str char_str (char);" EOL;
    builtinFnDefCode += "struct str char_str (char c) {" EOL;
    builtinFnDefCode += "  char buf[2] = {c, '\\0'};" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrConcatStr) {
    builtinFnDeclCode += "struct str cstr_concat_str (const char *, struct str);" EOL;
    builtinFnDefCode += "struct str cstr_concat_str (const char *r, struct str s) {" EOL;
    builtinFnDefCode += "  size_t l = s.l + strlen(r);" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(d, r, l - s.l);" EOL;
    builtinFnDefCode += "  memcpy(&d[l - s.l], s.d, s.l);" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrEqCstr) {
    builtinFnDeclCode += "bool cstr_eq_cstr (const char *, const char *);" EOL;
    builtinFnDefCode += "bool cstr_eq_cstr (const char *c1, const char *c2) {" EOL;
    builtinFnDefCode += "  size_t l = strlen(c1);" EOL;
    builtinFnDefCode += "  return l == strlen(c2) && memcmp(c1, c2, l) == 0;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrEqStr) {
    builtinFnDeclCode += "bool cstr_eq_str (const char *, struct str);" EOL;
    builtinFnDefCode += "bool cstr_eq_str (const char *c, struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l == strlen(c) && memcmp(s.d, c, s.l) == 0;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrNeCstr) {
    builtinFnDeclCode += "bool cstr_ne_cstr (const char *, const char *);" EOL;
    builtinFnDefCode += "bool cstr_ne_cstr (const char *c1, const char *c2) {" EOL;
    builtinFnDefCode += "  size_t l = strlen(c1);" EOL;
    builtinFnDefCode += "  return l != strlen(c2) || memcmp(c1, c2, l) != 0;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrNeStr) {
    builtinFnDeclCode += "bool cstr_ne_str (const char *, struct str);" EOL;
    builtinFnDefCode += "bool cstr_ne_str (const char *d, struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l != strlen(d) || memcmp(s.d, d, s.l) != 0;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnEnumStr) {
    builtinFnDeclCode += "struct str enum_str (int);" EOL;
    builtinFnDefCode += "struct str enum_str (int d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%d", d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnF32Str) {
    builtinFnDeclCode += "struct str f32_str (float);" EOL;
    builtinFnDefCode += "struct str f32_str (float f) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%f", f);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnF64Str) {
    builtinFnDeclCode += "struct str f64_str (double);" EOL;
    builtinFnDefCode += "struct str f64_str (double f) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%f", f);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnFloatStr) {
    builtinFnDeclCode += "struct str float_str (double);" EOL;
    builtinFnDefCode += "struct str float_str (double f) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%f", f);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI8Str) {
    builtinFnDeclCode += "struct str i8_str (int8_t);" EOL;
    builtinFnDefCode += "struct str i8_str (int8_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId8, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI16Str) {
    builtinFnDeclCode += "struct str i16_str (int16_t);" EOL;
    builtinFnDefCode += "struct str i16_str (int16_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId16, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI32Str) {
    builtinFnDeclCode += "struct str i32_str (int32_t);" EOL;
    builtinFnDefCode += "struct str i32_str (int32_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId32, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI64Str) {
    builtinFnDeclCode += "struct str i64_str (int64_t);" EOL;
    builtinFnDefCode += "struct str i64_str (int64_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId64, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnIntStr) {
    builtinFnDeclCode += "struct str int_str (int32_t);" EOL;
    builtinFnDefCode += "struct str int_str (int32_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId32, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnOSName) {
    builtinFnDeclCode += "struct str os_name ();" EOL;
    builtinFnDefCode += "struct str os_name () {" EOL;
    builtinFnDefCode += "  #ifdef THE_OS_WINDOWS" EOL;
    builtinFnDefCode += R"(    return str_alloc("Windows");)" EOL;
    builtinFnDefCode += "  #else" EOL;
    builtinFnDefCode += "    struct utsname buf;" EOL;
    builtinFnDefCode += "    if (uname(&buf) < 0) {" EOL;
    builtinFnDefCode += R"(      fprintf(stderr, "Error: failed to retrieve uname information" THE_EOL);)" EOL;
    builtinFnDefCode += "      exit(EXIT_FAILURE);" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += R"(    if (strcmp(buf.sysname, "Darwin") == 0) return str_alloc("macOS");)" EOL;
    builtinFnDefCode += "    return str_alloc(buf.sysname);" EOL;
    builtinFnDefCode += "  #endif" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnPathBasename) {
    builtinFnDeclCode += "struct str path_basename (struct str);" EOL;
    builtinFnDefCode += "struct str path_basename (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l == 0) return s;" EOL;
    builtinFnDefCode += "  #ifdef THE_OS_WINDOWS" EOL;
    builtinFnDefCode += R"(    char e = '\\';)" EOL;
    builtinFnDefCode += "  #else" EOL;
    builtinFnDefCode += "    char e = '/';" EOL;
    builtinFnDefCode += "  #endif" EOL;
    builtinFnDefCode += "  size_t a = 0;" EOL;
    builtinFnDefCode += "  size_t b = 0;" EOL;
    builtinFnDefCode += "  unsigned char k = 0;" EOL;
    builtinFnDefCode += "  for (size_t i = s.l - 1;; i--) {" EOL;
    builtinFnDefCode += "    if (k == 1 && s.d[i] == e) {" EOL;
    builtinFnDefCode += "      a = i + 1;" EOL;
    builtinFnDefCode += "      break;" EOL;
    builtinFnDefCode += "    } else if (k == 0 && s.d[i] != e) {" EOL;
    builtinFnDefCode += "      k = 1;" EOL;
    builtinFnDefCode += "      b = i + 1;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    if (i == 0) break;" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  if (k == 0) {" EOL;
    builtinFnDefCode += "    str_free(s);" EOL;
    builtinFnDefCode += R"(    return str_alloc("");)" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  size_t l = b - a;" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(d, &s.d[a], l);" EOL;
    builtinFnDefCode += "  str_free(s);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnPathDirname) {
    builtinFnDeclCode += "struct str path_dirname (struct str);" EOL;
    builtinFnDefCode += "struct str path_dirname (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l == 0) {" EOL;
    builtinFnDefCode += "    str_free(s);" EOL;
    builtinFnDefCode += R"(    return str_alloc(".");)" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  #ifdef THE_OS_WINDOWS" EOL;
    builtinFnDefCode += R"(    char e = '\\';)" EOL;
    builtinFnDefCode += "  #else" EOL;
    builtinFnDefCode += "    char e = '/';" EOL;
    builtinFnDefCode += "  #endif" EOL;
    builtinFnDefCode += "  size_t l = 0;" EOL;
    builtinFnDefCode += "  unsigned char k = 0;" EOL;
    builtinFnDefCode += "  for (size_t i = s.l - 1;; i--) {" EOL;
    builtinFnDefCode += "    if (k == 1 && s.d[i] == e) {" EOL;
    builtinFnDefCode += "      l = i;" EOL;
    builtinFnDefCode += "      break;" EOL;
    builtinFnDefCode += "    } else if (k == 0 && s.d[i] != e) {" EOL;
    builtinFnDefCode += "      k = 1;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    if (i == 0) break;" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  if (l == 0) {" EOL;
    builtinFnDefCode += "    s.d = re_alloc(s.d, 1);" EOL;
    builtinFnDefCode += "    if (s.d[0] != e) s.d[0] = '.';" EOL;
    builtinFnDefCode += "    return (struct str) {s.d, 1};" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(d, s.d, l);" EOL;
    builtinFnDefCode += "  str_free(s);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnPrint) {
    builtinFnDeclCode += "void print (FILE *, const char *, ...);" EOL;
    builtinFnDefCode += "void print (FILE *stream, const char *fmt, ...) {" EOL;
    builtinFnDefCode += "  va_list args;" EOL;
    builtinFnDefCode += "  va_start(args, fmt);" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += "  struct str s;" EOL;
    builtinFnDefCode += "  while (*fmt) {" EOL;
    builtinFnDefCode += "    switch (*fmt++) {" EOL;
    builtinFnDefCode += R"(      case 't': fputs(va_arg(args, int) ? "true" : "false", stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'b': sprintf(buf, "%u", va_arg(args, unsigned)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'c': fputc(va_arg(args, int), stream); break;" EOL;
    builtinFnDefCode += "      case 'e':" EOL;
    builtinFnDefCode += "      case 'f':" EOL;
    builtinFnDefCode += R"(      case 'g': snprintf(buf, 512, "%f", va_arg(args, double)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'h':" EOL;
    builtinFnDefCode += "      case 'j':" EOL;
    builtinFnDefCode += "      case 'v':" EOL;
    builtinFnDefCode += R"(      case 'w': sprintf(buf, "%d", va_arg(args, int)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'i':" EOL;
    builtinFnDefCode += R"(      case 'k': sprintf(buf, "%" PRId32, va_arg(args, int32_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'l': sprintf(buf, "%" PRId64, va_arg(args, int64_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'p': sprintf(buf, "%p", va_arg(args, void *)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 's': s = va_arg(args, struct str); fwrite(s.d, 1, s.l, stream); str_free(s); break;" EOL;
    builtinFnDefCode += R"(      case 'u': sprintf(buf, "%" PRIu32, va_arg(args, uint32_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'y': sprintf(buf, "%" PRIu64, va_arg(args, uint64_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'z': fputs(va_arg(args, char *), stream); break;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  va_end(args);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnSleepSync) {
    builtinFnDeclCode += "void sleep_sync (int32_t);" EOL;
    builtinFnDefCode += "void sleep_sync (int32_t i) {" EOL;
    builtinFnDefCode += "  #ifdef THE_OS_WINDOWS" EOL;
    builtinFnDefCode += "    Sleep((unsigned int) i);" EOL;
    builtinFnDefCode += "  #else" EOL;
    builtinFnDefCode += "    usleep((unsigned int) (i * 1000));" EOL;
    builtinFnDefCode += "  #endif" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrAlloc) {
    builtinFnDeclCode += "struct str str_alloc (const char *);" EOL;
    builtinFnDefCode += "struct str str_alloc (const char *r) {" EOL;
    builtinFnDefCode += "  size_t l = strlen(r);" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(d, r, l);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrAt) {
    builtinFnDeclCode += "char *str_at (struct str, int64_t);" EOL;
    builtinFnDefCode += "char *str_at (struct str s, int64_t i) {" EOL;
    builtinFnDefCode += "  if ((i >= 0 && i >= s.l) || (i < 0 && i < -((int64_t) s.l))) {" EOL;
    builtinFnDefCode += R"(    fprintf(stderr, "Error: index %" PRId64 " out of string bounds" THE_EOL, i);)" EOL;
    builtinFnDefCode += "    exit(EXIT_FAILURE);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return i < 0 ? &s.d[s.l + i] : &s.d[i];" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrConcatCstr) {
    builtinFnDeclCode += "struct str str_concat_cstr (struct str, const char *);" EOL;
    builtinFnDefCode += "struct str str_concat_cstr (struct str s, const char *r) {" EOL;
    builtinFnDefCode += "  size_t l = s.l + strlen(r);" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(d, s.d, s.l);" EOL;
    builtinFnDefCode += "  memcpy(&d[s.l], r, l - s.l);" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrConcatStr) {
    builtinFnDeclCode += "struct str str_concat_str (struct str, struct str);" EOL;
    builtinFnDefCode += "struct str str_concat_str (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  size_t l = s1.l + s2.l;" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(d, s1.d, s1.l);" EOL;
    builtinFnDefCode += "  memcpy(&d[s1.l], s2.d, s2.l);" EOL;
    builtinFnDefCode += "  free(s1.d);" EOL;
    builtinFnDefCode += "  free(s2.d);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrCopy) {
    builtinFnDeclCode += "struct str str_copy (const struct str);" EOL;
    builtinFnDefCode += "struct str str_copy (const struct str s) {" EOL;
    builtinFnDefCode += "  char *d = alloc(s.l);" EOL;
    builtinFnDefCode += "  memcpy(d, s.d, s.l);" EOL;
    builtinFnDefCode += "  return (struct str) {d, s.l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrCstr) {
    builtinFnDeclCode += "char *str_cstr (const struct str);" EOL;
    builtinFnDefCode += "char *str_cstr (const struct str s) {" EOL;
    builtinFnDefCode += "  char *d = alloc(s.l + 1);" EOL;
    builtinFnDefCode += "  memcpy(d, s.d, s.l);" EOL;
    builtinFnDefCode += "  d[s.l] = '\\0';" EOL;
    builtinFnDefCode += "  return d;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEmpty) {
    builtinFnDeclCode += "bool str_empty (struct str);" EOL;
    builtinFnDefCode += "bool str_empty (struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l == 0;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEqCstr) {
    builtinFnDeclCode += "bool str_eq_cstr (struct str, const char *);" EOL;
    builtinFnDefCode += "bool str_eq_cstr (struct str s, const char *r) {" EOL;
    builtinFnDefCode += "  bool d = s.l == strlen(r) && memcmp(s.d, r, s.l) == 0;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return d;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEqStr) {
    builtinFnDeclCode += "bool str_eq_str (struct str, struct str);" EOL;
    builtinFnDefCode += "bool str_eq_str (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  bool r = s1.l == s2.l && memcmp(s1.d, s2.d, s1.l) == 0;" EOL;
    builtinFnDefCode += "  free(s1.d);" EOL;
    builtinFnDefCode += "  free(s2.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEscape) {
    builtinFnDeclCode += "struct str str_escape (const struct str);" EOL;
    builtinFnDefCode += "struct str str_escape (const struct str s) {" EOL;
    builtinFnDefCode += "  char *d = alloc(s.l);" EOL;
    builtinFnDefCode += "  size_t l = 0;" EOL;
    builtinFnDefCode += "  for (size_t i = 0; i < s.l; i++) {" EOL;
    builtinFnDefCode += "    char c = s.d[i];" EOL;
    builtinFnDefCode += R"(    if (c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '"') {)" EOL;
    builtinFnDefCode += "      if (l + 2 > s.l) d = re_alloc(d, l + 2);" EOL;
    builtinFnDefCode += R"(      d[l++] = '\\';)" EOL;
    builtinFnDefCode += R"(      if (c == '\f') d[l++] = 'f';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\n') d[l++] = 'n';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\r') d[l++] = 'r';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\t') d[l++] = 't';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\v') d[l++] = 'v';)" EOL;
    builtinFnDefCode += R"(      else if (c == '"') d[l++] = '"';)" EOL;
    builtinFnDefCode += "      continue;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    if (l + 1 > s.l) d = re_alloc(d, l + 1);" EOL;
    builtinFnDefCode += "    d[l++] = c;" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrFind) {
    builtinFnDeclCode += "int32_t str_find (struct str, struct str);" EOL;
    builtinFnDefCode += "int32_t str_find (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  int32_t r = -1;" EOL;
    builtinFnDefCode += "  if (s1.l == s2.l) {" EOL;
    builtinFnDefCode += "    r = memcmp(s1.d, s2.d, s1.l) == 0 ? 0 : -1;" EOL;
    builtinFnDefCode += "  } else if (s1.l > s2.l) {" EOL;
    builtinFnDefCode += "    for (size_t i = 0; i <= s1.l - s2.l; i++) {" EOL;
    builtinFnDefCode += "      if (memcmp(&s1.d[i], s2.d, s2.l) == 0) {" EOL;
    builtinFnDefCode += "        r = (int32_t) i;" EOL;
    builtinFnDefCode += "        break;" EOL;
    builtinFnDefCode += "      }" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  free(s1.d);" EOL;
    builtinFnDefCode += "  free(s2.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrFree) {
    builtinFnDeclCode += "void str_free (struct str);" EOL;
    builtinFnDefCode += "void str_free (struct str s) {" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrLen) {
    builtinFnDeclCode += "size_t str_len (struct str);" EOL;
    builtinFnDefCode += "size_t str_len (struct str s) {" EOL;
    builtinFnDefCode += "  size_t l = s.l;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return l;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrLines) {
    builtinFnDeclCode += "struct " + Codegen::typeName("array_str") + " str_lines (struct str, unsigned char, bool);" EOL;
    builtinFnDefCode += "struct " + Codegen::typeName("array_str") + " str_lines (struct str s, unsigned char o1, bool n1) {" EOL;
    builtinFnDefCode += "  bool k = o1 == 0 ? false : n1;" EOL;
    builtinFnDefCode += "  struct str *r = NULL;" EOL;
    builtinFnDefCode += "  size_t l = 0;" EOL;
    builtinFnDefCode += "  if (s.l != 0) {" EOL;
    builtinFnDefCode += "    char *d = alloc(s.l);" EOL;
    builtinFnDefCode += "    size_t i = 0;" EOL;
    builtinFnDefCode += "    for (size_t j = 0; j < s.l; j++) {" EOL;
    builtinFnDefCode += "      char c = s.d[j];" EOL;
    builtinFnDefCode += R"(      if (c == '\r' || c == '\n') {)" EOL;
    builtinFnDefCode += "        if (k) d[i++] = c;" EOL;
    builtinFnDefCode += R"(        if (c == '\r' && j + 1 < s.l && s.d[j + 1] == '\n') {)" EOL;
    builtinFnDefCode += "          j++;" EOL;
    builtinFnDefCode += "          if (k) d[i++] = s.d[j];" EOL;
    builtinFnDefCode += "        }" EOL;
    builtinFnDefCode += "        char *a = alloc(i);" EOL;
    builtinFnDefCode += "        memcpy(a, d, i);" EOL;
    builtinFnDefCode += "        r = re_alloc(r, ++l * sizeof(struct str));" EOL;
    builtinFnDefCode += "        r[l - 1] = (struct str) {a, i};" EOL;
    builtinFnDefCode += "        i = 0;" EOL;
    builtinFnDefCode += "      } else {" EOL;
    builtinFnDefCode += "        d[i++] = c;" EOL;
    builtinFnDefCode += "      }" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    if (i != 0) {" EOL;
    builtinFnDefCode += "      char *a = alloc(i);" EOL;
    builtinFnDefCode += "      memcpy(a, d, i);" EOL;
    builtinFnDefCode += "      r = re_alloc(r, ++l * sizeof(struct str));" EOL;
    builtinFnDefCode += "      r[l - 1] = (struct str) {a, i};" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    free(d);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return (struct " + Codegen::typeName("array_str") + ") {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrLower) {
    builtinFnDeclCode += "struct str str_lower (struct str);" EOL;
    builtinFnDefCode += "struct str str_lower (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l != 0) {" EOL;
    builtinFnDefCode += "    for (size_t i = 0; i < s.l; i++) s.d[i] = tolower(s.d[i]);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return s;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrLowerFirst) {
    builtinFnDeclCode += "struct str str_lower_first (struct str);" EOL;
    builtinFnDefCode += "struct str str_lower_first (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l != 0) s.d[0] = tolower(s.d[0]);" EOL;
    builtinFnDefCode += "  return s;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrNeCstr) {
    builtinFnDeclCode += "bool str_ne_cstr (struct str, const char *);" EOL;
    builtinFnDefCode += "bool str_ne_cstr (struct str s, const char *c) {" EOL;
    builtinFnDefCode += "  bool r = s.l != strlen(c) || memcmp(s.d, c, s.l) != 0;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrNeStr) {
    builtinFnDeclCode += "bool str_ne_str (struct str, struct str);" EOL;
    builtinFnDefCode += "bool str_ne_str (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  bool r = s1.l != s2.l || memcmp(s1.d, s2.d, s1.l) != 0;" EOL;
    builtinFnDefCode += "  free(s1.d);" EOL;
    builtinFnDefCode += "  free(s2.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrNot) {
    builtinFnDeclCode += "bool str_not (struct str);" EOL;
    builtinFnDefCode += "bool str_not (struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l == 0;" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrRealloc) {
    builtinFnDeclCode += "struct str str_realloc (struct str, struct str);" EOL;
    builtinFnDefCode += "struct str str_realloc (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  free(s1.d);" EOL;
    builtinFnDefCode += "  return s2;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrSlice) {
    builtinFnDeclCode += "struct str str_slice (struct str, unsigned char, int64_t, unsigned char, int64_t);" EOL;
    builtinFnDefCode += "struct str str_slice (struct str s, unsigned char o1, int64_t n1, unsigned char o2, int64_t n2) {" EOL;
    builtinFnDefCode += "  int64_t i1 = o1 == 0 ? 0 : (int64_t) (n1 < 0 ? (n1 < -((int64_t) s.l) ? 0 : n1 + s.l) ";
    builtinFnDefCode += ": (n1 > s.l ? s.l : n1));" EOL;
    builtinFnDefCode += "  int64_t i2 = o2 == 0 ? (int64_t) s.l : (int64_t) (n2 < 0 ? (n2 < -((int64_t) s.l) ? 0 ";
    builtinFnDefCode += ": n2 + s.l) : (n2 > s.l ? s.l : n2));" EOL;
    builtinFnDefCode += "  if (i1 >= i2 || i1 >= s.l) {" EOL;
    builtinFnDefCode += "    free(s.d);" EOL;
    builtinFnDefCode += R"(    return str_alloc("");)" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  size_t l = i2 - i1;" EOL;
    builtinFnDefCode += "  char *d = alloc(l);" EOL;
    builtinFnDefCode += "  for (size_t i = 0; i1 < i2; i1++) d[i++] = s.d[i1];" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrToBuffer) {
    builtinFnDeclCode += "struct buffer str_to_buffer (struct str);" EOL;
    builtinFnDefCode += "struct buffer str_to_buffer (struct str s) {" EOL;
    builtinFnDefCode += "  return (struct buffer) {(unsigned char *) s.d, s.l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrTrim) {
    builtinFnDeclCode += "struct str str_trim (struct str);" EOL;
    builtinFnDefCode += "struct str str_trim (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l == 0) return s;" EOL;
    builtinFnDefCode += "  size_t i = 0;" EOL;
    builtinFnDefCode += "  size_t j = s.l;" EOL;
    builtinFnDefCode += "  while (i < s.l && isspace(s.d[i])) i++;" EOL;
    builtinFnDefCode += "  while (j >= 0 && isspace(s.d[j - 1])) {" EOL;
    builtinFnDefCode += "    j--;" EOL;
    builtinFnDefCode += "    if (j == 0) break;" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  if (i >= j) {" EOL;
    builtinFnDefCode += "    free(s.d);" EOL;
    builtinFnDefCode += R"(    return str_alloc("");)" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  size_t l = j - i;" EOL;
    builtinFnDefCode += "  char *r = alloc(l);" EOL;
    builtinFnDefCode += "  for (size_t k = 0; k < l;) r[k++] = s.d[i++];" EOL;
    builtinFnDefCode += "  free(s.d);" EOL;
    builtinFnDefCode += "  return (struct str) {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrUpper) {
    builtinFnDeclCode += "struct str str_upper (struct str);" EOL;
    builtinFnDefCode += "struct str str_upper (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l != 0) {" EOL;
    builtinFnDefCode += "    for (size_t i = 0; i < s.l; i++) s.d[i] = (char) toupper(s.d[i]);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return s;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrUpperFirst) {
    builtinFnDeclCode += "struct str str_upper_first (struct str);" EOL;
    builtinFnDefCode += "struct str str_upper_first (struct str s) {" EOL;
    builtinFnDefCode += "  if (s.l != 0) s.d[0] = (char) toupper(s.d[0]);" EOL;
    builtinFnDefCode += "  return s;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU8Str) {
    builtinFnDeclCode += "struct str u8_str (uint8_t);" EOL;
    builtinFnDefCode += "struct str u8_str (uint8_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu8, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU16Str) {
    builtinFnDeclCode += "struct str u16_str (uint16_t);" EOL;
    builtinFnDefCode += "struct str u16_str (uint16_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu16, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU32Str) {
    builtinFnDeclCode += "struct str u32_str (uint32_t);" EOL;
    builtinFnDefCode += "struct str u32_str (uint32_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu32, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU64Str) {
    builtinFnDeclCode += "struct str u64_str (uint64_t);" EOL;
    builtinFnDefCode += "struct str u64_str (uint64_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu64, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  for (const auto &[_, item] : this->api) {
    if (!item.enabled) {
      continue;
    }

    builtinFnDeclCode += item.decl;
    builtinFnDefCode += item.def;
  }

  fnDeclCode = builtinFnDeclCode + fnDeclCode;
  fnDefCode = builtinFnDefCode + fnDefCode;

  builtinDefineCode += builtinDefineCode.empty() ? "" : EOL;
  defineCode += defineCode.empty() ? "" : EOL;
  builtinVarCode += builtinVarCode.empty() ? "" : EOL;
  builtinStructDefCode += builtinStructDefCode.empty() ? "" : EOL;
  enumDeclCode += enumDeclCode.empty() ? "" : EOL;
  structDeclCode += structDeclCode.empty() ? "" : EOL;
  structDefCode += structDefCode.empty() ? "" : EOL;
  fnDeclCode += fnDeclCode.empty() ? "" : EOL;
  fnDefCode += fnDefCode.empty() ? "" : EOL;

  auto headers = std::string(this->builtins.libCtype ? "#include <ctype.h>" EOL : "");
  headers += this->builtins.libInttypes ? "#include <inttypes.h>" EOL : "";
  headers += this->builtins.libOpensslSsl ? "#include <openssl/ssl.h>" EOL : "";
  headers += this->builtins.libStdarg ? "#include <stdarg.h>" EOL : "";
  headers += this->builtins.libStdbool ? "#include <stdbool.h>" EOL : "";
  headers += this->builtins.libStddef && !this->builtins.libStdlib ? "#include <stddef.h>" EOL : "";
  headers += this->builtins.libStdint && !this->builtins.libInttypes ? "#include <stdint.h>" EOL : "";
  headers += this->builtins.libStdio ? "#include <stdio.h>" EOL : "";
  headers += this->builtins.libStdlib ? "#include <stdlib.h>" EOL : "";
  headers += this->builtins.libString ? "#include <string.h>" EOL : "";

  if (this->builtins.libSysTypes && !this->builtins.libSysSocket && !this->builtins.libSysStat && !this->builtins.libSysUtsname) {
    headers += this->builtins.libSysTypes ? "#include <sys/types.h>" EOL : "";
  }

  headers += this->builtins.libSysStat ? "#include <sys/stat.h>" EOL : "";

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
    this->builtins.libDirent ||
    this->builtins.libNetdb ||
    this->builtins.libNetinetIn ||
    this->builtins.libSysSocket ||
    this->builtins.libSysUtsname ||
    this->builtins.libUnistd
  ) {
    headers += "#ifndef THE_OS_WINDOWS" EOL;
    headers += this->builtins.libArpaInet ? "  #include <arpa/inet.h>" EOL : "";
    headers += this->builtins.libDirent ? "  #include <dirent.h>" EOL : "";
    headers += this->builtins.libNetdb ? "  #include <netdb.h>" EOL : "";
    headers += this->builtins.libNetinetIn ? "  #include <netinet/in.h>" EOL : "";
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
  } else if (name == "libArpaInet") {
    this->builtins.libArpaInet = true;
  } else if (name == "libCtype") {
    this->builtins.libCtype = true;
  } else if (name == "libDirent") {
    this->builtins.libDirent = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libInttypes") {
    this->builtins.libInttypes = true;
  } else if (name == "libNetdb") {
    this->builtins.libNetdb = true;
  } else if (name == "libNetinetIn") {
    this->builtins.libNetinetIn = true;
  } else if (name == "libOpensslSsl") {
    this->builtins.libOpensslSsl = true;

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lssl") == this->flags.end()) {
      this->flags.emplace_back("A:-lssl");
      this->flags.emplace_back("A:-lcrypto");
      this->flags.emplace_back("W:-lws2_32");
      this->flags.emplace_back("W:-lgdi32");
      this->flags.emplace_back("W:-ladvapi32");
      this->flags.emplace_back("W:-lcrypt32");
      this->flags.emplace_back("W:-luser32");
    }
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
  } else if (name == "fnAlloc") {
    this->builtins.fnAlloc = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");
  } else if (name == "fnAnyCopy") {
    this->builtins.fnAnyCopy = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeAny");
  } else if (name == "fnAnyFree") {
    this->builtins.fnAnyFree = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeAny");
  } else if (name == "fnAnyRealloc") {
    this->builtins.fnAnyRealloc = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeAny");
  } else if (name == "fnAnyStr") {
    this->builtins.fnAnyStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeAny");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnBoolStr") {
    this->builtins.fnBoolStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnBufferCopy") {
    this->builtins.fnBufferCopy = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeBuffer");
  } else if (name == "fnBufferEq") {
    this->builtins.fnBufferEq = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeBuffer");
  } else if (name == "fnBufferFree") {
    this->builtins.fnBufferFree = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeBuffer");
  } else if (name == "fnBufferNe") {
    this->builtins.fnBufferNe = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeBuffer");
  } else if (name == "fnBufferRealloc") {
    this->builtins.fnBufferRealloc = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeBuffer");
  } else if (name == "fnBufferStr") {
    this->builtins.fnBufferStr = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeBuffer");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnBufferToStr") {
    this->builtins.fnBufferToStr = true;
    this->_activateBuiltin("typeStr");
  } else if (name == "fnByteStr") {
    this->builtins.fnByteStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnCharIsAlpha") {
    this->builtins.fnCharIsAlpha = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libCtype");
  } else if (name == "fnCharIsAlphaNum") {
    this->builtins.fnCharIsAlphaNum = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libCtype");
  } else if (name == "fnCharIsDigit") {
    this->builtins.fnCharIsDigit = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libCtype");
  } else if (name == "fnCharIsSpace") {
    this->builtins.fnCharIsSpace = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libCtype");
  } else if (name == "fnCharRepeat") {
    this->builtins.fnCharRepeat = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdint");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnCharStr") {
    this->builtins.fnCharStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnCstrConcatStr") {
    this->builtins.fnCstrConcatStr = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnCstrEqCstr") {
    this->builtins.fnCstrEqCstr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libString");
  } else if (name == "fnCstrEqStr") {
    this->builtins.fnCstrEqStr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnCstrNeCstr") {
    this->builtins.fnCstrNeCstr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libString");
  } else if (name == "fnCstrNeStr") {
    this->builtins.fnCstrNeStr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnEnumStr") {
    this->builtins.fnEnumStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnF32Str") {
    this->builtins.fnF32Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnF64Str") {
    this->builtins.fnF64Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnFloatStr") {
    this->builtins.fnFloatStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnI8Str") {
    this->builtins.fnI8Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnI16Str") {
    this->builtins.fnI16Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnI32Str") {
    this->builtins.fnI32Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnI64Str") {
    this->builtins.fnI64Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnIntStr") {
    this->builtins.fnIntStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnOSName") {
    this->builtins.fnOSName = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("libSysUtsname");
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnPathBasename") {
    this->builtins.fnPathBasename = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("fnStrFree");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnPathDirname") {
    this->builtins.fnPathDirname = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnReAlloc");
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("fnStrFree");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnPrint") {
    this->builtins.fnPrint = true;
    this->_activateBuiltin("fnStrFree");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdarg");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnReAlloc") {
    this->builtins.fnReAlloc = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");
  } else if (name == "fnSleepSync") {
    this->builtins.fnSleepSync = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libStdint");
    this->_activateBuiltin("libUnistd");
    this->_activateBuiltin("libWindows");
  } else if (name == "fnStrAlloc") {
    this->builtins.fnStrAlloc = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrAt") {
    this->builtins.fnStrAt = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");
  } else if (name == "fnStrConcatCstr") {
    this->builtins.fnStrConcatCstr = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrConcatStr") {
    this->builtins.fnStrConcatStr = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrCopy") {
    this->builtins.fnStrCopy = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrCstr") {
    this->builtins.fnStrCstr = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrEmpty") {
    this->builtins.fnStrEmpty = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrEqCstr") {
    this->builtins.fnStrEqCstr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrEqStr") {
    this->builtins.fnStrEqStr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrEscape") {
    this->builtins.fnStrEscape = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnReAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrFind") {
    this->builtins.fnStrFind = true;
    this->_activateBuiltin("libStdint");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrFree") {
    this->builtins.fnStrFree = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrLen") {
    this->builtins.fnStrLen = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrLines") {
    this->builtins.fnStrLines = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnReAlloc");
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    auto typeInfo = this->_typeInfo(this->ast->typeMap.arrayOf(this->ast->typeMap.get("str")));
    this->_activateEntity(typeInfo.typeName);
  } else if (name == "fnStrLower") {
    this->builtins.fnStrLower = true;
    this->_activateBuiltin("libCtype");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrLowerFirst") {
    this->builtins.fnStrLowerFirst = true;
    this->_activateBuiltin("libCtype");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrNeCstr") {
    this->builtins.fnStrNeCstr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrNeStr") {
    this->builtins.fnStrNeStr = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrNot") {
    this->builtins.fnStrNot = true;
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrRealloc") {
    this->builtins.fnStrRealloc = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrSlice") {
    this->builtins.fnStrSlice = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdint");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrToBuffer") {
    this->builtins.fnStrToBuffer = true;
    this->_activateBuiltin("typeBuffer");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrTrim") {
    this->builtins.fnStrTrim = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libCtype");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrUpper") {
    this->builtins.fnStrUpper = true;
    this->_activateBuiltin("libCtype");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrUpperFirst") {
    this->builtins.fnStrUpperFirst = true;
    this->_activateBuiltin("libCtype");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnU8Str") {
    this->builtins.fnU8Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnU16Str") {
    this->builtins.fnU16Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnU32Str") {
    this->builtins.fnU32Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnU64Str") {
    this->builtins.fnU64Str = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
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
std::string Codegen::_apiEval (const std::string &code, int limit, const std::optional<std::set<std::string> *> &dependencies) {
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
      if (codegenMetadata.contains(name)) {
        for (const auto &dependency : codegenMetadata.at(name)) {
          if (dependencies == std::nullopt) {
            this->_activateBuiltin(dependency);
          } else if (!(*dependencies)->contains(dependency)) {
            (*dependencies)->emplace(dependency);
          }
        }
      } else if (this->api.contains(name)) {
        if (dependencies == std::nullopt) {
          this->_activateBuiltin(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (name.starts_with("array_")) {
        if (name.ends_with("_free")) {
          name = this->_typeNameArray(this->ast->typeMap.arrayOf(this->ast->typeMap.get(name.substr(6, name.size() - 11)))) + "_free";
        } else {
          name = this->_typeNameArray(this->ast->typeMap.arrayOf(this->ast->typeMap.get(name.substr(6))));
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else {
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

void Codegen::_apiLoad (const std::vector<std::string> &items) {
  for (const auto &item : items) {
    auto firstLine = std::string();

    for (auto i = static_cast<std::size_t>(0); i < item.size(); i++) {
      auto ch = item[i];

      if (ch == '\n' || (ch == '\r' && i + 1 < item.size() && item[i + 1] == '\n')) {
        break;
      } else {
        firstLine += ch;
      }
    }

    firstLine.erase(firstLine.size() - 2, 2);

    auto name = std::string();
    auto isName = false;
    auto isParamName = false;

    for (auto i = firstLine.size() - 1;; i--) {
      auto ch = firstLine[i];

      if (ch == ')' || ch == ',') {
        if (i != 0 && firstLine[i - 1] != '(') {
          isParamName = true;
        }
      } else if (ch == ' ' && i + 1 < firstLine.size() && firstLine[i + 1] == '(') {
        isParamName = false;
        isName = true;
      } else if (isParamName && (ch == '*' || ch == '&' || ch == ' ')) {
        isParamName = false;
      } else if (isName && (ch == ' ' || ch == '*' || ch == '&')) {
        isName = false;
      } else if (isName) {
        name.insert(0, 1, ch);
      }

      if (i == 0) {
        break;
      }
    }

    this->api[name].def = this->_apiEval(item, 0, &this->api[name].dependencies);
  }
}

void Codegen::_apiPreLoad (const std::vector<std::string> &items) {
  for (const auto &item : items) {
    auto firstLine = std::string();

    for (auto i = static_cast<std::size_t>(0); i < item.size(); i++) {
      auto ch = item[i];

      if (ch == '\n' || (ch == '\r' && i + 1 < item.size() && item[i + 1] == '\n')) {
        break;
      } else {
        firstLine += ch;
      }
    }

    firstLine.erase(firstLine.size() - 2, 2);

    auto apiItem = CodegenApiItem{};
    auto isName = false;
    auto isParamName = false;

    for (auto i = firstLine.size() - 1;; i--) {
      auto ch = firstLine[i];

      if (ch == ')' || ch == ',') {
        apiItem.decl.insert(0, 1, ch);

        if (i != 0 && firstLine[i - 1] != '(') {
          isParamName = true;
        }
      } else if (ch == ' ' && i + 1 < firstLine.size() && firstLine[i + 1] == '(') {
        apiItem.decl.insert(0, 1, ch);
        isParamName = false;
        isName = true;
      } else if (isParamName && ch == ' ') {
        isParamName = false;
      } else if (isParamName && (ch == '*' || ch == '&')) {
        isParamName = false;
        apiItem.decl.insert(0, 1, ch);
      } else if (isName && (ch == ' ' || ch == '*' || ch == '&')) {
        apiItem.decl.insert(0, 1, ch);
        isName = false;
      } else if (isName) {
        apiItem.name.insert(0, 1, ch);
        apiItem.decl.insert(0, 1, ch);
      } else if (!isParamName) {
        apiItem.decl.insert(0, 1, ch);
      }

      if (i == 0) {
        break;
      }
    }

    apiItem.decl = this->_apiEval(apiItem.decl, 0, &apiItem.dependencies) + ";" EOL;
    this->api[apiItem.name] = apiItem;
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
  auto bodyTypeCasts = std::map<std::string, Type *>{};
  auto altTypeCasts = std::map<std::string, Type *>{};

  if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) {
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

  return std::make_tuple(bodyTypeCasts, altTypeCasts);
}

std::string Codegen::_exprCallDefaultArg (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    this->_activateBuiltin("typeAny");
    return "(struct any) {}";
  } else if (
    typeInfo.type->isArray() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isUnion()
  ) {
    this->_activateEntity(typeInfo.typeName);
    return "(struct " + typeInfo.typeName + ") {}";
  } else if (typeInfo.type->isBool()) {
    this->_activateBuiltin("libStdbool");
    return "false";
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isObj() || typeInfo.type->isOpt()) {
    this->_activateBuiltin("libStdlib");
    return "NULL";
  } else if (typeInfo.type->isStr()) {
    this->_activateBuiltin("typeStr");
    return "(struct str) {}";
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
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeAny");
    return "(struct any) {0, NULL, 0, NULL, NULL}";
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    this->_activateEntity(typeInfo.typeName + "_alloc");
    return typeInfo.typeName + "_alloc(0)";
  } else if (typeInfo.type->isBool()) {
    this->_activateBuiltin("libStdbool");
    return "false";
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried object expression default field on invalid type");
  } else if (typeInfo.type->isOpt()) {
    this->_activateBuiltin("libStdlib");
    return "NULL";
  } else if (typeInfo.type->isObj()) {
    auto fieldsCode = std::string();

    for (const auto &typeField : typeInfo.type->fields) {
      if (typeField.builtin || typeField.method) {
        continue;
      }

      auto fieldTypeInfo = this->_typeInfo(typeField.type);
      fieldsCode += ", " + this->_exprObjDefaultField(fieldTypeInfo);
    }

    this->_activateEntity(typeInfo.typeName + "_alloc");
    return typeInfo.typeName + "_alloc(" + (fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2)) + ")";
  } else if (typeInfo.type->isStr()) {
    this->_activateBuiltin("fnStrAlloc");
    return R"(str_alloc(""))";
  } else {
    return "0";
  }
}

std::string Codegen::_fnDecl (
  Type *type,
  const std::string &codeName,
  const std::vector<std::shared_ptr<Var>> &stack,
  const std::vector<ASTFnDeclParam> &params,
  const ASTBlock &body,
  CodegenPhase phase
) {
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

      if (fnType.isMethod && fnType.methodInfo.isSelfFirst && fnType.methodInfo.selfType->shouldBeFreed()) {
        this->state.cleanUp.add(this->_genFreeFn(fnType.methodInfo.selfType, Codegen::name(fnType.methodInfo.selfCodeName)) + ";");
      }

      auto returnTypeInfo = this->_typeInfo(fnType.returnType);

      this->indent = 0;
      this->state.returnType = returnTypeInfo.type;
      bodyCode += this->_block(body, false);
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

      if (fnType.isMethod && fnType.methodInfo.isSelfFirst) {
        auto selfTypeInfo = this->_typeInfo(fnType.methodInfo.selfType);

        decl += ", " + (fnType.methodInfo.isSelfMut ? selfTypeInfo.typeCodeTrimmed : selfTypeInfo.typeCodeConstTrimmed);
        def += ", " + (fnType.methodInfo.isSelfMut ? selfTypeInfo.typeCode : selfTypeInfo.typeCodeConst) +
          Codegen::name(fnType.methodInfo.selfCodeName);
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
    this->_activateBuiltin("libStdlib");
    this->_activateEntity(typeName);

    code += " = (" + varTypeInfo.typeCodeTrimmed + ") {&" + typeName + ", NULL, 0};" EOL;
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
    this->_activateBuiltin("fnAnyCopy");
    result = "any_copy(" + result + ")";
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("fnBufferCopy");
    result = "buffer_copy(" + result + ")";
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
    this->_activateBuiltin("fnStrCopy");
    result = "str_copy(" + result + ")";
  }

  return result;
}

std::string Codegen::_genEqFn (Type *type, const std::string &leftCode, const std::string &rightCode, bool reverse) {
  auto initialState = this->state;

  auto eqFnOp = std::string(reverse ? "!=" : "==");
  auto eqFnName = std::string(reverse ? "ne" : "eq");
  auto eqFnNameB = Token::upperFirst(eqFnName);
  auto code = std::string();

  if (type->isAlias()) {
    code = this->_genEqFn(std::get<TypeAlias>(type->body).type, leftCode, rightCode, reverse);
  } else if (Type::real(type)->isObj() && Type::real(type)->builtin && Type::real(type)->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("fnBuffer" + eqFnNameB);
    code = "buffer_" + eqFnName + "(";
    code += this->_genCopyFn(Type::real(type), leftCode) + ", ";
    code += this->_genCopyFn(Type::real(type), rightCode) + ")";
  } else if (
    Type::real(type)->isArray() ||
    Type::real(type)->isMap() ||
    Type::real(type)->isObj() ||
    Type::real(type)->isOpt() ||
    Type::real(type)->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.realTypeName + "_" + eqFnName);
    code = typeInfo.realTypeName + "_" + eqFnName + "(";
    code += this->_genCopyFn(typeInfo.realType, leftCode) + ", ";
    code += this->_genCopyFn(typeInfo.realType, rightCode) + ")";
  } else if (Type::real(type)->isStr()) {
    this->_activateBuiltin("fnStr" + eqFnNameB + "Str");
    code = "str_" + eqFnName + "_str(";
    code += this->_genCopyFn(Type::real(type), leftCode) + ", ";
    code += this->_genCopyFn(Type::real(type), rightCode) + ")";
  } else {
    code = leftCode + " " + eqFnOp + " " + rightCode;
  }

  return code;
}

std::string Codegen::_genFreeFn (Type *type, const std::string &code) {
  auto initialState = this->state;
  auto result = code;

  if (type->isAlias()) {
    result = this->_genFreeFn(std::get<TypeAlias>(type->body).type, code);
  } else if (type->isAny()) {
    this->_activateBuiltin("fnAnyFree");
    result = "any_free((struct any) " + result + ")";
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("fnBufferFree");
    result = "buffer_free((struct buffer) " + result + ")";
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
    this->_activateBuiltin("fnStrFree");
    result = "str_free((struct str) " + result + ")";
  }

  return result;
}

std::string Codegen::_genReallocFn (Type *type, const std::string &leftCode, const std::string &rightCode) {
  auto initialState = this->state;
  auto result = std::string();

  if (type->isAny()) {
    this->_activateBuiltin("fnAnyRealloc");
    result = leftCode + " = any_realloc(" + leftCode + ", " + rightCode + ")";
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("fnBufferRealloc");
    result = leftCode + " = buffer_realloc(" + leftCode + ", " + rightCode + ")";
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
    this->_activateBuiltin("fnStrRealloc");
    result = leftCode + " = str_realloc(" + leftCode + ", " + rightCode + ")";
  }

  return result;
}

std::string Codegen::_genStrFn (Type *type, const std::string &code, bool copy, bool escape) {
  auto initialState = this->state;
  auto result = code;

  if (type->isAlias()) {
    result = this->_genStrFn(std::get<TypeAlias>(type->body).type, code, copy, escape);
  } else if (type->isAny()) {
    this->_activateBuiltin("fnAnyStr");
    result = "any_str(" + (copy ? this->_genCopyFn(type, result) : result) + ")";
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("fnBufferStr");
    result = "buffer_str(" + (copy ? this->_genCopyFn(type, result) : result) + ")";
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.realTypeName + "_str");
    result = typeInfo.realTypeName + "_str(" + (copy ? this->_genCopyFn(type, result) : result) + ")";
  } else if (type->isEnum()) {
    this->_activateBuiltin("fnEnumStr");
    result = "enum_str((int) " + result + ")";
  } else if (type->isStr() && escape) {
    this->_activateBuiltin("fnStrEscape");
    result = "str_escape(" + result + ")";
  } else if (type->isStr()) {
    result = copy ? this->_genCopyFn(type, result) : result;
  } else {
    auto typeInfo = this->_typeInfo(type);

    this->_activateBuiltin("fn" + Token::upperFirst(typeInfo.realTypeName) + "Str");
    result = typeInfo.realTypeName + "_str(" + result + ")";
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
    auto varTypeInfo = this->_typeInfo(nodeFnDecl.var->type);

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

    this->state.typeCasts.merge(bodyTypeCasts);
    this->varMap.save();
    code += this->_block(nodeIf.body);
    this->varMap.restore();
    this->state.typeCasts = initialStateTypeCasts;

    if (nodeIf.alt != std::nullopt) {
      code += std::string(this->indent, ' ') + "} else ";
      this->state.typeCasts.merge(altTypeCasts);

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

      if (objVar->builtin && objVar->codeName == "@os_EOL") {
        this->_activateBuiltin("definitions");
        this->_activateBuiltin("fnStrAlloc");
        code = "str_alloc(THE_EOL)";
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@path_SEP") {
        this->_activateBuiltin("definitions");
        this->_activateBuiltin("fnStrAlloc");
        code = "str_alloc(THE_PATH_SEP)";
        code = !root ? code : this->_genFreeFn(objVar->type, code);
      } else if (objVar->builtin && objVar->codeName == "@process_args") {
        this->needMainArgs = true;
        code = this->_apiEval("_{process_args}()");
        code = !root ? code : this->_genFreeFn(this->ast->typeMap.arrayOf(this->ast->typeMap.get("str")), code);
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

      if (exprAccess.prop == "cap" && objTypeInfo.realType->isMap()) {
        auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType);

        if (!objNodeExpr.parenthesized) {
          objCode = "(" + objCode + ")";
        }

        this->_activateEntity(objTypeInfo.realTypeName + "_cap");
        code = objTypeInfo.realTypeName + "_cap" + objCode;
      } else if (exprAccess.prop == "len" && (objTypeInfo.realType->isArray() || objTypeInfo.realType->isMap())) {
        auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType);

        if (!objNodeExpr.parenthesized) {
          objCode = "(" + objCode + ")";
        }

        this->_activateEntity(objTypeInfo.realTypeName + "_len");
        code = objTypeInfo.realTypeName + "_len" + objCode;
      } else if (exprAccess.prop == "rawValue" && objTypeInfo.realType->isEnum()) {
        auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType);

        if (!objNodeExpr.parenthesized) {
          objCode = "(" + objCode + ")";
        }

        this->_activateEntity(objTypeInfo.realTypeName + "_rawValue");
        code = objTypeInfo.realTypeName + "_rawValue" + objCode;
        code = !root ? code : this->_genFreeFn(this->ast->typeMap.get("str"), code);
      } else if (exprAccess.prop == "len" && objTypeInfo.realType->isStr()) {
        auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType);
        this->_activateBuiltin("fnStrLen");

        if (objNodeExpr.parenthesized) {
          code = "str_len" + objCode;
        } else {
          code = "str_len(" + objCode + ")";
        }
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
        auto objElemCode = this->_nodeExpr(*exprAccess.elem, exprAccess.elem->type);

        if (objTypeInfo.realType->isArray()) {
          this->_activateEntity(objTypeInfo.realTypeName + "_at");
          code = objTypeInfo.realTypeName + "_at(" + objCode + ", " + objElemCode + ")";
        } else if (objTypeInfo.realType->isStr()) {
          this->_activateBuiltin("fnStrAt");
          code = "str_at(" + objCode + ", " + objElemCode + ")";
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

      auto isExprAccessRef = nodeExpr.type->isRef() && targetType->isRef();
      auto isExprAccessEnumRawValue = objTypeInfo.realType->isEnum() && exprAccess.prop == "rawValue";

      if (!root && !isExprAccessRef && !isExprAccessEnumRawValue) {
        code = this->_genCopyFn(Type::real(nodeExpr.type), code);
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
          this->_activateBuiltin("fnStrConcatCstr");
          rightCode = "str_concat_cstr(" + this->_genCopyFn(this->ast->typeMap.get("str"), leftCode);
          rightCode += ", " + exprAssign.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr");
          rightCode = "str_concat_str(" + this->_genCopyFn(this->ast->typeMap.get("str"), leftCode);
          rightCode += ", " + this->_nodeExpr(exprAssign.right, nodeExpr.type) + ")";
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

      this->_activateBuiltin("fnBufferEq");
      code = "buffer_eq(" + leftCode + ", " + rightCode + ")";
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

      this->_activateBuiltin("fnBufferNe");
      code = "buffer_ne(" + leftCode + ", " + rightCode + ")";
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
        this->_activateBuiltin("fnCstrEqCstr");
        code = "cstr_eq_cstr(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")";
      } else if (exprBinary.left.isLit()) {
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));
        this->_activateBuiltin("fnCstrEqStr");
        code = "cstr_eq_str(" + exprBinary.left.litBody() + ", " + rightCode + ")";
      } else if (exprBinary.right.isLit()) {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        this->_activateBuiltin("fnStrEqCstr");
        code = "str_eq_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")";
      } else {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));

        this->_activateBuiltin("fnStrEqStr");
        code = "str_eq_str(" + leftCode + ", " + rightCode + ")";
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
      Type::real(exprBinary.left.type)->isStr() &&
      Type::real(exprBinary.right.type)->isStr()
    )) {
      if (exprBinary.left.isLit() && exprBinary.right.isLit()) {
        this->_activateBuiltin("fnCstrNeCstr");
        code = "cstr_ne_cstr(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")";
      } else if (exprBinary.left.isLit()) {
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));
        this->_activateBuiltin("fnCstrNeStr");
        code = "cstr_ne_str(" + exprBinary.left.litBody() + ", " + rightCode + ")";
      } else if (exprBinary.right.isLit()) {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        this->_activateBuiltin("fnStrNeCstr");
        code = "str_ne_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")";
      } else {
        auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"));

        this->_activateBuiltin("fnStrNeStr");
        code = "str_ne_str(" + leftCode + ", " + rightCode + ")";
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_ADD && (
      Type::real(exprBinary.left.type)->isStr() &&
      Type::real(exprBinary.right.type)->isStr()
    )) {
      if (root && nodeExpr.isLit()) {
        return this->_wrapNodeExpr(nodeExpr, targetType, root, nodeExpr.litBody());
      } else if (nodeExpr.isLit()) {
        this->_activateBuiltin("fnStrAlloc");
        code = "str_alloc(" + nodeExpr.litBody() + ")";
      } else if (exprBinary.left.isLit()) {
        this->_activateBuiltin("fnCstrConcatStr");
        code = "cstr_concat_str(" + exprBinary.left.litBody();
        code += ", " + this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str")) + ")";
      } else if (exprBinary.right.isLit()) {
        this->_activateBuiltin("fnStrConcatCstr");
        code = "str_concat_cstr(" + this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        code += ", " + exprBinary.right.litBody() + ")";
      } else {
        this->_activateBuiltin("fnStrConcatStr");
        code = "str_concat_str(" + this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"));
        code += ", " + this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str")) + ")";
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
    auto exprCallCalleeTypeInfo = this->_typeInfo(exprCall.callee.type);
    auto code = std::string();

    if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@exit") {
      auto arg1Expr = std::string("0");

      if (!exprCall.args.empty()) {
        arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("int"));
      }

      code = this->_apiEval("_{exit}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_chmodSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      auto arg2Expr = this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("int"));
      code = this->_apiEval("_{fs_chmodSync}(" + arg1Expr + ", " + arg2Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_chownSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      auto arg2Expr = this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("int"));
      auto arg3Expr = this->_nodeExpr(exprCall.args[2].expr, this->ast->typeMap.get("int"));
      code = this->_apiEval("_{fs_chownSync}(" + arg1Expr + ", " + arg2Expr + ", " + arg3Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_existsSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_existsSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_isAbsoluteSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_isAbsoluteSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_isDirectorySync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_isDirectorySync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_isFileSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_isFileSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_isSymbolicLinkSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_isSymbolicLinkSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_linkSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      auto arg2Expr = this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_linkSync}(" + arg1Expr + ", " + arg2Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_mkdirSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_mkdirSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_readFileSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_readFileSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_realpathSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_realpathSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_rmSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_rmSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_rmdirSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_rmdirSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_scandirSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_scandirSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_statSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_statSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_unlinkSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{fs_unlinkSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@fs_writeFileSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      auto arg2Expr = this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("buffer_Buffer"));
      code = this->_apiEval("_{fs_writeFileSync}(" + arg1Expr + ", " + arg2Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@os_name") {
      code = this->_apiEval("_{os_name}()", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@path_basename") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{path_basename}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@path_dirname") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{path_dirname}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@print") {
      auto separator = std::string(R"(" ")");
      auto isSeparatorLit = true;
      auto terminator = std::string("THE_EOL");
      auto isTerminatorLit = true;
      auto to = std::string("stdout");

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
            to = exprCallArg.expr.litBody() == R"("stderr")" ? "stderr" : "stdout";
          } else {
            this->_activateBuiltin("fnCstrEqStr");
            to = R"(cstr_eq_str("stderr", )" + this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str")) + R"() ? stderr : stdout)";
          }
        }
      }

      this->_activateBuiltin("definitions");
      this->_activateBuiltin("fnPrint");
      this->_activateBuiltin("libStdio");

      code = "print(" + to + R"(, ")";

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
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@process_cwd") {
      code = this->_apiEval("_{process_cwd}()");
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@process_getgid") {
      code = this->_apiEval("_{process_getgid}()");
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@process_getuid") {
      code = this->_apiEval("_{process_getuid}()");
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@process_runSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{process_runSync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@request_close") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.ref(this->ast->typeMap.get("request_Request")));
      code = this->_apiEval("_{request_close}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@request_open") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      auto arg2Expr = this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("str"));

      auto arg3Expr = std::string("(struct buffer) {NULL, 0}");
      auto isArg3ExprPlain = true;
      auto arg4Expr = std::string("(struct " + Codegen::typeName("array_request_Header") + ") {NULL, 0}");
      auto isArg4ExprPlain = true;

      for (const auto &exprCallArg : exprCall.args) {
        if (exprCallArg.id != std::nullopt && exprCallArg.id == "data") {
          arg3Expr = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("buffer_Buffer"));
          isArg3ExprPlain = false;
        } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "headers") {
          arg4Expr = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.arrayOf(this->ast->typeMap.get("request_Header")));
          isArg4ExprPlain = false;
        }
      }

      if (isArg3ExprPlain) {
        this->_activateBuiltin("libStdlib");
        this->_activateBuiltin("typeBuffer");
      }

      if (isArg4ExprPlain) {
        auto typeName = this->_typeNameArray(this->ast->typeMap.arrayOf(this->ast->typeMap.get("request_Header")));

        this->_activateBuiltin("libStdlib");
        this->_activateEntity(typeName);
      }

      code = this->_apiEval("_{request_open}(" + arg1Expr + ", " + arg2Expr + ", " + arg3Expr + ", " + arg4Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@request_read") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.ref(this->ast->typeMap.get("request_Request")));
      code = this->_apiEval("_{request_read}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@sleepSync") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("int"));
      code = this->_apiEval("_{sleep_sync}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@url_parse") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
      code = this->_apiEval("_{url_parse}(" + arg1Expr + ")", 1);
    } else if (exprCallCalleeTypeInfo.realType->builtin) {
      auto calleeExprAccess = std::get<ASTExprAccess>(*exprCall.callee.body);
      auto calleeNodeExpr = std::get<ASTNodeExpr>(*calleeExprAccess.expr);
      auto calleeTypeInfo = this->_typeInfo(calleeNodeExpr.type);

      if (exprCallCalleeTypeInfo.realType->codeName.ends_with(".str")) {
        auto typeStrFn = std::string();

        if (exprCallCalleeTypeInfo.realType->codeName == "@buffer_Buffer.str") {
          this->_activateBuiltin("fnBufferToStr");
          typeStrFn = "buffer_to_str";
        } else if (
          exprCallCalleeTypeInfo.realType->codeName == "@array.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@fn.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@map.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@obj.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@opt.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@union.str"
        ) {
          this->_activateEntity(calleeTypeInfo.realTypeName + "_str");
          typeStrFn = calleeTypeInfo.realTypeName + "_str";
        } else if (calleeTypeInfo.realType->isObj()) {
          this->_activateEntity(calleeTypeInfo.realType->name + "_str");
          typeStrFn = calleeTypeInfo.realType->name + "_str";
        } else {
          auto codeName = exprCallCalleeTypeInfo.realType->codeName.substr(1);

          this->_activateBuiltin("fn" + Token::upperFirst(codeName.substr(0, codeName.find('.'))) + "Str");
          typeStrFn = codeName.substr(0, codeName.find('.')) + "_str";
        }

        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);

        if (calleeTypeInfo.realType->isEnum()) {
          calleeCode = "((int) " + calleeCode + ")";
        } else if (!calleeNodeExpr.parenthesized) {
          calleeCode = "(" + calleeCode + ")";
        }

        code = typeStrFn + calleeCode;
      } else if (
        exprCallCalleeTypeInfo.realType->codeName == "@array.empty" ||
        exprCallCalleeTypeInfo.realType->codeName == "@map.empty"
      ) {
        this->_activateEntity(calleeTypeInfo.realTypeName + "_empty");
        code = calleeTypeInfo.realTypeName + "_empty(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.join") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        auto arg1Expr = std::string(exprCall.args.empty() ? "0" : "1");
        auto arg2Expr = std::string();

        if (exprCall.args.empty()) {
          this->_activateBuiltin("typeStr");
          arg2Expr = "(struct str) {}";
        } else {
          arg2Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
        }

        this->_activateEntity(calleeTypeInfo.realTypeName + "_join");
        code = calleeTypeInfo.realTypeName + "_join(" + calleeCode + ", " + arg1Expr + ", " + arg2Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.pop") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_pop");
        code = calleeTypeInfo.realTypeName + "_pop(" + calleeCode + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.push") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_push");
        code = calleeTypeInfo.realTypeName + "_push(" + calleeCode + ", " + std::to_string(exprCall.args.size());

        if (!exprCall.args.empty()) {
          auto elementTypeInfo = std::get<TypeArray>(calleeTypeInfo.realType->body).elementType;

          for (const auto &arg : exprCall.args) {
            code += ", " + this->_nodeExpr(arg.expr, elementTypeInfo);
          }
        }

        code += ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.reverse") {
        this->_activateEntity(calleeTypeInfo.realTypeName + "_reverse");
        code = calleeTypeInfo.realTypeName + "_reverse(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.slice") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        auto arg1Expr = std::string(exprCall.args.empty() ? "0" : "1");
        auto arg2Expr = exprCall.args.empty() ? "0" : this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("i64"));
        auto arg3Expr = std::string(exprCall.args.size() < 2 ? "0" : "1");
        auto arg4Expr = exprCall.args.size() < 2 ? "0" : this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("i64"));

        this->_activateEntity(calleeTypeInfo.realTypeName + "_slice");
        code = calleeTypeInfo.realTypeName + "_slice(" + calleeCode + ", " + arg1Expr;
        code += ", " + arg2Expr + ", " + arg3Expr + ", " + arg4Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@char.isAlpha") {
        this->_activateBuiltin("fnCharIsAlpha");
        code = "char_is_alpha(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@char.isAlphaNum") {
        this->_activateBuiltin("fnCharIsAlphaNum");
        code = "char_is_alpha_num(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@char.isDigit") {
        this->_activateBuiltin("fnCharIsDigit");
        code = "char_is_digit(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@char.isSpace") {
        this->_activateBuiltin("fnCharIsSpace");
        code = "char_is_space(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@char.repeat") {
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("int"));
        this->_activateBuiltin("fnCharRepeat");
        code = "char_repeat(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.clear") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_clear");
        code = calleeTypeInfo.realTypeName + "_clear(" + calleeCode + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.get") {
        auto mapType = std::get<TypeBodyMap>(calleeTypeInfo.realType->body);
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, mapType.keyType);

        this->_activateEntity(calleeTypeInfo.realTypeName + "_get");
        code = calleeTypeInfo.realTypeName + "_get(" + calleeCode + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.has") {
        auto mapType = std::get<TypeBodyMap>(calleeTypeInfo.realType->body);
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, mapType.keyType);

        this->_activateEntity(calleeTypeInfo.realTypeName + "_has");
        code = calleeTypeInfo.realTypeName + "_has(" + calleeCode + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.keys") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_keys");
        code = calleeTypeInfo.realTypeName + "_keys(" + calleeCode + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.merge") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, calleeTypeInfo.realType);

        this->_activateEntity(calleeTypeInfo.realTypeName + "_merge");
        code = calleeTypeInfo.realTypeName + "_merge(" + calleeCode + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.remove") {
        auto mapType = std::get<TypeBodyMap>(calleeTypeInfo.realType->body);
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, mapType.keyType);

        this->_activateEntity(calleeTypeInfo.realTypeName + "_remove");
        code = calleeTypeInfo.realTypeName + "_remove(" + calleeCode + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.reserve") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("int"));

        this->_activateEntity(calleeTypeInfo.realTypeName + "_reserve");
        code = calleeTypeInfo.realTypeName + "_reserve(" + calleeCode + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.set") {
        auto mapType = std::get<TypeBodyMap>(calleeTypeInfo.realType->body);
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, mapType.keyType);
        auto arg2Expr = this->_nodeExpr(exprCall.args[1].expr, mapType.valueType);

        this->_activateEntity(calleeTypeInfo.realTypeName + "_set");
        code = calleeTypeInfo.realTypeName + "_set(" + calleeCode + ", " + arg1Expr + ", " + arg2Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.shrink") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, this->ast->typeMap.ref(calleeTypeInfo.realType), true);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_shrink");
        code = calleeTypeInfo.realTypeName + "_shrink(" + calleeCode + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@map.values") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_values");
        code = calleeTypeInfo.realTypeName + "_values(" + calleeCode + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.empty") {
        this->_activateBuiltin("fnStrEmpty");
        code = "str_empty(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.find") {
        auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("str"));
        this->_activateBuiltin("fnStrFind");
        code = "str_find(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ", " + arg1Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.lines") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        auto arg1Expr = std::string(exprCall.args.empty() ? "0" : "1");
        auto arg2Expr = exprCall.args.empty() ? "0" : this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("bool"));

        this->_activateBuiltin("fnStrLines");
        code = "str_lines(" + calleeCode + ", " + arg1Expr + ", " + arg2Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.lower") {
        this->_activateBuiltin("fnStrLower");
        code = "str_lower(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.lowerFirst") {
        this->_activateBuiltin("fnStrLowerFirst");
        code = "str_lower_first(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.slice") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);
        auto arg1Expr = std::string(exprCall.args.empty() ? "0" : "1");
        auto arg2Expr = exprCall.args.empty() ? "0" : this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("i64"));
        auto arg3Expr = std::string(exprCall.args.size() < 2 ? "0" : "1");
        auto arg4Expr = exprCall.args.size() < 2 ? "0" : this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("i64"));

        this->_activateBuiltin("fnStrSlice");
        code = "str_slice(" + calleeCode + ", " + arg1Expr + ", " + arg2Expr + ", " + arg3Expr + ", " + arg4Expr + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.toBuffer") {
        this->_activateBuiltin("fnStrToBuffer");
        code = "str_to_buffer(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.trim") {
        this->_activateBuiltin("fnStrTrim");
        code = "str_trim(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.upper") {
        this->_activateBuiltin("fnStrUpper");
        code = "str_upper(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.upperFirst") {
        this->_activateBuiltin("fnStrUpperFirst");
        code = "str_upper_first(" + this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType) + ")";
      }
    } else {
      auto fnType = std::get<TypeFn>(exprCallCalleeTypeInfo.realType->body);
      auto bodyCode = std::string();

      if (!fnType.params.empty()) {
        auto paramsName = exprCallCalleeTypeInfo.realTypeName + "P";

        bodyCode += "(struct " + paramsName + ") {";
        this->_activateEntity(paramsName);

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

        bodyCode += "}";
      }

      auto fnName = fnType.isMethod
        ? Codegen::name(fnType.methodInfo.codeName)
        : this->_nodeExpr(exprCall.callee, exprCallCalleeTypeInfo.realType, true);

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

      code = fnName + ".f(" + fnName + ".x";

      if (fnType.isMethod && fnType.methodInfo.isSelfFirst) {
        auto exprAccess = std::get<ASTExprAccess>(*exprCall.callee.body);
        auto nodeExprAccess = std::get<ASTNodeExpr>(*exprAccess.expr);

        code += ", " + this->_genCopyFn(fnType.methodInfo.selfType, this->_nodeExpr(nodeExprAccess, fnType.methodInfo.selfType, true));
      }

      if (!bodyCode.empty()) {
        code += ", " + bodyCode;
      }

      code += ")";

      if (nodeExpr.type->isRef() && !targetType->isRef()) {
        code = "*" + code;
      }
    }

    code = !root ? code : this->_genFreeFn(nodeExpr.type, code);
    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto initialStateTypeCasts = this->state.typeCasts;
    auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(exprCond.cond);
    auto condCode = this->_nodeExpr(exprCond.cond, this->ast->typeMap.get("bool"));

    this->state.typeCasts.merge(bodyTypeCasts);
    auto bodyCode = this->_nodeExpr(exprCond.body, nodeExpr.type);
    this->state.typeCasts = initialStateTypeCasts;
    this->state.typeCasts.merge(altTypeCasts);
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
      this->_activateBuiltin("libStdbool");
    } else if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
      auto val = std::stoull(code);

      if (val > 9223372036854775807) {
        code += "U";
      }
    } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      code.erase(std::remove(code.begin(), code.end(), 'O'), code.end());
      code.erase(std::remove(code.begin(), code.end(), 'o'), code.end());
    } else if (exprLit.type == AST_EXPR_LIT_NIL) {
      this->_activateBuiltin("libStdlib");
      code = "NULL";
    } else if (!root && exprLit.type == AST_EXPR_LIT_STR) {
      this->_activateBuiltin("fnStrAlloc");
      code = "str_alloc(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprMap>(*nodeExpr.body)) {
    auto exprMap = std::get<ASTExprMap>(*nodeExpr.body);
    auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
    auto mapType = std::get<TypeBodyMap>(nodeTypeInfo.type->body);
    auto fieldsCode = std::to_string(exprMap.props.size());

    for (const auto &prop : exprMap.props) {
      this->_activateBuiltin("fnStrAlloc");
      fieldsCode += R"(, str_alloc(")" + prop.name + R"("), )" + this->_nodeExpr(prop.init, mapType.valueType);
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
      if (typeField.builtin || typeField.method) {
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
      this->_activateBuiltin("libStdbool");
      argCode = "((bool) " + argCode + ")";
    } else if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isStr()) {
      this->_activateBuiltin("fnStrNot");
      argCode = "str_not(" + argCode + ")";
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
    this->_activateBuiltin("libStdlib");
    return "{0, NULL, 0, NULL, NULL}";
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    this->_activateEntity(typeInfo.typeName + "_alloc");
    return typeInfo.typeName + "_alloc(0)";
  } else if (typeInfo.type->isBool()) {
    this->_activateBuiltin("libStdbool");
    return "false";
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried node variable declaration of invalid type");
  } else if (typeInfo.type->isObj()) {
    auto fieldsCode = std::string();

    for (const auto &typeField : typeInfo.type->fields) {
      if (!typeField.builtin && !typeField.method) {
        fieldsCode += ", " + this->_exprObjDefaultField(this->_typeInfo(typeField.type));
      }
    }

    auto allocCode = fieldsCode.empty() ? fieldsCode : fieldsCode.substr(2);
    return this->_apiEval("_{" + typeInfo.typeName + "_alloc}(" + allocCode + ")", 1);
  } else if (typeInfo.type->isOpt()) {
    this->_activateBuiltin("libStdlib");
    return "NULL";
  } else if (typeInfo.type->isStr()) {
    this->_activateBuiltin("fnStrAlloc");
    return R"(str_alloc(""))";
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
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
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

    decl += elementTypeInfo.typeRefCode + typeName + "_at (struct _{" + typeName + "}, _{int64_t});";
    def += elementTypeInfo.typeRefCode + typeName + "_at (struct _{" + typeName + "} n, _{int64_t} i) {" EOL;
    def += "  if ((i >= 0 && i >= n.l) || (i < 0 && i < -((_{int64_t}) n.l))) {" EOL;
    def += R"(    _{fprintf}(_{stderr}, "Error: index %" _{PRId64} " out of array bounds" _{THE_EOL}, i);)" EOL;
    def += "    _{exit}(_{EXIT_FAILURE});" EOL;
    def += "  }" EOL;
    def += "  return i < 0 ? &n.d[n.l + i] : &n.d[i];" EOL;
    def += "}";

    return freeFnEntityIdx;
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

    return atFnEntityIdx + 1;
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

  this->_apiEntity(typeName + "_len", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    decl += "_{size_t} " + typeName + "_len (struct _{" + typeName + "});";
    def += "_{size_t} " + typeName + "_len (struct _{" + typeName + "} n) {" EOL;
    def += "  _{size_t} l = n.l;" EOL;
    def += "  " + this->_genFreeFn(type, "n") + ";" EOL;
    def += "  return l;" EOL;
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

    decl += "void " + typeName + "_push (struct _{" + typeName + "} *, _{size_t}, ...);";
    def += "void " + typeName + "_push (struct _{" + typeName + "} *n, _{size_t} x, ...) {" EOL;
    def += "  if (x == 0) return;" EOL;
    def += "  n->l += x;" EOL;
    def += "  n->d = _{re_alloc}(n->d, n->l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
    def += "  _{va_list} args;" EOL;
    def += "  _{va_start}(args, x);" EOL;
    def += "  for (_{size_t} i = n->l - x; i < n->l; i++) ";
    def += "n->d[i] = _{va_arg}(args, " + elementType->vaArgCode(elementTypeInfo.typeCodeTrimmed) + ");" EOL;
    def += "  _{va_end}(args);" EOL;
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
    decl += "unsigned int, _{int64_t}, unsigned int, _{int64_t});";
    def += "struct _{" + typeName + "} " + typeName + "_slice (struct _{" + typeName + "} n, ";
    def += "unsigned int o1, _{int64_t} n1, unsigned int o2, _{int64_t} n2) {" EOL;
    def += "  _{int64_t} i1 = o1 == 0 ? 0 : (_{int64_t}) (n1 < 0 ? (n1 < -((_{int64_t}) n.l) ? 0 : n1 + n.l) ";
    def += ": (n1 > n.l ? n.l : n1));" EOL;
    def += "  _{int64_t} i2 = o2 == 0 ? (_{int64_t}) n.l : (_{int64_t}) (n2 < 0 ? (n2 < -((_{int64_t}) n.l) ? 0 : n2 + n.l) ";
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
  auto paramsName = Codegen::typeName(type->name + "P");
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

    if (fnType.isMethod && fnType.methodInfo.isSelfFirst) {
      auto selfTypeInfo = this->_typeInfo(fnType.methodInfo.selfType);
      functorArgs += ", " + (fnType.methodInfo.isSelfMut ? selfTypeInfo.typeCode : selfTypeInfo.typeCodeConst);
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
  auto typeName = Codegen::typeName(type->name);
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
    def += "  return r;" EOL;
    def += "}";

    return 0;
  });

  this->_apiEntity(typeName + "_keys", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
    auto keyTypeInfo = this->_typeInfo(mapType.keyType);
    auto keysTypeInfo = this->_typeInfo(this->ast->typeMap.arrayOf(mapType.keyType));

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
      def += "    " + this->_genFreeFn(mapType.keyType, "n->d[i].f") + ";" EOL;
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

    if (mapType.valueType->shouldBeFreed()) {
      def += "      " + this->_genFreeFn(mapType.valueType, "n.d[i].s") + ";" EOL;
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
    def += "  if (n->l != n->c) n->d = _{re_alloc}(n->d, (n->c = n->l) * sizeof(struct _{" + pairTypeName + "}));" EOL;
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
    auto valuesTypeInfo = this->_typeInfo(this->ast->typeMap.arrayOf(mapType.valueType));

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

std::string Codegen::_typeNameObj (Type *type, bool builtin) {
  auto typeName = builtin ? type->name : Codegen::typeName(type->codeName);

  this->_apiEntity(typeName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
    auto objFields = std::string();

    for (const auto &field : type->fields) {
      if (field.builtin || field.method) {
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

std::string Codegen::_typeNameObjDef (Type *type, const std::map<std::string, std::string> &extra, bool builtin) {
  auto saveStateBuiltins = this->state.builtins;
  auto saveStateEntities = this->state.entities;
  auto typeName = builtin ? type->name : Codegen::typeName(type->codeName);

  for (auto &entity : this->entities) {
    if (entity.name == typeName + "_alloc") {
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;

      auto allocFnParamTypes = std::string();
      auto allocFnParams = std::string();
      auto allocFnCode = std::string();

      for (const auto &field : type->fields) {
        if (!field.builtin && !field.method) {
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
        if (!field.builtin && !field.method) {
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
        if (!field.builtin && !field.method) {
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
        if (!field.builtin && !field.method && field.type->shouldBeFreed()) {
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
        if (!field.builtin && !field.method) {
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
        if (!field.builtin && !field.method) {
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
