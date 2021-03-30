/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/helpers.h>
#include "ast.h"

int main (DUC_UNUSED int argc, char *argv[]) {
  duc_file_t *file = duc_file_new(argv[1], DUC_FILE_READ);
  ast_t *ast = ast_new(file);

  ast_free(ast);
  duc_file_free(file);

  return 0;
}
