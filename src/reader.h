/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_READER_H
#define SRC_READER_H

#include <stdbool.h>
#include <stdio.h>

typedef struct reader_s reader_t;
typedef struct reader_location_s reader_location_t;

struct reader_location_s {
  size_t pos;
  size_t line;
  size_t col;
};

struct reader_s {
  char *content;
  reader_location_t loc;
  FILE *fp;
  char *path;
  size_t size;
};

reader_t *reader_init (const char *path);
void reader_free (reader_t *reader);

bool reader_eof (const reader_t *reader);
char reader_next (reader_t *reader);
void reader_seek (reader_t *reader, reader_location_t loc);

#endif
