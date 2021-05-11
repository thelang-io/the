/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/literal.h"
#include "../parser-test.h"

TEST(parser_literal, new_and_free) {
  PARSER_F("1.", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->token, LEXER_LIT_FLOAT);
  });

  PARSER_F("2", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->token, LEXER_LIT_INT_DEC);
  });

  PARSER_F("'c'", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->token, LEXER_LIT_CHAR);
  });

  PARSER_F("\"str\"", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->token, LEXER_LIT_STR);
  });
}

int main () {
  TEST_RUN(parser_literal, new_and_free);
}
