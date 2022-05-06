/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
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

  auto envVars = getEnvVars();
  auto testMemcheck = envVars.contains("CODEGEN_MEMCHECK") && envVars["CODEGEN_MEMCHECK"] == "ON";
  auto ast = testing::NiceMock<MockAST>(testStdin);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();

  ASSERT_EQ(expectedCode, std::get<0>(result).substr(150));
  ASSERT_EQ(expectedFlags, std::get<1>(result));

  auto filePath = "build/" + param + ".out";
  Codegen::compile(filePath, result, true);
  auto cmd = filePath;

  if (testMemcheck) {
    cmd = "valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes " + cmd;
  }

  auto [actualOutput, actualStderr, actualReturnCode] = execCmd(cmd, "build/" + param);
  std::filesystem::remove(filePath);

  EXPECT_EQ(expectedOutput, actualOutput);
  EXPECT_EQ(actualReturnCode, 0);

  if (!testMemcheck) {
    EXPECT_EQ(actualStderr, "");
  } else if (actualReturnCode != 0) {
    std::cout << actualStderr;
  }
}

INSTANTIATE_TEST_SUITE_P(General, CodegenTest, testing::Values(
  "empty"
));

INSTANTIATE_TEST_SUITE_P(ExprAccess, CodegenTest, testing::Values(
  "expr-access",
  "expr-access-str"
));

INSTANTIATE_TEST_SUITE_P(ExprAssign, CodegenTest, testing::Values(
  "expr-assign",
  "expr-assign-op",
  "expr-assign-str"
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
  "node-var-decl-str-mut-short",
  "node-var-decl-byte",
  "node-var-decl-byte-init",
  "node-var-decl-byte-mut",
  "node-var-decl-byte-mut-init",
  "node-var-decl-f32",
  "node-var-decl-f32-init",
  "node-var-decl-f32-mut",
  "node-var-decl-f32-mut-init",
  "node-var-decl-f64",
  "node-var-decl-f64-init",
  "node-var-decl-f64-mut",
  "node-var-decl-f64-mut-init",
  "node-var-decl-i8",
  "node-var-decl-i8-init",
  "node-var-decl-i8-mut",
  "node-var-decl-i8-mut-init",
  "node-var-decl-u8",
  "node-var-decl-u8-init",
  "node-var-decl-u8-mut",
  "node-var-decl-u8-mut-init",
  "node-var-decl-i16",
  "node-var-decl-i16-init",
  "node-var-decl-i16-mut",
  "node-var-decl-i16-mut-init",
  "node-var-decl-u16",
  "node-var-decl-u16-init",
  "node-var-decl-u16-mut",
  "node-var-decl-u16-mut-init",
  "node-var-decl-i32",
  "node-var-decl-i32-init",
  "node-var-decl-i32-mut",
  "node-var-decl-i32-mut-init",
  "node-var-decl-u32",
  "node-var-decl-u32-init",
  "node-var-decl-u32-mut",
  "node-var-decl-u32-mut-init",
  "node-var-decl-i64",
  "node-var-decl-i64-init",
  "node-var-decl-i64-mut",
  "node-var-decl-i64-mut-init",
  "node-var-decl-u64",
  "node-var-decl-u64-init",
  "node-var-decl-u64-mut",
  "node-var-decl-u64-mut-init"
));
