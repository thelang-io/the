/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TESTS_PARSER_TEST_H
#define TESTS_PARSER_TEST_H

#include "testing.h"

#define PARSER_F(text, token, body) \
  do { writefile("../test.out", text); \
  file_t *file = file_new("../test.out", FILE_READ); \
  parser_##token##_t *parser = parser_##token##_new_(file); \
  body \
  if (parser != NULL) parser_##token##_free_(parser); \
  file_free(file); \
  file_remove("../test.out"); } while (0)

#endif
