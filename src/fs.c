/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fs.h"

bool file_eof (const file_t *file) {
  return file->eof;
}

bool file_exists (const char *filepath) {
  return access(filepath, F_OK) == 0;
}

void file_free (file_t *file) {
  fclose(file->fp);
  free(file);
}

file_t *file_new (const char *filepath, file_mode mode) {
  file_t *file = malloc(sizeof(file_t));

  if (file == NULL) {
    return NULL;
  }

  char *open_mode = "w";

  if (mode == FILE_READ) {
    open_mode = "r";
  } else if (mode == FILE_APPEND) {
    open_mode = "a";
  }

  file->fp = fopen(filepath, open_mode);

  if (file->fp == NULL) {
    free(file);
    return NULL;
  }

  char *path = path_real(filepath);

  if (path == NULL) {
    fclose(file->fp);
    free(file);
    return NULL;
  }

  file->eof = feof(file->fp);
  strcpy(file->path, path);
  free(path);

  if (!file->eof) {
    file->eof = fgetc(file->fp) == EOF;
    fseek(file->fp, -1, SEEK_CUR);
  }

  return file;
}

size_t file_position (const file_t *file) {
  return (size_t) ftell(file->fp);
}

unsigned char file_readchar (file_t *file) {
  int res = fgetc(file->fp);
  file->eof = res == EOF;

  if (!file->eof) {
    file->eof = fgetc(file->fp) == EOF;
    fseek(file->fp, -1, SEEK_CUR);
  }

  return res <= 0 ? '\0' : (unsigned char) res;
}

char *file_readline (file_t *file) {
  char *line = NULL;
  size_t len = 0;
  file->eof = getline(&line, &len, file->fp) == -1;

  if (!file->eof) {
    file->eof = fgetc(file->fp) == EOF;
    fseek(file->fp, -1, SEEK_CUR);
  }

  return line;
}

void file_remove (const char *filepath) {
  remove(filepath);
}

void file_seek (file_t *file, size_t pos) {
  fseek(file->fp, (ssize_t) pos, SEEK_SET);
  file->eof = feof(file->fp);
}

void file_writechar (file_t *file, unsigned char ch) {
  fputc(ch, file->fp);
}

void file_writeline (file_t *file, const char *line) {
  fputs(line, file->fp);
}

char *path_cwd () {
  char *cwd = malloc(PATH_MAX);

  if (cwd == NULL) {
    return NULL;
  } else if (getcwd(cwd, PATH_MAX) == NULL) {
    free(cwd);
    return NULL;
  }

  return cwd;
}

char *path_real (const char *path) {
  char *real_path = malloc(PATH_MAX);

  if (real_path == NULL) {
    return NULL;
  } else if (realpath(path, real_path) == NULL) {
    free(real_path);
    return NULL;
  }

  return real_path;
}

char *readfile (const char *filepath) {
  FILE *fp = fopen(filepath, "rb");
  fseek(fp, 0, SEEK_END);
  size_t size = (size_t) ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *result = malloc(size + 1);

  if (fread(result, 1, size, fp) != size) {
    duc_throw("Failed to read from file");
  }

  result[size] = '\0';
  fclose(fp);

  return result;
}

void writefile (const char *filepath, const char *content) {
  FILE *fp = fopen(filepath, "wb");
  fwrite(content, strlen(content), 1, fp);
  fclose(fp);
}
