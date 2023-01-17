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

const auto passExitTests = std::set<std::string>{
  "builtin-exit-one"
};

const auto passStderrTests = std::set<std::string>{
  "builtin-print-to"
};

TEST(CodegenTest, GetsEnvVar) {
  EXPECT_NE(Codegen::getEnvVar("PATH"), "");
  EXPECT_EQ(Codegen::getEnvVar("test_non_existing"), "");
}

TEST(CodegenTest, StringifyFlags) {
  EXPECT_EQ(Codegen::stringifyFlags({}), "");
  EXPECT_EQ(Codegen::stringifyFlags({""}), "");
  EXPECT_EQ(Codegen::stringifyFlags({"test"}), "test");
  EXPECT_EQ(Codegen::stringifyFlags({"test1", "test2"}), "test1 test2");
  EXPECT_EQ(Codegen::stringifyFlags({"test1", "test2", "test3"}), "test1 test2 test3");
}

class CodegenPassTest : public testing::TestWithParam<const char *> {
 protected:
  bool isPlatformDefault_ = true;
  bool testCompile_ = false;
  bool testMemcheck_ = false;
  std::string testPlatform_ = "default";

  void SetUp () override {
    auto testCompile = getEnvVar("TEST_CODEGEN_COMPILE");
    auto testMemcheck = getEnvVar("TEST_CODEGEN_MEMCHECK");
    auto testPlatform = getEnvVar("TEST_CODEGEN_PLATFORM");

    this->testCompile_ = testCompile != std::nullopt && testCompile == "ON";
    this->testMemcheck_ = testMemcheck != std::nullopt && testMemcheck == "ON";

    if (testPlatform != std::nullopt) {
      this->testPlatform_ = *testPlatform;
      this->isPlatformDefault_ = this->testPlatform_ == "default";
    }
  }
};

class CodegenThrowTest : public testing::TestWithParam<const char *> {
 protected:
  bool isPlatformDefault_ = true;
  bool testCompile_ = false;
  std::string testPlatform_ = "default";

  void SetUp () override {
    auto testCompile = getEnvVar("TEST_CODEGEN_COMPILE");
    auto testPlatform = getEnvVar("TEST_CODEGEN_PLATFORM");

    this->testCompile_ = testCompile != std::nullopt && testCompile == "ON";

    if (testPlatform != std::nullopt) {
      this->testPlatform_ = *testPlatform;
      this->isPlatformDefault_ = this->testPlatform_ == "default";
    }
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
  ASSERT_EQ(sections["flags"], Codegen::stringifyFlags(std::get<1>(result)));

  if (!this->testCompile_) {
    return;
  }

  auto fileName = std::string("build") + OS_PATH_SEP + param;
  auto filePath = fileName + OS_FILE_EXT;
  Codegen::compile(filePath, result, this->testPlatform_, true);

  if (!this->isPlatformDefault_) {
    std::filesystem::remove(filePath);
    return;
  }

  auto cmd = (this->testMemcheck_ ? "valgrind " + valgrindArguments + " " : "") + filePath;
  auto [actualStdout, actualStderr, actualReturnCode] = execCmd(cmd, fileName);
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

  if (!passExitTests.contains(param)) {
    EXPECT_EQ(actualReturnCode, 0);
  }

  if (!this->testMemcheck_ && !passStderrTests.contains(param)) {
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
  ASSERT_EQ(sections["flags"], Codegen::stringifyFlags(std::get<1>(result)));

  if (!this->testCompile_) {
    return;
  }

  auto fileName = std::string("build") + OS_PATH_SEP + param;
  auto filePath = fileName + OS_FILE_EXT;
  Codegen::compile(filePath, result, this->testPlatform_, true);

  if (!this->isPlatformDefault_) {
    std::filesystem::remove(filePath);
    return;
  }

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

INSTANTIATE_TEST_SUITE_P(BuiltinGlobal, CodegenPassTest, testing::Values(
  "builtin-exit-empty",
  "builtin-exit-one",
  "builtin-exit-zero",
  "builtin-print",
  "builtin-print-to",
  "builtin-sleep-sync"
));

INSTANTIATE_TEST_SUITE_P(BuiltinAny, CodegenPassTest, testing::Values(
  "builtin-any-alloc",
  "builtin-any-str",
  "builtin-any-str-ref",
  "builtin-any-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinArray, CodegenPassTest, testing::Values(
  "builtin-array-alloc",
  "builtin-array-alloc-root",
  "builtin-array-at",
  "builtin-array-at-ref",
  "builtin-array-at-root",
  "builtin-array-empty",
  "builtin-array-empty-ref",
  "builtin-array-empty-root",
  "builtin-array-eq",
  "builtin-array-eq-ref",
  "builtin-array-eq-root",
  "builtin-array-join",
  "builtin-array-join-ref",
  "builtin-array-join-root",
  "builtin-array-len",
  "builtin-array-len-ref",
  "builtin-array-len-root",
  "builtin-array-pop",
  "builtin-array-pop-ref",
  "builtin-array-pop-root",
  "builtin-array-push",
  "builtin-array-push-ref",
  "builtin-array-reverse",
  "builtin-array-reverse-ref",
  "builtin-array-reverse-root",
  "builtin-array-slice",
  "builtin-array-slice-ref",
  "builtin-array-slice-root",
  "builtin-array-str",
  "builtin-array-str-ref",
  "builtin-array-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinBool, CodegenPassTest, testing::Values(
  "builtin-bool-str",
  "builtin-bool-str-ref",
  "builtin-bool-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinBuffer, CodegenPassTest, testing::Values(
  "builtin-buffer-eq",
  "builtin-buffer-eq-ref",
  "builtin-buffer-eq-root",
  "builtin-buffer-str",
  "builtin-buffer-str-ref",
  "builtin-buffer-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinByte, CodegenPassTest, testing::Values(
  "builtin-byte-str",
  "builtin-byte-str-ref",
  "builtin-byte-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinChar, CodegenPassTest, testing::Values(
  "builtin-char-is-alpha",
  "builtin-char-is-alpha-ref",
  "builtin-char-is-alpha-root",
  "builtin-char-is-alpha-num",
  "builtin-char-is-alpha-num-ref",
  "builtin-char-is-alpha-num-root",
  "builtin-char-is-digit",
  "builtin-char-is-digit-ref",
  "builtin-char-is-digit-root",
  "builtin-char-is-space",
  "builtin-char-is-space-ref",
  "builtin-char-is-space-root",
  "builtin-char-repeat",
  "builtin-char-repeat-ref",
  "builtin-char-repeat-root",
  "builtin-char-str",
  "builtin-char-str-ref",
  "builtin-char-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinEnum, CodegenPassTest, testing::Values(
  "builtin-enum-rawValue",
  "builtin-enum-rawValue-ref",
  "builtin-enum-rawValue-root",
  "builtin-enum-str",
  "builtin-enum-str-ref",
  "builtin-enum-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinF32, CodegenPassTest, testing::Values(
  "builtin-f32-str",
  "builtin-f32-str-ref",
  "builtin-f32-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinF64, CodegenPassTest, testing::Values(
  "builtin-f64-str",
  "builtin-f64-str-ref",
  "builtin-f64-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinFloat, CodegenPassTest, testing::Values(
  "builtin-float-str",
  "builtin-float-str-ref",
  "builtin-float-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinFn, CodegenPassTest, testing::Values(
  "builtin-fn-str",
  "builtin-fn-str-ref",
  "builtin-fn-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinFS, CodegenPassTest, testing::Values(
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
  "builtin-fs-stat-sync",
  "builtin-fs-stat-sync-root",
  "builtin-fs-unlink-sync",
  "builtin-fs-write-file-sync"
));

INSTANTIATE_TEST_SUITE_P(BuiltinI8, CodegenPassTest, testing::Values(
  "builtin-i8-str",
  "builtin-i8-str-ref",
  "builtin-i8-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinI16, CodegenPassTest, testing::Values(
  "builtin-i16-str",
  "builtin-i16-str-ref",
  "builtin-i16-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinI32, CodegenPassTest, testing::Values(
  "builtin-i32-str",
  "builtin-i32-str-ref",
  "builtin-i32-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinI64, CodegenPassTest, testing::Values(
  "builtin-i64-str",
  "builtin-i64-str-ref",
  "builtin-i64-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinInt, CodegenPassTest, testing::Values(
  "builtin-int-str",
  "builtin-int-str-ref",
  "builtin-int-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinObj, CodegenPassTest, testing::Values(
  "builtin-obj-eq",
  "builtin-obj-eq-ref",
  "builtin-obj-eq-root",
  "builtin-obj-str",
  "builtin-obj-str-ref",
  "builtin-obj-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinOpt, CodegenPassTest, testing::Values(
  "builtin-opt-alloc",
  "builtin-opt-eq",
  "builtin-opt-eq-ref",
  "builtin-opt-eq-root",
  "builtin-opt-str",
  "builtin-opt-str-ref",
  "builtin-opt-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinOS, CodegenPassTest, testing::Values(
  "builtin-os-eol",
  "builtin-os-eol-root",
  "builtin-os-name",
  "builtin-os-name-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinPath, CodegenPassTest, testing::Values(
  "builtin-path-basename",
  "builtin-path-basename-root",
  "builtin-path-dirname",
  "builtin-path-dirname-root",
  "builtin-path-sep",
  "builtin-path-sep-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinProcess, CodegenPassTest, testing::Values(
  "builtin-process-args",
  "builtin-process-args-root",
  "builtin-process-args-scoped",
  "builtin-process-cwd",
  "builtin-process-cwd-root",
  "builtin-process-getgid",
  "builtin-process-getgid-root",
  "builtin-process-getuid",
  "builtin-process-getuid-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinRequest, CodegenPassTest, testing::Values(
  "builtin-request-close",
  "builtin-request-open",
  "builtin-request-open-params",
  "builtin-request-open-root",
  "builtin-request-open-sleep",
  "builtin-request-read",
  "builtin-request-read-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinStr, CodegenPassTest, testing::Values(
  "builtin-str-at",
  "builtin-str-at-ref",
  "builtin-str-at-root",
  "builtin-str-empty",
  "builtin-str-empty-ref",
  "builtin-str-empty-root",
  "builtin-str-find",
  "builtin-str-find-ref",
  "builtin-str-find-root",
  "builtin-str-len",
  "builtin-str-len-ref",
  "builtin-str-len-root",
  "builtin-str-lines",
  "builtin-str-lines-ref",
  "builtin-str-lines-root",
  "builtin-str-lower",
  "builtin-str-lower-ref",
  "builtin-str-lower-root",
  "builtin-str-lower-first",
  "builtin-str-lower-first-ref",
  "builtin-str-lower-first-root",
  "builtin-str-slice",
  "builtin-str-slice-ref",
  "builtin-str-slice-root",
  "builtin-str-trim",
  "builtin-str-trim-ref",
  "builtin-str-trim-root",
  "builtin-str-upper",
  "builtin-str-upper-ref",
  "builtin-str-upper-root",
  "builtin-str-upper-first",
  "builtin-str-upper-first-ref",
  "builtin-str-upper-first-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinU8, CodegenPassTest, testing::Values(
  "builtin-u8-str",
  "builtin-u8-str-ref",
  "builtin-u8-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinU16, CodegenPassTest, testing::Values(
  "builtin-u16-str",
  "builtin-u16-str-ref",
  "builtin-u16-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinU32, CodegenPassTest, testing::Values(
  "builtin-u32-str",
  "builtin-u32-str-ref",
  "builtin-u32-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinU64, CodegenPassTest, testing::Values(
  "builtin-u64-str",
  "builtin-u64-str-ref",
  "builtin-u64-str-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinURL, CodegenPassTest, testing::Values(
  "builtin-url-parse",
  "builtin-url-parse-root"
));

INSTANTIATE_TEST_SUITE_P(ExprAccess, CodegenPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-any",
  "expr-access-array",
  "expr-access-enum",
  "expr-access-fn",
  "expr-access-obj",
  "expr-access-opt",
  "expr-access-str"
));

INSTANTIATE_TEST_SUITE_P(ExprArray, CodegenPassTest, testing::Values(
  "expr-array",
  "expr-array-any",
  "expr-array-array",
  "expr-array-enum",
  "expr-array-fn",
  "expr-array-obj",
  "expr-array-opt",
  "expr-array-str"
));

INSTANTIATE_TEST_SUITE_P(ExprAssign, CodegenPassTest, testing::Values(
  "expr-assign",
  "expr-assign-op",
  "expr-assign-elem",
  "expr-assign-any",
  "expr-assign-array",
  "expr-assign-enum",
  "expr-assign-obj",
  "expr-assign-opt",
  "expr-assign-ref",
  "expr-assign-str"
));

INSTANTIATE_TEST_SUITE_P(ExprBinary, CodegenPassTest, testing::Values(
  "expr-binary",
  "expr-binary-array",
  "expr-binary-enum",
  "expr-binary-obj",
  "expr-binary-opt",
  "expr-binary-str",
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
  "expr-cond-enum",
  "expr-cond-opt",
  "expr-cond-str",
  "expr-cond-operands",
  "expr-cond-type-casts"
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
  "expr-obj-empty",
  "expr-obj-nested"
));

INSTANTIATE_TEST_SUITE_P(ExprRef, CodegenPassTest, testing::Values(
  "expr-ref",
  "expr-ref-any",
  "expr-ref-array",
  "expr-ref-enum",
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

INSTANTIATE_TEST_SUITE_P(NodeEnumDecl, CodegenPassTest, testing::Values(
  "node-enum-decl",
  "node-enum-decl-init",
  "node-enum-decl-scoped",
  "node-enum-decl-hoisting"
));

INSTANTIATE_TEST_SUITE_P(NodeExpr, CodegenPassTest, testing::Values(
  "node-expr-access",
  "node-expr-access-any",
  "node-expr-access-array",
  "node-expr-access-enum",
  "node-expr-access-fn",
  "node-expr-access-obj",
  "node-expr-access-opt",
  "node-expr-access-str",
  "node-expr-array",
  "node-expr-array-any",
  "node-expr-array-array",
  "node-expr-array-enum",
  "node-expr-array-fn",
  "node-expr-array-obj",
  "node-expr-array-opt",
  "node-expr-array-str",
  "node-expr-assign",
  "node-expr-assign-any",
  "node-expr-assign-array",
  "node-expr-assign-enum",
  "node-expr-assign-fn",
  "node-expr-assign-obj",
  "node-expr-assign-opt",
  "node-expr-assign-ref",
  "node-expr-assign-str",
  "node-expr-binary",
  "node-expr-binary-array",
  "node-expr-binary-enum",
  "node-expr-binary-obj",
  "node-expr-binary-opt",
  "node-expr-binary-str",
  "node-expr-call",
  "node-expr-cond",
  "node-expr-cond-any",
  "node-expr-cond-array",
  "node-expr-cond-enum",
  "node-expr-cond-fn",
  "node-expr-cond-obj",
  "node-expr-cond-opt",
  "node-expr-cond-str",
  "node-expr-lit",
  "node-expr-obj",
  "node-expr-obj-any",
  "node-expr-obj-array",
  "node-expr-obj-enum",
  "node-expr-obj-fn",
  "node-expr-obj-obj",
  "node-expr-obj-opt",
  "node-expr-obj-str",
  "node-expr-ref",
  "node-expr-ref-any",
  "node-expr-ref-array",
  "node-expr-ref-enum",
  "node-expr-ref-fn",
  "node-expr-ref-obj",
  "node-expr-ref-opt",
  "node-expr-ref-str",
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
  "node-fn-decl-param-default-enum",
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
  "node-fn-decl-param-mut-enum",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-mut-opt",
  "node-fn-decl-param-mut-str",
  "node-fn-decl-param-variadic",
  "node-fn-decl-scope",
  "node-fn-decl-stack",
  "node-fn-decl-stack-any",
  "node-fn-decl-stack-array",
  "node-fn-decl-stack-enum",
  "node-fn-decl-stack-fn",
  "node-fn-decl-stack-obj",
  "node-fn-decl-stack-opt",
  "node-fn-decl-stack-str"
));

INSTANTIATE_TEST_SUITE_P(NodeIf, CodegenPassTest, testing::Values(
  "node-if",
  "node-if-cmp-num",
  "node-if-cmp-array",
  "node-if-cmp-enum",
  "node-if-cmp-obj",
  "node-if-cmp-opt",
  "node-if-cmp-str",
  "node-if-type-casts"
));

INSTANTIATE_TEST_SUITE_P(NodeLoop, CodegenPassTest, testing::Values(
  "node-loop",
  "node-loop-empty",
  "node-loop-while",
  "node-loop-parenthesized",
  "node-loop-array",
  "node-loop-enum",
  "node-loop-obj",
  "node-loop-opt",
  "node-loop-str",
  "node-loop-complex"
));

INSTANTIATE_TEST_SUITE_P(NodeReturn, CodegenPassTest, testing::Values(
  "node-return",
  "node-return-scope-cleanup"
));

INSTANTIATE_TEST_SUITE_P(NodeObjDecl, CodegenPassTest, testing::Values(
  "node-obj-decl",
  "node-obj-decl-default",
  "node-obj-decl-default-obj",
  "node-obj-decl-field-any",
  "node-obj-decl-field-array",
  "node-obj-decl-field-enum",
  "node-obj-decl-field-fn",
  "node-obj-decl-field-obj",
  "node-obj-decl-field-opt",
  "node-obj-decl-field-str",
  "node-obj-decl-field-variadic",
  "node-obj-decl-forward-decl",
  "node-obj-decl-mut",
  "node-obj-decl-method",
  "node-obj-decl-method-forward-decl",
  "node-obj-decl-method-forward-decl-with-function",
  "node-obj-decl-method-variadic"
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
  "node-var-decl-enum-init",
  "node-var-decl-enum-short",
  "node-var-decl-enum-mut-init",
  "node-var-decl-enum-mut-short",
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
  "node-var-decl-obj",
  "node-var-decl-obj-init",
  "node-var-decl-obj-short",
  "node-var-decl-obj-mut",
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
  "throw-builtin-fs-scandir-sync-non-directory",
  "throw-builtin-fs-scandir-sync-non-existing",
  "throw-builtin-fs-stat-sync-non-existing",
  "throw-builtin-fs-unlink-sync-non-existing",
  "throw-builtin-fs-write-file-sync-directory",
  "throw-builtin-request-open-invalid-host",
  "throw-builtin-request-open-invalid-port",
  "throw-builtin-request-open-invalid-protocol",
  "throw-builtin-request-open-long-port",
  "throw-builtin-str-at-empty",
  "throw-builtin-str-at-high",
  "throw-builtin-str-at-low",
  "throw-builtin-url-parse-auth",
  "throw-builtin-url-parse-empty",
  "throw-builtin-url-parse-empty-hostname",
  "throw-builtin-url-parse-invalid-origin",
  "throw-builtin-url-parse-invalid-port",
  "throw-builtin-url-parse-invalid-protocol",
  "throw-builtin-url-parse-string"
));
