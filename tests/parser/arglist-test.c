/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/arglist.h"
#include "../parser-test.h"

TEST(parser_arglist, new_and_free) {
  PARSER_FI(")", arglist, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_ARGLIST);
    ASSERT_TRUE(array_empty(parser->exprs));
  });

  PARSER_FI("test)", arglist, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_ARGLIST);
    ASSERT_EQ(array_length(parser->exprs), 1);
    ASSERT_NE(array_at(parser->exprs, 0), NULL);
  });

  PARSER_FI("test, \"test\")", arglist, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_ARGLIST);
    ASSERT_EQ(array_length(parser->exprs), 2);
    ASSERT_NE(array_at(parser->exprs, 0), NULL);
    ASSERT_NE(array_at(parser->exprs, 1), NULL);
  });
}

int main () {
  TEST_RUN(parser_arglist, new_and_free);
}
