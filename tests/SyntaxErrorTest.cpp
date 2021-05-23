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
  auto r1 = ::testing::NiceMock<MockReader>("@\n");
  auto r2 = ::testing::NiceMock<MockReader>("0123\n");
  auto r3 = ::testing::NiceMock<MockReader>("0123\n");
  auto r4 = ::testing::NiceMock<MockReader>("0123\n");

  const auto err1 = SyntaxError(&r1, {0, 1, 0}, {0, 1, 0}, "Inside test");
  const auto err2 = SyntaxError(&r2, {1, 1, 1}, {2, 1, 2}, "Inside test");
  const auto err3 = SyntaxError(&r3, {0, 1, 0}, {4, 1, 4}, "Inside test");
  const auto err4 = SyntaxError(&r4, {0, 1, 0}, {6, 1, 6}, "Inside test");

  EXPECT_STREQ(err1.what(), "/bin/sh:1:1: Inside test\n  1 | @\n    | ^\n");
  EXPECT_STREQ(err2.what(), "/bin/sh:1:2: Inside test\n  1 | 0123\n    |  ^\n");
  EXPECT_STREQ(err3.what(), "/bin/sh:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
  EXPECT_STREQ(err4.what(), "/bin/sh:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}

TEST(SyntaxErrorTest, AnotherLine) {
  auto r1 = ::testing::NiceMock<MockReader>("0123\n");
  const auto err1 = SyntaxError(&r1, {0, 1, 0}, {6, 2, 1}, "Inside test");
  EXPECT_STREQ(err1.what(), "/bin/sh:1:1: Inside test\n  1 | 0123\n    | ^~~~\n");
}
