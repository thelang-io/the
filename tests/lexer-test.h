/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TESTS_LEXER_TEST_H
#define TESTS_LEXER_TEST_H

#include "testing.h"

#define LEX_F(text, body) \
  do { writefile("../test.out", text); \
  file_t *file = file_new("../test.out", FILE_READ); \
  lexer_t *lexer = lexer_new(file); \
  printf("tok: %s, raw: %s, str: %s\n", lexer_token_str[lexer->token], lexer->raw, lexer->str); \
  body \
  lexer_free(lexer); \
  file_free(file); \
  file_remove("../test.out"); } while (0)

#define LEX_FE(raw_text, str_text, tok) \
  LEX_F(raw_text, { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, tok); \
    ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  }); \
  LEX_F(raw_text " ", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, tok); \
    ASSERT_MEMEQ(lexer->raw, raw_text, strlen(raw_text) + 1); \
    ASSERT_MEMEQ(lexer->str, str_text, strlen(str_text) + 1); \
  })

#define LEX_FS(text, tok) \
  LEX_F(text, { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, tok); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  }); \
  LEX_F(text " ", { \
    ASSERT_NE(lexer, NULL); \
    ASSERT_EQ(lexer->token, tok); \
    ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

#endif
