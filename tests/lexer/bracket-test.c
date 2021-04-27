/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/bracket.h"
#include "../utilities.h"

DUC_TEST(lexer_bracket, works) {
  LEXER_FS("{", LEXER_LBRACE);
  LEXER_FS("[", LEXER_LBRACK);
  LEXER_FS("(", LEXER_LPAR);
  LEXER_FS("}", LEXER_RBRACE);
  LEXER_FS("]", LEXER_RBRACK);
  LEXER_FS(")", LEXER_RPAR);
}

int main () {
  DUC_TEST_RUN(lexer_bracket, works);
}
