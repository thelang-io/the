/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/op.h"
#include "../lexer-test.h"

#define LEX_OP_F(op, tok) \
  LEX_FS(op, tok); \
  LEX_FS(op "=", tok##EQ)

DUC_TEST(lex_op, works) {
  LEX_OP_F("&", LEXER_OP_AND);
  LEX_OP_F("&&", LEXER_OP_ANDAND);
  LEX_OP_F("^", LEXER_OP_CARET);
  LEX_OP_F(":", LEXER_OP_COLON);
  LEX_OP_F("=", LEXER_OP_EQ);
  LEX_OP_F("!", LEXER_OP_EXCL);
  LEX_OP_F(">", LEXER_OP_GT);
  LEX_OP_F("<<", LEXER_OP_LSHIFT);
  LEX_OP_F("<", LEXER_OP_LT);
  LEX_OP_F("-", LEXER_OP_MINUS);
  LEX_OP_F("|", LEXER_OP_OR);
  LEX_OP_F("||", LEXER_OP_OROR);
  LEX_OP_F("%", LEXER_OP_PERCENT);
  LEX_OP_F("+", LEXER_OP_PLUS);
  LEX_OP_F("??", LEXER_OP_QNQN);
  LEX_OP_F(">>", LEXER_OP_RSHIFT);
  LEX_OP_F("/", LEXER_OP_SLASH);
  LEX_OP_F("*", LEXER_OP_STAR);
  LEX_OP_F("**", LEXER_OP_STARSTAR);

  LEX_FS(",", LEXER_OP_COMMA);
  LEX_FS(".", LEXER_OP_DOT);
  LEX_FS("{", LEXER_OP_LBRACE);
  LEX_FS("[", LEXER_OP_LBRACK);
  LEX_FS("(", LEXER_OP_LPAR);
  LEX_FS("?", LEXER_OP_QN);
  LEX_FS("}", LEXER_OP_RBRACE);
  LEX_FS("]", LEXER_OP_RBRACK);
  LEX_FS(")", LEXER_OP_RPAR);
  LEX_FS(";", LEXER_OP_SEMI);
  LEX_FS("~", LEXER_OP_TILDE);
}

int main () {
  DUC_TEST_RUN(lex_op, works);
}
