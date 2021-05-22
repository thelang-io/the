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
  do { auto reader = ::testing::StrictMock<MockReader>(); \
  const auto val = std::string(s); \
  const auto len = val.length(); \
  const auto start = ReaderLocation{0, 1, 0}; \
  const auto end = ReaderLocation{len, 1, len}; \
  { \
    const auto seq = ::testing::InSequence(); \
    EXPECT_CALL(reader, loc()).WillOnce(::testing::Return(start)); \
    EXPECT_CALL(reader, eof()).Times(static_cast<int>(len)).WillRepeatedly(::testing::Return(false)); \
    for (size_t i = 0; i < len; i++) { \
      EXPECT_CALL(reader, next()).WillOnce(::testing::Return(val[i])); \
      EXPECT_CALL(reader, loc()).WillOnce(::testing::Return(ReaderLocation{i + 1, 1, i + 1})); \
    } \
  } \
  EXPECT_EQ(Lexer(&reader).next(), Token(t, val, start, end)); } while (0)

TEST(LexerTest, Operators) {
  LEX_OP(opComma, ",");
  LEX_OP(opLBrace, "{");
  LEX_OP(opLBrack, "[");
  LEX_OP(opRBrace, "}");
  LEX_OP(opRBrack, "]");
  LEX_OP(opSemi, ";");
}
