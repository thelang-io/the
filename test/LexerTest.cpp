/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/Error.hpp"
#include "../src/Lexer.hpp"
#include "MockReader.hpp"
#include "utils.hpp"

TEST(LexerTest, ThrowsOnUnknownToken) {
  auto reader = ::testing::NiceMock<MockReader>("@");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, (std::string("/test:1:1: ") + E0000 + "\n  1 | @\n    | ^").c_str());
}

TEST(LexerTest, LexEof) {
  auto reader = ::testing::NiceMock<MockReader>("");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:1-1:1)");
}

TEST(LexerTest, LexSingleWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(" ");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:2-1:2)");
}

TEST(LexerTest, LexMultipleWhitespaces) {
  auto reader = ::testing::NiceMock<MockReader>(" \n\t");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(2:2-2:2)");
}

TEST(LexerTest, LexEmptyBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/**/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:5-1:5)");
}

TEST(LexerTest, LexAsteriskBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/***/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:6-1:6)");
}

TEST(LexerTest, LexSingleBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Kendall*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:18-1:18)");
}

TEST(LexerTest, LexMultipleBlockComments) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Kim*//*Hello Dream*//*Hello Stormi*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:45-1:45)");
}

TEST(LexerTest, LexMultilineBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Bella\nHello Olivia\nHello Rosa*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(3:13-3:13)");
}

TEST(LexerTest, ThrowsOnEmptyNotClosedBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, (std::string("/test:1:1: ") + E0001 + "\n  1 | /*\n    | ^~").c_str());
}

TEST(LexerTest, ThrowsOnNotClosedBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, (std::string("/test:1:1: ") + E0001 + "\n  1 | /*Hello\n    | ^~~~~~~").c_str());
}

TEST(LexerTest, LexEmptyLineCommentNoNewLine) {
  auto reader = ::testing::NiceMock<MockReader>("//");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:3-1:3)");
}

TEST(LexerTest, LexEmptyLineComment) {
  auto reader = ::testing::NiceMock<MockReader>("//\n");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(2:1-2:1)");
}

TEST(LexerTest, LexLineCommentNoNewLine) {
  auto reader = ::testing::NiceMock<MockReader>("//Hello Ariana");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(1:15-1:15)");
}

TEST(LexerTest, LexLineComment) {
  auto reader = ::testing::NiceMock<MockReader>("//Hello Hailey\n");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(2:1-2:1)");
}

TEST(LexerTest, LexWhitespacesCombinedWithComments) {
  auto reader = ::testing::NiceMock<MockReader>(" \t\r//Hello Mary\n/*Hello Linda*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "EOF(2:16-2:16)");
}

TEST(LexerTest, LexWhitespaceBeforeToken) {
  auto reader = ::testing::NiceMock<MockReader>(" \t;");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "OP_SEMI(1:3-1:4): ;");
}

TEST(LexerTest, LexWhitespaceAfterToken) {
  auto reader = ::testing::NiceMock<MockReader>("; \t");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "OP_SEMI(1:1-1:2): ;");
  EXPECT_EQ(lexer.next().str(), "EOF(1:4-1:4)");
}

TEST(LexerTest, LexBlockCommentBeforeToken) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Anna*/,");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "OP_COMMA(1:15-1:16): ,");
}

TEST(LexerTest, LexBlockCommentAfterToken) {
  auto reader = ::testing::NiceMock<MockReader>(",/*Hello Anna*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "OP_COMMA(1:1-1:2): ,");
  EXPECT_EQ(lexer.next().str(), "EOF(1:16-1:16)");
}

TEST(LexerTest, LexLineCommentBeforeToken) {
  auto reader = ::testing::NiceMock<MockReader>("//Hello Kylie\n+");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "OP_PLUS(2:1-2:2): +");
}

TEST(LexerTest, LexLineCommentAfterToken) {
  auto reader = ::testing::NiceMock<MockReader>("+//Hello Kylie\n");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "OP_PLUS(1:1-1:2): +");
  EXPECT_EQ(lexer.next().str(), "EOF(2:1-2:1)");
}

TEST(LexerTest, LexOperations) {
  auto r1 = ::testing::NiceMock<MockReader>("&^:,.=!>{[(<-|%+?}]);/~*");
  auto r2 = ::testing::NiceMock<MockReader>("&&!!<<--||++?.?\?>>**");
  auto r3 = ::testing::NiceMock<MockReader>("&=&&=^=:=...==!=>=<<=<=-=|=||=%=+=?\?=>>=/=*=**=");
  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_EQ(l1.next().str(), "OP_AND(1:1-1:2): &");
  EXPECT_EQ(l1.next().str(), "OP_CARET(1:2-1:3): ^");
  EXPECT_EQ(l1.next().str(), "OP_COLON(1:3-1:4): :");
  EXPECT_EQ(l1.next().str(), "OP_COMMA(1:4-1:5): ,");
  EXPECT_EQ(l1.next().str(), "OP_DOT(1:5-1:6): .");
  EXPECT_EQ(l1.next().str(), "OP_EQ(1:6-1:7): =");
  EXPECT_EQ(l1.next().str(), "OP_EXCL(1:7-1:8): !");
  EXPECT_EQ(l1.next().str(), "OP_GT(1:8-1:9): >");
  EXPECT_EQ(l1.next().str(), "OP_LBRACE(1:9-1:10): {");
  EXPECT_EQ(l1.next().str(), "OP_LBRACK(1:10-1:11): [");
  EXPECT_EQ(l1.next().str(), "OP_LPAR(1:11-1:12): (");
  EXPECT_EQ(l1.next().str(), "OP_LT(1:12-1:13): <");
  EXPECT_EQ(l1.next().str(), "OP_MINUS(1:13-1:14): -");
  EXPECT_EQ(l1.next().str(), "OP_OR(1:14-1:15): |");
  EXPECT_EQ(l1.next().str(), "OP_PERCENT(1:15-1:16): %");
  EXPECT_EQ(l1.next().str(), "OP_PLUS(1:16-1:17): +");
  EXPECT_EQ(l1.next().str(), "OP_QN(1:17-1:18): ?");
  EXPECT_EQ(l1.next().str(), "OP_RBRACE(1:18-1:19): }");
  EXPECT_EQ(l1.next().str(), "OP_RBRACK(1:19-1:20): ]");
  EXPECT_EQ(l1.next().str(), "OP_RPAR(1:20-1:21): )");
  EXPECT_EQ(l1.next().str(), "OP_SEMI(1:21-1:22): ;");
  EXPECT_EQ(l1.next().str(), "OP_SLASH(1:22-1:23): /");
  EXPECT_EQ(l1.next().str(), "OP_TILDE(1:23-1:24): ~");
  EXPECT_EQ(l1.next().str(), "OP_STAR(1:24-1:25): *");

  EXPECT_EQ(l2.next().str(), "OP_AND_AND(1:1-1:3): &&");
  EXPECT_EQ(l2.next().str(), "OP_EXCL_EXCL(1:3-1:5): !!");
  EXPECT_EQ(l2.next().str(), "OP_LSHIFT(1:5-1:7): <<");
  EXPECT_EQ(l2.next().str(), "OP_MINUS_MINUS(1:7-1:9): --");
  EXPECT_EQ(l2.next().str(), "OP_OR_OR(1:9-1:11): ||");
  EXPECT_EQ(l2.next().str(), "OP_PLUS_PLUS(1:11-1:13): ++");
  EXPECT_EQ(l2.next().str(), "OP_QN_DOT(1:13-1:15): ?.");
  EXPECT_EQ(l2.next().str(), "OP_QN_QN(1:15-1:17): ??");
  EXPECT_EQ(l2.next().str(), "OP_RSHIFT(1:17-1:19): >>");
  EXPECT_EQ(l2.next().str(), "OP_STAR_STAR(1:19-1:21): **");

  EXPECT_EQ(l3.next().str(), "OP_AND_EQ(1:1-1:3): &=");
  EXPECT_EQ(l3.next().str(), "OP_AND_AND_EQ(1:3-1:6): &&=");
  EXPECT_EQ(l3.next().str(), "OP_CARET_EQ(1:6-1:8): ^=");
  EXPECT_EQ(l3.next().str(), "OP_COLON_EQ(1:8-1:10): :=");
  EXPECT_EQ(l3.next().str(), "OP_DOT_DOT_DOT(1:10-1:13): ...");
  EXPECT_EQ(l3.next().str(), "OP_EQ_EQ(1:13-1:15): ==");
  EXPECT_EQ(l3.next().str(), "OP_EXCL_EQ(1:15-1:17): !=");
  EXPECT_EQ(l3.next().str(), "OP_GT_EQ(1:17-1:19): >=");
  EXPECT_EQ(l3.next().str(), "OP_LSHIFT_EQ(1:19-1:22): <<=");
  EXPECT_EQ(l3.next().str(), "OP_LT_EQ(1:22-1:24): <=");
  EXPECT_EQ(l3.next().str(), "OP_MINUS_EQ(1:24-1:26): -=");
  EXPECT_EQ(l3.next().str(), "OP_OR_EQ(1:26-1:28): |=");
  EXPECT_EQ(l3.next().str(), "OP_OR_OR_EQ(1:28-1:31): ||=");
  EXPECT_EQ(l3.next().str(), "OP_PERCENT_EQ(1:31-1:33): %=");
  EXPECT_EQ(l3.next().str(), "OP_PLUS_EQ(1:33-1:35): +=");
  EXPECT_EQ(l3.next().str(), "OP_QN_QN_EQ(1:35-1:38): ?\?=");
  EXPECT_EQ(l3.next().str(), "OP_RSHIFT_EQ(1:38-1:41): >>=");
  EXPECT_EQ(l3.next().str(), "OP_SLASH_EQ(1:41-1:43): /=");
  EXPECT_EQ(l3.next().str(), "OP_STAR_EQ(1:43-1:45): *=");
  EXPECT_EQ(l3.next().str(), "OP_STAR_STAR_EQ(1:45-1:48): **=");
}

TEST(LexerTest, LexOperationsWhitespace) {
  auto r1 = ::testing::NiceMock<MockReader>(" & ^ : , . = ! > { [ ( < - | % + ? } ] ) ; / * ~ ");
  auto r2 = ::testing::NiceMock<MockReader>(" && !! << -- || ++ ?. ?? >> ** ");
  auto r3 = ::testing::NiceMock<MockReader>(" &= &&= ^= := ... == != >= <<= <= -= |= ||= %= += ?\?= >>= /= *= **= ");
  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_EQ(l1.next().str(), "OP_AND(1:2-1:3): &");
  EXPECT_EQ(l1.next().str(), "OP_CARET(1:4-1:5): ^");
  EXPECT_EQ(l1.next().str(), "OP_COLON(1:6-1:7): :");
  EXPECT_EQ(l1.next().str(), "OP_COMMA(1:8-1:9): ,");
  EXPECT_EQ(l1.next().str(), "OP_DOT(1:10-1:11): .");
  EXPECT_EQ(l1.next().str(), "OP_EQ(1:12-1:13): =");
  EXPECT_EQ(l1.next().str(), "OP_EXCL(1:14-1:15): !");
  EXPECT_EQ(l1.next().str(), "OP_GT(1:16-1:17): >");
  EXPECT_EQ(l1.next().str(), "OP_LBRACE(1:18-1:19): {");
  EXPECT_EQ(l1.next().str(), "OP_LBRACK(1:20-1:21): [");
  EXPECT_EQ(l1.next().str(), "OP_LPAR(1:22-1:23): (");
  EXPECT_EQ(l1.next().str(), "OP_LT(1:24-1:25): <");
  EXPECT_EQ(l1.next().str(), "OP_MINUS(1:26-1:27): -");
  EXPECT_EQ(l1.next().str(), "OP_OR(1:28-1:29): |");
  EXPECT_EQ(l1.next().str(), "OP_PERCENT(1:30-1:31): %");
  EXPECT_EQ(l1.next().str(), "OP_PLUS(1:32-1:33): +");
  EXPECT_EQ(l1.next().str(), "OP_QN(1:34-1:35): ?");
  EXPECT_EQ(l1.next().str(), "OP_RBRACE(1:36-1:37): }");
  EXPECT_EQ(l1.next().str(), "OP_RBRACK(1:38-1:39): ]");
  EXPECT_EQ(l1.next().str(), "OP_RPAR(1:40-1:41): )");
  EXPECT_EQ(l1.next().str(), "OP_SEMI(1:42-1:43): ;");
  EXPECT_EQ(l1.next().str(), "OP_SLASH(1:44-1:45): /");
  EXPECT_EQ(l1.next().str(), "OP_STAR(1:46-1:47): *");
  EXPECT_EQ(l1.next().str(), "OP_TILDE(1:48-1:49): ~");
  EXPECT_EQ(l1.next().str(), "EOF(1:50-1:50)");

  EXPECT_EQ(l2.next().str(), "OP_AND_AND(1:2-1:4): &&");
  EXPECT_EQ(l2.next().str(), "OP_EXCL_EXCL(1:5-1:7): !!");
  EXPECT_EQ(l2.next().str(), "OP_LSHIFT(1:8-1:10): <<");
  EXPECT_EQ(l2.next().str(), "OP_MINUS_MINUS(1:11-1:13): --");
  EXPECT_EQ(l2.next().str(), "OP_OR_OR(1:14-1:16): ||");
  EXPECT_EQ(l2.next().str(), "OP_PLUS_PLUS(1:17-1:19): ++");
  EXPECT_EQ(l2.next().str(), "OP_QN_DOT(1:20-1:22): ?.");
  EXPECT_EQ(l2.next().str(), "OP_QN_QN(1:23-1:25): ??");
  EXPECT_EQ(l2.next().str(), "OP_RSHIFT(1:26-1:28): >>");
  EXPECT_EQ(l2.next().str(), "OP_STAR_STAR(1:29-1:31): **");
  EXPECT_EQ(l2.next().str(), "EOF(1:32-1:32)");

  EXPECT_EQ(l3.next().str(), "OP_AND_EQ(1:2-1:4): &=");
  EXPECT_EQ(l3.next().str(), "OP_AND_AND_EQ(1:5-1:8): &&=");
  EXPECT_EQ(l3.next().str(), "OP_CARET_EQ(1:9-1:11): ^=");
  EXPECT_EQ(l3.next().str(), "OP_COLON_EQ(1:12-1:14): :=");
  EXPECT_EQ(l3.next().str(), "OP_DOT_DOT_DOT(1:15-1:18): ...");
  EXPECT_EQ(l3.next().str(), "OP_EQ_EQ(1:19-1:21): ==");
  EXPECT_EQ(l3.next().str(), "OP_EXCL_EQ(1:22-1:24): !=");
  EXPECT_EQ(l3.next().str(), "OP_GT_EQ(1:25-1:27): >=");
  EXPECT_EQ(l3.next().str(), "OP_LSHIFT_EQ(1:28-1:31): <<=");
  EXPECT_EQ(l3.next().str(), "OP_LT_EQ(1:32-1:34): <=");
  EXPECT_EQ(l3.next().str(), "OP_MINUS_EQ(1:35-1:37): -=");
  EXPECT_EQ(l3.next().str(), "OP_OR_EQ(1:38-1:40): |=");
  EXPECT_EQ(l3.next().str(), "OP_OR_OR_EQ(1:41-1:44): ||=");
  EXPECT_EQ(l3.next().str(), "OP_PERCENT_EQ(1:45-1:47): %=");
  EXPECT_EQ(l3.next().str(), "OP_PLUS_EQ(1:48-1:50): +=");
  EXPECT_EQ(l3.next().str(), "OP_QN_QN_EQ(1:51-1:54): ?\?=");
  EXPECT_EQ(l3.next().str(), "OP_RSHIFT_EQ(1:55-1:58): >>=");
  EXPECT_EQ(l3.next().str(), "OP_SLASH_EQ(1:59-1:61): /=");
  EXPECT_EQ(l3.next().str(), "OP_STAR_EQ(1:62-1:64): *=");
  EXPECT_EQ(l3.next().str(), "OP_STAR_STAR_EQ(1:65-1:68): **=");
  EXPECT_EQ(l3.next().str(), "EOF(1:69-1:69)");
}

TEST(LexerTest, LexOperationsEof) {
  auto r1 = ::testing::NiceMock<MockReader>("&");
  auto r2 = ::testing::NiceMock<MockReader>("^");
  auto r3 = ::testing::NiceMock<MockReader>(":");
  auto r4 = ::testing::NiceMock<MockReader>(",");
  auto r5 = ::testing::NiceMock<MockReader>(".");
  auto r6 = ::testing::NiceMock<MockReader>("=");
  auto r7 = ::testing::NiceMock<MockReader>("!");
  auto r8 = ::testing::NiceMock<MockReader>(">");
  auto r9 = ::testing::NiceMock<MockReader>("{");
  auto r10 = ::testing::NiceMock<MockReader>("[");
  auto r11 = ::testing::NiceMock<MockReader>("(");
  auto r12 = ::testing::NiceMock<MockReader>("<");
  auto r13 = ::testing::NiceMock<MockReader>("-");
  auto r14 = ::testing::NiceMock<MockReader>("|");
  auto r15 = ::testing::NiceMock<MockReader>("%");
  auto r16 = ::testing::NiceMock<MockReader>("+");
  auto r17 = ::testing::NiceMock<MockReader>("?");
  auto r18 = ::testing::NiceMock<MockReader>("}");
  auto r19 = ::testing::NiceMock<MockReader>("]");
  auto r20 = ::testing::NiceMock<MockReader>(")");
  auto r21 = ::testing::NiceMock<MockReader>(";");
  auto r22 = ::testing::NiceMock<MockReader>("/");
  auto r23 = ::testing::NiceMock<MockReader>("*");
  auto r24 = ::testing::NiceMock<MockReader>("~");
  auto r25 = ::testing::NiceMock<MockReader>("&&");
  auto r26 = ::testing::NiceMock<MockReader>("!!");
  auto r27 = ::testing::NiceMock<MockReader>("<<");
  auto r28 = ::testing::NiceMock<MockReader>("--");
  auto r29 = ::testing::NiceMock<MockReader>("||");
  auto r30 = ::testing::NiceMock<MockReader>("++");
  auto r31 = ::testing::NiceMock<MockReader>("?.");
  auto r32 = ::testing::NiceMock<MockReader>("??");
  auto r33 = ::testing::NiceMock<MockReader>(">>");
  auto r34 = ::testing::NiceMock<MockReader>("**");
  auto r35 = ::testing::NiceMock<MockReader>("&=");
  auto r36 = ::testing::NiceMock<MockReader>("&&=");
  auto r37 = ::testing::NiceMock<MockReader>("^=");
  auto r38 = ::testing::NiceMock<MockReader>(":=");
  auto r39 = ::testing::NiceMock<MockReader>("...");
  auto r40 = ::testing::NiceMock<MockReader>("==");
  auto r41 = ::testing::NiceMock<MockReader>("!=");
  auto r42 = ::testing::NiceMock<MockReader>(">=");
  auto r43 = ::testing::NiceMock<MockReader>("<<=");
  auto r44 = ::testing::NiceMock<MockReader>("<=");
  auto r45 = ::testing::NiceMock<MockReader>("-=");
  auto r46 = ::testing::NiceMock<MockReader>("|=");
  auto r47 = ::testing::NiceMock<MockReader>("||=");
  auto r48 = ::testing::NiceMock<MockReader>("%=");
  auto r49 = ::testing::NiceMock<MockReader>("+=");
  auto r50 = ::testing::NiceMock<MockReader>("?\?=");
  auto r51 = ::testing::NiceMock<MockReader>(">>=");
  auto r52 = ::testing::NiceMock<MockReader>("/=");
  auto r53 = ::testing::NiceMock<MockReader>("*=");
  auto r54 = ::testing::NiceMock<MockReader>("**=");

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
  auto l54 = Lexer(&r54);

  EXPECT_EQ(l1.next().str(), "OP_AND(1:1-1:2): &");
  EXPECT_EQ(l2.next().str(), "OP_CARET(1:1-1:2): ^");
  EXPECT_EQ(l3.next().str(), "OP_COLON(1:1-1:2): :");
  EXPECT_EQ(l4.next().str(), "OP_COMMA(1:1-1:2): ,");
  EXPECT_EQ(l5.next().str(), "OP_DOT(1:1-1:2): .");
  EXPECT_EQ(l6.next().str(), "OP_EQ(1:1-1:2): =");
  EXPECT_EQ(l7.next().str(), "OP_EXCL(1:1-1:2): !");
  EXPECT_EQ(l8.next().str(), "OP_GT(1:1-1:2): >");
  EXPECT_EQ(l9.next().str(), "OP_LBRACE(1:1-1:2): {");
  EXPECT_EQ(l10.next().str(), "OP_LBRACK(1:1-1:2): [");
  EXPECT_EQ(l11.next().str(), "OP_LPAR(1:1-1:2): (");
  EXPECT_EQ(l12.next().str(), "OP_LT(1:1-1:2): <");
  EXPECT_EQ(l13.next().str(), "OP_MINUS(1:1-1:2): -");
  EXPECT_EQ(l14.next().str(), "OP_OR(1:1-1:2): |");
  EXPECT_EQ(l15.next().str(), "OP_PERCENT(1:1-1:2): %");
  EXPECT_EQ(l16.next().str(), "OP_PLUS(1:1-1:2): +");
  EXPECT_EQ(l17.next().str(), "OP_QN(1:1-1:2): ?");
  EXPECT_EQ(l18.next().str(), "OP_RBRACE(1:1-1:2): }");
  EXPECT_EQ(l19.next().str(), "OP_RBRACK(1:1-1:2): ]");
  EXPECT_EQ(l20.next().str(), "OP_RPAR(1:1-1:2): )");
  EXPECT_EQ(l21.next().str(), "OP_SEMI(1:1-1:2): ;");
  EXPECT_EQ(l22.next().str(), "OP_SLASH(1:1-1:2): /");
  EXPECT_EQ(l23.next().str(), "OP_STAR(1:1-1:2): *");
  EXPECT_EQ(l24.next().str(), "OP_TILDE(1:1-1:2): ~");

  EXPECT_EQ(l25.next().str(), "OP_AND_AND(1:1-1:3): &&");
  EXPECT_EQ(l26.next().str(), "OP_EXCL_EXCL(1:1-1:3): !!");
  EXPECT_EQ(l27.next().str(), "OP_LSHIFT(1:1-1:3): <<");
  EXPECT_EQ(l28.next().str(), "OP_MINUS_MINUS(1:1-1:3): --");
  EXPECT_EQ(l29.next().str(), "OP_OR_OR(1:1-1:3): ||");
  EXPECT_EQ(l30.next().str(), "OP_PLUS_PLUS(1:1-1:3): ++");
  EXPECT_EQ(l31.next().str(), "OP_QN_DOT(1:1-1:3): ?.");
  EXPECT_EQ(l32.next().str(), "OP_QN_QN(1:1-1:3): ??");
  EXPECT_EQ(l33.next().str(), "OP_RSHIFT(1:1-1:3): >>");
  EXPECT_EQ(l34.next().str(), "OP_STAR_STAR(1:1-1:3): **");

  EXPECT_EQ(l35.next().str(), "OP_AND_EQ(1:1-1:3): &=");
  EXPECT_EQ(l36.next().str(), "OP_AND_AND_EQ(1:1-1:4): &&=");
  EXPECT_EQ(l37.next().str(), "OP_CARET_EQ(1:1-1:3): ^=");
  EXPECT_EQ(l38.next().str(), "OP_COLON_EQ(1:1-1:3): :=");
  EXPECT_EQ(l39.next().str(), "OP_DOT_DOT_DOT(1:1-1:4): ...");
  EXPECT_EQ(l40.next().str(), "OP_EQ_EQ(1:1-1:3): ==");
  EXPECT_EQ(l41.next().str(), "OP_EXCL_EQ(1:1-1:3): !=");
  EXPECT_EQ(l42.next().str(), "OP_GT_EQ(1:1-1:3): >=");
  EXPECT_EQ(l43.next().str(), "OP_LSHIFT_EQ(1:1-1:4): <<=");
  EXPECT_EQ(l44.next().str(), "OP_LT_EQ(1:1-1:3): <=");
  EXPECT_EQ(l45.next().str(), "OP_MINUS_EQ(1:1-1:3): -=");
  EXPECT_EQ(l46.next().str(), "OP_OR_EQ(1:1-1:3): |=");
  EXPECT_EQ(l47.next().str(), "OP_OR_OR_EQ(1:1-1:4): ||=");
  EXPECT_EQ(l48.next().str(), "OP_PERCENT_EQ(1:1-1:3): %=");
  EXPECT_EQ(l49.next().str(), "OP_PLUS_EQ(1:1-1:3): +=");
  EXPECT_EQ(l50.next().str(), "OP_QN_QN_EQ(1:1-1:4): ?\?=");
  EXPECT_EQ(l51.next().str(), "OP_RSHIFT_EQ(1:1-1:4): >>=");
  EXPECT_EQ(l52.next().str(), "OP_SLASH_EQ(1:1-1:3): /=");
  EXPECT_EQ(l53.next().str(), "OP_STAR_EQ(1:1-1:3): *=");
  EXPECT_EQ(l54.next().str(), "OP_STAR_STAR_EQ(1:1-1:4): **=");
}

TEST(LexerTest, LexKeywords) {
  auto r1 = ::testing::NiceMock<MockReader>("as");
  auto r2 = ::testing::NiceMock<MockReader>("async");
  auto r3 = ::testing::NiceMock<MockReader>("await");
  auto r4 = ::testing::NiceMock<MockReader>("break");
  auto r5 = ::testing::NiceMock<MockReader>("case");
  auto r6 = ::testing::NiceMock<MockReader>("catch");
  auto r7 = ::testing::NiceMock<MockReader>("class");
  auto r8 = ::testing::NiceMock<MockReader>("const");
  auto r9 = ::testing::NiceMock<MockReader>("continue");
  auto r10 = ::testing::NiceMock<MockReader>("default");
  auto r11 = ::testing::NiceMock<MockReader>("deinit");
  auto r12 = ::testing::NiceMock<MockReader>("elif");
  auto r13 = ::testing::NiceMock<MockReader>("else");
  auto r14 = ::testing::NiceMock<MockReader>("enum");
  auto r15 = ::testing::NiceMock<MockReader>("export");
  auto r16 = ::testing::NiceMock<MockReader>("fallthrough");
  auto r17 = ::testing::NiceMock<MockReader>("false");
  auto r18 = ::testing::NiceMock<MockReader>("fn");
  auto r19 = ::testing::NiceMock<MockReader>("from");
  auto r20 = ::testing::NiceMock<MockReader>("if");
  auto r21 = ::testing::NiceMock<MockReader>("import");
  auto r22 = ::testing::NiceMock<MockReader>("in");
  auto r23 = ::testing::NiceMock<MockReader>("init");
  auto r24 = ::testing::NiceMock<MockReader>("interface");
  auto r25 = ::testing::NiceMock<MockReader>("is");
  auto r26 = ::testing::NiceMock<MockReader>("loop");
  auto r27 = ::testing::NiceMock<MockReader>("main");
  auto r28 = ::testing::NiceMock<MockReader>("match");
  auto r29 = ::testing::NiceMock<MockReader>("mut");
  auto r30 = ::testing::NiceMock<MockReader>("new");
  auto r31 = ::testing::NiceMock<MockReader>("nil");
  auto r32 = ::testing::NiceMock<MockReader>("obj");
  auto r33 = ::testing::NiceMock<MockReader>("op");
  auto r34 = ::testing::NiceMock<MockReader>("override");
  auto r35 = ::testing::NiceMock<MockReader>("priv");
  auto r36 = ::testing::NiceMock<MockReader>("prot");
  auto r37 = ::testing::NiceMock<MockReader>("pub");
  auto r38 = ::testing::NiceMock<MockReader>("return");
  auto r39 = ::testing::NiceMock<MockReader>("static");
  auto r40 = ::testing::NiceMock<MockReader>("super");
  auto r41 = ::testing::NiceMock<MockReader>("this");
  auto r42 = ::testing::NiceMock<MockReader>("throw");
  auto r43 = ::testing::NiceMock<MockReader>("true");
  auto r44 = ::testing::NiceMock<MockReader>("try");
  auto r45 = ::testing::NiceMock<MockReader>("type");
  auto r46 = ::testing::NiceMock<MockReader>("union");

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

  EXPECT_EQ(l1.next().str(), "KW_AS(1:1-1:3): as");
  EXPECT_EQ(l2.next().str(), "KW_ASYNC(1:1-1:6): async");
  EXPECT_EQ(l3.next().str(), "KW_AWAIT(1:1-1:6): await");
  EXPECT_EQ(l4.next().str(), "KW_BREAK(1:1-1:6): break");
  EXPECT_EQ(l5.next().str(), "KW_CASE(1:1-1:5): case");
  EXPECT_EQ(l6.next().str(), "KW_CATCH(1:1-1:6): catch");
  EXPECT_EQ(l7.next().str(), "KW_CLASS(1:1-1:6): class");
  EXPECT_EQ(l8.next().str(), "KW_CONST(1:1-1:6): const");
  EXPECT_EQ(l9.next().str(), "KW_CONTINUE(1:1-1:9): continue");
  EXPECT_EQ(l10.next().str(), "KW_DEFAULT(1:1-1:8): default");
  EXPECT_EQ(l11.next().str(), "KW_DEINIT(1:1-1:7): deinit");
  EXPECT_EQ(l12.next().str(), "KW_ELIF(1:1-1:5): elif");
  EXPECT_EQ(l13.next().str(), "KW_ELSE(1:1-1:5): else");
  EXPECT_EQ(l14.next().str(), "KW_ENUM(1:1-1:5): enum");
  EXPECT_EQ(l15.next().str(), "KW_EXPORT(1:1-1:7): export");
  EXPECT_EQ(l16.next().str(), "KW_FALLTHROUGH(1:1-1:12): fallthrough");
  EXPECT_EQ(l17.next().str(), "KW_FALSE(1:1-1:6): false");
  EXPECT_EQ(l18.next().str(), "KW_FN(1:1-1:3): fn");
  EXPECT_EQ(l19.next().str(), "KW_FROM(1:1-1:5): from");
  EXPECT_EQ(l20.next().str(), "KW_IF(1:1-1:3): if");
  EXPECT_EQ(l21.next().str(), "KW_IMPORT(1:1-1:7): import");
  EXPECT_EQ(l22.next().str(), "KW_IN(1:1-1:3): in");
  EXPECT_EQ(l23.next().str(), "KW_INIT(1:1-1:5): init");
  EXPECT_EQ(l24.next().str(), "KW_INTERFACE(1:1-1:10): interface");
  EXPECT_EQ(l25.next().str(), "KW_IS(1:1-1:3): is");
  EXPECT_EQ(l26.next().str(), "KW_LOOP(1:1-1:5): loop");
  EXPECT_EQ(l27.next().str(), "KW_MAIN(1:1-1:5): main");
  EXPECT_EQ(l28.next().str(), "KW_MATCH(1:1-1:6): match");
  EXPECT_EQ(l29.next().str(), "KW_MUT(1:1-1:4): mut");
  EXPECT_EQ(l30.next().str(), "KW_NEW(1:1-1:4): new");
  EXPECT_EQ(l31.next().str(), "KW_NIL(1:1-1:4): nil");
  EXPECT_EQ(l32.next().str(), "KW_OBJ(1:1-1:4): obj");
  EXPECT_EQ(l33.next().str(), "KW_OP(1:1-1:3): op");
  EXPECT_EQ(l34.next().str(), "KW_OVERRIDE(1:1-1:9): override");
  EXPECT_EQ(l35.next().str(), "KW_PRIV(1:1-1:5): priv");
  EXPECT_EQ(l36.next().str(), "KW_PROT(1:1-1:5): prot");
  EXPECT_EQ(l37.next().str(), "KW_PUB(1:1-1:4): pub");
  EXPECT_EQ(l38.next().str(), "KW_RETURN(1:1-1:7): return");
  EXPECT_EQ(l39.next().str(), "KW_STATIC(1:1-1:7): static");
  EXPECT_EQ(l40.next().str(), "KW_SUPER(1:1-1:6): super");
  EXPECT_EQ(l41.next().str(), "KW_THIS(1:1-1:5): this");
  EXPECT_EQ(l42.next().str(), "KW_THROW(1:1-1:6): throw");
  EXPECT_EQ(l43.next().str(), "KW_TRUE(1:1-1:5): true");
  EXPECT_EQ(l44.next().str(), "KW_TRY(1:1-1:4): try");
  EXPECT_EQ(l45.next().str(), "KW_TYPE(1:1-1:5): type");
  EXPECT_EQ(l46.next().str(), "KW_UNION(1:1-1:6): union");
}

TEST(LexerTest, LexKeywordsWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(
    " as async await break case catch class const continue default deinit elif else enum export fallthrough false fn"
    " from if import in init interface is loop main match mut new nil obj op override priv prot pub return static"
    " super this throw true try type union "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "KW_AS(1:2-1:4): as");
  EXPECT_EQ(lexer.next().str(), "KW_ASYNC(1:5-1:10): async");
  EXPECT_EQ(lexer.next().str(), "KW_AWAIT(1:11-1:16): await");
  EXPECT_EQ(lexer.next().str(), "KW_BREAK(1:17-1:22): break");
  EXPECT_EQ(lexer.next().str(), "KW_CASE(1:23-1:27): case");
  EXPECT_EQ(lexer.next().str(), "KW_CATCH(1:28-1:33): catch");
  EXPECT_EQ(lexer.next().str(), "KW_CLASS(1:34-1:39): class");
  EXPECT_EQ(lexer.next().str(), "KW_CONST(1:40-1:45): const");
  EXPECT_EQ(lexer.next().str(), "KW_CONTINUE(1:46-1:54): continue");
  EXPECT_EQ(lexer.next().str(), "KW_DEFAULT(1:55-1:62): default");
  EXPECT_EQ(lexer.next().str(), "KW_DEINIT(1:63-1:69): deinit");
  EXPECT_EQ(lexer.next().str(), "KW_ELIF(1:70-1:74): elif");
  EXPECT_EQ(lexer.next().str(), "KW_ELSE(1:75-1:79): else");
  EXPECT_EQ(lexer.next().str(), "KW_ENUM(1:80-1:84): enum");
  EXPECT_EQ(lexer.next().str(), "KW_EXPORT(1:85-1:91): export");
  EXPECT_EQ(lexer.next().str(), "KW_FALLTHROUGH(1:92-1:103): fallthrough");
  EXPECT_EQ(lexer.next().str(), "KW_FALSE(1:104-1:109): false");
  EXPECT_EQ(lexer.next().str(), "KW_FN(1:110-1:112): fn");
  EXPECT_EQ(lexer.next().str(), "KW_FROM(1:113-1:117): from");
  EXPECT_EQ(lexer.next().str(), "KW_IF(1:118-1:120): if");
  EXPECT_EQ(lexer.next().str(), "KW_IMPORT(1:121-1:127): import");
  EXPECT_EQ(lexer.next().str(), "KW_IN(1:128-1:130): in");
  EXPECT_EQ(lexer.next().str(), "KW_INIT(1:131-1:135): init");
  EXPECT_EQ(lexer.next().str(), "KW_INTERFACE(1:136-1:145): interface");
  EXPECT_EQ(lexer.next().str(), "KW_IS(1:146-1:148): is");
  EXPECT_EQ(lexer.next().str(), "KW_LOOP(1:149-1:153): loop");
  EXPECT_EQ(lexer.next().str(), "KW_MAIN(1:154-1:158): main");
  EXPECT_EQ(lexer.next().str(), "KW_MATCH(1:159-1:164): match");
  EXPECT_EQ(lexer.next().str(), "KW_MUT(1:165-1:168): mut");
  EXPECT_EQ(lexer.next().str(), "KW_NEW(1:169-1:172): new");
  EXPECT_EQ(lexer.next().str(), "KW_NIL(1:173-1:176): nil");
  EXPECT_EQ(lexer.next().str(), "KW_OBJ(1:177-1:180): obj");
  EXPECT_EQ(lexer.next().str(), "KW_OP(1:181-1:183): op");
  EXPECT_EQ(lexer.next().str(), "KW_OVERRIDE(1:184-1:192): override");
  EXPECT_EQ(lexer.next().str(), "KW_PRIV(1:193-1:197): priv");
  EXPECT_EQ(lexer.next().str(), "KW_PROT(1:198-1:202): prot");
  EXPECT_EQ(lexer.next().str(), "KW_PUB(1:203-1:206): pub");
  EXPECT_EQ(lexer.next().str(), "KW_RETURN(1:207-1:213): return");
  EXPECT_EQ(lexer.next().str(), "KW_STATIC(1:214-1:220): static");
  EXPECT_EQ(lexer.next().str(), "KW_SUPER(1:221-1:226): super");
  EXPECT_EQ(lexer.next().str(), "KW_THIS(1:227-1:231): this");
  EXPECT_EQ(lexer.next().str(), "KW_THROW(1:232-1:237): throw");
  EXPECT_EQ(lexer.next().str(), "KW_TRUE(1:238-1:242): true");
  EXPECT_EQ(lexer.next().str(), "KW_TRY(1:243-1:246): try");
  EXPECT_EQ(lexer.next().str(), "KW_TYPE(1:247-1:251): type");
  EXPECT_EQ(lexer.next().str(), "KW_UNION(1:252-1:257): union");
  EXPECT_EQ(lexer.next().str(), "EOF(1:258-1:258)");
}

TEST(LexerTest, LexKeywordsAsIdentifiers) {
  auto reader = ::testing::NiceMock<MockReader>(
    " asm asynchronously awaited breakpoint casein catching classic constant continues defaulted deinitialized elifffy"
    " elsewhere enumeration exported fallthroughout falser fname fromental iffy importing inside initialization"
    " interfaced isle looped maintain matching mutable newest nilled object operation overriding private protected"
    " public returned statically superb thistle throwing trues tryout typed unions "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "ID(1:2-1:5): asm");
  EXPECT_EQ(lexer.next().str(), "ID(1:6-1:20): asynchronously");
  EXPECT_EQ(lexer.next().str(), "ID(1:21-1:28): awaited");
  EXPECT_EQ(lexer.next().str(), "ID(1:29-1:39): breakpoint");
  EXPECT_EQ(lexer.next().str(), "ID(1:40-1:46): casein");
  EXPECT_EQ(lexer.next().str(), "ID(1:47-1:55): catching");
  EXPECT_EQ(lexer.next().str(), "ID(1:56-1:63): classic");
  EXPECT_EQ(lexer.next().str(), "ID(1:64-1:72): constant");
  EXPECT_EQ(lexer.next().str(), "ID(1:73-1:82): continues");
  EXPECT_EQ(lexer.next().str(), "ID(1:83-1:92): defaulted");
  EXPECT_EQ(lexer.next().str(), "ID(1:93-1:106): deinitialized");
  EXPECT_EQ(lexer.next().str(), "ID(1:107-1:114): elifffy");
  EXPECT_EQ(lexer.next().str(), "ID(1:115-1:124): elsewhere");
  EXPECT_EQ(lexer.next().str(), "ID(1:125-1:136): enumeration");
  EXPECT_EQ(lexer.next().str(), "ID(1:137-1:145): exported");
  EXPECT_EQ(lexer.next().str(), "ID(1:146-1:160): fallthroughout");
  EXPECT_EQ(lexer.next().str(), "ID(1:161-1:167): falser");
  EXPECT_EQ(lexer.next().str(), "ID(1:168-1:173): fname");
  EXPECT_EQ(lexer.next().str(), "ID(1:174-1:183): fromental");
  EXPECT_EQ(lexer.next().str(), "ID(1:184-1:188): iffy");
  EXPECT_EQ(lexer.next().str(), "ID(1:189-1:198): importing");
  EXPECT_EQ(lexer.next().str(), "ID(1:199-1:205): inside");
  EXPECT_EQ(lexer.next().str(), "ID(1:206-1:220): initialization");
  EXPECT_EQ(lexer.next().str(), "ID(1:221-1:231): interfaced");
  EXPECT_EQ(lexer.next().str(), "ID(1:232-1:236): isle");
  EXPECT_EQ(lexer.next().str(), "ID(1:237-1:243): looped");
  EXPECT_EQ(lexer.next().str(), "ID(1:244-1:252): maintain");
  EXPECT_EQ(lexer.next().str(), "ID(1:253-1:261): matching");
  EXPECT_EQ(lexer.next().str(), "ID(1:262-1:269): mutable");
  EXPECT_EQ(lexer.next().str(), "ID(1:270-1:276): newest");
  EXPECT_EQ(lexer.next().str(), "ID(1:277-1:283): nilled");
  EXPECT_EQ(lexer.next().str(), "ID(1:284-1:290): object");
  EXPECT_EQ(lexer.next().str(), "ID(1:291-1:300): operation");
  EXPECT_EQ(lexer.next().str(), "ID(1:301-1:311): overriding");
  EXPECT_EQ(lexer.next().str(), "ID(1:312-1:319): private");
  EXPECT_EQ(lexer.next().str(), "ID(1:320-1:329): protected");
  EXPECT_EQ(lexer.next().str(), "ID(1:330-1:336): public");
  EXPECT_EQ(lexer.next().str(), "ID(1:337-1:345): returned");
  EXPECT_EQ(lexer.next().str(), "ID(1:346-1:356): statically");
  EXPECT_EQ(lexer.next().str(), "ID(1:357-1:363): superb");
  EXPECT_EQ(lexer.next().str(), "ID(1:364-1:371): thistle");
  EXPECT_EQ(lexer.next().str(), "ID(1:372-1:380): throwing");
  EXPECT_EQ(lexer.next().str(), "ID(1:381-1:386): trues");
  EXPECT_EQ(lexer.next().str(), "ID(1:387-1:393): tryout");
  EXPECT_EQ(lexer.next().str(), "ID(1:394-1:399): typed");
  EXPECT_EQ(lexer.next().str(), "ID(1:400-1:406): unions");
}

TEST(LexerTest, LexIdentifier) {
  auto r1 = ::testing::NiceMock<MockReader>("a");
  auto r2 = ::testing::NiceMock<MockReader>("A");
  auto r3 = ::testing::NiceMock<MockReader>("_");
  auto r4 = ::testing::NiceMock<MockReader>("number1");
  auto r5 = ::testing::NiceMock<MockReader>("ANYTHING");
  auto r6 = ::testing::NiceMock<MockReader>("__I1_D2__");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);

  EXPECT_EQ(l1.next().str(), "ID(1:1-1:2): a");
  EXPECT_EQ(l2.next().str(), "ID(1:1-1:2): A");
  EXPECT_EQ(l3.next().str(), "ID(1:1-1:2): _");
  EXPECT_EQ(l4.next().str(), "ID(1:1-1:8): number1");
  EXPECT_EQ(l5.next().str(), "ID(1:1-1:9): ANYTHING");
  EXPECT_EQ(l6.next().str(), "ID(1:1-1:10): __I1_D2__");
}

TEST(LexerTest, LexIdentifierWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(" a A _ number1 ANYTHING __I1_D2__ ");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "ID(1:2-1:3): a");
  EXPECT_EQ(lexer.next().str(), "ID(1:4-1:5): A");
  EXPECT_EQ(lexer.next().str(), "ID(1:6-1:7): _");
  EXPECT_EQ(lexer.next().str(), "ID(1:8-1:15): number1");
  EXPECT_EQ(lexer.next().str(), "ID(1:16-1:24): ANYTHING");
  EXPECT_EQ(lexer.next().str(), "ID(1:25-1:34): __I1_D2__");
  EXPECT_EQ(lexer.next().str(), "EOF(1:35-1:35)");
}

TEST(LexerTest, LexLitChar) {
  auto reader = ::testing::NiceMock<MockReader>(R"(' ''!''A''a''9''\n''\"''\'''\\')");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:1-1:4): ' '");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:4-1:7): '!'");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:7-1:10): 'A'");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:10-1:13): 'a'");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:13-1:16): '9'");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:16-1:20): '\\n')");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:20-1:24): '\"')");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:24-1:28): '\'')");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:28-1:32): '\\')");
}

TEST(LexerTest, LexLitCharWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(R"( ' ' '!' 'A' 'a' '9' '\n' '\"' '\'' '\\' )");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:2-1:5): ' '");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:6-1:9): '!'");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:10-1:13): 'A'");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:14-1:17): 'a'");
  EXPECT_EQ(lexer.next().str(), "LIT_CHAR(1:18-1:21): '9'");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:22-1:26): '\\n')");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:27-1:31): '\"')");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:32-1:36): '\'')");
  EXPECT_EQ(lexer.next().str(), R"(LIT_CHAR(1:37-1:41): '\\')");
  EXPECT_EQ(lexer.next().str(), "EOF(1:42-1:42)");
}

TEST(LexerTest, LexLitCharEof) {
  auto r1 = ::testing::NiceMock<MockReader>("' '");
  auto r2 = ::testing::NiceMock<MockReader>("'!'");
  auto r3 = ::testing::NiceMock<MockReader>("'A'");
  auto r4 = ::testing::NiceMock<MockReader>("'a'");
  auto r5 = ::testing::NiceMock<MockReader>("'9'");
  auto r6 = ::testing::NiceMock<MockReader>(R"('\n')");
  auto r7 = ::testing::NiceMock<MockReader>(R"('\"')");
  auto r8 = ::testing::NiceMock<MockReader>(R"('\'')");
  auto r9 = ::testing::NiceMock<MockReader>(R"('\\')");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);
  auto l6 = Lexer(&r6);
  auto l7 = Lexer(&r7);
  auto l8 = Lexer(&r8);
  auto l9 = Lexer(&r9);

  EXPECT_EQ(l1.next().str(), "LIT_CHAR(1:1-1:4): ' '");
  EXPECT_EQ(l2.next().str(), "LIT_CHAR(1:1-1:4): '!'");
  EXPECT_EQ(l3.next().str(), "LIT_CHAR(1:1-1:4): 'A'");
  EXPECT_EQ(l4.next().str(), "LIT_CHAR(1:1-1:4): 'a'");
  EXPECT_EQ(l5.next().str(), "LIT_CHAR(1:1-1:4): '9'");
  EXPECT_EQ(l6.next().str(), R"(LIT_CHAR(1:1-1:5): '\\n')");
  EXPECT_EQ(l7.next().str(), R"(LIT_CHAR(1:1-1:5): '\"')");
  EXPECT_EQ(l8.next().str(), R"(LIT_CHAR(1:1-1:5): '\'')");
  EXPECT_EQ(l9.next().str(), R"(LIT_CHAR(1:1-1:5): '\\')");
}

TEST(LexerTest, ThrowsOnNotClosedLitChar) {
  auto r1 = ::testing::NiceMock<MockReader>("'");
  auto r2 = ::testing::NiceMock<MockReader>(R"('a)");
  auto r3 = ::testing::NiceMock<MockReader>(R"('\)");
  auto r4 = ::testing::NiceMock<MockReader>(R"('\n)");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0002 + "\n  1 | '\n    | ^").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0002 + "\n  1 | 'a\n    | ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l3.next();
  }, LexerError, (std::string("/test:1:1: ") + E0002 + "\n  1 | '\\\n    | ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l4.next();
  }, LexerError, (std::string("/test:1:1: ") + E0002 + "\n  1 | '\\n\n    | ^~~").c_str());
}

TEST(LexerTest, ThrowsOnEmptyLitChar) {
  auto reader = ::testing::NiceMock<MockReader>("''");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, (std::string("/test:1:1: ") + E0004 + "\n  1 | ''\n    | ^~").c_str());
}

TEST(LexerTest, ThrowsOnLitCharIllegalEscSeq) {
  auto r1 = ::testing::NiceMock<MockReader>(R"('\m)");
  auto r2 = ::testing::NiceMock<MockReader>(R"('\m')");
  auto r3 = ::testing::NiceMock<MockReader>(R"('\ma)");
  auto r4 = ::testing::NiceMock<MockReader>(R"('\ma')");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:2: ") + E0005 + "\n  1 | '\\m\n    |  ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:2: ") + E0005 + "\n  1 | '\\m'\n    |  ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l3.next();
  }, LexerError, (std::string("/test:1:2: ") + E0005 + "\n  1 | '\\ma\n    |  ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l4.next();
  }, LexerError, (std::string("/test:1:2: ") + E0005 + "\n  1 | '\\ma'\n    |  ^~").c_str());
}

TEST(LexerTest, ThrowsOnLitCharTooManyCharacters) {
  auto r1 = ::testing::NiceMock<MockReader>(R"('ch)");
  auto r2 = ::testing::NiceMock<MockReader>(R"('char')");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0006 + "\n  1 | 'ch\n    | ^~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0006 + "\n  1 | 'char'\n    | ^~~~~~").c_str());
}

TEST(LexerTest, LexLitFloat) {
  auto r1 = ::testing::NiceMock<MockReader>("0E0");
  auto r2 = ::testing::NiceMock<MockReader>("0E1");
  auto r3 = ::testing::NiceMock<MockReader>("1e308");
  auto r4 = ::testing::NiceMock<MockReader>("1e+0");
  auto r5 = ::testing::NiceMock<MockReader>("18446744073709551615e+1");
  auto r6 = ::testing::NiceMock<MockReader>("18446744073709551615e-308");
  auto r7 = ::testing::NiceMock<MockReader>("0.E0");
  auto r8 = ::testing::NiceMock<MockReader>("0.E1");
  auto r9 = ::testing::NiceMock<MockReader>("1.e308");
  auto r10 = ::testing::NiceMock<MockReader>("1.e+0");
  auto r11 = ::testing::NiceMock<MockReader>("18446744073709551615.e+1");
  auto r12 = ::testing::NiceMock<MockReader>("18446744073709551615.e-308");
  auto r13 = ::testing::NiceMock<MockReader>("0.0");
  auto r14 = ::testing::NiceMock<MockReader>("1.1");
  auto r15 = ::testing::NiceMock<MockReader>("18446744073709551615.18446744073709551615");
  auto r16 = ::testing::NiceMock<MockReader>("0.0E0");
  auto r17 = ::testing::NiceMock<MockReader>("0.0E1");
  auto r18 = ::testing::NiceMock<MockReader>("1.1e308");
  auto r19 = ::testing::NiceMock<MockReader>("1.1E+0");
  auto r20 = ::testing::NiceMock<MockReader>("18446744073709551615.18446744073709551615E+1");
  auto r21 = ::testing::NiceMock<MockReader>("18446744073709551615.18446744073709551615e-308");

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

  EXPECT_EQ(l1.next().str(), "LIT_FLOAT(1:1-1:4): 0E0");
  EXPECT_EQ(l2.next().str(), "LIT_FLOAT(1:1-1:4): 0E1");
  EXPECT_EQ(l3.next().str(), "LIT_FLOAT(1:1-1:6): 1e308");
  EXPECT_EQ(l4.next().str(), "LIT_FLOAT(1:1-1:5): 1e+0");
  EXPECT_EQ(l5.next().str(), "LIT_FLOAT(1:1-1:24): 18446744073709551615e+1");
  EXPECT_EQ(l6.next().str(), "LIT_FLOAT(1:1-1:26): 18446744073709551615e-308");
  EXPECT_EQ(l7.next().str(), "LIT_FLOAT(1:1-1:5): 0.E0");
  EXPECT_EQ(l8.next().str(), "LIT_FLOAT(1:1-1:5): 0.E1");
  EXPECT_EQ(l9.next().str(), "LIT_FLOAT(1:1-1:7): 1.e308");
  EXPECT_EQ(l10.next().str(), "LIT_FLOAT(1:1-1:6): 1.e+0");
  EXPECT_EQ(l11.next().str(), "LIT_FLOAT(1:1-1:25): 18446744073709551615.e+1");
  EXPECT_EQ(l12.next().str(), "LIT_FLOAT(1:1-1:27): 18446744073709551615.e-308");
  EXPECT_EQ(l13.next().str(), "LIT_FLOAT(1:1-1:4): 0.0");
  EXPECT_EQ(l14.next().str(), "LIT_FLOAT(1:1-1:4): 1.1");
  EXPECT_EQ(l15.next().str(), "LIT_FLOAT(1:1-1:42): 18446744073709551615.18446744073709551615");
  EXPECT_EQ(l16.next().str(), "LIT_FLOAT(1:1-1:6): 0.0E0");
  EXPECT_EQ(l17.next().str(), "LIT_FLOAT(1:1-1:6): 0.0E1");
  EXPECT_EQ(l18.next().str(), "LIT_FLOAT(1:1-1:8): 1.1e308");
  EXPECT_EQ(l19.next().str(), "LIT_FLOAT(1:1-1:7): 1.1E+0");
  EXPECT_EQ(l20.next().str(), "LIT_FLOAT(1:1-1:45): 18446744073709551615.18446744073709551615E+1");
  EXPECT_EQ(l21.next().str(), "LIT_FLOAT(1:1-1:47): 18446744073709551615.18446744073709551615e-308");
}

TEST(LexerTest, LexLitFloatWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(
    " 0E0 0E1 1e308 1e+0 18446744073709551615e+1 18446744073709551615e-308"
    " 0.E0 0.E1 1.e308 1.e+0 18446744073709551615.e+1 18446744073709551615.e-308"
    " 0.0 1.1 18446744073709551615.18446744073709551615"
    " 0.0E0 0.0E1 1.1e308 1.1E+0"
    " 18446744073709551615.18446744073709551615E+1 18446744073709551615.18446744073709551615e-308 "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:2-1:5): 0E0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:6-1:9): 0E1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:10-1:15): 1e308");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:16-1:20): 1e+0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:21-1:44): 18446744073709551615e+1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:45-1:70): 18446744073709551615e-308");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:71-1:75): 0.E0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:76-1:80): 0.E1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:81-1:87): 1.e308");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:88-1:93): 1.e+0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:94-1:118): 18446744073709551615.e+1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:119-1:145): 18446744073709551615.e-308");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:146-1:149): 0.0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:150-1:153): 1.1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:154-1:195): 18446744073709551615.18446744073709551615");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:196-1:201): 0.0E0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:202-1:207): 0.0E1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:208-1:215): 1.1e308");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:216-1:222): 1.1E+0");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:223-1:267): 18446744073709551615.18446744073709551615E+1");
  EXPECT_EQ(lexer.next().str(), "LIT_FLOAT(1:268-1:314): 18446744073709551615.18446744073709551615e-308");
  EXPECT_EQ(lexer.next().str(), "EOF(1:315-1:315)");
}

TEST(LexerTest, ThrowsOnInvalidLitFloat) {
  auto r1 = ::testing::NiceMock<MockReader>(R"(1234.)");
  auto r2 = ::testing::NiceMock<MockReader>(R"(1234.a)");
  auto r3 = ::testing::NiceMock<MockReader>(R"(1234.aZ)");
  auto r4 = ::testing::NiceMock<MockReader>(R"(1234.0a)");
  auto r5 = ::testing::NiceMock<MockReader>(R"(1234.1aZ)");
  auto r6 = ::testing::NiceMock<MockReader>(R"(1234e)");
  auto r7 = ::testing::NiceMock<MockReader>(R"(1234e+)");
  auto r8 = ::testing::NiceMock<MockReader>(R"(1234e-)");
  auto r9 = ::testing::NiceMock<MockReader>(R"(1234e+a1)");
  auto r10 = ::testing::NiceMock<MockReader>(R"(1234eZ)");
  auto r11 = ::testing::NiceMock<MockReader>(R"(1234eZa)");
  auto r12 = ::testing::NiceMock<MockReader>(R"(1234e5e6)");
  auto r13 = ::testing::NiceMock<MockReader>(R"(1234e))");
  auto r14 = ::testing::NiceMock<MockReader>(R"(1234e+))");
  auto r15 = ::testing::NiceMock<MockReader>(R"(1234e-h))");

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

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0012 + "\n  1 | 1234.\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0012 + "\n  1 | 1234.a\n    | ^~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l3.next();
  }, LexerError, (std::string("/test:1:1: ") + E0012 + "\n  1 | 1234.aZ\n    | ^~~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l4.next();
  }, LexerError, (std::string("/test:1:1: ") + E0012 + "\n  1 | 1234.0a\n    | ^~~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l5.next();
  }, LexerError, (std::string("/test:1:1: ") + E0012 + "\n  1 | 1234.1aZ\n    | ^~~~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l6.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e\n    |     ^").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l7.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e+\n    |     ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l8.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e-\n    |     ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l9.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e+a1\n    |     ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l10.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234eZ\n    |     ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l11.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234eZa\n    |     ^~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l12.next();
  }, LexerError, (std::string("/test:1:1: ") + E0012 + "\n  1 | 1234e5e6\n    | ^~~~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l13.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e)\n    |     ^").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l14.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e+)\n    |     ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l15.next();
  }, LexerError, (std::string("/test:1:5: ") + E0013 + "\n  1 | 1234e-h)\n    |     ^~~").c_str());
}

TEST(LexerTest, ThrowsOnNonDecLitFloat) {
  auto r1 = ::testing::NiceMock<MockReader>("0b1.0");
  auto r2 = ::testing::NiceMock<MockReader>("0xa.0");
  auto r3 = ::testing::NiceMock<MockReader>("0o1.0");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0014 + "\n  1 | 0b1.0\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0015 + "\n  1 | 0xa.0\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l3.next();
  }, LexerError, (std::string("/test:1:1: ") + E0016 + "\n  1 | 0o1.0\n    | ^~~~~").c_str());
}

TEST(LexerTest, LexLitInteger) {
  auto reader = ::testing::NiceMock<MockReader>(
    "..0B0..0b1..0B00000000000000000000000000000000..0b11111111111111111111111111111111"
    "..0..1..18446744073709551615"
    "..0X0..0x9..0XA..0xf..0X9999999999999999..0xffffffffffffffff"
    "..0O0..0o7..0O000000000000000000000..0o777777777777777777777.."
  );

  auto lexer = Lexer(&reader);

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:3-1:6): 0B0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:8-1:11): 0b1");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:13-1:47): 0B00000000000000000000000000000000");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:49-1:83): 0b11111111111111111111111111111111");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_DEC(1:85-1:86): 0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_DEC(1:88-1:89): 1");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_DEC(1:91-1:111): 18446744073709551615");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:113-1:116): 0X0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:118-1:121): 0x9");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:123-1:126): 0XA");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:128-1:131): 0xf");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:133-1:151): 0X9999999999999999");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:153-1:171): 0xffffffffffffffff");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:173-1:176): 0O0");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:178-1:181): 0o7");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:183-1:206): 0O000000000000000000000");

  lexer.next();
  lexer.next();
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:208-1:231): 0o777777777777777777777");
}

TEST(LexerTest, LexLitIntegerWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(
    " 0B0 0b1 0B00000000000000000000000000000000 0b11111111111111111111111111111111"
    " 0 1 18446744073709551615"
    " 0X0 0x9 0XA 0xf 0X9999999999999999 0xffffffffffffffff"
    " 0O0 0o7 0O000000000000000000000 0o777777777777777777777 "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:2-1:5): 0B0");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:6-1:9): 0b1");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:10-1:44): 0B00000000000000000000000000000000");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_BIN(1:45-1:79): 0b11111111111111111111111111111111");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_DEC(1:80-1:81): 0");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_DEC(1:82-1:83): 1");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_DEC(1:84-1:104): 18446744073709551615");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:105-1:108): 0X0");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:109-1:112): 0x9");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:113-1:116): 0XA");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:117-1:120): 0xf");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:121-1:139): 0X9999999999999999");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_HEX(1:140-1:158): 0xffffffffffffffff");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:159-1:162): 0O0");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:163-1:166): 0o7");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:167-1:190): 0O000000000000000000000");
  EXPECT_EQ(lexer.next().str(), "LIT_INT_OCT(1:191-1:214): 0o777777777777777777777");
  EXPECT_EQ(lexer.next().str(), "EOF(1:215-1:215)");
}

TEST(LexerTest, LexLitIntegerEof) {
  auto r1 = ::testing::NiceMock<MockReader>("0B0");
  auto r2 = ::testing::NiceMock<MockReader>("0b1");
  auto r3 = ::testing::NiceMock<MockReader>("0B00000000000000000000000000000000");
  auto r4 = ::testing::NiceMock<MockReader>("0b11111111111111111111111111111111");
  auto r5 = ::testing::NiceMock<MockReader>("0");
  auto r6 = ::testing::NiceMock<MockReader>("1");
  auto r7 = ::testing::NiceMock<MockReader>("18446744073709551615");
  auto r8 = ::testing::NiceMock<MockReader>("0X0");
  auto r9 = ::testing::NiceMock<MockReader>("0x9");
  auto r10 = ::testing::NiceMock<MockReader>("0XA");
  auto r11 = ::testing::NiceMock<MockReader>("0xf");
  auto r12 = ::testing::NiceMock<MockReader>("0X9999999999999999");
  auto r13 = ::testing::NiceMock<MockReader>("0xffffffffffffffff");
  auto r14 = ::testing::NiceMock<MockReader>("0O0");
  auto r15 = ::testing::NiceMock<MockReader>("0o7");
  auto r16 = ::testing::NiceMock<MockReader>("0O000000000000000000000");
  auto r17 = ::testing::NiceMock<MockReader>("0o777777777777777777777");

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

  EXPECT_EQ(l1.next().str(), "LIT_INT_BIN(1:1-1:4): 0B0");
  EXPECT_EQ(l2.next().str(), "LIT_INT_BIN(1:1-1:4): 0b1");
  EXPECT_EQ(l3.next().str(), "LIT_INT_BIN(1:1-1:35): 0B00000000000000000000000000000000");
  EXPECT_EQ(l4.next().str(), "LIT_INT_BIN(1:1-1:35): 0b11111111111111111111111111111111");
  EXPECT_EQ(l5.next().str(), "LIT_INT_DEC(1:1-1:2): 0");
  EXPECT_EQ(l6.next().str(), "LIT_INT_DEC(1:1-1:2): 1");
  EXPECT_EQ(l7.next().str(), "LIT_INT_DEC(1:1-1:21): 18446744073709551615");
  EXPECT_EQ(l8.next().str(), "LIT_INT_HEX(1:1-1:4): 0X0");
  EXPECT_EQ(l9.next().str(), "LIT_INT_HEX(1:1-1:4): 0x9");
  EXPECT_EQ(l10.next().str(), "LIT_INT_HEX(1:1-1:4): 0XA");
  EXPECT_EQ(l11.next().str(), "LIT_INT_HEX(1:1-1:4): 0xf");
  EXPECT_EQ(l12.next().str(), "LIT_INT_HEX(1:1-1:19): 0X9999999999999999");
  EXPECT_EQ(l13.next().str(), "LIT_INT_HEX(1:1-1:19): 0xffffffffffffffff");
  EXPECT_EQ(l14.next().str(), "LIT_INT_OCT(1:1-1:4): 0O0");
  EXPECT_EQ(l15.next().str(), "LIT_INT_OCT(1:1-1:4): 0o7");
  EXPECT_EQ(l16.next().str(), "LIT_INT_OCT(1:1-1:24): 0O000000000000000000000");
  EXPECT_EQ(l17.next().str(), "LIT_INT_OCT(1:1-1:24): 0o777777777777777777777");
}

TEST(LexerTest, ThrowsOnLitIntegerWithLeadingZero) {
  auto r1 = ::testing::NiceMock<MockReader>("04");
  auto r2 = ::testing::NiceMock<MockReader>("0400e0");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0007 + "\n  1 | 04\n    | ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0007 + "\n  1 | 0400e0\n    | ^~~~~~").c_str());
}

TEST(LexerTest, ThrowsOnInvalidLitInteger) {
  auto r1 = ::testing::NiceMock<MockReader>("0B");
  auto r2 = ::testing::NiceMock<MockReader>("0bG");
  auto r3 = ::testing::NiceMock<MockReader>("0bGz");
  auto r4 = ::testing::NiceMock<MockReader>("0b1g");
  auto r5 = ::testing::NiceMock<MockReader>("0b1gZ");
  auto r6 = ::testing::NiceMock<MockReader>("0b1b0");
  auto r7 = ::testing::NiceMock<MockReader>("1234g");
  auto r8 = ::testing::NiceMock<MockReader>("1234gZ");
  auto r9 = ::testing::NiceMock<MockReader>("1234g0a");
  auto r10 = ::testing::NiceMock<MockReader>("0x");
  auto r11 = ::testing::NiceMock<MockReader>("0xG");
  auto r12 = ::testing::NiceMock<MockReader>("0xGz");
  auto r13 = ::testing::NiceMock<MockReader>("0Xag");
  auto r14 = ::testing::NiceMock<MockReader>("0XagZ");
  auto r15 = ::testing::NiceMock<MockReader>("0x1x2");
  auto r16 = ::testing::NiceMock<MockReader>("0o");
  auto r17 = ::testing::NiceMock<MockReader>("0oG");
  auto r18 = ::testing::NiceMock<MockReader>("0oGz");
  auto r19 = ::testing::NiceMock<MockReader>("0O1g");
  auto r20 = ::testing::NiceMock<MockReader>("0O1gZ");
  auto r21 = ::testing::NiceMock<MockReader>("0o1o2");

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

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0008 + "\n  1 | 0B\n    | ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0008 + "\n  1 | 0bG\n    | ^~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l3.next();
  }, LexerError, (std::string("/test:1:1: ") + E0008 + "\n  1 | 0bGz\n    | ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l4.next();
  }, LexerError, (std::string("/test:1:1: ") + E0008 + "\n  1 | 0b1g\n    | ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l5.next();
  }, LexerError, (std::string("/test:1:1: ") + E0008 + "\n  1 | 0b1gZ\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l6.next();
  }, LexerError, (std::string("/test:1:1: ") + E0008 + "\n  1 | 0b1b0\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l7.next();
  }, LexerError, (std::string("/test:1:1: ") + E0009 + "\n  1 | 1234g\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l8.next();
  }, LexerError, (std::string("/test:1:1: ") + E0009 + "\n  1 | 1234gZ\n    | ^~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l9.next();
  }, LexerError, (std::string("/test:1:1: ") + E0009 + "\n  1 | 1234g0a\n    | ^~~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l10.next();
  }, LexerError, (std::string("/test:1:1: ") + E0010 + "\n  1 | 0x\n    | ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l11.next();
  }, LexerError, (std::string("/test:1:1: ") + E0010 + "\n  1 | 0xG\n    | ^~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l12.next();
  }, LexerError, (std::string("/test:1:1: ") + E0010 + "\n  1 | 0xGz\n    | ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l13.next();
  }, LexerError, (std::string("/test:1:1: ") + E0010 + "\n  1 | 0Xag\n    | ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l14.next();
  }, LexerError, (std::string("/test:1:1: ") + E0010 + "\n  1 | 0XagZ\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l15.next();
  }, LexerError, (std::string("/test:1:1: ") + E0010 + "\n  1 | 0x1x2\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l16.next();
  }, LexerError, (std::string("/test:1:1: ") + E0011 + "\n  1 | 0o\n    | ^~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l17.next();
  }, LexerError, (std::string("/test:1:1: ") + E0011 + "\n  1 | 0oG\n    | ^~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l18.next();
  }, LexerError, (std::string("/test:1:1: ") + E0011 + "\n  1 | 0oGz\n    | ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l19.next();
  }, LexerError, (std::string("/test:1:1: ") + E0011 + "\n  1 | 0O1g\n    | ^~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l20.next();
  }, LexerError, (std::string("/test:1:1: ") + E0011 + "\n  1 | 0O1gZ\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l21.next();
  }, LexerError, (std::string("/test:1:1: ") + E0011 + "\n  1 | 0o1o2\n    | ^~~~~").c_str());
}

TEST(LexerTest, LexLitStr) {
  auto reader = ::testing::NiceMock<MockReader>(
    R"("")"
    R"("test")"
    R"("Hello,\nWorld!")"
    R"("multiple \\n lines")"
    R"("\\0\\r\\t\"\'\\\0\r\t")"
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:1-1:3): "")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:3-1:9): "test")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:9-1:25): "Hello,\\nWorld!")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:25-1:45): "multiple \\\n lines")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:45-1:68): "\\\0\\\r\\\t\"\'\\\\0\\r\\t")");
}

TEST(LexerTest, LexLitStrWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(
    R"( "")"
    R"( "test")"
    R"( "Hello,\nWorld!")"
    R"( "multiple \\n lines")"
    R"( "\\0\\r\\t\"\'\\\0\r\t" )"
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:2-1:4): "")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:5-1:11): "test")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:12-1:28): "Hello,\\nWorld!")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:29-1:49): "multiple \\\n lines")");
  EXPECT_EQ(lexer.next().str(), R"(LIT_STR(1:50-1:73): "\\\0\\\r\\\t\"\'\\\\0\\r\\t")");
}

TEST(LexerTest, LexLitStrEof) {
  auto r1 = ::testing::NiceMock<MockReader>(R"("")");
  auto r2 = ::testing::NiceMock<MockReader>(R"("test")");
  auto r3 = ::testing::NiceMock<MockReader>(R"("Hello,\nWorld!")");
  auto r4 = ::testing::NiceMock<MockReader>(R"("multiple \\n lines")");
  auto r5 = ::testing::NiceMock<MockReader>(R"("\\0\\r\\t\"\'\\\0\r\t")");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);
  auto l5 = Lexer(&r5);

  EXPECT_EQ(l1.next().str(), R"(LIT_STR(1:1-1:3): "")");
  EXPECT_EQ(l2.next().str(), R"(LIT_STR(1:1-1:7): "test")");
  EXPECT_EQ(l3.next().str(), R"(LIT_STR(1:1-1:17): "Hello,\\nWorld!")");
  EXPECT_EQ(l4.next().str(), R"(LIT_STR(1:1-1:21): "multiple \\\n lines")");
  EXPECT_EQ(l5.next().str(), R"(LIT_STR(1:1-1:24): "\\\0\\\r\\\t\"\'\\\\0\\r\\t")");
}

TEST(LexerTest, ThrowsOnEmptyLitStr) {
  auto r1 = ::testing::NiceMock<MockReader>(R"(")");
  auto r2 = ::testing::NiceMock<MockReader>(R"("text)");
  auto r3 = ::testing::NiceMock<MockReader>(R"("text\)");
  auto r4 = ::testing::NiceMock<MockReader>(R"("text\")");

  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);
  auto l4 = Lexer(&r4);

  EXPECT_THROW_WITH_MESSAGE({
    l1.next();
  }, LexerError, (std::string("/test:1:1: ") + E0003 + "\n  1 | \"\n    | ^").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l2.next();
  }, LexerError, (std::string("/test:1:1: ") + E0003 + "\n  1 | \"text\n    | ^~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l3.next();
  }, LexerError, (std::string("/test:1:1: ") + E0003 + "\n  1 | \"text\\\n    | ^~~~~~").c_str());

  EXPECT_THROW_WITH_MESSAGE({
    l4.next();
  }, LexerError, (std::string("/test:1:1: ") + E0003 + "\n  1 | \"text\\\"\n    | ^~~~~~~").c_str());
}

TEST(LexerTest, ThrowsOnLitStrIllegalEscSeq) {
  auto reader = ::testing::NiceMock<MockReader>(R"("Hello, \m World!")");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, (std::string("/test:1:9: ") + E0005 + "\n  1 | \"Hello, \\m World!\"\n    |         ^~").c_str());
}
