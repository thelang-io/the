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

#ifndef SRC_CODEGEN_API_HPP
#define SRC_CODEGEN_API_HPP

#include <map>
#include "CodegenAPIItem.hpp"
#include "config.hpp"

extern inline const std::map<std::string, CodegenAPIItem> codegenAPI = {
  {"alloc", {
    false,
    "alloc",
    "void *alloc (size_t);" EOL,
    R"(void *alloc (size_t n1) {)" EOL
    R"(  void *r = malloc(n1);)" EOL
    R"(  if (r == NULL) error_alloc(&err_state, n1);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"error_alloc", "libStddef", "libStdlib", "varErrState"},
    {},
    false
  }},

  {"any_copy", {
    false,
    "any_copy",
    "struct any any_copy (const struct any);" EOL,
    R"(struct any any_copy (const struct any n) {)" EOL
    R"(  return n.d == NULL ? n : n._copy(n);)" EOL
    R"(})" EOL,
    {"libStddef", "typeAny"},
    {},
    false
  }},

  {"any_free", {
    false,
    "any_free",
    "void any_free (struct any);" EOL,
    R"(void any_free (struct any n) {)" EOL
    R"(  if (n.d != NULL) n._free(n);)" EOL
    R"(})" EOL,
    {"libStddef", "typeAny"},
    {},
    false
  }},

  {"any_realloc", {
    false,
    "any_realloc",
    "struct any any_realloc (struct any, struct any);" EOL,
    R"(struct any any_realloc (struct any n1, struct any n2) {)" EOL
    R"(  if (n1.d != NULL) n1._free(n1);)" EOL
    R"(  return n2;)" EOL
    R"(})" EOL,
    {"libStddef", "typeAny"},
    {},
    false
  }},

  {"any_str", {
    false,
    "any_str",
    "struct str any_str (struct any);" EOL,
    R"(struct str any_str (struct any n) {)" EOL
    R"(  if (n.d != NULL) n._free(n);)" EOL
    R"(  return str_alloc("any");)" EOL
    R"(})" EOL,
    {"libStddef", "str_alloc", "typeAny", "typeStr"},
    {},
    false
  }},

  {"bool_str", {
    false,
    "bool_str",
    "struct str bool_str (bool);" EOL,
    R"(struct str bool_str (bool t) {)" EOL
    R"(  return str_alloc(t ? "true" : "false");)" EOL
    R"(})" EOL,
    {"libStdbool", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"buffer_copy", {
    false,
    "buffer_copy",
    "struct buffer buffer_copy (const struct buffer);" EOL,
    R"(struct buffer buffer_copy (const struct buffer o) {)" EOL
    R"(  unsigned char *d = alloc(o.l);)" EOL
    R"(  memcpy(d, o.d, o.l);)" EOL
    R"(  return (struct buffer) {d, o.l};)" EOL
    R"(})" EOL,
    {"alloc", "libString", "typeBuffer"},
    {},
    false
  }},

  {"buffer_eq", {
    false,
    "buffer_eq",
    "bool buffer_eq (struct buffer, struct buffer);" EOL,
    R"(bool buffer_eq (struct buffer o1, struct buffer o2) {)" EOL
    R"(  bool r = o1.l == o2.l && memcmp(o1.d, o2.d, o1.l) == 0;)" EOL
    R"(  free(o1.d);)" EOL
    R"(  free(o2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeBuffer"},
    {},
    false
  }},

  {"buffer_free", {
    false,
    "buffer_free",
    "void buffer_free (struct buffer);" EOL,
    R"(void buffer_free (struct buffer o) {)" EOL
    R"(  free(o.d);)" EOL
    R"(})" EOL,
    {"libStdlib", "typeBuffer"},
    {},
    false
  }},

  {"buffer_ne", {
    false,
    "buffer_ne",
    "bool buffer_ne (struct buffer, struct buffer);" EOL,
    R"(bool buffer_ne (struct buffer o1, struct buffer o2) {)" EOL
    R"(  bool r = o1.l != o2.l || memcmp(o1.d, o2.d, o1.l) != 0;)" EOL
    R"(  free(o1.d);)" EOL
    R"(  free(o2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeBuffer"},
    {},
    false
  }},

  {"buffer_realloc", {
    false,
    "buffer_realloc",
    "struct buffer buffer_realloc (struct buffer, struct buffer);" EOL,
    R"(struct buffer buffer_realloc (struct buffer o1, struct buffer o2) {)" EOL
    R"(  free(o1.d);)" EOL
    R"(  return o2;)" EOL
    R"(})" EOL,
    {"libStdlib", "typeBuffer"},
    {},
    false
  }},

  {"buffer_str", {
    false,
    "buffer_str",
    "struct str buffer_str (struct buffer);" EOL,
    R"(struct str buffer_str (struct buffer b) {)" EOL
    R"(  size_t l = 8 + (b.l * 3);)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, "[Buffer", 7);)" EOL
    R"(  for (size_t i = 0; i < b.l; i++) sprintf(d + 7 + (i * 3), " %02x", b.d[i]);)" EOL
    R"(  d[l - 1] = ']';)" EOL
    R"(  free(b.d);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdio", "libStdlib", "libString", "typeBuffer", "typeStr"},
    {},
    false
  }},

  {"buffer_to_str", {
    false,
    "buffer_to_str",
    "struct str buffer_to_str (struct buffer);" EOL,
    R"(struct str buffer_to_str (struct buffer b) {)" EOL
    R"(  return (struct str) {(char *) b.d, b.l};)" EOL
    R"(})" EOL,
    {"typeBuffer", "typeStr"},
    {},
    false
  }},

  {"byte_str", {
    false,
    "byte_str",
    "struct str byte_str (unsigned char);" EOL,
    R"(struct str byte_str (unsigned char x) {)" EOL
    R"(  char buf[8];)" EOL
    R"(  sprintf(buf, "%u", x);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"char_byte", {
    false,
    "char_byte",
    "unsigned char char_byte (char);" EOL,
    R"(unsigned char char_byte (char c) {)" EOL
    R"(  return (unsigned char) c;)" EOL
    R"(})" EOL,
    {},
    {},
    false
  }},

  {"char_isDigit", {
    false,
    "char_isDigit",
    "bool char_isDigit (char);" EOL,
    R"(bool char_isDigit (char c) {)" EOL
    R"(  return isdigit(c);)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool"},
    {},
    false
  }},

  {"char_isLetter", {
    false,
    "char_isLetter",
    "bool char_isLetter (char);" EOL,
    R"(bool char_isLetter (char c) {)" EOL
    R"(  return isalpha(c);)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool"},
    {},
    false
  }},

  {"char_isLetterOrDigit", {
    false,
    "char_isLetterOrDigit",
    "bool char_isLetterOrDigit (char);" EOL,
    R"(bool char_isLetterOrDigit (char c) {)" EOL
    R"(  return isalnum(c);)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool"},
    {},
    false
  }},

  {"char_isLower", {
    false,
    "char_isLower",
    "bool char_isLower (char);" EOL,
    R"(bool char_isLower (char c) {)" EOL
    R"(  return islower(c);)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool"},
    {},
    false
  }},

  {"char_isUpper", {
    false,
    "char_isUpper",
    "bool char_isUpper (char);" EOL,
    R"(bool char_isUpper (char c) {)" EOL
    R"(  return isupper(c);)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool"},
    {},
    false
  }},

  {"char_isWhitespace", {
    false,
    "char_isWhitespace",
    "bool char_isWhitespace (char);" EOL,
    R"(bool char_isWhitespace (char c) {)" EOL
    R"(  return isspace(c);)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool"},
    {},
    false
  }},

  {"char_lower", {
    false,
    "char_lower",
    "char char_lower (char);" EOL,
    R"(char char_lower (char c) {)" EOL
    R"(  return tolower(c);)" EOL
    R"(})" EOL,
    {"libCtype"},
    {},
    false
  }},

  {"char_repeat", {
    false,
    "char_repeat",
    "struct str char_repeat (char, int32_t);" EOL,
    R"(struct str char_repeat (char c, int32_t k) {)" EOL
    R"(  if (k <= 0) return str_alloc("");)" EOL
    R"(  size_t l = (size_t) k;)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  size_t i = 0;)" EOL
    R"(  while (i < l) d[i++] = c;)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdint", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"char_str", {
    false,
    "char_str",
    "struct str char_str (char);" EOL,
    R"(struct str char_str (char c) {)" EOL
    R"(  char buf[2] = {c, '\0'};)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"str_alloc", "typeStr"},
    {},
    false
  }},

  {"char_upper", {
    false,
    "char_upper",
    "char char_upper (char);" EOL,
    R"(char char_upper (char c) {)" EOL
    R"(  return toupper(c);)" EOL
    R"(})" EOL,
    {"libCtype"},
    {},
    false
  }},

  {"cstr_concat_str", {
    false,
    "cstr_concat_str",
    "struct str cstr_concat_str (const char *, struct str);" EOL,
    R"(struct str cstr_concat_str (const char *r, struct str s) {)" EOL
    R"(  size_t l = s.l + strlen(r);)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, r, l - s.l);)" EOL
    R"(  memcpy(&d[l - s.l], s.d, s.l);)" EOL
    R"(  free(s.d);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"cstr_eq_cstr", {
    false,
    "cstr_eq_cstr",
    "bool cstr_eq_cstr (const char *, const char *);" EOL,
    R"(bool cstr_eq_cstr (const char *c1, const char *c2) {)" EOL
    R"(  size_t l = strlen(c1);)" EOL
    R"(  return l == strlen(c2) && memcmp(c1, c2, l) == 0;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStddef", "libString"},
    {},
    false
  }},

  {"cstr_eq_str", {
    false,
    "cstr_eq_str",
    "bool cstr_eq_str (const char *, struct str);" EOL,
    R"(bool cstr_eq_str (const char *c, struct str s) {)" EOL
    R"(  bool r = s.l == strlen(c) && memcmp(s.d, c, s.l) == 0;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"cstr_ne_cstr", {
    false,
    "cstr_ne_cstr",
    "bool cstr_ne_cstr (const char *, const char *);" EOL,
    R"(bool cstr_ne_cstr (const char *c1, const char *c2) {)" EOL
    R"(  size_t l = strlen(c1);)" EOL
    R"(  return l != strlen(c2) || memcmp(c1, c2, l) != 0;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStddef", "libString"},
    {},
    false
  }},

  {"cstr_ne_str", {
    false,
    "cstr_ne_str",
    "bool cstr_ne_str (const char *, struct str);" EOL,
    R"(bool cstr_ne_str (const char *d, struct str s) {)" EOL
    R"(  bool r = s.l != strlen(d) || memcmp(s.d, d, s.l) != 0;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"date_now", {
    false,
    "date_now",
    "uint64_t date_now ();" EOL,
    R"(uint64_t date_now () {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    FILETIME ft;)" EOL
    R"(    GetSystemTimePreciseAsFileTime(&ft);)" EOL
    R"(    return (((uint64_t) ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 1e4 - 116444736e5;)" EOL
    R"(  #else)" EOL
    R"(    struct timespec ts;)" EOL
    R"(    clock_gettime(CLOCK_REALTIME, &ts);)" EOL
    R"(    return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libStdint", "libTime", "libWindows"},
    {},
    false
  }},

  {"enum_str", {
    false,
    "enum_str",
    "struct str enum_str (int);" EOL,
    R"(struct str enum_str (int d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%d", d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"error_alloc", {
    false,
    "error_alloc",
    "void error_alloc (err_state_t *, size_t);" EOL,
    R"~(void error_alloc (err_state_t *fn_err_state, size_t n1) {)~" EOL
    R"~(  char d[4096];)~" EOL
    R"~(  size_t l = 0;)~" EOL
    R"~(  for (err_stack_t *it = fn_err_state->stack_last; it != NULL; it = it->prev) {)~" EOL
    R"~(    const char *fmt = THE_EOL "  at %s (%s)";)~" EOL
    R"~(    size_t z = snprintf(NULL, 0, fmt, it->name, it->file);)~" EOL
    R"~(    if (l + z >= 4096) break;)~" EOL
    R"~(    sprintf(&d[l], fmt, it->name, it->file);)~" EOL
    R"~(    l += z;)~" EOL
    R"~(  })~" EOL
    R"~(  fprintf(stderr, "Allocation Error: failed to allocate %zu bytes%s" THE_EOL, n1, d);)~" EOL
    R"~(  exit(EXIT_FAILURE);)~" EOL
    R"~(})~" EOL,
    {"definitions", "libStddef", "libStdio", "libStdlib", "typeErrStack", "typeErrState"},
    {},
    false
  }},

  {"error_assign", {
    false,
    "error_assign",
    "void error_assign (err_state_t *, int, void *, void (*f) (void *), int, int);" EOL,
    R"(void error_assign (err_state_t *fn_err_state, int id, void *ctx, void (*f) (void *), int line, int col) {)" EOL
    R"(  fn_err_state->id = id;)" EOL
    R"(  fn_err_state->ctx = ctx;)" EOL
    R"(  fn_err_state->_free = f;)" EOL
    R"(  error_stack_pos(fn_err_state, line, col);)" EOL
    R"(  error_stack_str(fn_err_state);)" EOL
    R"(})" EOL,
    {"error_stack_pos", "error_stack_str", "typeErrState"},
    {},
    false
  }},

  {"error_buf_decrease", {
    false,
    "error_buf_decrease",
    "void error_buf_decrease (err_state_t *);" EOL,
    R"(void error_buf_decrease (err_state_t *fn_err_state) {)" EOL
    R"(  err_buf_t *buf = fn_err_state->buf_last;)" EOL
    R"(  fn_err_state->buf_last = buf->prev;)" EOL
    R"(  free(buf);)" EOL
    R"(})" EOL,
    {"libStdlib", "typeErrBuf", "typeErrState"},
    {},
    false
  }},

  {"error_buf_increase", {
    false,
    "error_buf_increase",
    "err_buf_t *error_buf_increase (err_state_t *);" EOL,
    R"(err_buf_t *error_buf_increase (err_state_t *fn_err_state) {)" EOL
    R"(  err_buf_t *buf = alloc(sizeof(err_buf_t));)" EOL
    R"(  buf->next = NULL;)" EOL
    R"(  buf->prev = fn_err_state->buf_last;)" EOL
    R"(  if (fn_err_state->buf_first == NULL) fn_err_state->buf_first = buf;)" EOL
    R"(  if (fn_err_state->buf_last != NULL) fn_err_state->buf_last->next = buf;)" EOL
    R"(  fn_err_state->buf_last = buf;)" EOL
    R"(  return fn_err_state->buf_last;)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "typeErrBuf", "typeErrState"},
    {},
    false
  }},

  {"error_stack_pop", {
    false,
    "error_stack_pop",
    "void error_stack_pop (err_state_t *);" EOL,
    R"(void error_stack_pop (err_state_t *fn_err_state) {)" EOL
    R"(  err_stack_t *stack = fn_err_state->stack_last;)" EOL
    R"(  fn_err_state->stack_last = stack->prev;)" EOL
    R"(  free(stack);)" EOL
    R"(})" EOL,
    {"libStdlib", "typeErrStack", "typeErrState"},
    {},
    false
  }},

  {"error_stack_pos", {
    false,
    "error_stack_pos",
    "void error_stack_pos (err_state_t *, int, int);" EOL,
    R"(void error_stack_pos (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  if (line != 0) fn_err_state->stack_last->line = line;)" EOL
    R"(  if (col != 0) fn_err_state->stack_last->col = col;)" EOL
    R"(})" EOL,
    {"typeErrState"},
    {},
    false
  }},

  {"error_stack_push", {
    false,
    "error_stack_push",
    "void error_stack_push (err_state_t *, const char *, const char *, int, int);" EOL,
    R"(void error_stack_push (err_state_t *fn_err_state, const char *file, const char *name, int line, int col) {)" EOL
    R"(  error_stack_pos(fn_err_state, line, col);)" EOL
    R"(  err_stack_t *stack = alloc(sizeof(err_stack_t));)" EOL
    R"(  stack->file = file;)" EOL
    R"(  stack->name = name;)" EOL
    R"(  stack->next = NULL;)" EOL
    R"(  stack->prev = fn_err_state->stack_last;)" EOL
    R"(  if (fn_err_state->stack_first == NULL) fn_err_state->stack_first = stack;)" EOL
    R"(  if (fn_err_state->stack_last != NULL) fn_err_state->stack_last->next = stack;)" EOL
    R"(  fn_err_state->stack_last = stack;)" EOL
    R"(})" EOL,
    {"alloc", "error_stack_pos", "libStddef", "typeErrStack", "typeErrState"},
    {},
    false
  }},

  {"error_stack_str", {
    false,
    "error_stack_str",
    "void error_stack_str (err_state_t *);" EOL,
    R"~(void error_stack_str (err_state_t *fn_err_state) {)~" EOL
    R"~(  struct str *stack = (struct str *) &((struct error_Error *) fn_err_state->ctx)->__THE_0_stack;)~" EOL
    R"~(  struct str message = ((struct error_Error *) fn_err_state->ctx)->__THE_0_message;)~" EOL
    R"~(  stack->l = message.l;)~" EOL
    R"~(  stack->d = re_alloc(stack->d, stack->l);)~" EOL
    R"~(  memcpy(stack->d, message.d, stack->l);)~" EOL
    R"~(  for (err_stack_t *it = fn_err_state->stack_last; it != NULL; it = it->prev) {)~" EOL
    R"~(    size_t z;)~" EOL
    R"~(    char *fmt;)~" EOL
    R"~(    if (it->col == 0 && it->line == 0) {)~" EOL
    R"~(      fmt = THE_EOL "  at %s (%s)";)~" EOL
    R"~(      z = snprintf(NULL, 0, fmt, it->name, it->file);)~" EOL
    R"~(    } else if (it->col == 0) {)~" EOL
    R"~(      fmt = THE_EOL "  at %s (%s:%d)";)~" EOL
    R"~(      z = snprintf(NULL, 0, fmt, it->name, it->file, it->line);)~" EOL
    R"~(    } else {)~" EOL
    R"~(      fmt = THE_EOL "  at %s (%s:%d:%d)";)~" EOL
    R"~(      z = snprintf(NULL, 0, fmt, it->name, it->file, it->line, it->col);)~" EOL
    R"~(    })~" EOL
    R"~(    stack->d = re_alloc(stack->d, stack->l + z + 1);)~" EOL
    R"~(    if (it->col == 0 && it->line == 0) {)~" EOL
    R"~(      sprintf(&stack->d[stack->l], fmt, it->name, it->file);)~" EOL
    R"~(    } else if (it->col == 0) {)~" EOL
    R"~(      sprintf(&stack->d[stack->l], fmt, it->name, it->file, it->line);)~" EOL
    R"~(    } else {)~" EOL
    R"~(      sprintf(&stack->d[stack->l], fmt, it->name, it->file, it->line, it->col);)~" EOL
    R"~(    })~" EOL
    R"~(    stack->l += z;)~" EOL
    R"~(  })~" EOL
    R"~(})~" EOL,
    {"definitions", "libStddef", "libStdio", "libString", "re_alloc", "typeErrStack", "typeErrState", "typeStr"},
    {"error_Error"},
    false
  }},

  {"error_unset", {
    false,
    "error_unset",
    "void error_unset (err_state_t *);" EOL,
    R"(void error_unset (err_state_t *fn_err_state) {)" EOL
    R"(  fn_err_state->id = -1;)" EOL
    R"(  fn_err_state->_free = NULL;)" EOL
    R"(})" EOL,
    {"libStddef", "typeErrState"},
    {},
    false
  }},

  {"f32_str", {
    false,
    "f32_str",
    "struct str f32_str (float);" EOL,
    R"(struct str f32_str (float f) {)" EOL
    R"(  char buf[512];)" EOL
    R"(  sprintf(buf, "%f", f);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"f64_str", {
    false,
    "f64_str",
    "struct str f64_str (double);" EOL,
    R"(struct str f64_str (double f) {)" EOL
    R"(  char buf[512];)" EOL
    R"(  sprintf(buf, "%f", f);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"float_str", {
    false,
    "float_str",
    "struct str float_str (double);" EOL,
    R"(struct str float_str (double f) {)" EOL
    R"(  char buf[512];)" EOL
    R"(  sprintf(buf, "%f", f);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"fs_appendFileSync", {
    false,
    "fs_appendFileSync",
    "void fs_appendFileSync (err_state_t *, int, int, struct str, struct buffer);" EOL,
    R"(void fs_appendFileSync (err_state_t *fn_err_state, int line, int col, struct str s, struct buffer b) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  FILE *f = fopen(c, "ab");)" EOL
    R"(  if (f == NULL) {)" EOL
    R"(    const char *fmt = "failed to open file `%s` for writing";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_appendFileSync_cleanup1;)" EOL
    R"(  })" EOL
    R"(  if (b.l != 0) {)" EOL
    R"(    if (fwrite(b.d, b.l, 1, f) != 1) {)" EOL
    R"(      const char *fmt = "failed appending to file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_appendFileSync_cleanup2;)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(fs_appendFileSync_cleanup2:)" EOL
    R"(  fclose(f);)" EOL
    R"(fs_appendFileSync_cleanup1:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  buffer_free(b);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "buffer_free", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "str_free", "typeBuffer", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_chmodSync", {
    false,
    "fs_chmodSync",
    "void fs_chmodSync (err_state_t *, int, int, struct str, int32_t);" EOL,
    R"(void fs_chmodSync (err_state_t *fn_err_state, int line, int col, struct str s, int32_t m) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    bool r = _chmod(c, m) == 0;)" EOL
    R"(  #else)" EOL
    R"(    bool r = chmod(c, m) == 0;)" EOL
    R"(  #endif)" EOL
    R"(  if (!r) {)" EOL
    R"(    const char *fmt = "failed to change mode to %" PRId32 " for file `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, m, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, m, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_chmodSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_chmodSync_cleanup:)" EOL
    R"(  str_free(s);)" EOL
    R"(  free(c);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libInttypes", "libSetJmp", "libStdbool", "libStddef", "libStdint", "libStdio", "libStdlib", "libSysStat", "libWinIo", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_chownSync", {
    false,
    "fs_chownSync",
    "void fs_chownSync (err_state_t *, int, int, struct str, int32_t, int32_t);" EOL,
    R"(void fs_chownSync (err_state_t *fn_err_state, int line, int col, struct str s, int32_t u, int32_t g) {)" EOL
    R"(  #ifndef THE_OS_WINDOWS)" EOL
    R"(    char *c = str_cstr(s);)" EOL
    R"(    if (chown(c, u, g) != 0) {)" EOL
    R"(      const char *fmt = "failed to change owner to %" PRId32 " and group to %" PRId32 " for file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, u, g, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, u, g, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    })" EOL
    R"(    free(c);)" EOL
    R"(  #endif)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "libUnistd", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_copyDirectorySync", {
    false,
    "fs_copyDirectorySync",
    "void fs_copyDirectorySync (err_state_t *, int, int, struct str, struct str);" EOL,
    R"(void fs_copyDirectorySync (err_state_t *fn_err_state, int line, int col, struct str n1, struct str n2) {)" EOL
    R"(  if (setjmp(error_buf_increase(fn_err_state)->buf) != 0) goto fs_copyDirectorySync_cleanup1;)" EOL
    R"(  if (fs_existsSync(str_copy(n2))) {)" EOL
    R"(    if (fs_isDirectorySync(str_copy(n2))) {)" EOL
    R"(      fs_rmdirSync(fn_err_state, line, col, str_copy(n2));)" EOL
    R"(    } else {)" EOL
    R"(      fs_rmSync(fn_err_state, line, col, str_copy(n2));)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  if (n1.l > 0 && n1.d[n1.l - 1] != (THE_PATH_SEP)[0] && n1.d[n1.l - 1] != '/') {)" EOL
    R"(    n1.d = re_alloc(n1.d, ++n1.l);)" EOL
    R"(    n1.d[n1.l - 1] = (THE_PATH_SEP)[0];)" EOL
    R"(  })" EOL
    R"(  if (n2.l > 0 && n2.d[n2.l - 1] != (THE_PATH_SEP)[0] && n2.d[n2.l - 1] != '/') {)" EOL
    R"(    n2.d = re_alloc(n2.d, ++n2.l);)" EOL
    R"(    n2.d[n2.l - 1] = (THE_PATH_SEP)[0];)" EOL
    R"(  })" EOL
    R"(  struct __THE_1_array_str files = fs_scandirSync(fn_err_state, line, col, str_copy(n1));)" EOL
    R"(  if (setjmp(fn_err_state->buf_last->buf) != 0) goto fs_copyDirectorySync_cleanup2;)" EOL
    R"(  fs_mkdirSync(fn_err_state, line, col, str_copy(n2));)" EOL
    R"(  for (size_t i = 0; i < files.l; i++) {)" EOL
    R"(    struct str file = str_concat_str(str_copy(n1), str_copy(files.d[i]));)" EOL
    R"(    if (fs_isDirectorySync(str_copy(file))) {)" EOL
    R"(      fs_copyDirectorySync(fn_err_state, line, col, file, str_concat_str(str_copy(n2), str_copy(files.d[i])));)" EOL
    R"(    } else {)" EOL
    R"(      fs_copyFileSync(fn_err_state, line, col, file, str_concat_str(str_copy(n2), str_copy(files.d[i])));)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(fs_copyDirectorySync_cleanup2:)" EOL
    R"(  __THE_1_array_str_free(files);)" EOL
    R"(fs_copyDirectorySync_cleanup1:)" EOL
    R"(  error_buf_decrease(fn_err_state);)" EOL
    R"(  str_free(n2);)" EOL
    R"(  str_free(n1);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"definitions", "error_buf_decrease", "error_buf_increase", "fs_copyFileSync", "fs_existsSync", "fs_isDirectorySync", "fs_mkdirSync", "fs_rmSync", "fs_rmdirSync", "fs_scandirSync", "libSetJmp", "libStddef", "re_alloc", "str_concat_str", "str_copy", "str_free", "typeErrState", "typeStr"},
    {"array_str", "array_str_free"},
    true
  }},

  {"fs_copyFileSync", {
    false,
    "fs_copyFileSync",
    "void fs_copyFileSync (err_state_t *, int, int, struct str, struct str);" EOL,
    R"(void fs_copyFileSync (err_state_t *fn_err_state, int line, int col, struct str n1, struct str n2) {)" EOL
    R"(  char *c1 = str_cstr(n1);)" EOL
    R"(  char *c2 = str_cstr(n2);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    if (CopyFile(c1, c2, 0) == 0) {)" EOL
    R"(      const char *fmt = "failed to copy file from `%s` to `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c1, c2);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c1, c2);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(  #else)" EOL
    R"(    size_t buf_len = 8192;)" EOL
    R"(    char buf[buf_len];)" EOL
    R"(    int fd1 = open(c1, O_RDONLY);)" EOL
    R"(    if (fd1 < 0) {)" EOL
    R"(      const char *fmt = "failed to open file descriptor of `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c1);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c1);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(    struct stat sb1;)" EOL
    R"(    if (fstat(fd1, &sb1) != 0) {)" EOL
    R"(      const char *fmt = "failed to stat file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c1);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c1);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup2;)" EOL
    R"(    })" EOL
    R"(    if ((sb1.st_mode & S_IFMT) != S_IFREG) {)" EOL
    R"(      const char *fmt = "cannot copy non-file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c1);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c1);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup2;)" EOL
    R"(    })" EOL
    R"(    int fd2 = open(c2, O_WRONLY | O_CREAT);)" EOL
    R"(    if (fd2 < 0) {)" EOL
    R"(      const char *fmt = "failed to open file descriptor of `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c2);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c2);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup2;)" EOL
    R"(    })" EOL
    R"(    struct stat sb2;)" EOL
    R"(    if (fstat(fd2, &sb2) != 0) {)" EOL
    R"(      const char *fmt = "failed to stat file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c2);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c2);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup3;)" EOL
    R"(    })" EOL
    R"(    if (sb1.st_dev == sb2.st_dev && sb1.st_ino == sb2.st_ino) {)" EOL
    R"(      const char *fmt = "can't copy same file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c2);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c2);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup3;)" EOL
    R"(    })" EOL
    R"(    if (sb2.st_size > 0 && ftruncate(fd2, 0) != 0) {)" EOL
    R"(      const char *fmt = "failed to truncate file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c2);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c2);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup3;)" EOL
    R"(    })" EOL
    R"(    if (fchmod(fd2, sb1.st_mode) != 0) {)" EOL
    R"(      const char *fmt = "failed to chmod file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c2);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c2);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_copyFileSync_cleanup3;)" EOL
    R"(    })" EOL
    R"(    size_t bytes = sb1.st_size;)" EOL
    R"(    while (bytes != 0) {)" EOL
    R"(      ssize_t read_bytes_raw = read(fd1, buf, bytes > buf_len ? buf_len : bytes);)" EOL
    R"(      if (read_bytes_raw <= 0) {)" EOL
    R"(        const char *fmt = "failed to read data from file `%s`";)" EOL
    R"(        size_t z = snprintf(NULL, 0, fmt, c1);)" EOL
    R"(        char *d = alloc(z + 1);)" EOL
    R"(        sprintf(d, fmt, c1);)" EOL
    R"(        error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(        goto fs_copyFileSync_cleanup3;)" EOL
    R"(      })" EOL
    R"(      size_t read_bytes = read_bytes_raw;)" EOL
    R"(      size_t written_bytes = 0;)" EOL
    R"(      while (written_bytes != read_bytes) {)" EOL
    R"(        ssize_t written_bytes_raw = write(fd2, &buf[written_bytes], read_bytes - written_bytes);)" EOL
    R"(        if (written_bytes_raw <= 0) {)" EOL
    R"(          const char *fmt = "failed to write data to file `%s`";)" EOL
    R"(          size_t z = snprintf(NULL, 0, fmt, c2);)" EOL
    R"(          char *d = alloc(z + 1);)" EOL
    R"(          sprintf(d, fmt, c2);)" EOL
    R"(          error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(          goto fs_copyFileSync_cleanup3;)" EOL
    R"(        })" EOL
    R"(        written_bytes += (size_t) written_bytes_raw;)" EOL
    R"(      })" EOL
    R"(      bytes -= written_bytes;)" EOL
    R"(    })" EOL
    R"(fs_copyFileSync_cleanup3:)" EOL
    R"(    close(fd2);)" EOL
    R"(fs_copyFileSync_cleanup2:)" EOL
    R"(    close(fd1);)" EOL
    R"(  #endif)" EOL
    R"(fs_copyFileSync_cleanup1:)" EOL
    R"(  free(c2);)" EOL
    R"(  free(c1);)" EOL
    R"(  str_free(n2);)" EOL
    R"(  str_free(n1);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libFcntl", "libSetJmp", "libStddef", "libStdio", "libStdlib", "libSysStat", "libSysTypes", "libUnistd", "libWindows", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_existsSync", {
    false,
    "fs_existsSync",
    "bool fs_existsSync (struct str);" EOL,
    R"(bool fs_existsSync (struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    bool r = _access(c, 0) == 0;)" EOL
    R"(  #else)" EOL
    R"(    bool r = access(c, 0) == 0;)" EOL
    R"(  #endif)" EOL
    R"(  str_free(s);)" EOL
    R"(  free(c);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"definitions", "libStdbool", "libStdlib", "libUnistd", "libWinIo", "str_cstr", "str_free", "typeStr"},
    {},
    false
  }},

  {"fs_isAbsoluteSync", {
    false,
    "fs_isAbsoluteSync",
    "bool fs_isAbsoluteSync (struct str);" EOL,
    R"(bool fs_isAbsoluteSync (struct str s) {)" EOL
    R"(  bool r = (s.l > 0 && s.d[0] == '/') || (s.l > 2 && isalpha(s.d[0]) && s.d[1] == ':' && s.d[2] == '\\');)" EOL
    R"(  str_free(s);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool", "str_free", "typeStr"},
    {},
    false
  }},

  {"fs_isDirectorySync", {
    false,
    "fs_isDirectorySync",
    "bool fs_isDirectorySync (struct str);" EOL,
    R"(bool fs_isDirectorySync (struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  bool b = false;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    struct _stat sb;)" EOL
    R"(    if (_stat(c, &sb) == 0) {)" EOL
    R"(      b = (sb.st_mode & _S_IFMT) == _S_IFDIR;)" EOL
    R"(    })" EOL
    R"(  #else)" EOL
    R"(    struct stat sb;)" EOL
    R"(    if (stat(c, &sb) == 0) {)" EOL
    R"(      b = (sb.st_mode & S_IFMT) == S_IFDIR;)" EOL
    R"(    })" EOL
    R"(  #endif)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  return b;)" EOL
    R"(})" EOL,
    {"definitions", "libStdbool", "libStdlib", "libSysStat", "str_cstr", "str_free", "typeStr"},
    {},
    false
  }},

  {"fs_isFileSync", {
    false,
    "fs_isFileSync",
    "bool fs_isFileSync (struct str);" EOL,
    R"(bool fs_isFileSync (struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  bool b = false;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    struct _stat sb;)" EOL
    R"(    if (_stat(c, &sb) == 0) {)" EOL
    R"(      b = (sb.st_mode & _S_IFMT) == _S_IFREG;)" EOL
    R"(    })" EOL
    R"(  #else)" EOL
    R"(    struct stat sb;)" EOL
    R"(    if (stat(c, &sb) == 0) {)" EOL
    R"(      b = (sb.st_mode & S_IFMT) == S_IFREG;)" EOL
    R"(    })" EOL
    R"(  #endif)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  return b;)" EOL
    R"(})" EOL,
    {"definitions", "libStdbool", "libStdlib", "libSysStat", "str_cstr", "str_free", "typeStr"},
    {},
    false
  }},

  {"fs_isSymbolicLinkSync", {
    false,
    "fs_isSymbolicLinkSync",
    "bool fs_isSymbolicLinkSync (struct str);" EOL,
    R"(bool fs_isSymbolicLinkSync (struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  bool b = false;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    HANDLE h = CreateFile(c, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);)" EOL
    R"(    if (h == INVALID_HANDLE_VALUE) {)" EOL
    R"(      free(c);)" EOL
    R"(      return b;)" EOL
    R"(    })" EOL
    R"(    char d[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];)" EOL
    R"(    struct win_reparse_data_buffer *r = (struct win_reparse_data_buffer *) d;)" EOL
    R"(    DWORD y;)" EOL
    R"(    if (DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, NULL, 0, &d, sizeof(d), &y, NULL)) {)" EOL
    R"(      if (r->ReparseTag == IO_REPARSE_TAG_SYMLINK) {)" EOL
    R"(        b = true;)" EOL
    R"(      } else if (r->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {)" EOL
    R"(        char *t = r->MountPointReparseBuffer.PathBuffer + r->MountPointReparseBuffer.SubstituteNameOffset;)" EOL
    R"(        if ()" EOL
    R"(          r->MountPointReparseBuffer.SubstituteNameLength >= 6 &&)" EOL
    R"(          t[0] == '\\' && t[1] == '?' && t[2] == '?' && t[3] == '\\' &&)" EOL
    R"(          ((t[4] >= 'A' && t[4] <= 'Z') || (t[4] >= 'a' && t[4] <= 'z')) &&)" EOL
    R"(          t[5] == ':' && (r->MountPointReparseBuffer.SubstituteNameLength == 6 || t[6] == '\\'))" EOL
    R"(        ) {)" EOL
    R"(          b = true;)" EOL
    R"(        })" EOL
    R"(      } else if (r->ReparseTag == IO_REPARSE_TAG_APPEXECLINK && r->AppExecLinkReparseBuffer.Version == 3) {)" EOL
    R"(        char *t = r->AppExecLinkReparseBuffer.StringList;)" EOL
    R"(        bool f = false;)" EOL
    R"(        for (size_t i = 0; i < 2; i++) {)" EOL
    R"(          size_t l = strlen(t);)" EOL
    R"(          if (l == 0) {)" EOL
    R"(            f = true;)" EOL
    R"(            break;)" EOL
    R"(          })" EOL
    R"(          t += l + 1;)" EOL
    R"(        })" EOL
    R"(        if (!f) {)" EOL
    R"(          if (strlen(t) >= 3 && ((t[0] >= 'a' && t[0] <= 'z') || (t[0] >= 'A' && t[0] <= 'Z')) && t[1] == ':' && t[2] == '\\') {)" EOL
    R"(            b = true;)" EOL
    R"(          })" EOL
    R"(        })" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(    CloseHandle(h);)" EOL
    R"(  #else)" EOL
    R"(    struct stat sb;)" EOL
    R"(    if (lstat(c, &sb) == 0) {)" EOL
    R"(      b = (sb.st_mode & S_IFMT) == S_IFLNK;)" EOL
    R"(    })" EOL
    R"(  #endif)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  return b;)" EOL
    R"(})" EOL,
    {"definitions", "libStdbool", "libStddef", "libStdlib", "libString", "libSysStat", "libWindows", "str_cstr", "str_free", "typeStr", "typeWinReparseDataBuffer"},
    {},
    false
  }},

  {"fs_linkSync", {
    false,
    "fs_linkSync",
    "void fs_linkSync (err_state_t *, int, int, struct str, struct str);" EOL,
    R"(void fs_linkSync (err_state_t *fn_err_state, int line, int col, struct str s1, struct str s2) {)" EOL
    R"(  char *c1 = str_cstr(s1);)" EOL
    R"(  char *c2 = str_cstr(s2);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    for (size_t i = 0; i < s1.l; i++) c1[i] = (c1[i] == '/' ? '\\' : c1[i]);)" EOL
    R"(    for (size_t i = 0; i < s2.l; i++) c2[i] = (c2[i] == '/' ? '\\' : c2[i]);)" EOL
    R"(    DWORD flags = 0;)" EOL
    R"(    if (GetFileAttributes(c1) & FILE_ATTRIBUTE_DIRECTORY) flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;)" EOL
    R"(    bool r = CreateSymbolicLink(c2, c1, flags);)" EOL
    R"(  #else)" EOL
    R"(    bool r = symlink(c1, c2) == 0;)" EOL
    R"(  #endif)" EOL
    R"(  if (!r) {)" EOL
    R"(    const char *fmt = "failed to create symbolic link from `%s` to `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c1, c2);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c1, c2);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_linkSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_linkSync_cleanup:)" EOL
    R"(  str_free(s1);)" EOL
    R"(  str_free(s2);)" EOL
    R"(  free(c1);)" EOL
    R"(  free(c2);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStdbool", "libStddef", "libStdio", "libStdlib", "libUnistd", "libWindows", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_mkdirSync", {
    false,
    "fs_mkdirSync",
    "void fs_mkdirSync (err_state_t *, int, int, struct str);" EOL,
    R"(void fs_mkdirSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    bool r = CreateDirectory(c, NULL);)" EOL
    R"(  #else)" EOL
    R"(    bool r = mkdir(c, 0777) == 0;)" EOL
    R"(  #endif)" EOL
    R"(  if (!r) {)" EOL
    R"(    const char *fmt = "failed to create directory `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_mkdirSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_mkdirSync_cleanup:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStdbool", "libStddef", "libStdio", "libStdlib", "libSysStat", "libWindows", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_readFileSync", {
    false,
    "fs_readFileSync",
    "struct buffer fs_readFileSync (err_state_t *, int, int, struct str);" EOL,
    R"(struct buffer fs_readFileSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  FILE *f = fopen(c, "rb");)" EOL
    R"(  if (f == NULL) {)" EOL
    R"(    const char *fmt = "failed to open file `%s` for reading";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_readFileSync_cleanup;)" EOL
    R"(  })" EOL
    R"(  unsigned char *d = NULL;)" EOL
    R"(  unsigned char b[4096];)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  size_t y;)" EOL
    R"(  while ((y = fread(b, 1, sizeof(b), f)) > 0) {)" EOL
    R"(    d = re_alloc(d, l + y);)" EOL
    R"(    memcpy(&d[l], b, y);)" EOL
    R"(    l += y;)" EOL
    R"(  })" EOL
    R"(  fclose(f);)" EOL
    R"(fs_readFileSync_cleanup:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free((struct str) s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  return (struct buffer) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "libString", "re_alloc", "str_cstr", "str_free", "typeBuffer", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_realpathSync", {
    false,
    "fs_realpathSync",
    "struct str fs_realpathSync (err_state_t *, int, int, struct str);" EOL,
    R"(struct str fs_realpathSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  char *d = NULL;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    HANDLE h = CreateFile(c, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);)" EOL
    R"(    if (h == INVALID_HANDLE_VALUE) {)" EOL
    R"(      const char *fmt = "failed to create handle to get real path of file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_realpathSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(    size_t l = GetFinalPathNameByHandle(h, NULL, 0, VOLUME_NAME_DOS);)" EOL
    R"(    if (l == 0) {)" EOL
    R"(      const char *fmt = "not enough memory to get real path of file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_realpathSync_cleanup2;)" EOL
    R"(    })" EOL
    R"(    char *r = alloc(l + 1);)" EOL
    R"(    if (GetFinalPathNameByHandle(h, r, MAX_PATH, VOLUME_NAME_DOS) == 0) {)" EOL
    R"(      const char *fmt = "failed to get real path by handle of file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_realpathSync_cleanup3;)" EOL
    R"(    })" EOL
    R"(    if (memcmp(r, "\\\\?\\UNC\\", 8) == 0) {)" EOL
    R"(      l -= 6;)" EOL
    R"(      d = alloc(l);)" EOL
    R"(      memcpy(d, &r[6], l);)" EOL
    R"(      d[0] = '\\';)" EOL
    R"(    } else if (memcmp(r, "\\\\?\\", 4) == 0) {)" EOL
    R"(      l -= 4;)" EOL
    R"(      d = alloc(l);)" EOL
    R"(      memcpy(d, &r[4], l);)" EOL
    R"(    })" EOL
    R"(fs_realpathSync_cleanup3:)" EOL
    R"(    free(r);)" EOL
    R"(fs_realpathSync_cleanup2:)" EOL
    R"(    CloseHandle(h);)" EOL
    R"(    if (fn_err_state->id != -1) goto fs_realpathSync_cleanup1;)" EOL
    R"(  #else)" EOL
    R"(    d = realpath(c, NULL);)" EOL
    R"(    size_t l = d == NULL ? 0 : strlen(d);)" EOL
    R"(  #endif)" EOL
    R"(  if (d == NULL) {)" EOL
    R"(    const char *fmt = "failed to get real path of file `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *m = alloc(z + 1);)" EOL
    R"(    sprintf(m, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {m, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_realpathSync_cleanup1;)" EOL
    R"(  })" EOL
    R"(fs_realpathSync_cleanup1:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free((struct str) s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "libString", "libWindows", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_renameSync", {
    false,
    "fs_renameSync",
    "void fs_renameSync (err_state_t *, int, int, struct str, struct str);" EOL,
    R"(void fs_renameSync (err_state_t *fn_err_state, int line, int col, struct str n1, struct str n2) {)" EOL
    R"(  char *c1 = str_cstr(n1);)" EOL
    R"(  char *c2 = str_cstr(n2);)" EOL
    R"(  if (rename(c1, c2) != 0) {)" EOL
    R"(    const char *fmt = "failed to rename from `%s` to `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c1, c2);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c1, c2);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_renameSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_renameSync_cleanup:)" EOL
    R"(  free(c2);)" EOL
    R"(  free(c1);)" EOL
    R"(  str_free(n2);)" EOL
    R"(  str_free(n1);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_rmSync", {
    false,
    "fs_rmSync",
    "void fs_rmSync (err_state_t *, int, int, struct str);" EOL,
    R"(void fs_rmSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  if (remove(c) != 0) {)" EOL
    R"(    const char *fmt = "failed to remove file `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_rmSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_rmSync_cleanup:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_rmdirSync", {
    false,
    "fs_rmdirSync",
    "void fs_rmdirSync (err_state_t *, int, int, struct str);" EOL,
    R"(void fs_rmdirSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    bool r = RemoveDirectory(c);)" EOL
    R"(  #else)" EOL
    R"(    bool r = rmdir(c) == 0;)" EOL
    R"(  #endif)" EOL
    R"(  if (!r) {)" EOL
    R"(    const char *fmt = "failed to remove directory `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_rmdirSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_rmdirSync_cleanup:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStdbool", "libStddef", "libStdio", "libStdlib", "libUnistd", "libWindows", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_scandirSync", {
    false,
    "fs_scandirSync",
    "struct __THE_1_array_str fs_scandirSync (err_state_t *, int, int, struct str);" EOL,
    R"(struct __THE_1_array_str fs_scandirSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  struct str *r = NULL;)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    if (GetFileAttributes(c) == INVALID_FILE_ATTRIBUTES) {)" EOL
    R"(      const char *fmt = "directory `%s` doesn't exist";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    } else if (!(GetFileAttributes(c) & FILE_ATTRIBUTE_DIRECTORY)) {)" EOL
    R"(      const char *fmt = "failed to scan non-directory `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(    const char *fmt = s.l == 0 ? "./*" : ((s.d[s.l - 1] == '/' || s.d[s.l - 1] == '\\') ? "%s*" : "%s\\*");)" EOL
    R"(    char *b = alloc(s.l + 4);)" EOL
    R"(    snprintf(b, s.l + 3, fmt, c);)" EOL
    R"(    WIN32_FIND_DATA m;)" EOL
    R"(    HANDLE h = FindFirstFile(b, &m);)" EOL
    R"(    free(b);)" EOL
    R"(    if (h == INVALID_HANDLE_VALUE && GetLastError() != ERROR_FILE_NOT_FOUND) {)" EOL
    R"(      const char *fmt = "failed to open directory `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    } else if (h == INVALID_HANDLE_VALUE) {)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(    WIN32_FIND_DATA *n = &m;)" EOL
    R"(    while (true) {)" EOL
    R"(      if (strcmp(n->cFileName, ".") != 0 && strcmp(n->cFileName, "..") != 0) {)" EOL
    R"(        r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(        r[l - 1] = str_alloc(n->cFileName);)" EOL
    R"(      })" EOL
    R"(      bool g = FindNextFile(h, n);)" EOL
    R"(      if (!g && GetLastError() == ERROR_NO_MORE_FILES) {)" EOL
    R"(        break;)" EOL
    R"(      } else if (!g) {)" EOL
    R"(        const char *fmt = "failed to scan next file of directory `%s`";)" EOL
    R"(        size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(        char *d = alloc(z + 1);)" EOL
    R"(        sprintf(d, fmt, c);)" EOL
    R"(        error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(        goto fs_scandirSync_cleanup2;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(fs_scandirSync_cleanup2:)" EOL
    R"(    FindClose(h);)" EOL
    R"(  #else)" EOL
    R"(    struct stat sb;)" EOL
    R"(    if (stat(c, &sb) != 0) {)" EOL
    R"(      const char *fmt = "directory `%s` doesn't exist";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    } else if ((sb.st_mode & S_IFMT) != S_IFDIR) {)" EOL
    R"(      const char *fmt = "failed to scan non-directory `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(    DIR *f = opendir(c);)" EOL
    R"(    if (f == NULL) {)" EOL
    R"(      const char *fmt = "failed to open directory `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_scandirSync_cleanup1;)" EOL
    R"(    })" EOL
    R"(    struct dirent *a;)" EOL
    R"(    while ((a = readdir(f)) != NULL) {)" EOL
    R"(      if (strcmp(a->d_name, ".") == 0 || strcmp(a->d_name, "..") == 0) continue;)" EOL
    R"(      r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(      r[l - 1] = str_alloc(a->d_name);)" EOL
    R"(    })" EOL
    R"(    closedir(f);)" EOL
    R"(  #endif)" EOL
    R"(  for (size_t j = 0; j < l; j++) {)" EOL
    R"(    for (size_t k = j + 1; k < l; k++) {)" EOL
    R"(      if (memcmp(r[j].d, r[k].d, r[j].l > r[k].l ? r[k].l : r[j].l) > 0) {)" EOL
    R"(        struct str t = r[j];)" EOL
    R"(        r[j] = r[k];)" EOL
    R"(        r[k] = t;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(fs_scandirSync_cleanup1:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  return (struct __THE_1_array_str) {r, l};)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libDirent", "libSetJmp", "libStdbool", "libStddef", "libStdio", "libStdlib", "libString", "libSysStat", "libWindows", "re_alloc", "str_alloc", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "array_str", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_statSync", {
    false,
    "fs_statSync",
    "struct fs_Stats *fs_statSync (err_state_t *, int, int, struct str);" EOL,
    R"(struct fs_Stats *fs_statSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  struct fs_Stats *r;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    struct _stat sb;)" EOL
    R"(    if (_stat(c, &sb) != 0) {)" EOL
    R"(      const char *fmt = "failed to stat file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_statSync_cleanup;)" EOL
    R"(    })" EOL
    R"(    r = fs_Stats_alloc(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atime, 0, sb.st_mtime, 0, sb.st_ctime, 0, sb.st_ctime, 0, sb.st_size, 0, 4096);)" EOL
    R"(  #else)" EOL
    R"(    struct stat sb;)" EOL
    R"(    if (stat(c, &sb) != 0) {)" EOL
    R"(      const char *fmt = "failed to stat file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_statSync_cleanup;)" EOL
    R"(    })" EOL
    R"(    #ifdef THE_OS_MACOS)" EOL
    R"(      r = fs_Stats_alloc(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atimespec.tv_sec, sb.st_atimespec.tv_nsec, sb.st_mtimespec.tv_sec, sb.st_mtimespec.tv_nsec, sb.st_ctimespec.tv_sec, sb.st_ctimespec.tv_nsec, sb.st_birthtimespec.tv_sec, sb.st_birthtimespec.tv_nsec, sb.st_size, sb.st_blocks, sb.st_blksize);)" EOL
    R"(    #else)" EOL
    R"(      r = fs_Stats_alloc(sb.st_dev, sb.st_mode, sb.st_nlink, sb.st_ino, sb.st_uid, sb.st_gid, sb.st_rdev, sb.st_atim.tv_sec, sb.st_atim.tv_nsec, sb.st_mtim.tv_sec, sb.st_mtim.tv_nsec, sb.st_ctim.tv_sec, sb.st_ctim.tv_nsec, sb.st_ctim.tv_sec, sb.st_ctim.tv_nsec, sb.st_size, sb.st_blocks, sb.st_blksize);)" EOL
    R"(    #endif)" EOL
    R"(  #endif)" EOL
    R"(fs_statSync_cleanup:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "libSysStat", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free", "fs_Stats", "fs_Stats_alloc"},
    true
  }},

  {"fs_unlinkSync", {
    false,
    "fs_unlinkSync",
    "void fs_unlinkSync (err_state_t *, int, int, struct str);" EOL,
    R"(void fs_unlinkSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  bool r;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    if (GetFileAttributes(c) & FILE_ATTRIBUTE_DIRECTORY) {)" EOL
    R"(      r = RemoveDirectory(c);)" EOL
    R"(    } else {)" EOL
    R"(      r = _unlink(c) == 0;)" EOL
    R"(    })" EOL
    R"(  #else)" EOL
    R"(    r = unlink(c) == 0;)" EOL
    R"(  #endif)" EOL
    R"(  if (!r) {)" EOL
    R"(    const char *fmt = "failed to unlink path `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_unlinkSync_cleanup;)" EOL
    R"(  })" EOL
    R"(fs_unlinkSync_cleanup:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStdbool", "libStddef", "libStdio", "libStdlib", "libUnistd", "libWindows", "str_cstr", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"fs_writeFileSync", {
    false,
    "fs_writeFileSync",
    "void fs_writeFileSync (err_state_t *, int, int, struct str, struct buffer);" EOL,
    R"(void fs_writeFileSync (err_state_t *fn_err_state, int line, int col, struct str s, struct buffer b) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  FILE *f = fopen(c, "wb");)" EOL
    R"(  if (f == NULL) {)" EOL
    R"(    const char *fmt = "failed to open file `%s` for writing";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto fs_writeFileSync_cleanup1;)" EOL
    R"(  })" EOL
    R"(  if (b.l != 0) {)" EOL
    R"(    if (fwrite(b.d, b.l, 1, f) != 1) {)" EOL
    R"(      const char *fmt = "failed to write file `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, c);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto fs_writeFileSync_cleanup2;)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(fs_writeFileSync_cleanup2:)" EOL
    R"(  fclose(f);)" EOL
    R"(fs_writeFileSync_cleanup1:)" EOL
    R"(  free(c);)" EOL
    R"(  str_free(s);)" EOL
    R"(  buffer_free(b);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(})" EOL,
    {"alloc", "buffer_free", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "str_free", "typeBuffer", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"i16_str", {
    false,
    "i16_str",
    "struct str i16_str (int16_t);" EOL,
    R"(struct str i16_str (int16_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRId16, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"i32_str", {
    false,
    "i32_str",
    "struct str i32_str (int32_t);" EOL,
    R"(struct str i32_str (int32_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRId32, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"i64_str", {
    false,
    "i64_str",
    "struct str i64_str (int64_t);" EOL,
    R"(struct str i64_str (int64_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRId64, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"i8_str", {
    false,
    "i8_str",
    "struct str i8_str (int8_t);" EOL,
    R"(struct str i8_str (int8_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRId8, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"int_str", {
    false,
    "int_str",
    "struct str int_str (int32_t);" EOL,
    R"(struct str int_str (int32_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRId32, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"new_error", {
    false,
    "new_error",
    "struct error_Error *new_error (struct str);" EOL,
    R"(struct error_Error *new_error (struct str n1) {)" EOL
    R"(  return error_Error_alloc(n1, (struct str) {NULL, 0});)" EOL
    R"(})" EOL,
    {"libStddef", "typeStr"},
    {"error_Error", "error_Error_alloc"},
    false
  }},

  {"os_arch", {
    false,
    "os_arch",
    "struct str os_arch (err_state_t *, int, int);" EOL,
    R"(struct str os_arch (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    SYSTEM_INFO info;)" EOL
    R"(    GetSystemInfo(&info);)" EOL
    R"(    switch (info.wProcessorArchitecture) {)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_ALPHA:)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_ALPHA64: return str_alloc("alpha");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_AMD64: return str_alloc("x86_64");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_ARM: return str_alloc("arm");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_ARM64: return str_alloc("arm64");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64: return str_alloc("i686");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_IA64: return str_alloc("ia64");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_INTEL:)" EOL
    R"(        switch (info.wProcessorLevel) {)" EOL
    R"(          case 4: return str_alloc("i486");)" EOL
    R"(          case 5: return str_alloc("i586");)" EOL
    R"(          default: return str_alloc("i386");)" EOL
    R"(        })" EOL
    R"(      case PROCESSOR_ARCHITECTURE_MIPS: return str_alloc("mips");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_PPC: return str_alloc("powerpc");)" EOL
    R"(      case PROCESSOR_ARCHITECTURE_SHX: return str_alloc("sh");)" EOL
    R"(    })" EOL
    R"(    return str_alloc("unknown");)" EOL
    R"(  #else)" EOL
    R"(    struct utsname buf;)" EOL
    R"(    if (uname(&buf) < 0) {)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to retrieve uname information"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(    return str_alloc(buf.machine);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "error_assign", "libSetJmp", "libStddef", "libSysUtsname", "libWindows", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"os_name", {
    false,
    "os_name",
    "struct str os_name (err_state_t *, int, int);" EOL,
    R"(struct str os_name (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    return str_alloc("Windows");)" EOL
    R"(  #else)" EOL
    R"(    struct utsname buf;)" EOL
    R"(    if (uname(&buf) < 0) {)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to retrieve uname information"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(    if (strcmp(buf.sysname, "Darwin") == 0) return str_alloc("macOS");)" EOL
    R"(    return str_alloc(buf.sysname);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "error_assign", "libSetJmp", "libStddef", "libString", "libSysUtsname", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"path_basename", {
    false,
    "path_basename",
    "struct str path_basename (struct str);" EOL,
    R"(struct str path_basename (struct str s) {)" EOL
    R"(  if (s.l == 0) return s;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    char e = '\\';)" EOL
    R"(  #else)" EOL
    R"(    char e = '/';)" EOL
    R"(  #endif)" EOL
    R"(  size_t a = 0;)" EOL
    R"(  size_t b = 0;)" EOL
    R"(  unsigned char k = 0;)" EOL
    R"(  for (size_t i = s.l - 1;; i--) {)" EOL
    R"(    if (k == 1 && s.d[i] == e) {)" EOL
    R"(      a = i + 1;)" EOL
    R"(      break;)" EOL
    R"(    } else if (k == 0 && s.d[i] != e) {)" EOL
    R"(      k = 1;)" EOL
    R"(      b = i + 1;)" EOL
    R"(    })" EOL
    R"(    if (i == 0) break;)" EOL
    R"(  })" EOL
    R"(  if (k == 0) {)" EOL
    R"(    str_free(s);)" EOL
    R"(    return str_alloc("");)" EOL
    R"(  })" EOL
    R"(  size_t l = b - a;)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, &s.d[a], l);)" EOL
    R"(  str_free(s);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "libStddef", "libString", "str_alloc", "str_free", "typeStr"},
    {},
    false
  }},

  {"path_dirname", {
    false,
    "path_dirname",
    "struct str path_dirname (struct str);" EOL,
    R"(struct str path_dirname (struct str s) {)" EOL
    R"(  if (s.l == 0) {)" EOL
    R"(    str_free(s);)" EOL
    R"(    return str_alloc(".");)" EOL
    R"(  })" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    char e = '\\';)" EOL
    R"(  #else)" EOL
    R"(    char e = '/';)" EOL
    R"(  #endif)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  unsigned char k = 0;)" EOL
    R"(  for (size_t i = s.l - 1;; i--) {)" EOL
    R"(    if (k == 1 && s.d[i] == e) {)" EOL
    R"(      l = i;)" EOL
    R"(      break;)" EOL
    R"(    } else if (k == 0 && s.d[i] != e) {)" EOL
    R"(      k = 1;)" EOL
    R"(    })" EOL
    R"(    if (i == 0) break;)" EOL
    R"(  })" EOL
    R"(  if (l == 0) {)" EOL
    R"(    s.d = re_alloc(s.d, 1);)" EOL
    R"(    if (s.d[0] != e) s.d[0] = '.';)" EOL
    R"(    return (struct str) {s.d, 1};)" EOL
    R"(  })" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, s.d, l);)" EOL
    R"(  str_free(s);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "libStddef", "libString", "re_alloc", "str_alloc", "str_free", "typeStr"},
    {},
    false
  }},

  {"path_mktemp", {
    false,
    "path_mktemp",
    "char *path_mktemp (err_state_t *, int, int, unsigned char (*f) (char *));" EOL,
    R"(char *path_mktemp (err_state_t *fn_err_state, int line, int col, unsigned char (*f) (char *)) {)" EOL
    R"(  static const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";)" EOL
    R"(  static const size_t chars_len = 62;)" EOL
    R"(  static const size_t x = 6;)" EOL
    R"(  char *d = path_tmpdir(fn_err_state, line, col);)" EOL
    R"(  size_t l = strlen(d) + x;)" EOL
    R"(  d = re_alloc(d, l + 1);)" EOL
    R"(  memcpy(&d[l - x], "XXXXXX", x + 1);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    size_t j = TMP_MAX;)" EOL
    R"(    unsigned long long v;)" EOL
    R"(    do {)" EOL
    R"(      if (!SystemFunction036((void *) &v, sizeof(v))) {)" EOL
    R"(        free(d);)" EOL
    R"(        error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to generate random with `SystemFunction036`"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(        longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(      })" EOL
    R"(      char *p = &d[l - x];)" EOL
    R"(      for (size_t i = 0; i < x; i++) {)" EOL
    R"(        *p++ = chars[v % chars_len];)" EOL
    R"(        v /= chars_len;)" EOL
    R"(      })" EOL
    R"(      if (f(d)) return d;)" EOL
    R"(    } while (--j);)" EOL
    R"(    free(d);)" EOL
    R"(    return NULL;)" EOL
    R"(  #else)" EOL
    R"(    if (!f(d)) {)" EOL
    R"(      free(d);)" EOL
    R"(      return NULL;)" EOL
    R"(    })" EOL
    R"(    return d;)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "error_assign", "externSystemFunction036", "libSetJmp", "libStddef", "libStdio", "libStdlib", "libString", "path_tmpdir", "re_alloc", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"path_tempDirectory", {
    false,
    "path_tempDirectory",
    "struct str path_tempDirectory (err_state_t *, int, int);" EOL,
    R"(struct str path_tempDirectory (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  char *d = path_mktemp(fn_err_state, line, col, path_tempDirectoryFunctor);)" EOL
    R"(  if (d == NULL) {)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to create temporary directory"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  struct str r = str_alloc(d);)" EOL
    R"(  free(d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"error_assign", "libSetJmp", "libStddef", "libStdlib", "path_mktemp", "path_tempDirectoryFunctor", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"path_tempDirectoryFunctor", {
    false,
    "path_tempDirectoryFunctor",
    "unsigned char path_tempDirectoryFunctor (char *);" EOL,
    R"(unsigned char path_tempDirectoryFunctor (char *c) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    return CreateDirectory(c, NULL) == 0 ? 0 : 1;)" EOL
    R"(  #else)" EOL
    R"(    return mkdtemp(c) == NULL ? 0 : 1;)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libStddef", "libStdlib", "libUnistd", "libWindows"},
    {},
    false
  }},

  {"path_tempFile", {
    false,
    "path_tempFile",
    "struct str path_tempFile (err_state_t *, int, int);" EOL,
    R"(struct str path_tempFile (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  char *d = path_mktemp(fn_err_state, line, col, path_tempFileFunctor);)" EOL
    R"(  if (d == NULL) {)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to create temporary file"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  struct str r = str_alloc(d);)" EOL
    R"(  free(d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"error_assign", "libSetJmp", "libStddef", "libStdlib", "path_mktemp", "path_tempFileFunctor", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"path_tempFileFunctor", {
    false,
    "path_tempFileFunctor",
    "unsigned char path_tempFileFunctor (char *);" EOL,
    R"(unsigned char path_tempFileFunctor (char *c) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    return INVALID_HANDLE_VALUE == CreateFile(c, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL) ? 0 : 1;)" EOL
    R"(  #else)" EOL
    R"(    int fd = mkstemp(c);)" EOL
    R"(    if (fd != -1) close(fd);)" EOL
    R"(    return fd == -1 ? 0 : 1;)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libStddef", "libStdlib", "libUnistd", "libWindows"},
    {},
    false
  }},

  {"path_tmpdir", {
    false,
    "path_tmpdir",
    "char *path_tmpdir (err_state_t *, int, int);" EOL,
    R"(char *path_tmpdir (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  char *d;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    size_t l = GetTempPath(0, NULL);)" EOL
    R"(    if (l == 0) {)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to get temporary path"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(    l -= 1;)" EOL
    R"(    d = alloc(l + 1);)" EOL
    R"(    if (GetTempPath(l + 1, d) == 0) {)" EOL
    R"(      free(d);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to get temporary path"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(    if (d[l - 1] != '\\' && d[l - 1] != '/') {)" EOL
    R"(      d[l] = '\\';)" EOL
    R"(      d = re_alloc(d, ++l + 1);)" EOL
    R"(    })" EOL
    R"(    d[l] = '\0';)" EOL
    R"(  #else)" EOL
    R"(    char *f;)" EOL
    R"(    if ()" EOL
    R"(      (f = getenv("TMPDIR")) == NULL &&)" EOL
    R"(      (f = getenv("TMP")) == NULL &&)" EOL
    R"(      (f = getenv("TEMP")) == NULL &&)" EOL
    R"(      (f = getenv("TEMPDIR")) == NULL)" EOL
    R"(    ) {)" EOL
    R"(      f = "/tmp/";)" EOL
    R"(    })" EOL
    R"(    size_t l = strlen(f);)" EOL
    R"(    d = alloc(l + 1);)" EOL
    R"(    memcpy(d, f, l);)" EOL
    R"(    if (f[l - 1] != '/') {)" EOL
    R"(      d[l] = '/';)" EOL
    R"(      d = re_alloc(d, ++l + 1);)" EOL
    R"(    })" EOL
    R"(    d[l] = '\0';)" EOL
    R"(  #endif)" EOL
    R"(  return d;)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStddef", "libStdlib", "libString", "libWindows", "re_alloc", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"print", {
    false,
    "print",
    "void print (FILE *, const char *, ...);" EOL,
    R"(void print (FILE *stream, const char *fmt, ...) {)" EOL
    R"(  char *d = alloc(1024);)" EOL
    R"(  size_t cap = 1024;)" EOL
    R"(  size_t len = 0;)" EOL
    R"(  size_t y;)" EOL
    R"(  va_list args;)" EOL
    R"(  va_start(args, fmt);)" EOL
    R"(  while (*fmt) {)" EOL
    R"(    switch (*fmt++) {)" EOL
    R"(      case 't': {)" EOL
    R"(        int a = va_arg(args, int);)" EOL
    R"(        y = a == 0 ? 5 : 4;)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        memcpy(&d[len], a == 0 ? "false" : "true", y);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'b': {)" EOL
    R"(        unsigned int a = va_arg(args, unsigned int);)" EOL
    R"(        y = snprintf(NULL, 0, "%u", a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%u", a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'c': {)" EOL
    R"(        char a = va_arg(args, int);)" EOL
    R"(        y = snprintf(NULL, 0, "%c", a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%c", a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'e':)" EOL
    R"(      case 'f':)" EOL
    R"(      case 'g': {)" EOL
    R"(        double a = va_arg(args, double);)" EOL
    R"(        y = snprintf(NULL, 0, "%f", a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%f", a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'h':)" EOL
    R"(      case 'j':)" EOL
    R"(      case 'v':)" EOL
    R"(      case 'w': {)" EOL
    R"(        int a = va_arg(args, int);)" EOL
    R"(        y = snprintf(NULL, 0, "%d", a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%d", a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'i':)" EOL
    R"(      case 'k': {)" EOL
    R"(        int32_t a = va_arg(args, int32_t);)" EOL
    R"(        y = snprintf(NULL, 0, "%" PRId32, a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%" PRId32, a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'l': {)" EOL
    R"(        int64_t a = va_arg(args, int64_t);)" EOL
    R"(        y = snprintf(NULL, 0, "%" PRId64, a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%" PRId64, a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'p': {)" EOL
    R"(        void *a = va_arg(args, void *);)" EOL
    R"(        y = snprintf(NULL, 0, "%p", a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%p", a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 's': {)" EOL
    R"(        struct str a = va_arg(args, struct str);)" EOL
    R"(        y = a.l;)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        memcpy(&d[len], a.d, y);)" EOL
    R"(        free(a.d);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'u': {)" EOL
    R"(        uint32_t a = va_arg(args, uint32_t);)" EOL
    R"(        y = snprintf(NULL, 0, "%" PRIu32, a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%" PRIu32, a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'y': {)" EOL
    R"(        uint64_t a = va_arg(args, uint64_t);)" EOL
    R"(        y = snprintf(NULL, 0, "%" PRIu64, a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        sprintf(&d[len], "%" PRIu64, a);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(      case 'z': {)" EOL
    R"(        char *a = va_arg(args, char *);)" EOL
    R"(        y = strlen(a);)" EOL
    R"(        if (len + y >= cap) d = re_alloc(d, cap += (y / 1024 + 1) * 1024);)" EOL
    R"(        memcpy(&d[len], a, y);)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(    len += y;)" EOL
    R"(  })" EOL
    R"(  va_end(args);)" EOL
    R"(  d[len] = '\0';)" EOL
    R"(  fputs(d, stream);)" EOL
    R"(  fflush(stream);)" EOL
    R"(  free(d);)" EOL
    R"(})" EOL,
    {"alloc", "libInttypes", "libStdarg", "libStddef", "libStdint", "libStdio", "libStdlib", "libString", "re_alloc", "typeStr"},
    {},
    false
  }},

  {"process_args", {
    false,
    "process_args",
    "struct __THE_1_array_str process_args ();" EOL,
    R"(struct __THE_1_array_str process_args () {)" EOL
    R"(  struct str *d = alloc(argc * sizeof(struct str));)" EOL
    R"(  for (int i = 0; i < argc; i++) d[i] = str_alloc(argv[i]);)" EOL
    R"(  return (struct __THE_1_array_str) {d, (size_t) argc};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "str_alloc", "typeStr"},
    {"array_str"},
    false
  }},

  {"process_cwd", {
    false,
    "process_cwd",
    "struct str process_cwd (err_state_t *, int, int);" EOL,
    R"(struct str process_cwd (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  char buf[256];)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    char *p = _getcwd(buf, 256);)" EOL
    R"(  #else)" EOL
    R"(    char *p = getcwd(buf, 256);)" EOL
    R"(  #endif)" EOL
    R"(  if (p == NULL) {)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to retrieve current working directory information"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"definitions", "error_assign", "libSetJmp", "libStddef", "libUnistd", "libWinDirect", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"process_env", {
    false,
    "process_env",
    "struct __THE_1_map_strMSstrME process_env ();" EOL,
    R"(struct __THE_1_map_strMSstrME process_env () {)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  while (environ[l] != NULL) l++;)" EOL
    R"(  struct __THE_1_pair_strMSstrME *d = alloc(l * sizeof(struct __THE_1_pair_strMSstrME));)" EOL
    R"(  for (size_t i = 0; i < l; i++) {)" EOL
    R"(    char *p = environ[i];)" EOL
    R"(    size_t pl = strlen(p);)" EOL
    R"(    char *e = strchr(p, '=');)" EOL
    R"(    if (e == NULL) {)" EOL
    R"(      d[i] = (struct __THE_1_pair_strMSstrME) {str_calloc(p, pl), str_alloc("")};)" EOL
    R"(    } else {)" EOL
    R"(      size_t ei = e - p;)" EOL
    R"(      d[i] = (struct __THE_1_pair_strMSstrME) {str_calloc(p, ei), (ei == pl - 1) ? str_alloc("") : str_calloc(&p[ei + 1], pl - ei - 1)};)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  return (struct __THE_1_map_strMSstrME) {d, l, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libString", "str_alloc", "str_calloc", "varEnviron"},
    {"map_str_str", "pair_str_str"},
    false
  }},

  {"process_exit", {
    false,
    "process_exit",
    "void process_exit (unsigned char, int32_t);" EOL,
    R"(void process_exit (unsigned char o1, int32_t i) {)" EOL
    R"(  exit(o1 == 1 ? i : EXIT_SUCCESS);)" EOL
    R"(})" EOL,
    {"libStdint", "libStdlib"},
    {},
    false
  }},

  {"process_getgid", {
    false,
    "process_getgid",
    "int32_t process_getgid ();" EOL,
    R"(int32_t process_getgid () {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    return 0;)" EOL
    R"(  #else)" EOL
    R"(    return getgid();)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libStdint", "libUnistd"},
    {},
    false
  }},

  {"process_getuid", {
    false,
    "process_getuid",
    "int32_t process_getuid ();" EOL,
    R"(int32_t process_getuid () {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    return 0;)" EOL
    R"(  #else)" EOL
    R"(    return getuid();)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libStdint", "libUnistd"},
    {},
    false
  }},

  {"process_home", {
    false,
    "process_home",
    "struct str process_home (err_state_t *, int, int);" EOL,
    R"(struct str process_home (err_state_t *fn_err_state, int line, int col) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    char r[0xFFFF];)" EOL
    R"(    if (GetEnvironmentVariable("USERPROFILE", r, 0xFFFF) == 0) {)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("environment variable `USERPROFILE` is not set"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(  #else)" EOL
    R"(    char *r = getenv("HOME");)" EOL
    R"(    if (r == NULL) {)" EOL
    R"(      r = getpwuid(getuid())->pw_dir;)" EOL
    R"(    })" EOL
    R"(  #endif)" EOL
    R"(  return str_alloc(r);)" EOL
    R"(})" EOL,
    {"definitions", "error_assign", "libPwd", "libSetJmp", "libStddef", "libStdlib", "libUnistd", "libWindows", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"process_runSync", {
    false,
    "process_runSync",
    "struct buffer process_runSync (err_state_t *, int, int, struct str);" EOL,
    R"(struct buffer process_runSync (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  char *c = str_cstr(s);)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    FILE *f = _popen(c, "r");)" EOL
    R"(  #else)" EOL
    R"(    FILE *f = popen(c, "r");)" EOL
    R"(  #endif)" EOL
    R"(  if (f == NULL) {)" EOL
    R"(    const char *fmt = "failed to run process `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    str_free((struct str) s);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  unsigned char *d = NULL;)" EOL
    R"(  unsigned char b[4096];)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  size_t y;)" EOL
    R"(  while ((y = fread(b, 1, sizeof(b), f)) > 0) {)" EOL
    R"(    d = re_alloc(d, l + y);)" EOL
    R"(    memcpy(&d[l], b, y);)" EOL
    R"(    l += y;)" EOL
    R"(  })" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    int e = _pclose(f);)" EOL
    R"(  #else)" EOL
    R"(    int e = pclose(f) / 256;)" EOL
    R"(  #endif)" EOL
    R"(  if (e != 0) {)" EOL
    R"(    const char *fmt = "process `%s` exited with exit code %d";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c, e);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c, e);)" EOL
    R"(    free(c);)" EOL
    R"(    str_free((struct str) s);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  str_free((struct str) s);)" EOL
    R"(  return (struct buffer) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "error_assign", "libSetJmp", "libStddef", "libStdio", "libStdlib", "libString", "re_alloc", "str_cstr", "str_free", "typeBuffer", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"random_randomFloat", {
    false,
    "random_randomFloat",
    "double random_randomFloat (err_state_t *, int, int, unsigned char, double, unsigned char, double);" EOL,
    R"(double random_randomFloat (err_state_t *fn_err_state, int line, int col, unsigned char o1, double n1, unsigned char o2, double n2) {)" EOL
    R"(  double a = o1 == 0 ? 0 : n1;)" EOL
    R"(  double b = o2 == 0 ? 1 : n2;)" EOL
    R"(  unsigned char d[4];)" EOL
    R"(  if (RAND_bytes(d, 4) != 1) {)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to generate random bytes"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  uint32_t r = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];)" EOL
    R"(  return a + r / ((double) UINT32_MAX / (b - a));)" EOL
    R"(})" EOL,
    {"error_assign", "libOpensslRand", "libSetJmp", "libStddef", "libStdint", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"random_randomInt", {
    false,
    "random_randomInt",
    "int32_t random_randomInt (err_state_t *, int, int, unsigned char, int32_t, unsigned char, int32_t);" EOL,
    R"(int32_t random_randomInt (err_state_t *fn_err_state, int line, int col, unsigned char o1, int32_t n1, unsigned char o2, int32_t n2) {)" EOL
    R"(  int64_t a = o1 == 0 ? INT32_MIN : n1;)" EOL
    R"(  int64_t b = o2 == 0 ? INT32_MAX : n2;)" EOL
    R"(  unsigned char d[4];)" EOL
    R"(  if (RAND_bytes(d, 4) != 1) {)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to generate random bytes"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  uint32_t r = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];)" EOL
    R"(  return (int32_t) (a + r / (UINT32_MAX / (b - a + 1) + 1));)" EOL
    R"(})" EOL,
    {"error_assign", "libOpensslRand", "libSetJmp", "libStddef", "libStdint", "str_alloc", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"random_randomStr", {
    false,
    "random_randomStr",
    "struct str random_randomStr (err_state_t *, int, int, unsigned char, struct str, unsigned char, int32_t);" EOL,
    R"(struct str random_randomStr (err_state_t *fn_err_state, int line, int col, unsigned char o1, struct str n1, unsigned char o2, int32_t n2) {)" EOL
    R"(  struct str a = o1 == 0 ? str_alloc("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") : n1;)" EOL
    R"(  int32_t l = o2 == 0 ? 32 : n2;)" EOL
    R"(  unsigned char *d = alloc(l);)" EOL
    R"(  if (RAND_bytes(d, l) != 1) {)" EOL
    R"(    free(d);)" EOL
    R"(    str_free(a);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to generate random bytes"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  for (size_t i = 0; i < l; i++) d[i] = a.d[(size_t) round((double) d[i] / 0xFF * (double) (a.l - 1))];)" EOL
    R"(  struct str r = str_calloc((void *) d, l);)" EOL
    R"(  free(d);)" EOL
    R"(  str_free(a);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libMath", "libOpensslRand", "libSetJmp", "libStddef", "libStdint", "libStdlib", "str_alloc", "str_calloc", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"re_alloc", {
    false,
    "re_alloc",
    "void *re_alloc (void *, size_t);" EOL,
    R"(void *re_alloc (void *n1, size_t n2) {)" EOL
    R"(  void *r = realloc(n1, n2);)" EOL
    R"(  if (r == NULL) error_alloc(&err_state, n2);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"error_alloc", "libStddef", "libStdlib", "varErrState"},
    {},
    false
  }},

  {"request_close", {
    false,
    "request_close",
    "void request_close (struct request_Request **);" EOL,
    R"(void request_close (struct request_Request **r) {)" EOL
    R"(  struct request *req = (void *) *r;)" EOL
    R"(  if (req->ssl != NULL) {)" EOL
    R"(    SSL_CTX_free(req->ctx);)" EOL
    R"(    SSL_free(req->ssl);)" EOL
    R"(  } else if (req->fd != 0) {)" EOL
    R"(    #ifdef THE_OS_WINDOWS)" EOL
    R"(      closesocket(req->fd);)" EOL
    R"(    #else)" EOL
    R"(      close(req->fd);)" EOL
    R"(    #endif)" EOL
    R"(  })" EOL
    R"(  req->fd = 0;)" EOL
    R"(  req->ctx = NULL;)" EOL
    R"(  req->ssl = NULL;)" EOL
    R"(})" EOL,
    {"definitions", "libOpensslSsl", "libStddef", "libUnistd", "libWinsock2", "typeRequest"},
    {"request_Request"},
    false
  }},

  {"request_open", {
    false,
    "request_open",
    "struct request_Request *request_open (err_state_t *, int, int, struct str, struct str, unsigned char, struct buffer, unsigned char, struct __THE_1_array_request_Header);" EOL,
    R"(struct request_Request *request_open (err_state_t *fn_err_state, int line, int col, struct str method, struct str u, unsigned char o1, struct buffer data, unsigned char o2, struct __THE_1_array_request_Header headers) {)" EOL
    R"(  struct url_URL *url = url_parse(fn_err_state, line, col, u);)" EOL
    R"(  if ()" EOL
    R"(    !(url->__THE_0_protocol.l == 5 && memcmp(url->__THE_0_protocol.d, "http:", 5) == 0) &&)" EOL
    R"(    !(url->__THE_0_protocol.l == 6 && memcmp(url->__THE_0_protocol.d, "https:", 6) == 0))" EOL
    R"(  ) {)" EOL
    R"(    char *protocol = str_cstr(url->__THE_0_protocol);)" EOL
    R"(    const char *fmt = "can't perform request with protocol `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, protocol);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, protocol);)" EOL
    R"(    free(protocol);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto request_open_cleanup1;)" EOL
    R"(  } else if (url->__THE_0_port.l >= 6) {)" EOL
    R"(    char *port = str_cstr(url->__THE_0_port);)" EOL
    R"(    const char *fmt = "invalid port `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, port);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, port);)" EOL
    R"(    free(port);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto request_open_cleanup1;)" EOL
    R"(  })" EOL
    R"(  char port[6];)" EOL
    R"(  if (url->__THE_0_port.l != 0) {)" EOL
    R"(    memcpy(port, url->__THE_0_port.d, url->__THE_0_port.l);)" EOL
    R"(    port[url->__THE_0_port.l] = '\0';)" EOL
    R"(    unsigned long p = strtoul(port, NULL, 10);)" EOL
    R"(    if (p > 65535) {)" EOL
    R"(      const char *fmt = "invalid port `%s`";)" EOL
    R"(      size_t z = snprintf(NULL, 0, fmt, port);)" EOL
    R"(      char *d = alloc(z + 1);)" EOL
    R"(      sprintf(d, fmt, port);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto request_open_cleanup1;)" EOL
    R"(    })" EOL
    R"(  } else {)" EOL
    R"(    memcpy(port, url->__THE_0_protocol.l == 6 ? "443" : "80", url->__THE_0_protocol.l == 6 ? 3 : 2);)" EOL
    R"(    port[url->__THE_0_protocol.l == 6 ? 3 : 2] = '\0';)" EOL
    R"(  })" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    if (!lib_ws2_init) {)" EOL
    R"(      WSADATA w;)" EOL
    R"(      if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {)" EOL
    R"(        error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to initialize use of Windows Sockets DLL"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(        goto request_open_cleanup1;)" EOL
    R"(      })" EOL
    R"(      lib_ws2_init = true;)" EOL
    R"(    })" EOL
    R"(  #endif)" EOL
    R"(  char *hostname = str_cstr(url->__THE_0_hostname);)" EOL
    R"(  struct addrinfo *addr = NULL;)" EOL
    R"(  struct addrinfo hints;)" EOL
    R"(  memset(&hints, 0, sizeof(hints));)" EOL
    R"(  hints.ai_family = AF_INET;)" EOL
    R"(  hints.ai_socktype = SOCK_STREAM;)" EOL
    R"(  hints.ai_protocol = IPPROTO_TCP;)" EOL
    R"(  if (getaddrinfo(hostname, port, &hints, &addr) != 0) {)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to resolve hostname address"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto request_open_cleanup2;)" EOL
    R"(  })" EOL
    R"(  unsigned char req_free = 0;)" EOL
    R"(  struct request *req = alloc(sizeof(struct request));)" EOL
    R"(  req->fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);)" EOL
    R"(  req->ctx = NULL;)" EOL
    R"(  req->ssl = NULL;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    bool socket_res = req->fd != INVALID_SOCKET;)" EOL
    R"(  #else)" EOL
    R"(    bool socket_res = req->fd != -1;)" EOL
    R"(  #endif)" EOL
    R"(  if (!socket_res) {)" EOL
    R"(    req_free = 1;)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to create socket"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto request_open_cleanup3;)" EOL
    R"(  })" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    bool connect_res = connect(req->fd, addr->ai_addr, (int) addr->ai_addrlen) != SOCKET_ERROR;)" EOL
    R"(  #else)" EOL
    R"(    bool connect_res = connect(req->fd, addr->ai_addr, addr->ai_addrlen) != -1;)" EOL
    R"(  #endif)" EOL
    R"(  if (!connect_res) {)" EOL
    R"(    char *origin = str_cstr(url->__THE_0_origin);)" EOL
    R"(    const char *fmt = "failed to connect to `%s`";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, origin);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, origin);)" EOL
    R"(    free(origin);)" EOL
    R"(    req_free = 1;)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    goto request_open_cleanup4;)" EOL
    R"(  })" EOL
    R"(  if (strcmp(port, "443") == 0) {)" EOL
    R"(    if (!lib_openssl_init) {)" EOL
    R"(      SSL_library_init();)" EOL
    R"(      lib_openssl_init = true;)" EOL
    R"(    })" EOL
    R"(    req->ctx = SSL_CTX_new(TLS_client_method());)" EOL
    R"(    if (req->ctx == NULL) {)" EOL
    R"(      req_free = 1;)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to create SSL context"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto request_open_cleanup4;)" EOL
    R"(    })" EOL
    R"(    req->ssl = SSL_new(req->ctx);)" EOL
    R"(    SSL_set_fd(req->ssl, (int) req->fd);)" EOL
    R"(    SSL_set_tlsext_host_name(req->ssl, hostname);)" EOL
    R"(    if (SSL_connect(req->ssl) != 1) {)" EOL
    R"(      req_free = 1;)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to connect to socket with SSL"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto request_open_cleanup5;)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  char *req_headers = request_stringifyHeaders(headers, url, data);)" EOL
    R"(  char *req_method = str_cstr(method);)" EOL
    R"(  char *req_path = str_cstr(url->__THE_0_path);)" EOL
    R"(  char *fmt = "%s %s HTTP/1.1\r\n%s\r\n";)" EOL
    R"(  size_t req_len = snprintf(NULL, 0, fmt, req_method, req_path, req_headers);)" EOL
    R"(  char *request = alloc(req_len + (data.l == 0 ? 0 : data.l + 2) + 1);)" EOL
    R"(  sprintf(request, fmt, req_method, req_path, req_headers);)" EOL
    R"(  if (data.l != 0) {)" EOL
    R"(    memcpy(&request[req_len], data.d, data.l);)" EOL
    R"(    req_len += data.l;)" EOL
    R"(    memcpy(&request[req_len], "\r\n", 3);)" EOL
    R"(    req_len += 2;)" EOL
    R"(  })" EOL
    R"(  size_t y = 0;)" EOL
    R"(  while (y < req_len) {)" EOL
    R"(    int z = req->ssl == NULL ? (int) send(req->fd, &request[y], req_len - y, 0) : SSL_write(req->ssl, &request[y], (int) (req_len - y));)" EOL
    R"(    if (z < 0) {)" EOL
    R"(      req_free = 1;)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to write to socket"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      goto request_open_cleanup6;)" EOL
    R"(    })" EOL
    R"(    y += (size_t) z;)" EOL
    R"(  })" EOL
    R"(request_open_cleanup6:)" EOL
    R"(  free(request);)" EOL
    R"(  free(req_path);)" EOL
    R"(  free(req_method);)" EOL
    R"(  free(req_headers);)" EOL
    R"(request_open_cleanup5:)" EOL
    R"(  if (req_free == 1 && req->ssl != NULL) {)" EOL
    R"(    SSL_free(req->ssl);)" EOL
    R"(    SSL_CTX_free(req->ctx);)" EOL
    R"(    goto request_open_cleanup3;)" EOL
    R"(  })" EOL
    R"(request_open_cleanup4:)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    if (req_free == 1) closesocket(req->fd);)" EOL
    R"(  #else)" EOL
    R"(    if (req_free == 1) close(req->fd);)" EOL
    R"(  #endif)" EOL
    R"(request_open_cleanup3:)" EOL
    R"(  if (req_free == 1) free(req);)" EOL
    R"(  freeaddrinfo(addr);)" EOL
    R"(request_open_cleanup2:)" EOL
    R"(  free(hostname);)" EOL
    R"(request_open_cleanup1:)" EOL
    R"(  url_URL_free(url);)" EOL
    R"(  __THE_1_array_request_Header_free(headers);)" EOL
    R"(  buffer_free(data);)" EOL
    R"(  str_free(method);)" EOL
    R"(  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  return (struct request_Request *) req;)" EOL
    R"(})" EOL,
    {"alloc", "buffer_free", "definitions", "error_assign", "libNetdb", "libNetinetIn", "libOpensslSsl", "libSetJmp", "libStdbool", "libStddef", "libStdio", "libStdlib", "libString", "libSysSocket", "libUnistd", "libWinWs2tcpip", "libWindows", "libWinsock2", "request_stringifyHeaders", "str_alloc", "str_cstr", "str_free", "typeBuffer", "typeErrState", "typeRequest", "typeStr", "url_parse", "varLibOpensslInit", "varLibWs2Init"},
    {"TYPE_error_Error", "array_request_Header", "array_request_Header_free", "error_Error_alloc", "error_Error_free", "request_Request", "url_URL", "url_URL_free"},
    true
  }},

  {"request_read", {
    false,
    "request_read",
    "struct request_Response *request_read (err_state_t *, int, int, struct request_Request **);" EOL,
    R"(struct request_Response *request_read (err_state_t *fn_err_state, int line, int col, struct request_Request **r) {)" EOL
    R"(  struct request *req = (void *) *r;)" EOL
    R"(  unsigned char b[1024];)" EOL
    R"(  struct buffer data = {NULL, 0};)" EOL
    R"(  while (1) {)" EOL
    R"(    int y = req->ssl == NULL ? (int) recv(req->fd, b, sizeof(b), 0) : SSL_read(req->ssl, b, sizeof(b));)" EOL
    R"(    if (y < 0) {)" EOL
    R"(      free(data.d);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("failed to read from socket"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    } else if (y == 0 && data.l != 0) {)" EOL
    R"(      break;)" EOL
    R"(    } else if (y == 0) {)" EOL
    R"(      continue;)" EOL
    R"(    } else if (data.l == 0) {)" EOL
    R"(      (req->ssl == NULL ? shutdown(req->fd, 1) : SSL_shutdown(req->ssl));)" EOL
    R"(    })" EOL
    R"(    data.d = re_alloc(data.d, data.l + y);)" EOL
    R"(    memcpy(&data.d[data.l], b, y);)" EOL
    R"(    data.l += y;)" EOL
    R"(  })" EOL
    R"(  size_t i;)" EOL
    R"(  if (data.l > 8 && (memcmp(data.d, "HTTP/1.0 ", 9) == 0 || memcmp(data.d, "HTTP/1.1 ", 9) == 0)) {)" EOL
    R"(    i = 9;)" EOL
    R"(  } else if (data.l > 6 && (memcmp(data.d, "HTTP/2 ", 7) == 0 || memcmp(data.d, "HTTP/3 ", 7) == 0)) {)" EOL
    R"(    i = 7;)" EOL
    R"(  } else {)" EOL
    R"(    free(data.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid response HTTP version"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  size_t status_start = i;)" EOL
    R"(  while (i < data.l && isdigit(data.d[i])) i++;)" EOL
    R"(  if (status_start == i) {)" EOL
    R"(    free(data.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid response HTTP status code"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  size_t status_len = i - status_start;)" EOL
    R"(  char *status = alloc(status_len + 1);)" EOL
    R"(  memcpy(status, &data.d[status_start], status_len);)" EOL
    R"(  status[status_len] = '\0';)" EOL
    R"(  int32_t status_code = (int32_t) strtoul(status, NULL, 10);)" EOL
    R"(  free(status);)" EOL
    R"(  while (i < data.l) {)" EOL
    R"(    if (data.d[i] == '\r' && i + 1 < data.l && data.d[i + 1] == '\n') {)" EOL
    R"(      i += 2;)" EOL
    R"(      break;)" EOL
    R"(    })" EOL
    R"(    i++;)" EOL
    R"(  })" EOL
    R"(  struct __THE_1_array_request_Header headers = {NULL, 0};)" EOL
    R"(  while (true) {)" EOL
    R"(    if (data.d[i] == '\r' && i + 1 < data.l && data.d[i + 1] == '\n') {)" EOL
    R"(      i += 2;)" EOL
    R"(      break;)" EOL
    R"(    })" EOL
    R"(    size_t header_name_start = i;)" EOL
    R"(    size_t header_name_end = 0;)" EOL
    R"(    size_t header_value_start = 0;)" EOL
    R"(    size_t header_value_end = 0;)" EOL
    R"(    while (i < data.l) {)" EOL
    R"(      if (data.d[i] == '\r' && i + 1 < data.l && data.d[i + 1] == '\n') {)" EOL
    R"(        header_value_end = i;)" EOL
    R"(        i += 2;)" EOL
    R"(        break;)" EOL
    R"(      } else if (data.d[i] == ':' && header_name_end == 0) {)" EOL
    R"(        header_name_end = i;)" EOL
    R"(        i++;)" EOL
    R"(        while (i < data.l && data.d[i] == ' ') i++;)" EOL
    R"(        header_value_start = i;)" EOL
    R"(      })" EOL
    R"(      i++;)" EOL
    R"(    })" EOL
    R"(    struct str header_name;)" EOL
    R"(    header_name.l = header_name_end - header_name_start;)" EOL
    R"(    header_name.d = alloc(header_name.l);)" EOL
    R"(    memcpy(header_name.d, &data.d[header_name_start], header_name.l);)" EOL
    R"(    for (size_t j = 0; j < header_name.l; j++) header_name.d[j] = (char) tolower(header_name.d[j]);)" EOL
    R"(    struct str header_value;)" EOL
    R"(    header_value.l = header_value_end - header_value_start;)" EOL
    R"(    header_value.d = alloc(header_value.l);)" EOL
    R"(    memcpy(header_value.d, &data.d[header_value_start], header_value.l);)" EOL
    R"(    headers.d = re_alloc(headers.d, (headers.l + 1) * sizeof(struct request_Header *));)" EOL
    R"(    headers.d[headers.l] = request_Header_alloc(header_name, header_value);)" EOL
    R"(    headers.l++;)" EOL
    R"(  })" EOL
    R"(  for (size_t j = 0; j < headers.l; j++) {)" EOL
    R"(    for (size_t k = j + 1; k < headers.l; k++) {)" EOL
    R"(      size_t count = headers.d[j]->__THE_0_name.l > headers.d[k]->__THE_0_name.l ? headers.d[k]->__THE_0_name.l : headers.d[j]->__THE_0_name.l;)" EOL
    R"(      if (memcmp(headers.d[j]->__THE_0_name.d, headers.d[k]->__THE_0_name.d, count) > 0) {)" EOL
    R"(        struct request_Header *a = headers.d[j];)" EOL
    R"(        headers.d[j] = headers.d[k];)" EOL
    R"(        headers.d[k] = a;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  if (data.l - i > 0) {)" EOL
    R"(    data.l = data.l - i;)" EOL
    R"(    memmove(data.d, &data.d[i], data.l);)" EOL
    R"(    data.d = re_alloc(data.d, data.l);)" EOL
    R"(  } else {)" EOL
    R"(    free(data.d);)" EOL
    R"(    data.d = NULL;)" EOL
    R"(    data.l = 0;)" EOL
    R"(  })" EOL
    R"(  return request_Response_alloc(data, status_code, headers);)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libCtype", "libOpensslSsl", "libSetJmp", "libStdbool", "libStddef", "libStdint", "libStdlib", "libString", "libSysSocket", "libWinsock2", "re_alloc", "str_alloc", "typeBuffer", "typeErrState", "typeRequest", "typeStr"},
    {"TYPE_error_Error", "array_request_Header", "error_Error_alloc", "error_Error_free", "request_Header", "request_Header_alloc", "request_Request", "request_Response", "request_Response_alloc"},
    true
  }},

  {"request_stringifyHeaders", {
    false,
    "request_stringifyHeaders",
    "char *request_stringifyHeaders (struct __THE_1_array_request_Header, struct url_URL *, struct buffer);" EOL,
    R"(char *request_stringifyHeaders (struct __THE_1_array_request_Header headers, struct url_URL *url, struct buffer data) {)" EOL
    R"(  bool has_content_length = false;)" EOL
    R"(  bool has_host = false;)" EOL
    R"(  char *d = NULL;)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  for (size_t i = 0; i < headers.l; i++) {)" EOL
    R"(    struct request_Header *h = headers.d[i];)" EOL
    R"(    char *name = str_cstr(h->__THE_0_name);)" EOL
    R"(    for (size_t j = 0; j < h->__THE_0_name.l; j++) name[j] = (char) tolower(name[j]);)" EOL
    R"(    if (strcmp(name, "content-length") == 0) has_content_length = true;)" EOL
    R"(    else if (strcmp(name, "host") == 0) has_host = true;)" EOL
    R"(    free(name);)" EOL
    R"(    d = re_alloc(d, l + h->__THE_0_name.l + 2 + h->__THE_0_value.l + 3);)" EOL
    R"(    memcpy(&d[l], h->__THE_0_name.d, h->__THE_0_name.l);)" EOL
    R"(    memcpy(&d[l + h->__THE_0_name.l], ": ", 2);)" EOL
    R"(    memcpy(&d[l + h->__THE_0_name.l + 2], h->__THE_0_value.d, h->__THE_0_value.l);)" EOL
    R"(    memcpy(&d[l + h->__THE_0_name.l + 2 + h->__THE_0_value.l], "\r\n", 3);)" EOL
    R"(    l += h->__THE_0_name.l + 2 + h->__THE_0_value.l + 2;)" EOL
    R"(  })" EOL
    R"(  if (!has_host) {)" EOL
    R"(    char *h = str_cstr(url->__THE_0_hostname);)" EOL
    R"(    size_t z = snprintf(NULL, 0, "Host: %s\r\n", h);)" EOL
    R"(    d = re_alloc(d, l + z + 1);)" EOL
    R"(    sprintf(&d[l], "Host: %s\r\n", h);)" EOL
    R"(    l += z;)" EOL
    R"(    free(h);)" EOL
    R"(  })" EOL
    R"(  if (!has_content_length) {)" EOL
    R"(    size_t z = snprintf(NULL, 0, "Content-Length: %zu\r\n", data.l);)" EOL
    R"(    d = re_alloc(d, l + z + 1);)" EOL
    R"(    sprintf(&d[l], "Content-Length: %zu\r\n", data.l);)" EOL
    R"(    l += z;)" EOL
    R"(  })" EOL
    R"(  return d;)" EOL
    R"(})" EOL,
    {"libCtype", "libStdbool", "libStddef", "libStdio", "libStdlib", "libString", "re_alloc", "str_cstr", "typeBuffer"},
    {"array_request_Header", "request_Header", "url_URL"},
    false
  }},

  {"str_alloc", {
    false,
    "str_alloc",
    "struct str str_alloc (const char *);" EOL,
    R"(struct str str_alloc (const char *r) {)" EOL
    R"(  size_t l = strlen(r);)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, r, l);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libString", "typeStr"},
    {},
    false
  }},

  {"str_at", {
    false,
    "str_at",
    "char *str_at (err_state_t *, int, int, struct str, int32_t);" EOL,
    R"(char *str_at (err_state_t *fn_err_state, int line, int col, struct str s, int32_t i) {)" EOL
    R"(  if ((i >= 0 && i >= s.l) || (i < 0 && i < -((int32_t) s.l))) {)" EOL
    R"(    const char *fmt = "index %" PRId32 " out of string bounds";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, i);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, i);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  return i < 0 ? &s.d[s.l + i] : &s.d[i];)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_calloc", {
    false,
    "str_calloc",
    "struct str str_calloc (const char *, size_t);" EOL,
    R"(struct str str_calloc (const char *r, size_t l) {)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, r, l);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libString", "typeStr"},
    {},
    false
  }},

  {"str_concat_cstr", {
    false,
    "str_concat_cstr",
    "struct str str_concat_cstr (struct str, const char *);" EOL,
    R"(struct str str_concat_cstr (struct str s, const char *r) {)" EOL
    R"(  size_t l = s.l + strlen(r);)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, s.d, s.l);)" EOL
    R"(  memcpy(&d[s.l], r, l - s.l);)" EOL
    R"(  free(s.d);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_concat_str", {
    false,
    "str_concat_str",
    "struct str str_concat_str (struct str, struct str);" EOL,
    R"(struct str str_concat_str (struct str s1, struct str s2) {)" EOL
    R"(  size_t l = s1.l + s2.l;)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  memcpy(d, s1.d, s1.l);)" EOL
    R"(  memcpy(&d[s1.l], s2.d, s2.l);)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_contains", {
    false,
    "str_contains",
    "bool str_contains (struct str, struct str);" EOL,
    R"(bool str_contains (struct str self, struct str n1) {)" EOL
    R"(  bool r = n1.l == 0;)" EOL
    R"(  if (!r && self.l == n1.l) {)" EOL
    R"(    r = memcmp(self.d, n1.d, n1.l) == 0;)" EOL
    R"(  } else if (!r && self.l > n1.l) {)" EOL
    R"(    for (size_t i = 0; i < self.l - n1.l; i++) {)" EOL
    R"(      if (memcmp(&self.d[i], n1.d, n1.l) == 0) {)" EOL
    R"(        r = true;)" EOL
    R"(        break;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  free(self.d);)" EOL
    R"(  free(n1.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStddef", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_copy", {
    false,
    "str_copy",
    "struct str str_copy (const struct str);" EOL,
    R"(struct str str_copy (const struct str s) {)" EOL
    R"(  char *d = alloc(s.l);)" EOL
    R"(  memcpy(d, s.d, s.l);)" EOL
    R"(  return (struct str) {d, s.l};)" EOL
    R"(})" EOL,
    {"alloc", "libString", "typeStr"},
    {},
    false
  }},

  {"str_cstr", {
    false,
    "str_cstr",
    "char *str_cstr (const struct str);" EOL,
    R"(char *str_cstr (const struct str s) {)" EOL
    R"(  char *d = alloc(s.l + 1);)" EOL
    R"(  memcpy(d, s.d, s.l);)" EOL
    R"(  d[s.l] = '\0';)" EOL
    R"(  return d;)" EOL
    R"(})" EOL,
    {"alloc", "libString", "typeStr"},
    {},
    false
  }},

  {"str_empty", {
    false,
    "str_empty",
    "bool str_empty (struct str);" EOL,
    R"(bool str_empty (struct str s) {)" EOL
    R"(  bool r = s.l == 0;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "typeStr"},
    {},
    false
  }},

  {"str_eq_cstr", {
    false,
    "str_eq_cstr",
    "bool str_eq_cstr (struct str, const char *);" EOL,
    R"(bool str_eq_cstr (struct str s, const char *r) {)" EOL
    R"(  bool d = s.l == strlen(r) && memcmp(s.d, r, s.l) == 0;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return d;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_eq_str", {
    false,
    "str_eq_str",
    "bool str_eq_str (struct str, struct str);" EOL,
    R"(bool str_eq_str (struct str s1, struct str s2) {)" EOL
    R"(  bool r = s1.l == s2.l && memcmp(s1.d, s2.d, s1.l) == 0;)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_escape", {
    false,
    "str_escape",
    "struct str str_escape (const struct str);" EOL,
    R"(struct str str_escape (const struct str s) {)" EOL
    R"(  char *d = alloc(s.l);)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  for (size_t i = 0; i < s.l; i++) {)" EOL
    R"(    char c = s.d[i];)" EOL
    R"(    if (c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '"') {)" EOL
    R"(      if (l + 2 > s.l) d = re_alloc(d, l + 2);)" EOL
    R"(      d[l++] = '\\';)" EOL
    R"(      if (c == '\f') d[l++] = 'f';)" EOL
    R"(      else if (c == '\n') d[l++] = 'n';)" EOL
    R"(      else if (c == '\r') d[l++] = 'r';)" EOL
    R"(      else if (c == '\t') d[l++] = 't';)" EOL
    R"(      else if (c == '\v') d[l++] = 'v';)" EOL
    R"(      else if (c == '"') d[l++] = '"';)" EOL
    R"(      continue;)" EOL
    R"(    })" EOL
    R"(    if (l + 1 > s.l) d = re_alloc(d, l + 1);)" EOL
    R"(    d[l++] = c;)" EOL
    R"(  })" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "re_alloc", "typeStr"},
    {},
    false
  }},

  {"str_find", {
    false,
    "str_find",
    "int32_t str_find (struct str, struct str);" EOL,
    R"(int32_t str_find (struct str s1, struct str s2) {)" EOL
    R"(  int32_t r = -1;)" EOL
    R"(  for (size_t i = 0; i < s1.l; i++) {)" EOL
    R"(    if (memcmp(&s1.d[i], s2.d, s2.l) == 0) {)" EOL
    R"(      r = (int32_t) i;)" EOL
    R"(      break;)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStddef", "libStdint", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_free", {
    false,
    "str_free",
    "void str_free (struct str);" EOL,
    R"(void str_free (struct str s) {)" EOL
    R"(  free(s.d);)" EOL
    R"(})" EOL,
    {"libStdlib", "typeStr"},
    {},
    false
  }},

  {"str_ge", {
    false,
    "str_ge",
    "bool str_ge (struct str, struct str);" EOL,
    R"(bool str_ge (struct str s1, struct str s2) {)" EOL
    R"(  bool r = memcmp(s1.d, s2.d, s1.l > s2.l ? s1.l : s2.l) >= 0;)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_gt", {
    false,
    "str_gt",
    "bool str_gt (struct str, struct str);" EOL,
    R"(bool str_gt (struct str s1, struct str s2) {)" EOL
    R"(  bool r = memcmp(s1.d, s2.d, s1.l > s2.l ? s1.l : s2.l) > 0;)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_le", {
    false,
    "str_le",
    "bool str_le (struct str, struct str);" EOL,
    R"(bool str_le (struct str s1, struct str s2) {)" EOL
    R"(  bool r = memcmp(s1.d, s2.d, s1.l > s2.l ? s1.l : s2.l) <= 0;)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_len", {
    false,
    "str_len",
    "size_t str_len (struct str);" EOL,
    R"(size_t str_len (struct str s) {)" EOL
    R"(  size_t l = s.l;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return l;)" EOL
    R"(})" EOL,
    {"libStddef", "libStdlib", "typeStr"},
    {},
    false
  }},

  {"str_lines", {
    false,
    "str_lines",
    "struct __THE_1_array_str str_lines (struct str, unsigned char, bool);" EOL,
    R"(struct __THE_1_array_str str_lines (struct str s, unsigned char o1, bool n1) {)" EOL
    R"(  bool k = o1 == 0 ? false : n1;)" EOL
    R"(  struct str *r = NULL;)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  if (s.l != 0) {)" EOL
    R"(    char *d = alloc(s.l);)" EOL
    R"(    size_t i = 0;)" EOL
    R"(    for (size_t j = 0; j < s.l; j++) {)" EOL
    R"(      char c = s.d[j];)" EOL
    R"(      if (c == '\r' || c == '\n') {)" EOL
    R"(        if (k) d[i++] = c;)" EOL
    R"(        if (c == '\r' && j + 1 < s.l && s.d[j + 1] == '\n') {)" EOL
    R"(          j++;)" EOL
    R"(          if (k) d[i++] = s.d[j];)" EOL
    R"(        })" EOL
    R"(        char *a = alloc(i);)" EOL
    R"(        memcpy(a, d, i);)" EOL
    R"(        r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(        r[l - 1] = (struct str) {a, i};)" EOL
    R"(        i = 0;)" EOL
    R"(      } else {)" EOL
    R"(        d[i++] = c;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(    if (i != 0) {)" EOL
    R"(      char *a = alloc(i);)" EOL
    R"(      memcpy(a, d, i);)" EOL
    R"(      r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(      r[l - 1] = (struct str) {a, i};)" EOL
    R"(    })" EOL
    R"(    free(d);)" EOL
    R"(  })" EOL
    R"(  free(s.d);)" EOL
    R"(  return (struct __THE_1_array_str) {r, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStdbool", "libStddef", "libStdlib", "libString", "re_alloc", "typeStr"},
    {"array_str"},
    false
  }},

  {"str_lower", {
    false,
    "str_lower",
    "struct str str_lower (struct str);" EOL,
    R"(struct str str_lower (struct str s) {)" EOL
    R"(  if (s.l != 0) {)" EOL
    R"(    for (size_t i = 0; i < s.l; i++) s.d[i] = (char) tolower(s.d[i]);)" EOL
    R"(  })" EOL
    R"(  return s;)" EOL
    R"(})" EOL,
    {"libCtype", "libStddef", "typeStr"},
    {},
    false
  }},

  {"str_lowerFirst", {
    false,
    "str_lowerFirst",
    "struct str str_lowerFirst (struct str);" EOL,
    R"(struct str str_lowerFirst (struct str s) {)" EOL
    R"(  if (s.l != 0) s.d[0] = (char) tolower(s.d[0]);)" EOL
    R"(  return s;)" EOL
    R"(})" EOL,
    {"libCtype", "typeStr"},
    {},
    false
  }},

  {"str_lt", {
    false,
    "str_lt",
    "bool str_lt (struct str, struct str);" EOL,
    R"(bool str_lt (struct str s1, struct str s2) {)" EOL
    R"(  bool r = memcmp(s1.d, s2.d, s1.l > s2.l ? s1.l : s2.l) < 0;)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_ne_cstr", {
    false,
    "str_ne_cstr",
    "bool str_ne_cstr (struct str, const char *);" EOL,
    R"(bool str_ne_cstr (struct str s, const char *c) {)" EOL
    R"(  bool r = s.l != strlen(c) || memcmp(s.d, c, s.l) != 0;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_ne_str", {
    false,
    "str_ne_str",
    "bool str_ne_str (struct str, struct str);" EOL,
    R"(bool str_ne_str (struct str s1, struct str s2) {)" EOL
    R"(  bool r = s1.l != s2.l || memcmp(s1.d, s2.d, s1.l) != 0;)" EOL
    R"(  free(s1.d);)" EOL
    R"(  free(s2.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "libString", "typeStr"},
    {},
    false
  }},

  {"str_not", {
    false,
    "str_not",
    "bool str_not (struct str);" EOL,
    R"(bool str_not (struct str s) {)" EOL
    R"(  bool r = s.l == 0;)" EOL
    R"(  free(s.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"libStdbool", "libStdlib", "typeStr"},
    {},
    false
  }},

  {"str_realloc", {
    false,
    "str_realloc",
    "struct str str_realloc (struct str, struct str);" EOL,
    R"(struct str str_realloc (struct str s1, struct str s2) {)" EOL
    R"(  free(s1.d);)" EOL
    R"(  return s2;)" EOL
    R"(})" EOL,
    {"libStdlib", "typeStr"},
    {},
    false
  }},

  {"str_replace", {
    false,
    "str_replace",
    "struct str str_replace (struct str, struct str, struct str, unsigned char, int32_t);" EOL,
    R"(struct str str_replace (struct str self, struct str n1, struct str n2, unsigned char _o3, int32_t n3) {)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  char *d = NULL;)" EOL
    R"(  int32_t k = 0;)" EOL
    R"(  if (n1.l == 0 && n2.l > 0) {)" EOL
    R"(    l = self.l + (n3 > 0 && n3 <= self.l ? n3 : self.l + 1) * n2.l;)" EOL
    R"(    d = alloc(l);)" EOL
    R"(    memcpy(d, n2.d, n2.l);)" EOL
    R"(    size_t j = n2.l;)" EOL
    R"(    for (size_t i = 0; i < self.l; i++) {)" EOL
    R"(      d[j++] = self.d[i];)" EOL
    R"(      if (n3 <= 0 || ++k < n3) {)" EOL
    R"(        memcpy(&d[j], n2.d, n2.l);)" EOL
    R"(        j += n2.l;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(  } else if (self.l == n1.l && n1.l > 0) {)" EOL
    R"(    if (memcmp(self.d, n1.d, n1.l) != 0) {)" EOL
    R"(      l = self.l;)" EOL
    R"(      d = alloc(l);)" EOL
    R"(      memcpy(d, self.d, l);)" EOL
    R"(    } else if (n2.l > 0) {)" EOL
    R"(      l = n2.l;)" EOL
    R"(      d = alloc(l);)" EOL
    R"(      memcpy(d, n2.d, l);)" EOL
    R"(    })" EOL
    R"(  } else if (self.l > n1.l && n1.l > 0 && n2.l == 0) {)" EOL
    R"(    d = alloc(self.l);)" EOL
    R"(    for (size_t i = 0; i < self.l; i++) {)" EOL
    R"(      if (i <= self.l - n1.l && memcmp(&self.d[i], n1.d, n1.l) == 0 && (n3 <= 0 || k++ < n3)) {)" EOL
    R"(        i += n1.l - 1;)" EOL
    R"(      } else {)" EOL
    R"(        d[l++] = self.d[i];)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(    if (l == 0) {)" EOL
    R"(      free(d);)" EOL
    R"(      d = NULL;)" EOL
    R"(    } else if (l != self.l) {)" EOL
    R"(      d = re_alloc(d, l);)" EOL
    R"(    })" EOL
    R"(  } else if (self.l > n1.l && n1.l > 0 && n2.l > 0) {)" EOL
    R"(    l = self.l;)" EOL
    R"(    d = alloc(l);)" EOL
    R"(    size_t j = 0;)" EOL
    R"(    for (size_t i = 0; i < self.l; i++) {)" EOL
    R"(      if (i <= self.l - n1.l && memcmp(&self.d[i], n1.d, n1.l) == 0 && (n3 <= 0 || k++ < n3)) {)" EOL
    R"(        if (n1.l < n2.l) {)" EOL
    R"(          l += n2.l - n1.l;)" EOL
    R"(          if (l > self.l) {)" EOL
    R"(            d = re_alloc(d, l);)" EOL
    R"(          })" EOL
    R"(        } else if (n1.l > n2.l) {)" EOL
    R"(          l -= n1.l - n2.l;)" EOL
    R"(        })" EOL
    R"(        memcpy(&d[j], n2.d, n2.l);)" EOL
    R"(        j += n2.l;)" EOL
    R"(        i += n1.l - 1;)" EOL
    R"(      } else {)" EOL
    R"(        d[j++] = self.d[i];)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(    d = re_alloc(d, l);)" EOL
    R"(  } else if (self.l > 0) {)" EOL
    R"(    l = self.l;)" EOL
    R"(    d = alloc(l);)" EOL
    R"(    memcpy(d, self.d, l);)" EOL
    R"(  })" EOL
    R"(  free(n2.d);)" EOL
    R"(  free(n1.d);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdint", "libStdlib", "libString", "re_alloc", "typeStr"},
    {},
    false
  }},

  {"str_slice", {
    false,
    "str_slice",
    "struct str str_slice (struct str, unsigned char, int32_t, unsigned char, int32_t);" EOL,
    R"(struct str str_slice (struct str s, unsigned char o1, int32_t n1, unsigned char o2, int32_t n2) {)" EOL
    R"(  int32_t i1 = o1 == 0 ? 0 : (int32_t) (n1 < 0 ? (n1 < -((int32_t) s.l) ? 0 : n1 + s.l) : (n1 > s.l ? s.l : n1));)" EOL
    R"(  int32_t i2 = o2 == 0 ? (int32_t) s.l : (int32_t) (n2 < 0 ? (n2 < -((int32_t) s.l) ? 0 : n2 + s.l) : (n2 > s.l ? s.l : n2));)" EOL
    R"(  if (i1 >= i2 || i1 >= s.l) {)" EOL
    R"(    free(s.d);)" EOL
    R"(    return str_alloc("");)" EOL
    R"(  })" EOL
    R"(  size_t l = i2 - i1;)" EOL
    R"(  char *d = alloc(l);)" EOL
    R"(  for (size_t i = 0; i1 < i2; i1++) d[i++] = s.d[i1];)" EOL
    R"(  free(s.d);)" EOL
    R"(  return (struct str) {d, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdint", "libStdlib", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"str_split", {
    false,
    "str_split",
    "struct __THE_1_array_str str_split (struct str, unsigned char, struct str);" EOL,
    R"(struct __THE_1_array_str str_split (struct str self, unsigned char o1, struct str n1) {)" EOL
    R"(  struct str *r = NULL;)" EOL
    R"(  size_t l = 0;)" EOL
    R"(  if (self.l > 0 && n1.l == 0) {)" EOL
    R"(    l = self.l;)" EOL
    R"(    r = alloc(l * sizeof(struct str));)" EOL
    R"(    for (size_t i = 0; i < l; i++) {)" EOL
    R"(      r[i] = str_calloc(&self.d[i], 1);)" EOL
    R"(    })" EOL
    R"(  } else if (self.l < n1.l) {)" EOL
    R"(    r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(    r[0] = str_calloc(self.d, self.l);)" EOL
    R"(  } else if (n1.l > 0) {)" EOL
    R"(    size_t i = 0;)" EOL
    R"(    for (size_t j = 0; j <= self.l - n1.l; j++) {)" EOL
    R"(      if (memcmp(&self.d[j], n1.d, n1.l) == 0) {)" EOL
    R"(        r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(        r[l - 1] = str_calloc(&self.d[i], j - i);)" EOL
    R"(        j += n1.l;)" EOL
    R"(        i = j;)" EOL
    R"(      })" EOL
    R"(    })" EOL
    R"(    r = re_alloc(r, ++l * sizeof(struct str));)" EOL
    R"(    r[l - 1] = str_calloc(&self.d[i], self.l - i);)" EOL
    R"(  })" EOL
    R"(  free(n1.d);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (struct __THE_1_array_str) {r, l};)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdlib", "libString", "re_alloc", "str_calloc", "typeStr"},
    {"array_str"},
    false
  }},

  {"str_toBuffer", {
    false,
    "str_toBuffer",
    "struct buffer str_toBuffer (struct str);" EOL,
    R"(struct buffer str_toBuffer (struct str s) {)" EOL
    R"(  return (struct buffer) {(unsigned char *) s.d, s.l};)" EOL
    R"(})" EOL,
    {"typeBuffer", "typeStr"},
    {},
    false
  }},

  {"str_toF32", {
    false,
    "str_toF32",
    "float str_toF32 (err_state_t *, int, int, struct str);" EOL,
    R"(float str_toF32 (err_state_t *fn_err_state, int line, int col, struct str self) {)" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  float r = strtof(c, &e);)" EOL
    R"(  if (errno == ERANGE || r < -FLT_MAX || FLT_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libFloat", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toF64", {
    false,
    "str_toF64",
    "double str_toF64 (err_state_t *, int, int, struct str);" EOL,
    R"(double str_toF64 (err_state_t *fn_err_state, int line, int col, struct str self) {)" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  double r = strtod(c, &e);)" EOL
    R"(  if (errno == ERANGE || r < -DBL_MAX || DBL_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libFloat", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toFloat", {
    false,
    "str_toFloat",
    "double str_toFloat (err_state_t *, int, int, struct str);" EOL,
    R"(double str_toFloat (err_state_t *fn_err_state, int line, int col, struct str self) {)" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  double r = strtod(c, &e);)" EOL
    R"(  if (errno == ERANGE || r < -DBL_MAX || DBL_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libFloat", "libSetJmp", "libStddef", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toI16", {
    false,
    "str_toI16",
    "int16_t str_toI16 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(int16_t str_toI16 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  long r = strtol(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || r < INT16_MIN || INT16_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (int16_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toI32", {
    false,
    "str_toI32",
    "int32_t str_toI32 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(int32_t str_toI32 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  long r = strtol(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || r < INT32_MIN || INT32_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (int32_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toI64", {
    false,
    "str_toI64",
    "int64_t str_toI64 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(int64_t str_toI64 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  long long r = strtoll(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || r < INT64_MIN || INT64_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (int64_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toI8", {
    false,
    "str_toI8",
    "int8_t str_toI8 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(int8_t str_toI8 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  long r = strtol(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || r < INT8_MIN || INT8_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (int8_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toInt", {
    false,
    "str_toInt",
    "int32_t str_toInt (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(int32_t str_toInt (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  long r = strtol(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || r < INT32_MIN || INT32_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0) {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (int32_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toU16", {
    false,
    "str_toU16",
    "uint16_t str_toU16 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(uint16_t str_toU16 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  unsigned long r = strtoul(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || UINT16_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (uint16_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toU32", {
    false,
    "str_toU32",
    "uint32_t str_toU32 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(uint32_t str_toU32 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  unsigned long r = strtoul(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || UINT32_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (uint32_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toU64", {
    false,
    "str_toU64",
    "uint64_t str_toU64 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(uint64_t str_toU64 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  unsigned long long r = strtoull(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || UINT64_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (uint64_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_toU8", {
    false,
    "str_toU8",
    "uint8_t str_toU8 (err_state_t *, int, int, struct str, unsigned char, int32_t);" EOL,
    R"(uint8_t str_toU8 (err_state_t *fn_err_state, int line, int col, struct str self, unsigned char o1, int32_t n1) {)" EOL
    R"(  if (o1 == 1 && (n1 < 2 || n1 > 36) && n1 != 0) {)" EOL
    R"(    const char *fmt = "radix %" PRId32 " is invalid, must be >= 2 and <= 36, or 0";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, n1);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, n1);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  char *c = str_cstr(self);)" EOL
    R"(  char *e = NULL;)" EOL
    R"(  errno = 0;)" EOL
    R"(  unsigned long r = strtoul(c, &e, o1 == 0 ? 10 : n1);)" EOL
    R"(  if (errno == ERANGE || UINT8_MAX < r) {)" EOL
    R"(    const char *fmt = "value `%s` out of range";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == '-') {)" EOL
    R"(    const char *fmt = "value `%s` has invalid syntax";)" EOL
    R"(    size_t z = snprintf(NULL, 0, fmt, c);)" EOL
    R"(    char *d = alloc(z + 1);)" EOL
    R"(    sprintf(d, fmt, c);)" EOL
    R"(    free(c);)" EOL
    R"(    free(self.d);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc((struct str) {d, z}, (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  free(c);)" EOL
    R"(  free(self.d);)" EOL
    R"(  return (uint8_t) r;)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libErrno", "libInttypes", "libSetJmp", "libStddef", "libStdint", "libStdio", "libStdlib", "str_cstr", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free"},
    true
  }},

  {"str_trim", {
    false,
    "str_trim",
    "struct str str_trim (struct str);" EOL,
    R"(struct str str_trim (struct str s) {)" EOL
    R"(  if (s.l == 0) return s;)" EOL
    R"(  size_t i = 0;)" EOL
    R"(  size_t j = s.l;)" EOL
    R"(  while (i < s.l && isspace(s.d[i])) i++;)" EOL
    R"(  while (j >= 0 && isspace(s.d[j - 1])) {)" EOL
    R"(    j--;)" EOL
    R"(    if (j == 0) break;)" EOL
    R"(  })" EOL
    R"(  if (i >= j) {)" EOL
    R"(    free(s.d);)" EOL
    R"(    return str_alloc("");)" EOL
    R"(  })" EOL
    R"(  size_t l = j - i;)" EOL
    R"(  char *r = alloc(l);)" EOL
    R"(  for (size_t k = 0; k < l;) r[k++] = s.d[i++];)" EOL
    R"(  free(s.d);)" EOL
    R"(  return (struct str) {r, l};)" EOL
    R"(})" EOL,
    {"alloc", "libCtype", "libStddef", "libStdlib", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"str_trimEnd", {
    false,
    "str_trimEnd",
    "struct str str_trimEnd (struct str);" EOL,
    R"(struct str str_trimEnd (struct str s) {)" EOL
    R"(  if (s.l == 0) return s;)" EOL
    R"(  while (isspace(s.d[s.l - 1])) {)" EOL
    R"(    s.l--;)" EOL
    R"(    if (s.l == 0) break;)" EOL
    R"(  })" EOL
    R"(  if (s.l == 0) {)" EOL
    R"(    free(s.d);)" EOL
    R"(    s.d = NULL;)" EOL
    R"(  } else {)" EOL
    R"(    s.d = re_alloc(s.d, s.l);)" EOL
    R"(  })" EOL
    R"(  return s;)" EOL
    R"(})" EOL,
    {"libCtype", "libStddef", "libStdlib", "re_alloc", "typeStr"},
    {},
    false
  }},

  {"str_trimStart", {
    false,
    "str_trimStart",
    "struct str str_trimStart (struct str);" EOL,
    R"(struct str str_trimStart (struct str s) {)" EOL
    R"(  if (s.l == 0) return s;)" EOL
    R"(  size_t i = 0;)" EOL
    R"(  while (i < s.l && isspace(s.d[i])) i++;)" EOL
    R"(  if (i >= s.l) {)" EOL
    R"(    free(s.d);)" EOL
    R"(    s = (struct str) {NULL, 0};)" EOL
    R"(  } else {)" EOL
    R"(    memmove(s.d, &s.d[i], s.l - i);)" EOL
    R"(    s.l -= i;)" EOL
    R"(    s.d = re_alloc(s.d, s.l);)" EOL
    R"(  })" EOL
    R"(  return s;)" EOL
    R"(})" EOL,
    {"libCtype", "libStddef", "libStdlib", "libString", "re_alloc", "typeStr"},
    {},
    false
  }},

  {"str_upper", {
    false,
    "str_upper",
    "struct str str_upper (struct str);" EOL,
    R"(struct str str_upper (struct str s) {)" EOL
    R"(  if (s.l != 0) {)" EOL
    R"(    for (size_t i = 0; i < s.l; i++) s.d[i] = (char) toupper(s.d[i]);)" EOL
    R"(  })" EOL
    R"(  return s;)" EOL
    R"(})" EOL,
    {"libCtype", "libStddef", "typeStr"},
    {},
    false
  }},

  {"str_upperFirst", {
    false,
    "str_upperFirst",
    "struct str str_upperFirst (struct str);" EOL,
    R"(struct str str_upperFirst (struct str s) {)" EOL
    R"(  if (s.l != 0) s.d[0] = (char) toupper(s.d[0]);)" EOL
    R"(  return s;)" EOL
    R"(})" EOL,
    {"libCtype", "typeStr"},
    {},
    false
  }},

  {"thread_id", {
    false,
    "thread_id",
    "int32_t thread_id ();" EOL,
    R"(int32_t thread_id () {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    return (int32_t) GetCurrentThreadId();)" EOL
    R"(  #elif defined(THE_OS_MACOS))" EOL
    R"(    uint64_t id;)" EOL
    R"(    pthread_threadid_np(NULL, &id);)" EOL
    R"(    return (int32_t) id;)" EOL
    R"(  #else)" EOL
    R"(    return (int32_t) syscall(SYS_gettid);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libStddef", "libStdint", "libSysSyscall", "libUnistd", "libWindows"},
    {},
    false
  }},

  {"thread_sleep", {
    false,
    "thread_sleep",
    "void thread_sleep (int32_t);" EOL,
    R"(void thread_sleep (int32_t i) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    Sleep((unsigned int) i);)" EOL
    R"(  #else)" EOL
    R"(    usleep((unsigned int) (i * 1000));)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libStdint", "libUnistd", "libWindows"},
    {},
    false
  }},

  {"threadpool_add", {
    false,
    "threadpool_add",
    "void threadpool_add (threadpool_t *, threadpool_func_t, void *, void *, void *, threadpool_job_t *);" EOL,
    R"(void threadpool_add (threadpool_t *self, threadpool_func_t func, void *ctx, void *params, void *ret, threadpool_job_t *parent) {)" EOL
    R"(  threadpool_job_t *job = alloc(sizeof(threadpool_job_t));)" EOL
    R"(  job->parent = parent;)" EOL
    R"(  job->func = func;)" EOL
    R"(  job->ctx = ctx;)" EOL
    R"(  job->params = params;)" EOL
    R"(  job->ret = ret;)" EOL
    R"(  job->step = 0;)" EOL
    R"(  job->init = 0;)" EOL
    R"(  job->next = NULL;)" EOL
    R"(  threadpool_insert(self, job);)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "threadpool_insert", "typeThreadpool", "typeThreadpoolFunc", "typeThreadpoolJob"},
    {},
    false
  }},

  {"threadpool_cond_deinit", {
    false,
    "threadpool_cond_deinit",
    "void threadpool_cond_deinit (threadpool_cond_t *);" EOL,
    R"(void threadpool_cond_deinit (threadpool_cond_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    (void) &self;)" EOL
    R"(  #else)" EOL
    R"(    pthread_cond_destroy(&self->v);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "typeThreadpoolCond"},
    {},
    false
  }},

  {"threadpool_cond_init", {
    false,
    "threadpool_cond_init",
    "void threadpool_cond_init (threadpool_cond_t *);" EOL,
    R"(void threadpool_cond_init (threadpool_cond_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    InitializeConditionVariable(&self->v);)" EOL
    R"(  #else)" EOL
    R"(    pthread_cond_init(&self->v, NULL);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libStddef", "libWindows", "typeThreadpoolCond"},
    {},
    false
  }},

  {"threadpool_cond_signal", {
    false,
    "threadpool_cond_signal",
    "void threadpool_cond_signal (threadpool_cond_t *);" EOL,
    R"(void threadpool_cond_signal (threadpool_cond_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    WakeConditionVariable(&self->v);)" EOL
    R"(  #else)" EOL
    R"(    pthread_cond_signal(&self->v);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libWindows", "typeThreadpoolCond"},
    {},
    false
  }},

  {"threadpool_cond_wait", {
    false,
    "threadpool_cond_wait",
    "void threadpool_cond_wait (threadpool_cond_t *, threadpool_mutex_t *);" EOL,
    R"(void threadpool_cond_wait (threadpool_cond_t *self, threadpool_mutex_t *mutex) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    SleepConditionVariableCS(&self->v, &mutex->v, INFINITE);)" EOL
    R"(  #else)" EOL
    R"(    pthread_cond_wait(&self->v, &mutex->v);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libWindows", "typeThreadpoolCond", "typeThreadpoolMutex"},
    {},
    false
  }},

  {"threadpool_deinit", {
    false,
    "threadpool_deinit",
    "void threadpool_deinit (threadpool_t *);" EOL,
    R"(void threadpool_deinit (threadpool_t *self) {)" EOL
    R"(  self->active = false;)" EOL
    R"(  threadpool_mutex_lock(&self->lock1);)" EOL
    R"(  while (self->alive_threads != 0) {)" EOL
    R"(    threadpool_mutex_lock(&self->lock3);)" EOL
    R"(    threadpool_cond_signal(&self->cond2);)" EOL
    R"(    threadpool_mutex_unlock(&self->lock3);)" EOL
    R"(    threadpool_cond_wait(&self->cond1, &self->lock1);)" EOL
    R"(  })" EOL
    R"(  threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(  while (self->jobs != NULL) {)" EOL
    R"(    threadpool_job_t *next = self->jobs->next;)" EOL
    R"(    threadpool_job_deinit(self->jobs);)" EOL
    R"(    self->jobs = next;)" EOL
    R"(  })" EOL
    R"(  while (self->threads != NULL) {)" EOL
    R"(    threadpool_thread_t *next = self->threads->next;)" EOL
    R"(    threadpool_thread_deinit(self->threads);)" EOL
    R"(    self->threads = next;)" EOL
    R"(  })" EOL
    R"(  threadpool_cond_deinit(&self->cond1);)" EOL
    R"(  threadpool_cond_deinit(&self->cond2);)" EOL
    R"(  threadpool_mutex_deinit(&self->lock1);)" EOL
    R"(  threadpool_mutex_deinit(&self->lock2);)" EOL
    R"(  threadpool_mutex_deinit(&self->lock3);)" EOL
    R"(  free(self);)" EOL
    R"(})" EOL,
    {"libStdbool", "libStddef", "libStdlib", "threadpool_cond_deinit", "threadpool_cond_signal", "threadpool_cond_wait", "threadpool_job_deinit", "threadpool_mutex_deinit", "threadpool_mutex_lock", "threadpool_mutex_unlock", "threadpool_thread_deinit", "typeThreadpool", "typeThreadpoolJob", "typeThreadpoolThread"},
    {},
    false
  }},

  {"threadpool_error_assign", {
    false,
    "threadpool_error_assign",
    "void threadpool_error_assign (threadpool_t *, err_state_t *);" EOL,
    R"(void threadpool_error_assign (threadpool_t *self, err_state_t *fn_err_state) {)" EOL
    R"(  threadpool_mutex_lock(&self->lock1);)" EOL
    R"(  memcpy(&err_state, fn_err_state, sizeof(err_state_t));)" EOL
    R"(  threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(})" EOL,
    {"libString", "threadpool_mutex_lock", "threadpool_mutex_unlock", "typeErrState", "typeThreadpool", "varErrState"},
    {},
    false
  }},

  {"threadpool_error_assign_parent", {
    false,
    "threadpool_error_assign_parent",
    "void threadpool_error_assign_parent (threadpool_t *, threadpool_job_t *, err_state_t *);" EOL,
    R"(void threadpool_error_assign_parent (threadpool_t *self, threadpool_job_t *job, err_state_t *fn_err_state) {)" EOL
    R"(  memcpy(*((void **) job->params), fn_err_state, sizeof(err_state_t));)" EOL
    R"(})" EOL,
    {"libString", "typeErrState", "typeThreadpool", "typeThreadpoolJob"},
    {},
    false
  }},

  {"threadpool_get", {
    false,
    "threadpool_get",
    "threadpool_job_t *threadpool_get (threadpool_t *);" EOL,
    R"(threadpool_job_t *threadpool_get (threadpool_t *self) {)" EOL
    R"(  threadpool_mutex_lock(&self->lock2);)" EOL
    R"(  threadpool_job_t *job = self->jobs;)" EOL
    R"(  if (job != NULL) {)" EOL
    R"(    self->jobs = self->jobs->next;)" EOL
    R"(    job->next = NULL;)" EOL
    R"(    if (self->jobs == NULL) {)" EOL
    R"(      self->jobs_tail = NULL;)" EOL
    R"(    } else {)" EOL
    R"(      threadpool_mutex_lock(&self->lock3);)" EOL
    R"(      threadpool_cond_signal(&self->cond2);)" EOL
    R"(      threadpool_mutex_unlock(&self->lock3);)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  threadpool_mutex_unlock(&self->lock2);)" EOL
    R"(  return job;)" EOL
    R"(})" EOL,
    {"libStddef", "threadpool_cond_signal", "threadpool_mutex_lock", "threadpool_mutex_unlock", "typeThreadpool", "typeThreadpoolJob"},
    {},
    false
  }},

  {"threadpool_init", {
    false,
    "threadpool_init",
    "threadpool_t *threadpool_init (int);" EOL,
    R"(threadpool_t *threadpool_init (int count) {)" EOL
    R"(  threadpool_t *self = alloc(sizeof(threadpool_t));)" EOL
    R"(  self->active = true;)" EOL
    R"(  self->jobs = NULL;)" EOL
    R"(  self->jobs_tail = NULL;)" EOL
    R"(  self->working_threads = 0;)" EOL
    R"(  self->alive_threads = 0;)" EOL
    R"(  threadpool_cond_init(&self->cond1);)" EOL
    R"(  threadpool_cond_init(&self->cond2);)" EOL
    R"(  threadpool_mutex_init(&self->lock1);)" EOL
    R"(  threadpool_mutex_init(&self->lock2);)" EOL
    R"(  threadpool_mutex_init(&self->lock3);)" EOL
    R"(  self->threads = NULL;)" EOL
    R"(  for (int i = 0; i < count; i++) self->threads = threadpool_thread_init(self, self->threads);)" EOL
    R"(  threadpool_mutex_lock(&self->lock1);)" EOL
    R"(  while (self->alive_threads != count) threadpool_cond_wait(&self->cond1, &self->lock1);)" EOL
    R"(  threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(  return self;)" EOL
    R"(})" EOL,
    {"alloc", "libStdbool", "libStddef", "threadpool_cond_init", "threadpool_cond_wait", "threadpool_mutex_init", "threadpool_mutex_lock", "threadpool_mutex_unlock", "threadpool_thread_init", "typeThreadpool"},
    {},
    false
  }},

  {"threadpool_insert", {
    false,
    "threadpool_insert",
    "void threadpool_insert (threadpool_t *, threadpool_job_t *);" EOL,
    R"(void threadpool_insert (threadpool_t *self, threadpool_job_t *job) {)" EOL
    R"(  threadpool_mutex_lock(&self->lock2);)" EOL
    R"(  if (self->jobs == NULL) {)" EOL
    R"(    self->jobs = job;)" EOL
    R"(    self->jobs_tail = job;)" EOL
    R"(  } else {)" EOL
    R"(    self->jobs_tail->next = job;)" EOL
    R"(    self->jobs_tail = self->jobs_tail->next;)" EOL
    R"(  })" EOL
    R"(  threadpool_mutex_lock(&self->lock3);)" EOL
    R"(  threadpool_cond_signal(&self->cond2);)" EOL
    R"(  threadpool_mutex_unlock(&self->lock3);)" EOL
    R"(  threadpool_mutex_unlock(&self->lock2);)" EOL
    R"(})" EOL,
    {"libStddef", "threadpool_cond_signal", "threadpool_mutex_lock", "threadpool_mutex_unlock", "typeThreadpool", "typeThreadpoolJob"},
    {},
    false
  }},

  {"threadpool_job_deinit", {
    false,
    "threadpool_job_deinit",
    "void threadpool_job_deinit (threadpool_job_t *);" EOL,
    R"(void threadpool_job_deinit (threadpool_job_t *self) {)" EOL
    R"(  if (self->ctx != NULL) free(self->ctx);)" EOL
    R"(  free(self);)" EOL
    R"(})" EOL,
    {"libStddef", "libStdlib", "typeThreadpoolJob"},
    {},
    false
  }},

  {"threadpool_job_step", {
    false,
    "threadpool_job_step",
    "threadpool_job_t *threadpool_job_step (threadpool_job_t *, int);" EOL,
    R"(threadpool_job_t *threadpool_job_step (threadpool_job_t *self, int step) {)" EOL
    R"(  self->step = step;)" EOL
    R"(  return self;)" EOL
    R"(})" EOL,
    {"typeThreadpoolJob"},
    {},
    false
  }},

  {"threadpool_mutex_deinit", {
    false,
    "threadpool_mutex_deinit",
    "void threadpool_mutex_deinit (threadpool_mutex_t *);" EOL,
    R"(void threadpool_mutex_deinit (threadpool_mutex_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    DeleteCriticalSection(&self->v);)" EOL
    R"(  #else)" EOL
    R"(    pthread_mutex_destroy(&self->v);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libWindows", "typeThreadpoolMutex"},
    {},
    false
  }},

  {"threadpool_mutex_init", {
    false,
    "threadpool_mutex_init",
    "void threadpool_mutex_init (threadpool_mutex_t *);" EOL,
    R"(void threadpool_mutex_init (threadpool_mutex_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    InitializeCriticalSection(&self->v);)" EOL
    R"(  #else)" EOL
    R"(    pthread_mutex_init(&self->v, NULL);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libStddef", "libWindows", "typeThreadpoolMutex"},
    {},
    false
  }},

  {"threadpool_mutex_lock", {
    false,
    "threadpool_mutex_lock",
    "void threadpool_mutex_lock (threadpool_mutex_t *);" EOL,
    R"(void threadpool_mutex_lock (threadpool_mutex_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    EnterCriticalSection(&self->v);)" EOL
    R"(  #else)" EOL
    R"(    pthread_mutex_lock(&self->v);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libWindows", "typeThreadpoolMutex"},
    {},
    false
  }},

  {"threadpool_mutex_unlock", {
    false,
    "threadpool_mutex_unlock",
    "void threadpool_mutex_unlock (threadpool_mutex_t *);" EOL,
    R"(void threadpool_mutex_unlock (threadpool_mutex_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    LeaveCriticalSection(&self->v);)" EOL
    R"(  #else)" EOL
    R"(    pthread_mutex_unlock(&self->v);)" EOL
    R"(  #endif)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libWindows", "typeThreadpoolMutex"},
    {},
    false
  }},

  {"threadpool_thread_deinit", {
    false,
    "threadpool_thread_deinit",
    "void threadpool_thread_deinit (threadpool_thread_t *);" EOL,
    R"(void threadpool_thread_deinit (threadpool_thread_t *self) {)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    WaitForSingleObject(self->id, INFINITE);)" EOL
    R"(    CloseHandle(self->id);)" EOL
    R"(    MemoryBarrier();)" EOL
    R"(  #else)" EOL
    R"(    pthread_join(self->id, NULL);)" EOL
    R"(  #endif)" EOL
    R"(  free(self);)" EOL
    R"(})" EOL,
    {"definitions", "libPthread", "libStddef", "libStdlib", "libWindows", "typeThreadpoolThread"},
    {},
    false
  }},

  {"threadpool_thread_init", {
    false,
    "threadpool_thread_init",
    "threadpool_thread_t *threadpool_thread_init (threadpool_t *, threadpool_thread_t *);" EOL,
    R"(threadpool_thread_t *threadpool_thread_init (threadpool_t *tp, threadpool_thread_t *next) {)" EOL
    R"(  threadpool_thread_t *self = alloc(sizeof(threadpool_thread_t));)" EOL
    R"(  self->tp = tp;)" EOL
    R"(  self->next = next;)" EOL
    R"(  #ifdef THE_OS_WINDOWS)" EOL
    R"(    self->id = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadpool_worker, self, CREATE_SUSPENDED, NULL);)" EOL
    R"(    ResumeThread(self->id);)" EOL
    R"(  #else)" EOL
    R"(    pthread_create(&self->id, NULL, threadpool_worker, self);)" EOL
    R"(  #endif)" EOL
    R"(  return self;)" EOL
    R"(})" EOL,
    {"alloc", "definitions", "libPthread", "libStddef", "libWindows", "threadpool_worker", "typeThreadpool", "typeThreadpoolThread"},
    {},
    false
  }},

  {"threadpool_wait", {
    false,
    "threadpool_wait",
    "void threadpool_wait (threadpool_t *);" EOL,
    R"(void threadpool_wait (threadpool_t *self) {)" EOL
    R"(  threadpool_mutex_lock(&self->lock1);)" EOL
    R"(  while ((self->working_threads != 0 || self->jobs != NULL) && err_state.id == -1) {)" EOL
    R"(    threadpool_cond_wait(&self->cond1, &self->lock1);)" EOL
    R"(  })" EOL
    R"(  threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(})" EOL,
    {"libStddef", "threadpool_cond_wait", "threadpool_mutex_lock", "threadpool_mutex_unlock", "typeThreadpool", "varErrState"},
    {},
    false
  }},

  {"threadpool_worker", {
    false,
    "threadpool_worker",
    "void *threadpool_worker (void *);" EOL,
    R"(void *threadpool_worker (void *n) {)" EOL
    R"(  threadpool_thread_t *thread = n;)" EOL
    R"(  threadpool_t *self = thread->tp;)" EOL
    R"(  threadpool_mutex_lock(&self->lock1);)" EOL
    R"(  self->alive_threads++;)" EOL
    R"(  threadpool_cond_signal(&self->cond1);)" EOL
    R"(  threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(  while (self->active) {)" EOL
    R"(    threadpool_mutex_lock(&self->lock3);)" EOL
    R"(    while (self->active && self->jobs == NULL) threadpool_cond_wait(&self->cond2, &self->lock3);)" EOL
    R"(    threadpool_mutex_unlock(&self->lock3);)" EOL
    R"(    if (!self->active) break;)" EOL
    R"(    threadpool_mutex_lock(&self->lock1);)" EOL
    R"(    self->working_threads++;)" EOL
    R"(    threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(    threadpool_job_t *job = threadpool_get(self);)" EOL
    R"(    if (job != NULL) job->func(self, job, job->ctx, job->params, job->ret, job->step);)" EOL
    R"(    threadpool_mutex_lock(&self->lock1);)" EOL
    R"(    self->working_threads--;)" EOL
    R"(    if ((self->working_threads == 0 && self->jobs == NULL) || err_state.id != -1) threadpool_cond_signal(&self->cond1);)" EOL
    R"(    threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(  })" EOL
    R"(  threadpool_mutex_lock(&self->lock1);)" EOL
    R"(  self->alive_threads--;)" EOL
    R"(  threadpool_cond_signal(&self->cond1);)" EOL
    R"(  threadpool_mutex_unlock(&self->lock1);)" EOL
    R"(  return NULL;)" EOL
    R"(})" EOL,
    {"libStddef", "threadpool_cond_signal", "threadpool_cond_wait", "threadpool_get", "threadpool_mutex_lock", "threadpool_mutex_unlock", "typeThreadpool", "typeThreadpoolJob", "typeThreadpoolThread", "varErrState"},
    {},
    false
  }},

  {"u16_str", {
    false,
    "u16_str",
    "struct str u16_str (uint16_t);" EOL,
    R"(struct str u16_str (uint16_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRIu16, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"u32_str", {
    false,
    "u32_str",
    "struct str u32_str (uint32_t);" EOL,
    R"(struct str u32_str (uint32_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRIu32, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"u64_str", {
    false,
    "u64_str",
    "struct str u64_str (uint64_t);" EOL,
    R"(struct str u64_str (uint64_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRIu64, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdint", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"u8_str", {
    false,
    "u8_str",
    "struct str u8_str (uint8_t);" EOL,
    R"(struct str u8_str (uint8_t d) {)" EOL
    R"(  char buf[24];)" EOL
    R"(  sprintf(buf, "%" PRIu8, d);)" EOL
    R"(  return str_alloc(buf);)" EOL
    R"(})" EOL,
    {"libInttypes", "libStdio", "str_alloc", "typeStr"},
    {},
    false
  }},

  {"url_parse", {
    false,
    "url_parse",
    "struct url_URL *url_parse (err_state_t *, int, int, struct str);" EOL,
    R"(struct url_URL *url_parse (err_state_t *fn_err_state, int line, int col, struct str s) {)" EOL
    R"(  if (s.l == 0) {)" EOL
    R"(    str_free(s);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid URL"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  })" EOL
    R"(  size_t i = 0;)" EOL
    R"(  for (;; i++) {)" EOL
    R"(    char ch = s.d[i];)" EOL
    R"(    if (ch == ':' && i != 0) {)" EOL
    R"(      i++;)" EOL
    R"(      break;)" EOL
    R"(    } else if (!isalnum(ch) && ch != '.' && ch != '-' && ch != '+') {)" EOL
    R"(      str_free(s);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid URL protocol"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    } else if (i == s.l - 1) {)" EOL
    R"(      str_free(s);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid URL"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  struct str protocol;)" EOL
    R"(  protocol.l = i;)" EOL
    R"(  protocol.d = alloc(protocol.l);)" EOL
    R"(  memcpy(protocol.d, s.d, protocol.l);)" EOL
    R"(  while (i < s.l && s.d[i] == '/') i++;)" EOL
    R"(  if (i == s.l) {)" EOL
    R"(    str_free(s);)" EOL
    R"(    return url_URL_alloc(str_alloc(""), protocol, str_alloc(""), str_alloc(""), str_alloc(""), str_alloc(""), str_alloc(""), str_alloc(""), str_alloc(""));)" EOL
    R"(  })" EOL
    R"(  size_t protocol_end = i;)" EOL
    R"(  if ((protocol_end - protocol.l) < 2) {)" EOL
    R"(    i = protocol.l;)" EOL
    R"(    protocol_end = i;)" EOL
    R"(  })" EOL
    R"(  size_t hostname_start = protocol.l == protocol_end ? 0 : i;)" EOL
    R"(  size_t port_start = 0;)" EOL
    R"(  size_t pathname_start = protocol.l == protocol_end ? i : 0;)" EOL
    R"(  size_t search_start = 0;)" EOL
    R"(  size_t hash_start = 0;)" EOL
    R"(  for (;; i++) {)" EOL
    R"(    char ch = s.d[i];)" EOL
    R"(    if (ch == '@' && hostname_start != 0 && pathname_start == 0) {)" EOL
    R"(      str_free(protocol);)" EOL
    R"(      str_free(s);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("URL auth is not supported"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    } else if (ch == ':' && port_start != 0 && (pathname_start == 0 || search_start == 0 || hash_start == 0)) {)" EOL
    R"(      str_free(protocol);)" EOL
    R"(      str_free(s);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid URL port"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(    if (ch == ':' && hostname_start != 0 && pathname_start == 0) port_start = i;)" EOL
    R"(    else if (ch == '/' && pathname_start == 0) pathname_start = i;)" EOL
    R"(    else if (ch == '?' && search_start == 0) search_start = i;)" EOL
    R"(    else if (ch == '#' && hash_start == 0) hash_start = i;)" EOL
    R"(    if (i == s.l - 1) break;)" EOL
    R"(  })" EOL
    R"(  struct str hostname = str_alloc("");)" EOL
    R"(  size_t hostname_end = port_start != 0 ? port_start : pathname_start != 0 ? pathname_start : search_start != 0 ? search_start : hash_start != 0 ? hash_start : s.l;)" EOL
    R"(  if (hostname_start != 0 && hostname_start == hostname_end) {)" EOL
    R"(    str_free(hostname);)" EOL
    R"(    str_free(protocol);)" EOL
    R"(    str_free(s);)" EOL
    R"(    error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("invalid URL hostname"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(  } else if (hostname_start != 0 && hostname_start != hostname_end) {)" EOL
    R"(    hostname.l = hostname_end - hostname_start;)" EOL
    R"(    hostname.d = re_alloc(hostname.d, hostname.l);)" EOL
    R"(    memcpy(hostname.d, &s.d[hostname_start], hostname.l);)" EOL
    R"(  })" EOL
    R"(  struct str port = str_alloc("");)" EOL
    R"(  size_t port_end = pathname_start != 0 ? pathname_start : search_start != 0 ? search_start : hash_start != 0 ? hash_start : s.l;)" EOL
    R"(  if (port_start != 0 && port_start + 1 != port_end) {)" EOL
    R"(    port.l = port_end - port_start - 1;)" EOL
    R"(    port.d = re_alloc(port.d, port.l);)" EOL
    R"(    memcpy(port.d, &s.d[port_start + 1], port.l);)" EOL
    R"(  })" EOL
    R"(  struct str host = str_alloc("");)" EOL
    R"(  if (hostname.l != 0) {)" EOL
    R"(    host.l = hostname.l + (port.l == 0 ? 0 : port.l + 1);)" EOL
    R"(    host.d = re_alloc(host.d, host.l);)" EOL
    R"(    memcpy(host.d, hostname.d, hostname.l);)" EOL
    R"(    if (port.l != 0) {)" EOL
    R"(      memcpy(&host.d[hostname.l], ":", 1);)" EOL
    R"(      memcpy(&host.d[hostname.l + 1], port.d, port.l);)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  struct str origin = str_alloc("");)" EOL
    R"(  if (memcmp(protocol.d, "ftp:", 4) == 0 || memcmp(protocol.d, "http:", 5) == 0 || memcmp(protocol.d, "https:", 6) == 0 || memcmp(protocol.d, "ws:", 3) == 0 || memcmp(protocol.d, "wss:", 4) == 0) {)" EOL
    R"(    if (host.l == 0) {)" EOL
    R"(      str_free(origin);)" EOL
    R"(      str_free(host);)" EOL
    R"(      str_free(port);)" EOL
    R"(      str_free(hostname);)" EOL
    R"(      str_free(protocol);)" EOL
    R"(      str_free(s);)" EOL
    R"(      error_assign(fn_err_state, TYPE_error_Error, (void *) error_Error_alloc(str_alloc("URL origin is not present"), (struct str) {NULL, 0}), (void (*) (void *)) &error_Error_free, line, col);)" EOL
    R"(      longjmp(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
    R"(    })" EOL
    R"(    origin.l = protocol.l + 2 + host.l;)" EOL
    R"(    origin.d = re_alloc(origin.d, origin.l);)" EOL
    R"(    memcpy(origin.d, protocol.d, protocol.l);)" EOL
    R"(    memcpy(&origin.d[protocol.l], "//", 2);)" EOL
    R"(    memcpy(&origin.d[protocol.l + 2], host.d, host.l);)" EOL
    R"(  })" EOL
    R"(  struct str pathname = str_alloc("");)" EOL
    R"(  size_t pathname_end = search_start != 0 ? search_start : hash_start != 0 ? hash_start : s.l;)" EOL
    R"(  if (pathname_start != 0 && pathname_start != pathname_end) {)" EOL
    R"(    pathname.l = pathname_end - pathname_start;)" EOL
    R"(    pathname.d = re_alloc(pathname.d, pathname.l);)" EOL
    R"(    memcpy(pathname.d, &s.d[pathname_start], pathname.l);)" EOL
    R"(  } else if (memcmp(protocol.d, "ftp:", 4) == 0 || memcmp(protocol.d, "http:", 5) == 0 || memcmp(protocol.d, "https:", 6) == 0 || memcmp(protocol.d, "ws:", 3) == 0 || memcmp(protocol.d, "wss:", 4) == 0) {)" EOL
    R"(    pathname.l = 1;)" EOL
    R"(    pathname.d = re_alloc(pathname.d, pathname.l);)" EOL
    R"(    memcpy(pathname.d, "/", pathname.l);)" EOL
    R"(  })" EOL
    R"(  struct str search = str_alloc("");)" EOL
    R"(  size_t search_end = hash_start != 0 ? hash_start : s.l;)" EOL
    R"(  if (search_start != 0 && search_start != search_end) {)" EOL
    R"(    search.l = search_end - search_start;)" EOL
    R"(    search.d = re_alloc(search.d, search.l);)" EOL
    R"(    memcpy(search.d, &s.d[search_start], search.l);)" EOL
    R"(  })" EOL
    R"(  struct str path = str_alloc("");)" EOL
    R"(  if (pathname.l != 0 || search.l != 0) {)" EOL
    R"(    path.l = pathname.l + search.l;)" EOL
    R"(    path.d = re_alloc(path.d, path.l);)" EOL
    R"(    if (pathname.l != 0) {)" EOL
    R"(      memcpy(path.d, pathname.d, pathname.l);)" EOL
    R"(      if (search.l != 0) memcpy(&path.d[pathname.l], search.d, search.l);)" EOL
    R"(    } else if (search.l != 0) {)" EOL
    R"(      memcpy(path.d, search.d, search.l);)" EOL
    R"(    })" EOL
    R"(  })" EOL
    R"(  struct str hash = str_alloc("");)" EOL
    R"(  if (hash_start != 0) {)" EOL
    R"(    hash.l = s.l - hash_start;)" EOL
    R"(    hash.d = re_alloc(hash.d, hash.l);)" EOL
    R"(    memcpy(hash.d, &s.d[hash_start], hash.l);)" EOL
    R"(  })" EOL
    R"(  str_free(s);)" EOL
    R"(  return url_URL_alloc(origin, protocol, host, hostname, port, path, pathname, search, hash);)" EOL
    R"(})" EOL,
    {"alloc", "error_assign", "libCtype", "libSetJmp", "libStddef", "libString", "re_alloc", "str_alloc", "str_free", "typeErrState", "typeStr"},
    {"TYPE_error_Error", "error_Error_alloc", "error_Error_free", "url_URL", "url_URL_alloc"},
    true
  }},

  {"utils_swap", {
    false,
    "utils_swap",
    "void utils_swap (void *, void *, size_t);" EOL,
    R"(void utils_swap (void *a, void *b, size_t l) {)" EOL
    R"(  void *t = alloc(l);)" EOL
    R"(  memcpy(t, a, l);)" EOL
    R"(  memcpy(a, b, l);)" EOL
    R"(  memcpy(b, t, l);)" EOL
    R"(  free(t);)" EOL
    R"(})" EOL,
    {"alloc", "libStddef", "libStdlib", "libString"},
    {},
    false
  }},

  {"xalloc", {
    false,
    "xalloc",
    "void *xalloc (void *, size_t);" EOL,
    R"(void *xalloc (void *n1, size_t n2) {)" EOL
    R"(  void *r = malloc(n2);)" EOL
    R"(  if (r == NULL) error_alloc(&err_state, n2);)" EOL
    R"(  memcpy(r, n1, n2);)" EOL
    R"(  return r;)" EOL
    R"(})" EOL,
    {"error_alloc", "libStddef", "libStdlib", "libString", "varErrState"},
    {},
    false
  }}
};

#endif
