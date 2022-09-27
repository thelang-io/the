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
#include "../src/Lexer.hpp"
#include "../src/config.hpp"
#include "MockReader.hpp"
#include "utils.hpp"

TEST(LexerTest, LexUnknown) {
  auto reader = testing::NiceMock<MockReader>("∆");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "UNKNOWN(1:1-1:2): \xE2");
}

TEST(LexerTest, LexEof) {
  auto reader = testing::NiceMock<MockReader>("");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:1-1:1)");
}

TEST(LexerTest, LexSingleWhitespace) {
  auto reader = testing::NiceMock<MockReader>(" ");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:2-1:2)");
}

TEST(LexerTest, LexMultipleWhitespaces) {
  auto reader = testing::NiceMock<MockReader>(" " EOL "\t");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(2:2-2:2)");
}

TEST(LexerTest, LexEmptyBlockComment) {
  auto reader = testing::NiceMock<MockReader>("/**/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:5-1:5)");
}

TEST(LexerTest, LexAsteriskBlockComment) {
  auto reader = testing::NiceMock<MockReader>("/***/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:6-1:6)");
}

TEST(LexerTest, LexSingleBlockComment) {
  auto reader = testing::NiceMock<MockReader>("/*Hello Kendall*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:18-1:18)");
}

TEST(LexerTest, LexMultipleBlockComments) {
  auto reader = testing::NiceMock<MockReader>("/*Hello Kim*//*Hello Dream*//*Hello Stormi*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:45-1:45)");
}

TEST(LexerTest, LexMultilineBlockComment) {
  auto reader = testing::NiceMock<MockReader>("/*Hello Bella" EOL "Hello Olivia" EOL "Hello Rosa*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(3:13-3:13)");
}

TEST(LexerTest, ThrowsOnEmptyNotClosedBlockComment) {
  auto reader = testing::NiceMock<MockReader>("/*");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE(lexer.next(), std::string("/test:1:1: ") + E0001 + EOL "  1 | /*" EOL "    | ^~");
}

TEST(LexerTest, ThrowsOnNotClosedBlockComment) {
  auto reader = testing::NiceMock<MockReader>("/*Hello");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE(lexer.next(), std::string("/test:1:1: ") + E0001 + EOL "  1 | /*Hello" EOL "    | ^~~~~~~");
}

TEST(LexerTest, LexEmptyLineCommentNoNewLine) {
  auto reader = testing::NiceMock<MockReader>("//");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:3-1:3)");
}

TEST(LexerTest, LexEmptyLineComment) {
  auto reader = testing::NiceMock<MockReader>("//" EOL);
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(2:1-2:1)");
}

TEST(LexerTest, LexLineCommentNoNewLine) {
  auto reader = testing::NiceMock<MockReader>("//Hello Ariana");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:15-1:15)");
}

TEST(LexerTest, LexLineComment) {
  auto reader = testing::NiceMock<MockReader>("//Hello Hailey" EOL);
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(2:1-2:1)");
}

TEST(LexerTest, LexWhitespacesCombinedWithComments) {
  auto reader = testing::NiceMock<MockReader>(" \t\r//Hello Mary" EOL "/*Hello Linda*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(2:16-2:16)");
}

TEST(LexerTest, LexWhitespaceBeforeToken) {
  auto reader = testing::NiceMock<MockReader>(" \t;");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "OP_SEMI(1:3-1:4): ;");
}

TEST(LexerTest, LexWhitespaceAfterToken) {
  auto reader = testing::NiceMock<MockReader>("; \t");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "OP_SEMI(1:1-1:2): ;");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:4-1:4)");
}

TEST(LexerTest, LexBlockCommentBeforeToken) {
  auto reader = testing::NiceMock<MockReader>("/*Hello Anna*/,");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "OP_COMMA(1:15-1:16): ,");
}

TEST(LexerTest, LexBlockCommentAfterToken) {
  auto reader = testing::NiceMock<MockReader>(",/*Hello Anna*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "OP_COMMA(1:1-1:2): ,");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:16-1:16)");
}

TEST(LexerTest, LexLineCommentBeforeToken) {
  auto reader = testing::NiceMock<MockReader>("//Hello Kylie" EOL "+");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "OP_PLUS(2:1-2:2): +");
}

TEST(LexerTest, LexLineCommentAfterToken) {
  auto reader = testing::NiceMock<MockReader>("+//Hello Kylie" EOL);
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "OP_PLUS(1:1-1:2): +");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(2:1-2:1)");
}

TEST(LexerTest, LexOperations) {
  auto r1 = testing::NiceMock<MockReader>("&@\\`^:,$.=!>#{[(<-|%+?}]);/~*");
  auto r2 = testing::NiceMock<MockReader>("&&<<--||++>>");
  auto r3 = testing::NiceMock<MockReader>("&=&&=^=:=...==!=>=<<=<=-=|=||=%=+=>>=/=*=");
  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_AMP(1:1-1:2): &");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_AT(1:2-1:3): @");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_BACKSLASH(1:3-1:4): \\");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_BACKTICK(1:4-1:5): `");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_CARET(1:5-1:6): ^");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_COLON(1:6-1:7): :");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_COMMA(1:7-1:8): ,");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_DOLLAR(1:8-1:9): $");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_DOT(1:9-1:10): .");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_EQ(1:10-1:11): =");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_EXCL(1:11-1:12): !");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_GT(1:12-1:13): >");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_HASH(1:13-1:14): #");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LBRACE(1:14-1:15): {");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LBRACK(1:15-1:16): [");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LPAR(1:16-1:17): (");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LT(1:17-1:18): <");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_MINUS(1:18-1:19): -");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_PIPE(1:19-1:20): |");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_PERCENT(1:20-1:21): %");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_PLUS(1:21-1:22): +");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_QN(1:22-1:23): ?");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_RBRACE(1:23-1:24): }");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_RBRACK(1:24-1:25): ]");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_RPAR(1:25-1:26): )");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_SEMI(1:26-1:27): ;");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_SLASH(1:27-1:28): /");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_TILDE(1:28-1:29): ~");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_STAR(1:29-1:30): *");

  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_AMP_AMP(1:1-1:3): &&");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_LSHIFT(1:3-1:5): <<");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_MINUS_MINUS(1:5-1:7): --");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_PIPE_PIPE(1:7-1:9): ||");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_PLUS_PLUS(1:9-1:11): ++");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_RSHIFT(1:11-1:13): >>");

  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_AMP_EQ(1:1-1:3): &=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_AMP_AMP_EQ(1:3-1:6): &&=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_CARET_EQ(1:6-1:8): ^=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_COLON_EQ(1:8-1:10): :=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_DOT_DOT_DOT(1:10-1:13): ...");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_EQ_EQ(1:13-1:15): ==");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_EXCL_EQ(1:15-1:17): !=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_GT_EQ(1:17-1:19): >=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_LSHIFT_EQ(1:19-1:22): <<=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_LT_EQ(1:22-1:24): <=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_MINUS_EQ(1:24-1:26): -=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PIPE_EQ(1:26-1:28): |=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PIPE_PIPE_EQ(1:28-1:31): ||=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PERCENT_EQ(1:31-1:33): %=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PLUS_EQ(1:33-1:35): +=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_RSHIFT_EQ(1:35-1:38): >>=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_SLASH_EQ(1:38-1:40): /=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_STAR_EQ(1:40-1:42): *=");
}

TEST(LexerTest, LexOperationsWhitespace) {
  auto r1 = testing::NiceMock<MockReader>(" & @ \\ ` ^ : , $ . = ! > # { [ ( < - | % + ? } ] ) ; / * ~ ");
  auto r2 = testing::NiceMock<MockReader>(" && << -- || ++ >> ");
  auto r3 = testing::NiceMock<MockReader>(" &= &&= ^= := ... == != >= <<= <= -= |= ||= %= += >>= /= *= ");
  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_AMP(1:2-1:3): &");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_AT(1:4-1:5): @");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_BACKSLASH(1:6-1:7): \\");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_BACKTICK(1:8-1:9): `");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_CARET(1:10-1:11): ^");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_COLON(1:12-1:13): :");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_COMMA(1:14-1:15): ,");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_DOLLAR(1:16-1:17): $");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_DOT(1:18-1:19): .");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_EQ(1:20-1:21): =");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_EXCL(1:22-1:23): !");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_GT(1:24-1:25): >");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_HASH(1:26-1:27): #");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LBRACE(1:28-1:29): {");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LBRACK(1:30-1:31): [");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LPAR(1:32-1:33): (");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_LT(1:34-1:35): <");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_MINUS(1:36-1:37): -");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_PIPE(1:38-1:39): |");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_PERCENT(1:40-1:41): %");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_PLUS(1:42-1:43): +");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_QN(1:44-1:45): ?");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_RBRACE(1:46-1:47): }");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_RBRACK(1:48-1:49): ]");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_RPAR(1:50-1:51): )");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_SEMI(1:52-1:53): ;");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_SLASH(1:54-1:55): /");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_STAR(1:56-1:57): *");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_TILDE(1:58-1:59): ~");
  EXPECT_EQ(std::get<1>(l1.next()).str(), "EOF(1:60-1:60)");

  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_AMP_AMP(1:2-1:4): &&");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_LSHIFT(1:5-1:7): <<");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_MINUS_MINUS(1:8-1:10): --");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_PIPE_PIPE(1:11-1:13): ||");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_PLUS_PLUS(1:14-1:16): ++");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_RSHIFT(1:17-1:19): >>");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "EOF(1:20-1:20)");

  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_AMP_EQ(1:2-1:4): &=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_AMP_AMP_EQ(1:5-1:8): &&=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_CARET_EQ(1:9-1:11): ^=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_COLON_EQ(1:12-1:14): :=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_DOT_DOT_DOT(1:15-1:18): ...");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_EQ_EQ(1:19-1:21): ==");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_EXCL_EQ(1:22-1:24): !=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_GT_EQ(1:25-1:27): >=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_LSHIFT_EQ(1:28-1:31): <<=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_LT_EQ(1:32-1:34): <=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_MINUS_EQ(1:35-1:37): -=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PIPE_EQ(1:38-1:40): |=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PIPE_PIPE_EQ(1:41-1:44): ||=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PERCENT_EQ(1:45-1:47): %=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_PLUS_EQ(1:48-1:50): +=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_RSHIFT_EQ(1:51-1:54): >>=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_SLASH_EQ(1:55-1:57): /=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_STAR_EQ(1:58-1:60): *=");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "EOF(1:61-1:61)");
}

TEST(LexerTest, LexOperationsEof) {
  auto r1 = testing::NiceMock<MockReader>("&");
  auto r2 = testing::NiceMock<MockReader>("@");
  auto r3 = testing::NiceMock<MockReader>("\\");
  auto r4 = testing::NiceMock<MockReader>("`");
  auto r5 = testing::NiceMock<MockReader>("^");
  auto r6 = testing::NiceMock<MockReader>(":");
  auto r7 = testing::NiceMock<MockReader>(",");
  auto r8 = testing::NiceMock<MockReader>("$");
  auto r9 = testing::NiceMock<MockReader>(".");
  auto r10 = testing::NiceMock<MockReader>("=");
  auto r11 = testing::NiceMock<MockReader>("!");
  auto r12 = testing::NiceMock<MockReader>(">");
  auto r13 = testing::NiceMock<MockReader>("#");
  auto r14 = testing::NiceMock<MockReader>("{");
  auto r15 = testing::NiceMock<MockReader>("[");
  auto r16 = testing::NiceMock<MockReader>("(");
  auto r17 = testing::NiceMock<MockReader>("<");
  auto r18 = testing::NiceMock<MockReader>("-");
  auto r19 = testing::NiceMock<MockReader>("|");
  auto r20 = testing::NiceMock<MockReader>("%");
  auto r21 = testing::NiceMock<MockReader>("+");
  auto r22 = testing::NiceMock<MockReader>("?");
  auto r23 = testing::NiceMock<MockReader>("}");
  auto r24 = testing::NiceMock<MockReader>("]");
  auto r25 = testing::NiceMock<MockReader>(")");
  auto r26 = testing::NiceMock<MockReader>(";");
  auto r27 = testing::NiceMock<MockReader>("/");
  auto r28 = testing::NiceMock<MockReader>("*");
  auto r29 = testing::NiceMock<MockReader>("~");
  auto r30 = testing::NiceMock<MockReader>("&&");
  auto r31 = testing::NiceMock<MockReader>("<<");
  auto r32 = testing::NiceMock<MockReader>("--");
  auto r33 = testing::NiceMock<MockReader>("||");
  auto r34 = testing::NiceMock<MockReader>("++");
  auto r35 = testing::NiceMock<MockReader>(">>");
  auto r36 = testing::NiceMock<MockReader>("&=");
  auto r37 = testing::NiceMock<MockReader>("&&=");
  auto r38 = testing::NiceMock<MockReader>("^=");
  auto r39 = testing::NiceMock<MockReader>(":=");
  auto r40 = testing::NiceMock<MockReader>("...");
  auto r41 = testing::NiceMock<MockReader>("==");
  auto r42 = testing::NiceMock<MockReader>("!=");
  auto r43 = testing::NiceMock<MockReader>(">=");
  auto r44 = testing::NiceMock<MockReader>("<<=");
  auto r45 = testing::NiceMock<MockReader>("<=");
  auto r46 = testing::NiceMock<MockReader>("-=");
  auto r47 = testing::NiceMock<MockReader>("|=");
  auto r48 = testing::NiceMock<MockReader>("||=");
  auto r49 = testing::NiceMock<MockReader>("%=");
  auto r50 = testing::NiceMock<MockReader>("+=");
  auto r51 = testing::NiceMock<MockReader>(">>=");
  auto r52 = testing::NiceMock<MockReader>("/=");
  auto r53 = testing::NiceMock<MockReader>("*=");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l13 = Lexer(&r13);
  auto l14 = Lexer(&r14);
  auto l15 = Lexer(&r15);
  auto l16 = Lexer(&r16);
  auto l17 = Lexer(&r17);
  auto l18 = Lexer(&r18);
  auto l19 = Lexer(&r19);
  auto l20 = Lexer(&r20);
  auto l21 = Lexer(&r21);
  auto l22 = Lexer(&r22);
  auto l23 = Lexer(&r23);
  auto l24 = Lexer(&r24);
  auto l25 = Lexer(&r25);
  auto l26 = Lexer(&r26);
  auto l27 = Lexer(&r27);
  auto l28 = Lexer(&r28);
  auto l29 = Lexer(&r29);
  auto l30 = Lexer(&r30);
  auto l31 = Lexer(&r31);
  auto l32 = Lexer(&r32);
  auto l33 = Lexer(&r33);
  auto l34 = Lexer(&r34);
  auto l35 = Lexer(&r35);
  auto l36 = Lexer(&r36);
  auto l37 = Lexer(&r37);
  auto l38 = Lexer(&r38);
  auto l39 = Lexer(&r39);
  auto l40 = Lexer(&r40);
  auto l41 = Lexer(&r41);
  auto l42 = Lexer(&r42);
  auto l43 = Lexer(&r43);
  auto l44 = Lexer(&r44);
  auto l45 = Lexer(&r45);
  auto l46 = Lexer(&r46);
  auto l47 = Lexer(&r47);
  auto l48 = Lexer(&r48);
  auto l49 = Lexer(&r49);
  auto l50 = Lexer(&r50);
  auto l51 = Lexer(&r51);
  auto l52 = Lexer(&r52);
  auto l53 = Lexer(&r53);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "OP_AMP(1:1-1:2): &");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "OP_AT(1:1-1:2): @");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "OP_BACKSLASH(1:1-1:2): \\");
  EXPECT_EQ(std::get<1>(l4.next()).str(), "OP_BACKTICK(1:1-1:2): `");
  EXPECT_EQ(std::get<1>(l5.next()).str(), "OP_CARET(1:1-1:2): ^");
  EXPECT_EQ(std::get<1>(l6.next()).str(), "OP_COLON(1:1-1:2): :");
  EXPECT_EQ(std::get<1>(l7.next()).str(), "OP_COMMA(1:1-1:2): ,");
  EXPECT_EQ(std::get<1>(l8.next()).str(), "OP_DOLLAR(1:1-1:2): $");
  EXPECT_EQ(std::get<1>(l9.next()).str(), "OP_DOT(1:1-1:2): .");
  EXPECT_EQ(std::get<1>(l10.next()).str(), "OP_EQ(1:1-1:2): =");
  EXPECT_EQ(std::get<1>(l11.next()).str(), "OP_EXCL(1:1-1:2): !");
  EXPECT_EQ(std::get<1>(l12.next()).str(), "OP_GT(1:1-1:2): >");
  EXPECT_EQ(std::get<1>(l13.next()).str(), "OP_HASH(1:1-1:2): #");
  EXPECT_EQ(std::get<1>(l14.next()).str(), "OP_LBRACE(1:1-1:2): {");
  EXPECT_EQ(std::get<1>(l15.next()).str(), "OP_LBRACK(1:1-1:2): [");
  EXPECT_EQ(std::get<1>(l16.next()).str(), "OP_LPAR(1:1-1:2): (");
  EXPECT_EQ(std::get<1>(l17.next()).str(), "OP_LT(1:1-1:2): <");
  EXPECT_EQ(std::get<1>(l18.next()).str(), "OP_MINUS(1:1-1:2): -");
  EXPECT_EQ(std::get<1>(l19.next()).str(), "OP_PIPE(1:1-1:2): |");
  EXPECT_EQ(std::get<1>(l20.next()).str(), "OP_PERCENT(1:1-1:2): %");
  EXPECT_EQ(std::get<1>(l21.next()).str(), "OP_PLUS(1:1-1:2): +");
  EXPECT_EQ(std::get<1>(l22.next()).str(), "OP_QN(1:1-1:2): ?");
  EXPECT_EQ(std::get<1>(l23.next()).str(), "OP_RBRACE(1:1-1:2): }");
  EXPECT_EQ(std::get<1>(l24.next()).str(), "OP_RBRACK(1:1-1:2): ]");
  EXPECT_EQ(std::get<1>(l25.next()).str(), "OP_RPAR(1:1-1:2): )");
  EXPECT_EQ(std::get<1>(l26.next()).str(), "OP_SEMI(1:1-1:2): ;");
  EXPECT_EQ(std::get<1>(l27.next()).str(), "OP_SLASH(1:1-1:2): /");
  EXPECT_EQ(std::get<1>(l28.next()).str(), "OP_STAR(1:1-1:2): *");
  EXPECT_EQ(std::get<1>(l29.next()).str(), "OP_TILDE(1:1-1:2): ~");
  EXPECT_EQ(std::get<1>(l30.next()).str(), "OP_AMP_AMP(1:1-1:3): &&");
  EXPECT_EQ(std::get<1>(l31.next()).str(), "OP_LSHIFT(1:1-1:3): <<");
  EXPECT_EQ(std::get<1>(l32.next()).str(), "OP_MINUS_MINUS(1:1-1:3): --");
  EXPECT_EQ(std::get<1>(l33.next()).str(), "OP_PIPE_PIPE(1:1-1:3): ||");
  EXPECT_EQ(std::get<1>(l34.next()).str(), "OP_PLUS_PLUS(1:1-1:3): ++");
  EXPECT_EQ(std::get<1>(l35.next()).str(), "OP_RSHIFT(1:1-1:3): >>");
  EXPECT_EQ(std::get<1>(l36.next()).str(), "OP_AMP_EQ(1:1-1:3): &=");
  EXPECT_EQ(std::get<1>(l37.next()).str(), "OP_AMP_AMP_EQ(1:1-1:4): &&=");
  EXPECT_EQ(std::get<1>(l38.next()).str(), "OP_CARET_EQ(1:1-1:3): ^=");
  EXPECT_EQ(std::get<1>(l39.next()).str(), "OP_COLON_EQ(1:1-1:3): :=");
  EXPECT_EQ(std::get<1>(l40.next()).str(), "OP_DOT_DOT_DOT(1:1-1:4): ...");
  EXPECT_EQ(std::get<1>(l41.next()).str(), "OP_EQ_EQ(1:1-1:3): ==");
  EXPECT_EQ(std::get<1>(l42.next()).str(), "OP_EXCL_EQ(1:1-1:3): !=");
  EXPECT_EQ(std::get<1>(l43.next()).str(), "OP_GT_EQ(1:1-1:3): >=");
  EXPECT_EQ(std::get<1>(l44.next()).str(), "OP_LSHIFT_EQ(1:1-1:4): <<=");
  EXPECT_EQ(std::get<1>(l45.next()).str(), "OP_LT_EQ(1:1-1:3): <=");
  EXPECT_EQ(std::get<1>(l46.next()).str(), "OP_MINUS_EQ(1:1-1:3): -=");
  EXPECT_EQ(std::get<1>(l47.next()).str(), "OP_PIPE_EQ(1:1-1:3): |=");
  EXPECT_EQ(std::get<1>(l48.next()).str(), "OP_PIPE_PIPE_EQ(1:1-1:4): ||=");
  EXPECT_EQ(std::get<1>(l49.next()).str(), "OP_PERCENT_EQ(1:1-1:3): %=");
  EXPECT_EQ(std::get<1>(l50.next()).str(), "OP_PLUS_EQ(1:1-1:3): +=");
  EXPECT_EQ(std::get<1>(l51.next()).str(), "OP_RSHIFT_EQ(1:1-1:4): >>=");
  EXPECT_EQ(std::get<1>(l52.next()).str(), "OP_SLASH_EQ(1:1-1:3): /=");
  EXPECT_EQ(std::get<1>(l53.next()).str(), "OP_STAR_EQ(1:1-1:3): *=");
}

TEST(LexerTest, LexKeywords) {
  auto r4 = testing::NiceMock<MockReader>("break");
  auto r6 = testing::NiceMock<MockReader>("catch");
  auto r8 = testing::NiceMock<MockReader>("continue");
  auto r10 = testing::NiceMock<MockReader>("elif");
  auto r11 = testing::NiceMock<MockReader>("else");
  auto r12 = testing::NiceMock<MockReader>("enum");
  auto r15 = testing::NiceMock<MockReader>("false");
  auto r16 = testing::NiceMock<MockReader>("fn");
  auto r18 = testing::NiceMock<MockReader>("if");
  auto r20 = testing::NiceMock<MockReader>("is");
  auto r21 = testing::NiceMock<MockReader>("loop");
  auto r22 = testing::NiceMock<MockReader>("main");
  auto r24 = testing::NiceMock<MockReader>("mut");
  auto r25 = testing::NiceMock<MockReader>("nil");
  auto r26 = testing::NiceMock<MockReader>("obj");
  auto r27 = testing::NiceMock<MockReader>("ref");
  auto r28 = testing::NiceMock<MockReader>("return");
  auto r29 = testing::NiceMock<MockReader>("throw");
  auto r30 = testing::NiceMock<MockReader>("true");
  auto r31 = testing::NiceMock<MockReader>("try");
  auto r32 = testing::NiceMock<MockReader>("union");

  auto l4 = Lexer(&r4);
  auto l6 = Lexer(&r6);
  auto l8 = Lexer(&r8);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l15 = Lexer(&r15);
  auto l16 = Lexer(&r16);
  auto l18 = Lexer(&r18);
  auto l20 = Lexer(&r20);
  auto l21 = Lexer(&r21);
  auto l22 = Lexer(&r22);
  auto l24 = Lexer(&r24);
  auto l25 = Lexer(&r25);
  auto l26 = Lexer(&r26);
  auto l27 = Lexer(&r27);
  auto l28 = Lexer(&r28);
  auto l29 = Lexer(&r29);
  auto l30 = Lexer(&r30);
  auto l31 = Lexer(&r31);
  auto l32 = Lexer(&r32);

  EXPECT_EQ(std::get<1>(l4.next()).str(), "KW_BREAK(1:1-1:6): break");
  EXPECT_EQ(std::get<1>(l6.next()).str(), "KW_CATCH(1:1-1:6): catch");
  EXPECT_EQ(std::get<1>(l8.next()).str(), "KW_CONTINUE(1:1-1:9): continue");
  EXPECT_EQ(std::get<1>(l10.next()).str(), "KW_ELIF(1:1-1:5): elif");
  EXPECT_EQ(std::get<1>(l11.next()).str(), "KW_ELSE(1:1-1:5): else");
  EXPECT_EQ(std::get<1>(l12.next()).str(), "KW_ENUM(1:1-1:5): enum");
  EXPECT_EQ(std::get<1>(l15.next()).str(), "KW_FALSE(1:1-1:6): false");
  EXPECT_EQ(std::get<1>(l16.next()).str(), "KW_FN(1:1-1:3): fn");
  EXPECT_EQ(std::get<1>(l18.next()).str(), "KW_IF(1:1-1:3): if");
  EXPECT_EQ(std::get<1>(l20.next()).str(), "KW_IS(1:1-1:3): is");
  EXPECT_EQ(std::get<1>(l21.next()).str(), "KW_LOOP(1:1-1:5): loop");
  EXPECT_EQ(std::get<1>(l22.next()).str(), "KW_MAIN(1:1-1:5): main");
  EXPECT_EQ(std::get<1>(l24.next()).str(), "KW_MUT(1:1-1:4): mut");
  EXPECT_EQ(std::get<1>(l25.next()).str(), "KW_NIL(1:1-1:4): nil");
  EXPECT_EQ(std::get<1>(l26.next()).str(), "KW_OBJ(1:1-1:4): obj");
  EXPECT_EQ(std::get<1>(l27.next()).str(), "KW_REF(1:1-1:4): ref");
  EXPECT_EQ(std::get<1>(l28.next()).str(), "KW_RETURN(1:1-1:7): return");
  EXPECT_EQ(std::get<1>(l29.next()).str(), "KW_THROW(1:1-1:6): throw");
  EXPECT_EQ(std::get<1>(l30.next()).str(), "KW_TRUE(1:1-1:5): true");
  EXPECT_EQ(std::get<1>(l31.next()).str(), "KW_TRY(1:1-1:4): try");
  EXPECT_EQ(std::get<1>(l32.next()).str(), "KW_UNION(1:1-1:6): union");
}

TEST(LexerTest, LexKeywordsWhitespace) {
  auto reader = testing::NiceMock<MockReader>(
    " break catch continue elif else enum false fn if is loop main mut nil obj ref return throw true try union "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_BREAK(1:2-1:7): break");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_CATCH(1:8-1:13): catch");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_CONTINUE(1:14-1:22): continue");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_ELIF(1:23-1:27): elif");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_ELSE(1:28-1:32): else");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_ENUM(1:33-1:37): enum");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_FALSE(1:38-1:43): false");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_FN(1:44-1:46): fn");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_IF(1:47-1:49): if");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_IS(1:50-1:52): is");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_LOOP(1:53-1:57): loop");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_MAIN(1:58-1:62): main");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_MUT(1:63-1:66): mut");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_NIL(1:67-1:70): nil");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_OBJ(1:71-1:74): obj");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_REF(1:75-1:78): ref");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_RETURN(1:79-1:85): return");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_THROW(1:86-1:91): throw");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_TRUE(1:92-1:96): true");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_TRY(1:97-1:100): try");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "KW_UNION(1:101-1:106): union");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:107-1:107)");
}

TEST(LexerTest, LexKeywordsAsIdentifiers) {
  auto r1 = testing::NiceMock<MockReader>("asm");
  auto r2 = testing::NiceMock<MockReader>("asynchronously");
  auto r3 = testing::NiceMock<MockReader>("awaited");
  auto r4 = testing::NiceMock<MockReader>("breakpoint");
  auto r6 = testing::NiceMock<MockReader>("catching");
  auto r8 = testing::NiceMock<MockReader>("continues");
  auto r10 = testing::NiceMock<MockReader>("elifffy");
  auto r11 = testing::NiceMock<MockReader>("elsewhere");
  auto r12 = testing::NiceMock<MockReader>("enumeration");
  auto r13 = testing::NiceMock<MockReader>("exported");
  auto r15 = testing::NiceMock<MockReader>("falser");
  auto r16 = testing::NiceMock<MockReader>("fname");
  auto r17 = testing::NiceMock<MockReader>("fromental");
  auto r18 = testing::NiceMock<MockReader>("iffy");
  auto r19 = testing::NiceMock<MockReader>("importing");
  auto r20 = testing::NiceMock<MockReader>("isle");
  auto r21 = testing::NiceMock<MockReader>("looped");
  auto r22 = testing::NiceMock<MockReader>("maintain");
  auto r24 = testing::NiceMock<MockReader>("mutable");
  auto r25 = testing::NiceMock<MockReader>("nilled");
  auto r26 = testing::NiceMock<MockReader>("object");
  auto r27 = testing::NiceMock<MockReader>("referring");
  auto r28 = testing::NiceMock<MockReader>("returned");
  auto r29 = testing::NiceMock<MockReader>("throwing");
  auto r30 = testing::NiceMock<MockReader>("trues");
  auto r31 = testing::NiceMock<MockReader>("tryout");
  auto r32 = testing::NiceMock<MockReader>("unions");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l6 = Lexer(&r6);
  auto l8 = Lexer(&r8);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l13 = Lexer(&r13);
  auto l15 = Lexer(&r15);
  auto l16 = Lexer(&r16);
  auto l17 = Lexer(&r17);
  auto l18 = Lexer(&r18);
  auto l19 = Lexer(&r19);
  auto l20 = Lexer(&r20);
  auto l21 = Lexer(&r21);
  auto l22 = Lexer(&r22);
  auto l24 = Lexer(&r24);
  auto l25 = Lexer(&r25);
  auto l26 = Lexer(&r26);
  auto l27 = Lexer(&r27);
  auto l28 = Lexer(&r28);
  auto l29 = Lexer(&r29);
  auto l30 = Lexer(&r30);
  auto l31 = Lexer(&r31);
  auto l32 = Lexer(&r32);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "ID(1:1-1:4): asm");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "ID(1:1-1:15): asynchronously");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "ID(1:1-1:8): awaited");
  EXPECT_EQ(std::get<1>(l4.next()).str(), "ID(1:1-1:11): breakpoint");
  EXPECT_EQ(std::get<1>(l6.next()).str(), "ID(1:1-1:9): catching");
  EXPECT_EQ(std::get<1>(l8.next()).str(), "ID(1:1-1:10): continues");
  EXPECT_EQ(std::get<1>(l10.next()).str(), "ID(1:1-1:8): elifffy");
  EXPECT_EQ(std::get<1>(l11.next()).str(), "ID(1:1-1:10): elsewhere");
  EXPECT_EQ(std::get<1>(l12.next()).str(), "ID(1:1-1:12): enumeration");
  EXPECT_EQ(std::get<1>(l13.next()).str(), "ID(1:1-1:9): exported");
  EXPECT_EQ(std::get<1>(l15.next()).str(), "ID(1:1-1:7): falser");
  EXPECT_EQ(std::get<1>(l16.next()).str(), "ID(1:1-1:6): fname");
  EXPECT_EQ(std::get<1>(l17.next()).str(), "ID(1:1-1:10): fromental");
  EXPECT_EQ(std::get<1>(l18.next()).str(), "ID(1:1-1:5): iffy");
  EXPECT_EQ(std::get<1>(l19.next()).str(), "ID(1:1-1:10): importing");
  EXPECT_EQ(std::get<1>(l20.next()).str(), "ID(1:1-1:5): isle");
  EXPECT_EQ(std::get<1>(l21.next()).str(), "ID(1:1-1:7): looped");
  EXPECT_EQ(std::get<1>(l22.next()).str(), "ID(1:1-1:9): maintain");
  EXPECT_EQ(std::get<1>(l24.next()).str(), "ID(1:1-1:8): mutable");
  EXPECT_EQ(std::get<1>(l25.next()).str(), "ID(1:1-1:7): nilled");
  EXPECT_EQ(std::get<1>(l26.next()).str(), "ID(1:1-1:7): object");
  EXPECT_EQ(std::get<1>(l27.next()).str(), "ID(1:1-1:10): referring");
  EXPECT_EQ(std::get<1>(l28.next()).str(), "ID(1:1-1:9): returned");
  EXPECT_EQ(std::get<1>(l29.next()).str(), "ID(1:1-1:9): throwing");
  EXPECT_EQ(std::get<1>(l30.next()).str(), "ID(1:1-1:6): trues");
  EXPECT_EQ(std::get<1>(l31.next()).str(), "ID(1:1-1:7): tryout");
  EXPECT_EQ(std::get<1>(l32.next()).str(), "ID(1:1-1:7): unions");
}

TEST(LexerTest, LexIdentifier) {
  auto r1 = testing::NiceMock<MockReader>("a");
  auto r2 = testing::NiceMock<MockReader>("A");
  auto r3 = testing::NiceMock<MockReader>("_");
  auto r4 = testing::NiceMock<MockReader>("number1");
  auto r5 = testing::NiceMock<MockReader>("ANYTHING");
  auto r6 = testing::NiceMock<MockReader>("__I1_D2__");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "ID(1:1-1:2): a");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "ID(1:1-1:2): A");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "ID(1:1-1:2): _");
  EXPECT_EQ(std::get<1>(l4.next()).str(), "ID(1:1-1:8): number1");
  EXPECT_EQ(std::get<1>(l5.next()).str(), "ID(1:1-1:9): ANYTHING");
  EXPECT_EQ(std::get<1>(l6.next()).str(), "ID(1:1-1:10): __I1_D2__");
}

TEST(LexerTest, LexIdentifierWhitespace) {
  auto reader = testing::NiceMock<MockReader>(" a A _ number1 ANYTHING __I1_D2__ ");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "ID(1:2-1:3): a");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "ID(1:4-1:5): A");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "ID(1:6-1:7): _");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "ID(1:8-1:15): number1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "ID(1:16-1:24): ANYTHING");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "ID(1:25-1:34): __I1_D2__");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:35-1:35)");
}

TEST(LexerTest, LexLitChar) {
  auto reader = testing::NiceMock<MockReader>(R"(' ''!''A''a''9''\n''\"''\'''\\')");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:1-1:4): ' '");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:4-1:7): '!'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:7-1:10): 'A'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:10-1:13): 'a'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:13-1:16): '9'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:16-1:20): '\n')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:20-1:24): '\"')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:24-1:28): '\'')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:28-1:32): '\\')");
}

TEST(LexerTest, LexLitCharWhitespace) {
  auto reader = testing::NiceMock<MockReader>(R"( ' ' '!' 'A' 'a' '9' '\n' '"' '\'' '\\' )");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:2-1:5): ' '");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:6-1:9): '!'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:10-1:13): 'A'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:14-1:17): 'a'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_CHAR(1:18-1:21): '9'");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:22-1:26): '\n')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:27-1:30): '"')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:31-1:35): '\'')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_CHAR(1:36-1:40): '\\')");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:41-1:41)");
}

TEST(LexerTest, LexLitCharEof) {
  auto r1 = testing::NiceMock<MockReader>("' '");
  auto r2 = testing::NiceMock<MockReader>("'!'");
  auto r3 = testing::NiceMock<MockReader>("'A'");
  auto r4 = testing::NiceMock<MockReader>("'a'");
  auto r5 = testing::NiceMock<MockReader>("'9'");
  auto r6 = testing::NiceMock<MockReader>(R"('\n')");
  auto r7 = testing::NiceMock<MockReader>(R"('\"')");
  auto r8 = testing::NiceMock<MockReader>(R"('\'')");
  auto r9 = testing::NiceMock<MockReader>(R"('\\')");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "LIT_CHAR(1:1-1:4): ' '");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "LIT_CHAR(1:1-1:4): '!'");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "LIT_CHAR(1:1-1:4): 'A'");
  EXPECT_EQ(std::get<1>(l4.next()).str(), "LIT_CHAR(1:1-1:4): 'a'");
  EXPECT_EQ(std::get<1>(l5.next()).str(), "LIT_CHAR(1:1-1:4): '9'");
  EXPECT_EQ(std::get<1>(l6.next()).str(), R"(LIT_CHAR(1:1-1:5): '\n')");
  EXPECT_EQ(std::get<1>(l7.next()).str(), R"(LIT_CHAR(1:1-1:5): '\"')");
  EXPECT_EQ(std::get<1>(l8.next()).str(), R"(LIT_CHAR(1:1-1:5): '\'')");
  EXPECT_EQ(std::get<1>(l9.next()).str(), R"(LIT_CHAR(1:1-1:5): '\\')");
}

TEST(LexerTest, ThrowsOnNotClosedLitChar) {
  auto r1 = testing::NiceMock<MockReader>("'");
  auto r2 = testing::NiceMock<MockReader>(R"('a)");
  auto r3 = testing::NiceMock<MockReader>(R"('\)");
  auto r4 = testing::NiceMock<MockReader>(R"('\n)");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0002 + EOL "  1 | '" EOL "    | ^");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0002 + EOL "  1 | 'a" EOL "    | ^~");
  EXPECT_THROW_WITH_MESSAGE(l3.next(), std::string("/test:1:1: ") + E0002 + EOL "  1 | '\\" EOL "    | ^~");
  EXPECT_THROW_WITH_MESSAGE(l4.next(), std::string("/test:1:1: ") + E0002 + EOL "  1 | '\\n" EOL "    | ^~~");
}

TEST(LexerTest, ThrowsOnEmptyLitChar) {
  auto reader = testing::NiceMock<MockReader>("''");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE(lexer.next(), std::string("/test:1:1: ") + E0004 + EOL "  1 | ''" EOL "    | ^~");
}

TEST(LexerTest, ThrowsOnLitCharIllegalEscSeq) {
  auto r1 = testing::NiceMock<MockReader>(R"('\m)");
  auto r2 = testing::NiceMock<MockReader>(R"('\m')");
  auto r3 = testing::NiceMock<MockReader>(R"('\ma)");
  auto r4 = testing::NiceMock<MockReader>(R"('\ma')");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:2: ") + E0005 + EOL "  1 | '\\m" EOL "    |  ^~");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:2: ") + E0005 + EOL "  1 | '\\m'" EOL "    |  ^~");
  EXPECT_THROW_WITH_MESSAGE(l3.next(), std::string("/test:1:2: ") + E0005 + EOL "  1 | '\\ma" EOL "    |  ^~");
  EXPECT_THROW_WITH_MESSAGE(l4.next(), std::string("/test:1:2: ") + E0005 + EOL "  1 | '\\ma'" EOL "    |  ^~");
}

TEST(LexerTest, ThrowsOnLitCharTooManyCharacters) {
  auto r1 = testing::NiceMock<MockReader>(R"('ch)");
  auto r2 = testing::NiceMock<MockReader>(R"('char')");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0006 + EOL "  1 | 'ch" EOL "    | ^~~");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0006 + EOL "  1 | 'char'" EOL "    | ^~~~~~");
}

TEST(LexerTest, ThrowsOnLitCharNewLineChar) {
  auto r1 = testing::NiceMock<MockReader>("'" EOL "'");
  auto l1 = Lexer(&r1);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0017 + EOL "  1 | '" EOL "    | ^");
}

TEST(LexerTest, LexLitFloat) {
  auto r1 = testing::NiceMock<MockReader>("0E0");
  auto r2 = testing::NiceMock<MockReader>("0E1");
  auto r3 = testing::NiceMock<MockReader>("1e308");
  auto r4 = testing::NiceMock<MockReader>("1e+0");
  auto r5 = testing::NiceMock<MockReader>("18446744073709551615e+1");
  auto r6 = testing::NiceMock<MockReader>("18446744073709551615e-308");
  auto r7 = testing::NiceMock<MockReader>("0.E0");
  auto r8 = testing::NiceMock<MockReader>("0.E1");
  auto r9 = testing::NiceMock<MockReader>("1.e308");
  auto r10 = testing::NiceMock<MockReader>("1.e+0");
  auto r11 = testing::NiceMock<MockReader>("18446744073709551615.e+1");
  auto r12 = testing::NiceMock<MockReader>("18446744073709551615.e-308");
  auto r13 = testing::NiceMock<MockReader>("0.0");
  auto r14 = testing::NiceMock<MockReader>("1.1");
  auto r15 = testing::NiceMock<MockReader>("18446744073709551615.18446744073709551615");
  auto r16 = testing::NiceMock<MockReader>("0.0E0");
  auto r17 = testing::NiceMock<MockReader>("0.0E1");
  auto r18 = testing::NiceMock<MockReader>("1.1e308");
  auto r19 = testing::NiceMock<MockReader>("1.1E+0");
  auto r20 = testing::NiceMock<MockReader>("18446744073709551615.18446744073709551615E+1");
  auto r21 = testing::NiceMock<MockReader>("18446744073709551615.18446744073709551615e-308");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l13 = Lexer(&r13);
  auto l14 = Lexer(&r14);
  auto l15 = Lexer(&r15);
  auto l16 = Lexer(&r16);
  auto l17 = Lexer(&r17);
  auto l18 = Lexer(&r18);
  auto l19 = Lexer(&r19);
  auto l20 = Lexer(&r20);
  auto l21 = Lexer(&r21);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "LIT_FLOAT(1:1-1:4): 0E0");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "LIT_FLOAT(1:1-1:4): 0E1");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "LIT_FLOAT(1:1-1:6): 1e308");
  EXPECT_EQ(std::get<1>(l4.next()).str(), "LIT_FLOAT(1:1-1:5): 1e+0");
  EXPECT_EQ(std::get<1>(l5.next()).str(), "LIT_FLOAT(1:1-1:24): 18446744073709551615e+1");
  EXPECT_EQ(std::get<1>(l6.next()).str(), "LIT_FLOAT(1:1-1:26): 18446744073709551615e-308");
  EXPECT_EQ(std::get<1>(l7.next()).str(), "LIT_FLOAT(1:1-1:5): 0.E0");
  EXPECT_EQ(std::get<1>(l8.next()).str(), "LIT_FLOAT(1:1-1:5): 0.E1");
  EXPECT_EQ(std::get<1>(l9.next()).str(), "LIT_FLOAT(1:1-1:7): 1.e308");
  EXPECT_EQ(std::get<1>(l10.next()).str(), "LIT_FLOAT(1:1-1:6): 1.e+0");
  EXPECT_EQ(std::get<1>(l11.next()).str(), "LIT_FLOAT(1:1-1:25): 18446744073709551615.e+1");
  EXPECT_EQ(std::get<1>(l12.next()).str(), "LIT_FLOAT(1:1-1:27): 18446744073709551615.e-308");
  EXPECT_EQ(std::get<1>(l13.next()).str(), "LIT_FLOAT(1:1-1:4): 0.0");
  EXPECT_EQ(std::get<1>(l14.next()).str(), "LIT_FLOAT(1:1-1:4): 1.1");
  EXPECT_EQ(std::get<1>(l15.next()).str(), "LIT_FLOAT(1:1-1:42): 18446744073709551615.18446744073709551615");
  EXPECT_EQ(std::get<1>(l16.next()).str(), "LIT_FLOAT(1:1-1:6): 0.0E0");
  EXPECT_EQ(std::get<1>(l17.next()).str(), "LIT_FLOAT(1:1-1:6): 0.0E1");
  EXPECT_EQ(std::get<1>(l18.next()).str(), "LIT_FLOAT(1:1-1:8): 1.1e308");
  EXPECT_EQ(std::get<1>(l19.next()).str(), "LIT_FLOAT(1:1-1:7): 1.1E+0");
  EXPECT_EQ(std::get<1>(l20.next()).str(), "LIT_FLOAT(1:1-1:45): 18446744073709551615.18446744073709551615E+1");
  EXPECT_EQ(std::get<1>(l21.next()).str(), "LIT_FLOAT(1:1-1:47): 18446744073709551615.18446744073709551615e-308");
}

TEST(LexerTest, LexLitFloatWhitespace) {
  auto reader = testing::NiceMock<MockReader>(
    " 0E0 0E1 1e308 1e+0 18446744073709551615e+1 18446744073709551615e-308"
    " 0.E0 0.E1 1.e308 1.e+0 18446744073709551615.e+1 18446744073709551615.e-308"
    " 0.0 1.1 18446744073709551615.18446744073709551615"
    " 0.0E0 0.0E1 1.1e308 1.1E+0"
    " 18446744073709551615.18446744073709551615E+1 18446744073709551615.18446744073709551615e-308 "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:2-1:5): 0E0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:6-1:9): 0E1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:10-1:15): 1e308");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:16-1:20): 1e+0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:21-1:44): 18446744073709551615e+1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:45-1:70): 18446744073709551615e-308");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:71-1:75): 0.E0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:76-1:80): 0.E1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:81-1:87): 1.e308");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:88-1:93): 1.e+0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:94-1:118): 18446744073709551615.e+1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:119-1:145): 18446744073709551615.e-308");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:146-1:149): 0.0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:150-1:153): 1.1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:154-1:195): 18446744073709551615.18446744073709551615");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:196-1:201): 0.0E0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:202-1:207): 0.0E1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:208-1:215): 1.1e308");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:216-1:222): 1.1E+0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:223-1:267): 18446744073709551615.18446744073709551615E+1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_FLOAT(1:268-1:314): 18446744073709551615.18446744073709551615e-308");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:315-1:315)");
}

TEST(LexerTest, ThrowsOnInvalidLitFloat) {
  auto r1 = testing::NiceMock<MockReader>(R"(1234.)");
  auto r2 = testing::NiceMock<MockReader>(R"(1234.a)");
  auto r3 = testing::NiceMock<MockReader>(R"(1234.aZ)");
  auto r4 = testing::NiceMock<MockReader>(R"(1234.0a)");
  auto r5 = testing::NiceMock<MockReader>(R"(1234.1aZ)");
  auto r6 = testing::NiceMock<MockReader>(R"(1234e)");
  auto r7 = testing::NiceMock<MockReader>(R"(1234e+)");
  auto r8 = testing::NiceMock<MockReader>(R"(1234e-)");
  auto r9 = testing::NiceMock<MockReader>(R"(1234e+a1)");
  auto r10 = testing::NiceMock<MockReader>(R"(1234eZ)");
  auto r11 = testing::NiceMock<MockReader>(R"(1234eZa)");
  auto r12 = testing::NiceMock<MockReader>(R"(1234e5e6)");
  auto r13 = testing::NiceMock<MockReader>(R"(1234e))");
  auto r14 = testing::NiceMock<MockReader>(R"(1234e+))");
  auto r15 = testing::NiceMock<MockReader>(R"(1234e-h))");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l13 = Lexer(&r13);
  auto l14 = Lexer(&r14);
  auto l15 = Lexer(&r15);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0012 + EOL "  1 | 1234." EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0012 + EOL "  1 | 1234.a" EOL "    | ^~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l3.next(), std::string("/test:1:1: ") + E0012 + EOL "  1 | 1234.aZ" EOL "    | ^~~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l4.next(), std::string("/test:1:1: ") + E0012 + EOL "  1 | 1234.0a" EOL "    | ^~~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l5.next(), std::string("/test:1:1: ") + E0012 + EOL "  1 | 1234.1aZ" EOL "    | ^~~~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l6.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e" EOL "    |     ^");
  EXPECT_THROW_WITH_MESSAGE(l7.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e+" EOL "    |     ^~");
  EXPECT_THROW_WITH_MESSAGE(l8.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e-" EOL "    |     ^~");
  EXPECT_THROW_WITH_MESSAGE(l9.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e+a1" EOL "    |     ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l10.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234eZ" EOL "    |     ^~");
  EXPECT_THROW_WITH_MESSAGE(l11.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234eZa" EOL "    |     ^~~");
  EXPECT_THROW_WITH_MESSAGE(l12.next(), std::string("/test:1:1: ") + E0012 + EOL "  1 | 1234e5e6" EOL "    | ^~~~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l13.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e)" EOL "    |     ^");
  EXPECT_THROW_WITH_MESSAGE(l14.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e+)" EOL "    |     ^~");
  EXPECT_THROW_WITH_MESSAGE(l15.next(), std::string("/test:1:5: ") + E0013 + EOL "  1 | 1234e-h)" EOL "    |     ^~~");
}

TEST(LexerTest, ThrowsOnNonDecLitFloat) {
  auto r1 = testing::NiceMock<MockReader>("0b1.0");
  auto r2 = testing::NiceMock<MockReader>("0xa.0");
  auto r3 = testing::NiceMock<MockReader>("0o1.0");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0014 + EOL "  1 | 0b1.0" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0015 + EOL "  1 | 0xa.0" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l3.next(), std::string("/test:1:1: ") + E0016 + EOL "  1 | 0o1.0" EOL "    | ^~~~~");
}

TEST(LexerTest, LexLitInteger) {
  auto reader = testing::NiceMock<MockReader>(
    "..0B0..0b1..0B00000000000000000000000000000000..0b11111111111111111111111111111111"
    "..0..1..18446744073709551615"
    "..0X0..0x9..0XA..0xf..0X9999999999999999..0xffffffffffffffff"
    "..0O0..0o7..0O000000000000000000000..0o777777777777777777777.."
  );

  auto lexer = Lexer(&reader);

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:3-1:6): 0B0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:8-1:11): 0b1");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:13-1:47): 0B00000000000000000000000000000000");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:49-1:83): 0b11111111111111111111111111111111");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_DEC(1:85-1:86): 0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_DEC(1:88-1:89): 1");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_DEC(1:91-1:111): 18446744073709551615");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:113-1:116): 0X0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:118-1:121): 0x9");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:123-1:126): 0XA");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:128-1:131): 0xf");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:133-1:151): 0X9999999999999999");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:153-1:171): 0xffffffffffffffff");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:173-1:176): 0O0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:178-1:181): 0o7");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:183-1:206): 0O000000000000000000000");

  lexer.next();
  lexer.next();
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:208-1:231): 0o777777777777777777777");
}

TEST(LexerTest, LexLitIntegerWhitespace) {
  auto reader = testing::NiceMock<MockReader>(
    " 0B0 0b1 0B00000000000000000000000000000000 0b11111111111111111111111111111111"
    " 0 1 18446744073709551615"
    " 0X0 0x9 0XA 0xf 0X9999999999999999 0xffffffffffffffff"
    " 0O0 0o7 0O000000000000000000000 0o777777777777777777777 "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:2-1:5): 0B0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:6-1:9): 0b1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:10-1:44): 0B00000000000000000000000000000000");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_BIN(1:45-1:79): 0b11111111111111111111111111111111");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_DEC(1:80-1:81): 0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_DEC(1:82-1:83): 1");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_DEC(1:84-1:104): 18446744073709551615");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:105-1:108): 0X0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:109-1:112): 0x9");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:113-1:116): 0XA");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:117-1:120): 0xf");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:121-1:139): 0X9999999999999999");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_HEX(1:140-1:158): 0xffffffffffffffff");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:159-1:162): 0O0");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:163-1:166): 0o7");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:167-1:190): 0O000000000000000000000");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "LIT_INT_OCT(1:191-1:214): 0o777777777777777777777");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), "EOF(1:215-1:215)");
}

TEST(LexerTest, LexLitIntegerEof) {
  auto r1 = testing::NiceMock<MockReader>("0B0");
  auto r2 = testing::NiceMock<MockReader>("0b1");
  auto r3 = testing::NiceMock<MockReader>("0B00000000000000000000000000000000");
  auto r4 = testing::NiceMock<MockReader>("0b11111111111111111111111111111111");
  auto r5 = testing::NiceMock<MockReader>("0");
  auto r6 = testing::NiceMock<MockReader>("1");
  auto r7 = testing::NiceMock<MockReader>("18446744073709551615");
  auto r8 = testing::NiceMock<MockReader>("0X0");
  auto r9 = testing::NiceMock<MockReader>("0x9");
  auto r10 = testing::NiceMock<MockReader>("0XA");
  auto r11 = testing::NiceMock<MockReader>("0xf");
  auto r12 = testing::NiceMock<MockReader>("0X9999999999999999");
  auto r13 = testing::NiceMock<MockReader>("0xffffffffffffffff");
  auto r14 = testing::NiceMock<MockReader>("0O0");
  auto r15 = testing::NiceMock<MockReader>("0o7");
  auto r16 = testing::NiceMock<MockReader>("0O000000000000000000000");
  auto r17 = testing::NiceMock<MockReader>("0o777777777777777777777");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l13 = Lexer(&r13);
  auto l14 = Lexer(&r14);
  auto l15 = Lexer(&r15);
  auto l16 = Lexer(&r16);
  auto l17 = Lexer(&r17);

  EXPECT_EQ(std::get<1>(l1.next()).str(), "LIT_INT_BIN(1:1-1:4): 0B0");
  EXPECT_EQ(std::get<1>(l2.next()).str(), "LIT_INT_BIN(1:1-1:4): 0b1");
  EXPECT_EQ(std::get<1>(l3.next()).str(), "LIT_INT_BIN(1:1-1:35): 0B00000000000000000000000000000000");
  EXPECT_EQ(std::get<1>(l4.next()).str(), "LIT_INT_BIN(1:1-1:35): 0b11111111111111111111111111111111");
  EXPECT_EQ(std::get<1>(l5.next()).str(), "LIT_INT_DEC(1:1-1:2): 0");
  EXPECT_EQ(std::get<1>(l6.next()).str(), "LIT_INT_DEC(1:1-1:2): 1");
  EXPECT_EQ(std::get<1>(l7.next()).str(), "LIT_INT_DEC(1:1-1:21): 18446744073709551615");
  EXPECT_EQ(std::get<1>(l8.next()).str(), "LIT_INT_HEX(1:1-1:4): 0X0");
  EXPECT_EQ(std::get<1>(l9.next()).str(), "LIT_INT_HEX(1:1-1:4): 0x9");
  EXPECT_EQ(std::get<1>(l10.next()).str(), "LIT_INT_HEX(1:1-1:4): 0XA");
  EXPECT_EQ(std::get<1>(l11.next()).str(), "LIT_INT_HEX(1:1-1:4): 0xf");
  EXPECT_EQ(std::get<1>(l12.next()).str(), "LIT_INT_HEX(1:1-1:19): 0X9999999999999999");
  EXPECT_EQ(std::get<1>(l13.next()).str(), "LIT_INT_HEX(1:1-1:19): 0xffffffffffffffff");
  EXPECT_EQ(std::get<1>(l14.next()).str(), "LIT_INT_OCT(1:1-1:4): 0O0");
  EXPECT_EQ(std::get<1>(l15.next()).str(), "LIT_INT_OCT(1:1-1:4): 0o7");
  EXPECT_EQ(std::get<1>(l16.next()).str(), "LIT_INT_OCT(1:1-1:24): 0O000000000000000000000");
  EXPECT_EQ(std::get<1>(l17.next()).str(), "LIT_INT_OCT(1:1-1:24): 0o777777777777777777777");
}

TEST(LexerTest, ThrowsOnLitIntegerWithLeadingZero) {
  auto r1 = testing::NiceMock<MockReader>("04");
  auto r2 = testing::NiceMock<MockReader>("0400e0");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0007 + EOL "  1 | 04" EOL "    | ^~");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0007 + EOL "  1 | 0400e0" EOL "    | ^~~~~~");
}

TEST(LexerTest, ThrowsOnInvalidLitInteger) {
  auto r1 = testing::NiceMock<MockReader>("0B");
  auto r2 = testing::NiceMock<MockReader>("0bG");
  auto r3 = testing::NiceMock<MockReader>("0bGz");
  auto r4 = testing::NiceMock<MockReader>("0b1g");
  auto r5 = testing::NiceMock<MockReader>("0b1gZ");
  auto r6 = testing::NiceMock<MockReader>("0b1b0");
  auto r7 = testing::NiceMock<MockReader>("1234g");
  auto r8 = testing::NiceMock<MockReader>("1234gZ");
  auto r9 = testing::NiceMock<MockReader>("1234g0a");
  auto r10 = testing::NiceMock<MockReader>("0x");
  auto r11 = testing::NiceMock<MockReader>("0xG");
  auto r12 = testing::NiceMock<MockReader>("0xGz");
  auto r13 = testing::NiceMock<MockReader>("0Xag");
  auto r14 = testing::NiceMock<MockReader>("0XagZ");
  auto r15 = testing::NiceMock<MockReader>("0x1x2");
  auto r16 = testing::NiceMock<MockReader>("0o");
  auto r17 = testing::NiceMock<MockReader>("0oG");
  auto r18 = testing::NiceMock<MockReader>("0oGz");
  auto r19 = testing::NiceMock<MockReader>("0O1g");
  auto r20 = testing::NiceMock<MockReader>("0O1gZ");
  auto r21 = testing::NiceMock<MockReader>("0o1o2");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);
  auto l10 = Lexer(&r10);
  auto l11 = Lexer(&r11);
  auto l12 = Lexer(&r12);
  auto l13 = Lexer(&r13);
  auto l14 = Lexer(&r14);
  auto l15 = Lexer(&r15);
  auto l16 = Lexer(&r16);
  auto l17 = Lexer(&r17);
  auto l18 = Lexer(&r18);
  auto l19 = Lexer(&r19);
  auto l20 = Lexer(&r20);
  auto l21 = Lexer(&r21);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0008 + EOL "  1 | 0B" EOL "    | ^~");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0008 + EOL "  1 | 0bG" EOL "    | ^~~");
  EXPECT_THROW_WITH_MESSAGE(l3.next(), std::string("/test:1:1: ") + E0008 + EOL "  1 | 0bGz" EOL "    | ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l4.next(), std::string("/test:1:1: ") + E0008 + EOL "  1 | 0b1g" EOL "    | ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l5.next(), std::string("/test:1:1: ") + E0008 + EOL "  1 | 0b1gZ" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l6.next(), std::string("/test:1:1: ") + E0008 + EOL "  1 | 0b1b0" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l7.next(), std::string("/test:1:1: ") + E0009 + EOL "  1 | 1234g" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l8.next(), std::string("/test:1:1: ") + E0009 + EOL "  1 | 1234gZ" EOL "    | ^~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l9.next(), std::string("/test:1:1: ") + E0009 + EOL "  1 | 1234g0a" EOL "    | ^~~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l10.next(), std::string("/test:1:1: ") + E0010 + EOL "  1 | 0x" EOL "    | ^~");
  EXPECT_THROW_WITH_MESSAGE(l11.next(), std::string("/test:1:1: ") + E0010 + EOL "  1 | 0xG" EOL "    | ^~~");
  EXPECT_THROW_WITH_MESSAGE(l12.next(), std::string("/test:1:1: ") + E0010 + EOL "  1 | 0xGz" EOL "    | ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l13.next(), std::string("/test:1:1: ") + E0010 + EOL "  1 | 0Xag" EOL "    | ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l14.next(), std::string("/test:1:1: ") + E0010 + EOL "  1 | 0XagZ" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l15.next(), std::string("/test:1:1: ") + E0010 + EOL "  1 | 0x1x2" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l16.next(), std::string("/test:1:1: ") + E0011 + EOL "  1 | 0o" EOL "    | ^~");
  EXPECT_THROW_WITH_MESSAGE(l17.next(), std::string("/test:1:1: ") + E0011 + EOL "  1 | 0oG" EOL "    | ^~~");
  EXPECT_THROW_WITH_MESSAGE(l18.next(), std::string("/test:1:1: ") + E0011 + EOL "  1 | 0oGz" EOL "    | ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l19.next(), std::string("/test:1:1: ") + E0011 + EOL "  1 | 0O1g" EOL "    | ^~~~");
  EXPECT_THROW_WITH_MESSAGE(l20.next(), std::string("/test:1:1: ") + E0011 + EOL "  1 | 0O1gZ" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l21.next(), std::string("/test:1:1: ") + E0011 + EOL "  1 | 0o1o2" EOL "    | ^~~~~");
}

TEST(LexerTest, LexLitStr) {
  auto reader = testing::NiceMock<MockReader>(
    R"("")"
    R"("test")"
    R"("Hello,\n\r\tWorld!")"
    "\"Hello," EOL "World!\""
    "\"Hello,\rWorld!\""
    "\"Hello,\tWorld!\""
    R"("multiple \\n lines")"
    R"("\\0\\r\\t\\f\\v\"\'\\\0\r\t\f\v")"
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:1-1:3): "")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:3-1:9): "test")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:9-1:29): "Hello,\n\r\tWorld!")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:29-2:8): "Hello,)" ESC_EOL R"(World!")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(2:8-2:23): "Hello,\rWorld!")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(2:23-2:38): "Hello,\tWorld!")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(2:38-2:58): "multiple \\n lines")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(2:58-2:91)" R"(: "\\0\\r\\t\\f\\v\"\'\\\0\r\t\f\v")");
}

TEST(LexerTest, LexLitStrWhitespace) {
  auto reader = testing::NiceMock<MockReader>(
    R"( "")"
    R"( "test")"
    R"( "Hello,\nWorld!")"
    R"( "multiple \\n lines")"
    R"( "\\0\\r\\t\\f\\v\"\'\\\0\r\t\f\v")"
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:2-1:4): "")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:5-1:11): "test")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:12-1:28): "Hello,\nWorld!")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:29-1:49): "multiple \\n lines")");
  EXPECT_EQ(std::get<1>(lexer.next()).str(), R"(LIT_STR(1:50-1:83)" R"(: "\\0\\r\\t\\f\\v\"\'\\\0\r\t\f\v")");
}

TEST(LexerTest, LexLitStrEof) {
  auto r1 = testing::NiceMock<MockReader>(R"("")");
  auto r2 = testing::NiceMock<MockReader>(R"("test")");
  auto r3 = testing::NiceMock<MockReader>(R"("Hello,\nWorld!")");
  auto r4 = testing::NiceMock<MockReader>(R"("multiple \\n lines")");
  auto r5 = testing::NiceMock<MockReader>(R"("\\0\\r\\t\\f\\v\"\'\\\0\r\t\f\v")");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);

  EXPECT_EQ(std::get<1>(l1.next()).str(), R"(LIT_STR(1:1-1:3): "")");
  EXPECT_EQ(std::get<1>(l2.next()).str(), R"(LIT_STR(1:1-1:7): "test")");
  EXPECT_EQ(std::get<1>(l3.next()).str(), R"(LIT_STR(1:1-1:17): "Hello,\nWorld!")");
  EXPECT_EQ(std::get<1>(l4.next()).str(), R"(LIT_STR(1:1-1:21): "multiple \\n lines")");
  EXPECT_EQ(std::get<1>(l5.next()).str(), R"(LIT_STR(1:1-1:34)" R"(: "\\0\\r\\t\\f\\v\"\'\\\0\r\t\f\v")");
}

TEST(LexerTest, ThrowsOnEmptyLitStr) {
  auto r1 = testing::NiceMock<MockReader>(R"(")");
  auto r2 = testing::NiceMock<MockReader>(R"("text)");
  auto r3 = testing::NiceMock<MockReader>(R"("text\)");
  auto r4 = testing::NiceMock<MockReader>(R"("text\")");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);

  EXPECT_THROW_WITH_MESSAGE(l1.next(), std::string("/test:1:1: ") + E0003 + EOL "  1 | \"" EOL "    | ^");
  EXPECT_THROW_WITH_MESSAGE(l2.next(), std::string("/test:1:1: ") + E0003 + EOL "  1 | \"text" EOL "    | ^~~~~");
  EXPECT_THROW_WITH_MESSAGE(l3.next(), std::string("/test:1:1: ") + E0003 + EOL "  1 | \"text\\" EOL "    | ^~~~~~");
  EXPECT_THROW_WITH_MESSAGE(l4.next(), std::string("/test:1:1: ") + E0003 + EOL "  1 | \"text\\\"" EOL "    | ^~~~~~~");
}

TEST(LexerTest, ThrowsOnLitStrIllegalEscSeq) {
  auto reader = testing::NiceMock<MockReader>(R"("Hello, \m World!")");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE(lexer.next(), std::string("/test:1:9: ") + E0005 + EOL "  1 | \"Hello, \\m World!\"" EOL "    |         ^~");
}

TEST(LexerTest, SeeksTo) {
  auto reader = testing::NiceMock<MockReader>("main {" EOL "  print(\"Hello, World!\")" EOL "}" EOL);
  auto lexer = Lexer(&reader);

  lexer.seek(ReaderLocation{7, 1, 7});

  EXPECT_EQ(lexer.loc, (ReaderLocation{7, 1, 7}));
  EXPECT_EQ(lexer.reader->loc, (ReaderLocation{7, 1, 7}));
}
