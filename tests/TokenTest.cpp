/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
#include "../src/Token.hpp"

TEST(TokenTest, IsLitCharEscape) {
  EXPECT_TRUE(Token::isLitCharEscape('0'));
  EXPECT_TRUE(Token::isLitCharEscape('t'));
  EXPECT_TRUE(Token::isLitCharEscape('n'));
  EXPECT_TRUE(Token::isLitCharEscape('r'));
  EXPECT_TRUE(Token::isLitCharEscape('"'));
  EXPECT_TRUE(Token::isLitCharEscape('\''));
  EXPECT_TRUE(Token::isLitCharEscape('\\'));
}

TEST(TokenTest, IsLitIdContinue) {
  EXPECT_TRUE(Token::isLitIdContinue('A'));
  EXPECT_TRUE(Token::isLitIdContinue('Z'));
  EXPECT_TRUE(Token::isLitIdContinue('a'));
  EXPECT_TRUE(Token::isLitIdContinue('z'));
  EXPECT_TRUE(Token::isLitIdContinue('0'));
  EXPECT_TRUE(Token::isLitIdContinue('9'));
  EXPECT_TRUE(Token::isLitIdContinue('_'));
  EXPECT_FALSE(Token::isLitIdContinue('@'));
}

TEST(TokenTest, IsLitIdStart) {
  EXPECT_TRUE(Token::isLitIdStart('A'));
  EXPECT_TRUE(Token::isLitIdStart('Z'));
  EXPECT_TRUE(Token::isLitIdStart('a'));
  EXPECT_TRUE(Token::isLitIdStart('z'));
  EXPECT_TRUE(Token::isLitIdStart('_'));
  EXPECT_FALSE(Token::isLitIdStart('@'));
}

TEST(TokenTest, IsLitIntBin) {
  EXPECT_TRUE(Token::isLitIntBin('0'));
  EXPECT_TRUE(Token::isLitIntBin('1'));
  EXPECT_FALSE(Token::isLitIntBin('2'));
}

TEST(TokenTest, IsLitIntDec) {
  EXPECT_TRUE(Token::isLitIntDec('0'));
  EXPECT_TRUE(Token::isLitIntDec('9'));
  EXPECT_FALSE(Token::isLitIntDec('A'));
}

TEST(TokenTest, IsLitIntHex) {
  EXPECT_TRUE(Token::isLitIntHex('0'));
  EXPECT_TRUE(Token::isLitIntHex('9'));
  EXPECT_TRUE(Token::isLitIntHex('A'));
  EXPECT_TRUE(Token::isLitIntHex('F'));
  EXPECT_TRUE(Token::isLitIntHex('a'));
  EXPECT_TRUE(Token::isLitIntHex('f'));
  EXPECT_FALSE(Token::isLitIntHex('G'));
  EXPECT_FALSE(Token::isLitIntHex('g'));
}

TEST(TokenTest, IsLitIntOct) {
  EXPECT_TRUE(Token::isLitIntOct('0'));
  EXPECT_TRUE(Token::isLitIntOct('7'));
  EXPECT_FALSE(Token::isLitIntOct('8'));
}

TEST(TokenTest, IsLitStrEscape) {
  EXPECT_TRUE(Token::isLitStrEscape('0'));
  EXPECT_TRUE(Token::isLitStrEscape('t'));
  EXPECT_TRUE(Token::isLitStrEscape('n'));
  EXPECT_TRUE(Token::isLitStrEscape('r'));
  EXPECT_TRUE(Token::isLitStrEscape('"'));
  EXPECT_TRUE(Token::isLitStrEscape('\''));
  EXPECT_TRUE(Token::isLitStrEscape('\\'));
  EXPECT_TRUE(Token::isLitStrEscape('{'));
  EXPECT_FALSE(Token::isLitStrEscape('}'));
}

TEST(TokenTest, IsWhitespace) {
  EXPECT_TRUE(Token::isWhitespace('\r'));
  EXPECT_TRUE(Token::isWhitespace('\n'));
  EXPECT_TRUE(Token::isWhitespace('\t'));
  EXPECT_TRUE(Token::isWhitespace(' '));
  EXPECT_FALSE(Token::isWhitespace('a'));
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

TEST(TokenTest, EqualAnnotherToken) {
  EXPECT_EQ(Token(litId, "_", {}, {}), Token(litId, "_", {}, {}));
}

TEST(TokenTest, NotEqualAnnotherToken) {
  EXPECT_NE(Token(litId, "_", {}, {}), Token(litFloat, "_", {}, {}));
  EXPECT_NE(Token(litId, "_", {}, {}), Token(litId, "a", {}, {}));
  EXPECT_NE(Token(litId, "_", {1, 1, 1}, {}), Token(litId, "_", {2, 2, 2}, {}));
  EXPECT_NE(Token(litId, "_", {}, {1, 1, 1}), Token(litId, "_", {}, {2, 2, 2}));
}

TEST(TokenTest, EqualTokenType) {
  EXPECT_EQ(Token(litId, "_", {}, {}), litId);
}

TEST(TokenTest, NotEqualTokenType) {
  EXPECT_NE(Token(litId, "_", {}, {}), litFloat);
}

TEST(TokenTest, StrWithLocation) {
  const auto tok1 = Token(litId, "_", {0, 1, 0}, {1, 1, 1});
  EXPECT_EQ(tok1.str(), "litId(1:1-1:2): _");
  const auto tok2 = Token(litId, "a_b", {13, 11, 2}, {16, 11, 5});
  EXPECT_EQ(tok2.str(), "litId(11:3-11:6): a_b");
}
