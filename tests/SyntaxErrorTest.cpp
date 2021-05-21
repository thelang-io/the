/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>

#include "../src/SyntaxError.hpp"
#include "ReaderMock.hpp"

TEST(SyntaxErrorTest, EndOffset0) {
  auto reader = ::testing::StrictMock<MockReader>();

  EXPECT_CALL(reader, seek(ReaderLocation{0, 1, 0}))
    .Times(1);

  EXPECT_CALL(reader, eof())
    .Times(2)
    .WillRepeatedly(::testing::Return(false));

  EXPECT_CALL(reader, next())
    .Times(2)
    .WillOnce(::testing::Return('@'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(reader, path())
    .Times(1)
    .WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&reader, {0, 1, 0}, {0, 1, 0}, "Inside test");
  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:1: Inside test\n  1 | @\n    | ^\n");
}

TEST(SyntaxErrorTest, EndOffset1) {
  auto reader = ::testing::StrictMock<MockReader>();

  EXPECT_CALL(reader, seek(ReaderLocation{0, 1, 0}))
    .Times(1);

  EXPECT_CALL(reader, eof())
    .Times(4)
    .WillRepeatedly(::testing::Return(false));

  EXPECT_CALL(reader, next())
    .Times(4)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(reader, path())
    .Times(1)
    .WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&reader, {1, 1, 1}, {2, 1, 2}, "Inside test");
  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:2: Inside test\n  1 | 012\n    |  ^\n");
}

TEST(SyntaxErrorTest, EndOffset4) {
  auto reader = ::testing::StrictMock<MockReader>();

  EXPECT_CALL(reader, seek(ReaderLocation{0, 1, 0}))
    .Times(1);

  EXPECT_CALL(reader, eof())
    .Times(5)
    .WillRepeatedly(::testing::Return(false));

  EXPECT_CALL(reader, next())
    .Times(5)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('3'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(reader, path())
    .Times(1)
    .WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&reader, {0, 1, 0}, {4, 1, 4}, "Inside test");
  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}

TEST(SyntaxErrorTest, EndAfterEOL) {
  auto reader = ::testing::StrictMock<MockReader>();

  EXPECT_CALL(reader, seek(ReaderLocation{0, 1, 0}))
    .Times(1);

  EXPECT_CALL(reader, eof())
    .Times(5)
    .WillRepeatedly(::testing::Return(false));

  EXPECT_CALL(reader, next())
    .Times(5)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('3'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(reader, path())
    .Times(1)
    .WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&reader, {0, 1, 0}, {6, 1, 6}, "Inside test");
  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}

TEST(SyntaxErrorTest, EndOnNextLine) {
  auto reader = ::testing::StrictMock<MockReader>();

  EXPECT_CALL(reader, seek(ReaderLocation{0, 1, 0}))
    .Times(1);

  EXPECT_CALL(reader, eof())
    .Times(5)
    .WillRepeatedly(::testing::Return(false));

  EXPECT_CALL(reader, next())
    .Times(5)
    .WillOnce(::testing::Return('0'))
    .WillOnce(::testing::Return('1'))
    .WillOnce(::testing::Return('2'))
    .WillOnce(::testing::Return('3'))
    .WillOnce(::testing::Return('\n'));

  EXPECT_CALL(reader, path())
    .Times(1)
    .WillOnce(::testing::Return("/tmp/test.out"));

  const auto err1 = SyntaxError(&reader, {0, 1, 0}, {6, 2, 1}, "Inside test");
  EXPECT_STREQ(err1.what(), "/tmp/test.out:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}
