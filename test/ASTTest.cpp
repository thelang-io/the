/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/AST.hpp"
#include "MockParser.hpp"
#include "utils.hpp"

class ASTPassTest : public testing::TestWithParam<const char *> {
};

TEST_P(ASTPassTest, Passes) {
  auto testContent = readTestFile("ast", testing::TestWithParam<const char *>::GetParam());

  if (testContent.substr(0, 21) != "======= stdin =======") {
    throw Error("AST pass test file doesn't look like an actual test");
  }

  auto delimiter = std::string("\n======= stdout =======\n");
  auto delimiterPos = testContent.find(delimiter);

  if (delimiterPos == std::string::npos) {
    throw Error("AST pass test file doesn't have a delimiter");
  }

  auto testStdin = delimiterPos > 22 ? testContent.substr(22, delimiterPos - 22) : "";
  auto expectedOutput = testContent.substr(delimiterPos + delimiter.size());
  auto parser = testing::NiceMock<MockParser>(testStdin);
  auto ast = AST(&parser);
  auto actualOutput = std::string();

  for (const auto &node : ast.gen()) {
    actualOutput += node.xml() + "\n";
  }

  EXPECT_EQ(expectedOutput, actualOutput);
}

INSTANTIATE_TEST_SUITE_P(General, ASTPassTest, testing::Values(
  "node-break"
));
