/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>

#include "../src/SyntaxError.hpp"
#include "ReaderMock.hpp"

TEST(SyntaxErrorTest, SameLine) {
  auto r1 = ::testing::NiceMock<MockReader>();
  auto r2 = ::testing::NiceMock<MockReader>();
  auto r3 = ::testing::NiceMock<MockReader>();
  auto r4 = ::testing::NiceMock<MockReader>();

  EXPECT_CALL(r1, next())
    .Times(2)
    .WillOnce(::testing::Return('@'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(r2, next())
    .Times(4)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(r3, next())
    .Times(5)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('3'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(r4, next())
    .Times(5)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('3'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(r1, path()).WillOnce(::testing::Return("/tmp/test.out"));
  EXPECT_CALL(r2, path()).WillOnce(::testing::Return("/tmp/test.out"));
  EXPECT_CALL(r3, path()).WillOnce(::testing::Return("/tmp/test.out"));
  EXPECT_CALL(r4, path()).WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&r1, {0, 1, 0}, {0, 1, 0}, "Inside test");
  const auto err2 = SyntaxError(&r2, {1, 1, 1}, {2, 1, 2}, "Inside test");
  const auto err3 = SyntaxError(&r3, {0, 1, 0}, {4, 1, 4}, "Inside test");
  const auto err4 = SyntaxError(&r4, {0, 1, 0}, {6, 1, 6}, "Inside test");

  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:1: Inside test\n  1 | @\n    | ^\n");
  EXPECT_STREQ(err2.what(), "/tmp/test.out:1:2: Inside test\n  1 | 012\n    |  ^\n");
  EXPECT_STREQ(err3.what(), "/tmp/test.out:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
  EXPECT_STREQ(err4.what(), "/tmp/test.out:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}

TEST(SyntaxErrorTest, AnotherLine) {
  auto r1 = ::testing::NiceMock<MockReader>();

  EXPECT_CALL(r1, next())
    .Times(5)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('3'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(r1, path()).WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&r1, {0, 1, 0}, {6, 2, 1}, "Inside test");
  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}
