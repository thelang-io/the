/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "../src/Error.hpp"
#include "../src/Parser.hpp"
#include "MockLexer.hpp"
#include "utils.hpp"

std::string parserTestReadFile (const char *name) {
  auto path = "test/parser-test/" + std::string(name) + ".txt";
  auto file = std::ifstream(path, std::ios::in | std::ios::binary);

  if (!file.is_open()) {
    throw Error(R"(Unable to open parser test file ")" + path + R"(")");
  }

  auto size = static_cast<std::ptrdiff_t>(std::filesystem::file_size(path));
  auto content = std::string(size, '\0');

  file.read(content.data(), size);
  file.close();

  return content;
}

class ParserPassTest : public testing::TestWithParam<const char *> {
};

class ParserThrowTest : public testing::TestWithParam<const char *> {
};

TEST_P(ParserPassTest, Passes) {
  auto testContent = parserTestReadFile(testing::TestWithParam<const char *>::GetParam());

  if (testContent.substr(0, 21) != "======= stdin =======") {
    throw Error("Parser pass test file doesn't look like an actual test");
  }

  auto delimiter = std::string("\n======= stdout =======\n");
  auto delimiterPos = testContent.find(delimiter);

  if (delimiterPos == std::string::npos) {
    throw Error("Parser pass test file doesn't have a delimiter");
  }

  auto testStdin = delimiterPos > 22 ? testContent.substr(22, delimiterPos - 22) : "";
  auto expectedOutput = testContent.substr(delimiterPos + delimiter.size());
  auto lexer = testing::NiceMock<MockLexer>(testStdin);
  auto parser = Parser(&lexer);
  auto actualOutput = std::string();

  while (true) {
    auto stmt = parser.next();

    if (std::holds_alternative<ParserStmtEof>(stmt.body)) {
      break;
    }

    actualOutput += stmt.xml() + "\n";
  }

  EXPECT_EQ(expectedOutput, actualOutput);
}

TEST_P(ParserThrowTest, Throws) {
  auto testContent = parserTestReadFile(testing::TestWithParam<const char *>::GetParam());

  if (testContent.substr(0, 21) != "======= stdin =======") {
    throw Error("Parser throw test file doesn't look like an actual test");
  }

  auto delimiter = std::string("\n======= stderr =======\n");
  auto delimiterPos = testContent.find(delimiter);

  if (delimiterPos == std::string::npos) {
    throw Error("Parser throw test file doesn't have a delimiter");
  }

  auto delimiterEndPos = delimiterPos + delimiter.size();
  auto testStdin = testContent.substr(22, delimiterPos - 22);
  auto expectedOutput = testContent.substr(delimiterEndPos, testContent.size() - delimiterEndPos - 1);
  auto lexer = testing::NiceMock<MockLexer>(testStdin);
  auto parser = Parser(&lexer);

  EXPECT_THROW_WITH_MESSAGE(parser.next(), expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(General, ParserPassTest, testing::Values(
  "stmt-eof",
  "stmt-break",
  "stmt-continue"
));

INSTANTIATE_TEST_SUITE_P(StmtFnDecl, ParserPassTest, testing::Values(
  "stmt-fn-decl-empty",
  "stmt-fn-decl",
  "stmt-fn-decl-with-param-init",
  "stmt-fn-decl-with-obj",
  "stmt-fn-decl-recursive",
  "stmt-fn-decl-nested",
  "stmt-fn-decl-scoped"
));

INSTANTIATE_TEST_SUITE_P(StmtIf, ParserPassTest, testing::Values(
  "stmt-if",
  "stmt-if-with-else",
  "stmt-if-with-single-elif",
  "stmt-if-with-multi-elif",
  "stmt-if-nested"
));

INSTANTIATE_TEST_SUITE_P(StmtLoop, ParserPassTest, testing::Values(
  "stmt-loop-for-empty",
  "stmt-loop",
  "stmt-loop-mut",
  "stmt-loop-empty-init-condition",
  "stmt-loop-empty-init-update",
  "stmt-loop-empty-update",
  "stmt-loop-empty-body",
  "stmt-loop-nested",
  "stmt-loop-while-empty",
  "stmt-loop-while",
  "stmt-loop-while-nested"
));

INSTANTIATE_TEST_SUITE_P(StmtMain, ParserPassTest, testing::Values(
  "stmt-main-empty",
  "stmt-main"
));

INSTANTIATE_TEST_SUITE_P(StmtReturn, ParserPassTest, testing::Values(
  "stmt-return",
  "stmt-return-no-arg"
));

INSTANTIATE_TEST_SUITE_P(StmtVarDecl, ParserPassTest, testing::Values(
  "stmt-var-decl",
  "stmt-var-decl-mut",
  "stmt-var-decl-no-init",
  "stmt-var-decl-no-init-mut",
  "stmt-var-decl-short",
  "stmt-var-decl-short-mut"
));

INSTANTIATE_TEST_SUITE_P(ExprAccess, ParserPassTest, testing::Values(
  "expr-access"
));

INSTANTIATE_TEST_SUITE_P(General, ParserThrowTest, testing::Values(
  "throw-E0100-unexpected-mut-keyword",
  "throw-E0100-unexpected-statement"
));

INSTANTIATE_TEST_SUITE_P(StmtVarDecl, ParserThrowTest, testing::Values(
  "throw-E0102-stmt-var-decl",
  "throw-E0102-stmt-var-decl-mut"
));
