/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "../src/parser.h"
#include "testing.h"

TEST(parser, call_expr_new_free) {
  PARSER_F(" ", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test1", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test1@", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test1)", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test2(", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test2(test", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test2(test, \"test\"", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test2(test, \"test\"@", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test2(test, \"test\"(", call_expr, {
    ASSERT_EQ(parser, NULL);
    ASSERT_EQ(file_position(file), 0);
  });

  PARSER_F("test1()", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test1( )", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test)", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2( test )", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test,\"test\")", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test, \"test\")", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2( test, \"test\" )", call_expr, {
    ASSERT_NE(parser, NULL);
    ASSERT_NE(parser->id, NULL);
    ASSERT_NE(parser->arglist, NULL);
  });
}

TEST(parser, new_and_free) {
  char *filepath = "../test.out";

  writefile(filepath, " ");
  file_t *file = file_new(filepath, FILE_READ);
  parser_t *parser = parser_new(file);

  ASSERT_NE(parser, NULL);
  parser_free(parser);
  file_free(file);

  writefile(filepath, "@");
  file = file_new(filepath, FILE_READ);
  parser = parser_new(file);

  ASSERT_NE(parser, NULL);
  ASSERT_EQ(parser->call_expr, NULL);
  ASSERT_EQ(parser->token, PARSER_UNKNOWN);
  ASSERT_EQ(parser->ws, NULL);
  ASSERT_EQ(file_position(file), 0);

  parser_free(parser);
  file_free(file);

  file_remove(filepath);
}

int main () {
  TEST_RUN(parser, call_expr_new_free);
  TEST_RUN(parser, new_and_free);
}
