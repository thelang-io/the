/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
#include <fstream>

#include "../src/Lexer.hpp"

#define LEX_START(text) \
  do { std::ofstream f("test.out"); \
  f << text; \
  f.close(); } while (0);

#define LEX_END() fs::remove("test.out");

TEST(LexerTest, Operators) {
  const char *text = "test";

  LEX_START(text)
  LEX_END()
}
