/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/mark.h"
#include "../utilities.h"

DUC_TEST(lexer_mark, works) {
  LEXER_FS(":", LEXER_COLON);
  LEXER_FS(",", LEXER_COMMA);
  LEXER_FS(".", LEXER_DOT);
}

int main () {
  DUC_TEST_RUN(lexer_mark, works);
}
