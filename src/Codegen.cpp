/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Codegen.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "config.hpp"

const auto banner = std::string(
  "/*!" EOL
  " * Copyright (c) Aaron Delasy" EOL
  " *" EOL
  " * Unauthorized copying of this file, via any medium is strictly prohibited" EOL
  " * Proprietary and confidential" EOL
  " */" EOL
  EOL
);

void Codegen::compile (const std::string &path, const std::tuple<std::string, std::string> &result, bool debug) {
  auto code = std::get<0>(result);
  auto flags = std::get<1>(result);

  auto f = std::ofstream("build/output.c");
  f << code;
  f.close();

  auto cmd = "gcc build/output.c -w -o " + path + (debug ? " -g" : "") + (flags.empty() ? "" : " " + flags);
  std::system(cmd.c_str());
  std::filesystem::remove("build/output.c");
}

std::string Codegen::name (const std::string &name) {
  return "__THE_0_" + name;
}

std::string Codegen::typeName (const std::string &name) {
  return "__THE_1_" + name;
}

Codegen::Codegen (AST *a) {
  this->ast = a;
  this->reader = this->ast->reader;
}

std::tuple<std::string, std::string> Codegen::gen () {
  auto nodes = this->ast->gen();
  auto mainSetUp = std::string();
  auto mainCode = std::string();
  auto mainCleanUp = std::string();
  auto topLevelCode = std::string();

  for (const auto &node : nodes) {
    if (std::holds_alternative<ASTNodeMain>(*node.body)) {
      auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);

      mainSetUp += nodeSetUp;
      mainCode += nodeCode;
      mainCleanUp = nodeCleanUp + mainCleanUp;
    } else {
      auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);

      if (!nodeSetUp.empty() || !nodeCleanUp.empty()) {
        throw Error("Error: top level node has setUp or cleanUp");
      }

      topLevelCode += nodeCode;
    }
  }

  auto fnDeclCode = std::string();
  auto fnDefCode = std::string();
  auto structDeclCode = std::string();
  auto structDefCode = std::string();

  for (const auto &entity : this->entities) {
    if (!entity.active) {
      continue;
    }

    if (entity.type == CODEGEN_ENTITY_FN) {
      fnDeclCode += entity.decl + EOL;
      fnDefCode += entity.def + EOL;
    } else if (entity.type == CODEGEN_ENTITY_OBJ) {
      structDeclCode += entity.decl + EOL;
      structDefCode += entity.def + EOL;
    }
  }

  auto builtinFnDeclCode = std::string();
  auto builtinFnDefCode = std::string();
  auto builtinStructDefCode = std::string();

  if (this->builtins.fnCstrConcatStr) {
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str cstr_concat_str (const char *, struct str);" EOL;
    builtinFnDefCode += "struct str cstr_concat_str (const char *c, struct str s) {" EOL;
    builtinFnDefCode += "  size_t l = s.l + strlen(c);" EOL;
    builtinFnDefCode += "  char *r = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(r, s.c, s.l);" EOL;
    builtinFnDefCode += "  memcpy(&r[s.l], c, l - s.l);" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return (struct str) {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrEqCstr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libString");

    builtinFnDeclCode += "bool cstr_eq_cstr (const char *, const char *);" EOL;
    builtinFnDefCode += "bool cstr_eq_cstr (const char *c1, const char *c2) {" EOL;
    builtinFnDefCode += "  size_t l = strlen(c1);" EOL;
    builtinFnDefCode += "  return l == strlen(c2) && memcmp(c1, c2, l) == 0;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrEqStr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool cstr_eq_str (const char *, struct str);" EOL;
    builtinFnDefCode += "bool cstr_eq_str (const char *c, struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l == strlen(c) && memcmp(s.c, c, s.l) == 0;" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrNeCstr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libString");

    builtinFnDeclCode += "bool cstr_ne_cstr (const char *, const char *);" EOL;
    builtinFnDefCode += "bool cstr_ne_cstr (const char *c1, const char *c2) {" EOL;
    builtinFnDefCode += "  size_t l = strlen(c1);" EOL;
    builtinFnDefCode += "  return l != strlen(c2) || memcmp(c1, c2, l) != 0;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCstrNeStr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool cstr_ne_str (const char *, struct str);" EOL;
    builtinFnDefCode += "bool cstr_ne_str (const char *c, struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l != strlen(c) || memcmp(s.c, c, s.l) != 0;" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnIntStr) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str int_str (int32_t);" EOL;
    builtinFnDefCode += "struct str int_str (int32_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId32, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnPrint) {
    this->_activateBuiltin("fnStrFree");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdarg");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

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
    builtinFnDefCode += R"(      case 'h': sprintf(buf, "%" PRId8, va_arg(args, int)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'j': sprintf(buf, "%" PRId16, va_arg(args, int)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'i':" EOL;
    builtinFnDefCode += R"(      case 'k': sprintf(buf, "%" PRId32, va_arg(args, int32_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'l': sprintf(buf, "%" PRId64, va_arg(args, int64_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 's': s = va_arg(args, struct str); fwrite(s.c, 1, s.l, stream); str_free(s); break;" EOL;
    builtinFnDefCode += R"(      case 'v': sprintf(buf, "%" PRIu8, va_arg(args, int)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'w': sprintf(buf, "%" PRIu16, va_arg(args, int)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'u': sprintf(buf, "%" PRIu32, va_arg(args, uint32_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'y': sprintf(buf, "%" PRIu64, va_arg(args, uint64_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'z': fputs(va_arg(args, char *), stream); break;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  va_end(args);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrAlloc) {
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str str_alloc (const char *);" EOL;
    builtinFnDefCode += "struct str str_alloc (const char *c) {" EOL;
    builtinFnDefCode += "  size_t l = strlen(c);" EOL;
    builtinFnDefCode += "  char *r = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(r, c, l);" EOL;
    builtinFnDefCode += "  return (struct str) {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrConcatCstr) {
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str str_concat_cstr (struct str, const char *);" EOL;
    builtinFnDefCode += "struct str str_concat_cstr (struct str s, const char *c) {" EOL;
    builtinFnDefCode += "  size_t l = s.l + strlen(c);" EOL;
    builtinFnDefCode += "  char *r = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(r, s.c, s.l);" EOL;
    builtinFnDefCode += "  memcpy(&r[s.l], c, l - s.l);" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return (struct str) {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrConcatStr) {
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str str_concat_str (struct str, struct str);" EOL;
    builtinFnDefCode += "struct str str_concat_str (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  size_t l = s1.l + s2.l;" EOL;
    builtinFnDefCode += "  char *r = alloc(l);" EOL;
    builtinFnDefCode += "  memcpy(r, s1.c, s1.l);" EOL;
    builtinFnDefCode += "  memcpy(&r[s1.l], s2.c, s2.l);" EOL;
    builtinFnDefCode += "  free(s1.c);" EOL;
    builtinFnDefCode += "  free(s2.c);" EOL;
    builtinFnDefCode += "  return (struct str) {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrCopy) {
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str str_copy (const struct str);" EOL;
    builtinFnDefCode += "struct str str_copy (const struct str s) {" EOL;
    builtinFnDefCode += "  char *r = alloc(s.l);" EOL;
    builtinFnDefCode += "  memcpy(r, s.c, s.l);" EOL;
    builtinFnDefCode += "  return (struct str) {r, s.l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEqCstr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool str_eq_cstr (struct str, const char *);" EOL;
    builtinFnDefCode += "bool str_eq_cstr (struct str s, const char *c) {" EOL;
    builtinFnDefCode += "  bool r = s.l == strlen(c) && memcmp(s.c, c, s.l) == 0;" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEqStr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool str_eq_str (struct str, struct str);" EOL;
    builtinFnDefCode += "bool str_eq_str (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  bool r = s1.l == s2.l && memcmp(s1.c, s2.c, s1.l) == 0;" EOL;
    builtinFnDefCode += "  free(s1.c);" EOL;
    builtinFnDefCode += "  free(s2.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrEscape) {
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str str_escape (const struct str);" EOL;
    builtinFnDefCode += "struct str str_escape (const struct str s) {" EOL;
    builtinFnDefCode += "  char *r = alloc(s.l);" EOL;
    builtinFnDefCode += "  size_t l = 0;" EOL;
    builtinFnDefCode += "  for (size_t i = 0; i < s.l; i++) {" EOL;
    builtinFnDefCode += "    char c = s.c[i];" EOL;
    builtinFnDefCode += R"(    if (c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '"') {)" EOL;
    builtinFnDefCode += "      if (l + 2 > s.l) r = realloc(r, l + 2);" EOL;
    builtinFnDefCode += R"(      r[l++] = '\\';)" EOL;
    builtinFnDefCode += R"(      if (c == '\f') r[l++] = 'f';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\n') r[l++] = 'n';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\r') r[l++] = 'r';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\t') r[l++] = 't';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\v') r[l++] = 'v';)" EOL;
    builtinFnDefCode += R"(      else if (c == '"') r[l++] = '"';)" EOL;
    builtinFnDefCode += "      continue;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    if (l + 1 > s.l) r = realloc(r, l + 1);" EOL;
    builtinFnDefCode += "    r[l++] = c;" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return (struct str) {r, l};" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrFree) {
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "void str_free (struct str);" EOL;
    builtinFnDefCode += "void str_free (struct str s) {" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrNeCstr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool str_ne_cstr (struct str, const char *);" EOL;
    builtinFnDefCode += "bool str_ne_cstr (struct str s, const char *c) {" EOL;
    builtinFnDefCode += "  bool r = s.l != strlen(c) || memcmp(s.c, c, s.l) != 0;" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrNeStr) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool str_ne_str (struct str, struct str);" EOL;
    builtinFnDefCode += "bool str_ne_str (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  bool r = s1.l != s2.l || memcmp(s1.c, s2.c, s1.l) != 0;" EOL;
    builtinFnDefCode += "  free(s1.c);" EOL;
    builtinFnDefCode += "  free(s2.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrNot) {
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "bool str_not (struct str);" EOL;
    builtinFnDefCode += "bool str_not (struct str s) {" EOL;
    builtinFnDefCode += "  bool r = s.l == 0;" EOL;
    builtinFnDefCode += "  free(s.c);" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnStrRealloc) {
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str str_realloc (struct str, struct str);" EOL;
    builtinFnDefCode += "struct str str_realloc (struct str s1, struct str s2) {" EOL;
    builtinFnDefCode += "  free(s1.c);" EOL;
    builtinFnDefCode += "  return s2;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnAlloc) {
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");

    builtinFnDeclCode += "void *alloc (size_t);" EOL;
    builtinFnDefCode += "void *alloc (size_t l) {" EOL;
    builtinFnDefCode += "  void *r = malloc(l);" EOL;
    builtinFnDefCode += "  if (r == NULL) {" EOL;
    builtinFnDefCode += R"(    fprintf(stderr, "Error: failed to allocate %zu bytes)" ESC_EOL R"(", l);)" EOL;
    builtinFnDefCode += "    exit(EXIT_FAILURE);" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return r;" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.typeStr) {
    this->_activateBuiltin("libStdlib");

    builtinStructDefCode += "struct str {" EOL;
    builtinStructDefCode += "  char *c;" EOL;
    builtinStructDefCode += "  size_t l;" EOL;
    builtinStructDefCode += "};" EOL;
  }

  builtinStructDefCode += builtinStructDefCode.empty() ? "" : EOL;
  builtinFnDeclCode += builtinFnDeclCode.empty() ? "" : EOL;
  builtinFnDefCode += builtinFnDefCode.empty() ? "" : EOL;
  structDeclCode += structDeclCode.empty() ? "" : EOL;
  structDefCode += structDefCode.empty() ? "" : EOL;
  fnDeclCode += fnDeclCode.empty() ? "" : EOL;
  fnDefCode += fnDefCode.empty() ? "" : EOL;

  auto headers = std::string(this->builtins.libInttypes ? "#include <inttypes.h>" EOL : "");
  headers += this->builtins.libStdarg ? "#include <stdarg.h>" EOL : "";
  headers += this->builtins.libStdbool ? "#include <stdbool.h>" EOL : "";
  headers += this->builtins.libStdint && !this->builtins.libInttypes ? "#include <stdint.h>" EOL : "";
  headers += this->builtins.libStdio ? "#include <stdio.h>" EOL : "";
  headers += this->builtins.libStdlib ? "#include <stdlib.h>" EOL : "";
  headers += this->builtins.libString ? "#include <string.h>" EOL : "";
  headers += headers.empty() ? "" : EOL;

  auto output = std::string();
  output += banner;
  output += headers;
  output += builtinStructDefCode;
  output += builtinFnDeclCode;
  output += builtinFnDefCode;
  output += structDeclCode;
  output += structDefCode;
  output += fnDeclCode;
  output += fnDefCode;
  output += topLevelCode;
  output += "int main () {" EOL;
  output += mainSetUp;
  output += mainCode;
  output += mainCleanUp;
  output += "}" EOL;

  return std::make_tuple(output, this->_flags());
}

void Codegen::_activateBuiltin (const std::string &name, std::optional<std::vector<std::string> *> entityBuiltins) {
  if (entityBuiltins != std::nullopt) {
    auto b = *entityBuiltins;

    if (std::find(b->begin(), b->end(), name) == b->end()) {
      b->push_back(name);
    }

    return;
  }

  if (name == "fnAlloc") this->builtins.fnAlloc = true;
  else if (name == "fnBoolStr") this->builtins.fnBoolStr = true;
  else if (name == "fnByteStr") this->builtins.fnByteStr = true;
  else if (name == "fnCharStr") this->builtins.fnCharStr = true;
  else if (name == "fnCstrConcatStr") this->builtins.fnCstrConcatStr = true;
  else if (name == "fnCstrEqCstr") this->builtins.fnCstrEqCstr = true;
  else if (name == "fnCstrEqStr") this->builtins.fnCstrEqStr = true;
  else if (name == "fnCstrNeCstr") this->builtins.fnCstrNeCstr = true;
  else if (name == "fnCstrNeStr") this->builtins.fnCstrNeStr = true;
  else if (name == "fnF32Str") this->builtins.fnF32Str = true;
  else if (name == "fnF64Str") this->builtins.fnF64Str = true;
  else if (name == "fnFloatStr") this->builtins.fnFloatStr = true;
  else if (name == "fnI8Str") this->builtins.fnI8Str = true;
  else if (name == "fnI16Str") this->builtins.fnI16Str = true;
  else if (name == "fnI32Str") this->builtins.fnI32Str = true;
  else if (name == "fnI64Str") this->builtins.fnI64Str = true;
  else if (name == "fnIntStr") this->builtins.fnIntStr = true;
  else if (name == "fnPrint") this->builtins.fnPrint = true;
  else if (name == "fnStrAlloc") this->builtins.fnStrAlloc = true;
  else if (name == "fnStrConcatCstr") this->builtins.fnStrConcatCstr = true;
  else if (name == "fnStrConcatStr") this->builtins.fnStrConcatStr = true;
  else if (name == "fnStrCopy") this->builtins.fnStrCopy = true;
  else if (name == "fnStrEqCstr") this->builtins.fnStrEqCstr = true;
  else if (name == "fnStrEqStr") this->builtins.fnStrEqStr = true;
  else if (name == "fnStrEscape") this->builtins.fnStrEscape = true;
  else if (name == "fnStrFree") this->builtins.fnStrFree = true;
  else if (name == "fnStrNeCstr") this->builtins.fnStrNeCstr = true;
  else if (name == "fnStrNeStr") this->builtins.fnStrNeStr = true;
  else if (name == "fnStrNot") this->builtins.fnStrNot = true;
  else if (name == "fnStrRealloc") this->builtins.fnStrRealloc = true;
  else if (name == "fnU8Str") this->builtins.fnU8Str = true;
  else if (name == "fnU16Str") this->builtins.fnU16Str = true;
  else if (name == "fnU32Str") this->builtins.fnU32Str = true;
  else if (name == "fnU64Str") this->builtins.fnU64Str = true;
  else if (name == "libInttypes") this->builtins.libInttypes = true;
  else if (name == "libStdarg") this->builtins.libStdarg = true;
  else if (name == "libStdbool") this->builtins.libStdbool = true;
  else if (name == "libStdint") this->builtins.libStdint = true;
  else if (name == "libStdio") this->builtins.libStdio = true;
  else if (name == "libStdlib") this->builtins.libStdlib = true;
  else if (name == "libString") this->builtins.libString = true;
  else if (name == "typeStr") this->builtins.typeStr = true;
  else throw Error("Error: tried activating unknown builtin");
}

void Codegen::_activateEntity (const std::string &name, std::optional<std::vector<std::string> *> entityEntities) {
  if (entityEntities != std::nullopt) {
    auto e = *entityEntities;

    if (std::find(e->begin(), e->end(), name) == e->end()) {
      e->push_back(name);
    }

    return;
  }

  for (auto &entity : this->entities) {
    if (entity.name != name) {
      continue;
    } else if (entity.active) {
      break;
    }

    entity.active = true;

    for (const auto &builtin : entity.builtins) {
      this->_activateBuiltin(builtin);
    }

    for (const auto &child : entity.entities) {
      this->_activateEntity(child);
    }

    break;
  }
}

std::string Codegen::_block (
  const ASTBlock &block,
  std::optional<std::vector<std::string> *> entityBuiltins,
  std::optional<std::vector<std::string> *> entityEntities
) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  this->indent += 2;

  for (const auto &node : block) {
    auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node, true, entityBuiltins, entityEntities);

    setUp += nodeSetUp;
    code += nodeCode;
    cleanUp = nodeCleanUp + cleanUp;
  }

  this->indent -= 2;
  return setUp + code + cleanUp;
}

std::string Codegen::_exprAccess (
  const std::shared_ptr<ASTMemberObj> &exprAccessBody,
  std::optional<std::vector<std::string> *> entityBuiltins,
  std::optional<std::vector<std::string> *> entityEntities
) {
  if (std::holds_alternative<std::shared_ptr<Var>>(*exprAccessBody)) {
    auto id = std::get<std::shared_ptr<Var>>(*exprAccessBody);
    return Codegen::name(id->codeName);
  }

  auto member = std::get<ASTMember>(*exprAccessBody);
  auto memberObj = this->_exprAccess(member.obj, entityBuiltins, entityEntities);

  return memberObj + "->" + Codegen::name(member.prop);
}

std::string Codegen::_flags () const {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : this->flags) {
    result += (idx++ == 0 ? "" : " ") + flag;
  }

  return result;
}

CodegenNode Codegen::_node (
  const ASTNode &node,
  bool root,
  std::optional<std::vector<std::string> *> entityBuiltins,
  std::optional<std::vector<std::string> *> entityEntities
) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (std::holds_alternative<ASTNodeBreak>(*node.body)) {
    code = std::string(this->indent, ' ') + "break;" + EOL;
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeContinue>(*node.body)) {
    code = std::string(this->indent, ' ') + "continue;" EOL;
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*node.body);

    code = (root ? std::string(this->indent, ' ') : "") +
      this->_nodeExpr(nodeExpr, true, entityBuiltins, entityEntities) +
      (root ? ";" EOL : "");

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);
    auto fn = std::get<TypeFn>(nodeFnDecl.var->type->body);
    auto fnName = Codegen::name(nodeFnDecl.var->type->name);
    auto entity = CodegenEntity{fnName, CODEGEN_ENTITY_FN, {}, {}};
    auto bodyCode = std::string();
    auto paramTypes = std::string();
    auto params = std::string();

    // todo use by pointer
    if (!fn.stack.empty()) {
      auto stackName = Codegen::typeName(nodeFnDecl.var->type->name + "S");
      auto stackEntity = CodegenEntity{stackName, CODEGEN_ENTITY_OBJ};
      auto stackCode = std::string();

      for (const auto &stackVar : fn.stack) {
        auto stackVarName = Codegen::name(stackVar->codeName);
        auto stackVarType = this->_type(stackVar->type, stackVar->mut, &stackEntity.builtins, &stackEntity.entities);

        stackCode += "  " + stackVarType + stackVarName + ";" EOL;
        bodyCode += "  " + stackVarType + stackVarName + " = s." + stackVarName + ";" EOL;
      }

      stackEntity.decl = "struct " + stackName + ";";
      stackEntity.def = "struct " + stackName + " {" EOL + stackCode + "};";
      this->entities.push_back(stackEntity);
      this->_activateEntity(stackName, &entity.entities);

      paramTypes += "struct " + stackName;
      params += "struct " + stackName + " s";
    }

    this->varMap.save();
    auto tmpIndent = this->indent;
    this->indent = 2;

    // todo free in the end, otherwise leaks memory
    for (const auto &fnParam : fn.params) {
      // todo use default value if this param not passed
//      auto nodeFnDeclParam = std::find_if(nodeFnDecl.params.begin(), nodeFnDecl.params.end(), [&fnParam] (const auto &it) -> bool {
//        return it.var->name == fnParam.var->codeName;
//      });

      auto fnParamName = Codegen::name(fnParam.var->codeName);
      auto fnParamType = this->_type(fnParam.var->type, true, &entity.builtins, &entity.entities);
      auto delimiter = paramTypes.empty() ? "" : ", ";

      paramTypes += delimiter + (fnParamType.ends_with(' ') ? fnParamType.substr(0, fnParamType.size() - 1) : fnParamType);
      params += delimiter + fnParamType + fnParamName;
    }

    auto bodyCleanUp = std::string();

    for (const auto &nodeFnDeclBodyNode : nodeFnDecl.body) {
      auto [_, nodeCode, nodeCleanUp] = this->_node(nodeFnDeclBodyNode, true, &entity.builtins, &entity.entities);

      bodyCode += nodeCode;
      bodyCleanUp += nodeCleanUp;
    }

    this->indent = tmpIndent;
    this->varMap.restore();

    auto returnType = this->_type(fn.returnType, true, &entity.builtins, &entity.entities);

    entity.decl = returnType + fnName + "(" + paramTypes + ");";
    entity.def = returnType + fnName + "(" + params + ") {" EOL + bodyCode + bodyCleanUp + "}";
    this->entities.push_back(entity);

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    auto nodeIf = std::get<ASTNodeIf>(*node.body);

    this->varMap.save();
    code += std::string(this->indent, ' ') + this->_nodeIf(nodeIf, entityBuiltins, entityEntities) + EOL;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
    this->varMap.save();

    if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (1)";
    } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (" + this->_nodeExpr(*nodeLoop.cond, true, entityBuiltins, entityEntities) + ")";
    } else if (nodeLoop.init != std::nullopt && !std::get<2>(this->_node(*nodeLoop.init, true, entityBuiltins, entityEntities)).empty()) {
      code = std::string(this->indent, ' ') + "{" EOL;
      this->indent += 2;

      // todo delete setUp from entire app?
      auto [_, initCode, initCleanUp] = this->_node(*nodeLoop.init, true, entityBuiltins, entityEntities);

      code += initCode;
      code += std::string(this->indent, ' ') + "for (;";
      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, true, entityBuiltins, entityEntities)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, true, entityBuiltins, entityEntities));
      code += ") {" EOL + this->_block(nodeLoop.body, entityBuiltins, entityEntities) + std::string(this->indent, ' ') + "}" EOL;
      code += initCleanUp;

      this->indent -= 2;
      code += std::string(this->indent, ' ') + "}" EOL;
      this->varMap.restore();

      return this->_wrapNode(node, setUp, code, cleanUp);
    } else {
      code = std::string(this->indent, ' ') + "for (";
      code += (nodeLoop.init == std::nullopt ? "" : std::get<1>(this->_node(*nodeLoop.init, false, entityBuiltins, entityEntities))) + ";";
      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, true, entityBuiltins, entityEntities)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, true, entityBuiltins, entityEntities)) + ")";
    }

    code += " {" EOL + this->_block(nodeLoop.body, entityBuiltins, entityEntities) + std::string(this->indent, ' ') + "}" EOL;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(*node.body);

    this->varMap.save();
    code = this->_block(nodeMain.body, entityBuiltins, entityEntities);
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);
    auto obj = std::get<TypeObj>(nodeObjDecl.var->type->body);
    auto typeName = Codegen::typeName(nodeObjDecl.var->type->name);
    auto objEntity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ};
    auto type = this->_type(nodeObjDecl.var->type, true, &objEntity.builtins, &objEntity.entities);
    auto objCode = std::string();

    for (const auto &objField : obj.fields) {
      objCode += "  " + this->_type(objField.type, true, &objEntity.builtins, &objEntity.entities);
      objCode += Codegen::name(objField.name) + ";" EOL;
    }

    objEntity.decl = "struct " + typeName + ";";
    objEntity.def = "struct " + typeName + " {" EOL + objCode + "};";
    this->entities.push_back(objEntity);

    auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName }};
    auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName }};
    auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
    auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, {}, { typeName, typeName + "_free" }};
    auto strFnEntity = CodegenEntity{typeName + "_str", CODEGEN_ENTITY_FN, {
      "fnStrAlloc",
      "fnStrConcatCstr",
      "typeStr"
    }, { typeName, typeName + "_free" }};

    auto allocFnParamTypes = std::string();
    auto allocFnParams = std::string();
    auto allocFnCode = std::string("  " + type + "r = alloc(sizeof(struct " + typeName + "));" EOL);
    auto copyFnCode = std::string("  " + type + "r = alloc(sizeof(struct " + typeName + "));" EOL);
    auto freeFnCode = std::string();
    auto reallocFnCode = std::string("  " + typeName + "_free((" + type + ") o1);" EOL "  return o2;" EOL);
    auto strFnCode = std::string(R"(  struct str r = str_alloc(")" + nodeObjDecl.var->name + R"({");)" EOL);
    auto objFieldIdx = static_cast<std::size_t>(0);

    for (const auto &objField : obj.fields) {
      auto objFieldName = Codegen::name(objField.name);
      auto objFieldType = this->_type(objField.type, true, &objEntity.builtins, &objEntity.entities);
      auto strCodeComma = std::string(objFieldIdx == 0 ? "" : ", ");

      if (objField.type->isObj()) {
        auto objFieldTypeName = Codegen::typeName(objField.type->name);

        this->_activateEntity(objFieldTypeName + "_copy", &copyFnEntity.entities);
        this->_activateEntity(objFieldTypeName + "_free", &freeFnEntity.entities);
        this->_activateBuiltin("fnStrConcatStr", &strFnEntity.builtins);
        this->_activateEntity(objFieldTypeName + "_str", &strFnEntity.entities);

        copyFnCode += "  r->" + objFieldName + " = " + objFieldTypeName + "_copy(o->" + objFieldName + ");" EOL;
        freeFnCode += "  " + objFieldTypeName + "_free((" + objFieldType + ") o->" + objFieldName + ");" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeComma + objField.name + R"(: ");)" EOL;
        strFnCode += "  r = str_concat_str(r, " + objFieldTypeName + "_str(o->" + objFieldName + "));" EOL;
      } else if (objField.type->isStr()) {
        this->_activateBuiltin("fnStrCopy", &copyFnEntity.builtins);
        this->_activateBuiltin("fnStrFree", &freeFnEntity.builtins);
        this->_activateBuiltin("typeStr", &freeFnEntity.builtins);
        this->_activateBuiltin("fnStrConcatStr", &strFnEntity.builtins);
        this->_activateBuiltin("fnStrEscape", &strFnEntity.builtins);
        this->_activateBuiltin("typeStr", &strFnEntity.builtins);

        copyFnCode += "  r->" + objFieldName + " = str_copy(o->" + objFieldName + ");" EOL;
        freeFnCode += "  str_free((struct str) o->" + objFieldName + ");" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeComma + objField.name + R"(: \"");)" EOL;
        strFnCode += "  r = str_concat_str(r, str_escape(o->" + objFieldName + "));" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, "\"");)" EOL;
      } else if (objField.type->isAny() || objField.type->isFn() || objField.type->isVoid()) {
        auto typeStrContent = std::string();

        if (objField.type->isAny()) {
          typeStrContent = "any";
        } else if (objField.type->isFn()) {
          typeStrContent = "fn";
        } else if (objField.type->isVoid()) {
          typeStrContent = "void";
        }

        copyFnCode += "  r->" + objFieldName + " = o->" + objFieldName + ";" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeComma + objField.name + ": [" + typeStrContent + R"(]");)" EOL;
      } else {
        auto typeStrFn = std::string();

        if (objField.type->isBool()) {
          this->_activateBuiltin("fnBoolStr", &strFnEntity.builtins);
          typeStrFn = "bool_str";
        } else if (objField.type->isByte()) {
          this->_activateBuiltin("fnByteStr", &strFnEntity.builtins);
          typeStrFn = "byte_str";
        } else if (objField.type->isChar()) {
          this->_activateBuiltin("fnCharStr", &strFnEntity.builtins);
          typeStrFn = "char_str";
        } else if (objField.type->isF32()) {
          this->_activateBuiltin("fnF32Str", &strFnEntity.builtins);
          typeStrFn = "f32_str";
        } else if (objField.type->isF64()) {
          this->_activateBuiltin("fnF64Str", &strFnEntity.builtins);
          typeStrFn = "f64_str";
        } else if (objField.type->isFloat()) {
          this->_activateBuiltin("fnFloatStr", &strFnEntity.builtins);
          typeStrFn = "float_str";
        } else if (objField.type->isI8()) {
          this->_activateBuiltin("fnI8Str", &strFnEntity.builtins);
          typeStrFn = "i8_str";
        } else if (objField.type->isI16()) {
          this->_activateBuiltin("fnI16Str", &strFnEntity.builtins);
          typeStrFn = "i16_str";
        } else if (objField.type->isI32()) {
          this->_activateBuiltin("fnI32Str", &strFnEntity.builtins);
          typeStrFn = "i32_str";
        } else if (objField.type->isI64()) {
          this->_activateBuiltin("fnI64Str", &strFnEntity.builtins);
          typeStrFn = "i64_str";
        } else if (objField.type->isInt()) {
          this->_activateBuiltin("fnIntStr", &strFnEntity.builtins);
          typeStrFn = "int_str";
        } else if (objField.type->isU8()) {
          this->_activateBuiltin("fnU8Str", &strFnEntity.builtins);
          typeStrFn = "u8_str";
        } else if (objField.type->isU16()) {
          this->_activateBuiltin("fnU16Str", &strFnEntity.builtins);
          typeStrFn = "u16_str";
        } else if (objField.type->isU32()) {
          this->_activateBuiltin("fnU32Str", &strFnEntity.builtins);
          typeStrFn = "u32_str";
        } else if (objField.type->isU64()) {
          this->_activateBuiltin("fnU64Str", &strFnEntity.builtins);
          typeStrFn = "u64_str";
        }

        this->_activateBuiltin("fnStrConcatStr", &strFnEntity.builtins);

        copyFnCode += "  r->" + objFieldName + " = o->" + objFieldName + ";" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeComma + objField.name + R"(: ");)" EOL;
        strFnCode += "  r = str_concat_str(r, " + typeStrFn + "(o->" + objFieldName + "));" EOL;
      }

      allocFnParamTypes += ", " + (objFieldType.ends_with(" ") ? objFieldType.substr(0, objFieldType.size() - 1) : objFieldType);
      allocFnParams += ", " + objFieldType + objFieldName;
      allocFnCode += "  r->" + objFieldName + " = " + objFieldName + ";" EOL;

      objFieldIdx++;
    }

    allocFnCode += "  return r;" EOL;
    copyFnCode += "  return r;" EOL;
    freeFnCode += "  free(o);" EOL;
    strFnCode += "  " + typeName + "_free(o);" EOL + R"(  return str_concat_cstr(r, "}");)" EOL;

    allocFnEntity.decl = type + typeName + "_alloc (" + allocFnParamTypes.substr(2) + ");";
    allocFnEntity.def = type + typeName + "_alloc (" + allocFnParams.substr(2) + ") {" EOL + allocFnCode + "}";
    copyFnEntity.decl = type + typeName + "_copy (" + type + ");";
    copyFnEntity.def = type + typeName + "_copy (" + type + "o) {" EOL + copyFnCode + "}";
    freeFnEntity.decl = "void " + typeName + "_free (" + type + ");";
    freeFnEntity.def = "void " + typeName + "_free (" + type + "o) {" EOL + freeFnCode + "}";
    reallocFnEntity.decl = type + typeName + "_realloc (" + type + ", " + type + ");";
    reallocFnEntity.def = type + typeName + "_realloc (" + type + "o1, " + type + "o2) {" EOL + reallocFnCode + "}";
    strFnEntity.decl = "struct str " + typeName + "_str (" + type + ");";
    strFnEntity.def = "struct str " + typeName + "_str (" + type + "o) {" EOL + strFnCode + "}";

    this->entities.push_back(allocFnEntity);
    this->entities.push_back(copyFnEntity);
    this->entities.push_back(freeFnEntity);
    this->entities.push_back(reallocFnEntity);
    this->entities.push_back(strFnEntity);

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

    code = std::string(this->indent, ' ') + "return";
    code += (nodeReturn.body == std::nullopt ? "" : this->_nodeExpr(*nodeReturn.body, false, entityBuiltins, entityEntities)) + ";" EOL;

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
    auto name = Codegen::name(nodeVarDecl.var->codeName);
    auto type = this->_type(nodeVarDecl.var->type, nodeVarDecl.var->mut, entityBuiltins, entityEntities);
    auto initCode = std::string("0");

    if (nodeVarDecl.init != std::nullopt) {
      initCode = this->_nodeExpr(*nodeVarDecl.init, false, entityBuiltins, entityEntities);
    } else if (nodeVarDecl.var->type->isBool()) {
      this->_activateBuiltin("libStdbool", entityBuiltins);
      initCode = "false";
    } else if (nodeVarDecl.var->type->isChar()) {
      initCode = R"('\0')";
    } else if (nodeVarDecl.var->type->isStr()) {
      this->_activateBuiltin("fnStrAlloc", entityBuiltins);
      initCode = R"(str_alloc(""))";
    }

    code = (root ? std::string(this->indent, ' ') : "") + type + name + " = " + initCode + (root ? ";" EOL : "");

    if (nodeVarDecl.var->type->isObj()) {
      auto cleanUpType = this->_type(nodeVarDecl.var->type, true, entityBuiltins, entityEntities);
      auto cleanUpTypeName = Codegen::typeName(nodeVarDecl.var->type->name);

      this->_activateEntity(cleanUpTypeName + "_free", entityEntities);
      cleanUp = std::string(this->indent, ' ') + cleanUpTypeName + "_free((" + cleanUpType + ") " + name + ");" EOL;
    } else if (nodeVarDecl.var->type->isStr()) {
      this->_activateBuiltin("fnStrFree", entityBuiltins);
      cleanUp = std::string(this->indent, ' ') + "str_free((struct str) " + name + ");" EOL;
    }

    return this->_wrapNode(node, setUp, code, cleanUp);
  }

  throw Error("Error: tried to generate code for unknown node");
}

std::string Codegen::_nodeIf (
  const ASTNodeIf &nodeIf,
  std::optional<std::vector<std::string> *> entityBuiltins,
  std::optional<std::vector<std::string> *> entityEntities
) {
  auto code = "if (" + this->_nodeExpr(nodeIf.cond) + ") {" EOL + this->_block(nodeIf.body, entityBuiltins, entityEntities);

  if (nodeIf.alt != std::nullopt) {
    code += std::string(this->indent, ' ') + "} else ";

    if (std::holds_alternative<ASTBlock>(**nodeIf.alt)) {
      code += "{" EOL;
      code += this->_block(std::get<ASTBlock>(**nodeIf.alt), entityBuiltins, entityEntities);
      code += std::string(this->indent, ' ') + "}";
    } else if (std::holds_alternative<ASTNodeIf>(**nodeIf.alt)) {
      code += this->_nodeIf(std::get<ASTNodeIf>(**nodeIf.alt), entityBuiltins, entityEntities);
    }
  } else {
    code += std::string(this->indent, ' ') + "}";
  }

  return code;
}

std::string Codegen::_nodeExpr (
  const ASTNodeExpr &nodeExpr,
  bool root,
  std::optional<std::vector<std::string> *> entityBuiltins,
  std::optional<std::vector<std::string> *> entityEntities
) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
    auto code = this->_exprAccess(exprAccess.body, entityBuiltins, entityEntities);

    if (!root && nodeExpr.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);
      code = typeName + "_copy(" + code + ")";

      this->_activateEntity(typeName + "_copy", entityEntities);
    } else if (!root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrCopy", entityBuiltins);
      code = "str_copy(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto leftCode = this->_exprAccess(exprAssign.left.body, entityBuiltins, entityEntities);

    if (exprAssign.right.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);
      auto code = leftCode + " = " + typeName + "_realloc(" + leftCode + ", " +
        this->_nodeExpr(exprAssign.right, false, entityBuiltins, entityEntities) + ")";

      if (!root && nodeExpr.type->isStr()) {
        code = typeName + "_copy(" + code + ")";
      }

      this->_activateEntity(typeName + "_realloc", entityEntities);
      return this->_wrapNodeExpr(nodeExpr, code);
    } else if (exprAssign.right.type->isStr()) {
      auto rightCode = std::string();

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        if (exprAssign.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr", entityBuiltins);
          this->_activateBuiltin("fnStrCopy", entityBuiltins);

          rightCode = "str_concat_cstr(str_copy(" + leftCode + "), " + exprAssign.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr", entityBuiltins);
          this->_activateBuiltin("fnStrCopy", entityBuiltins);

          rightCode = "str_concat_str(str_copy(" + leftCode + "), " +
            this->_nodeExpr(exprAssign.right, false, entityBuiltins, entityEntities) + ")";
        }
      } else {
        rightCode = this->_nodeExpr(exprAssign.right, false, entityBuiltins, entityEntities);
      }

      this->_activateBuiltin("fnStrRealloc", entityBuiltins);
      auto code = leftCode + " = str_realloc(" + leftCode + ", " + rightCode + ")";

      if (!root && nodeExpr.type->isStr()) {
        this->_activateBuiltin("fnStrCopy", entityBuiltins);
        code = "str_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }

    auto opCode = std::string(" = ");
    auto rightCode = this->_nodeExpr(exprAssign.right, false, entityBuiltins, entityEntities);

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

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (exprBinary.left.type->isStr() && exprBinary.right.type->isStr()) {
      if (exprBinary.op == AST_EXPR_BINARY_ADD) {
        auto code = std::string();

        if (root && nodeExpr.isLit()) {
          return this->_wrapNodeExpr(nodeExpr, nodeExpr.litBody());
        } else if (nodeExpr.isLit()) {
          this->_activateBuiltin("fnStrAlloc", entityBuiltins);
          code = "str_alloc(" + nodeExpr.litBody() + ")";
        } else if (exprBinary.left.isLit()) {
          this->_activateBuiltin("fnCstrConcatStr", entityBuiltins);
          code = "cstr_concat_str(" + exprBinary.left.litBody() + ", " +
            this->_nodeExpr(exprBinary.right, false, entityBuiltins, entityEntities) + ")";
        } else if (exprBinary.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr", entityBuiltins);
          code = "str_concat_cstr(" + this->_nodeExpr(exprBinary.left, false, entityBuiltins, entityEntities) + ", " +
            exprBinary.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr", entityBuiltins);
          code = "str_concat_str(" + this->_nodeExpr(exprBinary.left, false, entityBuiltins, entityEntities) + ", " +
            this->_nodeExpr(exprBinary.right, false, entityBuiltins, entityEntities) + ")";
        }

        if (root) {
          this->_activateBuiltin("fnStrFree", entityBuiltins);
          code = "str_free(" + code + ")";
        }

        return this->_wrapNodeExpr(nodeExpr, code);
      } else if (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) {
        auto opName = std::string(exprBinary.op == AST_EXPR_BINARY_NE ? "ne" : "eq");

        if (nodeExpr.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnCstrEqCstr", entityBuiltins);
          } else {
            this->_activateBuiltin("fnCstrNeCstr", entityBuiltins);
          }

          auto code = "cstr_" + opName + "_cstr(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")";
          return this->_wrapNodeExpr(nodeExpr, code);
        } else if (exprBinary.left.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnCstrEqStr", entityBuiltins);
          } else {
            this->_activateBuiltin("fnCstrNeStr", entityBuiltins);
          }

          auto rightCode = this->_nodeExpr(exprBinary.right, false, entityBuiltins, entityEntities);
          return this->_wrapNodeExpr(nodeExpr, "cstr_" + opName + "_str(" + exprBinary.left.litBody() + ", " + rightCode + ")");
        } else if (exprBinary.right.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnStrEqCstr", entityBuiltins);
          } else {
            this->_activateBuiltin("fnStrNeCstr", entityBuiltins);
          }

          auto leftCode = this->_nodeExpr(exprBinary.left, false, entityBuiltins, entityEntities);
          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")");
        } else {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnStrEqStr", entityBuiltins);
          } else {
            this->_activateBuiltin("fnStrNeStr", entityBuiltins);
          }

          auto leftCode = this->_nodeExpr(exprBinary.left, false, entityBuiltins, entityEntities);
          auto rightCode = this->_nodeExpr(exprBinary.right, false, entityBuiltins, entityEntities);

          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_str(" + leftCode + ", " + rightCode + ")");
        }
      }
    }

    auto leftCode = this->_nodeExpr(exprBinary.left, false, entityBuiltins, entityEntities);
    auto rightCode = this->_nodeExpr(exprBinary.right, false, entityBuiltins, entityEntities);
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

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) {
    auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);

    if (exprCall.calleeType->builtin) {
      if (exprCall.calleeType->name == "@print") {
        this->_activateBuiltin("fnPrint", entityBuiltins);
        this->_activateBuiltin("libStdio", entityBuiltins);

        auto separator = std::string(R"(" ")");
        auto isSeparatorLit = true;
        auto terminator = std::string(R"(")" ESC_EOL R"(")");
        auto isTerminatorLit = true;

        for (const auto &exprCallArg : exprCall.args) {
          if (exprCallArg.id != std::nullopt && exprCallArg.id == "separator") {
            if (exprCallArg.expr.isLit()) {
              separator = exprCallArg.expr.litBody();
            } else {
              separator = this->_nodeExpr(exprCallArg.expr, false, entityBuiltins, entityEntities);
              isSeparatorLit = false;
            }
          } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "terminator") {
            if (exprCallArg.expr.isLit()) {
              terminator = exprCallArg.expr.litBody();
            } else {
              terminator = this->_nodeExpr(exprCallArg.expr, false, entityBuiltins, entityEntities);
              isTerminatorLit = false;
            }
          }
        }

        auto code = std::string(R"(print(stdout, ")");
        auto argsCode = std::string();
        auto argIdx = static_cast<std::size_t>(0);

        for (const auto &exprCallArg : exprCall.args) {
          if (exprCallArg.id != std::nullopt) {
            continue;
          }

          if (argIdx != 0 && separator != R"("")") {
            code += isSeparatorLit ? "%z" : "%s";
            argsCode += separator + ", ";
          }

          if (exprCallArg.expr.type->isObj()) {
            auto typeName = Codegen::typeName(exprCallArg.expr.type->name);
            this->_activateEntity(typeName + "_str", entityEntities);

            code += "%s";
            argsCode += typeName + "_str(" + this->_nodeExpr(exprCallArg.expr, false, entityBuiltins, entityEntities) + ")";
          } else if (exprCallArg.expr.type->isStr() && exprCallArg.expr.isLit()) {
            code += "%z";
            argsCode += exprCallArg.expr.litBody();
          } else {
            if (exprCallArg.expr.type->isBool()) code += "%t";
            else if (exprCallArg.expr.type->isByte()) code += "%b";
            else if (exprCallArg.expr.type->isChar()) code += "%c";
            else if (exprCallArg.expr.type->isF32()) code += "%e";
            else if (exprCallArg.expr.type->isF64()) code += "%g";
            else if (exprCallArg.expr.type->isFloat()) code += "%f";
            else if (exprCallArg.expr.type->isI8()) code += "%h";
            else if (exprCallArg.expr.type->isI16()) code += "%j";
            else if (exprCallArg.expr.type->isI32()) code += "%k";
            else if (exprCallArg.expr.type->isI64()) code += "%l";
            else if (exprCallArg.expr.type->isInt()) code += "%i";
            else if (exprCallArg.expr.type->isStr()) code += "%s";
            else if (exprCallArg.expr.type->isU8()) code += "%v";
            else if (exprCallArg.expr.type->isU16()) code += "%w";
            else if (exprCallArg.expr.type->isU32()) code += "%u";
            else if (exprCallArg.expr.type->isU64()) code += "%y";

            argsCode += this->_nodeExpr(exprCallArg.expr, false, entityBuiltins, entityEntities);
          }

          argsCode += ", ";
          argIdx++;
        }

        if (terminator == R"("")") {
          code += R"(", )" + argsCode.substr(0, argsCode.size() - 2);
        } else {
          code += std::string(isTerminatorLit ? "%z" : "%s") + R"(", )" + argsCode + terminator;
        }

        return this->_wrapNodeExpr(nodeExpr, code + ")");
      }
    } else {
      auto fn = std::get<TypeFn>(exprCall.calleeType->body);
      auto fnName = Codegen::name(exprCall.calleeType->name);
      auto paramsCode = std::string();

      if (!fn.stack.empty()) {
        paramsCode += "(struct " + Codegen::typeName(exprCall.calleeType->name + "S") + ") {";

        for (const auto &stackVar : fn.stack) {
          paramsCode += Codegen::name(stackVar->codeName) + ", ";
        }

        paramsCode = paramsCode.substr(0, paramsCode.size() - 2) + "}";
      }

      auto paramIdx = static_cast<std::size_t>(0);

      for (const auto &param : fn.params) {
        auto foundArg = std::optional<ASTExprCallArg>{};

        if (paramIdx < exprCall.args.size() && exprCall.args[paramIdx].id == std::nullopt) {
          foundArg = exprCall.args[paramIdx];
        } else {
          for (const auto &exprCallArg : exprCall.args) {
            if (exprCallArg.id == param.var->name) {
              foundArg = exprCallArg;
              break;
            }
          }
        }

        paramsCode += (paramsCode.empty() ? "" : ", ") + this->_nodeExpr(foundArg->expr, false, entityBuiltins, entityEntities);
        paramIdx++;
      }

      auto code = fnName + "(" + paramsCode + ")";
      this->_activateEntity(fnName, entityEntities);

      if (root && nodeExpr.type->isObj()) {
        auto typeName = Codegen::typeName(nodeExpr.type->name);
        this->_activateEntity(typeName + "_free", entityEntities);

        code = typeName + "_free(" + code + ")";
      } else if (root && nodeExpr.type->isStr()) {
        this->_activateBuiltin("fnStrFree", entityBuiltins);
        code = "str_free(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto condCode = this->_nodeExpr(exprCond.cond, false, entityBuiltins, entityEntities);
    auto bodyCode = this->_nodeExpr(exprCond.body, false, entityBuiltins, entityEntities);
    auto altCode = this->_nodeExpr(exprCond.alt, false, entityBuiltins, entityEntities);

    if (std::holds_alternative<ASTExprAssign>(*exprCond.alt.body) && !exprCond.alt.parenthesized && !exprCond.alt.type->isStr()) {
      altCode = "(" + altCode + ")";
    }

    auto code = condCode + " ? " + bodyCode + " : " + altCode;

    if (root && nodeExpr.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);
      this->_activateEntity(typeName + "_free", entityEntities);

      code = typeName + "_free(" + code + ")";
    } else if (root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrFree", entityBuiltins);
      code = "str_free(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprLit>(*nodeExpr.body)) {
    auto exprLit = std::get<ASTExprLit>(*nodeExpr.body);

    if (exprLit.type == AST_EXPR_LIT_BOOL) {
      this->_activateBuiltin("libStdbool", entityBuiltins);
    } else if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
      auto val = std::stoull(exprLit.body);

      if (val > 9223372036854775807) {
        return this->_wrapNodeExpr(nodeExpr, exprLit.body + "U");
      }
    } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      auto val = exprLit.body;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      return this->_wrapNodeExpr(nodeExpr, val);
    } else if (!root && exprLit.type == AST_EXPR_LIT_STR) {
      this->_activateBuiltin("fnStrAlloc", entityBuiltins);
      return this->_wrapNodeExpr(nodeExpr, "str_alloc(" + exprLit.body + ")");
    }

    return this->_wrapNodeExpr(nodeExpr, exprLit.body);
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
    auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);
    auto obj = std::get<TypeObj>(exprObj.type->body);
    auto typeName = Codegen::typeName(exprObj.type->name);
    auto fieldsCode = std::string();

    for (const auto &objField : obj.fields) {
      auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&objField] (const auto &it) -> bool {
        return it.id == objField.name;
      });

      fieldsCode += ", " + this->_nodeExpr(exprObjProp->init, false, entityBuiltins, entityEntities);
    }

    this->_activateEntity(typeName + "_alloc", entityEntities);
    auto code = typeName + "_alloc(" + fieldsCode.substr(2) + ")";

    if (root) {
      this->_activateEntity(typeName + "_free", entityEntities);
      code = typeName + "_free(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);
    auto argCode = this->_nodeExpr(exprUnary.arg, false, entityBuiltins, entityEntities);
    auto opCode = std::string();

    if (exprUnary.op == AST_EXPR_UNARY_BIT_NOT) opCode = "~";
    else if (exprUnary.op == AST_EXPR_UNARY_DECREMENT) opCode = "--";
    else if (exprUnary.op == AST_EXPR_UNARY_INCREMENT) opCode = "++";
    else if (exprUnary.op == AST_EXPR_UNARY_MINUS) opCode = "-";
    else if (exprUnary.op == AST_EXPR_UNARY_NOT) opCode = "!";
    else if (exprUnary.op == AST_EXPR_UNARY_PLUS) opCode = "+";

    if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isFloatNumber()) {
      this->_activateBuiltin("libStdbool", entityBuiltins);
      argCode = "((bool) " + argCode + ")";
    } else if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isStr()) {
      this->_activateBuiltin("fnStrNot", entityBuiltins);
      argCode = "str_not(" + argCode + ")";
      opCode = "";
    }

    return this->_wrapNodeExpr(nodeExpr, exprUnary.prefix ? opCode + argCode : argCode + opCode);
  }

  throw Error("Error: tried to generate code for unknown expression");
}

std::string Codegen::_type (
  const Type *type,
  bool mut,
  std::optional<std::vector<std::string> *> entityBuiltins,
  std::optional<std::vector<std::string> *> entityEntities
) {
  auto typeName = std::string();

  if (type->isAny()) {
    typeName = "void *";
    return std::string(mut ? "" : "const ") + "void *";
  } else if (type->isByte()) {
    typeName = "unsigned char";
  } else if (type->isChar()) {
    typeName = "char";
  } else if (type->isF32()) {
    typeName = "float";
  } else if (type->isF64() || type->isFloat()) {
    typeName = "double";
  } else if (type->isBool()) {
    this->_activateBuiltin("libStdbool", entityBuiltins);
    typeName = "bool";
  } else if (type->isI8()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "int8_t";
  } else if (type->isI16()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "int16_t";
  } else if (type->isI32() || type->isInt()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "int32_t";
  } else if (type->isI64()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "int64_t";
  } else if (type->isObj()) {
    typeName = Codegen::typeName(type->name);
    this->_activateEntity(typeName, entityEntities);

    return std::string(mut ? "" : "const ") + "struct " + typeName + " *";
  } else if (type->isStr()) {
    this->_activateBuiltin("typeStr", entityBuiltins);
    typeName = "struct str";
  } else if (type->isU8()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "uint8_t";
  } else if (type->isU16()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "uint16_t";
  } else if (type->isU32()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "uint32_t";
  } else if (type->isU64()) {
    this->_activateBuiltin("libStdint", entityBuiltins);
    typeName = "uint64_t";
  } else if (type->isVoid()) {
    return std::string(mut ? "" : "const ") + "void ";
  }

  return std::string(mut ? "" : "const ") + typeName + " ";
}

CodegenNode Codegen::_wrapNode (
  [[maybe_unused]] const ASTNode &node,
  const std::string &setUp,
  const std::string &code,
  const std::string &cleanUp
) const {
  return std::make_tuple(setUp, code, cleanUp);
}

std::string Codegen::_wrapNodeExpr (const ASTNodeExpr &nodeExpr, const std::string &code) const {
  auto result = code;

  if (nodeExpr.parenthesized) {
    result = "(" + result + ")";
  }

  return result;
}
