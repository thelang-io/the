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
  EXPECT_STREQ(err.what(), "Error: Inside test");
}
