/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "codegen.h"
#include "codegen-macos.h"

duc_binary_t *codegen (const ast_t *ast) {
  return codegen_macos(ast);
}
