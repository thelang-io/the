/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/lit-int-dec.h"
#include "../lexer-test.h"

#define LEX_LIT_INT_DEC_F(text) \
  LEX_FS(text, LEXER_LIT_INT_DEC); \
  LEX_F(text "..", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_LIT_INT_DEC); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text ".. ", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_LIT_INT_DEC); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

TEST(lex_lit_int_dec, works) {
  LEX_LIT_INT_DEC_F("0");
  LEX_LIT_INT_DEC_F("1");
  LEX_LIT_INT_DEC_F("9");
  LEX_LIT_INT_DEC_F("2147483647");
  LEX_LIT_INT_DEC_F("18446744073709551615");
}

int main () {
  TEST_RUN(lex_lit_int_dec, works);
}
