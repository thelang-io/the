/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-int-dec.h"
#include "../lexer-test.h"

DUC_TEST(lex_lit_int_dec, works) {
  LEX_FS("0", LEXER_LIT_INT_DEC);
  LEX_FS("1", LEXER_LIT_INT_DEC);
  LEX_FS("9", LEXER_LIT_INT_DEC);
  LEX_FS("2147483647", LEXER_LIT_INT_DEC);
  LEX_FS("18446744073709551615", LEXER_LIT_INT_DEC);
}

int main () {
  DUC_TEST_RUN(lex_lit_int_dec, works);
}
