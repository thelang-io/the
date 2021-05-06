/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TESTS_LEXER_TEST_H
#define TESTS_LEXER_TEST_H

#define LEX_F(text, body) \
  do { duc_writefile("../test.txt", text); \
  duc_file_t *file = duc_file_new("../test.txt", DUC_FILE_READ); \
  lexer_t *lexer = lexer_new(file); \
  printf("tok: %s, raw: %s, str: %s\n", lexer_token_str[lexer->token], lexer->raw, lexer->str); \
  body \
  lexer_free(lexer); \
  duc_file_free(file); \
  duc_file_remove("../test.txt"); } while (0)

#define LEX_FE(raw_text, str_text, tok) \
  LEX_F(raw_text, { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  }); \
  LEX_F(raw_text " ", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  })

#define LEX_FS(text, tok) \
  LEX_F(text, { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text " ", { \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

#endif
