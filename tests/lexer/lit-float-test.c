/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/lit-float.h"
#include "../lexer-test.h"

#define LEX_LIT_FLOAT_F(t) \
  LEX_FS(t, LEXER_LIT_FLOAT); \
  LEX_LIT_FLOAT_FS(t)

#define LEX_LIT_FLOAT_FS(t) \
  LEX_FS(t "E0", LEXER_LIT_FLOAT); \
  LEX_FS(t "E1", LEXER_LIT_FLOAT); \
  LEX_FS(t "E9", LEXER_LIT_FLOAT); \
  LEX_FS(t "E308", LEXER_LIT_FLOAT); \
  LEX_FS(t "E+0", LEXER_LIT_FLOAT); \
  LEX_FS(t "E+1", LEXER_LIT_FLOAT); \
  LEX_FS(t "E+9", LEXER_LIT_FLOAT); \
  LEX_FS(t "E+308", LEXER_LIT_FLOAT); \
  LEX_FS(t "E-0", LEXER_LIT_FLOAT); \
  LEX_FS(t "E-1", LEXER_LIT_FLOAT); \
  LEX_FS(t "E-9", LEXER_LIT_FLOAT); \
  LEX_FS(t "E-308", LEXER_LIT_FLOAT); \
  LEX_FS(t "e0", LEXER_LIT_FLOAT); \
  LEX_FS(t "e1", LEXER_LIT_FLOAT); \
  LEX_FS(t "e9", LEXER_LIT_FLOAT); \
  LEX_FS(t "e308", LEXER_LIT_FLOAT); \
  LEX_FS(t "e+0", LEXER_LIT_FLOAT); \
  LEX_FS(t "e+1", LEXER_LIT_FLOAT); \
  LEX_FS(t "e+9", LEXER_LIT_FLOAT); \
  LEX_FS(t "e+308", LEXER_LIT_FLOAT); \
  LEX_FS(t "e-0", LEXER_LIT_FLOAT); \
  LEX_FS(t "e-1", LEXER_LIT_FLOAT); \
  LEX_FS(t "e-9", LEXER_LIT_FLOAT); \
  LEX_FS(t "e-308", LEXER_LIT_FLOAT)

DUC_TEST(lex_lit_float, works) {
  LEX_LIT_FLOAT_FS("0");
  LEX_LIT_FLOAT_FS("1");
  LEX_LIT_FLOAT_FS("9");
  LEX_LIT_FLOAT_FS("9223372036854775807");

  LEX_LIT_FLOAT_F("0.");
  LEX_LIT_FLOAT_F("1.");
  LEX_LIT_FLOAT_F("9.");
  LEX_LIT_FLOAT_F("9223372036854775807.");

  LEX_LIT_FLOAT_F(".0");
  LEX_LIT_FLOAT_F(".1");
  LEX_LIT_FLOAT_F(".9");
  LEX_LIT_FLOAT_F(".9223372036854775807");

  LEX_LIT_FLOAT_F("0.0");
  LEX_LIT_FLOAT_F("0.1");
  LEX_LIT_FLOAT_F("0.9");
  LEX_LIT_FLOAT_F("0.9223372036854775807");
  LEX_LIT_FLOAT_F("1.0");
  LEX_LIT_FLOAT_F("1.1");
  LEX_LIT_FLOAT_F("1.9");
  LEX_LIT_FLOAT_F("1.9223372036854775807");
  LEX_LIT_FLOAT_F("9.0");
  LEX_LIT_FLOAT_F("9.1");
  LEX_LIT_FLOAT_F("9.9");
  LEX_LIT_FLOAT_F("9.9223372036854775807");
  LEX_LIT_FLOAT_F("9223372036854775807.0");
  LEX_LIT_FLOAT_F("9223372036854775807.1");
  LEX_LIT_FLOAT_F("9223372036854775807.9");
  LEX_LIT_FLOAT_F("9223372036854775807.9223372036854775807");
}

DUC_TEST(lexer_lit_float, is_char_special) {
  DUC_ASSERT_TRUE(lexer_lit_float_is_char_special('E'));
  DUC_ASSERT_TRUE(lexer_lit_float_is_char_special('e'));
  DUC_ASSERT_TRUE(lexer_lit_float_is_char_special('.'));
  DUC_ASSERT_FALSE(lexer_lit_float_is_char_special('@'));
}

int main () {
  DUC_TEST_RUN(lex_lit_float, works);
  DUC_TEST_RUN(lexer_lit_float, is_char_special);
}
