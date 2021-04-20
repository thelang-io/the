/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include <stdlib.h>
#include "../src/lexer.h"

#define LEXER_F(text, body) \
  do { duc_writefile("../test.txt", text); \
  duc_file_t *file = duc_file_new("../test.txt", DUC_FILE_READ); \
  lexer_t *lexer = malloc(sizeof(lexer_t)); \
  lexer->raw = NULL; \
  lexer->str = NULL; \
  lexer->token = LEXER_UNKNOWN; \
  body \
  lexer_free(lexer); \
  duc_file_free(file); \
  duc_file_remove("../test.txt"); } while (0)

DUC_TEST(lexer, is_bracket) {
  LEXER_F("(", {
    DUC_ASSERT_TRUE(lexer_is_bracket_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LPAR);
    DUC_ASSERT_MEMEQ(lexer->raw, "(", 2);
    DUC_ASSERT_MEMEQ(lexer->str, "(", 2);
  });

  LEXER_F(")", {
    DUC_ASSERT_TRUE(lexer_is_bracket_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_RPAR);
    DUC_ASSERT_MEMEQ(lexer->raw, ")", 2);
    DUC_ASSERT_MEMEQ(lexer->str, ")", 2);
  });
}

DUC_TEST(lexer, is_id) {
  LEXER_F("a", {
    DUC_ASSERT_TRUE(lexer_is_id_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_ID);
    DUC_ASSERT_MEMEQ(lexer->raw, "a", 2);
    DUC_ASSERT_MEMEQ(lexer->str, "a", 2);
  });

  LEXER_F("A_b1", {
    DUC_ASSERT_TRUE(lexer_is_id_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_ID);
    DUC_ASSERT_MEMEQ(lexer->raw, "A_b1", 5);
    DUC_ASSERT_MEMEQ(lexer->str, "A_b1", 5);
  });

  LEXER_F("a(", {
    DUC_ASSERT_TRUE(lexer_is_id_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_ID);
    DUC_ASSERT_MEMEQ(lexer->raw, "a", 2);
    DUC_ASSERT_MEMEQ(lexer->str, "a", 2);
  });

  LEXER_F("A_b1(", {
    DUC_ASSERT_TRUE(lexer_is_id_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_ID);
    DUC_ASSERT_MEMEQ(lexer->raw, "A_b1", 5);
    DUC_ASSERT_MEMEQ(lexer->str, "A_b1", 5);
  });
}

DUC_TEST(lexer, is_litstr) {
  LEXER_F("''", {
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_SQ);
    DUC_ASSERT_MEMEQ(lexer->raw, "''", 3);
    DUC_ASSERT_MEMEQ(lexer->str, "", 1);
  });

  LEXER_F("\"\"", {
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_DQ);
    DUC_ASSERT_MEMEQ(lexer->raw, "\"\"", 3);
    DUC_ASSERT_MEMEQ(lexer->str, "", 1);
  });

  LEXER_F("'", {
    DUC_ASSERT_FALSE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->raw, NULL);
    DUC_ASSERT_EQ(lexer->str, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  LEXER_F("\"", {
    DUC_ASSERT_FALSE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_EQ(lexer->raw, NULL);
    DUC_ASSERT_EQ(lexer->str, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  LEXER_F("'a'", {
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_SQ);
    DUC_ASSERT_MEMEQ(lexer->raw, "'a'", 4);
    DUC_ASSERT_MEMEQ(lexer->str, "a", 2);
  });

  LEXER_F("\"a\"", {
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_DQ);
    DUC_ASSERT_MEMEQ(lexer->raw, "\"a\"", 4);
    DUC_ASSERT_MEMEQ(lexer->str, "a", 2);
  });

  LEXER_F("'a", {
    DUC_ASSERT_FALSE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->raw, NULL);
    DUC_ASSERT_EQ(lexer->str, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  LEXER_F("\"a", {
    DUC_ASSERT_FALSE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->raw, NULL);
    DUC_ASSERT_EQ(lexer->str, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  LEXER_F("'Test'", {
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_SQ);
    DUC_ASSERT_MEMEQ(lexer->raw, "'Test'", 7);
    DUC_ASSERT_MEMEQ(lexer->str, "Test", 5);
  });

  LEXER_F("\"Test\"", {
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_DQ);
    DUC_ASSERT_MEMEQ(lexer->raw, "\"Test\"", 7);
    DUC_ASSERT_MEMEQ(lexer->str, "Test", 5);
  });

  LEXER_F("'Test", {
    DUC_ASSERT_FALSE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->raw, NULL);
    DUC_ASSERT_EQ(lexer->str, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });

  LEXER_F("\"Test", {
    DUC_ASSERT_FALSE(lexer_is_litstr_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->raw, NULL);
    DUC_ASSERT_EQ(lexer->str, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN);
    DUC_ASSERT_EQ(duc_file_position(file), 0);
  });
}

DUC_TEST(lexer, is_mark) {
  LEXER_F(",", {
    DUC_ASSERT_TRUE(lexer_is_mark_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMA);
    DUC_ASSERT_MEMEQ(lexer->raw, ",", 2);
    DUC_ASSERT_MEMEQ(lexer->str, ",", 2);
  });
}

DUC_TEST(lexer, is_ws) {
  LEXER_F(" ", {
    DUC_ASSERT_TRUE(lexer_is_ws_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_WS);
    DUC_ASSERT_MEMEQ(lexer->raw, " ", 2);
    DUC_ASSERT_MEMEQ(lexer->str, " ", 2);
  });

  LEXER_F(" test", {
    DUC_ASSERT_TRUE(lexer_is_ws_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_WS);
    DUC_ASSERT_MEMEQ(lexer->raw, " ", 2);
    DUC_ASSERT_MEMEQ(lexer->str, " ", 2);
  });

  LEXER_F("  ", {
    DUC_ASSERT_TRUE(lexer_is_ws_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_WS);
    DUC_ASSERT_MEMEQ(lexer->raw, "  ", 3);
    DUC_ASSERT_MEMEQ(lexer->str, "  ", 3);
  });

  LEXER_F("  test", {
    DUC_ASSERT_TRUE(lexer_is_ws_(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_WS);
    DUC_ASSERT_MEMEQ(lexer->raw, "  ", 3);
    DUC_ASSERT_MEMEQ(lexer->str, "  ", 3);
  });
}

DUC_TEST(lexer, new_and_free) {
  char *filepath = "../test.txt";

  duc_writefile(filepath, " ");
  duc_file_t *file = duc_file_new(filepath, DUC_FILE_READ);
  lexer_t *lexer = lexer_new(file);

  DUC_ASSERT_NE(lexer, NULL);
  lexer_free(lexer);
  duc_file_free(file);

  duc_writefile(filepath, "@");
  file = duc_file_new(filepath, DUC_FILE_READ);
  lexer = lexer_new(file);

  DUC_ASSERT_EQ(lexer, NULL);
  DUC_ASSERT_EQ(duc_file_position(file), 0);
  duc_file_free(file);

  duc_file_remove(filepath);
}

int main () {
  DUC_TEST_RUN(lexer, is_bracket);
  DUC_TEST_RUN(lexer, is_id);
  DUC_TEST_RUN(lexer, is_litstr);
  DUC_TEST_RUN(lexer, is_mark);
  DUC_TEST_RUN(lexer, is_ws);
  DUC_TEST_RUN(lexer, new_and_free);
}
