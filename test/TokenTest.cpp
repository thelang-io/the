/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "../src/Token.hpp"
#include "utils.hpp"

TEST(TokenTest, Escapes) {
  EXPECT_EQ(Token::escape(""), "");
  EXPECT_EQ(Token::escape("hello"), "hello");
  EXPECT_EQ(Token::escape(R"(")" "prefix \f\n\r\t\v postfix" R"(")"), R"(")" "prefix \\f\\n\\r\\t\\v postfix" R"(")");
  EXPECT_EQ(Token::escape(R"(")" "prefix \\f\\n\\r\\t\\v postfix" R"(")"), R"(")" "prefix \\f\\n\\r\\t\\v postfix" R"(")");
}

TEST(TokenTest, EscapesInsideAttributes) {
  EXPECT_EQ(Token::escape("", true), "");
  EXPECT_EQ(Token::escape("hello", true), "hello");
  EXPECT_EQ(Token::escape(R"(")" "prefix \f\n\r\t\v postfix" R"(")", true), R"(\")" "prefix \\f\\n\\r\\t\\v postfix" R"(\")");
  EXPECT_EQ(Token::escape(R"(")" "prefix \\f\\n\\r\\t\\v postfix" R"(")", true), R"(\")" "prefix \\f\\n\\r\\t\\v postfix" R"(\")");
}

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

TEST(TokenTest, Upper) {
  EXPECT_EQ(Token::upper(""), "");
  EXPECT_EQ(Token::upper(" test"), " TEST");
  EXPECT_EQ(Token::upper("a"), "A");
  EXPECT_EQ(Token::upper("1"), "1");
  EXPECT_EQ(Token::upper("test"), "TEST");
  EXPECT_EQ(Token::upper("test string"), "TEST STRING");
  EXPECT_EQ(Token::upper("testTest"), "TESTTEST");
}

TEST(TokenTest, UpperFirst) {
  EXPECT_EQ(Token::upperFirst(""), "");
  EXPECT_EQ(Token::upperFirst(" test"), " test");
  EXPECT_EQ(Token::upperFirst("a"), "A");
  EXPECT_EQ(Token::upperFirst("1"), "1");
  EXPECT_EQ(Token::upperFirst("test"), "Test");
  EXPECT_EQ(Token::upperFirst("test string"), "Test string");
  EXPECT_EQ(Token::upperFirst("testTest"), "TestTest");
}

TEST(TokenTest, AssociativityThrowsOnUnknown) {
  EXPECT_THROW_WITH_MESSAGE({
    Token{TK_EOF}.associativity();
  }, "tried associativity for unknown token");
}

TEST(TokenTest, AssociativityNone) {
  EXPECT_EQ(Token{TK_KW_REF}.associativity(), TK_ASSOC_NONE);
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
  EXPECT_EQ(Token{TK_OP_AMP}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_CARET}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_PIPE}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_AMP_AMP}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_PIPE_PIPE}.associativity(), TK_ASSOC_LEFT);
  EXPECT_EQ(Token{TK_OP_COMMA}.associativity(), TK_ASSOC_LEFT);
}

TEST(TokenTest, AssociativityRight) {
  EXPECT_EQ(Token{TK_OP_EXCL}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_MINUS}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PLUS}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_TILDE}.associativity(true), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_AMP_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_AMP_AMP_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_CARET_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_LSHIFT_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_MINUS_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PERCENT_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PIPE_EQ}.associativity(), TK_ASSOC_RIGHT);
  EXPECT_EQ(Token{TK_OP_PIPE_PIPE_EQ}.associativity(), TK_ASSOC_RIGHT);
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
  }, "tried precedence for unknown token");
}

TEST(TokenTest, PrecedenceParentheses) {
  EXPECT_EQ(Token{TK_OP_LPAR}.precedence(), 18);
  EXPECT_EQ(Token{TK_OP_RPAR}.precedence(), 18);
}

TEST(TokenTest, PrecedenceBrackets) {
  EXPECT_EQ(Token{TK_OP_LBRACK}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_RBRACK}.precedence(), 17);
}

TEST(TokenTest, PrecedenceSymbols) {
  EXPECT_EQ(Token{TK_OP_DOT}.precedence(), 17);
  EXPECT_EQ(Token{TK_OP_COLON}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_QN}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_COMMA}.precedence(), 1);
}

TEST(TokenTest, PrecedenceKeywords) {
  EXPECT_EQ(Token{TK_KW_REF}.precedence(), 15);
}

TEST(TokenTest, PrecedenceUnary) {
  EXPECT_EQ(Token{TK_OP_MINUS}.precedence(true), 14);
  EXPECT_EQ(Token{TK_OP_PLUS}.precedence(true), 14);
  EXPECT_EQ(Token{TK_OP_MINUS_MINUS}.precedence(true), 16);
  EXPECT_EQ(Token{TK_OP_PLUS_PLUS}.precedence(true), 16);
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
  EXPECT_EQ(Token{TK_OP_AMP}.precedence(), 7);
  EXPECT_EQ(Token{TK_OP_PIPE}.precedence(), 5);
  EXPECT_EQ(Token{TK_OP_AMP_AMP}.precedence(), 4);
  EXPECT_EQ(Token{TK_OP_PIPE_PIPE}.precedence(), 3);
}

TEST(TokenTest, PrecedenceAssignment) {
  EXPECT_EQ(Token{TK_OP_EQ_EQ}.precedence(), 8);
  EXPECT_EQ(Token{TK_OP_EXCL_EQ}.precedence(), 8);
  EXPECT_EQ(Token{TK_OP_AMP_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_AMP_AMP_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_CARET_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_LSHIFT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_MINUS_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PERCENT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PIPE_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PIPE_PIPE_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_PLUS_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_RSHIFT_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_SLASH_EQ}.precedence(), 2);
  EXPECT_EQ(Token{TK_OP_STAR_EQ}.precedence(), 2);
}
