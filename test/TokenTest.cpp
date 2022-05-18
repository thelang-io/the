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

TEST(TokenTest, AssociativityThrowsOnUnknown) {
  EXPECT_THROW_WITH_MESSAGE({
    Token{TK_EOF}.associativity();
  }, "Error: tried associativity for unknown token");
}

TEST(TokenTest, AssociativityNone) {
  EXPECT_EQ(Token{TK_OP_LPAR}.associativity(), TK_ASSOC_NONE);
  EXPECT_EQ(Token{TK_OP_RPAR}.associativity(), TK_ASSOC_NONE);
  EXPECT_EQ(Token{TK_OP_MINUS_MINUS}.associativity(true), TK_ASSOC_NONE);
  EXPECT_EQ(Token{TK_OP_PLUS_PLUS}.associativity(true), TK_ASSOC_NONE);
}

TEST(TokenTest, AssociativityLeft) {
  EXPECT_EQ(Token{TK_OP_DOT}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_LBRACK}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_RBRACK}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_PERCENT}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_SLASH}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_STAR}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_MINUS}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_PLUS}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_LSHIFT}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_RSHIFT}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_GT}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_GT_EQ}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_LT}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_LT_EQ}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_EQ_EQ}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_EXCL_EQ}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_CARET}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_COMMA}.associativity(), TK_ASSOC_LEFT);
}

TEST(TokenTest, AssociativityRight) {
  EXPECT_EQ(Token{TK_OP_EXCL}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_MINUS}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PLUS}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_TILDE}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_CARET_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_LSHIFT_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_MINUS_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PERCENT_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PLUS_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_RSHIFT_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_SLASH_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_STAR_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_COLON}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_QN}.associativity(), TK_ASSOC_RIGHT);
}

TEST(TokenTest, PrecedenceThrowsOnUnknown) {
  EXPECT_THROW_WITH_MESSAGE({
    Token{TK_EOF}.precedence();
  }, "Error: tried precedence for unknown token");
}

TEST(TokenTest, PrecedenceParentheses) {
  EXPECT_EQ(Token{TK_OP_LPAR}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_RPAR}.precedence(), 17);
}

TEST(TokenTest, PrecedenceBrackets) {
  EXPECT_EQ(Token{TK_OP_LBRACK}.precedence(), 16);
  EXPECT_EQ(Token{TK_OP_RBRACK}.precedence(), 16);
}

TEST(TokenTest, PrecedenceSymbols) {
  EXPECT_EQ(Token{TK_OP_DOT}.precedence(), 16);
  EXPECT_EQ(Token{TK_OP_COLON}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_QN}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_COMMA}.precedence(), 1);
}

TEST(TokenTest, PrecedenceUnary) {
  EXPECT_EQ(Token{TK_OP_MINUS}.precedence(true), 14);
  EXPECT_EQ(Token{TK_OP_PLUS}.precedence(true), 14);
  EXPECT_EQ(Token{TK_OP_MINUS_MINUS}.precedence(true), 15);
  EXPECT_EQ(Token{TK_OP_PLUS_PLUS}.precedence(true), 15);
  EXPECT_EQ(Token{TK_OP_EXCL}.precedence(true), 14);
  EXPECT_EQ(Token{TK_OP_TILDE}.precedence(true), 14);
}

TEST(TokenTest, PrecedenceBinary) {
  EXPECT_EQ(Token{TK_OP_PERCENT}.precedence(), 12);
  EXPECT_EQ(Token{TK_OP_SLASH}.precedence(), 12);
  EXPECT_EQ(Token{TK_OP_STAR}.precedence(), 12);
  EXPECT_EQ(Token{TK_OP_MINUS}.precedence(), 11);
  EXPECT_EQ(Token{TK_OP_PLUS}.precedence(), 11);
  EXPECT_EQ(Token{TK_OP_LSHIFT}.precedence(), 10);
  EXPECT_EQ(Token{TK_OP_RSHIFT}.precedence(), 10);
  EXPECT_EQ(Token{TK_OP_GT}.precedence(), 9);
  EXPECT_EQ(Token{TK_OP_GT_EQ}.precedence(), 9);
  EXPECT_EQ(Token{TK_OP_LT}.precedence(), 9);
  EXPECT_EQ(Token{TK_OP_LT_EQ}.precedence(), 9);
  EXPECT_EQ(Token{TK_OP_CARET}.precedence(), 6);
}

TEST(TokenTest, PrecedenceAssignment) {
  EXPECT_EQ(Token{TK_OP_EQ_EQ}.precedence(), 8);
  EXPECT_EQ(Token{TK_OP_EXCL_EQ}.precedence(), 8);
  EXPECT_EQ(Token{TK_OP_CARET_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_LSHIFT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_MINUS_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PERCENT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PLUS_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_RSHIFT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_SLASH_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_STAR_EQ}.precedence(), 2);
}
