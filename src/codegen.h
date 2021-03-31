/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CODEGEN_H
#define SRC_CODEGEN_H

#include <duc/binary.h>
#include "ast.h"

duc_binary_t *codegen (const ast_t *ast);

#endif
