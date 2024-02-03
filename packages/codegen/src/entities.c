/*!
 * Copyright (c) Aaron Delasy
 * Licensed under the MIT License
 */

// todo implement rune methods
// todo check all malloc of string has wchar_t
// todo wcscmp and other methods are utilized correctly
// todo replace str_alloc("") with just empty string

#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
  #define __THE_OS_WINDOWS
#elif defined(__APPLE__)
  #define __THE_OS_MACOS
#elif defined(__linux__)
  #define __THE_OS_LINUX
#endif

#if defined(__THE_OS_WINDOWS)
  #define __THE_EOL L"\r\n"
#else
  #define __THE_EOL L"\n"
#endif

#if defined(__THE_OS_WINDOWS)
  #define __THE_PATH_SEP L"\\"
#else
  #define __THE_PATH_SEP L"/"
#endif

#define TYPE_Error 1

typedef struct {
  wchar_t *d;
  size_t l;
} str_t;

typedef struct {
  str_t *d;
  size_t l;
} arr_str_t;

typedef struct any {
  int t;
  void *d;
  struct any (*_copy) (const struct any);
  bool (*_eq) (const struct any, const struct any);
  bool (*_ne) (const struct any, const struct any);
  void (*_free) (struct any);
  str_t (*_str) (const struct any);
} any_t;

typedef struct err_buf {
  jmp_buf buf;
  struct err_buf *next;
  struct err_buf *prev;
} err_buf_t;

typedef struct err_stack {
  const wchar_t *file;
  const wchar_t *name;
  int line;
  int col;
  struct err_stack *next;
  struct err_stack *prev;
} err_stack_t;

typedef struct {
  int id;
  void *ctx;
  err_buf_t *buf_first;
  err_buf_t *buf_last;
  err_stack_t *stack_first;
  err_stack_t *stack_last;
  void (*_free) (void *);
} err_state_t;

typedef struct {
  str_t message;
  str_t stack;
} Error_t;

err_state_t err_state;

any_t any_copy (const any_t self) {
  return self.d == NULL ? (any_t) {0, NULL} : self._copy(self);
}

bool any_eq (const any_t self, const any_t rhs) {
  return self.d == NULL ? false : self._eq(self, rhs);
}

void any_free (any_t self) {
  if (self.d != NULL) self._free(self);
}

bool any_ne (const any_t self, const any_t rhs) {
  return self.d == NULL ? true : self._ne(self, rhs);
}

any_t any_realloc (any_t self, const any_t n) {
  any_free(self);
  return any_copy(n);
}

str_t any_str (const any_t self) {
  return self.d == NULL ? str_alloc("any") : self._str(self);
}

str_t bool_str (bool self) {
  return str_alloc(self ? "true" : "false");
}

str_t byte_str (unsigned char self) {
  char buf[8];
  sprintf(buf, "%u", self);
  return str_alloc(buf);
}

unsigned char char_byte (char self) {
  return (unsigned char) self;
}

bool char_isDigit (char self) {
  return isdigit(self);
}

bool char_isLetter (char self) {
  return isalpha(self);
}

bool char_isLetterOrDigit (char self) {
  return isalnum(self);
}

bool char_isLower (char self) {
  return islower(self);
}

bool char_isUpper (char self) {
  return isupper(self);
}

bool char_isWhitespace (char self) {
  return isspace(self);
}

char char_lower (char self) {
  return tolower(self);
}

str_t char_repeat (char self, int32_t times) {
  if (times <= 0) return str_alloc("");
  size_t l = (size_t) times;
  wchar_t *d = safe_malloc(l * sizeof(wchar_t));
  for (size_t i = 0; i < l; i++) d[i] = (wchar_t) self;
  return (str_t) {d, l};
}

str_t char_str (char self) {
  char buf[2] = {self, '\0'};
  return str_alloc(buf);
}

char char_upper (char self) {
  return toupper(self);
}

str_t enum_str (int self) {
  char buf[24];
  sprintf(buf, "%d", self);
  return str_alloc(buf);
}

void error_alloc (const err_state_t *fn_err_state, size_t length) {
  wchar_t d[4096];
  size_t l = 0;
  for (err_stack_t *it = fn_err_state->stack_last; it != NULL; it = it->prev) {
    wchar_t *fmt;
    size_t z;
    if (it->col == 0 && it->line == 0) {
      fmt = __THE_EOL L"  at %s (%s)";
      z = swprintf(NULL, 0, fmt, it->name, it->file);
    } else if (it->col == 0) {
      fmt = __THE_EOL L"  at %s (%s:%d)";
      z = swprintf(NULL, 0, fmt, it->name, it->file, it->line);
    } else {
      fmt = __THE_EOL L"  at %s (%s:%d:%d)";
      z = swprintf(NULL, 0, fmt, it->name, it->file, it->line, it->col);
    }
    if (l + z >= 4096) break;
    if (it->col == 0 && it->line == 0) {
      swprintf(&d[l], z, fmt, it->name, it->file);
    } else if (it->col == 0) {
      swprintf(&d[l], z, fmt, it->name, it->file, it->line);
    } else {
      swprintf(&d[l], z, fmt, it->name, it->file, it->line, it->col);
    }
    l += z;
  }
  fwprintf(stderr, L"Allocation Error: failed to allocate %zu bytes%s" __THE_EOL, length, d);
  free(d);
  exit(EXIT_FAILURE);
}

void error_assign (err_state_t *fn_err_state, int line, int col, int id, void *ctx, void (*f) (void *)) {
  fn_err_state->id = id;
  fn_err_state->ctx = ctx;
  fn_err_state->_free = f;
  error_stack_pos(fn_err_state, line, col);
  error_stack_str(fn_err_state);
}

void error_assign_builtin (err_state_t *fn_err_state, int line, int col, str_t message) {
  fn_err_state->id = TYPE_Error;
  fn_err_state->ctx = (void *) Error_alloc(message, str_alloc(""));
  fn_err_state->_free = (void (*) (void *)) Error_free;
  error_stack_pos(fn_err_state, line, col);
  error_stack_str(fn_err_state);
}

void error_buf_decrease (err_state_t *fn_err_state) {
  err_buf_t *buf = fn_err_state->buf_last;
  fn_err_state->buf_last = buf->prev;
  free(buf);
}

err_buf_t *error_buf_increase (err_state_t *fn_err_state) {
  err_buf_t *buf = safe_malloc(sizeof(err_buf_t));
  buf->next = NULL;
  buf->prev = fn_err_state->buf_last;
  if (fn_err_state->buf_first == NULL) fn_err_state->buf_first = buf;
  if (fn_err_state->buf_last != NULL) fn_err_state->buf_last->next = buf;
  fn_err_state->buf_last = buf;
  return fn_err_state->buf_last;
}

void error_stack_pop (err_state_t *fn_err_state) {
  err_stack_t *stack = fn_err_state->stack_last;
  fn_err_state->stack_last = stack->prev;
  free(stack);
}

void error_stack_pos (err_state_t *fn_err_state, int line, int col) {
  if (line != 0) fn_err_state->stack_last->line = line;
  if (col != 0) fn_err_state->stack_last->col = col;
}

void error_stack_push (err_state_t *fn_err_state, const wchar_t *file, const wchar_t *name, int line, int col) {
  error_stack_pos(fn_err_state, line, col);
  err_stack_t *stack = safe_malloc(sizeof(err_stack_t));
  stack->file = file;
  stack->name = name;
  stack->next = NULL;
  stack->prev = fn_err_state->stack_last;
  if (fn_err_state->stack_first == NULL) fn_err_state->stack_first = stack;
  if (fn_err_state->stack_last != NULL) fn_err_state->stack_last->next = stack;
  fn_err_state->stack_last = stack;
}

void error_stack_str (err_state_t *fn_err_state) {
  Error_t *err = fn_err_state->ctx;
  err->stack = str_realloc(err->stack, err->message);
  for (err_stack_t *it = fn_err_state->stack_last; it != NULL; it = it->prev) {
    wchar_t *fmt;
    size_t z;
    if (it->col == 0 && it->line == 0) {
      fmt = __THE_EOL L"  at %s (%s)";
      z = swprintf(NULL, 0, fmt, it->name, it->file);
    } else if (it->col == 0) {
      fmt = __THE_EOL L"  at %s (%s:%d)";
      z = swprintf(NULL, 0, fmt, it->name, it->file, it->line);
    } else {
      fmt = __THE_EOL L"  at %s (%s:%d:%d)";
      z = swprintf(NULL, 0, fmt, it->name, it->file, it->line, it->col);
    }
    err->stack.d = safe_realloc(err->stack.d, (err->stack.l + z + 1) * sizeof(wchar_t));
    if (it->col == 0 && it->line == 0) {
      swprintf(&err->stack.d[err->stack.l], z, fmt, it->name, it->file);
    } else if (it->col == 0) {
      swprintf(&err->stack.d[err->stack.l], z, fmt, it->name, it->file, it->line);
    } else {
      swprintf(&err->stack.d[err->stack.l], z, fmt, it->name, it->file, it->line, it->col);
    }
    err->stack.l += z;
  }
}

void error_unset (err_state_t *fn_err_state) {
  fn_err_state->id = -1;
  fn_err_state->_free = NULL;
}

str_t f32_str (float self) {
  char buf[512];
  sprintf(buf, "%f", self);
  return str_alloc(buf);
}

str_t f64_str (double self) {
  char buf[512];
  sprintf(buf, "%f", self);
  return str_alloc(buf);
}

str_t float_str (double self) {
  char buf[512];
  sprintf(buf, "%f", self);
  return str_alloc(buf);
}

str_t i8_str (int8_t self) {
  char buf[24];
  sprintf(buf, "%" PRId8, self);
  return str_alloc(buf);
}

str_t i16_str (int16_t self) {
  char buf[24];
  sprintf(buf, "%" PRId16, self);
  return str_alloc(buf);
}

str_t i32_str (int32_t self) {
  char buf[24];
  sprintf(buf, "%" PRId32, self);
  return str_alloc(buf);
}

str_t i64_str (int64_t self) {
  char buf[24];
  sprintf(buf, "%" PRId64, self);
  return str_alloc(buf);
}

str_t int_str (int32_t self) {
  char buf[24];
  sprintf(buf, "%" PRId32, self);
  return str_alloc(buf);
}

str_t isize_str (size_t self) {
  char buf[24];
  sprintf(buf, "%zd", self);
  return str_alloc(buf);
}

str_t u8_str (uint8_t self) {
  char buf[24];
  sprintf(buf, "%" PRIu8, self);
  return str_alloc(buf);
}

str_t u16_str (uint16_t self) {
  char buf[24];
  sprintf(buf, "%" PRIu16, self);
  return str_alloc(buf);
}

str_t u32_str (uint32_t self) {
  char buf[24];
  sprintf(buf, "%" PRIu32, self);
  return str_alloc(buf);
}

str_t u64_str (uint64_t self) {
  char buf[24];
  sprintf(buf, "%" PRIu64, self);
  return str_alloc(buf);
}

str_t usize_str (ssize_t self) {
  char buf[24];
  sprintf(buf, "%zu", self);
  return str_alloc(buf);
}

void *safe_malloc (size_t size) {
  void *d = malloc(size);
  if (d == NULL) error_alloc(&err_state, size);
  return d;
}

void *safe_realloc (void *self, size_t size) {
  void *d = realloc(self, size);
  if (d == NULL) error_alloc(&err_state, size);
  return d;
}

void safe_swap (void *a, void *b, size_t size) {
  void *tmp = safe_malloc(size);
  memcpy(tmp, a, size);
  memcpy(a, b, size);
  memcpy(b, tmp, size);
  free(tmp);
}

str_t str_alloc (const char *self) {
  size_t l = strlen(self);
  if (l == 0) return (str_t) {NULL, 0};
  wchar_t *d = safe_malloc(l * sizeof(wchar_t));
  for (size_t i = 0; i < l; i++) d[i] = (wchar_t) self[i];
  return (str_t) {d, l};
}

str_t str_calloc (const wchar_t *self, size_t size) {
  if (size == 0) return (str_t) {NULL, 0};
  wchar_t *d = safe_malloc(size * sizeof(wchar_t));
  wmemcpy(d, self, size);
  return (str_t) {d, size};
}

str_t str_valloc (const wchar_t *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t l = vswprintf(NULL, 0, fmt, args);
  wchar_t *d = safe_malloc((l + 1) * sizeof(wchar_t));
  vswprintf(d, l, fmt, args);
  va_end(args);
  return (str_t) {d, l};
}

wchar_t *str_at (err_state_t *fn_err_state, int line, int col, const str_t self, int32_t index) {
  if ((index >= 0 && index >= self.l) || (index < 0 && index < -((int32_t) self.l))) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"index %" PRId32 L" out of string bounds", index));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  return index < 0 ? &self.d[self.l + index] : &self.d[index];
}

str_t str_concat (const str_t self, const str_t other) {
  size_t l = self.l + other.l;
  wchar_t *d = safe_malloc(l * sizeof(wchar_t));
  wmemcpy(d, self.d, self.l);
  wmemcpy(&d[self.l], other.d, other.l);
  return (str_t) {d, l};
}

bool str_contains (const str_t self, const str_t search) {
  bool r = self.l == 0;
  if (!r && self.l == search.l) {
    r = wmemcmp(self.d, search.d, self.l) == 0;
  } else if (!r && self.l > search.l) {
    for (size_t i = 0; i < self.l - search.l; i++) {
      if (wmemcmp(&self.d[i], search.d, search.l) == 0) {
        r = true;
        break;
      }
    }
  }
  return r;
}

str_t str_copy (const str_t self) {
  wchar_t *d = safe_malloc(self.l * sizeof(wchar_t));
  wmemcpy(d, self.d, self.l);
  return (str_t) {d, self.l};
}

wchar_t *str_cstr (const str_t self) {
  wchar_t *d = safe_malloc((self.l + 1) * sizeof(wchar_t));
  wmemcpy(d, self.d, self.l);
  d[self.l] = L'\0';
  return d;
}

bool str_empty (const str_t self) {
  return self.l == 0;
}

bool str_eq (const str_t self, const str_t rhs) {
  return self.l == rhs.l && wmemcmp(self.d, rhs.d, self.l) == 0;
}

str_t str_escape (const str_t self) {
  wchar_t *d = safe_malloc(self.l * sizeof(wchar_t));
  size_t l = 0;
  for (size_t i = 0; i < self.l; i++) {
    wchar_t c = self.d[i];
    if (c == L'\f' || c == L'\n' || c == L'\r' || c == L'\t' || c == L'\v' || c == L'"') {
      if (l + 2 > self.l) d = safe_realloc(d, (l + 2) * sizeof(wchar_t));
      d[l++] = L'\\';
      if (c == L'\f') d[l++] = L'f';
      else if (c == L'\n') d[l++] = L'n';
      else if (c == L'\r') d[l++] = L'r';
      else if (c == L'\t') d[l++] = L't';
      else if (c == L'\v') d[l++] = L'v';
      else if (c == L'"') d[l++] = L'"';
      continue;
    }
    if (l + 1 > self.l) d = safe_realloc(d, (l + 1) * sizeof(wchar_t));
    d[l++] = c;
  }
  return (str_t) {d, l};
}

int32_t str_find (const str_t self, const str_t search) {
  int32_t r = -1;
  for (size_t i = 0; i < self.l; i++) {
    if (wmemcmp(&self.d[i], search.d, search.l) == 0) {
      r = (int32_t) i;
      break;
    }
  }
  return r;
}

void str_free (str_t self) {
  free(self.d);
}

bool str_ge (const str_t self, const str_t rhs) {
  return wmemcmp(self.d, rhs.d, self.l > rhs.l ? self.l : rhs.l) >= 0;
}

bool str_gt (const str_t self, const str_t rhs) {
  return wmemcmp(self.d, rhs.d, self.l > rhs.l ? self.l : rhs.l) > 0;
}

bool str_le (const str_t self, const str_t rhs) {
  return wmemcmp(self.d, rhs.d, self.l > rhs.l ? self.l : rhs.l) <= 0;
}

size_t str_len (const str_t self) {
  return self.l;
}

bool str_lt (const str_t self, const str_t rhs) {
  return wmemcmp(self.d, rhs.d, self.l > rhs.l ? self.l : rhs.l) < 0;
}

bool str_ne (const str_t self, const str_t rhs) {
  return self.l != rhs.l || wmemcmp(self.d, rhs.d, self.l) != 0;
}

str_t str_realloc (str_t self, const str_t rhs) {
  str_free(self);
  return str_copy(rhs);
}

arr_str_t str_lines (const str_t self, unsigned char o1, bool keepLineBreaks) {
  if (self.l != 0) return (arr_str_t) {NULL, 0};
  bool k = o1 == 0 ? false : keepLineBreaks;
  str_t *r = NULL;
  size_t l = 0;
  wchar_t *d = safe_malloc(self.l * sizeof(wchar_t));
  size_t i = 0;
  for (size_t j = 0; j < self.l; j++) {
    wchar_t c = self.d[j];
    if (c == L'\r' || c == L'\n') {
      if (k) d[i++] = c;
      if (c == L'\r' && j + 1 < self.l && self.d[j + 1] == L'\n') {
        j++;
        if (k) d[i++] = self.d[j];
      }
      wchar_t *a = safe_malloc(i * sizeof(wchar_t));
      wmemcpy(a, d, i);
      r = safe_realloc(r, ++l * sizeof(str_t));
      r[l - 1] = (str_t) {a, i};
      i = 0;
    } else {
      d[i++] = c;
    }
  }
  if (i != 0) {
    wchar_t *a = safe_malloc(i * sizeof(wchar_t));
    wmemcpy(a, d, i);
    r = safe_realloc(r, ++l * sizeof(str_t));
    r[l - 1] = (str_t) {a, i};
  }
  free(d);
  return (arr_str_t) {r, l};
}

str_t str_lower (const str_t self) {
  str_t d = str_copy(self);
  if (self.l == 0) return d;
  for (size_t i = 0; i < d.l; i++) d.d[i] = (wchar_t) tolower(d.d[i]);
  return d;
}

str_t str_lowerFirst (const str_t self) {
  str_t d = str_copy(self);
  if (self.l == 0) return d;
  d.d[0] = (wchar_t) tolower(d.d[0]);
  return d;
}

bool str_not (const str_t self) {
  return self.l == 0;
}

str_t str_replace (const str_t self, const str_t search, const str_t replacement, unsigned char o3, int32_t count) {
  size_t l = 0;
  wchar_t *d = NULL;
  int32_t k = 0;
  if (search.l == 0 && replacement.l > 0) {
    l = self.l + (count > 0 && count <= self.l ? count : self.l + 1) * replacement.l;
    d = safe_malloc(l * sizeof(wchar_t));
    wmemcpy(d, replacement.d, replacement.l);
    size_t j = replacement.l;
    for (size_t i = 0; i < self.l; i++) {
      d[j++] = self.d[i];
      if (count <= 0 || ++k < count) {
        wmemcpy(&d[j], replacement.d, replacement.l);
        j += replacement.l;
      }
    }
  } else if (self.l == search.l && search.l > 0) {
    if (wmemcmp(self.d, search.d, search.l) != 0) {
      l = self.l;
      d = safe_malloc(l * sizeof(wchar_t));
      wmemcpy(d, self.d, l);
    } else if (replacement.l > 0) {
      l = replacement.l;
      d = safe_malloc(l * sizeof(wchar_t));
      wmemcpy(d, replacement.d, l);
    }
  } else if (self.l > search.l && search.l > 0 && replacement.l == 0) {
    d = safe_malloc(self.l * sizeof(wchar_t));
    for (size_t i = 0; i < self.l; i++) {
      if (i <= self.l - search.l && wmemcmp(&self.d[i], search.d, search.l) == 0 && (count <= 0 || k++ < count)) {
        i += search.l - 1;
      } else {
        d[l++] = self.d[i];
      }
    }
    if (l == 0) {
      free(d);
      d = NULL;
    } else if (l != self.l) {
      d = safe_realloc(d, l * sizeof(wchar_t));
    }
  } else if (self.l > search.l && search.l > 0 && replacement.l > 0) {
    l = self.l;
    d = safe_malloc(l * sizeof(wchar_t));
    size_t j = 0;
    for (size_t i = 0; i < self.l; i++) {
      if (i <= self.l - search.l && wmemcmp(&self.d[i], search.d, search.l) == 0 && (count <= 0 || k++ < count)) {
        if (search.l < replacement.l) {
          l += replacement.l - search.l;
          if (l > self.l) {
            d = safe_realloc(d, l * sizeof(wchar_t));
          }
        } else if (search.l > replacement.l) {
          l -= search.l - replacement.l;
        }
        wmemcpy(&d[j], replacement.d, replacement.l);
        j += replacement.l;
        i += search.l - 1;
      } else {
        d[j++] = self.d[i];
      }
    }
    d = safe_realloc(d, l * sizeof(wchar_t));
  } else if (self.l > 0) {
    l = self.l;
    d = safe_malloc(l * sizeof(wchar_t));
    wmemcpy(d, self.d, l);
  }
  return (str_t) {d, l};
}

str_t str_slice (const str_t self, unsigned char o1, int32_t start, unsigned char o2, int32_t end) {
  int32_t i = o1 == 0 ? 0 : (int32_t) (start < 0 ? (start < -((int32_t) self.l) ? 0 : start + self.l) : (start > self.l ? self.l : start));
  int32_t j = o2 == 0 ? (int32_t) self.l : (int32_t) (end < 0 ? (end < -((int32_t) self.l) ? 0 : end + self.l) : (end > self.l ? self.l : end));
  if (i >= j || i >= self.l) return str_alloc("");
  size_t l = j - i;
  return str_calloc(&self.d[i], l);
}

arr_str_t str_split (const str_t self, unsigned char o1, const str_t delimiter) {
  str_t *r = NULL;
  size_t l = 0;
  if (self.l > 0 && delimiter.l == 0) {
    l = self.l;
    r = safe_malloc(l * sizeof(str_t));
    for (size_t i = 0; i < l; i++) {
      r[i] = str_calloc(&self.d[i], 1);
    }
  } else if (self.l < delimiter.l) {
    r = safe_realloc(r, ++l * sizeof(str_t));
    r[0] = str_calloc(self.d, self.l);
  } else if (delimiter.l > 0) {
    size_t i = 0;
    for (size_t j = 0; j <= self.l - delimiter.l; j++) {
      if (wmemcmp(&self.d[j], delimiter.d, delimiter.l) == 0) {
        r = safe_realloc(r, ++l * sizeof(str_t));
        r[l - 1] = str_calloc(&self.d[i], j - i);
        j += delimiter.l;
        i = j;
      }
    }
    r = safe_realloc(r, ++l * sizeof(str_t));
    r[l - 1] = str_calloc(&self.d[i], self.l - i);
  }
  return (arr_str_t) {r, l};
}

double str_toFloat (err_state_t *fn_err_state, int line, int col, const str_t self) {
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  double r = wcstod(c, &e);
  if (errno == ERANGE || r < -DBL_MAX || DBL_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

float str_toF32 (err_state_t *fn_err_state, int line, int col, const str_t self) {
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  float r = wcstof(c, &e);
  if (errno == ERANGE || r < -FLT_MAX || FLT_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

double str_toF64 (err_state_t *fn_err_state, int line, int col, const str_t self) {
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  double r = wcstod(c, &e);
  if (errno == ERANGE || r < -DBL_MAX || DBL_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

ssize_t str_toIsize (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  long long r = wcstoll(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || r < ((-SSIZE_MAX) - 1) || SSIZE_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

int8_t str_toI8 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  long r = wcstol(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || r < INT8_MIN || INT8_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

int16_t str_toI16 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  long r = wcstol(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || r < INT16_MIN || INT16_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

int32_t str_toI32 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  long r = wcstol(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || r < INT32_MIN || INT32_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

int64_t str_toI64 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  long long r = wcstoll(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || r < INT64_MIN || INT64_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

size_t str_toUsize (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  unsigned long long r = wcstoull(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || SIZE_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == L'-') {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

uint8_t str_toU8 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  unsigned long r = wcstoul(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || UINT8_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == L'-') {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

uint16_t str_toU16 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  unsigned long r = wcstoul(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || UINT16_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == L'-') {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

uint32_t str_toU32 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  unsigned long r = wcstoul(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || UINT32_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == L'-') {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

uint64_t str_toU64 (err_state_t *fn_err_state, int line, int col, const str_t self, unsigned char o1, int32_t radix) {
  if (o1 == 1 && (radix < 2 || radix > 36) && radix != 0) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"radix %" PRId32 L" is invalid, must be >= 2 and <= 36, or 0", radix));
    longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  }
  wchar_t *c = str_cstr(self);
  wchar_t *e = NULL;
  errno = 0;
  unsigned long long r = wcstoull(c, &e, o1 == 0 ? 10 : radix);
  if (errno == ERANGE || UINT64_MAX < r) {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` out of range", c));
  } else if (errno != 0 || e == c || *e != 0 || self.d[0] == L'-') {
    error_assign_builtin(fn_err_state, line, col, str_valloc(L"value `%ls` has invalid syntax", c));
  }
  free(c);
  if (fn_err_state->id != -1) longjmp(fn_err_state->buf_last->buf, fn_err_state->id);
  return r;
}

str_t str_trim (const str_t self) {
  if (self.l == 0) return self;
  size_t i = 0;
  size_t j = self.l;
  while (i < self.l && isspace(self.d[i])) i++;
  while (j >= 0 && isspace(self.d[j - 1])) {
    j--;
    if (j == 0) break;
  }
  if (i >= j) return str_alloc("");
  size_t l = j - i;
  return str_calloc(&self.d[i], l);
}

str_t str_trimEnd (const str_t self) {
  if (self.l == 0) return self;
  size_t l = self.l;
  while (isspace(self.d[l - 1])) {
    l--;
    if (l == 0) return str_alloc("");
  }
  return str_calloc(self.d, l);
}

str_t str_trimStart (const str_t self) {
  if (self.l == 0) return self;
  size_t i = 0;
  while (i < self.l && isspace(self.d[i])) i++;
  if (i >= self.l) return str_alloc("");
  size_t l = self.l - i;
  return str_calloc(&self.d[i], l);
}

str_t str_upper (const str_t self) {
  str_t d = str_copy(self);
  if (self.l == 0) return d;
  for (size_t i = 0; i < d.l; i++) d.d[i] = (wchar_t) toupper(d.d[i]);
  return d;
}

str_t str_upperFirst (const str_t self) {
  str_t d = str_copy(self);
  if (self.l == 0) return d;
  d.d[0] = (wchar_t) toupper(d.d[0]);
  return d;
}
