/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-str.h"
#include "utilities.h"

#define LEX_LIT_STR_F(raw_text, str_text) \
  LEX_F(raw_text, { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LIT_STR); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  }); \
  LEX_F(raw_text " ", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LIT_STR); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  })

DUC_TEST(lexer_lit_str, works) {
  LEX_LIT_STR_F("\"\"", "");
  LEX_LIT_STR_F("\"a\"", "a");
  LEX_LIT_STR_F("\"Test\"", "Test");
}

int main () {
  DUC_TEST_RUN(lexer_lit_str, works);
}
