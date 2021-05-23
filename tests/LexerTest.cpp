/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>

#include "../src/Lexer.hpp"
#include "ReaderMock.hpp"

#define LEX(t, c, v) \
  do { auto r = ::testing::NiceMock<MockReader>(c); \
  const auto val = std::string(v); \
  size_t line = 1; \
  size_t col = 0; \
  for (size_t i = 0, size = val.length(); i < size; i++) { \
    if (val[i] == '\n') { \
      col = 0; \
      line += 1; \
    } else { \
      col += 1; \
    } \
  } \
  EXPECT_EQ(Lexer(&r).next(), Token(t, v, {0, 1, 0}, {sizeof(v) - 1, line, col})); } while (0)

#define LEX_WS(t, v) \
  LEX(t, v, v); \
  LEX(t, v "\n", v)

#define LEX_NUM(t, v) \
  LEX(t, v, v); \
  LEX(t, v "\n", v); \
  LEX(t, v "..", v)

TEST(LexerTest, Misc) {
  LEX(eof, "", "");
  LEX(whitespace, "\n", "\n");
  LEX(whitespace, "\r", "\r");
  LEX(whitespace, "\t", "\t");
  LEX(whitespace, " ", " ");
  LEX(whitespace, "  ", "  ");
  LEX(whitespace, "\n\r\t ", "\n\r\t ");
}

TEST(LexerTest, Comments) {
  LEX_WS(commentLine, "//");
  LEX_WS(commentLine, "//text");
  LEX_WS(commentBlock, "/**/");
  LEX_WS(commentBlock, "/*text*/");
  LEX_WS(commentBlock, "/*\n*/");
  LEX_WS(commentBlock, "/*text\ntext*/");
  LEX_WS(commentBlock, "/*\n\n*/");
  LEX_WS(commentBlock, "/*text\ntext\ntext*/");
}

TEST(LexerTest, Literals) {
  LEX_WS(litChar, "' '");
  LEX_WS(litChar, "'!'");
  LEX_WS(litChar, "'A'");
  LEX_WS(litChar, "'a'");
  LEX_WS(litChar, "'0'");
  LEX_WS(litChar, "'9'");
  LEX_WS(litChar, "'\\0'");
  LEX_WS(litChar, "'\\t'");
  LEX_WS(litChar, "'\\n'");
  LEX_WS(litChar, "'\\r'");
  LEX_WS(litChar, "'\\\"'");
  LEX_WS(litChar, "'\\''");
  LEX_WS(litChar, "'\\\\'");

  LEX_WS(litId, "_");
  LEX_WS(litId, "A");
  LEX_WS(litId, "a");
  LEX_WS(litId, "Aa_09");
  LEX_WS(litId, "_zZ12");

  LEX_NUM(litIntBin, "0B0");
  LEX_NUM(litIntBin, "0b0");
  LEX_NUM(litIntBin, "0B1");
  LEX_NUM(litIntBin, "0b1");
  LEX_NUM(litIntBin, "0B01111111111111111111111111111111");
  LEX_NUM(litIntBin, "0b01111111111111111111111111111111");
  LEX_NUM(litIntBin, "0B11111111111111111111111111111111");
  LEX_NUM(litIntBin, "0b11111111111111111111111111111111");
}

TEST(LexerTest, Operators) {
  LEX_WS(opAnd, "&");
  LEX_WS(opAndAnd, "&&");
  LEX_WS(opAndAndEq, "&&=");
  LEX_WS(opAndEq, "&=");
  LEX_WS(opCaret, "^");
  LEX_WS(opCaretEq, "^=");
  LEX_WS(opColon, ":");
  LEX_WS(opColonEq, ":=");
  LEX_WS(opComma, ",");
  LEX_WS(opDot, ".");
  LEX_WS(opDotDot, "..");
  LEX_WS(opDotDotDot, "...");
  LEX_WS(opDotDotEq, "..=");
  LEX_WS(opEq, "=");
  LEX_WS(opEqEq, "==");
  LEX_WS(opExcl, "!");
  LEX_WS(opExclEq, "!=");
  LEX_WS(opExclExcl, "!!");
  LEX_WS(opGt, ">");
  LEX_WS(opGtEq, ">=");
  LEX_WS(opLBrace, "{");
  LEX_WS(opLBrack, "[");
  LEX_WS(opLPar, "(");
  LEX_WS(opLShift, "<<");
  LEX_WS(opLShiftEq, "<<=");
  LEX_WS(opLt, "<");
  LEX_WS(opLtEq, "<=");
  LEX_WS(opMinus, "-");
  LEX_WS(opMinusEq, "-=");
  LEX_WS(opMinusMinus, "--");
  LEX_WS(opOr, "|");
  LEX_WS(opOrEq, "|=");
  LEX_WS(opOrOr, "||");
  LEX_WS(opOrOrEq, "||=");
  LEX_WS(opPercent, "%");
  LEX_WS(opPercentEq, "%=");
  LEX_WS(opPlus, "+");
  LEX_WS(opPlusEq, "+=");
  LEX_WS(opPlusPlus, "++");
  LEX_WS(opQn, "?");
  LEX_WS(opQnDot, "?.");
  LEX_WS(opQnQn, "??");
  LEX_WS(opQnQnEq, "\?\?=");
  LEX_WS(opRBrace, "}");
  LEX_WS(opRBrack, "]");
  LEX_WS(opRPar, ")");
  LEX_WS(opRShift, ">>");
  LEX_WS(opRShiftEq, ">>=");
  LEX_WS(opSemi, ";");
  LEX_WS(opSlash, "/");
  LEX_WS(opSlashEq, "/=");
  LEX_WS(opStar, "*");
  LEX_WS(opStarEq, "*=");
  LEX_WS(opStarStar, "**");
  LEX_WS(opStarStarEq, "**=");
  LEX_WS(opTilde, "~");
}
