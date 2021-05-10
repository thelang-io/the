/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "../src/array.h"
#include "testing.h"

TEST(array, at) {
  int *a = malloc(sizeof(int));
  int *b = malloc(sizeof(int));

  *a = 1;
  *b = 2;

  array_t *arr = array_new();

  array_push(arr, a);
  array_push(arr, b);
  array_push(arr, NULL);

  ASSERT_EQ(*((int *) array_at(arr, 0)), 1);
  ASSERT_EQ(*((int *) array_at(arr, 1)), 2);
  ASSERT_EQ(array_at(arr, 2), NULL);
  ASSERT_EQ(array_at(arr, 3), NULL);
  ASSERT_EQ(array_at(arr, 4), NULL);

  array_free(arr, duc_free_simple);
}

TEST(array, empty_and_length) {
  array_t *arr = array_new();

  ASSERT_TRUE(array_empty(arr));
  ASSERT_EQ(array_length(arr), 0);
  array_push(arr, NULL);

  ASSERT_FALSE(array_empty(arr));
  ASSERT_EQ(array_length(arr), 1);
  array_push(arr, NULL);

  ASSERT_FALSE(array_empty(arr));
  ASSERT_EQ(array_length(arr), 2);

  array_free(arr, duc_free_simple);
}

TEST(array, new_and_free) {
  array_t *arr = array_new();
  ASSERT_NE(arr, NULL);

  array_push(arr, NULL);
  array_push(arr, NULL);
  array_push(arr, NULL);
  array_free(arr, duc_free_simple);
  array_free(array_new(), duc_free_simple);
}

TEST(array, push_and_shift) {
  array_t *arr = array_new();

  ASSERT_EQ(array_length(arr), 0);
  array_push(arr, NULL);
  ASSERT_EQ(array_length(arr), 1);
  array_push(arr, NULL);
  ASSERT_EQ(array_length(arr), 2);

  ASSERT_EQ(array_shift(arr), NULL);
  ASSERT_EQ(array_length(arr), 1);
  ASSERT_EQ(array_shift(arr), NULL);
  ASSERT_EQ(array_length(arr), 0);
  ASSERT_EQ(array_shift(arr), NULL);
  ASSERT_EQ(array_length(arr), 0);

  array_free(arr, duc_free_simple);
}

int main () {
  TEST_RUN(array, at);
  TEST_RUN(array, empty_and_length);
  TEST_RUN(array, new_and_free);
  TEST_RUN(array, push_and_shift);
}
