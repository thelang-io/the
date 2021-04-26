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

#define LEXER_F_SHORT(text, method, tok) \
  LEXER_F(text, { \
    DUC_ASSERT_TRUE(lexer_is_##method##_(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

#define LEXER_F_SHORT_SPACE(text, method, tok) \
  LEXER_F_SHORT(text, method, tok); \
  LEXER_F(text " ", { \
    DUC_ASSERT_TRUE(lexer_is_##method##_(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

#define LEXER_F_KEYWORD(text, tok) \
  LEXER_F_SHORT_SPACE(text, keyword, tok); \
  LEXER_F(text "s", { \
    DUC_ASSERT_FALSE(lexer_is_keyword_(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->raw, NULL); \
    DUC_ASSERT_EQ(lexer->str, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_UNKNOWN); \
    DUC_ASSERT_EQ(duc_file_position(file), 0); \
  })

#define LEXER_F_LITINT(letter1, letter2, text, tok) \
  LEXER_F_SHORT_SPACE("0" letter1 text, litint, tok); \
  LEXER_F_SHORT_SPACE("0" letter2 text, litint, tok)

#define LEXER_F_LITSTR(raw_text, str_text) \
  LEXER_F(raw_text, { \
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  }); \
  LEXER_F(raw_text " ", { \
    DUC_ASSERT_TRUE(lexer_is_litstr_(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  })

#define LEXER_F_WS(text) \
  LEXER_F_SHORT(text, ws, LEXER_WS); \
  LEXER_F(text "t", { \
    DUC_ASSERT_TRUE(lexer_is_ws_(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_WS); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

DUC_TEST(lexer, is_bracket) {
  LEXER_F_SHORT("{", bracket, LEXER_LBRACE);
  LEXER_F_SHORT("[", bracket, LEXER_LBRACK);
  LEXER_F_SHORT("(", bracket, LEXER_LPAR);
  LEXER_F_SHORT("}", bracket, LEXER_RBRACE);
  LEXER_F_SHORT("]", bracket, LEXER_RBRACK);
  LEXER_F_SHORT(")", bracket, LEXER_RPAR);
}

DUC_TEST(lexer, is_id) {
  LEXER_F_SHORT_SPACE("a", id, LEXER_ID);
  LEXER_F_SHORT_SPACE("A_b1", id, LEXER_ID);
  LEXER_F_SHORT_SPACE("_Ab1", id, LEXER_ID);
}

DUC_TEST(lexer, is_keyword) {
  LEXER_F_KEYWORD("fn", LEXER_FN);
  LEXER_F_KEYWORD("in", LEXER_IN);
  LEXER_F_KEYWORD("loop", LEXER_LOOP);
  LEXER_F_KEYWORD("main", LEXER_MAIN);
  LEXER_F_KEYWORD("mut", LEXER_MUT);
  LEXER_F_KEYWORD("return", LEXER_RETURN);
}

DUC_TEST(lexer, is_litint) {
  LEXER_F_SHORT_SPACE("0", litint, LEXER_LITINT_DEC);
  LEXER_F_SHORT_SPACE("1", litint, LEXER_LITINT_DEC);
  LEXER_F_SHORT_SPACE("9", litint, LEXER_LITINT_DEC);
  LEXER_F_SHORT_SPACE("2147483647", litint, LEXER_LITINT_DEC);
  LEXER_F_SHORT_SPACE("01", litint, LEXER_LITINT_OCT);
  LEXER_F_SHORT_SPACE("07", litint, LEXER_LITINT_OCT);
  LEXER_F_SHORT_SPACE("017777777777", litint, LEXER_LITINT_OCT);
  LEXER_F_LITINT("O", "o", "0", LEXER_LITINT_OCT);
  LEXER_F_LITINT("O", "o", "1", LEXER_LITINT_OCT);
  LEXER_F_LITINT("O", "o", "7", LEXER_LITINT_OCT);
  LEXER_F_LITINT("O", "o", "17777777777", LEXER_LITINT_OCT);
  LEXER_F_LITINT("B", "b", "0", LEXER_LITINT_BIN);
  LEXER_F_LITINT("B", "b", "1", LEXER_LITINT_BIN);
  LEXER_F_LITINT("B", "b", "01111111111111111111111111111111", LEXER_LITINT_BIN);
  LEXER_F_LITINT("X", "x", "0", LEXER_LITINT_HEX);
  LEXER_F_LITINT("X", "x", "1", LEXER_LITINT_HEX);
  LEXER_F_LITINT("X", "x", "9", LEXER_LITINT_HEX);
  LEXER_F_LITINT("X", "x", "F", LEXER_LITINT_HEX);
  LEXER_F_LITINT("X", "x", "7FFFFFFF", LEXER_LITINT_HEX);
}

DUC_TEST(lexer, is_litstr) {
  LEXER_F_LITSTR("\"\"", "");
  LEXER_F_LITSTR("\"a\"", "a");
  LEXER_F_LITSTR("\"Test\"", "Test");
}

DUC_TEST(lexer, is_mark) {
  LEXER_F_SHORT(":", mark, LEXER_COLON);
  LEXER_F_SHORT(",", mark, LEXER_COMMA);
}

DUC_TEST(lexer, is_ws) {
  LEXER_F_WS(" ");
  LEXER_F_WS("\t\t");
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
  DUC_TEST_RUN(lexer, is_keyword);
  DUC_TEST_RUN(lexer, is_litint);
  DUC_TEST_RUN(lexer, is_litstr);
  DUC_TEST_RUN(lexer, is_mark);
  DUC_TEST_RUN(lexer, is_ws);
  DUC_TEST_RUN(lexer, new_and_free);
}
