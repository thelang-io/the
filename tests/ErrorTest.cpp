/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/Error.hpp"

TEST(ErrorTest, AddsPrefix) {
  Error err("An error occurred");
  EXPECT_STREQ(err.what(), "Error: An error occurred");
}
