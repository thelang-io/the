/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/expr.h"
#include "../parser-test.h"

TEST(parser_expr, new_and_free) {
  PARSER_FI("id", expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_EXPR);
    ASSERT_EQ(parser->parser->tok, PARSER_ID);
  });

  PARSER_FI("\"str\"", expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_EXPR);
    ASSERT_EQ(parser->parser->tok, PARSER_LITERAL);
  });
}

int main () {
  TEST_RUN(parser_expr, new_and_free);
}
