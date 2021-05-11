/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/call-expr.h"
#include "../parser-test.h"

TEST(parser_call_expr, new_and_free) {
  PARSER_F("test1()", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test1( )", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test)", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2( test )", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test,\"test\")", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test, \"test\")", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2( test, \"test\" )", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_CALL_EXPR);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });
}

int main () {
  TEST_RUN(parser_call_expr, new_and_free);
}
