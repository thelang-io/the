/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/id.h"
#include "../utilities.h"

DUC_TEST(lexer_id, works) {
  LEXER_FSS("a", id, LEXER_ID);
  LEXER_FSS("A_b1", id, LEXER_ID);
  LEXER_FSS("_Ab1", id, LEXER_ID);
}

DUC_TEST(lexer_id, is_char_and_is_char_start) {
  DUC_ASSERT_TRUE(lexer_id_is_char('A'));
  DUC_ASSERT_TRUE(lexer_id_is_char('a'));
  DUC_ASSERT_TRUE(lexer_id_is_char('0'));
  DUC_ASSERT_TRUE(lexer_id_is_char('9'));
  DUC_ASSERT_TRUE(lexer_id_is_char('_'));
  DUC_ASSERT_FALSE(lexer_id_is_char('@'));

  DUC_ASSERT_TRUE(lexer_id_is_char_start('A'));
  DUC_ASSERT_TRUE(lexer_id_is_char_start('a'));
  DUC_ASSERT_TRUE(lexer_id_is_char_start('_'));
  DUC_ASSERT_FALSE(lexer_id_is_char_start('0'));
  DUC_ASSERT_FALSE(lexer_id_is_char_start('9'));
  DUC_ASSERT_FALSE(lexer_id_is_char_start('@'));
}

int main () {
  DUC_TEST_RUN(lexer_id, works);
  DUC_TEST_RUN(lexer_id, is_char_and_is_char_start);
}
