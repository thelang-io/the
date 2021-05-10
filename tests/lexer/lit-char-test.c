/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/lit-char.h"
#include "../lexer-test.h"

TEST(lex_lit_char, works) {
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

TEST(lexer_lit_char, is_escape) {
  ASSERT_FALSE(lexer_lit_char_is_escape('$'));
  ASSERT_TRUE(lexer_lit_char_is_escape('0'));
  ASSERT_TRUE(lexer_lit_char_is_escape('t'));
  ASSERT_TRUE(lexer_lit_char_is_escape('n'));
  ASSERT_TRUE(lexer_lit_char_is_escape('r'));
  ASSERT_TRUE(lexer_lit_char_is_escape('"'));
  ASSERT_TRUE(lexer_lit_char_is_escape('\''));
  ASSERT_TRUE(lexer_lit_char_is_escape('\\'));
}

int main () {
  TEST_RUN(lex_lit_char, works);
  TEST_RUN(lexer_lit_char, is_escape);
}
