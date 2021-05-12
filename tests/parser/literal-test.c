/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/literal.h"
#include "../parser-test.h"

TEST(parser_literal, new_and_free) {
  PARSER_FI("1.", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_FLOAT);
  });

  PARSER_FI("0b10", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_INT_BIN);
  });

  PARSER_FI("3", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_INT_DEC);
  });

  PARSER_FI("0x04", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_INT_HEX);
  });

  PARSER_FI("0o05", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_INT_OCT);
  });

  PARSER_FI("'c'", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_CHAR);
  });

  PARSER_FI("\"str\"", literal, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_LITERAL);
    ASSERT_EQ(parser->lexer->tok, LEXER_LIT_STR);
  });
}

int main () {
  TEST_RUN(parser_literal, new_and_free);
}
