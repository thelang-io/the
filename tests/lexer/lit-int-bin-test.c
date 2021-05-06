/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-int-bin.h"
#include "../lexer-test.h"

#define LEX_LIT_INT_BIN_F(text) \
  LEX_FS(text, LEXER_LIT_INT_BIN); \
  LEX_F(text "..", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LIT_INT_BIN); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text ".. ", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LIT_INT_BIN); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

DUC_TEST(lex_lit_int_bin, works) {
  LEX_LIT_INT_BIN_F("0B0");
  LEX_LIT_INT_BIN_F("0b0");
  LEX_LIT_INT_BIN_F("0B1");
  LEX_LIT_INT_BIN_F("0b1");
  LEX_LIT_INT_BIN_F("0B01111111111111111111111111111111");
  LEX_LIT_INT_BIN_F("0b01111111111111111111111111111111");
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
