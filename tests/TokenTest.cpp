/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
#include "../src/Token.hpp"

TEST(TokenTest, IsIdContinue) {
  EXPECT_TRUE(Token::isIdContinue('A'));
  EXPECT_TRUE(Token::isIdContinue('Z'));
  EXPECT_TRUE(Token::isIdContinue('a'));
  EXPECT_TRUE(Token::isIdContinue('z'));
  EXPECT_TRUE(Token::isIdContinue('0'));
  EXPECT_TRUE(Token::isIdContinue('9'));
  EXPECT_TRUE(Token::isIdContinue('_'));
}

TEST(TokenTest, IsIdStart) {
  EXPECT_TRUE(Token::isIdStart('A'));
  EXPECT_TRUE(Token::isIdStart('Z'));
  EXPECT_TRUE(Token::isIdStart('a'));
  EXPECT_TRUE(Token::isIdStart('z'));
  EXPECT_TRUE(Token::isIdStart('_'));
}

TEST(TokenTest, IsWhitespace) {
  EXPECT_TRUE(Token::isIdStart('\r'));
  EXPECT_TRUE(Token::isIdStart('\n'));
  EXPECT_TRUE(Token::isIdStart('\t'));
  EXPECT_TRUE(Token::isIdStart(' '));
}
