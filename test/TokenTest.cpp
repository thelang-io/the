/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/Token.hpp"
#include "utils.hpp"

TEST(TokenTest, IsDigit) {
  EXPECT_TRUE(Token::isDigit('0'));
  EXPECT_TRUE(Token::isDigit('9'));
}

TEST(TokenTest, IsNotDigit) {
  EXPECT_FALSE(Token::isDigit('a'));
  EXPECT_FALSE(Token::isDigit('#'));
}

TEST(TokenTest, IsIdContinue) {
  EXPECT_TRUE(Token::isIdContinue('a'));
  EXPECT_TRUE(Token::isIdContinue('Z'));
  EXPECT_TRUE(Token::isIdContinue('0'));
  EXPECT_TRUE(Token::isIdContinue('_'));
}

TEST(TokenTest, IsNotIdContinue) {
  EXPECT_FALSE(Token::isIdContinue('#'));
  EXPECT_FALSE(Token::isIdContinue(' '));
}

TEST(TokenTest, IsIdStart) {
  EXPECT_TRUE(Token::isIdStart('a'));
  EXPECT_TRUE(Token::isIdStart('Z'));
  EXPECT_TRUE(Token::isIdStart('_'));
}

TEST(TokenTest, IsNotIdStart) {
  EXPECT_FALSE(Token::isIdStart('0'));
  EXPECT_FALSE(Token::isIdStart('#'));
}

TEST(TokenTest, IsLitCharEscape) {
  EXPECT_TRUE(Token::isLitCharEscape('0'));
  EXPECT_TRUE(Token::isLitCharEscape('t'));
  EXPECT_TRUE(Token::isLitCharEscape('n'));
}

TEST(TokenTest, IsNotLitCharEscape) {
  EXPECT_FALSE(Token::isLitCharEscape('T'));
  EXPECT_FALSE(Token::isLitCharEscape('b'));
  EXPECT_FALSE(Token::isLitCharEscape('1'));
}

TEST(TokenTest, IsLitIntBin) {
  EXPECT_TRUE(Token::isLitIntBin('0'));
  EXPECT_TRUE(Token::isLitIntBin('1'));
}

TEST(TokenTest, IsNotLitIntBin) {
  EXPECT_FALSE(Token::isLitIntBin('2'));
  EXPECT_FALSE(Token::isLitIntBin('a'));
}

TEST(TokenTest, IsLitIntDec) {
  EXPECT_TRUE(Token::isLitIntDec('0'));
  EXPECT_TRUE(Token::isLitIntDec('9'));
}

TEST(TokenTest, IsNotLitIntDec) {
  EXPECT_FALSE(Token::isLitIntDec('a'));
  EXPECT_FALSE(Token::isLitIntDec('#'));
}

TEST(TokenTest, IsLitIntHex) {
  EXPECT_TRUE(Token::isLitIntHex('0'));
  EXPECT_TRUE(Token::isLitIntHex('9'));
  EXPECT_TRUE(Token::isLitIntHex('a'));
  EXPECT_TRUE(Token::isLitIntHex('F'));
}

TEST(TokenTest, IsNotLitIntHex) {
  EXPECT_FALSE(Token::isLitIntHex('g'));
  EXPECT_FALSE(Token::isLitIntHex('Z'));
  EXPECT_FALSE(Token::isLitIntHex('#'));
}

TEST(TokenTest, IsLitIntOct) {
  EXPECT_TRUE(Token::isLitIntOct('0'));
  EXPECT_TRUE(Token::isLitIntOct('7'));
}

TEST(TokenTest, IsNotLitIntOct) {
  EXPECT_FALSE(Token::isLitIntOct('8'));
  EXPECT_FALSE(Token::isLitIntOct('9'));
  EXPECT_FALSE(Token::isLitIntOct('a'));
  EXPECT_FALSE(Token::isLitIntOct('Z'));
  EXPECT_FALSE(Token::isLitIntOct('#'));
}

TEST(TokenTest, IsLitStrEscape) {
  EXPECT_FALSE(Token::isLitStrEscape('{'));
  EXPECT_TRUE(Token::isLitStrEscape('n'));
}

TEST(TokenTest, IsNotLitStrEscape) {
  EXPECT_FALSE(Token::isLitStrEscape('}'));
  EXPECT_FALSE(Token::isLitStrEscape(' '));
}

TEST(TokenTest, IsWhitespace) {
  EXPECT_TRUE(Token::isWhitespace(' '));
  EXPECT_TRUE(Token::isWhitespace('\n'));
  EXPECT_TRUE(Token::isWhitespace('\t'));
}

TEST(TokenTest, IsNotWhitespace) {
  EXPECT_FALSE(Token::isWhitespace('0'));
  EXPECT_FALSE(Token::isWhitespace('a'));
  EXPECT_FALSE(Token::isWhitespace('#'));
}

TEST(TokenTest, ThrowsOnUnknownPrecedenceToken) {
  EXPECT_THROW_WITH_MESSAGE(Token{TK_EOF}.precedence(), "Error: tried precedence for unknown token");
}

TEST(TokenTest, ParenthesesPrecision) {
  EXPECT_EQ(Token{TK_OP_LPAR}.precedence(), 18);
  EXPECT_EQ(Token{TK_OP_RPAR}.precedence(), 18);
}

TEST(TokenTest, BracketsPrecision) {
  EXPECT_EQ(Token{TK_OP_LBRACK}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_RBRACK}.precedence(), 17);
}

TEST(TokenTest, SymbolsPrecision) {
  EXPECT_EQ(Token{TK_OP_DOT}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_DOT_DOT_DOT}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_QN_DOT}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_COLON}.precedence(), 3);
  EXPECT_EQ(Token{TK_OP_QN}.precedence(), 3);
  EXPECT_EQ(Token{TK_OP_COMMA}.precedence(), 1);
}

TEST(TokenTest, UnaryPrecision) {
  EXPECT_EQ(Token{TK_OP_MINUS_MINUS}.precedence(), 16);
  EXPECT_EQ(Token{TK_OP_PLUS_PLUS}.precedence(), 16);
  EXPECT_EQ(Token{TK_OP_EXCL}.precedence(), 15);
  EXPECT_EQ(Token{TK_OP_TILDE}.precedence(), 15);
}

TEST(TokenTest, BinaryPrecision) {
  EXPECT_EQ(Token{TK_OP_STAR_STAR}.precedence(), 14);
  EXPECT_EQ(Token{TK_OP_PERCENT}.precedence(), 13);
  EXPECT_EQ(Token{TK_OP_SLASH}.precedence(), 13);
  EXPECT_EQ(Token{TK_OP_STAR}.precedence(), 13);
  EXPECT_EQ(Token{TK_OP_MINUS}.precedence(), 12);
  EXPECT_EQ(Token{TK_OP_PLUS}.precedence(), 12);
  EXPECT_EQ(Token{TK_OP_LSHIFT}.precedence(), 11);
  EXPECT_EQ(Token{TK_OP_RSHIFT}.precedence(), 11);
  EXPECT_EQ(Token{TK_OP_GT}.precedence(), 10);
  EXPECT_EQ(Token{TK_OP_GT_EQ}.precedence(), 10);
  EXPECT_EQ(Token{TK_OP_LT}.precedence(), 10);
  EXPECT_EQ(Token{TK_OP_LT_EQ}.precedence(), 10);
  EXPECT_EQ(Token{TK_OP_AND}.precedence(), 8);
  EXPECT_EQ(Token{TK_OP_CARET}.precedence(), 7);
  EXPECT_EQ(Token{TK_OP_OR}.precedence(), 6);
  EXPECT_EQ(Token{TK_OP_AND_AND}.precedence(), 5);
  EXPECT_EQ(Token{TK_OP_OR_OR}.precedence(), 4);
  EXPECT_EQ(Token{TK_OP_QN_QN}.precedence(), 4);
}

TEST(TokenTest, AssignmentPrecision) {
  EXPECT_EQ(Token{TK_OP_EQ_EQ}.precedence(), 9);
  EXPECT_EQ(Token{TK_OP_EXCL_EQ}.precedence(), 9);
  EXPECT_EQ(Token{TK_OP_AND_AND_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_AND_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_CARET_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_LSHIFT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_MINUS_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_OR_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_OR_OR_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PERCENT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PLUS_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_QN_QN_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_RSHIFT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_SLASH_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_STAR_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_STAR_STAR_EQ}.precedence(), 2);
}
