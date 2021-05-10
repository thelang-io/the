/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../src/ast.h"
#include "testing.h"

#define AST_F(text, body) \
  do { writefile("../test.txt", text); \
  writefile("../test.txt", text); \
  file_t *file = file_new("../test.txt", FILE_READ); \
  ast_t* ast = ast_new(file); \
  body \
  if (ast != NULL) ast_free(ast); \
  file_free(file); \
  file_remove("../test.txt"); } while (0)

TEST(ast, new_and_free) {
  AST_F("", {
    ASSERT_NE(ast, NULL);
    ASSERT_TRUE(array_empty(ast->parsers));
  });

  AST_F(" ", {
    ASSERT_NE(ast, NULL);
    ASSERT_TRUE(array_empty(ast->parsers));
  });

  AST_F("print()", {
    ASSERT_NE(ast, NULL);
    ASSERT_EQ(array_length(ast->parsers), 1);
  });

  AST_F(" print()", {
    ASSERT_NE(ast, NULL);
    ASSERT_EQ(array_length(ast->parsers), 1);
  });

  AST_F("print()print()", {
    ASSERT_NE(ast, NULL);
    ASSERT_EQ(array_length(ast->parsers), 2);
  });

  AST_F("print() print()", {
    ASSERT_NE(ast, NULL);
    ASSERT_EQ(array_length(ast->parsers), 2);
  });
}

int main () {
  TEST_RUN(ast, new_and_free);
}
