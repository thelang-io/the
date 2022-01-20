/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/Error.hpp"
#include "utils.hpp"

TEST(ErrorTest, ThrowsExactMessage) {
  EXPECT_THROW_WITH_MESSAGE({
    throw Error("Hello, World!");
  }, Error, "Hello, World!");
}
