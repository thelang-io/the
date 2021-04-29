/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-int-hex.h"
#include "../lexer-test.h"

DUC_TEST(lex_lit_int_hex, works) {
  LEX_FS("0X0", LEXER_LIT_INT_HEX);
  LEX_FS("0x0", LEXER_LIT_INT_HEX);
  LEX_FS("0X1", LEXER_LIT_INT_HEX);
  LEX_FS("0x1", LEXER_LIT_INT_HEX);
  LEX_FS("0X9", LEXER_LIT_INT_HEX);
  LEX_FS("0x9", LEXER_LIT_INT_HEX);
  LEX_FS("0XF", LEXER_LIT_INT_HEX);
  LEX_FS("0xF", LEXER_LIT_INT_HEX);
  LEX_FS("0XFFFFFFFF", LEXER_LIT_INT_HEX);
  LEX_FS("0xFFFFFFFF", LEXER_LIT_INT_HEX);
}

DUC_TEST(lexer_lit_int_hex, is_special) {
  DUC_ASSERT_FALSE(lexer_lit_int_hex_is_special('$'));
  DUC_ASSERT_TRUE(lexer_lit_int_hex_is_special('X'));
  DUC_ASSERT_TRUE(lexer_lit_int_hex_is_special('x'));
}

int main () {
  DUC_TEST_RUN(lex_lit_int_hex, works);
  DUC_TEST_RUN(lexer_lit_int_hex, is_special);
}
