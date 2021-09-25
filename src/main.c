/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "reader.h"

int main () {
  reader_t *reader = reader_init("reader-test.adl");

  while (!reader_eof(reader)) {
    const char ch = reader_next(reader);
    printf("%c", ch);
  }

  reader_free(reader);
  return 0;
}
