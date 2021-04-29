/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-int-oct.h"
#include "../lexer-test.h"

DUC_TEST(lex_lit_int_oct, works) {
  LEX_FS("00", LEXER_LIT_INT_OCT);
  LEX_FS("0O0", LEXER_LIT_INT_OCT);
  LEX_FS("0o0", LEXER_LIT_INT_OCT);

  LEX_FS("01", LEXER_LIT_INT_OCT);
  LEX_FS("0O1", LEXER_LIT_INT_OCT);
  LEX_FS("0o1", LEXER_LIT_INT_OCT);

  LEX_FS("07", LEXER_LIT_INT_OCT);
  LEX_FS("0O7", LEXER_LIT_INT_OCT);
  LEX_FS("0o7", LEXER_LIT_INT_OCT);

  LEX_FS("017777777777", LEXER_LIT_INT_OCT);
  LEX_FS("0O17777777777", LEXER_LIT_INT_OCT);
  LEX_FS("0o17777777777", LEXER_LIT_INT_OCT);
}

DUC_TEST(lexer_lit_int_oct, is_special) {
  DUC_ASSERT_FALSE(lexer_lit_int_oct_is_special('$'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('0'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('1'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('2'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('3'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('4'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('5'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('6'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('7'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('O'));
  DUC_ASSERT_TRUE(lexer_lit_int_oct_is_special('o'));
}

int main () {
  DUC_TEST_RUN(lex_lit_int_oct, works);
  DUC_TEST_RUN(lexer_lit_int_oct, is_special);
}
