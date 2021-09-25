/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <sys/stat.h>
#include <stdlib.h>
#include "reader.h"

reader_t *reader_init (const char *path) {
  char *resolved_path = realpath(path, NULL);

  if (resolved_path == NULL) {
    fprintf(stderr, "No such file \"%s\"\n", path);
    exit(EXIT_FAILURE);
  }

  struct stat path_stat;
  stat(resolved_path, &path_stat);

  if (!S_ISREG(path_stat.st_mode)) {
    fprintf(stderr, "Path \"%s\" is not a file\n", path);
    exit(EXIT_FAILURE);
  }

  FILE *fp = fopen(path, "r");

  if (fp == NULL) {
    fprintf(stderr, "Unable to read file \"%s\"\n", path);
    exit(EXIT_FAILURE);
  }

  fseek(fp, 0, SEEK_END);
  const size_t content_size = (size_t) ftell(fp);
  fseek(fp, 0, SEEK_SET);

  reader_t *reader = malloc(sizeof(reader_t));

  if (reader == NULL) {
    fprintf(stderr, "Unable to allocate memory for reader\n");
    exit(EXIT_FAILURE);
  }

  reader->content = NULL;
  reader->fp = fp;
  reader->loc.pos = 0;
  reader->loc.line = 1;
  reader->loc.col = 0;
  reader->path = resolved_path;
  reader->size = 0;

  if (content_size > 0) {
    reader->content = malloc(content_size);
    reader->size = content_size;

    if (reader->content == NULL) {
      fprintf(stderr, "Unable to allocate %lu bytes for reader content\n", content_size);
      exit(EXIT_FAILURE);
    } else if (fread(reader->content, content_size, 1, fp) != 1) {
      fprintf(stderr, "Unable to fully read file \"%s\"\n", path);
      exit(EXIT_FAILURE);
    }
  }

  return reader;
}

void reader_free (reader_t *reader) {
  if (reader->content != NULL) {
    free(reader->content);
  }

  fclose(reader->fp);
  free(reader->path);
  free(reader);
}

bool reader_eof (const reader_t *reader) {
  return reader->loc.pos >= reader->size;
}

char reader_next (reader_t *reader) {
  if (reader_eof(reader)) {
    fprintf(stderr, "Tried to read on file end\n");
    exit(EXIT_FAILURE);
  }

  const char ch = reader->content[reader->loc.pos];

  if (ch == '\n') {
    reader->loc.col = 0;
    reader->loc.line += 1;
  } else {
    reader->loc.col += 1;
  }

  reader->loc.pos += 1;
  return ch;
}

void reader_seek (reader_t *reader, reader_location_t loc) {
  reader->loc = loc;
}
