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
  auto message = std::string("/test:1:1: ") + E0000 + "\n  1 | @\n    | ^";

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, message.c_str());
}

TEST(LexerTest, LexEof) {
  auto reader = ::testing::NiceMock<MockReader>("");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{0, 1, 0}, ReaderLocation{0, 1, 0}, ""}));
}

TEST(LexerTest, LexSingleWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(" ");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{1, 1, 1}, ReaderLocation{1, 1, 1}, ""}));
}

TEST(LexerTest, LexMultipleWhitespaces) {
  auto reader = ::testing::NiceMock<MockReader>(" \n\t");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{3, 2, 1}, ReaderLocation{3, 2, 1}, ""}));
}

TEST(LexerTest, LexEmptyBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/**/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{4, 1, 4}, ReaderLocation{4, 1, 4}, ""}));
}

TEST(LexerTest, LexAsteriskBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/***/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{5, 1, 5}, ReaderLocation{5, 1, 5}, ""}));
}

TEST(LexerTest, LexSingleBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Kendall*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{17, 1, 17}, ReaderLocation{17, 1, 17}, ""}));
}

TEST(LexerTest, LexMultipleBlockComments) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Kim*//*Hello Dream*//*Hello Stormi*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{44, 1, 44}, ReaderLocation{44, 1, 44}, ""}));
}

TEST(LexerTest, LexMultilineBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Bella\nHello Olivia\nHello Rosa*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{39, 3, 12}, ReaderLocation{39, 3, 12}, ""}));
}

TEST(LexerTest, ThrowsOnEmptyNotClosedBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*");
  auto lexer = Lexer(&reader);
  auto message = std::string("/test:1:1: ") + E0001 + "\n  1 | /*\n    | ^~";

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, message.c_str());
}

TEST(LexerTest, ThrowsOnNotClosedBlockComment) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello");
  auto lexer = Lexer(&reader);
  auto message = std::string("/test:1:1: ") + E0001 + "\n  1 | /*Hello\n    | ^~~~~~~";

  EXPECT_THROW_WITH_MESSAGE({
    lexer.next();
  }, LexerError, message.c_str());
}

TEST(LexerTest, LexEmptyLineCommentNoNewLine) {
  auto reader = ::testing::NiceMock<MockReader>("//");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{2, 1, 2}, ReaderLocation{2, 1, 2}, ""}));
}

TEST(LexerTest, LexEmptyLineComment) {
  auto reader = ::testing::NiceMock<MockReader>("//\n");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{3, 2, 0}, ReaderLocation{3, 2, 0}, ""}));
}

TEST(LexerTest, LexLineCommentNoNewLine) {
  auto reader = ::testing::NiceMock<MockReader>("//Hello Ariana");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{14, 1, 14}, ReaderLocation{14, 1, 14}, ""}));
}

TEST(LexerTest, LexLineComment) {
  auto reader = ::testing::NiceMock<MockReader>("//Hello Hailey\n");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{15, 2, 0}, ReaderLocation{15, 2, 0}, ""}));
}

TEST(LexerTest, LexWhitespacesCombinedWithComments) {
  auto reader = ::testing::NiceMock<MockReader>(" \t\r//Hello Mary\n/*Hello Linda*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{31, 2, 15}, ReaderLocation{31, 2, 15}, ""}));
}

TEST(LexerTest, LexWhitespaceBeforeToken) {
  auto reader = ::testing::NiceMock<MockReader>(" \t;");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_OP_SEMI, ReaderLocation{2, 1, 2}, ReaderLocation{3, 1, 3}, ";"}));
}

TEST(LexerTest, LexWhitespaceAfterToken) {
  auto reader = ::testing::NiceMock<MockReader>("; \t");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_OP_SEMI, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ";"}));
  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{3, 1, 3}, ReaderLocation{3, 1, 3}, ""}));
}

TEST(LexerTest, LexBlockCommentBeforeToken) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello Anna*/,");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_OP_COMMA, ReaderLocation{14, 1, 14}, ReaderLocation{15, 1, 15}, ","}));
}

TEST(LexerTest, LexBlockCommentAfterToken) {
  auto reader = ::testing::NiceMock<MockReader>(",/*Hello Anna*/");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_OP_COMMA, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ","}));
  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{15, 1, 15}, ReaderLocation{15, 1, 15}, ""}));
}

TEST(LexerTest, LexLineCommentBeforeToken) {
  auto reader = ::testing::NiceMock<MockReader>("//Hello Kylie\n+");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_OP_PLUS, ReaderLocation{14, 2, 0}, ReaderLocation{15, 2, 1}, "+"}));
}

TEST(LexerTest, LexLineCommentAfterToken) {
  auto reader = ::testing::NiceMock<MockReader>("+//Hello Kylie\n");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_OP_PLUS, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "+"}));
  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{15, 2, 0}, ReaderLocation{15, 2, 0}, ""}));
}

TEST(LexerTest, LexOperations) {
  auto r1 = ::testing::NiceMock<MockReader>("&^:,.=!>{[(<-|%+?}]);/~*");
  auto r2 = ::testing::NiceMock<MockReader>("&&!!<<--||++?.?\?>>**");
  auto r3 = ::testing::NiceMock<MockReader>("&=&&=^=:=...==!=>=<<=<=-=|=||=%=+=?\?=>>=/=*=**=");
  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_EQ(l1.next(), (Token{TK_OP_AND, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "&"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_CARET, ReaderLocation{1, 1, 1}, ReaderLocation{2, 1, 2}, "^"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_COLON, ReaderLocation{2, 1, 2}, ReaderLocation{3, 1, 3}, ":"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_COMMA, ReaderLocation{3, 1, 3}, ReaderLocation{4, 1, 4}, ","}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_DOT, ReaderLocation{4, 1, 4}, ReaderLocation{5, 1, 5}, "."}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_EQ, ReaderLocation{5, 1, 5}, ReaderLocation{6, 1, 6}, "="}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_EXCL, ReaderLocation{6, 1, 6}, ReaderLocation{7, 1, 7}, "!"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_GT, ReaderLocation{7, 1, 7}, ReaderLocation{8, 1, 8}, ">"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LBRACE, ReaderLocation{8, 1, 8}, ReaderLocation{9, 1, 9}, "{"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LBRACK, ReaderLocation{9, 1, 9}, ReaderLocation{10, 1, 10}, "["}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LPAR, ReaderLocation{10, 1, 10}, ReaderLocation{11, 1, 11}, "("}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LT, ReaderLocation{11, 1, 11}, ReaderLocation{12, 1, 12}, "<"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_MINUS, ReaderLocation{12, 1, 12}, ReaderLocation{13, 1, 13}, "-"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_OR, ReaderLocation{13, 1, 13}, ReaderLocation{14, 1, 14}, "|"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_PERCENT, ReaderLocation{14, 1, 14}, ReaderLocation{15, 1, 15}, "%"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_PLUS, ReaderLocation{15, 1, 15}, ReaderLocation{16, 1, 16}, "+"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_QN, ReaderLocation{16, 1, 16}, ReaderLocation{17, 1, 17}, "?"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_RBRACE, ReaderLocation{17, 1, 17}, ReaderLocation{18, 1, 18}, "}"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_RBRACK, ReaderLocation{18, 1, 18}, ReaderLocation{19, 1, 19}, "]"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_RPAR, ReaderLocation{19, 1, 19}, ReaderLocation{20, 1, 20}, ")"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_SEMI, ReaderLocation{20, 1, 20}, ReaderLocation{21, 1, 21}, ";"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_SLASH, ReaderLocation{21, 1, 21}, ReaderLocation{22, 1, 22}, "/"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_TILDE, ReaderLocation{22, 1, 22}, ReaderLocation{23, 1, 23}, "~"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_STAR, ReaderLocation{23, 1, 23}, ReaderLocation{24, 1, 24}, "*"}));

  EXPECT_EQ(l2.next(), (Token{TK_OP_AND_AND, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "&&"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_EXCL_EXCL, ReaderLocation{2, 1, 2}, ReaderLocation{4, 1, 4}, "!!"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_LSHIFT, ReaderLocation{4, 1, 4}, ReaderLocation{6, 1, 6}, "<<"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_MINUS_MINUS, ReaderLocation{6, 1, 6}, ReaderLocation{8, 1, 8}, "--"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_OR_OR, ReaderLocation{8, 1, 8}, ReaderLocation{10, 1, 10}, "||"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_PLUS_PLUS, ReaderLocation{10, 1, 10}, ReaderLocation{12, 1, 12}, "++"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_QN_DOT, ReaderLocation{12, 1, 12}, ReaderLocation{14, 1, 14}, "?."}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_QN_QN, ReaderLocation{14, 1, 14}, ReaderLocation{16, 1, 16}, "??"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_RSHIFT, ReaderLocation{16, 1, 16}, ReaderLocation{18, 1, 18}, ">>"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_STAR_STAR, ReaderLocation{18, 1, 18}, ReaderLocation{20, 1, 20}, "**"}));

  EXPECT_EQ(l3.next(), (Token{TK_OP_AND_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "&="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_AND_AND_EQ, ReaderLocation{2, 1, 2}, ReaderLocation{5, 1, 5}, "&&="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_CARET_EQ, ReaderLocation{5, 1, 5}, ReaderLocation{7, 1, 7}, "^="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_COLON_EQ, ReaderLocation{7, 1, 7}, ReaderLocation{9, 1, 9}, ":="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_DOT_DOT_DOT, ReaderLocation{9, 1, 9}, ReaderLocation{12, 1, 12}, "..."}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_EQ_EQ, ReaderLocation{12, 1, 12}, ReaderLocation{14, 1, 14}, "=="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_EXCL_EQ, ReaderLocation{14, 1, 14}, ReaderLocation{16, 1, 16}, "!="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_GT_EQ, ReaderLocation{16, 1, 16}, ReaderLocation{18, 1, 18}, ">="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_LSHIFT_EQ, ReaderLocation{18, 1, 18}, ReaderLocation{21, 1, 21}, "<<="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_LT_EQ, ReaderLocation{21, 1, 21}, ReaderLocation{23, 1, 23}, "<="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_MINUS_EQ, ReaderLocation{23, 1, 23}, ReaderLocation{25, 1, 25}, "-="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_OR_EQ, ReaderLocation{25, 1, 25}, ReaderLocation{27, 1, 27}, "|="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_OR_OR_EQ, ReaderLocation{27, 1, 27}, ReaderLocation{30, 1, 30}, "||="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_PERCENT_EQ, ReaderLocation{30, 1, 30}, ReaderLocation{32, 1, 32}, "%="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_PLUS_EQ, ReaderLocation{32, 1, 32}, ReaderLocation{34, 1, 34}, "+="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_QN_QN_EQ, ReaderLocation{34, 1, 34}, ReaderLocation{37, 1, 37}, "?\?="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_RSHIFT_EQ, ReaderLocation{37, 1, 37}, ReaderLocation{40, 1, 40}, ">>="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_SLASH_EQ, ReaderLocation{40, 1, 40}, ReaderLocation{42, 1, 42}, "/="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_STAR_EQ, ReaderLocation{42, 1, 42}, ReaderLocation{44, 1, 44}, "*="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_STAR_STAR_EQ, ReaderLocation{44, 1, 44}, ReaderLocation{47, 1, 47}, "**="}));
}

TEST(LexerTest, LexOperationsWhitespace) {
  auto r1 = ::testing::NiceMock<MockReader>(" & ^ : , . = ! > { [ ( < - | % + ? } ] ) ; / * ~ ");
  auto r2 = ::testing::NiceMock<MockReader>(" && !! << -- || ++ ?. ?? >> ** ");
  auto r3 = ::testing::NiceMock<MockReader>(" &= &&= ^= := ... == != >= <<= <= -= |= ||= %= += ?\?= >>= /= *= **= ");
  auto l1 = Lexer(&r1);
  auto l2 = Lexer(&r2);
  auto l3 = Lexer(&r3);

  EXPECT_EQ(l1.next(), (Token{TK_OP_AND, ReaderLocation{1, 1, 1}, ReaderLocation{2, 1, 2}, "&"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_CARET, ReaderLocation{3, 1, 3}, ReaderLocation{4, 1, 4}, "^"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_COLON, ReaderLocation{5, 1, 5}, ReaderLocation{6, 1, 6}, ":"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_COMMA, ReaderLocation{7, 1, 7}, ReaderLocation{8, 1, 8}, ","}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_DOT, ReaderLocation{9, 1, 9}, ReaderLocation{10, 1, 10}, "."}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_EQ, ReaderLocation{11, 1, 11}, ReaderLocation{12, 1, 12}, "="}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_EXCL, ReaderLocation{13, 1, 13}, ReaderLocation{14, 1, 14}, "!"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_GT, ReaderLocation{15, 1, 15}, ReaderLocation{16, 1, 16}, ">"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LBRACE, ReaderLocation{17, 1, 17}, ReaderLocation{18, 1, 18}, "{"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LBRACK, ReaderLocation{19, 1, 19}, ReaderLocation{20, 1, 20}, "["}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LPAR, ReaderLocation{21, 1, 21}, ReaderLocation{22, 1, 22}, "("}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_LT, ReaderLocation{23, 1, 23}, ReaderLocation{24, 1, 24}, "<"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_MINUS, ReaderLocation{25, 1, 25}, ReaderLocation{26, 1, 26}, "-"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_OR, ReaderLocation{27, 1, 27}, ReaderLocation{28, 1, 28}, "|"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_PERCENT, ReaderLocation{29, 1, 29}, ReaderLocation{30, 1, 30}, "%"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_PLUS, ReaderLocation{31, 1, 31}, ReaderLocation{32, 1, 32}, "+"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_QN, ReaderLocation{33, 1, 33}, ReaderLocation{34, 1, 34}, "?"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_RBRACE, ReaderLocation{35, 1, 35}, ReaderLocation{36, 1, 36}, "}"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_RBRACK, ReaderLocation{37, 1, 37}, ReaderLocation{38, 1, 38}, "]"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_RPAR, ReaderLocation{39, 1, 39}, ReaderLocation{40, 1, 40}, ")"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_SEMI, ReaderLocation{41, 1, 41}, ReaderLocation{42, 1, 42}, ";"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_SLASH, ReaderLocation{43, 1, 43}, ReaderLocation{44, 1, 44}, "/"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_STAR, ReaderLocation{45, 1, 45}, ReaderLocation{46, 1, 46}, "*"}));
  EXPECT_EQ(l1.next(), (Token{TK_OP_TILDE, ReaderLocation{47, 1, 47}, ReaderLocation{48, 1, 48}, "~"}));
  EXPECT_EQ(l1.next(), (Token{TK_EOF, ReaderLocation{49, 1, 49}, ReaderLocation{49, 1, 49}, ""}));

  EXPECT_EQ(l2.next(), (Token{TK_OP_AND_AND, ReaderLocation{1, 1, 1}, ReaderLocation{3, 1, 3}, "&&"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_EXCL_EXCL, ReaderLocation{4, 1, 4}, ReaderLocation{6, 1, 6}, "!!"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_LSHIFT, ReaderLocation{7, 1, 7}, ReaderLocation{9, 1, 9}, "<<"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_MINUS_MINUS, ReaderLocation{10, 1, 10}, ReaderLocation{12, 1, 12}, "--"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_OR_OR, ReaderLocation{13, 1, 13}, ReaderLocation{15, 1, 15}, "||"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_PLUS_PLUS, ReaderLocation{16, 1, 16}, ReaderLocation{18, 1, 18}, "++"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_QN_DOT, ReaderLocation{19, 1, 19}, ReaderLocation{21, 1, 21}, "?."}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_QN_QN, ReaderLocation{22, 1, 22}, ReaderLocation{24, 1, 24}, "??"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_RSHIFT, ReaderLocation{25, 1, 25}, ReaderLocation{27, 1, 27}, ">>"}));
  EXPECT_EQ(l2.next(), (Token{TK_OP_STAR_STAR, ReaderLocation{28, 1, 28}, ReaderLocation{30, 1, 30}, "**"}));
  EXPECT_EQ(l2.next(), (Token{TK_EOF, ReaderLocation{31, 1, 31}, ReaderLocation{31, 1, 31}, ""}));

  EXPECT_EQ(l3.next(), (Token{TK_OP_AND_EQ, ReaderLocation{1, 1, 1}, ReaderLocation{3, 1, 3}, "&="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_AND_AND_EQ, ReaderLocation{4, 1, 4}, ReaderLocation{7, 1, 7}, "&&="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_CARET_EQ, ReaderLocation{8, 1, 8}, ReaderLocation{10, 1, 10}, "^="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_COLON_EQ, ReaderLocation{11, 1, 11}, ReaderLocation{13, 1, 13}, ":="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_DOT_DOT_DOT, ReaderLocation{14, 1, 14}, ReaderLocation{17, 1, 17}, "..."}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_EQ_EQ, ReaderLocation{18, 1, 18}, ReaderLocation{20, 1, 20}, "=="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_EXCL_EQ, ReaderLocation{21, 1, 21}, ReaderLocation{23, 1, 23}, "!="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_GT_EQ, ReaderLocation{24, 1, 24}, ReaderLocation{26, 1, 26}, ">="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_LSHIFT_EQ, ReaderLocation{27, 1, 27}, ReaderLocation{30, 1, 30}, "<<="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_LT_EQ, ReaderLocation{31, 1, 31}, ReaderLocation{33, 1, 33}, "<="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_MINUS_EQ, ReaderLocation{34, 1, 34}, ReaderLocation{36, 1, 36}, "-="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_OR_EQ, ReaderLocation{37, 1, 37}, ReaderLocation{39, 1, 39}, "|="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_OR_OR_EQ, ReaderLocation{40, 1, 40}, ReaderLocation{43, 1, 43}, "||="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_PERCENT_EQ, ReaderLocation{44, 1, 44}, ReaderLocation{46, 1, 46}, "%="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_PLUS_EQ, ReaderLocation{47, 1, 47}, ReaderLocation{49, 1, 49}, "+="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_QN_QN_EQ, ReaderLocation{50, 1, 50}, ReaderLocation{53, 1, 53}, "?\?="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_RSHIFT_EQ, ReaderLocation{54, 1, 54}, ReaderLocation{57, 1, 57}, ">>="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_SLASH_EQ, ReaderLocation{58, 1, 58}, ReaderLocation{60, 1, 60}, "/="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_STAR_EQ, ReaderLocation{61, 1, 61}, ReaderLocation{63, 1, 63}, "*="}));
  EXPECT_EQ(l3.next(), (Token{TK_OP_STAR_STAR_EQ, ReaderLocation{64, 1, 64}, ReaderLocation{67, 1, 67}, "**="}));
  EXPECT_EQ(l3.next(), (Token{TK_EOF, ReaderLocation{68, 1, 68}, ReaderLocation{68, 1, 68}, ""}));
}

TEST(LexerTest, LexOperationsEof) {
  auto r1 = ::testing::NiceMock<MockReader>("&");
  auto l1 = Lexer(&r1);
  EXPECT_EQ(l1.next(), (Token{TK_OP_AND, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "&"}));

  auto r2 = ::testing::NiceMock<MockReader>("^");
  auto l2 = Lexer(&r2);
  EXPECT_EQ(l2.next(), (Token{TK_OP_CARET, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "^"}));

  auto r3 = ::testing::NiceMock<MockReader>(":");
  auto l3 = Lexer(&r3);
  EXPECT_EQ(l3.next(), (Token{TK_OP_COLON, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ":"}));

  auto r4 = ::testing::NiceMock<MockReader>(",");
  auto l4 = Lexer(&r4);
  EXPECT_EQ(l4.next(), (Token{TK_OP_COMMA, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ","}));

  auto r5 = ::testing::NiceMock<MockReader>(".");
  auto l5 = Lexer(&r5);
  EXPECT_EQ(l5.next(), (Token{TK_OP_DOT, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "."}));

  auto r6 = ::testing::NiceMock<MockReader>("=");
  auto l6 = Lexer(&r6);
  EXPECT_EQ(l6.next(), (Token{TK_OP_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "="}));

  auto r7 = ::testing::NiceMock<MockReader>("!");
  auto l7 = Lexer(&r7);
  EXPECT_EQ(l7.next(), (Token{TK_OP_EXCL, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "!"}));

  auto r8 = ::testing::NiceMock<MockReader>(">");
  auto l8 = Lexer(&r8);
  EXPECT_EQ(l8.next(), (Token{TK_OP_GT, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ">"}));

  auto r9 = ::testing::NiceMock<MockReader>("{");
  auto l9 = Lexer(&r9);
  EXPECT_EQ(l9.next(), (Token{TK_OP_LBRACE, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "{"}));

  auto r10 = ::testing::NiceMock<MockReader>("[");
  auto l10 = Lexer(&r10);
  EXPECT_EQ(l10.next(), (Token{TK_OP_LBRACK, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "["}));

  auto r11 = ::testing::NiceMock<MockReader>("(");
  auto l11 = Lexer(&r11);
  EXPECT_EQ(l11.next(), (Token{TK_OP_LPAR, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "("}));

  auto r12 = ::testing::NiceMock<MockReader>("<");
  auto l12 = Lexer(&r12);
  EXPECT_EQ(l12.next(), (Token{TK_OP_LT, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "<"}));

  auto r13 = ::testing::NiceMock<MockReader>("-");
  auto l13 = Lexer(&r13);
  EXPECT_EQ(l13.next(), (Token{TK_OP_MINUS, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "-"}));

  auto r14 = ::testing::NiceMock<MockReader>("|");
  auto l14 = Lexer(&r14);
  EXPECT_EQ(l14.next(), (Token{TK_OP_OR, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "|"}));

  auto r15 = ::testing::NiceMock<MockReader>("%");
  auto l15 = Lexer(&r15);
  EXPECT_EQ(l15.next(), (Token{TK_OP_PERCENT, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "%"}));

  auto r16 = ::testing::NiceMock<MockReader>("+");
  auto l16 = Lexer(&r16);
  EXPECT_EQ(l16.next(), (Token{TK_OP_PLUS, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "+"}));

  auto r17 = ::testing::NiceMock<MockReader>("?");
  auto l17 = Lexer(&r17);
  EXPECT_EQ(l17.next(), (Token{TK_OP_QN, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "?"}));

  auto r18 = ::testing::NiceMock<MockReader>("}");
  auto l18 = Lexer(&r18);
  EXPECT_EQ(l18.next(), (Token{TK_OP_RBRACE, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "}"}));

  auto r19 = ::testing::NiceMock<MockReader>("]");
  auto l19 = Lexer(&r19);
  EXPECT_EQ(l19.next(), (Token{TK_OP_RBRACK, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "]"}));

  auto r20 = ::testing::NiceMock<MockReader>(")");
  auto l20 = Lexer(&r20);
  EXPECT_EQ(l20.next(), (Token{TK_OP_RPAR, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ")"}));

  auto r21 = ::testing::NiceMock<MockReader>(";");
  auto l21 = Lexer(&r21);
  EXPECT_EQ(l21.next(), (Token{TK_OP_SEMI, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, ";"}));

  auto r22 = ::testing::NiceMock<MockReader>("/");
  auto l22 = Lexer(&r22);
  EXPECT_EQ(l22.next(), (Token{TK_OP_SLASH, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "/"}));

  auto r23 = ::testing::NiceMock<MockReader>("*");
  auto l23 = Lexer(&r23);
  EXPECT_EQ(l23.next(), (Token{TK_OP_STAR, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "*"}));

  auto r24 = ::testing::NiceMock<MockReader>("~");
  auto l24 = Lexer(&r24);
  EXPECT_EQ(l24.next(), (Token{TK_OP_TILDE, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "~"}));

  auto r25 = ::testing::NiceMock<MockReader>("&&");
  auto l25 = Lexer(&r25);
  EXPECT_EQ(l25.next(), (Token{TK_OP_AND_AND, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "&&"}));

  auto r26 = ::testing::NiceMock<MockReader>("!!");
  auto l26 = Lexer(&r26);
  EXPECT_EQ(l26.next(), (Token{TK_OP_EXCL_EXCL, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "!!"}));

  auto r27 = ::testing::NiceMock<MockReader>("<<");
  auto l27 = Lexer(&r27);
  EXPECT_EQ(l27.next(), (Token{TK_OP_LSHIFT, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "<<"}));

  auto r28 = ::testing::NiceMock<MockReader>("--");
  auto l28 = Lexer(&r28);
  EXPECT_EQ(l28.next(), (Token{TK_OP_MINUS_MINUS, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "--"}));

  auto r29 = ::testing::NiceMock<MockReader>("||");
  auto l29 = Lexer(&r29);
  EXPECT_EQ(l29.next(), (Token{TK_OP_OR_OR, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "||"}));

  auto r30 = ::testing::NiceMock<MockReader>("++");
  auto l30 = Lexer(&r30);
  EXPECT_EQ(l30.next(), (Token{TK_OP_PLUS_PLUS, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "++"}));

  auto r31 = ::testing::NiceMock<MockReader>("?.");
  auto l31 = Lexer(&r31);
  EXPECT_EQ(l31.next(), (Token{TK_OP_QN_DOT, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "?."}));

  auto r32 = ::testing::NiceMock<MockReader>("??");
  auto l32 = Lexer(&r32);
  EXPECT_EQ(l32.next(), (Token{TK_OP_QN_QN, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "??"}));

  auto r33 = ::testing::NiceMock<MockReader>(">>");
  auto l33 = Lexer(&r33);
  EXPECT_EQ(l33.next(), (Token{TK_OP_RSHIFT, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, ">>"}));

  auto r34 = ::testing::NiceMock<MockReader>("**");
  auto l34 = Lexer(&r34);
  EXPECT_EQ(l34.next(), (Token{TK_OP_STAR_STAR, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "**"}));

  auto r35 = ::testing::NiceMock<MockReader>("&=");
  auto l35 = Lexer(&r35);
  EXPECT_EQ(l35.next(), (Token{TK_OP_AND_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "&="}));

  auto r36 = ::testing::NiceMock<MockReader>("&&=");
  auto l36 = Lexer(&r36);
  EXPECT_EQ(l36.next(), (Token{TK_OP_AND_AND_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "&&="}));

  auto r37 = ::testing::NiceMock<MockReader>("^=");
  auto l37 = Lexer(&r37);
  EXPECT_EQ(l37.next(), (Token{TK_OP_CARET_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "^="}));

  auto r38 = ::testing::NiceMock<MockReader>(":=");
  auto l38 = Lexer(&r38);
  EXPECT_EQ(l38.next(), (Token{TK_OP_COLON_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, ":="}));

  auto r39 = ::testing::NiceMock<MockReader>("...");
  auto l39 = Lexer(&r39);
  EXPECT_EQ(l39.next(), (Token{TK_OP_DOT_DOT_DOT, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "..."}));

  auto r40 = ::testing::NiceMock<MockReader>("==");
  auto l40 = Lexer(&r40);
  EXPECT_EQ(l40.next(), (Token{TK_OP_EQ_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "=="}));

  auto r41 = ::testing::NiceMock<MockReader>("!=");
  auto l41 = Lexer(&r41);
  EXPECT_EQ(l41.next(), (Token{TK_OP_EXCL_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "!="}));

  auto r42 = ::testing::NiceMock<MockReader>(">=");
  auto l42 = Lexer(&r42);
  EXPECT_EQ(l42.next(), (Token{TK_OP_GT_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, ">="}));

  auto r43 = ::testing::NiceMock<MockReader>("<<=");
  auto l43 = Lexer(&r43);
  EXPECT_EQ(l43.next(), (Token{TK_OP_LSHIFT_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "<<="}));

  auto r44 = ::testing::NiceMock<MockReader>("<=");
  auto l44 = Lexer(&r44);
  EXPECT_EQ(l44.next(), (Token{TK_OP_LT_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "<="}));

  auto r45 = ::testing::NiceMock<MockReader>("-=");
  auto l45 = Lexer(&r45);
  EXPECT_EQ(l45.next(), (Token{TK_OP_MINUS_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "-="}));

  auto r46 = ::testing::NiceMock<MockReader>("|=");
  auto l46 = Lexer(&r46);
  EXPECT_EQ(l46.next(), (Token{TK_OP_OR_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "|="}));

  auto r47 = ::testing::NiceMock<MockReader>("||=");
  auto l47 = Lexer(&r47);
  EXPECT_EQ(l47.next(), (Token{TK_OP_OR_OR_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "||="}));

  auto r48 = ::testing::NiceMock<MockReader>("%=");
  auto l48 = Lexer(&r48);
  EXPECT_EQ(l48.next(), (Token{TK_OP_PERCENT_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "%="}));

  auto r49 = ::testing::NiceMock<MockReader>("+=");
  auto l49 = Lexer(&r49);
  EXPECT_EQ(l49.next(), (Token{TK_OP_PLUS_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "+="}));

  auto r50 = ::testing::NiceMock<MockReader>("?\?=");
  auto l50 = Lexer(&r50);
  EXPECT_EQ(l50.next(), (Token{TK_OP_QN_QN_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "?\?="}));

  auto r51 = ::testing::NiceMock<MockReader>(">>=");
  auto l51 = Lexer(&r51);
  EXPECT_EQ(l51.next(), (Token{TK_OP_RSHIFT_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, ">>="}));

  auto r52 = ::testing::NiceMock<MockReader>("/=");
  auto l52 = Lexer(&r52);
  EXPECT_EQ(l52.next(), (Token{TK_OP_SLASH_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "/="}));

  auto r53 = ::testing::NiceMock<MockReader>("*=");
  auto l53 = Lexer(&r53);
  EXPECT_EQ(l53.next(), (Token{TK_OP_STAR_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "*="}));

  auto r54 = ::testing::NiceMock<MockReader>("**=");
  auto l54 = Lexer(&r54);
  EXPECT_EQ(l54.next(), (Token{TK_OP_STAR_STAR_EQ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "**="}));
}

TEST(LexerTest, LexKeywords) {
  auto r1 = ::testing::NiceMock<MockReader>("as");
  auto l1 = Lexer(&r1);
  EXPECT_EQ(l1.next(), (Token{TK_KW_AS, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "as"}));

  auto r2 = ::testing::NiceMock<MockReader>("async");
  auto l2 = Lexer(&r2);
  EXPECT_EQ(l2.next(), (Token{TK_KW_ASYNC, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "async"}));

  auto r3 = ::testing::NiceMock<MockReader>("await");
  auto l3 = Lexer(&r3);
  EXPECT_EQ(l3.next(), (Token{TK_KW_AWAIT, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "await"}));

  auto r4 = ::testing::NiceMock<MockReader>("break");
  auto l4 = Lexer(&r4);
  EXPECT_EQ(l4.next(), (Token{TK_KW_BREAK, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "break"}));

  auto r5 = ::testing::NiceMock<MockReader>("case");
  auto l5 = Lexer(&r5);
  EXPECT_EQ(l5.next(), (Token{TK_KW_CASE, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "case"}));

  auto r6 = ::testing::NiceMock<MockReader>("catch");
  auto l6 = Lexer(&r6);
  EXPECT_EQ(l6.next(), (Token{TK_KW_CATCH, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "catch"}));

  auto r7 = ::testing::NiceMock<MockReader>("class");
  auto l7 = Lexer(&r7);
  EXPECT_EQ(l7.next(), (Token{TK_KW_CLASS, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "class"}));

  auto r8 = ::testing::NiceMock<MockReader>("const");
  auto l8 = Lexer(&r8);
  EXPECT_EQ(l8.next(), (Token{TK_KW_CONST, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "const"}));

  auto r9 = ::testing::NiceMock<MockReader>("continue");
  auto l9 = Lexer(&r9);
  EXPECT_EQ(l9.next(), (Token{TK_KW_CONTINUE, ReaderLocation{0, 1, 0}, ReaderLocation{8, 1, 8}, "continue"}));

  auto r10 = ::testing::NiceMock<MockReader>("default");
  auto l10 = Lexer(&r10);
  EXPECT_EQ(l10.next(), (Token{TK_KW_DEFAULT, ReaderLocation{0, 1, 0}, ReaderLocation{7, 1, 7}, "default"}));

  auto r11 = ::testing::NiceMock<MockReader>("deinit");
  auto l11 = Lexer(&r11);
  EXPECT_EQ(l11.next(), (Token{TK_KW_DEINIT, ReaderLocation{0, 1, 0}, ReaderLocation{6, 1, 6}, "deinit"}));

  auto r12 = ::testing::NiceMock<MockReader>("elif");
  auto l12 = Lexer(&r12);
  EXPECT_EQ(l12.next(), (Token{TK_KW_ELIF, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "elif"}));

  auto r13 = ::testing::NiceMock<MockReader>("else");
  auto l13 = Lexer(&r13);
  EXPECT_EQ(l13.next(), (Token{TK_KW_ELSE, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "else"}));

  auto r14 = ::testing::NiceMock<MockReader>("enum");
  auto l14 = Lexer(&r14);
  EXPECT_EQ(l14.next(), (Token{TK_KW_ENUM, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "enum"}));

  auto r15 = ::testing::NiceMock<MockReader>("export");
  auto l15 = Lexer(&r15);
  EXPECT_EQ(l15.next(), (Token{TK_KW_EXPORT, ReaderLocation{0, 1, 0}, ReaderLocation{6, 1, 6}, "export"}));

  auto r16 = ::testing::NiceMock<MockReader>("fallthrough");
  auto l16 = Lexer(&r16);
  EXPECT_EQ(l16.next(), (Token{TK_KW_FALLTHROUGH, ReaderLocation{0, 1, 0}, ReaderLocation{11, 1, 11}, "fallthrough"}));

  auto r17 = ::testing::NiceMock<MockReader>("false");
  auto l17 = Lexer(&r17);
  EXPECT_EQ(l17.next(), (Token{TK_KW_FALSE, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "false"}));

  auto r18 = ::testing::NiceMock<MockReader>("fn");
  auto l18 = Lexer(&r18);
  EXPECT_EQ(l18.next(), (Token{TK_KW_FN, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "fn"}));

  auto r19 = ::testing::NiceMock<MockReader>("from");
  auto l19 = Lexer(&r19);
  EXPECT_EQ(l19.next(), (Token{TK_KW_FROM, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "from"}));

  auto r20 = ::testing::NiceMock<MockReader>("if");
  auto l20 = Lexer(&r20);
  EXPECT_EQ(l20.next(), (Token{TK_KW_IF, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "if"}));

  auto r21 = ::testing::NiceMock<MockReader>("import");
  auto l21 = Lexer(&r21);
  EXPECT_EQ(l21.next(), (Token{TK_KW_IMPORT, ReaderLocation{0, 1, 0}, ReaderLocation{6, 1, 6}, "import"}));

  auto r22 = ::testing::NiceMock<MockReader>("in");
  auto l22 = Lexer(&r22);
  EXPECT_EQ(l22.next(), (Token{TK_KW_IN, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "in"}));

  auto r23 = ::testing::NiceMock<MockReader>("init");
  auto l23 = Lexer(&r23);
  EXPECT_EQ(l23.next(), (Token{TK_KW_INIT, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "init"}));

  auto r24 = ::testing::NiceMock<MockReader>("interface");
  auto l24 = Lexer(&r24);
  EXPECT_EQ(l24.next(), (Token{TK_KW_INTERFACE, ReaderLocation{0, 1, 0}, ReaderLocation{9, 1, 9}, "interface"}));

  auto r25 = ::testing::NiceMock<MockReader>("is");
  auto l25 = Lexer(&r25);
  EXPECT_EQ(l25.next(), (Token{TK_KW_IS, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "is"}));

  auto r26 = ::testing::NiceMock<MockReader>("loop");
  auto l26 = Lexer(&r26);
  EXPECT_EQ(l26.next(), (Token{TK_KW_LOOP, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "loop"}));

  auto r27 = ::testing::NiceMock<MockReader>("main");
  auto l27 = Lexer(&r27);
  EXPECT_EQ(l27.next(), (Token{TK_KW_MAIN, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "main"}));

  auto r28 = ::testing::NiceMock<MockReader>("match");
  auto l28 = Lexer(&r28);
  EXPECT_EQ(l28.next(), (Token{TK_KW_MATCH, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "match"}));

  auto r29 = ::testing::NiceMock<MockReader>("mut");
  auto l29 = Lexer(&r29);
  EXPECT_EQ(l29.next(), (Token{TK_KW_MUT, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "mut"}));

  auto r30 = ::testing::NiceMock<MockReader>("new");
  auto l30 = Lexer(&r30);
  EXPECT_EQ(l30.next(), (Token{TK_KW_NEW, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "new"}));

  auto r31 = ::testing::NiceMock<MockReader>("nil");
  auto l31 = Lexer(&r31);
  EXPECT_EQ(l31.next(), (Token{TK_KW_NIL, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "nil"}));

  auto r32 = ::testing::NiceMock<MockReader>("obj");
  auto l32 = Lexer(&r32);
  EXPECT_EQ(l32.next(), (Token{TK_KW_OBJ, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "obj"}));

  auto r33 = ::testing::NiceMock<MockReader>("op");
  auto l33 = Lexer(&r33);
  EXPECT_EQ(l33.next(), (Token{TK_KW_OP, ReaderLocation{0, 1, 0}, ReaderLocation{2, 1, 2}, "op"}));

  auto r34 = ::testing::NiceMock<MockReader>("override");
  auto l34 = Lexer(&r34);
  EXPECT_EQ(l34.next(), (Token{TK_KW_OVERRIDE, ReaderLocation{0, 1, 0}, ReaderLocation{8, 1, 8}, "override"}));

  auto r35 = ::testing::NiceMock<MockReader>("priv");
  auto l35 = Lexer(&r35);
  EXPECT_EQ(l35.next(), (Token{TK_KW_PRIV, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "priv"}));

  auto r36 = ::testing::NiceMock<MockReader>("prot");
  auto l36 = Lexer(&r36);
  EXPECT_EQ(l36.next(), (Token{TK_KW_PROT, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "prot"}));

  auto r37 = ::testing::NiceMock<MockReader>("pub");
  auto l37 = Lexer(&r37);
  EXPECT_EQ(l37.next(), (Token{TK_KW_PUB, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "pub"}));

  auto r38 = ::testing::NiceMock<MockReader>("return");
  auto l38 = Lexer(&r38);
  EXPECT_EQ(l38.next(), (Token{TK_KW_RETURN, ReaderLocation{0, 1, 0}, ReaderLocation{6, 1, 6}, "return"}));

  auto r39 = ::testing::NiceMock<MockReader>("static");
  auto l39 = Lexer(&r39);
  EXPECT_EQ(l39.next(), (Token{TK_KW_STATIC, ReaderLocation{0, 1, 0}, ReaderLocation{6, 1, 6}, "static"}));

  auto r40 = ::testing::NiceMock<MockReader>("super");
  auto l40 = Lexer(&r40);
  EXPECT_EQ(l40.next(), (Token{TK_KW_SUPER, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "super"}));

  auto r41 = ::testing::NiceMock<MockReader>("this");
  auto l41 = Lexer(&r41);
  EXPECT_EQ(l41.next(), (Token{TK_KW_THIS, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "this"}));

  auto r42 = ::testing::NiceMock<MockReader>("throw");
  auto l42 = Lexer(&r42);
  EXPECT_EQ(l42.next(), (Token{TK_KW_THROW, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "throw"}));

  auto r43 = ::testing::NiceMock<MockReader>("true");
  auto l43 = Lexer(&r43);
  EXPECT_EQ(l43.next(), (Token{TK_KW_TRUE, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "true"}));

  auto r44 = ::testing::NiceMock<MockReader>("try");
  auto l44 = Lexer(&r44);
  EXPECT_EQ(l44.next(), (Token{TK_KW_TRY, ReaderLocation{0, 1, 0}, ReaderLocation{3, 1, 3}, "try"}));

  auto r45 = ::testing::NiceMock<MockReader>("type");
  auto l45 = Lexer(&r45);
  EXPECT_EQ(l45.next(), (Token{TK_KW_TYPE, ReaderLocation{0, 1, 0}, ReaderLocation{4, 1, 4}, "type"}));

  auto r46 = ::testing::NiceMock<MockReader>("union");
  auto l46 = Lexer(&r46);
  EXPECT_EQ(l46.next(), (Token{TK_KW_UNION, ReaderLocation{0, 1, 0}, ReaderLocation{5, 1, 5}, "union"}));
}

TEST(LexerTest, LexKeywordsWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(
    " as async await break case catch class const continue default deinit elif else enum export fallthrough false fn"
    " from if import in init interface is loop main match mut new nil obj op override priv prot pub return static"
    " super this throw true try type union "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_KW_AS, ReaderLocation{1, 1, 1}, ReaderLocation{3, 1, 3}, "as"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_ASYNC, ReaderLocation{4, 1, 4}, ReaderLocation{9, 1, 9}, "async"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_AWAIT, ReaderLocation{10, 1, 10}, ReaderLocation{15, 1, 15}, "await"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_BREAK, ReaderLocation{16, 1, 16}, ReaderLocation{21, 1, 21}, "break"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_CASE, ReaderLocation{22, 1, 22}, ReaderLocation{26, 1, 26}, "case"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_CATCH, ReaderLocation{27, 1, 27}, ReaderLocation{32, 1, 32}, "catch"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_CLASS, ReaderLocation{33, 1, 33}, ReaderLocation{38, 1, 38}, "class"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_CONST, ReaderLocation{39, 1, 39}, ReaderLocation{44, 1, 44}, "const"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_CONTINUE, ReaderLocation{45, 1, 45}, ReaderLocation{53, 1, 53}, "continue"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_DEFAULT, ReaderLocation{54, 1, 54}, ReaderLocation{61, 1, 61}, "default"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_DEINIT, ReaderLocation{62, 1, 62}, ReaderLocation{68, 1, 68}, "deinit"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_ELIF, ReaderLocation{69, 1, 69}, ReaderLocation{73, 1, 73}, "elif"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_ELSE, ReaderLocation{74, 1, 74}, ReaderLocation{78, 1, 78}, "else"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_ENUM, ReaderLocation{79, 1, 79}, ReaderLocation{83, 1, 83}, "enum"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_EXPORT, ReaderLocation{84, 1, 84}, ReaderLocation{90, 1, 90}, "export"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_FALLTHROUGH, ReaderLocation{91, 1, 91}, ReaderLocation{102, 1, 102}, "fallthrough"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_FALSE, ReaderLocation{103, 1, 103}, ReaderLocation{108, 1, 108}, "false"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_FN, ReaderLocation{109, 1, 109}, ReaderLocation{111, 1, 111}, "fn"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_FROM, ReaderLocation{112, 1, 112}, ReaderLocation{116, 1, 116}, "from"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_IF, ReaderLocation{117, 1, 117}, ReaderLocation{119, 1, 119}, "if"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_IMPORT, ReaderLocation{120, 1, 120}, ReaderLocation{126, 1, 126}, "import"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_IN, ReaderLocation{127, 1, 127}, ReaderLocation{129, 1, 129}, "in"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_INIT, ReaderLocation{130, 1, 130}, ReaderLocation{134, 1, 134}, "init"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_INTERFACE, ReaderLocation{135, 1, 135}, ReaderLocation{144, 1, 144}, "interface"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_IS, ReaderLocation{145, 1, 145}, ReaderLocation{147, 1, 147}, "is"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_LOOP, ReaderLocation{148, 1, 148}, ReaderLocation{152, 1, 152}, "loop"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_MAIN, ReaderLocation{153, 1, 153}, ReaderLocation{157, 1, 157}, "main"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_MATCH, ReaderLocation{158, 1, 158}, ReaderLocation{163, 1, 163}, "match"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_MUT, ReaderLocation{164, 1, 164}, ReaderLocation{167, 1, 167}, "mut"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_NEW, ReaderLocation{168, 1, 168}, ReaderLocation{171, 1, 171}, "new"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_NIL, ReaderLocation{172, 1, 172}, ReaderLocation{175, 1, 175}, "nil"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_OBJ, ReaderLocation{176, 1, 176}, ReaderLocation{179, 1, 179}, "obj"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_OP, ReaderLocation{180, 1, 180}, ReaderLocation{182, 1, 182}, "op"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_OVERRIDE, ReaderLocation{183, 1, 183}, ReaderLocation{191, 1, 191}, "override"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_PRIV, ReaderLocation{192, 1, 192}, ReaderLocation{196, 1, 196}, "priv"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_PROT, ReaderLocation{197, 1, 197}, ReaderLocation{201, 1, 201}, "prot"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_PUB, ReaderLocation{202, 1, 202}, ReaderLocation{205, 1, 205}, "pub"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_RETURN, ReaderLocation{206, 1, 206}, ReaderLocation{212, 1, 212}, "return"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_STATIC, ReaderLocation{213, 1, 213}, ReaderLocation{219, 1, 219}, "static"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_SUPER, ReaderLocation{220, 1, 220}, ReaderLocation{225, 1, 225}, "super"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_THIS, ReaderLocation{226, 1, 226}, ReaderLocation{230, 1, 230}, "this"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_THROW, ReaderLocation{231, 1, 231}, ReaderLocation{236, 1, 236}, "throw"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_TRUE, ReaderLocation{237, 1, 237}, ReaderLocation{241, 1, 241}, "true"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_TRY, ReaderLocation{242, 1, 242}, ReaderLocation{245, 1, 245}, "try"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_TYPE, ReaderLocation{246, 1, 246}, ReaderLocation{250, 1, 250}, "type"}));
  EXPECT_EQ(lexer.next(), (Token{TK_KW_UNION, ReaderLocation{251, 1, 251}, ReaderLocation{256, 1, 256}, "union"}));
  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{257, 1, 257}, ReaderLocation{257, 1, 257}, ""}));
}

TEST(LexerTest, LexKeywordsAsIdentifiers) {
  auto reader = ::testing::NiceMock<MockReader>(
    " asm asynchronously awaited breakpoint casein catching classic constant continues defaulted deinitialized elifffy"
    " elsewhere enumeration exported fallthroughout falser fname fromental iffy importing inside initialization"
    " interfaced isle looped maintain matching mutable newest nilled object operation overriding private protected"
    " public returned statically superb thistle throwing trues tryout typed unions "
  );

  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{1, 1, 1}, ReaderLocation{4, 1, 4}, "asm"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{5, 1, 5}, ReaderLocation{19, 1, 19}, "asynchronously"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{20, 1, 20}, ReaderLocation{27, 1, 27}, "awaited"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{28, 1, 28}, ReaderLocation{38, 1, 38}, "breakpoint"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{39, 1, 39}, ReaderLocation{45, 1, 45}, "casein"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{46, 1, 46}, ReaderLocation{54, 1, 54}, "catching"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{55, 1, 55}, ReaderLocation{62, 1, 62}, "classic"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{63, 1, 63}, ReaderLocation{71, 1, 71}, "constant"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{72, 1, 72}, ReaderLocation{81, 1, 81}, "continues"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{82, 1, 82}, ReaderLocation{91, 1, 91}, "defaulted"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{92, 1, 92}, ReaderLocation{105, 1, 105}, "deinitialized"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{106, 1, 106}, ReaderLocation{113, 1, 113}, "elifffy"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{114, 1, 114}, ReaderLocation{123, 1, 123}, "elsewhere"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{124, 1, 124}, ReaderLocation{135, 1, 135}, "enumeration"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{136, 1, 136}, ReaderLocation{144, 1, 144}, "exported"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{145, 1, 145}, ReaderLocation{159, 1, 159}, "fallthroughout"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{160, 1, 160}, ReaderLocation{166, 1, 166}, "falser"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{167, 1, 167}, ReaderLocation{172, 1, 172}, "fname"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{173, 1, 173}, ReaderLocation{182, 1, 182}, "fromental"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{183, 1, 183}, ReaderLocation{187, 1, 187}, "iffy"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{188, 1, 188}, ReaderLocation{197, 1, 197}, "importing"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{198, 1, 198}, ReaderLocation{204, 1, 204}, "inside"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{205, 1, 205}, ReaderLocation{219, 1, 219}, "initialization"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{220, 1, 220}, ReaderLocation{230, 1, 230}, "interfaced"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{231, 1, 231}, ReaderLocation{235, 1, 235}, "isle"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{236, 1, 236}, ReaderLocation{242, 1, 242}, "looped"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{243, 1, 243}, ReaderLocation{251, 1, 251}, "maintain"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{252, 1, 252}, ReaderLocation{260, 1, 260}, "matching"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{261, 1, 261}, ReaderLocation{268, 1, 268}, "mutable"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{269, 1, 269}, ReaderLocation{275, 1, 275}, "newest"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{276, 1, 276}, ReaderLocation{282, 1, 282}, "nilled"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{283, 1, 283}, ReaderLocation{289, 1, 289}, "object"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{290, 1, 290}, ReaderLocation{299, 1, 299}, "operation"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{300, 1, 300}, ReaderLocation{310, 1, 310}, "overriding"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{311, 1, 311}, ReaderLocation{318, 1, 318}, "private"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{319, 1, 319}, ReaderLocation{328, 1, 328}, "protected"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{329, 1, 329}, ReaderLocation{335, 1, 335}, "public"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{336, 1, 336}, ReaderLocation{344, 1, 344}, "returned"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{345, 1, 345}, ReaderLocation{355, 1, 355}, "statically"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{356, 1, 356}, ReaderLocation{362, 1, 362}, "superb"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{363, 1, 363}, ReaderLocation{370, 1, 370}, "thistle"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{371, 1, 371}, ReaderLocation{379, 1, 379}, "throwing"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{380, 1, 380}, ReaderLocation{385, 1, 385}, "trues"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{386, 1, 386}, ReaderLocation{392, 1, 392}, "tryout"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{393, 1, 393}, ReaderLocation{398, 1, 398}, "typed"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{399, 1, 399}, ReaderLocation{405, 1, 405}, "unions"}));
}

TEST(LexerTest, LexIdentifier) {
  auto r1 = ::testing::NiceMock<MockReader>("a");
  auto l1 = Lexer(&r1);
  EXPECT_EQ(l1.next(), (Token{TK_ID, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "a"}));

  auto r2 = ::testing::NiceMock<MockReader>("A");
  auto l2 = Lexer(&r2);
  EXPECT_EQ(l2.next(), (Token{TK_ID, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "A"}));

  auto r3 = ::testing::NiceMock<MockReader>("_");
  auto l3 = Lexer(&r3);
  EXPECT_EQ(l3.next(), (Token{TK_ID, ReaderLocation{0, 1, 0}, ReaderLocation{1, 1, 1}, "_"}));

  auto r4 = ::testing::NiceMock<MockReader>("number1");
  auto l4 = Lexer(&r4);
  EXPECT_EQ(l4.next(), (Token{TK_ID, ReaderLocation{0, 1, 0}, ReaderLocation{7, 1, 7}, "number1"}));

  auto r5 = ::testing::NiceMock<MockReader>("ANYTHING");
  auto l5 = Lexer(&r5);
  EXPECT_EQ(l5.next(), (Token{TK_ID, ReaderLocation{0, 1, 0}, ReaderLocation{8, 1, 8}, "ANYTHING"}));

  auto r6 = ::testing::NiceMock<MockReader>("__I1_D2__");
  auto l6 = Lexer(&r6);
  EXPECT_EQ(l6.next(), (Token{TK_ID, ReaderLocation{0, 1, 0}, ReaderLocation{9, 1, 9}, "__I1_D2__"}));
}

TEST(LexerTest, LexIdentifierWhitespace) {
  auto reader = ::testing::NiceMock<MockReader>(" a A _ number1 ANYTHING __I1_D2__ ");
  auto lexer = Lexer(&reader);

  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{1, 1, 1}, ReaderLocation{2, 1, 2}, "a"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{3, 1, 3}, ReaderLocation{4, 1, 4}, "A"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{5, 1, 5}, ReaderLocation{6, 1, 6}, "_"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{7, 1, 7}, ReaderLocation{14, 1, 14}, "number1"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{15, 1, 15}, ReaderLocation{23, 1, 23}, "ANYTHING"}));
  EXPECT_EQ(lexer.next(), (Token{TK_ID, ReaderLocation{24, 1, 24}, ReaderLocation{33, 1, 33}, "__I1_D2__"}));
  EXPECT_EQ(lexer.next(), (Token{TK_EOF, ReaderLocation{34, 1, 34}, ReaderLocation{34, 1, 34}, ""}));
}
