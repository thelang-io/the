/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-id.h"
#include "utilities.h"

DUC_TEST(lex_lit_id, works) {
  LEX_FS("a", LEXER_LIT_ID);
  LEX_FS("A_b1", LEXER_LIT_ID);
  LEX_FS("_Ab1", LEXER_LIT_ID);

  LEX_FS("fn", LEXER_KW_FN);
  LEX_FS("fns", LEXER_LIT_ID);
  LEX_FS("in", LEXER_KW_IN);
  LEX_FS("ins", LEXER_LIT_ID);
  LEX_FS("loop", LEXER_KW_LOOP);
  LEX_FS("loops", LEXER_LIT_ID);
  LEX_FS("main", LEXER_KW_MAIN);
  LEX_FS("mains", LEXER_LIT_ID);
  LEX_FS("mut", LEXER_KW_MUT);
  LEX_FS("mute", LEXER_LIT_ID);
  LEX_FS("return", LEXER_KW_RETURN);
  LEX_FS("returns", LEXER_LIT_ID);
}

DUC_TEST(lexer_lit_id, is_char_and_is_char_start) {
  DUC_ASSERT_TRUE(lexer_lit_id_is_char('A'));
  DUC_ASSERT_TRUE(lexer_lit_id_is_char('a'));
  DUC_ASSERT_TRUE(lexer_lit_id_is_char('0'));
  DUC_ASSERT_TRUE(lexer_lit_id_is_char('9'));
  DUC_ASSERT_TRUE(lexer_lit_id_is_char('_'));
  DUC_ASSERT_FALSE(lexer_lit_id_is_char('@'));

  DUC_ASSERT_TRUE(lexer_lit_id_is_char_start('A'));
  DUC_ASSERT_TRUE(lexer_lit_id_is_char_start('a'));
  DUC_ASSERT_TRUE(lexer_lit_id_is_char_start('_'));
  DUC_ASSERT_FALSE(lexer_lit_id_is_char_start('0'));
  DUC_ASSERT_FALSE(lexer_lit_id_is_char_start('9'));
  DUC_ASSERT_FALSE(lexer_lit_id_is_char_start('@'));
}

int main () {
  DUC_TEST_RUN(lex_lit_id, works);
  DUC_TEST_RUN(lexer_lit_id, is_char_and_is_char_start);
}
