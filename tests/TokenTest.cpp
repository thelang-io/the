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
  EXPECT_TRUE(Token::isWhitespace('\r'));
  EXPECT_TRUE(Token::isWhitespace('\n'));
  EXPECT_TRUE(Token::isWhitespace('\t'));
  EXPECT_TRUE(Token::isWhitespace(' '));
}

TEST(TokenTest, CtorSetConstMembers) {
  const auto start = ReaderLocation{1, 1, 1};
  const auto end = ReaderLocation{2, 1, 2};
  const auto tok = Token(litId, "_", start, end);

  EXPECT_EQ(tok.end, end);
  EXPECT_EQ(tok.start, start);
  EXPECT_EQ(tok.type, litId);
  EXPECT_EQ(tok.val, "_");
}
