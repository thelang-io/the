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

TEST(LexerTest, Operators) {
  auto reader = ::testing::StrictMock<MockReader>();

  EXPECT_CALL(reader, eof())
    .Times(1)
    .WillRepeatedly(::testing::Return(false));

  EXPECT_CALL(reader, loc())
    .Times(2)
    .WillOnce(::testing::Return(ReaderLocation{1, 1, 1}))
    .WillOnce(::testing::Return(ReaderLocation{2, 1, 2}));

  EXPECT_CALL(reader, next())
    .Times(1)
    .WillOnce(::testing::Return(';'));

  auto lexer = Lexer(&reader);
  EXPECT_EQ(lexer.next(), Token(opSemi, ";", {1, 1, 1}, {2, 1, 2}));
}
