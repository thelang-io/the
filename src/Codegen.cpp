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
  auto mainCode = std::string();

  for (const auto &node : nodes) {
    mainCode += this->_node(node);
  }

  mainCode += this->state.cleanUp.gen(2);

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

  if (this->builtins.fnBoolStr) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str bool_str (bool);" EOL;
    builtinFnDefCode += "struct str bool_str (bool t) {" EOL;
    builtinFnDefCode += R"(  return str_alloc(t ? "true" : "false");)" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnByteStr) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str byte_str (unsigned char);" EOL;
    builtinFnDefCode += "struct str byte_str (unsigned char x) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%u", x);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnCharStr) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str char_str (char);" EOL;
    builtinFnDefCode += "struct str char_str (char c) {" EOL;
    builtinFnDefCode += "  char buf[2] = {c, '\\0'};" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

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

  if (this->builtins.fnF32Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str f32_str (float);" EOL;
    builtinFnDefCode += "struct str f32_str (float f) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%f", f);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnF64Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str f64_str (double);" EOL;
    builtinFnDefCode += "struct str f64_str (double f) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%f", f);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnFloatStr) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str float_str (double);" EOL;
    builtinFnDefCode += "struct str float_str (double f) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%f", f);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI8Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str i8_str (int8_t);" EOL;
    builtinFnDefCode += "struct str i8_str (int8_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId8, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI16Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str i16_str (int16_t);" EOL;
    builtinFnDefCode += "struct str i16_str (int16_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId16, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI32Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str i32_str (int32_t);" EOL;
    builtinFnDefCode += "struct str i32_str (int32_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId32, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnI64Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str i64_str (int64_t);" EOL;
    builtinFnDefCode += "struct str i64_str (int64_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRId64, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
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
    builtinFnDefCode += "      case 'h':" EOL;
    builtinFnDefCode += "      case 'j':" EOL;
    builtinFnDefCode += "      case 'v':" EOL;
    builtinFnDefCode += R"(      case 'w': sprintf(buf, "%d", va_arg(args, int)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'i':" EOL;
    builtinFnDefCode += R"(      case 'k': sprintf(buf, "%" PRId32, va_arg(args, int32_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'l': sprintf(buf, "%" PRId64, va_arg(args, int64_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'p': sprintf(buf, "%p", va_arg(args, void *)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 's': s = va_arg(args, struct str); fwrite(s.c, 1, s.l, stream); str_free(s); break;" EOL;
    builtinFnDefCode += R"(      case 'u': sprintf(buf, "%" PRIu32, va_arg(args, uint32_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += R"(      case 'y': sprintf(buf, "%" PRIu64, va_arg(args, uint64_t)); fputs(buf, stream); break;)" EOL;
    builtinFnDefCode += "      case 'z': fputs(va_arg(args, char *), stream); break;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  va_end(args);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnRefStr) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str ref_str (void *);" EOL;
    builtinFnDefCode += "struct str ref_str (void *p) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%p", p);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
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

  if (this->builtins.fnU8Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str u8_str (uint8_t);" EOL;
    builtinFnDefCode += "struct str u8_str (uint8_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu8, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU16Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str u16_str (uint16_t);" EOL;
    builtinFnDefCode += "struct str u16_str (uint16_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu16, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU32Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str u32_str (uint32_t);" EOL;
    builtinFnDefCode += "struct str u32_str (uint32_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu32, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
    builtinFnDefCode += "}" EOL;
  }

  if (this->builtins.fnU64Str) {
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");

    builtinFnDeclCode += "struct str u64_str (uint64_t);" EOL;
    builtinFnDefCode += "struct str u64_str (uint64_t d) {" EOL;
    builtinFnDefCode += "  char buf[512];" EOL;
    builtinFnDefCode += R"(  sprintf(buf, "%" PRIu64, d);)" EOL;
    builtinFnDefCode += "  return str_alloc(buf);" EOL;
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
  output += mainCode;
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
  else if (name == "fnRefStr") this->builtins.fnRefStr = true;
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
  else throw Error("tried activating unknown builtin");
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

std::string Codegen::_block (const ASTBlock &nodes, bool saveCleanUp) {
  auto initialIndent = this->indent;
  auto initialCleanUp = this->state.cleanUp;
  auto code = std::string();

  if (saveCleanUp) {
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialCleanUp);
  }

  this->indent += 2;

  for (const auto &node : nodes) {
    code += this->_node(node);
  }

  if (saveCleanUp) {
    code += this->state.cleanUp.gen(this->indent);

    if (
      !ASTChecker(nodes).endsWith<ASTNodeBreak>() &&
      !ASTChecker(nodes).endsWith<ASTNodeContinue>() &&
      !ASTChecker(nodes).endsWith<ASTNodeReturn>() &&
      this->state.cleanUp.breakVarUsed
    ) {
      code += std::string(this->indent, ' ') + "if (" + initialCleanUp.currentBreakVar() + " == 1) break;" EOL;
    }

    if (!ASTChecker(nodes).endsWith<ASTNodeReturn>() && this->state.cleanUp.returnVarUsed) {
      code += std::string(this->indent, ' ') + "if (r == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
    }

    this->state.cleanUp = initialCleanUp;
  }

  this->indent = initialIndent;
  return code;
}

std::string Codegen::_flags () const {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : this->flags) {
    result += (idx++ == 0 ? "" : " ") + flag;
  }

  return result;
}

std::string Codegen::_node (const ASTNode &node, bool root) {
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
  } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*node.body);
    code = this->_nodeExpr(nodeExpr, nodeExpr.type, true);

    if (root) {
      code = std::string(this->indent, ' ') + code + ";" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);
    auto fn = std::get<TypeFn>(nodeFnDecl.var->type->body);
    auto fnName = Codegen::name(nodeFnDecl.var->codeName);
    auto fnTypeName = this->_typeNameFn(nodeFnDecl.var->type);
    auto typeName = Codegen::typeName(nodeFnDecl.var->codeName);
    auto contextName = typeName + "X";
    auto paramsName = fnTypeName + "P";

    auto entity = CodegenEntity{typeName, CODEGEN_ENTITY_FN, {}, {
      fnTypeName,
      contextName,
      paramsName
    }};

    auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc", "libString" }, {
      fnTypeName,
      typeName,
      contextName
    }};

    auto saveIndent = this->indent;
    auto saveStateBuiltins = this->state.builtins;
    auto saveStateEntities = this->state.entities;
    auto initialStateCleanUp = this->state.cleanUp;
    auto saveStateContextVars = this->state.contextVars;

    this->varMap.save();
    this->indent = 2;
    this->state.builtins = &entity.builtins;
    this->state.entities = &entity.entities;
    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_FN, &initialStateCleanUp);

    auto bodyCode = std::string();

    if (!nodeFnDecl.stack.empty()) {
      bodyCode += "  struct " + contextName + " *x = px;" EOL;

      auto contextEntity = CodegenEntity{contextName, CODEGEN_ENTITY_OBJ};
      auto contextCode = std::string();

      for (const auto &contextVar : nodeFnDecl.stack) {
        auto contextVarName = Codegen::name(contextVar->codeName);
        auto contextVarType = this->_type(contextVar->type, contextVar->mut, true);

        contextCode += "  " + contextVarType + contextVarName + ";" EOL;
        bodyCode += "  " + contextVarType + contextVarName + " = x->" + contextVarName + ";" EOL;

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
        auto paramType = this->_type(param.var->type, param.var->mut, false);
        auto paramIdxStr = std::to_string(paramIdx);

        bodyCode += "  " + paramType + paramName + " = ";

        if (param.init == std::nullopt) {
          bodyCode += "p.n" + paramIdxStr;
        } else {
          auto initCode = this->_nodeExpr(*param.init, param.var->type);
          bodyCode += "p.o" + paramIdxStr + " == 1 ? p.n" + paramIdxStr + " : " + initCode;
        }

        bodyCode += ";" EOL;

        if (param.var->type->isFn()) {
          auto paramTypeName = this->_typeNameFn(param.var->type);

          this->_activateEntity(paramTypeName + "_free");
          this->state.cleanUp.add(paramTypeName + "_free((struct " + paramTypeName + ") " + paramName + ");");
        } else if (param.var->type->isObj()) {
          auto cleanUpType = this->_type(param.var->type, true, false);
          auto cleanUpTypeName = Codegen::typeName(param.var->type->codeName);

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
    this->state.returnType = fn.returnType;
    bodyCode += this->_block(nodeFnDecl.body, false);
    this->indent = 2;

    this->varMap.restore();
    auto returnType = this->_type(fn.returnType, true, false);

    if (!fn.returnType->isVoid() && this->state.cleanUp.valueVarUsed) {
      bodyCode.insert(0, std::string(this->indent, ' ') + returnType + "v;" EOL);
      bodyCode += this->state.cleanUp.gen(this->indent);
      bodyCode += std::string(this->indent, ' ') + "return v;" EOL;
    } else {
      bodyCode += this->state.cleanUp.gen(this->indent);
    }

    if (this->state.cleanUp.returnVarUsed) {
      bodyCode.insert(0, std::string(this->indent, ' ') + "unsigned char r = 0;" EOL);
    }

    if (nodeFnDecl.params.empty()) {
      entity.decl = returnType + typeName + " (void *);";
      entity.def = returnType + typeName + " (void *px) {" EOL + bodyCode + "}";
    } else {
      entity.decl = returnType + typeName + " (void *, struct " + paramsName + ");";
      entity.def = returnType + typeName + " (void *px, struct " + paramsName + " p) {" EOL + bodyCode + "}";
    }

    allocFnEntity.decl = "void " + typeName + "_alloc (struct " + fnTypeName + " *, struct " + contextName + ");";
    allocFnEntity.def = "void " + typeName + "_alloc (struct " + fnTypeName + " *n, struct " + contextName + " x) {" EOL;
    allocFnEntity.def += "  size_t l = sizeof(struct " + contextName + ");" EOL;
    allocFnEntity.def += "  struct " + contextName + " *r = alloc(l);" EOL;
    allocFnEntity.def += "  memcpy(r, &x, l);" EOL;
    allocFnEntity.def += "  n->f = &" + typeName + ";" EOL;
    allocFnEntity.def += "  n->x = r;" EOL;
    allocFnEntity.def += "  n->l = l;" EOL;
    allocFnEntity.def += "}";

    this->entities.push_back(entity);
    this->entities.push_back(allocFnEntity);

    this->indent = saveIndent == 0 ? 2 : saveIndent;
    this->state.builtins = saveStateBuiltins;
    this->state.entities = saveStateEntities;
    this->state.cleanUp = initialStateCleanUp;
    this->state.contextVars = saveStateContextVars;

    code = std::string(this->indent, ' ');
    code += "const struct " + fnTypeName + " " + fnName;
    this->_activateEntity(fnTypeName);

    if (nodeFnDecl.stack.empty()) {
      code += " = (struct " + fnTypeName + ") {&" + typeName + ", NULL, 0};";

      this->_activateBuiltin("libStdlib");
      this->_activateEntity(typeName);
    } else {
      code += ";" EOL;

      this->_activateEntity(typeName + "_alloc");
      code += std::string(this->indent, ' ') + typeName + "_alloc((struct " + fnTypeName + " *) &" + fnName + ", ";

      this->_activateEntity(contextName);
      code += "(struct " + contextName + ") {";

      auto contextVarIdx = static_cast<std::size_t>(0);

      for (const auto &contextVar : nodeFnDecl.stack) {
        auto contextVarName = Codegen::name(contextVar->codeName);
        auto contextVarType = this->_type(contextVar->type, contextVar->mut, true);

        code += contextVarIdx == 0 ? "" : ", ";
        code += (this->state.contextVars.contains(contextVarName) ? "" : "&") + contextVarName;

        contextVarIdx++;
      }

      code += "});";

      this->_activateEntity(fnTypeName + "_free");
      this->state.cleanUp.add(fnTypeName + "_free((struct " + fnTypeName + ") " + fnName + ");");
    }

    this->indent = saveIndent;
    return this->_wrapNode(node, code + EOL);
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    auto nodeIf = std::get<ASTNodeIf>(*node.body);

    code = std::string(this->indent, ' ') + "if (" + this->_nodeExpr(nodeIf.cond, this->ast->typeMap.get("bool")) + ") {" EOL;
    this->varMap.save();
    code += this->_block(nodeIf.body);
    this->varMap.restore();

    if (nodeIf.alt != std::nullopt) {
      code += std::string(this->indent, ' ') + "} else ";

      if (std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
        code += "{" EOL + this->_block(std::get<ASTBlock>(*nodeIf.alt));
        code += std::string(this->indent, ' ') + "}" EOL;
      } else if (std::holds_alternative<ASTNode>(*nodeIf.alt)) {
        auto elseIfCode = this->_node(std::get<ASTNode>(*nodeIf.alt));
        // todo test
        code += elseIfCode.starts_with(" ") ? elseIfCode.substr(elseIfCode.find_first_not_of(' ')) : elseIfCode;
      }
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
    auto obj = std::get<TypeObj>(nodeObjDecl.type->body);
    auto typeName = Codegen::typeName(nodeObjDecl.type->codeName);
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

    auto type = this->_type(nodeObjDecl.type, true, false);
    auto bodyCode = std::string();
    auto allocFnParamTypes = std::string();
    auto allocFnParams = std::string();
    auto allocFnCode = std::string("  " + type + "r = alloc(sizeof(struct " + typeName + "));" EOL);
    auto copyFnCode = std::string("  " + type + "r = alloc(sizeof(struct " + typeName + "));" EOL);
    auto freeFnCode = std::string();
    auto reallocFnCode = std::string("  " + typeName + "_free((" + type + ") o1);" EOL "  return o2;" EOL);
    auto strFnCode = std::string(R"(  struct str r = str_alloc(")" + nodeObjDecl.type->name + R"({");)" EOL);
    auto objFieldIdx = static_cast<std::size_t>(0);

    for (const auto &objField : obj.fields) {
      auto objFieldName = Codegen::name(objField.name);
      auto objFieldType = this->_type(objField.type, true, false);
      auto strCodeDelimiter = std::string(objFieldIdx == 0 ? "" : ", ");

      if (objField.type->isFn()) {
        auto objFieldTypeName = this->_typeNameFn(objField.type);

        this->_activateEntity(objFieldTypeName + "_copy", &copyFnEntity.entities);
        this->_activateEntity(objFieldTypeName + "_free", &freeFnEntity.entities);

        copyFnCode += "  r->" + objFieldName + " = " + objFieldTypeName + "_copy(o->" + objFieldName + ");" EOL;
        freeFnCode += "  " + objFieldTypeName + "_free((struct " + objFieldTypeName + ") o->" + objFieldName + ");" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + ": ";
        strFnCode += "[Function: " + objField.name + R"(]");)" EOL;
      } else if (objField.type->isObj()) {
        auto objFieldTypeName = Codegen::typeName(objField.type->codeName);

        this->_activateEntity(objFieldTypeName + "_copy", &copyFnEntity.entities);
        this->_activateEntity(objFieldTypeName + "_free", &freeFnEntity.entities);
        this->_activateBuiltin("fnStrConcatStr", &strFnEntity.builtins);
        this->_activateEntity(objFieldTypeName + "_copy", &strFnEntity.entities);
        this->_activateEntity(objFieldTypeName + "_str", &strFnEntity.entities);

        copyFnCode += "  r->" + objFieldName + " = " + objFieldTypeName + "_copy(o->" + objFieldName + ");" EOL;
        freeFnCode += "  " + objFieldTypeName + "_free((" + objFieldType + ") o->" + objFieldName + ");" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + R"(: ");)" EOL;
        strFnCode += "  r = str_concat_str(r, " + objFieldTypeName + "_str(";
        strFnCode += objFieldTypeName + "_copy(o->" + objFieldName + ")));" EOL;
      } else if (objField.type->isRef()) {
        this->_activateBuiltin("fnRefStr", &strFnEntity.builtins);
        this->_activateBuiltin("fnStrConcatStr", &strFnEntity.builtins);

        copyFnCode += "  r->" + objFieldName + " = o->" + objFieldName + ";" EOL;
        strFnCode += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + objField.name + R"(: ");)" EOL;
        strFnCode += "  r = str_concat_str(r, ref_str((void *) o->" + objFieldName + "));" EOL;
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
      } else if (objField.type->isAny() || objField.type->isVoid()) {
        auto typeStrContent = std::string();

        if (objField.type->isAny()) {
          typeStrContent = "any";
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
    copyFnEntity.decl = type + typeName + "_copy (const " + type + ");";
    copyFnEntity.def = type + typeName + "_copy (const " + type + "o) {" EOL + copyFnCode + "}";
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

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

    if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_FN) || this->state.cleanUp.returnVarUsed) {
      if (this->state.cleanUp.parent != nullptr && this->state.cleanUp.parent->hasCleanUp(CODEGEN_CLEANUP_FN)) {
        code += std::string(this->indent, ' ') + this->state.cleanUp.currentReturnVar() + " = 1;" EOL;
      }

      if (nodeReturn.body != std::nullopt) {
        code += std::string(this->indent, ' ') + this->state.cleanUp.currentValueVar() + " = ";
        code += this->_nodeExpr(*nodeReturn.body, this->state.returnType) + ";" EOL;
      }

      if (!ASTChecker(node.parent).is<ASTNodeFnDecl>() || !ASTChecker(node).isLast()) {
        code += std::string(this->indent, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
      }
    } else if (nodeReturn.body != std::nullopt) {
      code = std::string(this->indent, ' ') + "return " + this->_nodeExpr(*nodeReturn.body, this->state.returnType) + ";" EOL;
    } else {
      code = std::string(this->indent, ' ') + "return;" EOL;
    }

    return this->_wrapNode(node, code);
  } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
    auto name = Codegen::name(nodeVarDecl.var->codeName);
    auto type = this->_type(nodeVarDecl.var->type, nodeVarDecl.var->mut, false);
    auto initCode = std::string("0");

    if (nodeVarDecl.init != std::nullopt) {
      initCode = this->_nodeExpr(*nodeVarDecl.init, nodeVarDecl.var->type);
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

    if (nodeVarDecl.var->type->isFn()) {
      auto cleanUpTypeName = this->_typeNameFn(nodeVarDecl.var->type);

      this->_activateEntity(cleanUpTypeName + "_free");
      this->state.cleanUp.add(cleanUpTypeName + "_free((struct " + cleanUpTypeName + ") " + name + ");");
    } else if (nodeVarDecl.var->type->isObj()) {
      auto cleanUpType = this->_type(nodeVarDecl.var->type, true, false);
      auto cleanUpTypeName = Codegen::typeName(nodeVarDecl.var->type->codeName);

      this->_activateEntity(cleanUpTypeName + "_free");
      this->state.cleanUp.add(cleanUpTypeName + "_free((" + cleanUpType + ") " + name + ");");
    } else if (nodeVarDecl.var->type->isStr()) {
      this->_activateBuiltin("fnStrFree");
      this->state.cleanUp.add("str_free((struct str) " + name + ");");
    }

    return this->_wrapNode(node, code);
  }

  throw Error("tried to generate code for unknown node");
}

std::string Codegen::_nodeExpr (const ASTNodeExpr &nodeExpr, Type *targetType, bool root) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
    auto code = std::string();

    if (std::holds_alternative<std::shared_ptr<Var>>(exprAccess.expr)) {
      auto objVar = std::get<std::shared_ptr<Var>>(exprAccess.expr);
      code = Codegen::name(objVar->codeName);

      if (this->state.contextVars.contains(code) && (nodeExpr.type->isRef() || !targetType->isRef())) {
        code = "*" + code;
      }

      if (!this->state.contextVars.contains(code) && !nodeExpr.type->isRef() && targetType->isRef()) {
        code = "&" + code;
      } else if (nodeExpr.type->isRef() && !targetType->isRef()) {
        code = "*" + code;
      }
    } else {
      auto objNodeExpr = std::get<ASTNodeExpr>(exprAccess.expr);
      auto objCode = this->_nodeExpr(objNodeExpr, Type::real(objNodeExpr.type), true);

      if (objCode.starts_with("*")) {
        objCode = "(" + objCode + ")";
      }

      code = objCode + "->" + Codegen::name(*exprAccess.prop);

      if (!nodeExpr.type->isRef() && targetType->isRef()) {
        code = "&" + code;
      } else if (nodeExpr.type->isRef() && !targetType->isRef()) {
        code = "*" + code;
      }
    }

    if (!nodeExpr.type->isRef() || !targetType->isRef()) {
      auto realType = Type::real(nodeExpr.type);

      if (!root && realType->isFn()) {
        auto typeName = this->_typeNameFn(realType);

        this->_activateEntity(typeName + "_copy");
        code = typeName + "_copy(" + code + ")";
      } else if (!root && realType->isObj()) {
        auto typeName = Codegen::typeName(realType->codeName);

        this->_activateEntity(typeName + "_copy");
        code = typeName + "_copy(" + code + ")";
      } else if (!root && realType->isStr()) {
        this->_activateBuiltin("fnStrCopy");
        code = "str_copy(" + code + ")";
      }
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto leftCode = this->_nodeExpr(exprAssign.left, nodeExpr.type, true);

    if (exprAssign.right.type->isFn()) {
      auto typeName = this->_typeNameFn(nodeExpr.type);

      this->_activateEntity(typeName + "_realloc");
      auto code = leftCode + " = " + typeName + "_realloc(" + leftCode + ", " + this->_nodeExpr(exprAssign.right, nodeExpr.type) + ")";

      if (!root && nodeExpr.type->isFn()) {
        this->_activateEntity(typeName + "_copy");
        code = typeName + "_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    } else if (exprAssign.right.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->codeName);

      this->_activateEntity(typeName + "_realloc");
      auto code = leftCode + " = " + typeName + "_realloc(" + leftCode + ", " + this->_nodeExpr(exprAssign.right, nodeExpr.type) + ")";

      if (!root && nodeExpr.type->isObj()) {
        this->_activateEntity(typeName + "_copy");
        code = typeName + "_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    } else if (exprAssign.right.type->isStr()) {
      auto rightCode = std::string();

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        this->_activateBuiltin("fnStrCopy");

        if (exprAssign.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr");
          rightCode = "str_concat_cstr(str_copy(" + leftCode + "), " + exprAssign.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr");
          rightCode = "str_concat_str(str_copy(" + leftCode + "), " + this->_nodeExpr(exprAssign.right, nodeExpr.type) + ")";
        }
      } else {
        rightCode = this->_nodeExpr(exprAssign.right, nodeExpr.type);
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

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (Type::real(exprBinary.left.type)->isStr() && Type::real(exprBinary.right.type)->isStr()) {
      if (exprBinary.op == AST_EXPR_BINARY_ADD) {
        auto code = std::string();

        if (root && nodeExpr.isLit()) {
          return this->_wrapNodeExpr(nodeExpr, nodeExpr.litBody());
        } else if (nodeExpr.isLit()) {
          this->_activateBuiltin("fnStrAlloc");
          code = "str_alloc(" + nodeExpr.litBody() + ")";
        } else if (exprBinary.left.isLit()) {
          this->_activateBuiltin("fnCstrConcatStr");
          code = "cstr_concat_str(" + exprBinary.left.litBody() + ", " + this->_nodeExpr(exprBinary.right, nodeExpr.type) + ")";
        } else if (exprBinary.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr");
          code = "str_concat_cstr(" + this->_nodeExpr(exprBinary.left, nodeExpr.type) + ", " + exprBinary.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr");
          code = "str_concat_str(" + this->_nodeExpr(exprBinary.left, nodeExpr.type) + ", " + this->_nodeExpr(exprBinary.right, nodeExpr.type) + ")";
        }

        if (root) {
          this->_activateBuiltin("fnStrFree");
          code = "str_free((struct str) " + code + ")";
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

          auto rightCode = this->_nodeExpr(exprBinary.right, nodeExpr.type);
          return this->_wrapNodeExpr(nodeExpr, "cstr_" + opName + "_str(" + exprBinary.left.litBody() + ", " + rightCode + ")");
        } else if (exprBinary.right.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnStrEqCstr");
          } else {
            this->_activateBuiltin("fnStrNeCstr");
          }

          auto leftCode = this->_nodeExpr(exprBinary.left, nodeExpr.type);
          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")");
        } else {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->_activateBuiltin("fnStrEqStr");
          } else {
            this->_activateBuiltin("fnStrNeStr");
          }

          auto leftCode = this->_nodeExpr(exprBinary.left, nodeExpr.type);
          auto rightCode = this->_nodeExpr(exprBinary.right, nodeExpr.type);

          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_str(" + leftCode + ", " + rightCode + ")");
        }
      }
    }

    auto leftCode = this->_nodeExpr(exprBinary.left, nodeExpr.type);
    auto rightCode = this->_nodeExpr(exprBinary.right, nodeExpr.type);
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
      if (exprCall.calleeType->codeName == "@print") {
        auto separator = std::string(R"(" ")");
        auto isSeparatorLit = true;
        auto terminator = std::string(R"(")" ESC_EOL R"(")");
        auto isTerminatorLit = true;

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

          if (exprCallArg.expr.type->isFn()) {
            auto argCode = std::string();

            if (std::holds_alternative<ASTExprAccess>(*exprCallArg.expr.body)) {
              auto exprAccess = std::get<ASTExprAccess>(*exprCallArg.expr.body);

              if (exprAccess.prop != std::nullopt) {
                argCode = *exprAccess.prop;
              } else {
                auto exprAccessVar = std::get<std::shared_ptr<Var>>(exprAccess.expr);
                argCode = exprAccessVar->name;
              }
            } else {
              argCode = "(anonymous)";
            }

            code += "z";
            argsCode += "(" + this->_nodeExpr(exprCallArg.expr, exprCallArg.expr.type, true);
            argsCode += R"(, "[Function: )" + argCode + R"(]"))";
          } else if (exprCallArg.expr.type->isObj()) {
            auto typeName = Codegen::typeName(exprCallArg.expr.type->codeName);
            code += "s";

            this->_activateEntity(typeName + "_str");
            argsCode += typeName + "_str(" + this->_nodeExpr(exprCallArg.expr, exprCallArg.expr.type) + ")";
          } else if (exprCallArg.expr.type->isRef()) {
            code += "p";
            argsCode += this->_nodeExpr(exprCallArg.expr, exprCallArg.expr.type);
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

            argsCode += this->_nodeExpr(exprCallArg.expr, exprCallArg.expr.type);
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
      auto typeName = this->_typeNameFn(exprCall.calleeType);
      auto paramsName = typeName + "P";
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
            bodyCode += this->_nodeExpr(foundArg->expr, param.type);
          } else if (param.type->isBool()) {
            this->_activateBuiltin("libStdbool");
            bodyCode += "false";
          } else if (param.type->isChar()) {
            bodyCode += R"('\0')";
          } else if (param.type->isFn()) {
            auto paramTypeName = this->_typeNameFn(param.type);

            this->_activateEntity(paramTypeName);
            bodyCode += "(struct " + paramTypeName + ") {}";
          } else if (param.type->isObj() || param.type->isRef()) {
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

      auto fnName = this->_nodeExpr(exprCall.callee, exprCall.calleeType, true);

      if (fnName.starts_with("*")) {
        fnName = "(" + fnName + ")";
      }

      auto code = fnName + ".f(" + fnName + ".x" + (bodyCode.empty() ? "" : ", ") + bodyCode + ")";

      if (nodeExpr.type->isRef() && !targetType->isRef()) {
        code = "*" + code;
      }

      if (root && nodeExpr.type->isFn()) {
        auto nodeExprTypeName = this->_typeNameFn(nodeExpr.type);

        this->_activateEntity(nodeExprTypeName + "_free");
        code = nodeExprTypeName + "_free((struct " + nodeExprTypeName + ") " + code + ")";
      } else if (root && nodeExpr.type->isObj()) {
        auto nodeExprTypeName = Codegen::typeName(nodeExpr.type->codeName);
        auto nodeExprType = this->_type(nodeExpr.type, true, false);

        this->_activateEntity(nodeExprTypeName + "_free");
        code = nodeExprTypeName + "_free((" + nodeExprType + ") " + code + ")";
      } else if (root && nodeExpr.type->isStr()) {
        this->_activateBuiltin("fnStrFree");
        code = "str_free((struct str) " + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto condCode = this->_nodeExpr(exprCond.cond, this->ast->typeMap.get("bool"));
    auto bodyCode = this->_nodeExpr(exprCond.body, nodeExpr.type);
    auto altCode = this->_nodeExpr(exprCond.alt, nodeExpr.type);

    if (
      std::holds_alternative<ASTExprAssign>(*exprCond.alt.body) &&
      !exprCond.alt.parenthesized &&
      !exprCond.alt.type->isRef() &&
      !exprCond.alt.type->isStr()
    ) {
      altCode = "(" + altCode + ")";
    }

    auto code = condCode + " ? " + bodyCode + " : " + altCode;

    if (root && nodeExpr.type->isFn()) {
      auto typeName = this->_typeNameFn(nodeExpr.type);

      this->_activateEntity(typeName + "_free");
      code = typeName + "_free((struct " + typeName + ") (" + code + "))";
    } else if (root && nodeExpr.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->codeName);
      auto type = this->_type(nodeExpr.type, true, false);

      this->_activateEntity(typeName + "_free");
      code = typeName + "_free((" + type + ") (" + code + "))";
    } else if (root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrFree");
      code = "str_free((struct str) (" + code + "))";
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
    auto typeName = Codegen::typeName(exprObj.type->codeName);
    auto fieldsCode = std::string();

    for (const auto &objField : obj.fields) {
      auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&objField] (const auto &it) -> bool {
        return it.id == objField.name;
      });

      fieldsCode += ", " + this->_nodeExpr(exprObjProp->init, objField.type);
    }

    this->_activateEntity(typeName + "_alloc");
    auto code = typeName + "_alloc(" + fieldsCode.substr(2) + ")";

    if (root) {
      auto type = this->_type(exprObj.type, true, false);

      this->_activateEntity(typeName + "_free");
      code = typeName + "_free((" + type + ") " + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprRef>(*nodeExpr.body)) {
    auto exprRef = std::get<ASTExprRef>(*nodeExpr.body);
    auto code = this->_nodeExpr(exprRef.expr, targetType, targetType->isRef());

    return this->_wrapNodeExpr(nodeExpr, code);
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

    return this->_wrapNodeExpr(nodeExpr, exprUnary.prefix ? opCode + argCode : argCode + opCode);
  }

  throw Error("tried to generate code for unknown expression");
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
    typeName = this->_typeNameFn(type);

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
    typeName = Codegen::typeName(type->codeName);

    this->_activateEntity(typeName);
    return std::string(mut ? "" : "const ") + "struct " + typeName + " *" + (ref ? "*" : "");
  } else if (type->isRef()) {
    auto typeRef = std::get<TypeRef>(type->body);
    return this->_type(typeRef.type, mut, ref) + "*";
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

std::string Codegen::_typeNameFn (const Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return entity.name;
    }
  }

  auto fn = std::get<TypeFn>(type->body);
  auto paramsName = Codegen::typeName(type->name + "P");

  if (!fn.params.empty()) {
    auto paramsEntity = CodegenEntity{paramsName, CODEGEN_ENTITY_OBJ};
    auto paramIdx = static_cast<std::size_t>(0);

    paramsEntity.decl = "struct " + paramsName + ";";
    paramsEntity.def = "struct " + paramsName + " {" EOL;

    for (const auto &param : fn.params) {
      auto paramType = this->_type(param.type, true, false);
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

  auto entity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ, { "libStdlib" }, { paramsName }};
  entity.decl = "struct " + typeName + ";";
  entity.def = "struct " + typeName + " {" EOL;
  entity.def += "  " + this->_type(fn.returnType, true, false) + "(*f) ";
  entity.def += "(void *" + (fn.params.empty() ? "" : ", struct " + paramsName) + ");" EOL;
  entity.def += "  void *x;" EOL;
  entity.def += "  size_t l;" EOL;
  entity.def += "};";

  auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdlib", "libString" }, { typeName }};
  copyFnEntity.decl = "struct " + typeName + " " + typeName + "_copy (const struct " + typeName + ");";
  copyFnEntity.def = "struct " + typeName + " " + typeName + "_copy (const struct " + typeName + " n) {" EOL;
  copyFnEntity.def += "  if (n.x == NULL) return n;" EOL;
  copyFnEntity.def += "  void *x = alloc(n.l);" EOL;
  copyFnEntity.def += "  memcpy(x, n.x, n.l);" EOL;
  copyFnEntity.def += "  return (struct " + typeName + ") {n.f, x, n.l};" EOL;
  copyFnEntity.def += "}";

  auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
  freeFnEntity.decl = "void " + typeName + "_free (struct " + typeName + ");";
  freeFnEntity.def = "void " + typeName + "_free (struct " + typeName + " n) {" EOL;
  freeFnEntity.def += "  free(n.x);" EOL;
  freeFnEntity.def += "}";

  auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
  reallocFnEntity.decl += "struct " + typeName + " " + typeName + "_realloc (struct " + typeName + ", struct " + typeName + ");";
  reallocFnEntity.def += "struct " + typeName + " " + typeName + "_realloc (struct " + typeName + " n1, struct " + typeName + " n2) {" EOL;
  reallocFnEntity.def += "  if (n1.x != NULL) free(n1.x);" EOL;
  reallocFnEntity.def += "  return n2;" EOL;
  reallocFnEntity.def += "}";

  this->entities.push_back(entity);
  this->entities.push_back(copyFnEntity);
  this->entities.push_back(freeFnEntity);
  this->entities.push_back(reallocFnEntity);

  return typeName;
}

std::string Codegen::_wrapNode ([[maybe_unused]] const ASTNode &node, const std::string &code) const {
  return code;
}

std::string Codegen::_wrapNodeExpr (const ASTNodeExpr &nodeExpr, const std::string &code) const {
  auto result = code;

  if (nodeExpr.parenthesized) {
    result = "(" + result + ")";
  }

  return result;
}
