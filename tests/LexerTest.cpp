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

#define LEX_OP(t, s) \
  do { auto reader = ::testing::NiceMock<MockReader>(); \
  const auto val = std::string(s); \
  const auto len = val.length(); \
  const auto start = ReaderLocation{0, 1, 0}; \
  const auto end = ReaderLocation{len, 1, len}; \
  EXPECT_CALL(reader, eof()).WillRepeatedly(::testing::Return(true)); \
  EXPECT_CALL(reader, eof()) \
    .Times(static_cast<int>(len)) \
    .WillRepeatedly(::testing::Return(false)) \
    .RetiresOnSaturation(); \
  { \
    const auto seq = ::testing::InSequence(); \
    for (size_t i = 0; i < len; i++) { \
      EXPECT_CALL(reader, next()).WillOnce(::testing::Return(val[i])); \
      EXPECT_CALL(reader, loc()).WillOnce(::testing::Return(ReaderLocation{i + 1, 1, i + 1})); \
    } \
  } \
  EXPECT_CALL(reader, loc()).WillOnce(::testing::Return(start)).RetiresOnSaturation(); \
  EXPECT_EQ(Lexer(&reader).next(), Token(t, val, start, end)); } while (0)

TEST(LexerTest, Eof) {
  auto reader = ::testing::NiceMock<MockReader>();

  EXPECT_CALL(reader, loc())
    .WillOnce(::testing::Return(ReaderLocation{0, 1, 0}))
    .WillOnce(::testing::Return(ReaderLocation{1, 1, 1}));

  EXPECT_CALL(reader, eof()).WillOnce(::testing::Return(true));
  EXPECT_EQ(Lexer(&reader).next(), Token(eof, std::string(1, -1), {0, 1, 0}, {1, 1, 1}));
}

TEST(LexerTest, Operators) {
  LEX_OP(opAnd, "&");
  LEX_OP(opAndAnd, "&&");
  LEX_OP(opAndEq, "&=");
  LEX_OP(opCaretEq, "^=");
  LEX_OP(opColonEq, ":=");
  LEX_OP(opComma, ",");
  LEX_OP(opEqEq, "==");
  LEX_OP(opExclEq, "!=");
  LEX_OP(opExclExcl, "!!");
  LEX_OP(opLBrace, "{");
  LEX_OP(opLBrack, "[");
  LEX_OP(opLPar, "(");
  LEX_OP(opLtEq, "<=");
  LEX_OP(opLShiftEq, "<<=");
  LEX_OP(opRBrace, "}");
  LEX_OP(opRBrack, "]");
  LEX_OP(opRPar, ")");
  LEX_OP(opSemi, ";");
  LEX_OP(opTilde, "~");
}
