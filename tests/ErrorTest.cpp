/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
#include "../src/Error.hpp"

TEST(ErrorTest, AddsPrefix) {
  auto err = Error("An error occurred");
  EXPECT_EQ(err.message, "Error: An error occurred");
}
