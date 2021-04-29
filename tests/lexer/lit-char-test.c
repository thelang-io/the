/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-char.h"
#include "../lexer-test.h"

DUC_TEST(lex_lit_char, works) {
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

DUC_TEST(lexer_lit_char, is_escape) {
  DUC_ASSERT_FALSE(lexer_lit_char_is_escape('$'));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('0'));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('t'));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('n'));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('r'));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('"'));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('\''));
  DUC_ASSERT_TRUE(lexer_lit_char_is_escape('\\'));
}

int main () {
  DUC_TEST_RUN(lex_lit_char, works);
  DUC_TEST_RUN(lexer_lit_char, is_escape);
}
