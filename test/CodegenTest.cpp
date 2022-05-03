/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include <filesystem>
#include "../src/Codegen.hpp"
#include "MockAST.hpp"
#include "utils.hpp"

class CodegenTest : public testing::TestWithParam<const char *> {
};

TEST_P(CodegenTest,) {
  auto param = std::string(testing::TestWithParam<const char *>::GetParam());
  auto testContent = readTestFile("codegen", param);

  if (testContent.substr(0, 22) != "======= stdin =======\n") {
    throw Error("Codegen test file doesn't look like an actual test");
  }

  testContent.erase(0, 22);

  auto delimiterCode = std::string("======= code =======\n");
  auto delimiterCodePos = testContent.find(delimiterCode);

  if (delimiterCodePos == std::string::npos) {
    throw Error("Codegen test file doesn't have a code delimiter");
  }

  auto testStdin = testContent.substr(0, delimiterCodePos);
  testContent.erase(0, delimiterCodePos + delimiterCode.size());

  auto delimiterFlags = std::string("======= flags =======\n");
  auto delimiterFlagsPos = testContent.find(delimiterFlags);

  if (delimiterFlagsPos == std::string::npos) {
    throw Error("Codegen test file doesn't have a flags delimiter");
  }

  auto expectedCode = testContent.substr(0, delimiterFlagsPos);
  testContent.erase(0, delimiterFlagsPos + delimiterFlags.size());

  auto delimiterOutput = std::string("======= stdout =======\n");
  auto delimiterOutputPos = testContent.find(delimiterOutput);

  if (delimiterOutputPos == std::string::npos) {
    throw Error("Codegen test file doesn't have an output delimiter");
  }

  auto expectedFlags = testContent.substr(0, delimiterOutputPos);
  testContent.erase(0, delimiterOutputPos + delimiterOutput.size());
  auto expectedOutput = testContent.substr(0, testContent.size());

  if (!expectedFlags.empty()) {
    expectedFlags = expectedFlags.substr(0, expectedFlags.size() - 1);
  }

  auto ast = testing::NiceMock<MockAST>(testStdin);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();

  EXPECT_EQ(expectedCode, std::get<0>(result).substr(150));
  EXPECT_EQ(expectedFlags, std::get<1>(result));

  auto filePath = param + ".out";
  Codegen::compile(filePath, result);
  auto actualOutput = execCmd("./" + filePath);
  std::filesystem::remove(filePath);

  EXPECT_EQ(expectedOutput, actualOutput);
}

INSTANTIATE_TEST_SUITE_P(General, CodegenTest, testing::Values(
  "empty"
));

INSTANTIATE_TEST_SUITE_P(NodeVarDecl, CodegenTest, testing::Values(
  "node-var-decl-bool",
  "node-var-decl-bool-init",
  "node-var-decl-bool-short",
  "node-var-decl-bool-mut",
  "node-var-decl-bool-mut-init",
  "node-var-decl-bool-mut-short",
  "node-var-decl-int",
  "node-var-decl-int-init",
  "node-var-decl-int-short",
  "node-var-decl-int-mut",
  "node-var-decl-int-mut-init",
  "node-var-decl-int-mut-short",
  "node-var-decl-float",
  "node-var-decl-float-init",
  "node-var-decl-float-short",
  "node-var-decl-float-mut",
  "node-var-decl-float-mut-init",
  "node-var-decl-float-mut-short",
  "node-var-decl-char",
  "node-var-decl-char-init",
  "node-var-decl-char-short",
  "node-var-decl-char-mut",
  "node-var-decl-char-mut-init",
  "node-var-decl-char-mut-short",
  "node-var-decl-str",
  "node-var-decl-str-init",
  "node-var-decl-str-short",
  "node-var-decl-str-mut",
  "node-var-decl-str-mut-init",
  "node-var-decl-str-mut-short"
));
