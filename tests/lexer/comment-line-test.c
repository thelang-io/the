/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/lexer/comment-line.h"
#include "../lexer-test.h"

TEST(lex_comment_line, works) {
  LEX_F("//", {
    ASSERT_NE(lexer, NULL);
    ASSERT_EQ(lexer->tok, LEXER_COMMENT_LINE);
    ASSERT_MEMEQ(lexer->raw, "//", strlen("//") + 1);
    ASSERT_MEMEQ(lexer->str, "", 1);
  });

  LEX_F("//\n", {
    ASSERT_NE(lexer, NULL);
    ASSERT_EQ(lexer->tok, LEXER_COMMENT_LINE);
    ASSERT_MEMEQ(lexer->raw, "//", strlen("//") + 1);
    ASSERT_MEMEQ(lexer->str, "", 1);
  });

  LEX_F("// ", {
    ASSERT_NE(lexer, NULL);
    ASSERT_EQ(lexer->tok, LEXER_COMMENT_LINE);
    ASSERT_MEMEQ(lexer->raw, "// ", strlen("// ") + 1);
    ASSERT_MEMEQ(lexer->str, " ", strlen(" ") + 1);
  });

  LEX_F("// \n", {
    ASSERT_NE(lexer, NULL);
    ASSERT_EQ(lexer->tok, LEXER_COMMENT_LINE);
    ASSERT_MEMEQ(lexer->raw, "// ", strlen("// ") + 1);
    ASSERT_MEMEQ(lexer->str, " ", strlen(" ") + 1);
  });

  LEX_F("//Test", {
    ASSERT_NE(lexer, NULL);
    ASSERT_EQ(lexer->tok, LEXER_COMMENT_LINE);
    ASSERT_MEMEQ(lexer->raw, "//Test", strlen("//Test") + 1);
    ASSERT_MEMEQ(lexer->str, "Test", strlen("Test") + 1);
  });

  LEX_F("//Test\n", {
    ASSERT_NE(lexer, NULL);
    ASSERT_EQ(lexer->tok, LEXER_COMMENT_LINE);
    ASSERT_MEMEQ(lexer->raw, "//Test", strlen("//Test") + 1);
    ASSERT_MEMEQ(lexer->str, "Test", strlen("Test") + 1);
  });
}

int main () {
  TEST_RUN(lex_comment_line, works);
}
