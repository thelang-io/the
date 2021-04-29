/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-int-bin.h"
#include "../lexer-test.h"

DUC_TEST(lex_lit_int_bin, works) {
  LEX_FS("0B0", LEXER_LIT_INT_BIN);
  LEX_FS("0b0", LEXER_LIT_INT_BIN);
  LEX_FS("0B1", LEXER_LIT_INT_BIN);
  LEX_FS("0b1", LEXER_LIT_INT_BIN);
  LEX_FS("0B01111111111111111111111111111111", LEXER_LIT_INT_BIN);
  LEX_FS("0b01111111111111111111111111111111", LEXER_LIT_INT_BIN);
}

DUC_TEST(lexer_lit_int_bin, is_special) {
  DUC_ASSERT_FALSE(lexer_lit_int_bin_is_special('$'));
  DUC_ASSERT_TRUE(lexer_lit_int_bin_is_special('B'));
  DUC_ASSERT_TRUE(lexer_lit_int_bin_is_special('b'));
}

int main () {
  DUC_TEST_RUN(lex_lit_int_bin, works);
  DUC_TEST_RUN(lexer_lit_int_bin, is_special);
}
