/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/id.h"
#include "../parser-test.h"

TEST(parser_id, new_and_free) {
  PARSER_F("_", id, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_ID);
    ASSERT_NE(parser->lexer, NULL);
    ASSERT_EQ(parser->lexer->token, LEXER_LIT_ID);
  });

  PARSER_F("test", id, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->tok, PARSER_ID);
    ASSERT_NE(parser->lexer, NULL);
    ASSERT_EQ(parser->lexer->token, LEXER_LIT_ID);
  });
}

int main () {
  TEST_RUN(parser_id, new_and_free);
}
