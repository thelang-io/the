#include <duc/testing.h>
#include <stdlib.h>
#include "../src/lexer.h"

#define LEXER_F(text, body) \
  do { duc_writefile("../test.txt", text); \
  duc_file_t *file = duc_file_new("../test.txt", DUC_FILE_READ); \
  lexer_t *lexer = malloc(sizeof(lexer_t)); \
  body \
  lexer_free(lexer); \
  duc_file_free(file); \
  duc_file_remove("../test.txt"); } while (0)

DUC_TEST(lexer, is_bracket) {
  LEXER_F("(", {
    DUC_ASSERT_TRUE(lexer_is_bracket(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LPAR);
    DUC_ASSERT_EQ(lexer->raw[0], '(');
    DUC_ASSERT_EQ(lexer->raw[1], '\0');
    DUC_ASSERT_EQ(lexer->str[0], '(');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });

  LEXER_F(")", {
    DUC_ASSERT_TRUE(lexer_is_bracket(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_RPAR);
    DUC_ASSERT_EQ(lexer->raw[0], ')');
    DUC_ASSERT_EQ(lexer->raw[1], '\0');
    DUC_ASSERT_EQ(lexer->str[0], ')');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });
}

DUC_TEST(lexer, is_id) {
  LEXER_F("a", {
    DUC_ASSERT_TRUE(lexer_is_id(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_ID);
    DUC_ASSERT_EQ(lexer->raw[0], 'a');
    DUC_ASSERT_EQ(lexer->raw[1], '\0');
    DUC_ASSERT_EQ(lexer->str[0], 'a');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });

  LEXER_F("$A_b1", {
    DUC_ASSERT_TRUE(lexer_is_id(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_ID);
    DUC_ASSERT_EQ(lexer->raw[0], '$');
    DUC_ASSERT_EQ(lexer->raw[1], 'A');
    DUC_ASSERT_EQ(lexer->raw[2], '_');
    DUC_ASSERT_EQ(lexer->raw[3], 'b');
    DUC_ASSERT_EQ(lexer->raw[4], '1');
    DUC_ASSERT_EQ(lexer->raw[5], '\0');
    DUC_ASSERT_EQ(lexer->str[0], '$');
    DUC_ASSERT_EQ(lexer->str[1], 'A');
    DUC_ASSERT_EQ(lexer->str[2], '_');
    DUC_ASSERT_EQ(lexer->str[3], 'b');
    DUC_ASSERT_EQ(lexer->str[4], '1');
    DUC_ASSERT_EQ(lexer->str[5], '\0');
  });
}

DUC_TEST(lexer, is_litstr) {
  LEXER_F("''", {
    DUC_ASSERT_TRUE(lexer_is_litstr(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_SQ);
    DUC_ASSERT_EQ(lexer->raw[0], '\'');
    DUC_ASSERT_EQ(lexer->raw[1], '\'');
    DUC_ASSERT_EQ(lexer->raw[2], '\0');
    DUC_ASSERT_EQ(lexer->str[0], '\0');
  });

  LEXER_F("\"\"", {
    DUC_ASSERT_TRUE(lexer_is_litstr(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_DQ);
    DUC_ASSERT_EQ(lexer->raw[0], '"');
    DUC_ASSERT_EQ(lexer->raw[1], '"');
    DUC_ASSERT_EQ(lexer->raw[2], '\0');
    DUC_ASSERT_EQ(lexer->str[0], '\0');
  });

  LEXER_F("'a'", {
    DUC_ASSERT_TRUE(lexer_is_litstr(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_SQ);
    DUC_ASSERT_EQ(lexer->raw[0], '\'');
    DUC_ASSERT_EQ(lexer->raw[1], 'a');
    DUC_ASSERT_EQ(lexer->raw[2], '\'');
    DUC_ASSERT_EQ(lexer->raw[3], '\0');
    DUC_ASSERT_EQ(lexer->str[0], 'a');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });

  LEXER_F("\"a\"", {
    DUC_ASSERT_TRUE(lexer_is_litstr(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_DQ);
    DUC_ASSERT_EQ(lexer->raw[0], '"');
    DUC_ASSERT_EQ(lexer->raw[1], 'a');
    DUC_ASSERT_EQ(lexer->raw[2], '"');
    DUC_ASSERT_EQ(lexer->raw[3], '\0');
    DUC_ASSERT_EQ(lexer->str[0], 'a');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });

  LEXER_F("'Test'", {
    DUC_ASSERT_TRUE(lexer_is_litstr(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_SQ);
    DUC_ASSERT_EQ(lexer->raw[0], '\'');
    DUC_ASSERT_EQ(lexer->raw[1], 'T');
    DUC_ASSERT_EQ(lexer->raw[2], 'e');
    DUC_ASSERT_EQ(lexer->raw[3], 's');
    DUC_ASSERT_EQ(lexer->raw[4], 't');
    DUC_ASSERT_EQ(lexer->raw[5], '\'');
    DUC_ASSERT_EQ(lexer->raw[6], '\0');
    DUC_ASSERT_EQ(lexer->str[0], 'T');
    DUC_ASSERT_EQ(lexer->str[1], 'e');
    DUC_ASSERT_EQ(lexer->str[2], 's');
    DUC_ASSERT_EQ(lexer->str[3], 't');
    DUC_ASSERT_EQ(lexer->str[4], '\0');
  });

  LEXER_F("\"Test\"", {
    DUC_ASSERT_TRUE(lexer_is_litstr(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_LITSTR_DQ);
    DUC_ASSERT_EQ(lexer->raw[0], '"');
    DUC_ASSERT_EQ(lexer->raw[1], 'T');
    DUC_ASSERT_EQ(lexer->raw[2], 'e');
    DUC_ASSERT_EQ(lexer->raw[3], 's');
    DUC_ASSERT_EQ(lexer->raw[4], 't');
    DUC_ASSERT_EQ(lexer->raw[5], '"');
    DUC_ASSERT_EQ(lexer->raw[6], '\0');
    DUC_ASSERT_EQ(lexer->str[0], 'T');
    DUC_ASSERT_EQ(lexer->str[1], 'e');
    DUC_ASSERT_EQ(lexer->str[2], 's');
    DUC_ASSERT_EQ(lexer->str[3], 't');
    DUC_ASSERT_EQ(lexer->str[4], '\0');
  });
}

DUC_TEST(lexer, is_mark) {
  LEXER_F(",", {
    DUC_ASSERT_TRUE(lexer_is_mark(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_COMMA);
    DUC_ASSERT_EQ(lexer->raw[0], ',');
    DUC_ASSERT_EQ(lexer->raw[1], '\0');
    DUC_ASSERT_EQ(lexer->str[0], ',');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });
}

DUC_TEST(lexer, is_ws) {
  LEXER_F(" ", {
    DUC_ASSERT_TRUE(lexer_is_ws(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_WS);
    DUC_ASSERT_EQ(lexer->raw[0], ' ');
    DUC_ASSERT_EQ(lexer->raw[1], '\0');
    DUC_ASSERT_EQ(lexer->str[0], ' ');
    DUC_ASSERT_EQ(lexer->str[1], '\0');
  });

  LEXER_F("  ", {
    DUC_ASSERT_TRUE(lexer_is_ws(file, lexer, 0));
    DUC_ASSERT_NE(lexer, NULL);
    DUC_ASSERT_EQ(lexer->token, LEXER_WS);
    DUC_ASSERT_EQ(lexer->raw[0], ' ');
    DUC_ASSERT_EQ(lexer->raw[1], ' ');
    DUC_ASSERT_EQ(lexer->raw[2], '\0');
    DUC_ASSERT_EQ(lexer->str[0], ' ');
    DUC_ASSERT_EQ(lexer->str[1], ' ');
    DUC_ASSERT_EQ(lexer->str[2], '\0');
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
