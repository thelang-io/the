/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../../src/parser/ws.h"
#include "../parser-test.h"

#define PARSER_WS_F(text, alloc, body) \
  do { writefile("../test.out", text); \
  file_t *file = file_new("../test.out", FILE_READ); \
  parser_ws_t *parser = parser_ws_new(file, alloc); \
  body \
  if (parser != NULL) parser_ws_free(parser); \
  file_free(file); \
  file_remove("../test.out"); } while (0)

TEST(parser_ws, new_and_free) {
  PARSER_WS_F(" ", true, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_WS);
    ASSERT_EQ(array_length(parser->lexers), 1);
  });

  PARSER_WS_F(" \n\r\t/**/", true, {
    ASSERT_NE(parser, NULL);
    ASSERT_EQ(parser->tok, PARSER_WS);
    ASSERT_EQ(array_length(parser->lexers), 2);
  });

  PARSER_WS_F(" ", false, {
    ASSERT_EQ(parser, NULL);
  });

  PARSER_WS_F(" \n\r\t/**/", false, {
    ASSERT_EQ(parser, NULL);
  });
}

int main () {
  TEST_RUN(parser_ws, new_and_free);
}
