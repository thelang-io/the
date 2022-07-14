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
#include "ASTChecker.hpp"
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
  // todo delete setUp from entire app?
  auto mainSetUp = std::string();
  auto mainCode = std::string();
  auto mainCleanUp = std::string();

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

      mainSetUp += nodeSetUp;
      mainCode += nodeCode;
      mainCleanUp = nodeCleanUp + mainCleanUp;
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
  output += "int main () {" EOL;
  output += mainSetUp;
  output += mainCode;
  output += mainCleanUp;
  output += "}" EOL;

  return std::make_tuple(output, this->_flags());
}

void Codegen::_activateBuiltin (const std::string &name, std::optional<std::vector<std::string> *> entityBuiltins) {
  if (entityBuiltins != std::nullopt || this->state.builtins != std::nullopt) {
    auto b = entityBuiltins == std::nullopt ? *this->state.builtins : *entityBuiltins;

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

std::tuple<std::string, std::string> Codegen::_block (const ASTBlock &nodes, bool saveCleanUp) {
  auto initialIndent = this->indent;
  auto initialCleanUp = this->state.cleanUp;
  auto code = std::string();

  if (saveCleanUp) {
    this->state.cleanUp = CodegenCleanUp(this->state.cleanUp.labelIdx);
  }

  this->indent += 2;

  for (const auto &node : nodes) {
    if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
      auto fnDeclSaveCleanUp = this->state.cleanUp;
      this->state.cleanUp = CodegenCleanUp{this->state.cleanUp.labelIdx};

      auto [_1, nodeCode, _2] = this->_node(node);
      code += nodeCode;

      this->state.cleanUp = fnDeclSaveCleanUp.update(this->state.cleanUp.labelIdx);
      continue;
    }

    auto [_1, nodeCode, _2] = this->_node(node);
    code += nodeCode;
  }

  if (saveCleanUp) {
    code += this->state.cleanUp.gen(this->indent);
    this->state.cleanUp = initialCleanUp.update(this->state.cleanUp.labelIdx);

    if (
      ASTChecker(nodes).has<ASTNodeReturn>() &&
      !ASTChecker(nodes).endsWith<ASTNodeReturn>() &&
      !this->state.cleanUp.empty()
    ) {
      code += std::string(this->indent, ' ') + "if (r == 1) goto " + this->state.cleanUp.currentLabel() + ";" EOL;
    }
  }

  this->indent = initialIndent;
  return std::make_tuple(code, "");
}

std::string Codegen::_exprAccess (const std::shared_ptr<ASTMemberObj> &exprAccessBody) {
  if (std::holds_alternative<std::shared_ptr<Var>>(*exprAccessBody)) {
    auto id = std::get<std::shared_ptr<Var>>(*exprAccessBody);
    auto code = Codegen::name(id->codeName);

    if (this->state.contextVars.contains(code)) {
      code = "*" + code;
    }

    return code;
  }

  auto member = std::get<ASTMember>(*exprAccessBody);
  auto memberObj = this->_exprAccess(member.obj);

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

CodegenNode Codegen::_node (const ASTNode &node, bool root) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (std::holds_alternative<ASTNodeBreak>(*node.body)) {
    code = std::string(this->indent, ' ') + "break;" EOL;
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeContinue>(*node.body)) {
    code = std::string(this->indent, ' ') + "continue;" EOL;
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*node.body);
    code = this->_nodeExpr(nodeExpr, true);

    if (root) {
      code = std::string(this->indent, ' ') + code + ";" EOL;
    }

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);
    auto fn = std::get<TypeFn>(nodeFnDecl.var->type->body);
    auto fnId = this->_typeFnId(nodeFnDecl.var->type);
    auto fnName = Codegen::name(nodeFnDecl.var->type->name);
    auto allocFnName = Codegen::typeName(nodeFnDecl.var->type->name + "A");
    auto contextName = Codegen::typeName(nodeFnDecl.var->type->name + "C");
    auto functorName = Codegen::typeName(nodeFnDecl.var->type->name + "F");
    auto paramsName = fnId + "P";

    auto functorEntity = CodegenEntity{functorName, CODEGEN_ENTITY_FN, {}, {
      fnId,
      contextName,
      paramsName
    }};

    auto allocFnEntity = CodegenEntity{allocFnName, CODEGEN_ENTITY_FN, { "fnAlloc", "libString" }, {
      fnId,
      functorName,
      contextName
    }};

    auto saveIndent = this->indent;
    auto saveStateBuiltins = this->state.builtins;
    auto saveStateEntities = this->state.entities;
    auto saveStateCleanUp = this->state.cleanUp;
    auto saveStateContextVars = this->state.contextVars;

    this->varMap.save();
    this->indent = 2;
    this->state.builtins = &functorEntity.builtins;
    this->state.entities = &functorEntity.entities;
    this->state.cleanUp = CodegenCleanUp{this->state.cleanUp.labelIdx};

    auto bodyCode = std::string();

    if (!nodeFnDecl.stack.empty()) {
      bodyCode += "  struct " + contextName + " *c = _c;" EOL;

      auto contextEntity = CodegenEntity{contextName, CODEGEN_ENTITY_OBJ};
      auto contextCode = std::string();

      for (const auto &contextVar : nodeFnDecl.stack) {
        auto contextVarName = Codegen::name(contextVar->codeName);
        auto contextVarType = this->_type(contextVar->type, contextVar->mut, true);

        contextCode += "  " + contextVarType + contextVarName + ";" EOL;
        bodyCode += "  " + contextVarType + contextVarName + " = c->" + contextVarName + ";" EOL;

        this->state.contextVars.insert(contextVarName);
      }

      contextEntity.decl = "struct " + contextName + ";";
      contextEntity.def = "struct " + contextName + " {" EOL + contextCode + "};";

      this->entities.push_back(contextEntity);
    }

    if (!nodeFnDecl.params.empty()) {
      auto paramIdx = static_cast<std::size_t>(0);

      for (const auto &param : nodeFnDecl.params) {
        auto paramName = Codegen::name(param.var->codeName);
        auto paramType = this->_type(param.var->type, false);
        auto paramIdxStr = std::to_string(paramIdx);

        bodyCode += "  " + paramType + paramName + " = ";

        if (param.init == std::nullopt) {
          bodyCode += "p.n" + paramIdxStr;
        } else {
          auto initCode = this->_nodeExpr(*param.init);

          bodyCode += "p.o" + paramIdxStr + " == 1 ? p.n" + paramIdxStr + " : " + initCode;
        }

        bodyCode += ";" EOL;

        if (param.var->type->isObj()) {
          auto cleanUpType = this->_type(param.var->type, true);
          auto cleanUpTypeName = Codegen::typeName(param.var->type->name);

          this->_activateEntity(cleanUpTypeName + "_free");
          this->state.cleanUp.add(cleanUpTypeName + "_free((" + cleanUpType + ") " + paramName + ");");
        } else if (param.var->type->isStr()) {
          this->_activateBuiltin("fnStrFree");
          this->state.cleanUp.add("str_free((struct str) " + paramName + ");");
        }

        paramIdx++;
      }
    }

    this->indent = 0;
    auto [bodyBlockCode, _] = this->_block(nodeFnDecl.body, false);
    this->indent = 2;

    bodyCode += bodyBlockCode;

    this->varMap.restore();
    auto returnType = this->_type(fn.returnType, true);

    if (!fn.returnType->isVoid() && !this->state.cleanUp.empty()) {
      auto bodyCodeStart = std::string(this->indent, ' ') + "unsigned char r = 0;" EOL;
      bodyCodeStart += std::string(this->indent, ' ') + returnType + "v;" EOL;
      auto bodyCodeEnd = std::string(this->indent, ' ') + "return v;" EOL;

      bodyCode = bodyCodeStart + bodyCode + this->state.cleanUp.gen(this->indent) + bodyCodeEnd;
    } else if (!this->state.cleanUp.empty()) {
      bodyCode = bodyCode + this->state.cleanUp.gen(this->indent);
    }

    if (nodeFnDecl.params.empty()) {
      functorEntity.decl = returnType + functorName + " (void *);";
      functorEntity.def = returnType + functorName + " (void *_c) {" EOL + bodyCode + "}";
    } else {
      functorEntity.decl = returnType + functorName + " (void *, struct " + paramsName + ");";
      functorEntity.def = returnType + functorName + " (void *_c, struct " + paramsName + " p) {" EOL + bodyCode + "}";
    }

    this->entities.push_back(functorEntity);

    allocFnEntity.decl = "void " + allocFnName + " (struct " + fnId + " *, struct " + contextName + ");";
    allocFnEntity.def = "void " + allocFnName + " (struct " + fnId + " *n, struct " + contextName + " c) {" EOL;
    allocFnEntity.def += "  size_t l = sizeof(struct " + contextName + ");" EOL;
    allocFnEntity.def += "  struct " + contextName + " *r = alloc(l);" EOL;
    allocFnEntity.def += "  memcpy(r, &c, l);" EOL;
    allocFnEntity.def += "  n->f = &" + functorName + ";" EOL;
    allocFnEntity.def += "  n->c = r;" EOL;
    allocFnEntity.def += "}";
    this->entities.push_back(allocFnEntity);

    this->indent = saveIndent;
    this->state.builtins = saveStateBuiltins;
    this->state.entities = saveStateEntities;
    this->state.cleanUp = saveStateCleanUp.update(this->state.cleanUp.labelIdx);
    this->state.contextVars = saveStateContextVars;

    code = std::string(this->indent == 0 ? 2 : this->indent, ' ');
    code += "const struct " + fnId + " " + fnName;
    this->_activateEntity(fnId);

    if (nodeFnDecl.stack.empty()) {
      code += " = (struct " + fnId + ") {&" + functorName + ", NULL};";

      this->_activateBuiltin("libStdlib");
      this->_activateEntity(functorName);
    } else {
      code += ";" EOL;
      code += std::string(this->indent, ' ') + allocFnName + "((struct " + fnId + " *) &" + fnName + ", ";
      code += "(struct " + contextName + ") {";

      this->_activateEntity(allocFnName);
      this->_activateEntity(contextName);

      auto contextVarIdx = static_cast<std::size_t>(0);

      for (const auto &contextVar : nodeFnDecl.stack) {
        auto contextVarName = Codegen::name(contextVar->codeName);
        auto contextVarType = this->_type(contextVar->type, contextVar->mut, true);

        code += contextVarIdx == 0 ? "" : ", ";
        code += (this->state.contextVars.contains(contextVarName) ? "" : "&") + contextVarName;

        contextVarIdx++;
      }

      code += "});";
    }

    return this->_wrapNode(node, setUp, code + EOL, cleanUp);
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    auto nodeIf = std::get<ASTNodeIf>(*node.body);

    this->varMap.save();
    code += std::string(this->indent, ' ') + this->_nodeIf(nodeIf) + EOL;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
    this->varMap.save();

    if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (1)";
    } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (" + this->_nodeExpr(*nodeLoop.cond, true) + ")";
    } else if (nodeLoop.init != std::nullopt) {
      auto initialCleanUp = this->state.cleanUp;
      auto initialIndent = this->indent;

      this->state.cleanUp = CodegenCleanUp{this->state.cleanUp.labelIdx};

      this->indent += 2;
      auto [_1, initCode, _2] = this->_node(*nodeLoop.init);
      this->indent = initialIndent;

      if (this->state.cleanUp.empty()) {
        auto [_3, nodeExprCode, _4] = this->_node(*nodeLoop.init, false);
        code = std::string(this->indent, ' ') + "for (" + nodeExprCode + ";";
      } else {
        code = std::string(this->indent, ' ') + "{" EOL + initCode;
        this->indent += 2;
        code += std::string(this->indent, ' ') + "for (;";
      }

      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, true)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, true));

      auto [bodyCode, _3] = this->_block(nodeLoop.body);
      code += ") {" EOL + bodyCode + std::string(this->indent, ' ') + "}" EOL;

      if (!this->state.cleanUp.empty()) {
        code += this->state.cleanUp.gen(this->indent);
        this->indent = initialIndent;
        code += std::string(this->indent, ' ') + "}" EOL;
      }

      this->state.cleanUp = initialCleanUp.update(this->state.cleanUp.labelIdx);
      this->varMap.restore();

      return this->_wrapNode(node, setUp, code, cleanUp);
    } else {
      code = std::string(this->indent, ' ') + "for (;";
      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, true)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, true)) + ")";
    }

    auto [bodyCode, bodyCleanUp] = this->_block(nodeLoop.body);
    code += " {" EOL + bodyCode + bodyCleanUp + std::string(this->indent, ' ') + "}" EOL;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(*node.body);
    auto [bodyCode, bodyCleanUp] = this->_block(nodeMain.body);

    this->varMap.save();
    code = bodyCode + bodyCleanUp;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);
    auto obj = std::get<TypeObj>(nodeObjDecl.var->type->body);
    auto typeName = Codegen::typeName(nodeObjDecl.var->type->name);
    auto objEntity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ};

    auto saveStateBuiltins = this->state.builtins;
    auto saveStateEntities = this->state.entities;

    this->state.builtins = &objEntity.builtins;
    this->state.entities = &objEntity.entities;

    auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName }};
    auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName }};
    auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
    auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, {}, { typeName, typeName + "_free" }};
    auto strFnEntity = CodegenEntity{typeName + "_str", CODEGEN_ENTITY_FN, {
      "fnStrAlloc",
      "fnStrConcatCstr",
      "typeStr"
    }, { typeName, typeName + "_free" }};

    auto type = this->_type(nodeObjDecl.var->type, true);
    auto bodyCode = std::string();
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
      auto objFieldType = this->_type(objField.type, true);
      auto strCodeDelimiter = std::string(objFieldIdx == 0 ? "" : ", ");

      if (objField.type->isObj()) {
        auto objFieldTypeName = Codegen::typeName(objField.type->name);

        this->_activateEntity(objFieldTypeName + "_copy", &copyFnEntity.entities);
        this->_activateEntity(objFieldTypeName + "_free", &freeFnEntity.entities);
        this->_activateBuiltin("fnStrConcatStr", &strFnEntity.builtins);
        this->_activateEntity(objFieldTypeName + "_str", &strFnEntity.entities);

        copyFnCode += "  r->" + objFieldName + " = " + objFieldTypeName + "_copy(o->" + objFieldName + ");" EOL;
        freeFnCode += "  " + objFieldTypeName + "_free((" + objFieldType + ") o->" + objFieldName + ");" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + R"(: ");)" EOL;
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
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + R"(: \"");)" EOL;
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
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + ": [" + typeStrContent + R"(]");)" EOL;
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
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + R"(: ");)" EOL;
        strFnCode += "  r = str_concat_str(r, " + typeStrFn + "(o->" + objFieldName + "));" EOL;
      }

      bodyCode += "  " + objFieldType + objFieldName + ";" EOL;
      allocFnParamTypes += ", " + (objFieldType.ends_with(" ") ? objFieldType.substr(0, objFieldType.size() - 1) : objFieldType);
      allocFnParams += ", " + objFieldType + objFieldName;
      allocFnCode += "  r->" + objFieldName + " = " + objFieldName + ";" EOL;

      objFieldIdx++;
    }

    objEntity.decl = "struct " + typeName + ";";
    objEntity.def = "struct " + typeName + " {" EOL + bodyCode + "};";
    this->entities.push_back(objEntity);

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

    this->state.builtins = saveStateBuiltins;
    this->state.entities = saveStateEntities;

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

    if (!this->state.cleanUp.empty() && nodeReturn.body != std::nullopt) {
      if (nodeReturn.body != std::nullopt) {
        code = std::string(this->indent, ' ') + "v = " + this->_nodeExpr(*nodeReturn.body) + ";" EOL;
      }

      if (!ASTChecker(node.parent).is<ASTNodeFnDecl>() || !ASTChecker(node).isLast()) {
        code += std::string(this->indent, ' ') + "r = 1;" EOL;
        code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
      }
    } else if (nodeReturn.body != std::nullopt) {
      code = std::string(this->indent, ' ') + "return " + this->_nodeExpr(*nodeReturn.body) + ";" EOL;
    } else {
      code = std::string(this->indent, ' ') + "return";
    }

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
    auto name = Codegen::name(nodeVarDecl.var->codeName);
    auto type = this->_type(nodeVarDecl.var->type, nodeVarDecl.var->mut);
    auto initCode = std::string("0");

    if (nodeVarDecl.init != std::nullopt) {
      initCode = this->_nodeExpr(*nodeVarDecl.init);
    } else if (nodeVarDecl.var->type->isBool()) {
      this->_activateBuiltin("libStdbool");
      initCode = "false";
    } else if (nodeVarDecl.var->type->isChar()) {
      initCode = R"('\0')";
    } else if (nodeVarDecl.var->type->isStr()) {
      this->_activateBuiltin("fnStrAlloc");
      initCode = R"(str_alloc(""))";
    }

    code = (root ? std::string(this->indent, ' ') : "") + type + name + " = " + initCode + (root ? ";" EOL : "");

    if (nodeVarDecl.var->type->isObj()) {
      auto cleanUpType = this->_type(nodeVarDecl.var->type, true);
      auto cleanUpTypeName = Codegen::typeName(nodeVarDecl.var->type->name);

      this->_activateEntity(cleanUpTypeName + "_free");
      this->state.cleanUp.add(cleanUpTypeName + "_free((" + cleanUpType + ") " + name + ");");
    } else if (nodeVarDecl.var->type->isStr()) {
      this->_activateBuiltin("fnStrFree");
      this->state.cleanUp.add("str_free((struct str) " + name + ");");
    }

    return this->_wrapNode(node, setUp, code, cleanUp);
  }

  throw Error("Error: tried to generate code for unknown node");
}

std::string Codegen::_nodeIf (const ASTNodeIf &nodeIf) {
  auto [bodyCode, bodyCleanUp] = this->_block(nodeIf.body);
  auto code = "if (" + this->_nodeExpr(nodeIf.cond) + ") {" EOL + bodyCode + bodyCleanUp;

  if (nodeIf.alt != std::nullopt) {
    code += std::string(this->indent, ' ') + "} else ";

    if (std::holds_alternative<ASTBlock>(**nodeIf.alt)) {
      auto [altCode, altCleanUp] = this->_block(std::get<ASTBlock>(**nodeIf.alt));
      code += "{" EOL + altCode + altCleanUp + std::string(this->indent, ' ') + "}";
    } else if (std::holds_alternative<ASTNodeIf>(**nodeIf.alt)) {
      code += this->_nodeIf(std::get<ASTNodeIf>(**nodeIf.alt));
    }
  } else {
    code += std::string(this->indent, ' ') + "}";
  }

  return code;
}

std::string Codegen::_nodeExpr (const ASTNodeExpr &nodeExpr, bool root) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
    auto code = this->_exprAccess(exprAccess.body);

    if (!root && nodeExpr.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);

      this->_activateEntity(typeName + "_copy");
      code = typeName + "_copy(" + code + ")";
    } else if (!root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrCopy");
      code = "str_copy(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto leftCode = this->_exprAccess(exprAssign.left.body);

    if (exprAssign.right.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);

      this->_activateEntity(typeName + "_realloc");
      auto code = leftCode + " = " + typeName + "_realloc(" + leftCode + ", " + this->_nodeExpr(exprAssign.right) + ")";

      if (!root && nodeExpr.type->isStr()) {
        code = typeName + "_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    } else if (exprAssign.right.type->isStr()) {
      auto rightCode = std::string();

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        if (exprAssign.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr");
          this->_activateBuiltin("fnStrCopy");

          rightCode = "str_concat_cstr(str_copy(" + leftCode + "), " + exprAssign.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr");
          this->_activateBuiltin("fnStrCopy");

          rightCode = "str_concat_str(str_copy(" + leftCode + "), " + this->_nodeExpr(exprAssign.right) + ")";
        }
      } else {
        rightCode = this->_nodeExpr(exprAssign.right);
      }

      this->_activateBuiltin("fnStrRealloc");
      auto code = leftCode + " = str_realloc(" + leftCode + ", " + rightCode + ")";

      if (!root && nodeExpr.type->isStr()) {
        this->_activateBuiltin("fnStrCopy");
        code = "str_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }

    auto opCode = std::string(" = ");
    auto rightCode = this->_nodeExpr(exprAssign.right);

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
          this->_activateBuiltin("fnStrAlloc");
          code = "str_alloc(" + nodeExpr.litBody() + ")";
        } else if (exprBinary.left.isLit()) {
          this->_activateBuiltin("fnCstrConcatStr");
          code = "cstr_concat_str(" + exprBinary.left.litBody() + ", " + this->_nodeExpr(exprBinary.right) + ")";
        } else if (exprBinary.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr");
          code = "str_concat_cstr(" + this->_nodeExpr(exprBinary.left) + ", " + exprBinary.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr");
          code = "str_concat_str(" + this->_nodeExpr(exprBinary.left) + ", " + this->_nodeExpr(exprBinary.right) + ")";
        }

        if (root) {
          this->_activateBuiltin("fnStrFree");
          code = "str_free(" + code + ")";
        }

        return this->_wrapNodeExpr(nodeExpr, code);
      } else if (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) {
        auto opName = std::string(exprBinary.op == AST_EXPR_BINARY_NE ? "ne" : "eq");

        if (nodeExpr.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnCstrEqCstr");
          } else {
            this->_activateBuiltin("fnCstrNeCstr");
          }

          auto code = "cstr_" + opName + "_cstr(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")";
          return this->_wrapNodeExpr(nodeExpr, code);
        } else if (exprBinary.left.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnCstrEqStr");
          } else {
            this->_activateBuiltin("fnCstrNeStr");
          }

          auto rightCode = this->_nodeExpr(exprBinary.right);
          return this->_wrapNodeExpr(nodeExpr, "cstr_" + opName + "_str(" + exprBinary.left.litBody() + ", " + rightCode + ")");
        } else if (exprBinary.right.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnStrEqCstr");
          } else {
            this->_activateBuiltin("fnStrNeCstr");
          }

          auto leftCode = this->_nodeExpr(exprBinary.left);
          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")");
        } else {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnStrEqStr");
          } else {
            this->_activateBuiltin("fnStrNeStr");
          }

          auto leftCode = this->_nodeExpr(exprBinary.left);
          auto rightCode = this->_nodeExpr(exprBinary.right);

          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_str(" + leftCode + ", " + rightCode + ")");
        }
      }
    }

    auto leftCode = this->_nodeExpr(exprBinary.left);
    auto rightCode = this->_nodeExpr(exprBinary.right);
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
        auto separator = std::string(R"(" ")");
        auto isSeparatorLit = true;
        auto terminator = std::string(R"(")" ESC_EOL R"(")");
        auto isTerminatorLit = true;

        for (const auto &exprCallArg : exprCall.args) {
          if (exprCallArg.id != std::nullopt && exprCallArg.id == "separator") {
            if (exprCallArg.expr.isLit()) {
              separator = exprCallArg.expr.litBody();
            } else {
              separator = this->_nodeExpr(exprCallArg.expr);
              isSeparatorLit = false;
            }
          } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "terminator") {
            if (exprCallArg.expr.isLit()) {
              terminator = exprCallArg.expr.litBody();
            } else {
              terminator = this->_nodeExpr(exprCallArg.expr);
              isTerminatorLit = false;
            }
          }
        }

        this->_activateBuiltin("fnPrint");
        this->_activateBuiltin("libStdio");

        auto code = std::string(R"(print(stdout, ")");
        auto argsCode = std::string();
        auto argIdx = static_cast<std::size_t>(0);

        for (const auto &exprCallArg : exprCall.args) {
          if (exprCallArg.id != "items") {
            continue;
          }

          if (argIdx != 0 && separator != R"("")") {
            code += isSeparatorLit ? "z" : "s";
            argsCode += separator + ", ";
          }

          if (exprCallArg.expr.type->isObj()) {
            auto typeName = Codegen::typeName(exprCallArg.expr.type->name);
            code += "s";

            this->_activateEntity(typeName + "_str");
            argsCode += typeName + "_str(" + this->_nodeExpr(exprCallArg.expr) + ")";
          } else if (exprCallArg.expr.type->isStr() && exprCallArg.expr.isLit()) {
            code += "z";
            argsCode += exprCallArg.expr.litBody();
          } else {
            if (exprCallArg.expr.type->isBool()) code += "t";
            else if (exprCallArg.expr.type->isByte()) code += "b";
            else if (exprCallArg.expr.type->isChar()) code += "c";
            else if (exprCallArg.expr.type->isF32()) code += "e";
            else if (exprCallArg.expr.type->isF64()) code += "g";
            else if (exprCallArg.expr.type->isFloat()) code += "f";
            else if (exprCallArg.expr.type->isI8()) code += "h";
            else if (exprCallArg.expr.type->isI16()) code += "j";
            else if (exprCallArg.expr.type->isI32()) code += "k";
            else if (exprCallArg.expr.type->isI64()) code += "l";
            else if (exprCallArg.expr.type->isInt()) code += "i";
            else if (exprCallArg.expr.type->isStr()) code += "s";
            else if (exprCallArg.expr.type->isU8()) code += "v";
            else if (exprCallArg.expr.type->isU16()) code += "w";
            else if (exprCallArg.expr.type->isU32()) code += "u";
            else if (exprCallArg.expr.type->isU64()) code += "y";

            argsCode += this->_nodeExpr(exprCallArg.expr);
          }

          argsCode += ", ";
          argIdx++;
        }

        if (terminator == R"("")") {
          code += R"(", )" + argsCode.substr(0, argsCode.size() - 2);
        } else {
          code += std::string(isTerminatorLit ? "z" : "s") + R"(", )" + argsCode + terminator;
        }

        return this->_wrapNodeExpr(nodeExpr, code + ")");
      }
    } else {
      auto fn = std::get<TypeFn>(exprCall.calleeType->body);
      auto paramsName = this->_typeFnId(exprCall.calleeType) + "P";
      auto bodyCode = std::string();

      if (!fn.params.empty()) {
        bodyCode += "(struct " + paramsName + ") {";
        this->_activateEntity(paramsName);

        auto paramIdx = static_cast<std::size_t>(0);

        for (const auto &param : fn.params) {
          auto foundArg = std::optional<ASTExprCallArg>{};

          if (param.name != std::nullopt) {
            for (const auto &exprCallArg : exprCall.args) {
              if (exprCallArg.id == param.name) {
                foundArg = exprCallArg;
                break;
              }
            }
          } else if (paramIdx < exprCall.args.size()) {
            foundArg = exprCall.args[paramIdx];
          }

          if (!param.required) {
            bodyCode += std::string(paramIdx == 0 ? "" : ", ") + (foundArg == std::nullopt ? "0" : "1");
          }

          bodyCode += paramIdx == 0 && param.required ? "" : ", ";

          if (foundArg != std::nullopt) {
            bodyCode += this->_nodeExpr(foundArg->expr);
          } else if (param.type->isBool()) {
            this->_activateBuiltin("libStdbool");
            bodyCode += "false";
          } else if (param.type->isChar()) {
            bodyCode += R"('\0')";
          } else if (param.type->isObj()) {
            this->_activateBuiltin("libStdlib");
            bodyCode += "NULL";
          } else if (param.type->isStr()) {
            this->_activateBuiltin("typeStr");
            bodyCode += "(struct str) {}";
          } else {
            bodyCode += "0";
          }

          paramIdx++;
        }

        bodyCode += "}";
      }

      auto fnName = this->_exprAccess(exprCall.callee.body);
      auto code = fnName + ".f(" + fnName + ".c" + (bodyCode.empty() ? "" : ", ") + bodyCode + ")";

      if (root && nodeExpr.type->isObj()) {
        auto nodeExprTypeName = Codegen::typeName(nodeExpr.type->name);

        this->_activateEntity(nodeExprTypeName + "_free");
        code = nodeExprTypeName + "_free(" + code + ")";
      } else if (root && nodeExpr.type->isStr()) {
        this->_activateBuiltin("fnStrFree");
        code = "str_free(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto condCode = this->_nodeExpr(exprCond.cond);
    auto bodyCode = this->_nodeExpr(exprCond.body);
    auto altCode = this->_nodeExpr(exprCond.alt);

    if (std::holds_alternative<ASTExprAssign>(*exprCond.alt.body) && !exprCond.alt.parenthesized && !exprCond.alt.type->isStr()) {
      altCode = "(" + altCode + ")";
    }

    auto code = condCode + " ? " + bodyCode + " : " + altCode;

    if (root && nodeExpr.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);

      this->_activateEntity(typeName + "_free");
      code = typeName + "_free(" + code + ")";
    } else if (root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrFree");
      code = "str_free(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprLit>(*nodeExpr.body)) {
    auto exprLit = std::get<ASTExprLit>(*nodeExpr.body);

    if (exprLit.type == AST_EXPR_LIT_BOOL) {
      this->_activateBuiltin("libStdbool");
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
      this->_activateBuiltin("fnStrAlloc");
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

      fieldsCode += ", " + this->_nodeExpr(exprObjProp->init);
    }

    this->_activateEntity(typeName + "_alloc");
    auto code = typeName + "_alloc(" + fieldsCode.substr(2) + ")";

    if (root) {
      this->_activateEntity(typeName + "_free");
      code = typeName + "_free(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);
    auto argCode = this->_nodeExpr(exprUnary.arg);
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
    }

    return this->_wrapNodeExpr(nodeExpr, exprUnary.prefix ? opCode + argCode : argCode + opCode);
  }

  throw Error("Error: tried to generate code for unknown expression");
}

std::string Codegen::_type (const Type *type, bool mut, bool ref) {
  auto typeName = std::string();

  if (type->isAny()) {
    return std::string(mut ? "" : "const ") + "void *" + (ref ? "*" : "");
  } else if (type->isByte()) {
    typeName = "unsigned char";
  } else if (type->isChar()) {
    typeName = "char";
  } else if (type->isF32()) {
    typeName = "float";
  } else if (type->isF64() || type->isFloat()) {
    typeName = "double";
  } else if (type->isBool()) {
    this->_activateBuiltin("libStdbool");
    typeName = "bool";
  } else if (type->isFn()) {
    typeName = this->_typeFnId(type);

    this->_activateEntity(typeName);
    return std::string(mut ? "" : "const ") + "struct " + typeName + " " + (ref ? "*" : "");
  } else if (type->isI8()) {
    this->_activateBuiltin("libStdint");
    typeName = "int8_t";
  } else if (type->isI16()) {
    this->_activateBuiltin("libStdint");
    typeName = "int16_t";
  } else if (type->isI32() || type->isInt()) {
    this->_activateBuiltin("libStdint");
    typeName = "int32_t";
  } else if (type->isI64()) {
    this->_activateBuiltin("libStdint");
    typeName = "int64_t";
  } else if (type->isObj()) {
    typeName = Codegen::typeName(type->name);

    this->_activateEntity(typeName);
    return std::string(mut ? "" : "const ") + "struct " + typeName + " *" + (ref ? "*" : "");
  } else if (type->isStr()) {
    this->_activateBuiltin("typeStr");
    typeName = "struct str";
  } else if (type->isU8()) {
    this->_activateBuiltin("libStdint");
    typeName = "uint8_t";
  } else if (type->isU16()) {
    this->_activateBuiltin("libStdint");
    typeName = "uint16_t";
  } else if (type->isU32()) {
    this->_activateBuiltin("libStdint");
    typeName = "uint32_t";
  } else if (type->isU64()) {
    this->_activateBuiltin("libStdint");
    typeName = "uint64_t";
  } else if (type->isVoid()) {
    return std::string(mut ? "" : "const ") + "void " + (ref ? "*" : "");
  }

  return std::string(mut ? "" : "const ") + typeName + " " + (ref ? "*" : "");
}

std::string Codegen::_typeFnId (const Type *searchType) {
  for (const auto &[idx, type] : this->functions) {
    if (searchType->match(type, false)) {
      return "fn$" + std::to_string(idx);
    }
  }

  auto idx = this->functions.size();
  this->functions.emplace_back(idx, searchType);

  auto fn = std::get<TypeFn>(searchType->body);
  auto fnId = "fn$" + std::to_string(idx);
  auto paramsName = fnId + "P";

  if (!fn.params.empty()) {
    auto paramsEntity = CodegenEntity{paramsName, CODEGEN_ENTITY_OBJ};
    auto paramIdx = static_cast<std::size_t>(0);

    paramsEntity.decl = "struct " + paramsName + ";";
    paramsEntity.def = "struct " + paramsName + " {" EOL;

    for (const auto &param : fn.params) {
      auto paramType = this->_type(param.type, false);
      auto paramIdxStr = std::to_string(paramIdx);

      if (!param.required) {
        paramsEntity.def += "  unsigned char o" + paramIdxStr + ";" EOL;
      }

      paramsEntity.def += "  " + paramType + "n" + paramIdxStr + ";" EOL;
      paramIdx++;
    }

    paramsEntity.def += "};";
    this->entities.push_back(paramsEntity);
  }

  auto typeEntity = CodegenEntity{fnId, CODEGEN_ENTITY_OBJ, {}, { paramsName }};

  typeEntity.decl = "struct " + fnId + ";";
  typeEntity.def = "struct " + fnId + " {" EOL;
  typeEntity.def += "  " + this->_type(fn.returnType, true) + "(*f) ";
  typeEntity.def += "(void *" + (fn.params.empty() ? "" : ", struct " + paramsName) + ");" EOL;
  typeEntity.def += "  void *c;" EOL;
  typeEntity.def += "};";
  this->entities.push_back(typeEntity);

  return fnId;
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
