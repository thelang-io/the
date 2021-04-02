/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "codegen.h"
#include "codegen-macos.h"

duc_binary_t *codegen (const ast_t *ast) {
  cgm_t *cg = cgm_new(ast);
  duc_binary_t *bin = cgm_binary(cg);

  cgm_free(cg);
  return bin;
}
