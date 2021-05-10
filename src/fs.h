/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_FS_H
#define SRC_FS_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct file_s file_t;

typedef enum {
  FILE_APPEND,
  FILE_READ,
  FILE_WRITE
} file_mode;

struct file_s {
  bool eof;
  FILE *fp;
  char path[PATH_MAX];
};

bool file_eof (const file_t *file);
bool file_exists (const char *filepath);
void file_free (file_t *file);
file_t *file_new (const char *filepath, file_mode mode);
size_t file_position (const file_t *file);
unsigned char file_readchar (file_t *file);
char *file_readline (file_t *file);
void file_remove (const char *filepath);
void file_seek (file_t *file, size_t pos);
void file_writechar (file_t *file, unsigned char ch);
void file_writeline (file_t *file, const char *line);

char *path_cwd ();
char *path_real (const char *path);

char *readfile (const char *filepath);
void writefile (const char *filepath, const char *content);

#endif
