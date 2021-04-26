/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/ws.h"
#include "../utilities.h"

#define LEXER_WS_F(text) \
  LEXER_FS(text, ws, LEXER_WS); \
  LEXER_F(text "t", { \
    DUC_ASSERT_TRUE(lexer_ws(file, lexer, 0)); \
    DUC_ASSERT_NE(lexer, NULL); \
    DUC_ASSERT_EQ(lexer->token, LEXER_WS); \
    DUC_ASSERT_MEMEQ(lexer->raw, text, strlen(text) + 1); \
    DUC_ASSERT_MEMEQ(lexer->str, text, strlen(text) + 1); \
  })

DUC_TEST(lexer_ws, works) {
  LEXER_WS_F(" ");
  LEXER_WS_F("\t\t");
}

int main () {
  DUC_TEST_RUN(lexer_ws, works);
}
