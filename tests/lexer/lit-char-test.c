/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-char.h"
#include "../lexer-test.h"

DUC_TEST(lexer_lit_char, works) {
  LEX_FE("' '", " ", LEXER_LIT_CHAR);
  LEX_FE("'!'", "!", LEXER_LIT_CHAR);
  LEX_FE("'0'", "0", LEXER_LIT_CHAR);
  LEX_FE("'A'", "A", LEXER_LIT_CHAR);
  LEX_FE("'a'", "a", LEXER_LIT_CHAR);
  LEX_FE("'~'", "~", LEXER_LIT_CHAR);
  LEX_FE("'\\0'", "\\0", LEXER_LIT_CHAR);
  LEX_FE("'\\t'", "\\t", LEXER_LIT_CHAR);
  LEX_FE("'\\n'", "\\n", LEXER_LIT_CHAR);
  LEX_FE("'\\r'", "\\r", LEXER_LIT_CHAR);
  LEX_FE("'\\\"'", "\\\"", LEXER_LIT_CHAR);
  LEX_FE("'\\''", "\\'", LEXER_LIT_CHAR);
  LEX_FE("'\\\\'", "\\\\", LEXER_LIT_CHAR);
}

int main () {
  DUC_TEST_RUN(lexer_lit_char, works);
}
