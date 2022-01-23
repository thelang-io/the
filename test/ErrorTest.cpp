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

TEST(ErrorTest, ThrowsExactMessage) {
  EXPECT_THROW_WITH_MESSAGE({
    throw Error("Hello, World!");
  }, Error, "Hello, World!");
}

TEST(LexerErrorTest, SingleToken) {
  auto reader = ::testing::NiceMock<MockReader>("@");
  auto lexer = Lexer(&reader);

  EXPECT_THROW_WITH_MESSAGE({
    throw LexerError(&lexer, E0000);
  }, LexerError, (std::string("/test:1:1: ") + E0000 + "\n  1 | @\n    | ^").c_str());
}

TEST(LexerErrorTest, MultipleTokens) {
  auto reader = ::testing::NiceMock<MockReader>("test");
  auto lexer = Lexer(&reader);

  reader.loc = ReaderLocation{4, 1, 4};

  EXPECT_THROW_WITH_MESSAGE({
    throw LexerError(&lexer, E0000);
  }, LexerError, (std::string("/test:1:1: ") + E0000 + "\n  1 | test\n    | ^~~~").c_str());
}

TEST(LexerErrorTest, MultipleTokensAfterTokens) {
  auto reader = ::testing::NiceMock<MockReader>("1 + test");
  auto lexer = Lexer(&reader);

  lexer.loc = ReaderLocation{4, 1, 4};
  reader.loc = ReaderLocation{9, 1, 9};

  EXPECT_THROW_WITH_MESSAGE({
    throw LexerError(&lexer, E0000);
  }, LexerError, (std::string("/test:1:5: ") + E0000 + "\n  1 | 1 + test\n    |     ^~~~").c_str());
}

TEST(LexerErrorTest, MultipleTokensAfterNewLine) {
  auto reader = ::testing::NiceMock<MockReader>("print()\n/*Hello");
  auto lexer = Lexer(&reader);

  lexer.loc = ReaderLocation{8, 2, 0};
  reader.loc = ReaderLocation{15, 2, 7};

  EXPECT_THROW_WITH_MESSAGE({
    throw LexerError(&lexer, E0001);
  }, LexerError, (std::string("/test:2:1: ") + E0001 + "\n  2 | /*Hello\n    | ^~~~~~~").c_str());
}

TEST(LexerErrorTest, MultipleTokensBetweenNewLines) {
  auto reader = ::testing::NiceMock<MockReader>("print()\n/*Hello\nDenis");
  auto lexer = Lexer(&reader);

  lexer.loc = ReaderLocation{8, 2, 0};
  reader.loc = ReaderLocation{21, 3, 5};

  EXPECT_THROW_WITH_MESSAGE({
    throw LexerError(&lexer, E0001);
  }, LexerError, (std::string("/test:2:1: ") + E0001 + "\n  2 | /*Hello\n    | ^~~~~~~").c_str());
}

TEST(LexerErrorTest, MultipleTokensBeforeNewLine) {
  auto reader = ::testing::NiceMock<MockReader>("/*Hello\nDenis");
  auto lexer = Lexer(&reader);

  reader.loc = ReaderLocation{13, 2, 5};

  EXPECT_THROW_WITH_MESSAGE({
    throw LexerError(&lexer, E0001);
  }, LexerError, (std::string("/test:1:1: ") + E0001 + "\n  1 | /*Hello\n    | ^~~~~~~").c_str());
}
