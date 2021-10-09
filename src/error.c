/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

void throw_error (const char *fmt, ...) {
  char *err_fmt = "Error: %s\n";
  size_t err_fmt_len = (size_t) snprintf(NULL, 0, err_fmt, fmt);
  char *fmt_buf = malloc(err_fmt_len + 1);
  sprintf(fmt_buf, err_fmt, fmt);

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt_buf, args);
  va_end(args);

  free(fmt_buf);
  exit(EXIT_FAILURE);
}

void throw_syntax_error (reader_t *reader, reader_location_t start, const char *msg) {
  reader_location_t end = reader->loc;
  reader_seek(reader, (reader_location_t){start.pos - start.col, start.line, 0});
  size_t line_len = 0;
  char *line = malloc(line_len + 1);

  while (!reader_eof(reader)) {
    char ch = reader_next(reader);

    if (ch == '\n') {
      break;
    }

    line = realloc(line, ++line_len + 1);
    line[line_len - 1] = ch;
  }

  line[line_len] = '\0';

  size_t tilde_len = start.line == end.line
    ? line_len > end.col
      ? end.col - start.col
      : line_len > start.col
        ? line_len - start.col
        : 0
    : line_len > start.col
      ? line_len - start.col
      : 0;

  size_t line_num_len = (size_t) snprintf(NULL, 0, "%lu", start.line);
  char *line_num_str = malloc(line_num_len + 1);
  char *col_num_str = malloc((size_t) snprintf(NULL, 0, "%lu", start.col + 1) + 1);
  sprintf(line_num_str, "%lu", start.line);
  sprintf(col_num_str, "%lu", start.col + 1);

  char *line_num_spaces = malloc(line_num_len + 1);
  char *col_num_spaces = malloc(start.col + 1);
  for (size_t i = 0; i < line_num_len; i++) line_num_spaces[i] = ' ';
  for (size_t i = 0; i < start.col; i++) col_num_spaces[i] = ' ';
  line_num_spaces[line_num_len] = '\0';
  col_num_spaces[start.col] = '\0';

  char *tilde_str = NULL;

  if (tilde_len != 0) {
    tilde_str = malloc(tilde_len);
    for (size_t i = 0; i < tilde_len - 1; i++) tilde_str[i] = '~';
    tilde_str[tilde_len - 1] = '\0';
  }

  printf(
    "SyntaxError: %s:%s:%s: %s\n"
    "  %s | %s\n"
    "  %s | %s^%s\n",
    reader->path,
    line_num_str,
    col_num_str,
    msg,
    line_num_str,
    line,
    line_num_spaces,
    col_num_spaces,
    tilde_str == NULL ? "" : tilde_str
  );

  free(tilde_str);
  free(col_num_spaces);
  free(line_num_spaces);
  free(col_num_str);
  free(line_num_str);
  free(line);
  exit(EXIT_FAILURE);
}
