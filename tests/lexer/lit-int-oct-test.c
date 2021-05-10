/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/lit-int-oct.h"
#include "../lexer-test.h"

#define LEX_LIT_INT_OCT_F(text) \
  LEX_FS(text, LEXER_LIT_INT_OCT); \
  LEX_F(text "..", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_LIT_INT_OCT); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text ".. ", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_LIT_INT_OCT); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

TEST(lex_lit_int_oct, works) {
  LEX_LIT_INT_OCT_F("00");
  LEX_LIT_INT_OCT_F("0O0");
  LEX_LIT_INT_OCT_F("0o0");

  LEX_LIT_INT_OCT_F("01");
  LEX_LIT_INT_OCT_F("0O1");
  LEX_LIT_INT_OCT_F("0o1");

  LEX_LIT_INT_OCT_F("07");
  LEX_LIT_INT_OCT_F("0O7");
  LEX_LIT_INT_OCT_F("0o7");

  LEX_LIT_INT_OCT_F("017777777777");
  LEX_LIT_INT_OCT_F("0O17777777777");
  LEX_LIT_INT_OCT_F("0o17777777777");
}

TEST(lexer_lit_int_oct, is_special) {
  ASSERT_FALSE(lexer_lit_int_oct_is_special('$'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('0'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('1'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('2'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('3'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('4'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('5'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('6'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('7'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('O'));
  ASSERT_TRUE(lexer_lit_int_oct_is_special('o'));
}

int main () {
  TEST_RUN(lex_lit_int_oct, works);
  TEST_RUN(lexer_lit_int_oct, is_special);
}
