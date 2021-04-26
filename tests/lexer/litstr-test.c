/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/litstr.h"
#include "../utilities.h"

#define LEXER_LITSTR_F(raw_text, str_text) \
  LEXER_F(raw_text, { \
    DUC_ASSERT_TRUE(lexer_litstr(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  }); \
  LEXER_F(raw_text " ", { \
    DUC_ASSERT_TRUE(lexer_litstr(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  })

DUC_TEST(lexer_litstr, works) {
  LEXER_LITSTR_F("\"\"", "");
  LEXER_LITSTR_F("\"a\"", "a");
  LEXER_LITSTR_F("\"Test\"", "Test");
}

int main () {
  DUC_TEST_RUN(lexer_litstr, works);
}
