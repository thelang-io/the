/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/comment-line.h"
#include "../lexer-test.h"

DUC_TEST(lex_comment_line, works) {
  LEX_F("//", {
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMENT_LINE);
    DUC_ASSERT_MEMEQ(lexer->raw, "//", strlen("//") + 1);
    DUC_ASSERT_MEMEQ(lexer->str, "", 1);
  });

  LEX_F("//\n", {
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMENT_LINE);
    DUC_ASSERT_MEMEQ(lexer->raw, "//", strlen("//") + 1);
    DUC_ASSERT_MEMEQ(lexer->str, "", 1);
  });

  LEX_F("// ", {
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMENT_LINE);
    DUC_ASSERT_MEMEQ(lexer->raw, "// ", strlen("// ") + 1);
    DUC_ASSERT_MEMEQ(lexer->str, " ", strlen(" ") + 1);
  });

  LEX_F("// \n", {
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMENT_LINE);
    DUC_ASSERT_MEMEQ(lexer->raw, "// ", strlen("// ") + 1);
    DUC_ASSERT_MEMEQ(lexer->str, " ", strlen(" ") + 1);
  });

  LEX_F("//Test", {
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMENT_LINE);
    DUC_ASSERT_MEMEQ(lexer->raw, "//Test", strlen("//Test") + 1);
    DUC_ASSERT_MEMEQ(lexer->str, "Test", strlen("Test") + 1);
  });

  LEX_F("//Test\n", {
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMENT_LINE);
    DUC_ASSERT_MEMEQ(lexer->raw, "//Test", strlen("//Test") + 1);
    DUC_ASSERT_MEMEQ(lexer->str, "Test", strlen("Test") + 1);
  });
}

int main () {
  DUC_TEST_RUN(lex_comment_line, works);
}
