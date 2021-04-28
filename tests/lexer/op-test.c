/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/op.h"
#include "../lexer-test.h"

DUC_TEST(lex_op, works) {
  LEX_FS("&", LEXER_OP_AND);
  LEX_FS(":", LEXER_OP_COLON);
  LEX_FS(",", LEXER_OP_COMMA);
  LEX_FS(".", LEXER_OP_DOT);
  LEX_FS("=", LEXER_OP_EQ);
  LEX_FS("!", LEXER_OP_EXCL);
  LEX_FS(">", LEXER_OP_GT);
  LEX_FS("{", LEXER_OP_LBRACE);
  LEX_FS("[", LEXER_OP_LBRACK);
  LEX_FS("(", LEXER_OP_LPAR);
  LEX_FS("<", LEXER_OP_LT);
  LEX_FS("-", LEXER_OP_MINUS);
  LEX_FS("|", LEXER_OP_OR);
  LEX_FS("%", LEXER_OP_PERCENT);
  LEX_FS("+", LEXER_OP_PLUS);
  LEX_FS("}", LEXER_OP_RBRACE);
  LEX_FS("]", LEXER_OP_RBRACK);
  LEX_FS(")", LEXER_OP_RPAR);
  LEX_FS("/", LEXER_OP_SLASH);
  LEX_FS("*", LEXER_OP_STAR);
}

int main () {
  DUC_TEST_RUN(lex_op, works);
}
