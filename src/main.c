/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <sys/stat.h>
#include "codegen.h"

int main (__unused int argc, char *argv[]) {
  duc_file_t *file = duc_file_new(argv[1], DUC_FILE_READ);
  ast_t *ast = ast_new(file);
  duc_binary_t *bin = codegen(ast);

  if (bin == NULL) {
    duc_throw("Something went wrong");
  }

  duc_binary_write(bin, "a.out");
  chmod("a.out", 0755);

  duc_binary_free(bin);
  ast_free(ast);
  duc_file_free(file);

  return 0;
}
