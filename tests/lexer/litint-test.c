/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/litint.h"
#include "../utilities.h"

#define LEXER_LITINT_F(letter1, letter2, text, tok) \
  LEXER_FSS("0" letter1 text, litint, tok); \
  LEXER_FSS("0" letter2 text, litint, tok)

DUC_TEST(lexer_litint, works) {
  LEXER_FSS("0", litint, LEXER_LITINT_DEC);
  LEXER_FSS("1", litint, LEXER_LITINT_DEC);
  LEXER_FSS("9", litint, LEXER_LITINT_DEC);
  LEXER_FSS("2147483647", litint, LEXER_LITINT_DEC);
  LEXER_FSS("01", litint, LEXER_LITINT_OCT);
  LEXER_FSS("07", litint, LEXER_LITINT_OCT);
  LEXER_FSS("017777777777", litint, LEXER_LITINT_OCT);
  LEXER_LITINT_F("O", "o", "0", LEXER_LITINT_OCT);
  LEXER_LITINT_F("O", "o", "1", LEXER_LITINT_OCT);
  LEXER_LITINT_F("O", "o", "7", LEXER_LITINT_OCT);
  LEXER_LITINT_F("O", "o", "17777777777", LEXER_LITINT_OCT);
  LEXER_LITINT_F("B", "b", "0", LEXER_LITINT_BIN);
  LEXER_LITINT_F("B", "b", "1", LEXER_LITINT_BIN);
  LEXER_LITINT_F("B", "b", "01111111111111111111111111111111", LEXER_LITINT_BIN);
  LEXER_LITINT_F("X", "x", "0", LEXER_LITINT_HEX);
  LEXER_LITINT_F("X", "x", "1", LEXER_LITINT_HEX);
  LEXER_LITINT_F("X", "x", "9", LEXER_LITINT_HEX);
  LEXER_LITINT_F("X", "x", "F", LEXER_LITINT_HEX);
  LEXER_LITINT_F("X", "x", "7FFFFFFF", LEXER_LITINT_HEX);
}

int main () {
  DUC_TEST_RUN(lexer_litint, works);
}
