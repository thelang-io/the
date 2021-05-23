/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
#include <fstream>

#include "../src/Lexer.hpp"
#include "ReaderMock.hpp"

#define LEX(t, c, v) \
  do { auto r = ::testing::NiceMock<MockReader>(c); \
  EXPECT_EQ(Lexer(&r).next(), Token(t, v, {0, 1, 0}, {sizeof(v), 1, sizeof(v)})); } while (0)

TEST(LexerTest, Misc) {
  LEX(eof, "", "");
}

TEST(LexerTest, Operators) {
  LEX(opAnd, "&", "&");
  LEX(opAndAnd, "&&", "&&");
  LEX(opAndAndEq, "&&=", "&&=");
  LEX(opAndEq, "&=", "&=");
  LEX(opCaret, "^", "^");
  LEX(opCaretEq, "^=", "^=");
  LEX(opColon, ":", ":");
  LEX(opColonEq, ":=", ":=");
  LEX(opComma, ",", ",");
  LEX(opDot, ".", ".");
  LEX(opDotDot, "..", "..");
  LEX(opDotDotDot, "...", "...");
  LEX(opDotDotEq, "..=", "..=");
  LEX(opEq, "=", "=");
  LEX(opEqEq, "==", "==");
  LEX(opExcl, "!", "!");
  LEX(opExclEq, "!=", "!=");
  LEX(opExclExcl, "!!", "!!");
  LEX(opGt, ">", ">");
  LEX(opGtEq, ">=", ">=");
  LEX(opLBrace, "{", "{");
  LEX(opLBrack, "[", "[");
  LEX(opLPar, "(", "(");
  LEX(opLShift, "<<", "<<");
  LEX(opLShiftEq, "<<=", "<<=");
  LEX(opLt, "<", "<");
  LEX(opLtEq, "<=", "<=");
  LEX(opMinus, "-", "-");
  LEX(opMinusEq, "-=", "-=");
  LEX(opMinusMinus, "--", "--");
  LEX(opOr, "|", "|");
  LEX(opOrEq, "|=", "|=");
  LEX(opOrOr, "||", "||");
  LEX(opOrOrEq, "||=", "||=");
  LEX(opPercent, "%", "%");
  LEX(opPercentEq, "%=", "%=");
  LEX(opPlus, "+", "+");
  LEX(opPlusEq, "+=", "+=");
  LEX(opPlusPlus, "++", "++");
  LEX(opQn, "?", "?");
  LEX(opQnDot, "?.", "?.");
  LEX(opQnQn, "??", "??");
  LEX(opQnQnEq, "\?\?=", "\?\?=");
  LEX(opRBrace, "}", "}");
  LEX(opRBrack, "]", "]");
  LEX(opRPar, ")", ")");
  LEX(opRShift, ">>", ">>");
  LEX(opRShiftEq, ">>=", ">>=");
  LEX(opSemi, ";", ";");
  LEX(opSlash, "/", "/");
  LEX(opSlashEq, "/=", "/=");
  LEX(opStar, "*", "*");
  LEX(opStarEq, "*=", "*=");
  LEX(opStarStar, "**", "**");
  LEX(opStarStarEq, "**=", "**=");
  LEX(opTilde, "~", "~");
}
