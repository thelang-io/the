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

class CodegenTest : public testing::TestWithParam<const char *> {
 protected:
  bool testMemcheck_ = false;

  void SetUp () override {
    auto envVars = getEnvVars();
    this->testMemcheck_ = envVars.contains("CODEGEN_MEMCHECK") && envVars["CODEGEN_MEMCHECK"] == "ON";
  }
};

TEST_P(CodegenTest, Passes) {
  auto param = std::string(testing::TestWithParam<const char *>::GetParam());
  auto sections = readTestFile("codegen", param, {"stdin", "code", "code-windows", "flags", "stdout"});
  auto ast = testing::NiceMock<MockAST>(sections["stdin"]);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();
  auto expectedCode = sections["code"];

  #ifdef OS_WINDOWS
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
  EXPECT_EQ(sections["stdout"], actualStdout);
  EXPECT_EQ(actualReturnCode, 0);

  if (!this->testMemcheck_) {
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

INSTANTIATE_TEST_SUITE_P(ExprBinary, CodegenTest, testing::Values(
  "expr-binary",
  "expr-binary-str",
  "expr-binary-nested"
));

INSTANTIATE_TEST_SUITE_P(ExprCall, CodegenTest, testing::Values(
  "expr-call",
  "expr-call-obj-prop"
));

INSTANTIATE_TEST_SUITE_P(ExprCond, CodegenTest, testing::Values(
  "expr-cond",
  "expr-cond-nested",
  "expr-cond-str",
  "expr-cond-operands"
));

INSTANTIATE_TEST_SUITE_P(ExprLit, CodegenTest, testing::Values(
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
  "expr-lit-str",
  "expr-lit-str-esc"
));

INSTANTIATE_TEST_SUITE_P(ExprObj, CodegenTest, testing::Values(
  "expr-obj",
  "expr-obj-nested"
));

INSTANTIATE_TEST_SUITE_P(ExprUnary, CodegenTest, testing::Values(
  "expr-unary",
  "expr-unary-nested",
  "expr-unary-str"
));

INSTANTIATE_TEST_SUITE_P(NodeExpr, CodegenTest, testing::Values(
  "node-expr-access",
  "node-expr-access-str",
  "node-expr-access-obj",
  "node-expr-assign",
  "node-expr-assign-str",
  "node-expr-assign-obj",
  "node-expr-binary",
  "node-expr-binary-str",
  "node-expr-binary-obj",
  "node-expr-call",
  "node-expr-cond",
  "node-expr-cond-str",
  "node-expr-cond-obj",
  "node-expr-lit",
  "node-expr-obj",
  "node-expr-obj-str",
  "node-expr-unary",
  "node-expr-unary-str",
  "node-expr-unary-obj"
));

INSTANTIATE_TEST_SUITE_P(NodeFnDecl, CodegenTest, testing::Values(
  "node-fn-decl-empty",
  "node-fn-decl-stack",
  "node-fn-decl-stack-str",
  "node-fn-decl-stack-obj",
  "node-fn-decl-stack-fn",
  "node-fn-decl-scope",
  "node-fn-decl-param-default",
  "node-fn-decl-param-default-str",
  "node-fn-decl-param-default-obj",
  "node-fn-decl-param-default-fn",
  "node-fn-decl-param-fn",
  "node-fn-decl-nested",
  "node-fn-decl-complex"
));

INSTANTIATE_TEST_SUITE_P(NodeIf, CodegenTest, testing::Values(
  "node-if",
  "node-if-cmp-num",
  "node-if-cmp-str"
));

INSTANTIATE_TEST_SUITE_P(NodeLoop, CodegenTest, testing::Values(
  "node-loop",
  "node-loop-empty",
  "node-loop-while",
  "node-loop-str"
));

INSTANTIATE_TEST_SUITE_P(NodeObjDecl, CodegenTest, testing::Values(
  "node-obj-decl"
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
