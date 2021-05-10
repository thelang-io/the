/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

void free_simple (void *it) {
  if (it != NULL) {
    free(it);
  }
}

void throw (const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}
