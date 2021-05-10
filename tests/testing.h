/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TESTS_TESTING_H
#define TESTS_TESTING_H

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_EQ(lhs_expr, rhs_expr) \
  assert((lhs_expr) == (rhs_expr))

#define ASSERT_FALSE(expr) \
  assert((expr) == false)

#define ASSERT_GE(lhs_expr, rhs_expr) \
  assert((lhs_expr) >= (rhs_expr))

#define ASSERT_GT(lhs_expr, rhs_expr) \
  assert((lhs_expr) > (rhs_expr))

#define ASSERT_LE(lhs_expr, rhs_expr) \
  assert((lhs_expr) <= (rhs_expr))

#define ASSERT_LT(lhs_expr, rhs_expr) \
  assert((lhs_expr) < (rhs_expr))

#define ASSERT_MEMEQ(lhs_expr, rhs_expr, len) \
  assert(memcmp(lhs_expr, rhs_expr, len) == 0)

#define ASSERT_MEMNE(lhs_expr, rhs_expr, len) \
  assert(memcmp(lhs_expr, rhs_expr, len) != 0)

#define ASSERT_NE(lhs_expr, rhs_expr) \
  assert((lhs_expr) != (rhs_expr))

#define ASSERT_STREQ(lhs_expr, rhs_expr) \
  assert(strcmp(lhs_expr, rhs_expr) == 0)

#define ASSERT_STRNE(lhs_expr, rhs_expr) \
  assert(strcmp(lhs_expr, rhs_expr) != 0)

#define ASSERT_TRUE(expr) \
  assert((expr) == true)

#define TEST(test_suite_name, test_name) \
  void test_suite_name##_##test_name##_test () \

#define TEST_RUN(test_suite_name, test_name) \
  test_suite_name##_##test_name##_test()

#endif
