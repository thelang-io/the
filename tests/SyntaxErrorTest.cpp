/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>

#include "../src/SyntaxError.hpp"
#include "ReaderMock.hpp"

#define SYNTAX_ERROR(t, s0, s1, s2, e1, e2, e3, w) \
  do { auto r = ::testing::NiceMock<MockReader>(t); \
  EXPECT_STREQ(SyntaxError(&r, {s0, s1, s2}, {e1, e2, e3}, "Test").what(), "/bin/sh:" w); } while (0)

TEST(SyntaxErrorTest, SameLine) {
  SYNTAX_ERROR("@", 0, 1, 0, 0, 1, 0, "1:1: Test\n  1 | @\n    | ^\n");
  SYNTAX_ERROR("@\n", 0, 1, 0, 0, 1, 0, "1:1: Test\n  1 | @\n    | ^\n");
  SYNTAX_ERROR("0123", 1, 1, 1, 2, 1, 2, "1:2: Test\n  1 | 0123\n    |  ^\n");
  SYNTAX_ERROR("0123\n", 1, 1, 1, 2, 1, 2, "1:2: Test\n  1 | 0123\n    |  ^\n");
  SYNTAX_ERROR("0123", 0, 1, 0, 4, 1, 4, "1:1: Test\n  1 | 0123\n    | ^~~~\n");
  SYNTAX_ERROR("0123\n", 0, 1, 0, 4, 1, 4, "1:1: Test\n  1 | 0123\n    | ^~~~\n");
  SYNTAX_ERROR("0123", 0, 1, 0, 6, 1, 6, "1:1: Test\n  1 | 0123\n    | ^~~~\n");
  SYNTAX_ERROR("0123\n", 0, 1, 0, 6, 1, 6, "1:1: Test\n  1 | 0123\n    | ^~~~\n");
}

TEST(SyntaxErrorTest, AnotherLine) {
  SYNTAX_ERROR("0123", 0, 1, 0, 6, 2, 1, "1:1: Test\n  1 | 0123\n    | ^~~~\n");
  SYNTAX_ERROR("0123\n", 0, 1, 0, 6, 2, 1, "1:1: Test\n  1 | 0123\n    | ^~~~\n");
}
