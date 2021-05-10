/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/lit-int-bin.h"
#include "../lexer-test.h"

#define LEX_LIT_INT_BIN_F(text) \
  LEX_FS(text, LEXER_LIT_INT_BIN); \
  LEX_F(text "..", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_LIT_INT_BIN); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text ".. ", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_LIT_INT_BIN); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

TEST(lex_lit_int_bin, works) {
  LEX_LIT_INT_BIN_F("0B0");
  LEX_LIT_INT_BIN_F("0b0");
  LEX_LIT_INT_BIN_F("0B1");
  LEX_LIT_INT_BIN_F("0b1");
  LEX_LIT_INT_BIN_F("0B01111111111111111111111111111111");
  LEX_LIT_INT_BIN_F("0b01111111111111111111111111111111");
}

TEST(lexer_lit_int_bin, is_special) {
  ASSERT_FALSE(lexer_lit_int_bin_is_special('$'));
  ASSERT_TRUE(lexer_lit_int_bin_is_special('B'));
  ASSERT_TRUE(lexer_lit_int_bin_is_special('b'));
}

int main () {
  TEST_RUN(lex_lit_int_bin, works);
  TEST_RUN(lexer_lit_int_bin, is_special);
}
