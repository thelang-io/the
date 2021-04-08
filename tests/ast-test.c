/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../src/ast.h"

#define AST_F(text, body) \
  do { duc_writefile("../test.txt", text); \
  duc_writefile("../test.txt", text); \
  duc_file_t *file = duc_file_new("../test.txt", DUC_FILE_READ); \
  ast_t* ast = ast_new(file); \
  body \
  if (ast != NULL) ast_free(ast); \
  duc_file_free(file); \
  duc_file_remove("../test.txt"); } while (0)

DUC_TEST(ast, new_and_free) {
  AST_F("", {
    DUC_ASSERT_NE(ast, NULL);
    DUC_ASSERT_TRUE(duc_array_empty(ast->parsers));
  });

  AST_F(" ", {
    DUC_ASSERT_NE(ast, NULL);
    DUC_ASSERT_TRUE(duc_array_empty(ast->parsers));
  });

  AST_F("print()", {
    DUC_ASSERT_NE(ast, NULL);
    DUC_ASSERT_EQ(duc_array_length(ast->parsers), 1);
  });

  AST_F(" print()", {
    DUC_ASSERT_NE(ast, NULL);
    DUC_ASSERT_EQ(duc_array_length(ast->parsers), 1);
  });

  AST_F("print()puts()", {
    DUC_ASSERT_NE(ast, NULL);
    DUC_ASSERT_EQ(duc_array_length(ast->parsers), 2);
  });

  AST_F("print() puts()", {
    DUC_ASSERT_NE(ast, NULL);
    DUC_ASSERT_EQ(duc_array_length(ast->parsers), 2);
  });
}

int main () {
  DUC_TEST_RUN(ast, new_and_free);
}
