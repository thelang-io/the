/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/lit-int-hex.h"
#include "../lexer-test.h"

#define LEX_LIT_INT_HEX_F(text) \
  LEX_FS(text, LEXER_LIT_INT_HEX); \
  LEX_F(text "..", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->tok, LEXER_LIT_INT_HEX); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text ".. ", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->tok, LEXER_LIT_INT_HEX); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

TEST(lex_lit_int_hex, works) {
  LEX_LIT_INT_HEX_F("0X0");
  LEX_LIT_INT_HEX_F("0x0");
  LEX_LIT_INT_HEX_F("0X1");
  LEX_LIT_INT_HEX_F("0x1");
  LEX_LIT_INT_HEX_F("0X9");
  LEX_LIT_INT_HEX_F("0x9");
  LEX_LIT_INT_HEX_F("0XF");
  LEX_LIT_INT_HEX_F("0xF");
  LEX_LIT_INT_HEX_F("0XFFFFFFFF");
  LEX_LIT_INT_HEX_F("0xFFFFFFFF");
}

TEST(lexer_lit_int_hex, is_special) {
  ASSERT_FALSE(lexer_lit_int_hex_is_special('$'));
  ASSERT_TRUE(lexer_lit_int_hex_is_special('X'));
  ASSERT_TRUE(lexer_lit_int_hex_is_special('x'));
}

int main () {
  TEST_RUN(lex_lit_int_hex, works);
  TEST_RUN(lexer_lit_int_hex, is_special);
}
