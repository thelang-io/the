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
#include <filesystem>
#include <iostream>
#include <regex>
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
  bool testCompile_ = false;
  bool testMemcheck_ = false;

  void SetUp () override {
    auto testCompile = getEnvVar("TEST_CODEGEN_COMPILE");
    auto testMemcheck = getEnvVar("TEST_CODEGEN_MEMCHECK");

    this->testCompile_ = testCompile != std::nullopt && testCompile == "ON";
    this->testMemcheck_ = testMemcheck != std::nullopt && testMemcheck == "ON";
  }
};

class CodegenThrowTest : public testing::TestWithParam<const char *> {
 protected:
  bool testCompile_ = false;

  void SetUp () override {
    auto testCompile = getEnvVar("TEST_CODEGEN_COMPILE");
    this->testCompile_ = testCompile != std::nullopt && testCompile == "ON";
  }
};

TEST_P(CodegenPassTest, Passes) {
  auto param = std::string(testing::TestWithParam<const char *>::GetParam());
  auto sections = readTestFile("codegen", param, {"stdin", "code", "flags", "stdout"});
  auto ast = testing::NiceMock<MockAST>(sections["stdin"]);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();
  auto expectedCode = sections["code"];
  auto expectedOutput = sections["stdout"];

  ASSERT_EQ(expectedCode, std::get<0>(result).substr(148 + std::string(EOL).size() * 7));
  ASSERT_EQ(sections["flags"], std::get<1>(result));

  if (!this->testCompile_) {
    return;
  }

  auto fileName = std::string("build") + OS_PATH_SEP + param;
  auto filePath = fileName + OS_FILE_EXT;
  Codegen::compile(filePath, result, "default", true);

  auto [actualStdout, actualStderr, actualReturnCode] = execCmd(
    (this->testMemcheck_ ? "valgrind " + valgrindArguments + " " : "") + filePath,
    fileName
  );

  std::filesystem::remove(filePath);

  while (expectedOutput.find("{{ ") != std::string::npos) {
    auto placeholderStart = expectedOutput.find("{{ ");
    auto placeholderEnd = expectedOutput.find(" }}") + 3;
    auto placeholderLen = placeholderEnd - placeholderStart;
    auto placeholderRegexPattern = expectedOutput.substr(placeholderStart + 3, placeholderLen - 6);
    auto placeholderRegex = std::regex(placeholderRegexPattern);

    const auto actualStdoutSlice = actualStdout.substr(placeholderStart);
    auto match = std::smatch();
    std::regex_search(actualStdoutSlice, match, placeholderRegex);

    auto placeholderValue = actualStdout.substr(placeholderStart, match[0].length());
    expectedOutput.replace(placeholderStart, placeholderLen, placeholderValue);
  }

  EXPECT_EQ(expectedOutput, actualStdout);

  if (param == "builtin-exit-one") {
    EXPECT_EQ(actualReturnCode, 1);
  } else {
    EXPECT_EQ(actualReturnCode, 0);
  }

  if (!this->testMemcheck_ && !std::set<std::string>{"builtin-print-to"}.contains(param)) {
    EXPECT_EQ(actualStderr, "");
  } else if (actualReturnCode != 0) {
    std::cout << actualStderr;
  }
}

TEST_P(CodegenThrowTest, Throws) {
  auto param = std::string(testing::TestWithParam<const char *>::GetParam());
  auto sections = readTestFile("codegen", param, {"stdin", "code", "flags", "stderr"});
  auto ast = testing::NiceMock<MockAST>(sections["stdin"]);
  auto codegen = Codegen(&ast);
  auto result = codegen.gen();
  auto expectedCode = sections["code"];
  auto expectedStderr = sections["stderr"];

  ASSERT_EQ(expectedCode, std::get<0>(result).substr(148 + std::string(EOL).size() * 7));
  ASSERT_EQ(sections["flags"], std::get<1>(result));

  if (!this->testCompile_) {
    return;
  }

  auto fileName = std::string("build") + OS_PATH_SEP + param;
  auto filePath = fileName + OS_FILE_EXT;

  Codegen::compile(filePath, result, "default", true);
  auto [actualStdout, actualStderr, actualReturnCode] = execCmd(filePath, fileName);
  std::filesystem::remove(filePath);
  actualStderr.erase(actualStderr.find_last_not_of("\r\n") + 1);

  while (expectedStderr.find("{{ ") != std::string::npos) {
    auto placeholderStart = expectedStderr.find("{{ ");
    auto placeholderEnd = expectedStderr.find(" }}") + 3;
    auto placeholderLen = placeholderEnd - placeholderStart;
    auto placeholderRegexPattern = expectedStderr.substr(placeholderStart + 3, placeholderLen - 6);
    auto placeholderRegex = std::regex(placeholderRegexPattern);

    const auto actualStderrSlice = actualStderr.substr(placeholderStart);
    auto match = std::smatch();
    std::regex_search(actualStderrSlice, match, placeholderRegex);

    auto placeholderValue = actualStderr.substr(placeholderStart, match[0].length());
    expectedStderr.replace(placeholderStart, placeholderLen, placeholderValue);
  }

  EXPECT_EQ(expectedStderr, actualStderr);
  EXPECT_NE(actualReturnCode, 0);
}

INSTANTIATE_TEST_SUITE_P(General, CodegenPassTest, testing::Values(
  "empty"
));

INSTANTIATE_TEST_SUITE_P(Builtin, CodegenPassTest, testing::Values(
  "builtin-any-alloc",
  "builtin-any-str",
  "builtin-any-str-root",
  "builtin-array-alloc",
  "builtin-array-alloc-root",
  "builtin-array-at",
  "builtin-array-at-root",
  "builtin-array-eq",
  "builtin-array-join",
  "builtin-array-join-root",
  "builtin-array-len",
  "builtin-array-len-root",
  "builtin-array-pop",
  "builtin-array-push",
  "builtin-array-reverse",
  "builtin-array-reverse-root",
  "builtin-array-slice",
  "builtin-array-slice-root",
  "builtin-array-str",
  "builtin-array-str-root",
  "builtin-bool-str",
  "builtin-bool-str-root",
  "builtin-buffer-eq",
  "builtin-buffer-str",
  "builtin-buffer-str-root",
  "builtin-byte-str",
  "builtin-byte-str-root",
  "builtin-char-is-alpha",
  "builtin-char-is-alpha-root",
  "builtin-char-is-alpha-num",
  "builtin-char-is-alpha-num-root",
  "builtin-char-is-digit",
  "builtin-char-is-digit-root",
  "builtin-char-is-space",
  "builtin-char-is-space-root",
  "builtin-char-repeat",
  "builtin-char-repeat-root",
  "builtin-char-str",
  "builtin-char-str-root",
  "builtin-exit-empty",
  "builtin-exit-one",
  "builtin-exit-zero",
  "builtin-f32-str",
  "builtin-f32-str-root",
  "builtin-f64-str",
  "builtin-f64-str-root",
  "builtin-float-str",
  "builtin-float-str-root",
  "builtin-fn-str",
  "builtin-fn-str-root",
  "builtin-fs-chmod-sync",
  "builtin-fs-chown-sync",
  "builtin-fs-exists-sync",
  "builtin-fs-exists-sync-root",
  "builtin-fs-is-absolute-sync",
  "builtin-fs-is-absolute-sync-root",
  "builtin-fs-is-directory-sync",
  "builtin-fs-is-directory-sync-root",
  "builtin-fs-is-file-sync",
  "builtin-fs-is-file-sync-root",
  "builtin-fs-is-symbolic-link-sync",
  "builtin-fs-is-symbolic-link-sync-root",
  "builtin-fs-link-sync",
  "builtin-fs-mkdir-sync",
  "builtin-fs-read-file-sync",
  "builtin-fs-read-file-sync-root",
  "builtin-fs-realpath-sync",
  "builtin-fs-realpath-sync-root",
  "builtin-fs-rm-sync",
  "builtin-fs-rmdir-sync",
  "builtin-fs-scandir-sync",
  "builtin-fs-scandir-sync-root",
  "builtin-i8-str",
  "builtin-i8-str-root",
  "builtin-i16-str",
  "builtin-i16-str-root",
  "builtin-i32-str",
  "builtin-i32-str-root",
  "builtin-i64-str",
  "builtin-i64-str-root",
  "builtin-int-str",
  "builtin-int-str-root",
  "builtin-obj-eq",
  "builtin-obj-str",
  "builtin-obj-str-root",
  "builtin-opt-alloc",
  "builtin-opt-eq",
  "builtin-opt-str",
  "builtin-opt-str-root",
  "builtin-os-eol",
  "builtin-os-eol-root",
  "builtin-os-name",
  "builtin-os-name-root",
  "builtin-path-basename",
  "builtin-path-basename-root",
  "builtin-path-dirname",
  "builtin-path-dirname-root",
  "builtin-print",
  "builtin-print-to",
  "builtin-process-args",
  "builtin-process-args-root",
  "builtin-process-cwd",
  "builtin-process-cwd-root",
  "builtin-process-getgid",
  "builtin-process-getgid-root",
  "builtin-process-getuid",
  "builtin-process-getuid-root",
  "builtin-process-run-sync",
  "builtin-process-run-sync-root",
  "builtin-sleep-sync",
  "builtin-str-at",
  "builtin-str-at-root",
  "builtin-str-find",
  "builtin-str-find-root",
  "builtin-str-len",
  "builtin-str-len-root",
  "builtin-str-lines",
  "builtin-str-lines-root",
  "builtin-str-lower",
  "builtin-str-lower-root",
  "builtin-str-lower-first",
  "builtin-str-lower-first-root",
  "builtin-str-slice",
  "builtin-str-slice-root",
  "builtin-str-trim",
  "builtin-str-trim-root",
  "builtin-str-upper",
  "builtin-str-upper-root",
  "builtin-str-upper-first",
  "builtin-str-upper-first-root",
  "builtin-u8-str",
  "builtin-u8-str-root",
  "builtin-u16-str",
  "builtin-u16-str-root",
  "builtin-u32-str",
  "builtin-u32-str-root",
  "builtin-u64-str",
  "builtin-u64-str-root"
));

INSTANTIATE_TEST_SUITE_P(ExprAccess, CodegenPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-any",
  "expr-access-array",
  "expr-access-fn",
  "expr-access-obj",
  "expr-access-opt",
  "expr-access-str"
));

INSTANTIATE_TEST_SUITE_P(ExprAssign, CodegenPassTest, testing::Values(
  "expr-assign",
  "expr-assign-op",
  "expr-assign-elem",
  "expr-assign-any",
  "expr-assign-array",
  "expr-assign-opt",
  "expr-assign-str"
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
  "expr-cond-any",
  "expr-cond-array",
  "expr-cond-opt",
  "expr-cond-str",
  "expr-cond-operands"
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
  "expr-ref-any",
  "expr-ref-array",
  "expr-ref-fn",
  "expr-ref-obj",
  "expr-ref-opt",
  "expr-ref-str"
));

INSTANTIATE_TEST_SUITE_P(ExprUnary, CodegenPassTest, testing::Values(
  "expr-unary",
  "expr-unary-nested",
  "expr-unary-str"
));

INSTANTIATE_TEST_SUITE_P(NodeExpr, CodegenPassTest, testing::Values(
  "node-expr-access",
  "node-expr-access-any",
  "node-expr-access-array",
  "node-expr-access-fn",
  "node-expr-access-obj",
  "node-expr-access-opt",
  "node-expr-access-str",
  "node-expr-assign",
  "node-expr-assign-any",
  "node-expr-assign-array",
  "node-expr-assign-fn",
  "node-expr-assign-obj",
  "node-expr-assign-opt",
  "node-expr-assign-str",
  "node-expr-binary",
  "node-expr-binary-obj",
  "node-expr-binary-opt",
  "node-expr-binary-str",
  "node-expr-call",
  "node-expr-cond",
  "node-expr-cond-any",
  "node-expr-cond-array",
  "node-expr-cond-fn",
  "node-expr-cond-obj",
  "node-expr-cond-opt",
  "node-expr-cond-str",
  "node-expr-lit",
  "node-expr-obj",
  "node-expr-obj-any",
  "node-expr-obj-array",
  "node-expr-obj-fn",
  "node-expr-obj-obj",
  "node-expr-obj-opt",
  "node-expr-obj-str",
  "node-expr-unary",
  "node-expr-unary-context",
  "node-expr-unary-obj",
  "node-expr-unary-str"
));

INSTANTIATE_TEST_SUITE_P(NodeFnDecl, CodegenPassTest, testing::Values(
  "node-fn-decl-empty",
  "node-fn-decl-hoisting",
  "node-fn-decl-param-default",
  "node-fn-decl-param-default-any",
  "node-fn-decl-param-default-array",
  "node-fn-decl-param-default-fn",
  "node-fn-decl-param-default-obj",
  "node-fn-decl-param-default-opt",
  "node-fn-decl-param-default-str",
  "node-fn-decl-param-fn",
  "node-fn-decl-nested",
  "node-fn-decl-complex",
  "node-fn-decl-param-mut",
  "node-fn-decl-param-mut-any",
  "node-fn-decl-param-mut-array",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-mut-opt",
  "node-fn-decl-param-mut-str",
  "node-fn-decl-scope",
  "node-fn-decl-stack",
  "node-fn-decl-stack-any",
  "node-fn-decl-stack-array",
  "node-fn-decl-stack-fn",
  "node-fn-decl-stack-obj",
  "node-fn-decl-stack-opt",
  "node-fn-decl-stack-str"
));

INSTANTIATE_TEST_SUITE_P(NodeIf, CodegenPassTest, testing::Values(
  "node-if",
  "node-if-cmp-num",
  "node-if-cmp-opt",
  "node-if-cmp-str"
));

INSTANTIATE_TEST_SUITE_P(NodeLoop, CodegenPassTest, testing::Values(
  "node-loop",
  "node-loop-empty",
  "node-loop-while",
  "node-loop-array",
  "node-loop-str",
  "node-loop-obj",
  "node-loop-opt",
  "node-loop-complex"
));

INSTANTIATE_TEST_SUITE_P(NodeReturn, CodegenPassTest, testing::Values(
  "node-return"
));

INSTANTIATE_TEST_SUITE_P(NodeObjDecl, CodegenPassTest, testing::Values(
  "node-obj-decl",
  "node-obj-decl-default",
  "node-obj-decl-field-any",
  "node-obj-decl-field-array",
  "node-obj-decl-field-fn",
  "node-obj-decl-field-obj",
  "node-obj-decl-field-opt",
  "node-obj-decl-field-str",
  "node-obj-decl-forward-decl",
  "node-obj-decl-mut"
));

INSTANTIATE_TEST_SUITE_P(NodeVarDecl, CodegenPassTest, testing::Values(
  "node-var-decl-any",
  "node-var-decl-any-init",
  "node-var-decl-any-mut",
  "node-var-decl-any-mut-init",
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
  "throw-builtin-fs-chmod-sync-not-existing",
  "throw-builtin-fs-chown-sync-not-existing",
  "throw-builtin-fs-link-sync-existing",
  "throw-builtin-fs-link-sync-same-name",
  "throw-builtin-fs-mkdir-sync-existing",
  "throw-builtin-fs-read-file-sync-non-existing",
  "throw-builtin-fs-realpath-sync-non-existing",
  "throw-builtin-process-run-sync-exit-code",
  "throw-builtin-fs-rm-sync-directory",
  "throw-builtin-fs-rm-sync-non-existing",
  "throw-builtin-fs-rmdir-sync-file",
  "throw-builtin-fs-rmdir-sync-non-existing",
  "throw-builtin-fs-scandir-sync-non-existing",
  "throw-builtin-str-at-empty",
  "throw-builtin-str-at-high",
  "throw-builtin-str-at-low"
));
