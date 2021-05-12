/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../src/parser.h"
#include "testing.h"

TEST(parser, new_and_free) {
  char *filepath = "../test.out";

  writefile(filepath, "id");
  file_t *file = file_new(filepath, FILE_READ);
  parser_t *parser = parser_new(file);
  ASSERT_NE(parser, NULL);
  ASSERT_EQ(parser->tok, PARSER_ID);
  parser_free(parser);
  file_free(file);

  writefile(filepath, "@");
  file = file_new(filepath, FILE_READ);
  parser = parser_new(file);
  ASSERT_EQ(parser, NULL);
  ASSERT_EQ(file_position(file), 0);
  file_free(file);

  file_remove(filepath);
}

int main () {
  TEST_RUN(parser, new_and_free);
}
