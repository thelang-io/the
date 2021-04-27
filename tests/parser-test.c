/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../src/parser.h"

// TODO Decrease size by packing into macros

#define PARSER_F(text, token, body) \
  do { duc_writefile("../test.txt", text); \
  duc_file_t *file = duc_file_new("../test.txt", DUC_FILE_READ); \
  parser_##token##_t *parser = parser_##token##_new_(file); \
  body \
  if (parser != NULL) parser_##token##_free_(parser); \
  duc_file_free(file); \
  duc_file_remove("../test.txt"); } while (0)

#define PARSER_WS_F(text, alloc, body) \
  do { duc_writefile("../test.txt", text); \
  duc_file_t *file = duc_file_new("../test.txt", DUC_FILE_READ); \
  parser_ws_t *parser = parser_ws_new_(file, alloc); \
  body \
  if (parser != NULL) parser_ws_free_(parser); \
  duc_file_free(file); \
  duc_file_remove("../test.txt"); } while (0)

DUC_TEST(parser, arglist_new_free) {
  PARSER_F("@", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F(",", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test@", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test,", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test, \"test\"", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test, \"test\"(", arglist, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F(")", arglist, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_TRUE(duc_array_empty(parser->exprs));
  });

  PARSER_F("test)", arglist, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_EQ(duc_array_length(parser->exprs), 1);
    DUC_ASSERT_NE(duc_array_at(parser->exprs, 0), NULL);
  });

  PARSER_F("test, \"test\")", arglist, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_EQ(duc_array_length(parser->exprs), 2);
    DUC_ASSERT_NE(duc_array_at(parser->exprs, 0), NULL);
    DUC_ASSERT_NE(duc_array_at(parser->exprs, 1), NULL);
  });
}

DUC_TEST(parser, call_expr_new_free) {
  PARSER_F(" ", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test1", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test1@", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test1)", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test2(", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test2(test", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test2(test, \"test\"", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test2(test, \"test\"@", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test2(test, \"test\"(", call_expr, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test1()", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test1( )", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test)", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2( test )", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test,\"test\")", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2(test, \"test\")", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });

  PARSER_F("test2( test, \"test\" )", call_expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_NE(parser->arglist, NULL);
  });
}

DUC_TEST(parser, expr_new_free) {
  PARSER_F("test", expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->id, NULL);
    DUC_ASSERT_EQ(parser->token, PARSER_ID);
  });

  PARSER_F("\"test\"", expr, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->literal, NULL);
    DUC_ASSERT_EQ(parser->token, PARSER_LITERAL);
  });
}

DUC_TEST(parser, id_new_free) {
  PARSER_F("@", id, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("test", id, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->lexer, NULL);
  });
}

DUC_TEST(parser, literal_new_free) {
  PARSER_F("@", literal, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_F("\"test\"", literal, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_NE(parser->lexer, NULL);
  });
}

DUC_TEST(parser, new_and_free) {
  char *filepath = "../test.txt";

  duc_writefile(filepath, " ");
  duc_file_t *file = duc_file_new(filepath, DUC_FILE_READ);
  parser_t *parser = parser_new(file);

  DUC_ASSERT_NE(parser, NULL);
  parser_free(parser);
  duc_file_free(file);

  duc_writefile(filepath, "@");
  file = duc_file_new(filepath, DUC_FILE_READ);
  parser = parser_new(file);

  DUC_ASSERT_NE(parser, NULL);
  DUC_ASSERT_EQ(parser->call_expr, NULL);
  DUC_ASSERT_EQ(parser->token, PARSER_UNKNOWN);
  DUC_ASSERT_EQ(parser->ws, NULL);
  DUC_ASSERT_EQ(duc_file_position(file), 0);

  parser_free(parser);
  duc_file_free(file);

  duc_file_remove(filepath);
}

DUC_TEST(parser, ws_new_free) {
  PARSER_WS_F("@", true, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_WS_F("\t \r\n", true, {
    DUC_ASSERT_NE(parser, NULL);
    DUC_ASSERT_EQ(duc_array_length(parser->lexers), 1);
  });

  PARSER_WS_F("@", false, {
    DUC_ASSERT_EQ(parser, NULL);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  PARSER_WS_F("\t \r\n", false, {
    DUC_ASSERT_EQ(parser, NULL);
  });
}

int main () {
  DUC_TEST_RUN(parser, arglist_new_free);
  DUC_TEST_RUN(parser, call_expr_new_free);
  DUC_TEST_RUN(parser, expr_new_free);
  DUC_TEST_RUN(parser, id_new_free);
  DUC_TEST_RUN(parser, literal_new_free);
  DUC_TEST_RUN(parser, new_and_free);
  DUC_TEST_RUN(parser, ws_new_free);
}
