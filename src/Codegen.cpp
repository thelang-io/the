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

  auto mainCode = std::string("int main () {" EOL);
  mainCode += this->_block(nodes, false);
  mainCode += this->state.cleanUp.gen(2);
  mainCode += "}" EOL;

  auto fnDeclCode = std::string();
  auto fnDefCode = std::string();
  auto structDeclCode = std::string();
  auto structDefCode = std::string();

  for (const auto &entity : this->entities) {
    if (!entity.active || entity.decl.empty() || entity.def.empty()) {
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

  if (this->builtins.typeStr) {
    builtinStructDefCode += "struct str {" EOL;
    builtinStructDefCode += "  char *d;" EOL;
    builtinStructDefCode += "  size_t l;" EOL;
    builtinStructDefCode += "};" EOL;
  }

  if (this->builtins.fnAlloc) {
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

  if (this->builtins.fnBoolStr) {
    builtinFnDeclCode += "struct str bool_str (bool);" EOL;
    builtinFnDefCode += "struct str bool_str (bool t) {" EOL;
    builtinFnDefCode += R"(  return str_alloc(t ? "true" : "false");)" EOL;
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
    builtinFnDefCode += "  if ((i >= 0 && i >= s.l) || (i < 0 && i < -s.l)) {" EOL;
    builtinFnDefCode += R"(    fprintf(stderr, "Error: index %" PRId64 " out of string bounds)" ESC_EOL R"(", i);)" EOL;
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
    builtinFnDefCode += "      if (l + 2 > s.l) d = realloc(d, l + 2);" EOL;
    builtinFnDefCode += R"(      d[l++] = '\\';)" EOL;
    builtinFnDefCode += R"(      if (c == '\f') d[l++] = 'f';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\n') d[l++] = 'n';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\r') d[l++] = 'r';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\t') d[l++] = 't';)" EOL;
    builtinFnDefCode += R"(      else if (c == '\v') d[l++] = 'v';)" EOL;
    builtinFnDefCode += R"(      else if (c == '"') d[l++] = '"';)" EOL;
    builtinFnDefCode += "      continue;" EOL;
    builtinFnDefCode += "    }" EOL;
    builtinFnDefCode += "    if (l + 1 > s.l) d = realloc(d, l + 1);" EOL;
    builtinFnDefCode += "    d[l++] = c;" EOL;
    builtinFnDefCode += "  }" EOL;
    builtinFnDefCode += "  return (struct str) {d, l};" EOL;
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
    builtinFnDefCode += "  int64_t i1 = o1 == 0 ? 0 : (n1 < 0 ? (n1 < -s.l ? 0 : n1 + s.l) : (n1 > s.l ? s.l : n1));" EOL;
    builtinFnDefCode += "  int64_t i2 = o2 == 0 ? s.l : (n2 < 0 ? (n2 < -s.l ? 0 : n2 + s.l) : (n2 > s.l ? s.l : n2));" EOL;
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
  headers += this->builtins.libUnistd ? "#include <unistd.h>" EOL : "";
  headers += this->builtins.libWindows ? "#include <windows.h>" EOL : "";
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
  output += mainCode;

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

  if (name == "libInttypes") {
    this->builtins.libInttypes = true;
  } else if (name == "libStdarg") {
    this->builtins.libStdarg = true;
  } else if (name == "libStdbool") {
    this->builtins.libStdbool = true;
  } else if (name == "libStdint") {
    this->builtins.libStdint = true;
  } else if (name == "libStdio") {
    this->builtins.libStdio = true;
  } else if (name == "libStdlib") {
    this->builtins.libStdlib = true;
  } else if (name == "libString") {
    this->builtins.libString = true;
  } else if (name == "libUnistd") {
    this->builtins.libUnistd = true;
  } else if (name == "libWindows") {
    this->builtins.libWindows = true;
  } else if (name == "fnAlloc") {
    this->builtins.fnAlloc = true;
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("libStdlib");
  } else if (name == "fnBoolStr") {
    this->builtins.fnBoolStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnByteStr") {
    this->builtins.fnByteStr = true;
    this->_activateBuiltin("fnStrAlloc");
    this->_activateBuiltin("libStdio");
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
  } else if (name == "fnPrint") {
    this->builtins.fnPrint = true;
    this->_activateBuiltin("fnStrFree");
    this->_activateBuiltin("libInttypes");
    this->_activateBuiltin("libStdarg");
    this->_activateBuiltin("libStdio");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrAlloc") {
    this->builtins.fnStrAlloc = true;
    this->_activateBuiltin("fnAlloc");
    this->_activateBuiltin("libString");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrAt") {
    this->builtins.fnStrAt = true;
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
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrFree") {
    this->builtins.fnStrFree = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeStr");
  } else if (name == "fnStrLen") {
    this->builtins.fnStrLen = true;
    this->_activateBuiltin("libStdlib");
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
  } else if (name == "typeStr") {
    this->builtins.typeStr = true;
    this->_activateBuiltin("libStdlib");
  } else {
    throw Error("tried activating unknown builtin");
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

  throw Error("tried activating unknown entity");
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
    if (
      std::holds_alternative<ASTNodeFnDecl>(*nodes[i].body) &&
      i != nodes.size() - 1 &&
      std::holds_alternative<ASTNodeFnDecl>(*nodes[i + 1].body)
    ) {
      for (auto j = i; j < nodes.size() && std::holds_alternative<ASTNodeFnDecl>(*nodes[j].body); j++) {
        code += this->_node(nodes[j], true, CODEGEN_PHASE_ALLOC);
      }

      for (; i < nodes.size() && std::holds_alternative<ASTNodeFnDecl>(*nodes[i].body); i++) {
        code += this->_node(nodes[i], true, CODEGEN_PHASE_INIT);
      }

      i--;
    } else if (std::holds_alternative<ASTNodeMain>(*nodes[i].body)) {
      auto saveIndent = this->indent;

      this->indent = 0;
      code += this->_node(nodes[i]);
      this->indent = saveIndent;
    } else {
      code += this->_node(nodes[i]);
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

    if (!ASTChecker(nodes).endsWith<ASTNodeReturn>() && this->state.cleanUp.returnVarUsed) {
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

        auto exprBinaryLeftAccess = std::get<ASTExprAccess>(*exprBinaryLeft.body);
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
      }
    }
  }

  return std::make_tuple(bodyTypeCasts, altTypeCasts);
}

std::string Codegen::_flags () const {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : this->flags) {
    result += (idx++ == 0 ? "" : " ") + flag;
  }

  return result;
}

std::string Codegen::_genEqFn (
  const CodegenTypeInfo &typeInfo,
  const std::variant<std::string, ASTNodeExpr> &exprLeft,
  const std::variant<std::string, ASTNodeExpr> &exprRight,
  const std::optional<std::vector<std::string> *> &entityBuiltins,
  const std::optional<std::vector<std::string> *> &entityEntities,
  bool reverse
) {
  auto initialState = this->state;

  if (entityBuiltins != std::nullopt && entityEntities != std::nullopt) {
    this->state.builtins = entityBuiltins;
    this->state.entities = entityEntities;
  }

  auto eqFnOp = std::string(reverse ? "!=" : "==");
  auto eqFnName = std::string(reverse ? "ne" : "eq");
  auto eqFnNameB = Token::upperFirst(eqFnName);
  auto code = std::string();

  if (std::holds_alternative<std::string>(exprLeft) && std::holds_alternative<std::string>(exprRight)) {
    auto exprLeftCode = std::get<std::string>(exprLeft);
    auto exprRightCode = std::get<std::string>(exprRight);

    if (typeInfo.realType->isArray() || typeInfo.realType->isObj() || typeInfo.realType->isOpt()) {
      this->_activateEntity(typeInfo.realTypeName + "_copy");
      this->_activateEntity(typeInfo.realTypeName + "_" + eqFnName);

      code = typeInfo.realTypeName + "_" + eqFnName + "(";
      code += typeInfo.realTypeName + "_copy(" + exprLeftCode + "), ";
      code += typeInfo.realTypeName + "_copy(" + exprRightCode + "))";
    } else if (typeInfo.realType->isStr()) {
      this->_activateBuiltin("fnStrCopy");
      this->_activateBuiltin("fnStr" + eqFnNameB + "Str");
      code = "str_" + eqFnName + "_str(str_copy(" + exprLeftCode + "), str_copy(" + exprRightCode + "))";
    } else {
      code = exprLeftCode + " " + eqFnOp + " " + exprRightCode;
    }
  } else {
    auto exprLeftExpr = std::get<ASTNodeExpr>(exprLeft);
    auto exprRightExpr = std::get<ASTNodeExpr>(exprRight);

    if (
      (Type::real(exprLeftExpr.type)->isArray() && Type::real(exprRightExpr.type)->isArray()) ||
      (Type::real(exprLeftExpr.type)->isObj() && Type::real(exprRightExpr.type)->isObj()) ||
      (Type::real(exprLeftExpr.type)->isOpt() && Type::real(exprRightExpr.type)->isOpt())
    ) {
      auto nodeTypeInfo = this->_typeInfo(exprLeftExpr.type);
      auto leftCode = this->_nodeExpr(exprLeftExpr, exprLeftExpr.type);
      auto rightCode = this->_nodeExpr(exprRightExpr, exprRightExpr.type);

      this->_activateEntity(nodeTypeInfo.realTypeName + "_" + eqFnName);
      code = nodeTypeInfo.realTypeName + "_" + eqFnName + "(" + leftCode + ", " + rightCode + ")";
    } else if (Type::real(exprLeftExpr.type)->isStr() && Type::real(exprRightExpr.type)->isStr()) {
      if (exprLeftExpr.isLit() && exprRightExpr.isLit()) {
        this->_activateBuiltin("fnCstr" + eqFnNameB + "Cstr");
        code = "cstr_" + eqFnName + "_cstr(" + exprLeftExpr.litBody() + ", " + exprRightExpr.litBody() + ")";
      } else if (exprLeftExpr.isLit()) {
        auto rightCode = this->_nodeExpr(exprRightExpr, exprRightExpr.type);
        this->_activateBuiltin("fnCstr" + eqFnNameB + "Str");
        code = "cstr_" + eqFnName + "_str(" + exprLeftExpr.litBody() + ", " + rightCode + ")";
      } else if (exprRightExpr.isLit()) {
        auto leftCode = this->_nodeExpr(exprLeftExpr, exprLeftExpr.type);
        this->_activateBuiltin("fnStr" + eqFnNameB + "Cstr");
        code = "str_" + eqFnName + "_cstr(" + leftCode + ", " + exprRightExpr.litBody() + ")";
      } else {
        auto leftCode = this->_nodeExpr(exprLeftExpr, exprLeftExpr.type);
        auto rightCode = this->_nodeExpr(exprRightExpr, exprRightExpr.type);

        this->_activateBuiltin("fnStr" + eqFnNameB + "Str");
        code = "str_" + eqFnName + "_str(" + leftCode + ", " + rightCode + ")";
      }
    }
  }

  if (entityBuiltins != std::nullopt && entityEntities != std::nullopt) {
    this->state = initialState;
  }

  return code;
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
    auto typeName = Codegen::typeName(nodeFnDecl.var->codeName);
    auto contextName = typeName + "X";

    auto saveIndent = this->indent;

    if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
      auto saveStateBuiltins = this->state.builtins;
      auto saveStateEntities = this->state.entities;
      auto initialStateCleanUp = this->state.cleanUp;
      auto saveStateContextVars = this->state.contextVars;

      auto paramsName = varTypeInfo.typeName + "P";
      auto entity = CodegenEntity{typeName, CODEGEN_ENTITY_FN, {}, { varTypeInfo.typeName }};

      this->varMap.save();
      this->indent = 2;
      this->state.builtins = &entity.builtins;
      this->state.entities = &entity.entities;
      this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_FN, &initialStateCleanUp);

      auto bodyCode = std::string();

      if (!nodeFnDecl.stack.empty()) {
        bodyCode += "  struct " + contextName + " *x = px;" EOL;

        auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc", "libString" }, { varTypeInfo.typeName, typeName, contextName }};
        allocFnEntity.decl += "void " + typeName + "_alloc (" + varTypeInfo.typeRefCode + ", struct " + contextName + ");";
        allocFnEntity.def += "void " + typeName + "_alloc (" + varTypeInfo.typeRefCode + "n, struct " + contextName + " x) {" EOL;
        allocFnEntity.def += "  size_t l = sizeof(struct " + contextName + ");" EOL;
        allocFnEntity.def += "  struct " + contextName + " *r = alloc(l);" EOL;
        allocFnEntity.def += "  memcpy(r, &x, l);" EOL;
        allocFnEntity.def += "  n->f = &" + typeName + ";" EOL;
        allocFnEntity.def += "  n->x = r;" EOL;
        allocFnEntity.def += "  n->l = l;" EOL;
        allocFnEntity.def += "}";

        auto contextEntity = CodegenEntity{contextName, CODEGEN_ENTITY_OBJ};
        contextEntity.decl += "struct " + contextName + ";";
        contextEntity.def += "struct " + contextName + " {" EOL;

        for (const auto &contextVar : nodeFnDecl.stack) {
          auto contextVarName = Codegen::name(contextVar->codeName);
          auto contextVarTypeInfo = this->_typeInfo(contextVar->type);

          contextEntity.def += "  " + (contextVar->mut ? contextVarTypeInfo.typeRefCode : contextVarTypeInfo.typeRefCodeConst) + contextVarName + ";" EOL;
          bodyCode += "  " + (contextVar->mut ? contextVarTypeInfo.typeRefCode : contextVarTypeInfo.typeRefCodeConst) + contextVarName + " = x->" + contextVarName + ";" EOL;

          this->state.contextVars.insert(contextVarName);
        }

        contextEntity.def += "};";

        this->entities.push_back(allocFnEntity);
        this->entities.push_back(contextEntity);
        this->_activateEntity(contextName, &entity.entities);
      }

      if (!nodeFnDecl.params.empty()) {
        auto paramIdx = static_cast<std::size_t>(0);

        for (const auto &param : nodeFnDecl.params) {
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

          if (
            paramTypeInfo.type->isArray() ||
            paramTypeInfo.type->isFn() ||
            paramTypeInfo.type->isObj() ||
            paramTypeInfo.type->isOpt()
          ) {
            this->_activateEntity(paramTypeInfo.typeName + "_free");
            this->state.cleanUp.add(paramTypeInfo.typeName + "_free((" + paramTypeInfo.typeCodeTrimmed + ") " + paramName + ");");
          } else if (paramTypeInfo.type->isStr()) {
            this->_activateBuiltin("fnStrFree");
            this->state.cleanUp.add("str_free((struct str) " + paramName + ");");
          }

          paramIdx++;
        }

        this->_activateEntity(paramsName, &entity.entities);
      }

      auto returnTypeInfo = this->_typeInfo(std::get<TypeFn>(nodeFnDecl.var->type->body).returnType);

      this->indent = 0;
      this->state.returnType = returnTypeInfo.type;
      bodyCode += this->_block(nodeFnDecl.body, false);
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
        bodyCode.insert(0, std::string(this->indent, ' ') + "unsigned char r = 0;" EOL);
      }

      if (nodeFnDecl.params.empty()) {
        entity.decl += returnTypeInfo.typeCode + typeName + " (void *);";
        entity.def += returnTypeInfo.typeCode + typeName + " (void *px) {" EOL + bodyCode + "}";
      } else {
        entity.decl += returnTypeInfo.typeCode + typeName + " (void *, struct " + paramsName + ");";
        entity.def += returnTypeInfo.typeCode + typeName + " (void *px, struct " + paramsName + " p) {" EOL + bodyCode + "}";
      }

      this->entities.push_back(entity);

      this->state.builtins = saveStateBuiltins;
      this->state.entities = saveStateEntities;
      this->state.cleanUp = initialStateCleanUp;
      this->state.contextVars = saveStateContextVars;
    }

    this->indent = saveIndent;
    auto fnName = Codegen::name(nodeFnDecl.var->codeName);

    if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
      this->_activateEntity(varTypeInfo.typeName);
      code += std::string(this->indent, ' ') + "const " + varTypeInfo.typeCode + fnName;
    }

    if ((phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) && nodeFnDecl.stack.empty()) {
      this->_activateBuiltin("libStdlib");
      this->_activateEntity(typeName);

      code += " = (" + varTypeInfo.typeCodeTrimmed + ") {&" + typeName + ", NULL, 0};" EOL;
    } else if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
      code += ";" EOL;
    }

    if ((phase == CODEGEN_PHASE_INIT || phase == CODEGEN_PHASE_FULL) && !nodeFnDecl.stack.empty()) {
      this->_activateEntity(typeName + "_alloc");
      code += std::string(this->indent, ' ') + typeName + "_alloc((" + varTypeInfo.typeRefCode + ") &" + fnName + ", ";

      this->_activateEntity(contextName);
      code += "(struct " + contextName + ") {";

      auto contextVarIdx = static_cast<std::size_t>(0);

      for (const auto &contextVar : nodeFnDecl.stack) {
        auto contextVarName = Codegen::name(contextVar->codeName);

        code += contextVarIdx == 0 ? "" : ", ";
        code += (this->state.contextVars.contains(contextVarName) ? "" : "&") + contextVarName;

        contextVarIdx++;
      }

      code += "});" EOL;

      this->_activateEntity(varTypeInfo.typeName + "_free");
      this->state.cleanUp.add(varTypeInfo.typeName + "_free((" + varTypeInfo.typeCodeTrimmed + ") " + fnName + ");");
    }

    this->indent = saveIndent;
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
    auto typeName = Codegen::typeName(nodeObjDecl.type->codeName);

    auto saveStateBuiltins = this->state.builtins;
    auto saveStateEntities = this->state.entities;

    auto objEntity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ};
    objEntity.decl += "struct " + typeName + ";";
    objEntity.def += "struct " + typeName + " {" EOL;

    this->state.builtins = &objEntity.builtins;
    this->state.entities = &objEntity.entities;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      objEntity.def += "  " + fieldTypeInfo.typeCode + fieldName + ";" EOL;
    }

    objEntity.def += "};";

    this->entities.push_back(objEntity);
    auto typeInfo = this->_typeInfo(nodeObjDecl.type);
    auto fieldIdx = static_cast<std::size_t>(0);

    auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName }};
    auto allocFnParamTypes = std::string();
    auto allocFnParams = std::string();
    auto allocFnCode = std::string();

    this->state.builtins = &allocFnEntity.builtins;
    this->state.entities = &allocFnEntity.entities;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      allocFnParamTypes += ", " + fieldTypeInfo.typeCodeTrimmed;
      allocFnParams += ", " + fieldTypeInfo.typeCode + fieldName;
      allocFnCode += "  r->" + fieldName + " = " + fieldName + ";" EOL;
    }

    allocFnEntity.decl += typeInfo.typeCode + typeName + "_alloc (" + allocFnParamTypes.substr(2) + ");";
    allocFnEntity.def += typeInfo.typeCode + typeName + "_alloc (" + allocFnParams.substr(2) + ") {" EOL;
    allocFnEntity.def += "  " + typeInfo.typeCode + "r = alloc(sizeof(struct " + typeName + "));" EOL;
    allocFnEntity.def += allocFnCode;
    allocFnEntity.def += "  return r;" EOL;
    allocFnEntity.def += "}";

    auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName }};
    copyFnEntity.decl += typeInfo.typeCode + typeName + "_copy (const " + typeInfo.typeCode + ");";
    copyFnEntity.def += typeInfo.typeCode + typeName + "_copy (const " + typeInfo.typeCode + "o) {" EOL;
    copyFnEntity.def += "  " + typeInfo.typeCode + "r = alloc(sizeof(struct " + typeName + "));" EOL;

    this->state.builtins = &copyFnEntity.builtins;
    this->state.entities = &copyFnEntity.entities;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      if (
        fieldTypeInfo.type->isArray() ||
        fieldTypeInfo.type->isFn() ||
        fieldTypeInfo.type->isObj() ||
        fieldTypeInfo.type->isOpt()
      ) {
        this->_activateEntity(fieldTypeInfo.typeName + "_copy");
        copyFnEntity.def += "  r->" + fieldName + " = " + fieldTypeInfo.typeName + "_copy(o->" + fieldName + ");" EOL;
      } else if (fieldTypeInfo.type->isStr()) {
        this->_activateBuiltin("fnStrCopy");
        copyFnEntity.def += "  r->" + fieldName + " = str_copy(o->" + fieldName + ");" EOL;
      } else {
        copyFnEntity.def += "  r->" + fieldName + " = o->" + fieldName + ";" EOL;
      }
    }

    copyFnEntity.def += "  return r;" EOL;
    copyFnEntity.def += "}";

    auto eqFnEntity = CodegenEntity{typeName + "_eq", CODEGEN_ENTITY_FN, { "libStdbool", "libStdlib" }, { typeName, typeName + "_free" }};
    eqFnEntity.decl += "bool " + typeName + "_eq (" + typeInfo.typeCode + ", " + typeInfo.typeCode + ");";
    eqFnEntity.def += "bool " + typeName + "_eq (" + typeInfo.typeCode + "o1, " + typeInfo.typeCode + "o2) {" EOL;
    eqFnEntity.def += "  bool r = ";

    fieldIdx = 0;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      eqFnEntity.def += (fieldIdx == 0 ? "" : " && ") + this->_genEqFn(fieldTypeInfo, "o1->" + fieldName, "o2->" + fieldName, &eqFnEntity.builtins, &eqFnEntity.entities);
      fieldIdx++;
    }

    eqFnEntity.def += ";" EOL;
    eqFnEntity.def += "  " + typeName + "_free((" + typeInfo.typeCodeTrimmed + ") o1);" EOL;
    eqFnEntity.def += "  " + typeName + "_free((" + typeInfo.typeCodeTrimmed + ") o2);" EOL;
    eqFnEntity.def += "  return r;" EOL;
    eqFnEntity.def += "}";

    auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
    freeFnEntity.decl += "void " + typeName + "_free (" + typeInfo.typeCode + ");";
    freeFnEntity.def += "void " + typeName + "_free (" + typeInfo.typeCode + "o) {" EOL;

    this->state.builtins = &freeFnEntity.builtins;
    this->state.entities = &freeFnEntity.entities;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      if (
        fieldTypeInfo.type->isArray() ||
        fieldTypeInfo.type->isFn() ||
        fieldTypeInfo.type->isObj() ||
        fieldTypeInfo.type->isOpt()
      ) {
        this->_activateEntity(fieldTypeInfo.typeName + "_free");

        freeFnEntity.def += "  " + fieldTypeInfo.typeName + "_free((" + fieldTypeInfo.typeCodeTrimmed + ") o->" + fieldName + ");" EOL;
      } else if (fieldTypeInfo.type->isStr()) {
        this->_activateBuiltin("fnStrFree");
        freeFnEntity.def += "  str_free((struct str) o->" + fieldName + ");" EOL;
      }
    }

    freeFnEntity.def += "  free(o);" EOL;
    freeFnEntity.def += "}";

    auto neFnEntity = CodegenEntity{typeName + "_ne", CODEGEN_ENTITY_FN, { "libStdbool", "libStdlib" }, { typeName, typeName + "_free" }};
    neFnEntity.decl += "bool " + typeName + "_ne (" + typeInfo.typeCode + ", " + typeInfo.typeCode + ");";
    neFnEntity.def += "bool " + typeName + "_ne (" + typeInfo.typeCode + "o1, " + typeInfo.typeCode + "o2) {" EOL;
    neFnEntity.def += "  bool r = ";

    fieldIdx = 0;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);

      neFnEntity.def += (fieldIdx == 0 ? "" : " || ") + this->_genEqFn(fieldTypeInfo, "o1->" + fieldName, "o2->" + fieldName, &neFnEntity.builtins, &neFnEntity.entities, true);
      fieldIdx++;
    }

    neFnEntity.def += ";" EOL;
    neFnEntity.def += "  " + typeName + "_free((" + typeInfo.typeCodeTrimmed + ") o1);" EOL;
    neFnEntity.def += "  " + typeName + "_free((" + typeInfo.typeCodeTrimmed + ") o2);" EOL;
    neFnEntity.def += "  return r;" EOL;
    neFnEntity.def += "}";

    auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, {}, { typeName, typeName + "_free" }};
    reallocFnEntity.decl += typeInfo.typeCode + typeName + "_realloc (" + typeInfo.typeCode + ", " + typeInfo.typeCode + ");";
    reallocFnEntity.def += typeInfo.typeCode + typeName + "_realloc (" + typeInfo.typeCode + "o1, " + typeInfo.typeCode + "o2) {" EOL;
    reallocFnEntity.def += "  " + typeName + "_free((" + typeInfo.typeCodeTrimmed + ") o1);" EOL;
    reallocFnEntity.def += "  return o2;" EOL;
    reallocFnEntity.def += "}";

    auto strFnEntity = CodegenEntity{typeName + "_str", CODEGEN_ENTITY_FN, {
      "fnStrAlloc",
      "fnStrConcatCstr",
      "fnStrConcatStr",
      "typeStr"
    }, { typeName, typeName + "_free" }};

    strFnEntity.decl += "struct str " + typeName + "_str (" + typeInfo.typeCode + ");";
    strFnEntity.def += "struct str " + typeName + "_str (" + typeInfo.typeCode + "o) {" EOL;
    strFnEntity.def += R"(  struct str r = str_alloc(")" + nodeObjDecl.type->name + R"({");)" EOL;

    this->state.builtins = &strFnEntity.builtins;
    this->state.entities = &strFnEntity.entities;

    fieldIdx = 0;

    for (const auto &field : nodeObjDecl.type->fields) {
      if (field.builtin) {
        continue;
      }

      auto fieldName = Codegen::name(field.name);
      auto fieldTypeInfo = this->_typeInfo(field.type);
      auto fieldRealTypeCode = std::string(fieldTypeInfo.type->isRef() ? "*" : "") + "o->" + fieldName;
      auto strCodeDelimiter = std::string(fieldIdx == 0 ? "" : ", ");

      if (
        fieldTypeInfo.realType->isArray() ||
        fieldTypeInfo.realType->isFn() ||
        fieldTypeInfo.realType->isObj() ||
        fieldTypeInfo.realType->isOpt()
      ) {
        this->_activateEntity(fieldTypeInfo.realTypeName + "_copy");
        this->_activateEntity(fieldTypeInfo.realTypeName + "_str");

        strFnEntity.def += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + field.name + R"(: ");)" EOL;
        strFnEntity.def += "  r = str_concat_str(r, " + fieldTypeInfo.realTypeName + "_str(" + fieldTypeInfo.realTypeName + "_copy(" + fieldRealTypeCode + ")));" EOL;
      } else if (fieldTypeInfo.realType->isStr()) {
        this->_activateBuiltin("fnStrEscape");

        strFnEntity.def += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + field.name + R"(: \"");)" EOL;
        strFnEntity.def += "  r = str_concat_str(r, str_escape(" + fieldRealTypeCode + "));" EOL;
        strFnEntity.def += R"(  r = str_concat_cstr(r, "\"");)" EOL;
      } else {
        auto fieldTypeBuiltinName = Token::upperFirst(fieldTypeInfo.realTypeName);
        this->_activateBuiltin("fn" + fieldTypeBuiltinName + "Str");

        strFnEntity.def += R"(  r = str_concat_cstr(r, ")" + strCodeDelimiter + field.name + R"(: ");)" EOL;
        strFnEntity.def += "  r = str_concat_str(r, " + fieldTypeInfo.realTypeName + "_str(" + fieldRealTypeCode + "));" EOL;
      }

      fieldIdx++;
    }

    strFnEntity.def += "  " + typeName + "_free((" + typeInfo.typeCodeTrimmed + ") o);" EOL;
    strFnEntity.def += R"(  return str_concat_cstr(r, "}");)" EOL;
    strFnEntity.def += "}";

    this->entities.push_back(allocFnEntity);
    this->entities.push_back(copyFnEntity);
    this->entities.push_back(eqFnEntity);
    this->entities.push_back(freeFnEntity);
    this->entities.push_back(neFnEntity);
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
    auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);
    auto initCode = std::string();

    if (nodeVarDecl.init != std::nullopt) {
      initCode = this->_nodeExpr(*nodeVarDecl.init, typeInfo.type);
    } else if (typeInfo.type->isArray()) {
      this->_activateEntity(typeInfo.typeName + "_alloc");
      initCode = typeInfo.typeName + "_alloc(0)";
    } else if (typeInfo.type->isBool()) {
      this->_activateBuiltin("libStdbool");
      initCode = "false";
    } else if (typeInfo.type->isChar()) {
      initCode = R"('\0')";
    } else if (typeInfo.type->isOpt()) {
      this->_activateBuiltin("libStdlib");
      initCode = "NULL";
    } else if (typeInfo.type->isStr()) {
      this->_activateBuiltin("fnStrAlloc");
      initCode = R"(str_alloc(""))";
    } else if (!typeInfo.type->isFn() && !typeInfo.type->isObj() && !typeInfo.type->isRef()) {
      initCode = "0";
    }

    if (root) {
      code = std::string(this->indent, ' ');
    }

    code += (nodeVarDecl.var->mut ? typeInfo.typeCode :typeInfo.typeCodeConst) + name + " = " + initCode + (root ? ";" EOL : "");

    if (
      typeInfo.type->isArray() ||
      typeInfo.type->isFn() ||
      typeInfo.type->isObj() ||
      typeInfo.type->isOpt()
    ) {
      this->_activateEntity(typeInfo.typeName + "_free");
      this->state.cleanUp.add(typeInfo.typeName + "_free((" + typeInfo.typeCodeTrimmed + ") " + name + ");");
    } else if (typeInfo.type->isStr()) {
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
      auto objCode = Codegen::name(objVar->codeName);
      auto objType = this->state.typeCasts.contains(objCode) ? this->state.typeCasts[objCode] : objVar->type;

      code = objCode;

      if (objVar->type->isOpt() && !objType->isOpt()) {
        code = "*" + code;
      }

      if (this->state.contextVars.contains(objCode) && (nodeExpr.type->isRefExt() || !targetType->isRefExt())) {
        code = "*" + code;
      }

      if (!this->state.contextVars.contains(objCode) && !nodeExpr.type->isRefExt() && targetType->isRefExt()) {
        code = "&" + code;
      } else if (nodeExpr.type->isRefExt() && !targetType->isRefExt()) {
        code = "*" + code;
      }

      if (!objType->isRef() || !targetType->isRef()) {
        auto nodeTypeInfo = this->_typeInfo(objType);

        if (
          (!root && nodeTypeInfo.realType->isArray()) ||
          (!root && nodeTypeInfo.realType->isFn()) ||
          (!root && nodeTypeInfo.realType->isObj()) ||
          (!root && nodeTypeInfo.realType->isOpt())
        ) {
          this->_activateEntity(nodeTypeInfo.realTypeName + "_copy");
          code = nodeTypeInfo.realTypeName + "_copy(" + code + ")";
        } else if (!root && nodeTypeInfo.realType->isStr()) {
          this->_activateBuiltin("fnStrCopy");
          code = "str_copy(" + code + ")";
        }
      }

      return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
    }

    auto objNodeExpr = std::get<ASTNodeExpr>(exprAccess.expr);
    auto objTypeInfo = this->_typeInfo(objNodeExpr.type);

    if (exprAccess.prop == "len" && objTypeInfo.realType->isArray()) {
      auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType);

      if (!objNodeExpr.parenthesized) {
        objCode = "(" + objCode + ")";
      }

      this->_activateEntity(objTypeInfo.realTypeName + "_len");
      code = objTypeInfo.realTypeName + "_len" + objCode;
    } else if (exprAccess.prop == "len" && objTypeInfo.realType->isStr()) {
      auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType);
      this->_activateBuiltin("fnStrLen");

      if (objNodeExpr.parenthesized) {
        code = "str_len" + objCode;
      } else {
        code = "str_len(" + objCode + ")";
      }
    } else if (exprAccess.prop != std::nullopt) {
      auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, true);

      if (objCode.starts_with("*")) {
        objCode = "(" + objCode + ")";
      }

      code = objCode + "->" + Codegen::name(*exprAccess.prop);
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

    if (!nodeExpr.type->isRef() && targetType->isRef()) {
      code = "&" + code;
    } else if (nodeExpr.type->isRef() && !targetType->isRef()) {
      code = "*" + code;
    }

    if (!nodeExpr.type->isRef() || !targetType->isRef()) {
      auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);

      if (
        (!root && nodeTypeInfo.realType->isArray()) ||
        (!root && nodeTypeInfo.realType->isFn()) ||
        (!root && nodeTypeInfo.realType->isObj()) ||
        (!root && nodeTypeInfo.realType->isOpt())
      ) {
        this->_activateEntity(nodeTypeInfo.realTypeName + "_copy");
        code = nodeTypeInfo.realTypeName + "_copy(" + code + ")";
      } else if (!root && nodeTypeInfo.realType->isStr()) {
        this->_activateBuiltin("fnStrCopy");
        code = "str_copy(" + code + ")";
      }
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprArray>(*nodeExpr.body)) {
    auto exprArray = std::get<ASTExprArray>(*nodeExpr.body);
    auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
    auto arrayType = std::get<TypeArray>(nodeTypeInfo.type->body);

    this->_activateEntity(nodeTypeInfo.typeName + "_alloc");
    auto code = nodeTypeInfo.typeName + "_alloc(" + std::to_string(exprArray.elements.size());

    for (auto idx = static_cast<std::size_t>(0); idx < exprArray.elements.size(); idx++) {
      code += ", " + this->_nodeExpr(exprArray.elements[idx], arrayType.elementType);
    }

    code += ")";

    if (root) {
      this->_activateEntity(nodeTypeInfo.typeName + "_free");
      code = nodeTypeInfo.typeName + "_free((" + nodeTypeInfo.typeCodeTrimmed + ") " + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
    auto leftCode = this->_nodeExpr(exprAssign.left, nodeTypeInfo.type, true);

    if (
      exprAssign.left.type->isArray() ||
      exprAssign.left.type->isFn() ||
      exprAssign.left.type->isObj() ||
      exprAssign.left.type->isOpt()
    ) {
      this->_activateEntity(nodeTypeInfo.typeName + "_realloc");
      auto code = leftCode + " = " + nodeTypeInfo.typeName + "_realloc(" + leftCode + ", " + this->_nodeExpr(exprAssign.right, nodeTypeInfo.type) + ")";

      if (!root) {
        this->_activateEntity(nodeTypeInfo.typeName + "_copy");
        code = nodeTypeInfo.typeName + "_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
    } else if (exprAssign.left.type->isStr() || exprAssign.right.type->isStr()) {
      auto rightCode = std::string();

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        this->_activateBuiltin("fnStrCopy");

        if (exprAssign.right.isLit()) {
          this->_activateBuiltin("fnStrConcatCstr");
          rightCode = "str_concat_cstr(str_copy(" + leftCode + "), " + exprAssign.right.litBody() + ")";
        } else {
          this->_activateBuiltin("fnStrConcatStr");
          rightCode = "str_concat_str(str_copy(" + leftCode + "), " + this->_nodeExpr(exprAssign.right, nodeTypeInfo.type) + ")";
        }
      } else {
        rightCode = this->_nodeExpr(exprAssign.right, nodeTypeInfo.type);
      }

      this->_activateBuiltin("fnStrRealloc");
      auto code = leftCode + " = str_realloc(" + leftCode + ", " + rightCode + ")";

      if (!root) {
        this->_activateBuiltin("fnStrCopy");
        code = "str_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
    }

    auto opCode = std::string(" = ");
    auto rightCode = this->_nodeExpr(exprAssign.right, nodeTypeInfo.type);

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

    return this->_wrapNodeExpr(nodeExpr, targetType, root, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if ((exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) && (
      (Type::real(exprBinary.left.type)->isArray() && Type::real(exprBinary.right.type)->isArray()) ||
      (Type::real(exprBinary.left.type)->isObj() && Type::real(exprBinary.right.type)->isObj()) ||
      (Type::real(exprBinary.left.type)->isOpt() && Type::real(exprBinary.right.type)->isOpt()) ||
      (Type::real(exprBinary.left.type)->isStr() && Type::real(exprBinary.right.type)->isStr())
    )) {
      auto nodeTypeInfo = this->_typeInfo(exprBinary.left.type);
      auto code = this->_genEqFn(nodeTypeInfo, exprBinary.left, exprBinary.right, std::nullopt, std::nullopt, exprBinary.op == AST_EXPR_BINARY_NE);
      return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
    }

    if (Type::real(exprBinary.left.type)->isStr() && Type::real(exprBinary.right.type)->isStr()) {
      if (exprBinary.op == AST_EXPR_BINARY_ADD) {
        auto code = std::string();

        if (root && nodeExpr.isLit()) {
          return this->_wrapNodeExpr(nodeExpr, targetType, root, nodeExpr.litBody());
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

        return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
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

    return this->_wrapNodeExpr(nodeExpr, targetType, root, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) {
    auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);
    auto exprCallCalleeTypeInfo = this->_typeInfo(exprCall.callee.type);
    auto code = std::string();

    if (exprCallCalleeTypeInfo.realType->builtin && (
      exprCallCalleeTypeInfo.realType->codeName == "@array.join" ||
      exprCallCalleeTypeInfo.realType->codeName == "@array.pop" ||
      exprCallCalleeTypeInfo.realType->codeName == "@array.push" ||
      exprCallCalleeTypeInfo.realType->codeName == "@array.reverse" ||
      exprCallCalleeTypeInfo.realType->codeName == "@array.slice" ||
      exprCallCalleeTypeInfo.realType->codeName == "@array.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@bool.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@byte.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@char.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@f32.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@f64.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@float.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@fn.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@i8.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@i16.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@i32.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@i64.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@int.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@opt.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@str.slice" ||
      exprCallCalleeTypeInfo.realType->codeName == "@u8.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@u16.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@u32.str" ||
      exprCallCalleeTypeInfo.realType->codeName == "@u64.str"
    )) {
      auto calleeExprAccess = std::get<ASTExprAccess>(*exprCall.callee.body);
      auto calleeNodeExpr = std::get<ASTNodeExpr>(calleeExprAccess.expr);
      auto calleeTypeInfo = this->_typeInfo(calleeNodeExpr.type);

      if (exprCallCalleeTypeInfo.realType->codeName == "@array.join") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.type);
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

        for (const auto &arg : exprCall.args) {
          code += ", " + this->_nodeExpr(arg.expr, arg.expr.type);
        }

        code += ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.reverse") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.type);
        this->_activateEntity(calleeTypeInfo.realTypeName + "_reverse");
        code = calleeTypeInfo.realTypeName + "_reverse(" + calleeCode + ")";
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@array.slice") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.type);
        auto arg1Expr = std::string(exprCall.args.empty() ? "0" : "1");
        auto arg2Expr = exprCall.args.empty() ? "0" : this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("i64"));
        auto arg3Expr = std::string(exprCall.args.size() < 2 ? "0" : "1");
        auto arg4Expr = exprCall.args.size() < 2 ? "0" : this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("i64"));

        this->_activateEntity(calleeTypeInfo.realTypeName + "_slice");
        code = calleeTypeInfo.realTypeName + "_slice(" + calleeCode + ", " + arg1Expr + ", " + arg2Expr + ", " + arg3Expr + ", " + arg4Expr + ")";
      } else if (
        exprCallCalleeTypeInfo.realType->codeName == "@array.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@bool.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@byte.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@char.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@f32.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@f64.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@float.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@fn.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@i8.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@i16.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@i32.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@i64.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@int.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@opt.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@u8.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@u16.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@u32.str" ||
        exprCallCalleeTypeInfo.realType->codeName == "@u64.str"
      ) {
        auto typeStrFn = std::string();

        if (
          exprCallCalleeTypeInfo.realType->codeName == "@array.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@fn.str" ||
          exprCallCalleeTypeInfo.realType->codeName == "@opt.str"
        ) {
          this->_activateEntity(calleeTypeInfo.realTypeName + "_str");
          typeStrFn = calleeTypeInfo.realTypeName + "_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@bool.str") {
          this->_activateBuiltin("fnBoolStr");
          typeStrFn = "bool_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@byte.str") {
          this->_activateBuiltin("fnByteStr");
          typeStrFn = "byte_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@char.str") {
          this->_activateBuiltin("fnCharStr");
          typeStrFn = "char_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@f32.str") {
          this->_activateBuiltin("fnF32Str");
          typeStrFn = "f32_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@f64.str") {
          this->_activateBuiltin("fnF64Str");
          typeStrFn = "f64_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@float.str") {
          this->_activateBuiltin("fnFloatStr");
          typeStrFn = "float_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@i8.str") {
          this->_activateBuiltin("fnI8Str");
          typeStrFn = "i8_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@i16.str") {
          this->_activateBuiltin("fnI16Str");
          typeStrFn = "i16_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@i32.str") {
          this->_activateBuiltin("fnI32Str");
          typeStrFn = "i32_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@i64.str") {
          this->_activateBuiltin("fnI64Str");
          typeStrFn = "i64_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@int.str") {
          this->_activateBuiltin("fnIntStr");
          typeStrFn = "int_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@u8.str") {
          this->_activateBuiltin("fnU8Str");
          typeStrFn = "u8_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@u16.str") {
          this->_activateBuiltin("fnU16Str");
          typeStrFn = "u16_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@u32.str") {
          this->_activateBuiltin("fnU32Str");
          typeStrFn = "u32_str";
        } else if (exprCallCalleeTypeInfo.realType->codeName == "@u64.str") {
          this->_activateBuiltin("fnU64Str");
          typeStrFn = "u64_str";
        }

        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.realType);

        if (!calleeNodeExpr.parenthesized) {
          calleeCode = "(" + calleeCode + ")";
        }

        code = typeStrFn + calleeCode;
      } else if (exprCallCalleeTypeInfo.realType->codeName == "@str.slice") {
        auto calleeCode = this->_nodeExpr(calleeNodeExpr, calleeTypeInfo.type);
        auto arg1Expr = std::string(exprCall.args.empty() ? "0" : "1");
        auto arg2Expr = exprCall.args.empty() ? "0" : this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("i64"));
        auto arg3Expr = std::string(exprCall.args.size() < 2 ? "0" : "1");
        auto arg4Expr = exprCall.args.size() < 2 ? "0" : this->_nodeExpr(exprCall.args[1].expr, this->ast->typeMap.get("i64"));

        this->_activateBuiltin("fnStrSlice");
        code = "str_slice(" + calleeCode + ", " + arg1Expr + ", " + arg2Expr + ", " + arg3Expr + ", " + arg4Expr + ")";
      }
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@exit") {
      auto arg1Expr = std::string("0");

      if (!exprCall.args.empty()) {
        arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("int"));
      }

      this->_activateBuiltin("libStdlib");
      code = "exit(" + arg1Expr + ")";
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@print") {
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

      code = std::string(R"(print(stdout, ")");

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

        if (
          argTypeInfo.type->isArray() ||
            argTypeInfo.type->isFn() ||
            argTypeInfo.type->isObj() ||
            argTypeInfo.type->isOpt()
          ) {
          code += "s";

          this->_activateEntity(argTypeInfo.typeName + "_str");
          argsCode += argTypeInfo.typeName + "_str(" + this->_nodeExpr(exprCallArg.expr, argTypeInfo.type) + ")";
        } else if (argTypeInfo.type->isRef()) {
          code += "p";
          argsCode += this->_nodeExpr(exprCallArg.expr, argTypeInfo.type);
        } else if (argTypeInfo.type->isStr() && exprCallArg.expr.isLit()) {
          code += "z";
          argsCode += exprCallArg.expr.litBody();
        } else {
          if (argTypeInfo.type->isBool()) code += "t";
          else if (argTypeInfo.type->isByte()) code += "b";
          else if (argTypeInfo.type->isChar()) code += "c";
          else if (argTypeInfo.type->isF32()) code += "e";
          else if (argTypeInfo.type->isF64()) code += "g";
          else if (argTypeInfo.type->isFloat()) code += "f";
          else if (argTypeInfo.type->isI8()) code += "h";
          else if (argTypeInfo.type->isI16()) code += "j";
          else if (argTypeInfo.type->isI32()) code += "k";
          else if (argTypeInfo.type->isI64()) code += "l";
          else if (argTypeInfo.type->isInt()) code += "i";
          else if (argTypeInfo.type->isStr()) code += "s";
          else if (argTypeInfo.type->isU8()) code += "v";
          else if (argTypeInfo.type->isU16()) code += "w";
          else if (argTypeInfo.type->isU32()) code += "u";
          else if (argTypeInfo.type->isU64()) code += "y";

          argsCode += this->_nodeExpr(exprCallArg.expr, argTypeInfo.type);
        }

        argsCode += ", ";
        argIdx++;
      }

      if (terminator == R"("")") {
        code += R"(", )" + argsCode.substr(0, argsCode.size() - 2);
      } else {
        code += std::string(isTerminatorLit ? "z" : "s") + R"(", )" + argsCode + terminator;
      }

      code += ")";
    } else if (exprCallCalleeTypeInfo.realType->builtin && exprCallCalleeTypeInfo.realType->codeName == "@sleep") {
      auto arg1Expr = this->_nodeExpr(exprCall.args[0].expr, this->ast->typeMap.get("u64"));

      #if defined(OS_WINDOWS)
        this->_activateBuiltin("libWindows");
        code = "Sleep(" + arg1Expr + ")";
      #else
        if (!exprCall.args[0].expr.parenthesized) {
          arg1Expr = "(" + arg1Expr + ")";
        }

        this->_activateBuiltin("libUnistd");
        code = "usleep(" + arg1Expr + " * 1000)";
      #endif
    } else if (!exprCallCalleeTypeInfo.realType->builtin) {
      auto fn = std::get<TypeFn>(exprCallCalleeTypeInfo.realType->body);
      auto paramsName = exprCallCalleeTypeInfo.realTypeName + "P";
      auto bodyCode = std::string();

      if (!fn.params.empty()) {
        bodyCode += "(struct " + paramsName + ") {";
        this->_activateEntity(paramsName);

        auto paramIdx = static_cast<std::size_t>(0);

        for (const auto &param : fn.params) {
          auto paramTypeInfo = this->_typeInfo(param.type);
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
          } else if (
            param.type->isArray() ||
            param.type->isFn()
          ) {
            this->_activateEntity(paramTypeInfo.typeName);
            bodyCode += "(struct " + paramTypeInfo.typeName + ") {}";
          } else if (param.type->isBool()) {
            this->_activateBuiltin("libStdbool");
            bodyCode += "false";
          } else if (param.type->isChar()) {
            bodyCode += R"('\0')";
          } else if (param.type->isObj() || param.type->isOpt()) {
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

      auto fnName = this->_nodeExpr(exprCall.callee, exprCallCalleeTypeInfo.realType, true);

      if (fnName.starts_with("*")) {
        fnName = "(" + fnName + ")";
      }

      code = fnName + ".f(" + fnName + ".x" + (bodyCode.empty() ? "" : ", ") + bodyCode + ")";

      if (nodeExpr.type->isRef() && !targetType->isRef()) {
        code = "*" + code;
      }
    }

    if (
      (root && nodeExpr.type->isArray()) ||
      (root && nodeExpr.type->isFn()) ||
      (root && nodeExpr.type->isObj()) ||
      (root && nodeExpr.type->isOpt())
    ) {
      auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);
      this->_activateEntity(nodeTypeInfo.typeName + "_free");
      code = nodeTypeInfo.typeName + "_free((" + nodeTypeInfo.typeCodeTrimmed + ") " + code + ")";
    } else if (root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrFree");
      code = "str_free((struct str) " + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
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

    if (
      (root && nodeExpr.type->isArray()) ||
      (root && nodeExpr.type->isFn()) ||
      (root && nodeExpr.type->isObj()) ||
      (root && nodeExpr.type->isOpt())
    ) {
      auto nodeTypeInfo = this->_typeInfo(nodeExpr.type);

      this->_activateEntity(nodeTypeInfo.typeName + "_free");
      code = nodeTypeInfo.typeName + "_free((" + nodeTypeInfo.typeCodeTrimmed + ") (" + code + "))";
    } else if (root && nodeExpr.type->isStr()) {
      this->_activateBuiltin("fnStrFree");
      code = "str_free((struct str) (" + code + "))";
    }

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
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
    auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);
    auto typeInfo = this->_typeInfo(exprObj.type);
    auto fieldsCode = std::string();

    for (const auto &typeField : exprObj.type->fields) {
      if (typeField.builtin) {
        continue;
      }

      auto fieldTypeInfo = this->_typeInfo(typeField.type);

      auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&typeField] (const auto &it) -> bool {
        return it.id == typeField.name;
      });

      fieldsCode += ", ";

      if (exprObjProp != exprObj.props.end()) {
        fieldsCode += this->_nodeExpr(exprObjProp->init, typeField.type);
      } else if (typeField.type->isArray()) {
        this->_activateEntity(fieldTypeInfo.typeName + "_alloc");
        fieldsCode += fieldTypeInfo.typeName + "_alloc(0)";
      } else if (typeField.type->isBool()) {
        this->_activateBuiltin("libStdbool");
        fieldsCode += "false";
      } else if (typeField.type->isChar()) {
        fieldsCode += R"('\0')";
      } else if (typeField.type->isOpt()) {
        this->_activateBuiltin("libStdlib");
        fieldsCode += "NULL";
      } else if (typeField.type->isStr()) {
        this->_activateBuiltin("fnStrAlloc");
        fieldsCode += R"(str_alloc(""))";
      } else if (!typeField.type->isFn() && !typeField.type->isObj() && !typeField.type->isRef()) {
        fieldsCode += "0";
      }
    }

    this->_activateEntity(typeInfo.typeName + "_alloc");
    auto code = typeInfo.typeName + "_alloc(" + fieldsCode.substr(2) + ")";

    if (root) {
      this->_activateEntity(typeInfo.typeName + "_free");
      code = typeInfo.typeName + "_free((" + typeInfo.typeCodeTrimmed + ") " + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
  } else if (std::holds_alternative<ASTExprRef>(*nodeExpr.body)) {
    auto exprRef = std::get<ASTExprRef>(*nodeExpr.body);
    auto code = std::string();

    if (targetType->isOpt()) {
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

std::string Codegen::_type (const Type *type) {
  if (type->isAny()) {
    return "void *";
  } else if (type->isArray()) {
    if (type->builtin) {
      return type->name;
    }

    auto typeName = this->_typeNameArray(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isByte()) {
    return "unsigned char ";
  } else if (type->isChar()) {
    return "char ";
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
  } else if (type->isObj()) {
    if (type->builtin) {
      return type->name;
    }

    auto typeName = Codegen::typeName(type->codeName);
    this->_activateEntity(typeName);
    return "struct " + typeName + " *";
  } else if (type->isOpt()) {
    if (type->builtin) {
      return type->name;
    }

    this->_typeNameOpt(type);
    auto typeOpt = std::get<TypeOptional>(type->body);
    return this->_type(typeOpt.type) + "*";
  } else if (type->isRef()) {
    auto typeRef = std::get<TypeRef>(type->body);
    return this->_type(typeRef.refType) + "*";
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
  } else if (type->isVoid()) {
    return "void ";
  }

  throw Error("tried generating unknown type");
}

CodegenTypeInfo Codegen::_typeInfo (Type *type) {
  auto typeCode = this->_type(type);
  auto typeCodeTrimmed = typeCode.substr(0, typeCode.find_last_not_of(' ') + 1);
  auto typeName = std::string();

  if (type->isArray() && !type->builtin) {
    typeName = this->_typeNameArray(type);
  } else if (type->isFn() && !type->builtin) {
    typeName = this->_typeNameFn(type);
  } else if (type->isObj() && !type->builtin) {
    typeName = Codegen::typeName(type->codeName);
  } else if (type->isOpt() && !type->builtin) {
    typeName = this->_typeNameOpt(type);
  } else {
    typeName = type->name;
  }

  if (!type->isRef()) {
    return CodegenTypeInfo{
      type,
      typeName,
      typeCode,
      "const " + typeCode,
      typeCodeTrimmed,
      "const " + typeCodeTrimmed,
      typeCode + "*",
      "const " + typeCode + "*",
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

  auto realType = std::get<TypeRef>(type->body).refType;
  auto realTypeCode = this->_type(realType);
  auto realTypeCodeTrimmed = realTypeCode.substr(0, realTypeCode.find_last_not_of(' ') + 1);
  auto realTypeName = std::string();

  if (realType->isArray() && !type->builtin) {
    realTypeName = this->_typeNameArray(realType);
  } else if (realType->isFn() && !type->builtin) {
    realTypeName = this->_typeNameFn(realType);
  } else if (realType->isObj() && !type->builtin) {
    realTypeName = Codegen::typeName(realType->codeName);
  } else if (realType->isOpt() && !type->builtin) {
    realTypeName = this->_typeNameOpt(realType);
  } else {
    realTypeName = realType->name;
  }

  return CodegenTypeInfo{
    type,
    typeName,
    typeCode,
    "const " + typeCode,
    typeCodeTrimmed,
    "const " + typeCodeTrimmed,
    typeCode + "*",
    "const " + typeCode + "*",
    realType,
    realTypeName,
    realTypeCode,
    "const " + realTypeCode,
    realTypeCodeTrimmed,
    "const " + realTypeCodeTrimmed,
    realTypeCode + "*",
    "const" + realTypeCode + "*"
  };
}

std::string Codegen::_typeNameArray (const Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto elementTypeInfo = this->_typeInfo(std::get<TypeArray>(type->body).elementType);
  auto varArgTypeCode = elementTypeInfo.type->isSmallForVarArg()
    ? elementTypeInfo.type->isF32() ? "double" : "int"
    : elementTypeInfo.typeCodeTrimmed;
  auto elementRealTypeCode = std::string(elementTypeInfo.type->isRef() ? "*" : "") + "n.d[i]";

  auto entity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ, { "libStdlib" }};
  entity.decl += "struct " + typeName + ";";
  entity.def += "struct " + typeName + " {" EOL;
  entity.def += "  " + elementTypeInfo.typeRefCode + "d;" EOL;
  entity.def += "  size_t l;" EOL;
  entity.def += "};";

  auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdarg", "libStdlib" }, { typeName }};
  allocFnEntity.decl += "struct " + typeName + " " + typeName + "_alloc (size_t, ...);";
  allocFnEntity.def += "struct " + typeName + " " + typeName + "_alloc (size_t x, ...) {" EOL;
  allocFnEntity.def += "  if (x == 0) return (struct " + typeName + ") {NULL, 0};" EOL;
  allocFnEntity.def += "  " + elementTypeInfo.typeRefCode + "d = alloc(x * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
  allocFnEntity.def += "  va_list args;" EOL;
  allocFnEntity.def += "  va_start(args, x);" EOL;
  allocFnEntity.def += "  for (size_t i = 0; i < x; i++) d[i] = va_arg(args, " + varArgTypeCode + ");" EOL;
  allocFnEntity.def += "  va_end(args);" EOL;
  allocFnEntity.def += "  return (struct " + typeName + ") {d, x};" EOL;
  allocFnEntity.def += "}";

  auto atFnEntity = CodegenEntity{typeName + "_at", CODEGEN_ENTITY_FN, { "libInttypes", "libStdio", "libStdlib" }, { typeName }};
  atFnEntity.decl += elementTypeInfo.typeRefCode + typeName + "_at (struct " + typeName + ", int64_t);";
  atFnEntity.def += elementTypeInfo.typeRefCode + typeName + "_at (struct " + typeName + " n, int64_t i) {" EOL;
  atFnEntity.def += "  if ((i >= 0 && i >= n.l) || (i < 0 && i < -n.l)) {" EOL;
  atFnEntity.def += R"(    fprintf(stderr, "Error: index %" PRId64 " out of array bounds)" ESC_EOL R"(", i);)" EOL;
  atFnEntity.def += "    exit(EXIT_FAILURE);" EOL;
  atFnEntity.def += "  }" EOL;
  atFnEntity.def += "  return i < 0 ? &n.d[n.l + i] : &n.d[i];" EOL;
  atFnEntity.def += "}";

  auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdlib" }, { typeName }};
  copyFnEntity.decl += "struct " + typeName + " " + typeName + "_copy (const struct " + typeName + ");";
  copyFnEntity.def += "struct " + typeName + " " + typeName + "_copy (const struct " + typeName + " n) {" EOL;
  copyFnEntity.def += "  if (n.l == 0) return (struct " + typeName + ") {NULL, 0};" EOL;
  copyFnEntity.def += "  " + elementTypeInfo.typeRefCode + "d = alloc(n.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
  copyFnEntity.def += "  for (size_t i = 0; i < n.l; i++) d[i] = ";

  if (
    elementTypeInfo.type->isArray() ||
    elementTypeInfo.type->isFn() ||
    elementTypeInfo.type->isObj() ||
    elementTypeInfo.type->isOpt()
  ) {
    this->_activateEntity(elementTypeInfo.typeName + "_copy", &copyFnEntity.entities);
    copyFnEntity.def += elementTypeInfo.typeName + "_copy(n.d[i]);" EOL;
  } else if (elementTypeInfo.type->isStr()) {
    this->_activateBuiltin("fnStrCopy", &copyFnEntity.builtins);
    copyFnEntity.def += "str_copy(n.d[i]);" EOL;
  } else {
    copyFnEntity.def += "n.d[i];" EOL;
  }

  copyFnEntity.def += "  return (struct " + typeName + ") {d, n.l};" EOL;
  copyFnEntity.def += "}";

  auto eqFnEntity = CodegenEntity{typeName + "_eq", CODEGEN_ENTITY_FN, { "libStdbool", "libStdlib" }, { typeName, typeName + "_free" }};
  eqFnEntity.decl += "bool " + typeName + "_eq (struct " + typeName + ", struct " + typeName + ");";
  eqFnEntity.def += "bool " + typeName + "_eq (struct " + typeName + " n1, struct " + typeName + " n2) {" EOL;
  eqFnEntity.def += "  bool r = n1.l == n2.l;" EOL;
  eqFnEntity.def += "  if (r) {" EOL;
  eqFnEntity.def += "    for (size_t i = 0; i < n1.l; i++) {" EOL;
  eqFnEntity.def += "      if (" + this->_genEqFn(elementTypeInfo, "n1.d[i]", "n2.d[i]", &eqFnEntity.builtins, &eqFnEntity.entities, true) + ") {" EOL;
  eqFnEntity.def += "        r = false;" EOL;
  eqFnEntity.def += "        break;" EOL;
  eqFnEntity.def += "      }" EOL;
  eqFnEntity.def += "    }" EOL;
  eqFnEntity.def += "  }" EOL;
  eqFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n1);" EOL;
  eqFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n2);" EOL;
  eqFnEntity.def += "  return r;" EOL;
  eqFnEntity.def += "}";

  auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
  freeFnEntity.decl += "void " + typeName + "_free (struct " + typeName + ");";
  freeFnEntity.def += "void " + typeName + "_free (struct " + typeName + " n) {" EOL;

  if (
    elementTypeInfo.type->isArray() ||
    elementTypeInfo.type->isFn() ||
    elementTypeInfo.type->isObj() ||
    elementTypeInfo.type->isOpt()
  ) {
    this->_activateEntity(elementTypeInfo.typeName + "_free", &freeFnEntity.entities);
    freeFnEntity.def += "  for (size_t i = 0; i < n.l; i++) " + elementTypeInfo.typeName + "_free((" + elementTypeInfo.typeCodeTrimmed + ") n.d[i]);" EOL;
  } else if (elementTypeInfo.type->isStr()) {
    this->_activateBuiltin("fnStrFree", &freeFnEntity.builtins);
    freeFnEntity.def += "  for (size_t i = 0; i < n.l; i++) str_free((struct str) n.d[i]);" EOL;
  }

  freeFnEntity.def += "  free(n.d);" EOL;
  freeFnEntity.def += "}";

  auto joinFnEntity = CodegenEntity{typeName + "_join", CODEGEN_ENTITY_FN, {
    "fnStrAlloc",
    "fnStrConcatStr",
    "fnStrCopy",
    "fnStrFree",
    "typeStr"
  }, { typeName, typeName + "_free" }};

  joinFnEntity.decl += "struct str " + typeName + "_join (struct " + typeName + ", unsigned char, struct str);";
  joinFnEntity.def += "struct str " + typeName + "_join (struct " + typeName + " n, unsigned char o1, struct str n1) {" EOL;
  joinFnEntity.def += R"(  struct str x = o1 == 0 ? str_alloc(",") : n1;)" EOL;
  joinFnEntity.def += R"(  struct str r = str_alloc("");)" EOL;
  joinFnEntity.def += "  for (size_t i = 0; i < n.l; i++) {" EOL;
  joinFnEntity.def += "    if (i != 0) r = str_concat_str(r, str_copy(x));" EOL;

  if (
    elementTypeInfo.realType->isArray() ||
    elementTypeInfo.realType->isFn() ||
    elementTypeInfo.realType->isObj() ||
    elementTypeInfo.realType->isOpt()
  ) {
    this->_activateEntity(elementTypeInfo.realTypeName + "_copy", &joinFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName + "_str", &joinFnEntity.entities);

    joinFnEntity.def += "    r = str_concat_str(r, " + elementTypeInfo.realTypeName + "_str(" + elementTypeInfo.realTypeName + "_copy(" + elementRealTypeCode + ")));" EOL;
  } else if (elementTypeInfo.realType->isStr()) {
    joinFnEntity.def += "    r = str_concat_str(r, str_copy(" + elementRealTypeCode + "));" EOL;
  } else {
    auto elementTypeBuiltinName = Token::upperFirst(elementTypeInfo.realTypeName);
    this->_activateBuiltin("fn" + elementTypeBuiltinName + "Str", &joinFnEntity.builtins);

    joinFnEntity.def += "    r = str_concat_str(r, " + elementTypeInfo.realTypeName + "_str(" + elementRealTypeCode + "));" EOL;
  }

  joinFnEntity.def += "  }" EOL;
  joinFnEntity.def += "  str_free((struct str) x);" EOL;
  joinFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n);" EOL;
  joinFnEntity.def += R"(  return r;)" EOL;
  joinFnEntity.def += "}";

  auto lenFnEntity = CodegenEntity{typeName + "_len", CODEGEN_ENTITY_FN, {}, { typeName, typeName + "_free" }};
  lenFnEntity.decl += "size_t " + typeName + "_len (struct " + typeName + ");";
  lenFnEntity.def += "size_t " + typeName + "_len (struct " + typeName + " n) {" EOL;
  lenFnEntity.def += "  size_t l = n.l;" EOL;
  lenFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n);" EOL;
  lenFnEntity.def += "  return l;" EOL;
  lenFnEntity.def += "}";

  auto neFnEntity = CodegenEntity{typeName + "_ne", CODEGEN_ENTITY_FN, { "libStdbool", "libStdlib" }, { typeName, typeName + "_free" }};
  neFnEntity.decl += "bool " + typeName + "_ne (struct " + typeName + ", struct " + typeName + ");";
  neFnEntity.def += "bool " + typeName + "_ne (struct " + typeName + " n1, struct " + typeName + " n2) {" EOL;
  neFnEntity.def += "  bool r = n1.l != n2.l;" EOL;
  neFnEntity.def += "  if (!r) {" EOL;
  neFnEntity.def += "    r = false;" EOL;
  neFnEntity.def += "    for (size_t i = 0; i < n1.l; i++) {" EOL;
  neFnEntity.def += "      if (" + this->_genEqFn(elementTypeInfo, "n1.d[i]", "n2.d[i]", &neFnEntity.builtins, &neFnEntity.entities, true) + ") {" EOL;
  neFnEntity.def += "        r = true;" EOL;
  neFnEntity.def += "        break;" EOL;
  neFnEntity.def += "      }" EOL;
  neFnEntity.def += "    }" EOL;
  neFnEntity.def += "  }" EOL;
  neFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n1);" EOL;
  neFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n2);" EOL;
  neFnEntity.def += "  return r;" EOL;
  neFnEntity.def += "}";

  auto popFnEntity = CodegenEntity{typeName + "_pop", CODEGEN_ENTITY_FN, {}, { typeName }};
  popFnEntity.decl += elementTypeInfo.typeCode + typeName + "_pop (struct " + typeName + " *);";
  popFnEntity.def += elementTypeInfo.typeCode + typeName + "_pop (struct " + typeName + " *n) {" EOL;
  popFnEntity.def += "  n->l--;" EOL;
  popFnEntity.def += "  return n->d[n->l];" EOL;
  popFnEntity.def += "}";

  auto pushFnEntity = CodegenEntity{typeName + "_push", CODEGEN_ENTITY_FN, { "libStdarg", "libStdlib" }, { typeName }};
  pushFnEntity.decl += "void " + typeName + "_push (struct " + typeName + " *, size_t, ...);";
  pushFnEntity.def += "void " + typeName + "_push (struct " + typeName + " *n, size_t x, ...) {" EOL;
  pushFnEntity.def += "  if (x == 0) return;" EOL;
  pushFnEntity.def += "  n->l += x;" EOL;
  pushFnEntity.def += "  n->d = realloc(n->d, n->l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
  pushFnEntity.def += "  va_list args;" EOL;
  pushFnEntity.def += "  va_start(args, x);" EOL;
  pushFnEntity.def += "  for (size_t i = n->l - x; i < n->l; i++) n->d[i] = va_arg(args, " + varArgTypeCode + ");" EOL;
  pushFnEntity.def += "  va_end(args);" EOL;
  pushFnEntity.def += "}";

  auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, {}, { typeName, typeName + "_free" }};
  reallocFnEntity.decl += "struct " + typeName + " " + typeName + "_realloc (struct " + typeName + ", struct " + typeName + ");";
  reallocFnEntity.def += "struct " + typeName + " " + typeName + "_realloc (struct " + typeName + " n1, struct " + typeName + " n2) {" EOL;
  reallocFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n1);" EOL;
  reallocFnEntity.def += "  return n2;" EOL;
  reallocFnEntity.def += "}";

  auto reverseFnEntity = CodegenEntity{typeName + "_reverse", CODEGEN_ENTITY_FN, { "fnAlloc" }, { typeName, typeName + "_free" }};
  reverseFnEntity.decl += "struct " + typeName + " " + typeName + "_reverse (struct " + typeName + ");";
  reverseFnEntity.def += "struct " + typeName + " " + typeName + "_reverse (struct " + typeName + " n) {" EOL;
  reverseFnEntity.def += "  if (n.l == 0) {" EOL;
  reverseFnEntity.def += "    " + typeName + "_free((struct " + typeName + ") n);" EOL;
  reverseFnEntity.def += "    return (struct " + typeName + ") {NULL, 0};" EOL;
  reverseFnEntity.def += "  }" EOL;
  reverseFnEntity.def += "  " + elementTypeInfo.typeRefCode + "d = alloc(n.l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
  reverseFnEntity.def += "  for (size_t i = 0; i < n.l; i++) d[i] = n.d[n.l - i - 1];" EOL;
  reverseFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n);" EOL;
  reverseFnEntity.def += "  return (struct " + typeName + ") {d, n.l};" EOL;
  reverseFnEntity.def += "}";

  auto sliceFnEntity = CodegenEntity{typeName + "_slice", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdint", "libStdlib" }, {
    typeName,
    typeName + "_free"
  }};

  sliceFnEntity.decl += "struct " + typeName + " " + typeName + "_slice (struct " + typeName + ", unsigned int, int64_t, unsigned int, int64_t);";
  sliceFnEntity.def += "struct " + typeName + " " + typeName + "_slice (struct " + typeName + " n, unsigned int o1, int64_t n1, unsigned int o2, int64_t n2) {" EOL;
  sliceFnEntity.def += "  int64_t i1 = o1 == 0 ? 0 : (n1 < 0 ? (n1 < -n.l ? 0 : n1 + n.l) : (n1 > n.l ? n.l : n1));" EOL;
  sliceFnEntity.def += "  int64_t i2 = o2 == 0 ? n.l : (n2 < 0 ? (n2 < -n.l ? 0 : n2 + n.l) : (n2 > n.l ? n.l : n2));" EOL;
  sliceFnEntity.def += "  if (i1 > i2 || i1 >= n.l) {" EOL;
  sliceFnEntity.def += "    " + typeName + "_free((struct " + typeName + ") n);" EOL;
  sliceFnEntity.def += "    return (struct " + typeName + ") {NULL, 0};" EOL;
  sliceFnEntity.def += "  }" EOL;
  sliceFnEntity.def += "  size_t l = i2 - i1;" EOL;
  sliceFnEntity.def += "  " + elementTypeInfo.typeRefCode + "d = alloc(l * sizeof(" + elementTypeInfo.typeCodeTrimmed + "));" EOL;
  sliceFnEntity.def += "  for (size_t i = 0; i1 < i2; i1++) d[i++] = n.d[i1];" EOL;
  sliceFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n);" EOL;
  sliceFnEntity.def += "  return (struct " + typeName + ") {d, l};" EOL;
  sliceFnEntity.def += "}";

  auto strFnEntity = CodegenEntity{typeName + "_str", CODEGEN_ENTITY_FN, {
    "fnStrAlloc",
    "fnStrConcatCstr",
    "fnStrConcatStr",
    "typeStr"
  }, { typeName, typeName + "_free" }};

  strFnEntity.decl += "struct str " + typeName + "_str (struct " + typeName + ");";
  strFnEntity.def += "struct str " + typeName + "_str (struct " + typeName + " n) {" EOL;
  strFnEntity.def += R"(  struct str r = str_alloc("[");)" EOL;
  strFnEntity.def += "  for (size_t i = 0; i < n.l; i++) {" EOL;
  strFnEntity.def += R"(    if (i != 0) r = str_concat_cstr(r, ", ");)" EOL;

  if (
    elementTypeInfo.realType->isArray() ||
    elementTypeInfo.realType->isFn() ||
    elementTypeInfo.realType->isObj() ||
    elementTypeInfo.realType->isOpt()
  ) {
    this->_activateEntity(elementTypeInfo.realTypeName + "_copy", &strFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName + "_str", &strFnEntity.entities);

    strFnEntity.def += "    r = str_concat_str(r, " + elementTypeInfo.realTypeName + "_str(" + elementTypeInfo.realTypeName + "_copy(" + elementRealTypeCode + ")));" EOL;
  } else if (elementTypeInfo.realType->isStr()) {
    this->_activateBuiltin("fnStrEscape", &strFnEntity.builtins);

    strFnEntity.def += R"(    r = str_concat_cstr(r, "\"");)" EOL;
    strFnEntity.def += "    r = str_concat_str(r, str_escape(" + elementRealTypeCode + "));" EOL;
    strFnEntity.def += R"(    r = str_concat_cstr(r, "\"");)" EOL;
  } else {
    auto elementTypeBuiltinName = Token::upperFirst(elementTypeInfo.realTypeName);
    this->_activateBuiltin("fn" + elementTypeBuiltinName + "Str", &strFnEntity.builtins);

    strFnEntity.def += "    r = str_concat_str(r, " + elementTypeInfo.realTypeName + "_str(" + elementRealTypeCode + "));" EOL;
  }

  strFnEntity.def += "  }" EOL;
  strFnEntity.def += "  " + typeName + "_free((struct " + typeName + ") n);" EOL;
  strFnEntity.def += R"(  return str_concat_cstr(r, "]");)" EOL;
  strFnEntity.def += "}";

  if (!elementTypeInfo.realType->builtin) {
    this->_activateEntity(elementTypeInfo.realTypeName, &entity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &allocFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &atFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &copyFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &popFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &pushFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &reverseFnEntity.entities);
    this->_activateEntity(elementTypeInfo.realTypeName, &sliceFnEntity.entities);
  }

  this->entities.push_back(entity);
  this->entities.push_back(allocFnEntity);
  this->entities.push_back(atFnEntity);
  this->entities.push_back(copyFnEntity);
  this->entities.push_back(eqFnEntity);
  this->entities.push_back(freeFnEntity);
  this->entities.push_back(joinFnEntity);
  this->entities.push_back(lenFnEntity);
  this->entities.push_back(neFnEntity);
  this->entities.push_back(popFnEntity);
  this->entities.push_back(pushFnEntity);
  this->entities.push_back(reallocFnEntity);
  this->entities.push_back(reverseFnEntity);
  this->entities.push_back(sliceFnEntity);
  this->entities.push_back(strFnEntity);

  return typeName;
}

std::string Codegen::_typeNameFn (const Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto fn = std::get<TypeFn>(type->body);
  auto paramsName = Codegen::typeName(type->name + "P");
  auto returnTypeInfo = this->_typeInfo(fn.returnType);
  auto entity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ, { "libStdlib" }};

  if (!fn.params.empty()) {
    auto paramsEntity = CodegenEntity{paramsName, CODEGEN_ENTITY_OBJ};
    auto paramIdx = static_cast<std::size_t>(0);

    paramsEntity.decl += "struct " + paramsName + ";";
    paramsEntity.def += "struct " + paramsName + " {" EOL;

    for (const auto &param : fn.params) {
      auto paramTypeInfo = this->_typeInfo(param.type);
      auto paramIdxStr = std::to_string(paramIdx);

      if (!param.required) {
        paramsEntity.def += "  unsigned char o" + paramIdxStr + ";" EOL;
      }

      paramsEntity.def += "  " + paramTypeInfo.typeCode + "n" + paramIdxStr + ";" EOL;
      paramIdx++;
    }

    paramsEntity.def += "};";
    this->entities.push_back(paramsEntity);
    this->_activateEntity(paramsName, &entity.entities);
  }

  entity.decl += "struct " + typeName + ";";
  entity.def += "struct " + typeName + " {" EOL;
  entity.def += "  " + returnTypeInfo.typeCode + "(*f) ";
  entity.def += "(void *" + (fn.params.empty() ? "" : ", struct " + paramsName) + ");" EOL;
  entity.def += "  void *x;" EOL;
  entity.def += "  size_t l;" EOL;
  entity.def += "};";

  auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdlib", "libString" }, { typeName }};
  copyFnEntity.decl += "struct " + typeName + " " + typeName + "_copy (const struct " + typeName + ");";
  copyFnEntity.def += "struct " + typeName + " " + typeName + "_copy (const struct " + typeName + " n) {" EOL;
  copyFnEntity.def += "  if (n.x == NULL) return n;" EOL;
  copyFnEntity.def += "  void *x = alloc(n.l);" EOL;
  copyFnEntity.def += "  memcpy(x, n.x, n.l);" EOL;
  copyFnEntity.def += "  return (struct " + typeName + ") {n.f, x, n.l};" EOL;
  copyFnEntity.def += "}";

  auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
  freeFnEntity.decl += "void " + typeName + "_free (struct " + typeName + ");";
  freeFnEntity.def += "void " + typeName + "_free (struct " + typeName + " n) {" EOL;
  freeFnEntity.def += "  free(n.x);" EOL;
  freeFnEntity.def += "}";

  auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, { "libStdlib" }, { typeName }};
  reallocFnEntity.decl += "struct " + typeName + " " + typeName + "_realloc (struct " + typeName + ", struct " + typeName + ");";
  reallocFnEntity.def += "struct " + typeName + " " + typeName + "_realloc (struct " + typeName + " n1, struct " + typeName + " n2) {" EOL;
  reallocFnEntity.def += "  if (n1.x != NULL) free(n1.x);" EOL;
  reallocFnEntity.def += "  return n2;" EOL;
  reallocFnEntity.def += "}";

  auto strFnEntity = CodegenEntity{typeName + "_str", CODEGEN_ENTITY_FN, { "fnStrAlloc", "typeStr" }, { typeName }};
  strFnEntity.decl += "struct str " + typeName + "_str (struct " + typeName + ");";
  strFnEntity.def += "struct str " + typeName + "_str (struct " + typeName + " n) {" EOL;
  strFnEntity.def += R"(  return str_alloc("[Function]");)" EOL;
  strFnEntity.def += "}";

  this->entities.push_back(entity);
  this->entities.push_back(copyFnEntity);
  this->entities.push_back(freeFnEntity);
  this->entities.push_back(reallocFnEntity);
  this->entities.push_back(strFnEntity);

  return typeName;
}

std::string Codegen::_typeNameOpt (const Type *type) {
  auto typeName = Codegen::typeName(type->name);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  auto underlyingTypeInfo = this->_typeInfo(std::get<TypeOptional>(type->body).type);
  auto underlyingRealTypeCode = std::string(underlyingTypeInfo.type->isRef() ? "*" : "") + "*n";

  auto entity = CodegenEntity{typeName, CODEGEN_ENTITY_OBJ, {}};

  auto allocFnEntity = CodegenEntity{typeName + "_alloc", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdlib" }, {}};
  allocFnEntity.decl += underlyingTypeInfo.typeRefCode + typeName + "_alloc (" + underlyingTypeInfo.typeCodeTrimmed + ");";
  allocFnEntity.def += underlyingTypeInfo.typeRefCode + typeName + "_alloc (" + underlyingTypeInfo.typeCode + "n) {" EOL;
  allocFnEntity.def += "  " + underlyingTypeInfo.typeRefCode + "r = alloc(sizeof(" + underlyingTypeInfo.typeCodeTrimmed + "));" EOL;
  allocFnEntity.def += "  *r = n;" EOL;
  allocFnEntity.def += "  return r;" EOL;
  allocFnEntity.def += "}";

  auto copyFnEntity = CodegenEntity{typeName + "_copy", CODEGEN_ENTITY_FN, { "fnAlloc", "libStdlib" }, {}};
  copyFnEntity.decl += underlyingTypeInfo.typeRefCode + typeName + "_copy (const " + underlyingTypeInfo.typeRefCode + ");";
  copyFnEntity.def += underlyingTypeInfo.typeRefCode + typeName + "_copy (const " + underlyingTypeInfo.typeRefCode + "n) {" EOL;
  copyFnEntity.def += "  if (n == NULL) return NULL;" EOL;
  copyFnEntity.def += "  " + underlyingTypeInfo.typeRefCode + "r = alloc(sizeof(" + underlyingTypeInfo.typeCodeTrimmed + "));" EOL;
  copyFnEntity.def += "  *r = ";

  if (
    underlyingTypeInfo.type->isArray() ||
    underlyingTypeInfo.type->isFn() ||
    underlyingTypeInfo.type->isObj() ||
    underlyingTypeInfo.type->isOpt()
  ) {
    this->_activateEntity(underlyingTypeInfo.typeName + "_copy", &copyFnEntity.entities);
    copyFnEntity.def += underlyingTypeInfo.typeName + "_copy(*n);" EOL;
  } else if (underlyingTypeInfo.type->isStr()) {
    this->_activateBuiltin("fnStrCopy", &copyFnEntity.builtins);
    copyFnEntity.def += "str_copy(*n);" EOL;
  } else {
    copyFnEntity.def += "*n;" EOL;
  }

  copyFnEntity.def += "  return r;" EOL;
  copyFnEntity.def += "}";

  auto eqFnEntity = CodegenEntity{typeName + "_eq", CODEGEN_ENTITY_FN, { "libStdbool", "libStdlib" }, { typeName + "_free" }};
  eqFnEntity.decl += "bool " + typeName + "_eq (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
  eqFnEntity.def += "bool " + typeName + "_eq (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
  eqFnEntity.def += "  bool r = (n1 == NULL || n2 == NULL) ? n1 == NULL && n2 == NULL : ";
  eqFnEntity.def += this->_genEqFn(underlyingTypeInfo, "*n1", "*n2", &eqFnEntity.builtins, &eqFnEntity.entities) + ";" EOL;
  eqFnEntity.def += "  " + typeName + "_free((" + underlyingTypeInfo.typeRefCode + ") n1);" EOL;
  eqFnEntity.def += "  " + typeName + "_free((" + underlyingTypeInfo.typeRefCode + ") n2);" EOL;
  eqFnEntity.def += "  return r;" EOL;
  eqFnEntity.def += "}";

  auto freeFnEntity = CodegenEntity{typeName + "_free", CODEGEN_ENTITY_FN, { "libStdlib" }, {}};
  freeFnEntity.decl += "void " + typeName + "_free (" + underlyingTypeInfo.typeRefCode + ");";
  freeFnEntity.def += "void " + typeName + "_free (" + underlyingTypeInfo.typeRefCode + "n) {" EOL;
  freeFnEntity.def += "  if (n == NULL) return;" EOL;

  if (
    underlyingTypeInfo.type->isArray() ||
    underlyingTypeInfo.type->isFn() ||
    underlyingTypeInfo.type->isObj() ||
    underlyingTypeInfo.type->isOpt()
  ) {
    this->_activateEntity(underlyingTypeInfo.typeName + "_free", &freeFnEntity.entities);
    freeFnEntity.def += "  " + underlyingTypeInfo.typeName + "_free(*n);" EOL;
  } else if (underlyingTypeInfo.type->isStr()) {
    this->_activateBuiltin("fnStrFree", &freeFnEntity.builtins);
    freeFnEntity.def += "  str_free((struct str) *n);" EOL;
  }

  freeFnEntity.def += "  free(n);" EOL;
  freeFnEntity.def += "}";

  auto neFnEntity = CodegenEntity{typeName + "_ne", CODEGEN_ENTITY_FN, { "libStdbool", "libStdlib" }, { typeName + "_free" }};
  neFnEntity.decl += "bool " + typeName + "_ne (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
  neFnEntity.def += "bool " + typeName + "_ne (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
  neFnEntity.def += "  bool r = (n1 == NULL || n2 == NULL) ? n1 != NULL || n2 != NULL : ";
  neFnEntity.def += this->_genEqFn(underlyingTypeInfo, "*n1", "*n2", &neFnEntity.builtins, &neFnEntity.entities, true) + ";" EOL;
  neFnEntity.def += "  " + typeName + "_free((" + underlyingTypeInfo.typeRefCode + ") n1);" EOL;
  neFnEntity.def += "  " + typeName + "_free((" + underlyingTypeInfo.typeRefCode + ") n2);" EOL;
  neFnEntity.def += "  return r;" EOL;
  neFnEntity.def += "}";

  auto reallocFnEntity = CodegenEntity{typeName + "_realloc", CODEGEN_ENTITY_FN, {}, { typeName + "_free" }};
  reallocFnEntity.decl += underlyingTypeInfo.typeRefCode + typeName + "_realloc (" + underlyingTypeInfo.typeRefCode + ", " + underlyingTypeInfo.typeRefCode + ");";
  reallocFnEntity.def += underlyingTypeInfo.typeRefCode + typeName + "_realloc (" + underlyingTypeInfo.typeRefCode + "n1, " + underlyingTypeInfo.typeRefCode + "n2) {" EOL;
  reallocFnEntity.def += "  " + typeName + "_free((" + underlyingTypeInfo.typeRefCode + ") n1);" EOL;
  reallocFnEntity.def += "  return n2;" EOL;
  reallocFnEntity.def += "}";

  auto strFnEntity = CodegenEntity{typeName + "_str", CODEGEN_ENTITY_FN, {
    "libStdlib",
    "fnStrAlloc",
    "fnStrConcatCstr",
    "fnStrConcatStr",
    "typeStr"
  }, { typeName + "_free" }};

  strFnEntity.decl += "struct str " + typeName + "_str (" + underlyingTypeInfo.typeRefCode + ");";
  strFnEntity.def += "struct str " + typeName + "_str (" + underlyingTypeInfo.typeRefCode + "n) {" EOL;
  strFnEntity.def += R"(  if (n == NULL) return str_alloc("nil");)" EOL;

  if (
    underlyingTypeInfo.realType->isArray() ||
    underlyingTypeInfo.realType->isFn() ||
    underlyingTypeInfo.realType->isObj() ||
    underlyingTypeInfo.realType->isOpt()
  ) {
    this->_activateEntity(underlyingTypeInfo.realTypeName + "_copy", &strFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName + "_str", &strFnEntity.entities);

    strFnEntity.def += "  struct str r = " + underlyingTypeInfo.realTypeName + "_str(" + underlyingTypeInfo.realTypeName + "_copy(" + underlyingRealTypeCode + "));" EOL;
  } else if (underlyingTypeInfo.realType->isStr()) {
    this->_activateBuiltin("fnStrEscape", &strFnEntity.builtins);

    strFnEntity.def += R"(  struct str r = str_alloc("\"");)" EOL;
    strFnEntity.def += "  r = str_concat_str(r, str_escape(" + underlyingRealTypeCode + "));" EOL;
    strFnEntity.def += R"(  r = str_concat_cstr(r, "\"");)" EOL;
  } else {
    auto elementTypeBuiltinName = Token::upperFirst(underlyingTypeInfo.realTypeName);
    this->_activateBuiltin("fn" + elementTypeBuiltinName + "Str", &strFnEntity.builtins);
    strFnEntity.def += "  struct str r = " + underlyingTypeInfo.realTypeName + "_str(" + underlyingRealTypeCode + ");" EOL;
  }

  strFnEntity.def += "  " + typeName + "_free((" + underlyingTypeInfo.typeRefCode + ") n);" EOL;
  strFnEntity.def += "  return r;" EOL;
  strFnEntity.def += "}";

  if (!underlyingTypeInfo.realType->builtin) {
    this->_activateEntity(underlyingTypeInfo.realTypeName, &entity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &allocFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &copyFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &eqFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &freeFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &neFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &reallocFnEntity.entities);
    this->_activateEntity(underlyingTypeInfo.realTypeName, &strFnEntity.entities);
  }

  this->entities.push_back(entity);
  this->entities.push_back(allocFnEntity);
  this->entities.push_back(copyFnEntity);
  this->entities.push_back(eqFnEntity);
  this->entities.push_back(freeFnEntity);
  this->entities.push_back(neFnEntity);
  this->entities.push_back(reallocFnEntity);
  this->entities.push_back(strFnEntity);

  return typeName;
}

std::string Codegen::_wrapNode ([[maybe_unused]] const ASTNode &node, const std::string &code) {
  return code;
}

std::string Codegen::_wrapNodeExpr (const ASTNodeExpr &nodeExpr, Type *targetType, bool root, const std::string &code) {
  auto result = code;

  if (!root && Type::real(targetType)->isOpt() && !Type::real(nodeExpr.type)->isOpt()) {
    auto targetTypeInfo = this->_typeInfo(Type::real(targetType));
    this->_activateEntity(targetTypeInfo.typeName + "_alloc");
    result = targetTypeInfo.typeName + "_alloc(" + code + ")";
  }

  if (nodeExpr.parenthesized) {
    result = "(" + result + ")";
  }

  return result;
}
