/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <sys/stat.h>
#include <stdlib.h>
#include "error.h"
#include "reader.h"

reader_t *reader_init (const char *path) {
  char *resolved_path = realpath(path, NULL);

  if (resolved_path == NULL) {
    throw_error("No such file \"%s\"", path);
  }

  struct stat path_stat;
  stat(resolved_path, &path_stat);

  if (!S_ISREG(path_stat.st_mode)) {
    throw_error("Path \"%s\" is not a file", path);
  }

  FILE *fp = fopen(path, "r");

  if (fp == NULL) {
    throw_error("Unable to read file \"%s\"", path);
  }

  fseek(fp, 0, SEEK_END);
  size_t content_size = (size_t) ftell(fp);
  fseek(fp, 0, SEEK_SET);

  reader_t *this = malloc(sizeof(reader_t));

  if (this == NULL) {
    throw_error("Unable to allocate memory for reader");
  }

  this->path = resolved_path;
  this->fp = fp;
  this->content = NULL;
  this->size = 0;
  this->loc = reader_location_init();

  if (content_size > 0) {
    this->content = malloc(content_size);
    this->size = content_size;

    if (this->content == NULL) {
      throw_error("Unable to allocate %lu bytes for reader content", content_size);
    } else if (fread(this->content, content_size, 1, fp) != 1) {
      throw_error("Unable to read file \"%s\"", path);
    }
  }

  return this;
}

void reader_free (reader_t *this) {
  if (this->content != NULL) {
    free(this->content);
  }

  fclose(this->fp);
  free(this->path);
  free(this);
}

bool reader_eof (const reader_t *this) {
  return this->loc.pos >= this->size;
}

char reader_next (reader_t *this) {
  if (reader_eof(this)) {
    throw_error("Tried to read on file end");
  }

  char ch = this->content[this->loc.pos];

  if (ch == '\n') {
    this->loc.col = 0;
    this->loc.line += 1;
  } else {
    this->loc.col += 1;
  }

  this->loc.pos += 1;
  return ch;
}

void reader_seek (reader_t *this, reader_location_t loc) {
  this->loc = loc;
}

reader_location_t reader_location_init () {
  reader_location_t rl = { 0, 1, 0 };
  return rl;
}
