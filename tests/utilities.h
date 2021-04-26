/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TESTS_UTILITIES_H
#define TESTS_UTILITIES_H

#include <stdlib.h>

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

#define LEXER_FS(text, method, tok) \
  LEXER_F(text, { \
    DUC_ASSERT_TRUE(lexer_##method(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

#define LEXER_FSS(text, method, tok) \
  LEXER_FS(text, method, tok); \
  LEXER_F(text " ", { \
    DUC_ASSERT_TRUE(lexer_##method(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, tok); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

#endif
