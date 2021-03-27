#include <duc/testing.h>
#include "../src/ast.h"

DUC_TEST(ast, new_and_free) {
  char *filepath = "../test.txt";

  duc_writefile(filepath, "");
  duc_file_t *file = duc_file_new(filepath, DUC_FILE_READ);
  ast_t* ast = ast_new(file);

  DUC_ASSERT_NE(ast, NULL);
  DUC_ASSERT_TRUE(duc_array_empty(ast->parsers));
  ast_free(ast);
  duc_file_free(file);

  duc_writefile(filepath, " ");
  file = duc_file_new(filepath, DUC_FILE_READ);
  ast = ast_new(file);

  DUC_ASSERT_NE(ast, NULL);
  DUC_ASSERT_TRUE(duc_array_empty(ast->parsers));
  ast_free(ast);
  duc_file_free(file);

  duc_writefile(filepath, "test()");
  file = duc_file_new(filepath, DUC_FILE_READ);
  ast = ast_new(file);

  DUC_ASSERT_NE(ast, NULL);
  DUC_ASSERT_EQ(duc_array_length(ast->parsers), 1);
  ast_free(ast);
  duc_file_free(file);

  duc_writefile(filepath, " test()");
  file = duc_file_new(filepath, DUC_FILE_READ);
  ast = ast_new(file);

  DUC_ASSERT_NE(ast, NULL);
  DUC_ASSERT_EQ(duc_array_length(ast->parsers), 1);
  ast_free(ast);
  duc_file_free(file);

  duc_writefile(filepath, "test()test()");
  file = duc_file_new(filepath, DUC_FILE_READ);
  ast = ast_new(file);

  DUC_ASSERT_NE(ast, NULL);
  DUC_ASSERT_EQ(duc_array_length(ast->parsers), 2);
  ast_free(ast);
  duc_file_free(file);

  duc_writefile(filepath, "test() test()");
  file = duc_file_new(filepath, DUC_FILE_READ);
  ast = ast_new(file);

  DUC_ASSERT_NE(ast, NULL);
  DUC_ASSERT_EQ(duc_array_length(ast->parsers), 2);
  ast_free(ast);
  duc_file_free(file);
}

int main () {
  DUC_TEST_RUN(ast, new_and_free);
}
