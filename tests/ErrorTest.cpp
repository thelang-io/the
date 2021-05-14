/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
#include "../src/Error.hpp"

TEST(ErrorTest, AddsPrefix) {
  const auto err = Error("Inside test");
  EXPECT_EQ(err.message, "Error: Inside test");
}

TEST(LexerTest, ConstructsAndAddsPrefix) {
  const auto err1 = Error("Inside test1", "/var/tmp/test.out");
  EXPECT_EQ(err1.message, "Error: Inside test1\n  at /var/tmp/test.out");
  const auto err2 = Error("Inside test2", "/var/tmp/test.out", 2);
  EXPECT_EQ(err2.message, "Error: Inside test2\n  at /var/tmp/test.out:2");
  const auto err3 = Error("Inside test3", "/var/tmp/test.out", 3, 4);
  EXPECT_EQ(err3.message, "Error: Inside test3\n  at /var/tmp/test.out:3:4");
}
