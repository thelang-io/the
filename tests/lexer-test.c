/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../src/lexer.h"

DUC_TEST(lexer, new_and_free) {
  char *filepath = "../test.txt";

  duc_writefile(filepath, " ");
  duc_file_t *file = duc_file_new(filepath, DUC_FILE_READ);
  lexer_t *lexer = lexer_new(file);

  DUC_ASSERT_NE(lexer, NULL);
  lexer_free(lexer);
  duc_file_free(file);

  duc_writefile(filepath, "@");
  file = duc_file_new(filepath, DUC_FILE_READ);
  lexer = lexer_new(file);

  DUC_ASSERT_NE(lexer, NULL);
  DUC_ASSERT_EQ(lexer->raw, NULL);
  DUC_ASSERT_EQ(lexer->str, NULL);
  DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
  DUC_ASSERT_EQ(duc_file_position(file), 0);
  duc_file_free(file);

  duc_file_remove(filepath);
}

int main () {
  DUC_TEST_RUN(lexer, new_and_free);
}
