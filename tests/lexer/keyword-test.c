/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/keyword.h"
#include "../utilities.h"

#define LEXER_KEYWORD_F(text, tok) \
  LEXER_FSS(text, keyword, tok); \
  LEXER_F(text "s", { \
    DUC_ASSERT_FALSE(lexer_keyword(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->raw, NULL); \
    DUC_ASSERT_EQ(lexer->str, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN); \
    DUC_ASSERT_EQ(duc_file_position(file), 0); \
  })

DUC_TEST(lexer_keyword, works) {
  LEXER_KEYWORD_F("fn", LEXER_FN);
  LEXER_KEYWORD_F("in", LEXER_IN);
  LEXER_KEYWORD_F("loop", LEXER_LOOP);
  LEXER_KEYWORD_F("main", LEXER_MAIN);
  LEXER_KEYWORD_F("mut", LEXER_MUT);
  LEXER_KEYWORD_F("return", LEXER_RETURN);
}

int main () {
  DUC_TEST_RUN(lexer_keyword, works);
}
