/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/keyword.h"
#include "../utilities.h"

DUC_TEST(lexer_keyword, works) {
  LEXER_FSS("fn", LEXER_FN);
  LEXER_FSS("fns", LEXER_ID);
  LEXER_FSS("in", LEXER_IN);
  LEXER_FSS("ins", LEXER_ID);
  LEXER_FSS("loop", LEXER_LOOP);
  LEXER_FSS("loops", LEXER_ID);
  LEXER_FSS("main", LEXER_MAIN);
  LEXER_FSS("mains", LEXER_ID);
  LEXER_FSS("mut", LEXER_MUT);
  LEXER_FSS("mute", LEXER_ID);
  LEXER_FSS("return", LEXER_RETURN);
  LEXER_FSS("returns", LEXER_ID);
}

int main () {
  DUC_TEST_RUN(lexer_keyword, works);
}
