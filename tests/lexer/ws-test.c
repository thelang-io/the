/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/ws.h"
#include "../lexer-test.h"

#define LEX_WS_F(text) \
  LEX_F(text, { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_WS); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text "t", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, LEXER_WS); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

TEST(lex_ws, works) {
  LEX_WS_F(" ");
  LEX_WS_F("\n");
  LEX_WS_F("\r");
  LEX_WS_F("\t");
  LEX_WS_F("\t ");
  LEX_WS_F("\t\t");
  LEX_WS_F("\n\n\n");
}

int main () {
  TEST_RUN(lex_ws, works);
}
