/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-str.h"
#include "../lexer-test.h"

DUC_TEST(lexer_lit_str, works) {
  LEX_FE("\"\"", "", LEXER_LIT_STR);
  LEX_FE("\"a\"", "a", LEXER_LIT_STR);
  LEX_FE("\"Test\"", "Test", LEXER_LIT_STR);
}

int main () {
  DUC_TEST_RUN(lexer_lit_str, works);
}
