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
#include "../src/config.hpp"
#include "MockAST.hpp"
#include "utils.hpp"

const auto valgrindArguments = std::string(
  "--error-exitcode=1 "
  "--leak-check=full "
  "--show-leak-kinds=all "
  "--track-origins=yes"
);

class CodegenPassTest : public testing::TestWithParam<const char *> {
 protected:
  bool testMemcheck_ = false;

  void SetUp () override {
    auto envVars = getEnvVars();
    this->testMemcheck_ = envVars.contains("CODEGEN_MEMCHECK") && envVars["CODEGEN_MEMCHECK"] == "ON";
  }
};

class CodegenThrowTest : public testing::TestWithParam<const char *> {
};

TEST_P(CodegenPassTest, Passes) {
  auto param = std::string(testing::TestWithParam<const char *>::GetParam());
  auto sections = readTestFile("codegen", param, {"stdin", "code", "code-windows", "flags", "stdout", "stdout-linux"});
  auto ast = testing::NiceMock<MockAST>(sections["stdin"]);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();
  auto expectedCode = sections["code"];
  auto expectedOutput = sections["stdout"];

  #if defined(OS_LINUX)
    if (sections.contains("stdout-linux")) {
      expectedOutput = sections["stdout-linux"];
    }
  #elif defined(OS_WINDOWS)
    if (sections.contains("code-windows")) {
      expectedCode = sections["code-windows"];
    }
  #endif

  ASSERT_EQ(expectedCode, std::get<0>(result).substr(143 + std::string(EOL).size() * 7));
  ASSERT_EQ(sections["flags"], std::get<1>(result));

  auto fileName = std::string("build") + OS_PATH_SEP + param;
  auto filePath = fileName + OS_FILE_EXT;
  Codegen::compile(filePath, result, true);

  auto [actualStdout, actualStderr, actualReturnCode] = execCmd(
    (this->testMemcheck_ ? "valgrind " + valgrindArguments + " " : "") + filePath,
    fileName
  );

  std::filesystem::remove(filePath);

  while (expectedOutput.find("{{--") != std::string::npos) {
    auto placeholderStart = expectedOutput.find("{{--");
    auto placeholderEnd = expectedOutput.find("--}}") + 4;
    auto placeholderLen = placeholderEnd - placeholderStart;
    auto placeholderValue = actualStdout.substr(placeholderStart, placeholderLen);

    expectedOutput.replace(placeholderStart, placeholderLen, placeholderValue);
  }

  EXPECT_EQ(expectedOutput, actualStdout);

  if (param == "builtin-exit-one") {
    EXPECT_NE(actualReturnCode, 0);
  } else {
    EXPECT_EQ(actualReturnCode, 0);
  }

  if (!this->testMemcheck_) {
    EXPECT_EQ(actualStderr, "");
  } else if (actualReturnCode != 0) {
    std::cout << actualStderr;
  }
}

TEST_P(CodegenThrowTest, Throws) {
  auto param = std::string(testing::TestWithParam<const char *>::GetParam());
  auto sections = readTestFile("codegen", param, {"stdin", "code", "code-windows", "flags", "stderr"});
  auto ast = testing::NiceMock<MockAST>(sections["stdin"]);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();
  auto expectedCode = sections["code"];

  #if defined(OS_WINDOWS)
    if (sections.contains("code-windows")) {
      expectedCode = sections["code-windows"];
    }
  #endif

  ASSERT_EQ(expectedCode, std::get<0>(result).substr(143 + std::string(EOL).size() * 7));
  ASSERT_EQ(sections["flags"], std::get<1>(result));

  auto fileName = std::string("build") + OS_PATH_SEP + param;
  auto filePath = fileName + OS_FILE_EXT;

  Codegen::compile(filePath, result, true);
  auto [actualStdout, actualStderr, actualReturnCode] = execCmd(filePath, fileName);
  std::filesystem::remove(filePath);
  actualStderr.erase(actualStderr.find_last_not_of("\r\n") + 1);

  EXPECT_EQ(sections["stderr"], actualStderr);
  EXPECT_NE(actualReturnCode, 0);
}

INSTANTIATE_TEST_SUITE_P(General, CodegenPassTest, testing::Values(
  "empty"
));

INSTANTIATE_TEST_SUITE_P(Builtin, CodegenPassTest, testing::Values(
  "builtin-array-alloc",
  "builtin-array-at",
  "builtin-array-join",
  "builtin-array-len",
  "builtin-array-pop",
  "builtin-array-push",
  "builtin-array-reverse",
  "builtin-array-slice",
  "builtin-array-str",
  "builtin-bool-str",
  "builtin-byte-str",
  "builtin-char-str",
  "builtin-exit-empty",
  "builtin-exit-one",
  "builtin-exit-zero",
  "builtin-f32-str",
  "builtin-f64-str",
  "builtin-float-str",
  "builtin-i8-str",
  "builtin-i16-str",
  "builtin-i32-str",
  "builtin-i64-str",
  "builtin-int-str",
  "builtin-opt-alloc",
  "builtin-opt-str",
  "builtin-print",
  "builtin-sleep",
  "builtin-str-at",
  "builtin-str-len",
  "builtin-str-slice",
  "builtin-u8-str",
  "builtin-u16-str",
  "builtin-u32-str",
  "builtin-u64-str"
));

INSTANTIATE_TEST_SUITE_P(ExprAccess, CodegenPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-str",
  "expr-access-obj",
  "expr-access-fn",
  "expr-access-array",
  "expr-access-opt"
));

INSTANTIATE_TEST_SUITE_P(ExprAssign, CodegenPassTest, testing::Values(
  "expr-assign",
  "expr-assign-op",
  "expr-assign-str",
  "expr-assign-elem",
  "expr-assign-array",
  "expr-assign-opt"
));

INSTANTIATE_TEST_SUITE_P(ExprBinary, CodegenPassTest, testing::Values(
  "expr-binary",
  "expr-binary-str",
  "expr-binary-opt",
  "expr-binary-nested"
));

INSTANTIATE_TEST_SUITE_P(ExprCall, CodegenPassTest, testing::Values(
  "expr-call",
  "expr-call-obj-prop",
  "expr-call-args"
));

INSTANTIATE_TEST_SUITE_P(ExprCond, CodegenPassTest, testing::Values(
  "expr-cond",
  "expr-cond-nested",
  "expr-cond-array",
  "expr-cond-str",
  "expr-cond-operands",
  "expr-cond-opt"
));

INSTANTIATE_TEST_SUITE_P(ExprLit, CodegenPassTest, testing::Values(
  "expr-lit-bool-false",
  "expr-lit-bool-true",
  "expr-lit-char",
  "expr-lit-char-esc",
  "expr-lit-float",
  "expr-lit-float-exp",
  "expr-lit-int-bin",
  "expr-lit-int-dec",
  "expr-lit-int-hex",
  "expr-lit-int-oct",
  "expr-lit-nil",
  "expr-lit-str",
  "expr-lit-str-esc"
));

INSTANTIATE_TEST_SUITE_P(ExprObj, CodegenPassTest, testing::Values(
  "expr-obj",
  "expr-obj-nested"
));

INSTANTIATE_TEST_SUITE_P(ExprRef, CodegenPassTest, testing::Values(
  "expr-ref",
  "expr-ref-str",
  "expr-ref-fn",
  "expr-ref-obj",
  "expr-ref-array",
  "expr-ref-opt"
));

INSTANTIATE_TEST_SUITE_P(ExprUnary, CodegenPassTest, testing::Values(
  "expr-unary",
  "expr-unary-nested",
  "expr-unary-str",
  "node-expr-unary-context"
));

INSTANTIATE_TEST_SUITE_P(NodeExpr, CodegenPassTest, testing::Values(
  "node-expr-access",
  "node-expr-access-str",
  "node-expr-access-obj",
  "node-expr-access-array",
  "node-expr-access-fn",
  "node-expr-access-opt",
  "node-expr-assign",
  "node-expr-assign-str",
  "node-expr-assign-obj",
  "node-expr-assign-array",
  "node-expr-assign-fn",
  "node-expr-assign-opt",
  "node-expr-binary",
  "node-expr-binary-str",
  "node-expr-binary-obj",
  "node-expr-call",
  "node-expr-cond",
  "node-expr-cond-str",
  "node-expr-cond-array",
  "node-expr-cond-obj",
  "node-expr-cond-fn",
  "node-expr-cond-opt",
  "node-expr-lit",
  "node-expr-obj",
  "node-expr-obj-str",
  "node-expr-obj-array",
  "node-expr-obj-fn",
  "node-expr-obj-opt",
  "node-expr-unary",
  "node-expr-unary-str",
  "node-expr-unary-obj"
));

INSTANTIATE_TEST_SUITE_P(NodeFnDecl, CodegenPassTest, testing::Values(
  "node-fn-decl-empty",
  "node-fn-decl-stack",
  "node-fn-decl-stack-str",
  "node-fn-decl-stack-obj",
  "node-fn-decl-stack-fn",
  "node-fn-decl-stack-array",
  "node-fn-decl-stack-opt",
  "node-fn-decl-scope",
  "node-fn-decl-param-default",
  "node-fn-decl-param-default-str",
  "node-fn-decl-param-default-array",
  "node-fn-decl-param-default-obj",
  "node-fn-decl-param-default-fn",
  "node-fn-decl-param-default-opt",
  "node-fn-decl-param-fn",
  "node-fn-decl-nested",
  "node-fn-decl-complex",
  "node-fn-decl-param-mut",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-param-mut-array",
  "node-fn-decl-param-mut-opt",
  "node-fn-decl-param-mut-str"
));

INSTANTIATE_TEST_SUITE_P(NodeIf, CodegenPassTest, testing::Values(
  "node-if",
  "node-if-cmp-num",
  "node-if-cmp-str",
  "node-if-cmp-opt"
));

INSTANTIATE_TEST_SUITE_P(NodeLoop, CodegenPassTest, testing::Values(
  "node-loop",
  "node-loop-empty",
  "node-loop-while",
  "node-loop-str",
  "node-loop-array",
  "node-loop-obj",
  "node-loop-opt",
  "node-loop-complex"
));

INSTANTIATE_TEST_SUITE_P(NodeReturn, CodegenPassTest, testing::Values(
  "node-return"
));

INSTANTIATE_TEST_SUITE_P(NodeObjDecl, CodegenPassTest, testing::Values(
  "node-obj-decl",
  "node-obj-decl-field-array",
  "node-obj-decl-field-fn",
  "node-obj-decl-field-obj",
  "node-obj-decl-field-opt",
  "node-obj-decl-field-str"
));

INSTANTIATE_TEST_SUITE_P(NodeVarDecl, CodegenPassTest, testing::Values(
  "node-var-decl-array",
  "node-var-decl-array-init",
  "node-var-decl-array-short",
  "node-var-decl-array-mut",
  "node-var-decl-array-mut-init",
  "node-var-decl-array-mut-short",
  "node-var-decl-bool",
  "node-var-decl-bool-init",
  "node-var-decl-bool-short",
  "node-var-decl-bool-mut",
  "node-var-decl-bool-mut-init",
  "node-var-decl-bool-mut-short",
  "node-var-decl-byte",
  "node-var-decl-byte-init",
  "node-var-decl-byte-mut",
  "node-var-decl-byte-mut-init",
  "node-var-decl-char",
  "node-var-decl-char-init",
  "node-var-decl-char-short",
  "node-var-decl-char-mut",
  "node-var-decl-char-mut-init",
  "node-var-decl-char-mut-short",
  "node-var-decl-f32",
  "node-var-decl-f32-init",
  "node-var-decl-f32-mut",
  "node-var-decl-f32-mut-init",
  "node-var-decl-f64",
  "node-var-decl-f64-init",
  "node-var-decl-f64-mut",
  "node-var-decl-f64-mut-init",
  "node-var-decl-float",
  "node-var-decl-float-init",
  "node-var-decl-float-short",
  "node-var-decl-float-mut",
  "node-var-decl-float-mut-init",
  "node-var-decl-float-mut-short",
  "node-var-decl-fn-init",
  "node-var-decl-fn-short",
  "node-var-decl-fn-mut-init",
  "node-var-decl-fn-mut-short",
  "node-var-decl-i8",
  "node-var-decl-i8-init",
  "node-var-decl-i8-mut",
  "node-var-decl-i8-mut-init",
  "node-var-decl-i16",
  "node-var-decl-i16-init",
  "node-var-decl-i16-mut",
  "node-var-decl-i16-mut-init",
  "node-var-decl-i32",
  "node-var-decl-i32-init",
  "node-var-decl-i32-mut",
  "node-var-decl-i32-mut-init",
  "node-var-decl-i64",
  "node-var-decl-i64-init",
  "node-var-decl-i64-mut",
  "node-var-decl-i64-mut-init",
  "node-var-decl-int",
  "node-var-decl-int-init",
  "node-var-decl-int-short",
  "node-var-decl-int-mut",
  "node-var-decl-int-mut-init",
  "node-var-decl-int-mut-short",
  "node-var-decl-obj-init",
  "node-var-decl-obj-short",
  "node-var-decl-obj-mut-init",
  "node-var-decl-obj-mut-short",
  "node-var-decl-opt",
  "node-var-decl-opt-init",
  "node-var-decl-opt-mut",
  "node-var-decl-opt-mut-init",
  "node-var-decl-ref-init",
  "node-var-decl-ref-short",
  "node-var-decl-ref-mut-init",
  "node-var-decl-ref-mut-short",
  "node-var-decl-str",
  "node-var-decl-str-init",
  "node-var-decl-str-short",
  "node-var-decl-str-mut",
  "node-var-decl-str-mut-init",
  "node-var-decl-str-mut-short",
  "node-var-decl-u8",
  "node-var-decl-u8-init",
  "node-var-decl-u8-mut",
  "node-var-decl-u8-mut-init",
  "node-var-decl-u16",
  "node-var-decl-u16-init",
  "node-var-decl-u16-mut",
  "node-var-decl-u16-mut-init",
  "node-var-decl-u32",
  "node-var-decl-u32-init",
  "node-var-decl-u32-mut",
  "node-var-decl-u32-mut-init",
  "node-var-decl-u64",
  "node-var-decl-u64-init",
  "node-var-decl-u64-mut",
  "node-var-decl-u64-mut-init"
));

INSTANTIATE_TEST_SUITE_P(Builtin, CodegenThrowTest, testing::Values(
  "throw-builtin-array-at-empty",
  "throw-builtin-array-at-high",
  "throw-builtin-array-at-low",
  "throw-builtin-str-at-empty",
  "throw-builtin-str-at-high",
  "throw-builtin-str-at-low"
));
