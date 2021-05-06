/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-int-hex.h"
#include "../lexer-test.h"

#define LEX_LIT_INT_HEX_F(text) \
  LEX_FS(text, LEXER_LIT_INT_HEX); \
  LEX_F(text "..", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LIT_INT_HEX); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text ".. ", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LIT_INT_HEX); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

DUC_TEST(lex_lit_int_hex, works) {
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

DUC_TEST(lexer_lit_int_hex, is_special) {
  DUC_ASSERT_FALSE(lexer_lit_int_hex_is_special('$'));
  DUC_ASSERT_TRUE(lexer_lit_int_hex_is_special('X'));
  DUC_ASSERT_TRUE(lexer_lit_int_hex_is_special('x'));
}

int main () {
  DUC_TEST_RUN(lex_lit_int_hex, works);
  DUC_TEST_RUN(lexer_lit_int_hex, is_special);
}
