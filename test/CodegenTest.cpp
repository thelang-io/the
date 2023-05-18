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
  "--error-exitcode=255 "
  "--errors-for-leak-kinds=all "
  "--leak-check=full "
  "--quiet "
  "--show-leak-kinds=all "
  "--track-origins=yes"
);

const auto passExitTests = std::set<std::string>{
  "builtin-process-exit-one"
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

  #if defined(OS_MACOS)
    std::filesystem::remove_all(filePath + ".dSYM");
  #elif defined(OS_WINDOWS)
    std::filesystem::remove(fileName + ".ilk");
    std::filesystem::remove(fileName + ".pdb");
  #endif

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

  auto cmd = (this->testMemcheck_ ? "valgrind " + valgrindArguments + " " : "") + filePath;
  auto [actualStdout, actualStderr, actualReturnCode] = execCmd(cmd, fileName);
  std::filesystem::remove(filePath);

  #if defined(OS_MACOS)
    std::filesystem::remove_all(filePath + ".dSYM");
  #elif defined(OS_WINDOWS)
    std::filesystem::remove(fileName + ".ilk");
    std::filesystem::remove(fileName + ".pdb");
  #endif

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

INSTANTIATE_TEST_SUITE_P(BuiltinGlobals, CodegenPassTest, testing::Values(
  "builtin-print",
  "builtin-print-alias",
  "builtin-print-any",
  "builtin-print-array",
  "builtin-print-buffer",
  "builtin-print-enum",
  "builtin-print-fn",
  "builtin-print-map",
  "builtin-print-obj",
  "builtin-print-opt",
  "builtin-print-ref",
  "builtin-print-str",
  "builtin-print-union",
  "builtin-print-args",
  "builtin-print-to"
));

INSTANTIATE_TEST_SUITE_P(BuiltinAny, CodegenPassTest, testing::Values(
  "builtin-any-alloc",
  "builtin-any-alloc-alias",
  "builtin-any-alloc-any",
  "builtin-any-alloc-array",
  "builtin-any-alloc-enum",
  "builtin-any-alloc-fn",
  "builtin-any-alloc-map",
  "builtin-any-alloc-obj",
  "builtin-any-alloc-opt",
  "builtin-any-alloc-ref",
  "builtin-any-alloc-str",
  "builtin-any-alloc-union",
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
  "builtin-array-clear",
  "builtin-array-clear-ref",
  "builtin-array-clear-root",
  "builtin-array-concat",
  "builtin-array-concat-ref",
  "builtin-array-concat-root",
  "builtin-array-contains",
  "builtin-array-contains-ref",
  "builtin-array-contains-root",
  "builtin-array-empty",
  "builtin-array-empty-ref",
  "builtin-array-empty-root",
  "builtin-array-eq",
  "builtin-array-eq-ref",
  "builtin-array-eq-root",
  "builtin-array-filter",
  "builtin-array-filter-ref",
  "builtin-array-filter-root",
  "builtin-array-first",
  "builtin-array-first-ref",
  "builtin-array-first-root",
  "builtin-array-for-each",
  "builtin-array-for-each-ref",
  "builtin-array-join",
  "builtin-array-join-ref",
  "builtin-array-join-root",
  "builtin-array-last",
  "builtin-array-last-ref",
  "builtin-array-last-root",
  "builtin-array-len",
  "builtin-array-len-ref",
  "builtin-array-len-root",
  "builtin-array-merge",
  "builtin-array-merge-ref",
  "builtin-array-merge-root",
  "builtin-array-pop",
  "builtin-array-pop-ref",
  "builtin-array-pop-root",
  "builtin-array-push",
  "builtin-array-push-ref",
  "builtin-array-reverse",
  "builtin-array-remove",
  "builtin-array-remove-ref",
  "builtin-array-remove-root",
  "builtin-array-reverse-ref",
  "builtin-array-reverse-root",
  "builtin-array-slice",
  "builtin-array-slice-ref",
  "builtin-array-slice-root",
  "builtin-array-sort",
  "builtin-array-sort-ref",
  "builtin-array-sort-root",
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
  "builtin-char-byte",
  "builtin-char-byte-ref",
  "builtin-char-byte-root",
  "builtin-char-is-digit",
  "builtin-char-is-digit-ref",
  "builtin-char-is-digit-root",
  "builtin-char-is-letter",
  "builtin-char-is-letter-ref",
  "builtin-char-is-letter-root",
  "builtin-char-is-letter-or-digit",
  "builtin-char-is-letter-or-digit-ref",
  "builtin-char-is-letter-or-digit-root",
  "builtin-char-is-lower",
  "builtin-char-is-lower-ref",
  "builtin-char-is-lower-root",
  "builtin-char-is-upper",
  "builtin-char-is-upper-ref",
  "builtin-char-is-upper-root",
  "builtin-char-is-whitespace",
  "builtin-char-is-whitespace-ref",
  "builtin-char-is-whitespace-root",
  "builtin-char-lower",
  "builtin-char-lower-ref",
  "builtin-char-lower-root",
  "builtin-char-repeat",
  "builtin-char-repeat-ref",
  "builtin-char-repeat-root",
  "builtin-char-str",
  "builtin-char-str-ref",
  "builtin-char-str-root",
  "builtin-char-upper",
  "builtin-char-upper-ref",
  "builtin-char-upper-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinDate, CodegenPassTest, testing::Values(
  "builtin-date-now",
  "builtin-date-now-root"
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
  "builtin-fs-append-file-sync",
  "builtin-fs-append-file-sync-empty-data",
  "builtin-fs-append-file-sync-existing",
  "builtin-fs-chmod-sync",
  "builtin-fs-chown-sync",
  "builtin-fs-copy-directory-sync",
  "builtin-fs-copy-directory-sync-existing-directory",
  "builtin-fs-copy-directory-sync-existing-file",
  "builtin-fs-copy-directory-sync-empty",
  "builtin-fs-copy-file-sync",
  "builtin-fs-copy-file-sync-empty",
  "builtin-fs-copy-file-sync-big",
  "builtin-fs-copy-file-sync-existing",
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
  "builtin-fs-rename-sync-file",
  "builtin-fs-rename-sync-empty-directory",
  "builtin-fs-rename-sync-directory",
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

INSTANTIATE_TEST_SUITE_P(BuiltinMap, CodegenPassTest, testing::Values(
  "builtin-map-alloc",
  "builtin-map-cap",
  "builtin-map-cap-ref",
  "builtin-map-cap-root",
  "builtin-map-clear",
  "builtin-map-clear-ref",
  "builtin-map-clear-root",
  "builtin-map-empty",
  "builtin-map-empty-ref",
  "builtin-map-empty-root",
  "builtin-map-eq",
  "builtin-map-eq-ref",
  "builtin-map-eq-root",
  "builtin-map-get",
  "builtin-map-get-ref",
  "builtin-map-get-root",
  "builtin-map-has",
  "builtin-map-has-ref",
  "builtin-map-has-root",
  "builtin-map-keys",
  "builtin-map-keys-ref",
  "builtin-map-keys-root",
  "builtin-map-len",
  "builtin-map-len-ref",
  "builtin-map-len-root",
  "builtin-map-merge",
  "builtin-map-merge-ref",
  "builtin-map-merge-root",
  "builtin-map-remove",
  "builtin-map-remove-ref",
  "builtin-map-remove-root",
  "builtin-map-reserve",
  "builtin-map-reserve-ref",
  "builtin-map-reserve-root",
  "builtin-map-set",
  "builtin-map-set-ref",
  "builtin-map-set-root",
  "builtin-map-shrink",
  "builtin-map-shrink-ref",
  "builtin-map-shrink-root",
  "builtin-map-str",
  "builtin-map-str-ref",
  "builtin-map-str-root",
  "builtin-map-values",
  "builtin-map-values-ref",
  "builtin-map-values-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinMath, CodegenPassTest, testing::Values(
  "builtin-math-max-float",
  "builtin-math-max-float-root",
  "builtin-math-min-float",
  "builtin-math-min-float-root",
  "builtin-math-max-f32",
  "builtin-math-max-f32-root",
  "builtin-math-min-f32",
  "builtin-math-min-f32-root",
  "builtin-math-max-f64",
  "builtin-math-max-f64-root",
  "builtin-math-min-f64",
  "builtin-math-min-f64-root",
  "builtin-math-max-int",
  "builtin-math-max-int-root",
  "builtin-math-min-int",
  "builtin-math-min-int-root",
  "builtin-math-max-i8",
  "builtin-math-max-i8-root",
  "builtin-math-min-i8",
  "builtin-math-min-i8-root",
  "builtin-math-max-i16",
  "builtin-math-max-i16-root",
  "builtin-math-min-i16",
  "builtin-math-min-i16-root",
  "builtin-math-max-i32",
  "builtin-math-max-i32-root",
  "builtin-math-min-i32",
  "builtin-math-min-i32-root",
  "builtin-math-max-i64",
  "builtin-math-max-i64-root",
  "builtin-math-min-i64",
  "builtin-math-min-i64-root",
  "builtin-math-max-u8",
  "builtin-math-max-u8-root",
  "builtin-math-min-u8",
  "builtin-math-min-u8-root",
  "builtin-math-max-u16",
  "builtin-math-max-u16-root",
  "builtin-math-min-u16",
  "builtin-math-min-u16-root",
  "builtin-math-max-u32",
  "builtin-math-max-u32-root",
  "builtin-math-min-u32",
  "builtin-math-min-u32-root",
  "builtin-math-max-u64",
  "builtin-math-max-u64-root",
  "builtin-math-min-u64",
  "builtin-math-min-u64-root"
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
  "builtin-opt-alloc-alias",
  "builtin-opt-alloc-any",
  "builtin-opt-alloc-array",
  "builtin-opt-alloc-enum",
  "builtin-opt-alloc-fn",
  "builtin-opt-alloc-map",
  "builtin-opt-alloc-obj",
  "builtin-opt-alloc-opt",
  "builtin-opt-alloc-ref",
  "builtin-opt-alloc-str",
  "builtin-opt-alloc-union",
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
  "builtin-path-sep-root",
  "builtin-path-temp-directory",
  "builtin-path-temp-directory-root",
  "builtin-path-temp-file",
  "builtin-path-temp-file-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinProcess, CodegenPassTest, testing::Values(
  "builtin-process-args",
  "builtin-process-args-root",
  "builtin-process-args-scoped",
  "builtin-process-cwd",
  "builtin-process-cwd-root",
  "builtin-process-env",
  "builtin-process-env-root",
  "builtin-process-exit-empty",
  "builtin-process-exit-one",
  "builtin-process-exit-zero",
  "builtin-process-getgid",
  "builtin-process-getgid-root",
  "builtin-process-getuid",
  "builtin-process-getuid-root",
  "builtin-process-home",
  "builtin-process-home-root",
  "builtin-process-run-sync",
  "builtin-process-run-sync-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinRandom, CodegenPassTest, testing::Values(
  "builtin-random-random-float",
  "builtin-random-random-float-root",
  "builtin-random-random-int",
  "builtin-random-random-int-root",
  "builtin-random-random-str",
  "builtin-random-random-str-root"
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

INSTANTIATE_TEST_SUITE_P(BuiltinThread, CodegenPassTest, testing::Values(
  "builtin-thread-sleep"
));

INSTANTIATE_TEST_SUITE_P(BuiltinStr, CodegenPassTest, testing::Values(
  "builtin-str-at",
  "builtin-str-at-ref",
  "builtin-str-at-root",
  "builtin-str-contains",
  "builtin-str-contains-ref",
  "builtin-str-contains-root",
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
  "builtin-str-replace",
  "builtin-str-replace-ref",
  "builtin-str-replace-root",
  "builtin-str-slice",
  "builtin-str-slice-ref",
  "builtin-str-slice-root",
  "builtin-str-split",
  "builtin-str-split-ref",
  "builtin-str-split-root",
  "builtin-str-to-buffer",
  "builtin-str-to-buffer-ref",
  "builtin-str-to-buffer-root",
  "builtin-str-to-float",
  "builtin-str-to-float-ref",
  "builtin-str-to-float-root",
  "builtin-str-to-f32",
  "builtin-str-to-f32-ref",
  "builtin-str-to-f32-root",
  "builtin-str-to-f64",
  "builtin-str-to-f64-ref",
  "builtin-str-to-f64-root",
  "builtin-str-to-int",
  "builtin-str-to-int-ref",
  "builtin-str-to-int-root",
  "builtin-str-to-i8",
  "builtin-str-to-i8-ref",
  "builtin-str-to-i8-root",
  "builtin-str-to-i16",
  "builtin-str-to-i16-ref",
  "builtin-str-to-i16-root",
  "builtin-str-to-i32",
  "builtin-str-to-i32-ref",
  "builtin-str-to-i32-root",
  "builtin-str-to-i64",
  "builtin-str-to-i64-ref",
  "builtin-str-to-i64-root",
  "builtin-str-to-u8",
  "builtin-str-to-u8-ref",
  "builtin-str-to-u8-root",
  "builtin-str-to-u16",
  "builtin-str-to-u16-ref",
  "builtin-str-to-u16-root",
  "builtin-str-to-u32",
  "builtin-str-to-u32-ref",
  "builtin-str-to-u32-root",
  "builtin-str-to-u64",
  "builtin-str-to-u64-ref",
  "builtin-str-to-u64-root",
  "builtin-str-trim",
  "builtin-str-trim-ref",
  "builtin-str-trim-root",
  "builtin-str-trim-end",
  "builtin-str-trim-end-ref",
  "builtin-str-trim-end-root",
  "builtin-str-trim-start",
  "builtin-str-trim-start-ref",
  "builtin-str-trim-start-root",
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

INSTANTIATE_TEST_SUITE_P(BuiltinUnion, CodegenPassTest, testing::Values(
  "builtin-union-alloc",
  "builtin-union-alloc-alias",
  "builtin-union-alloc-array",
  "builtin-union-alloc-enum",
  "builtin-union-alloc-fn",
  "builtin-union-alloc-map",
  "builtin-union-alloc-obj",
  "builtin-union-alloc-opt",
  "builtin-union-alloc-ref",
  "builtin-union-alloc-str",
  "builtin-union-alloc-union"
));

INSTANTIATE_TEST_SUITE_P(BuiltinURL, CodegenPassTest, testing::Values(
  "builtin-url-parse",
  "builtin-url-parse-root"
));

INSTANTIATE_TEST_SUITE_P(BuiltinUtils, CodegenPassTest, testing::Values(
  "builtin-utils-swap",
  "builtin-utils-swap-alias",
  "builtin-utils-swap-any",
  "builtin-utils-swap-array",
  "builtin-utils-swap-buffer",
  "builtin-utils-swap-enum",
  "builtin-utils-swap-fn",
  "builtin-utils-swap-map",
  "builtin-utils-swap-obj",
  "builtin-utils-swap-opt",
  "builtin-utils-swap-ref",
  "builtin-utils-swap-str",
  "builtin-utils-swap-union"
));

INSTANTIATE_TEST_SUITE_P(ExprAccess, CodegenPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-member",
  "expr-access-alias",
  "expr-access-any",
  "expr-access-array",
  "expr-access-enum",
  "expr-access-fn",
  "expr-access-map",
  "expr-access-obj",
  "expr-access-opt",
  "expr-access-ref",
  "expr-access-str",
  "expr-access-union"
));

INSTANTIATE_TEST_SUITE_P(ExprArray, CodegenPassTest, testing::Values(
  "expr-array",
  "expr-array-alias",
  "expr-array-any",
  "expr-array-array",
  "expr-array-enum",
  "expr-array-fn",
  "expr-array-map",
  "expr-array-obj",
  "expr-array-opt",
  "expr-array-ref",
  "expr-array-str",
  "expr-array-union"
));

INSTANTIATE_TEST_SUITE_P(ExprAssign, CodegenPassTest, testing::Values(
  "expr-assign",
  "expr-assign-op",
  "expr-assign-elem",
  "expr-assign-alias",
  "expr-assign-any",
  "expr-assign-array",
  "expr-assign-enum",
  "expr-assign-fn",
  "expr-assign-map",
  "expr-assign-obj",
  "expr-assign-opt",
  "expr-assign-ref",
  "expr-assign-str",
  "expr-assign-union"
));

INSTANTIATE_TEST_SUITE_P(ExprBinary, CodegenPassTest, testing::Values(
  "expr-binary",
  "expr-binary-alias",
  "expr-binary-array",
  "expr-binary-enum",
  "expr-binary-map",
  "expr-binary-obj",
  "expr-binary-opt",
  "expr-binary-ref",
  "expr-binary-str",
  "expr-binary-union",
  "expr-binary-nested",
  "expr-binary-type-casts"
));

INSTANTIATE_TEST_SUITE_P(ExprCall, CodegenPassTest, testing::Values(
  "expr-call",
  "expr-call-obj-prop",
  "expr-call-args"
));

INSTANTIATE_TEST_SUITE_P(ExprCond, CodegenPassTest, testing::Values(
  "expr-cond",
  "expr-cond-nested",
  "expr-cond-alias",
  "expr-cond-any",
  "expr-cond-array",
  "expr-cond-enum",
  "expr-cond-fn",
  "expr-cond-map",
  "expr-cond-obj",
  "expr-cond-opt",
  "expr-cond-ref",
  "expr-cond-str",
  "expr-cond-union",
  "expr-cond-operands",
  "expr-cond-type-casts"
));

INSTANTIATE_TEST_SUITE_P(ExprIs, CodegenPassTest, testing::Values(
  "expr-is",
  "expr-is-alias",
  "expr-is-any",
  "expr-is-array",
  "expr-is-enum",
  "expr-is-fn",
  "expr-is-map",
  "expr-is-obj",
  "expr-is-opt",
  "expr-is-ref",
  "expr-is-str",
  "expr-is-union"
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

INSTANTIATE_TEST_SUITE_P(ExprMap, CodegenPassTest, testing::Values(
  "expr-map",
  "expr-map-alias",
  "expr-map-any",
  "expr-map-array",
  "expr-map-enum",
  "expr-map-fn",
  "expr-map-map",
  "expr-map-obj",
  "expr-map-opt",
  "expr-map-ref",
  "expr-map-str",
  "expr-map-union"
));

INSTANTIATE_TEST_SUITE_P(ExprObj, CodegenPassTest, testing::Values(
  "expr-obj",
  "expr-obj-empty",
  "expr-obj-nested"
));

INSTANTIATE_TEST_SUITE_P(ExprRef, CodegenPassTest, testing::Values(
  "expr-ref",
  "expr-ref-alias",
  "expr-ref-any",
  "expr-ref-array",
  "expr-ref-enum",
  "expr-ref-fn",
  "expr-ref-map",
  "expr-ref-obj",
  "expr-ref-opt",
  "expr-ref-ref",
  "expr-ref-str",
  "expr-ref-union"
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
  "node-expr-access-alias",
  "node-expr-access-any",
  "node-expr-access-array",
  "node-expr-access-enum",
  "node-expr-access-fn",
  "node-expr-access-map",
  "node-expr-access-obj",
  "node-expr-access-opt",
  "node-expr-access-ref",
  "node-expr-access-str",
  "node-expr-access-union",
  "node-expr-array",
  "node-expr-array-alias",
  "node-expr-array-any",
  "node-expr-array-array",
  "node-expr-array-enum",
  "node-expr-array-fn",
  "node-expr-array-map",
  "node-expr-array-obj",
  "node-expr-array-opt",
  "node-expr-array-ref",
  "node-expr-array-str",
  "node-expr-array-union",
  "node-expr-assign",
  "node-expr-assign-alias",
  "node-expr-assign-any",
  "node-expr-assign-array",
  "node-expr-assign-enum",
  "node-expr-assign-fn",
  "node-expr-assign-map",
  "node-expr-assign-obj",
  "node-expr-assign-opt",
  "node-expr-assign-ref",
  "node-expr-assign-str",
  "node-expr-assign-union",
  "node-expr-binary",
  "node-expr-binary-alias",
  "node-expr-binary-array",
  "node-expr-binary-enum",
  "node-expr-binary-map",
  "node-expr-binary-obj",
  "node-expr-binary-opt",
  "node-expr-binary-ref",
  "node-expr-binary-str",
  "node-expr-binary-union",
  "node-expr-call",
  "node-expr-cond",
  "node-expr-cond-alias",
  "node-expr-cond-any",
  "node-expr-cond-array",
  "node-expr-cond-enum",
  "node-expr-cond-fn",
  "node-expr-cond-map",
  "node-expr-cond-obj",
  "node-expr-cond-opt",
  "node-expr-cond-ref",
  "node-expr-cond-str",
  "node-expr-cond-union",
  "node-expr-lit",
  "node-expr-map",
  "node-expr-map-alias",
  "node-expr-map-any",
  "node-expr-map-array",
  "node-expr-map-enum",
  "node-expr-map-fn",
  "node-expr-map-map",
  "node-expr-map-obj",
  "node-expr-map-opt",
  "node-expr-map-ref",
  "node-expr-map-str",
  "node-expr-map-union",
  "node-expr-obj",
  "node-expr-obj-alias",
  "node-expr-obj-any",
  "node-expr-obj-array",
  "node-expr-obj-enum",
  "node-expr-obj-fn",
  "node-expr-obj-map",
  "node-expr-obj-obj",
  "node-expr-obj-opt",
  "node-expr-obj-ref",
  "node-expr-obj-str",
  "node-expr-obj-union",
  "node-expr-ref",
  "node-expr-ref-alias",
  "node-expr-ref-any",
  "node-expr-ref-array",
  "node-expr-ref-enum",
  "node-expr-ref-fn",
  "node-expr-ref-map",
  "node-expr-ref-obj",
  "node-expr-ref-opt",
  "node-expr-ref-ref",
  "node-expr-ref-str",
  "node-expr-ref-union",
  "node-expr-unary",
  "node-expr-unary-context",
  "node-expr-unary-obj",
  "node-expr-unary-str"
));

INSTANTIATE_TEST_SUITE_P(NodeFnDecl, CodegenPassTest, testing::Values(
  "node-fn-decl-empty",
  "node-fn-decl-body-empty",
  "node-fn-decl-body-before",
  "node-fn-decl-body-after",
  "node-fn-decl-hoisting",
  "node-fn-decl-param-default",
  "node-fn-decl-param-default-alias",
  "node-fn-decl-param-default-any",
  "node-fn-decl-param-default-array",
  "node-fn-decl-param-default-enum",
  "node-fn-decl-param-default-fn",
  "node-fn-decl-param-default-map",
  "node-fn-decl-param-default-obj",
  "node-fn-decl-param-default-opt",
  "node-fn-decl-param-default-ref",
  "node-fn-decl-param-default-str",
  "node-fn-decl-param-default-union",
  "node-fn-decl-param-fn",
  "node-fn-decl-nested",
  "node-fn-decl-complex",
  "node-fn-decl-param-mut",
  "node-fn-decl-param-mut-alias",
  "node-fn-decl-param-mut-any",
  "node-fn-decl-param-mut-array",
  "node-fn-decl-param-mut-enum",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-param-mut-map",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-mut-opt",
  "node-fn-decl-param-mut-ref",
  "node-fn-decl-param-mut-str",
  "node-fn-decl-param-mut-union",
  "node-fn-decl-param-variadic",
  "node-fn-decl-scope",
  "node-fn-decl-stack",
  "node-fn-decl-stack-alias",
  "node-fn-decl-stack-any",
  "node-fn-decl-stack-array",
  "node-fn-decl-stack-enum",
  "node-fn-decl-stack-fn",
  "node-fn-decl-stack-map",
  "node-fn-decl-stack-obj",
  "node-fn-decl-stack-opt",
  "node-fn-decl-stack-ref",
  "node-fn-decl-stack-str",
  "node-fn-decl-stack-union"
));

INSTANTIATE_TEST_SUITE_P(NodeIf, CodegenPassTest, testing::Values(
  "node-if",
  "node-if-cmp-num",
  "node-if-cmp-alias",
  "node-if-cmp-array",
  "node-if-cmp-enum",
  "node-if-cmp-map",
  "node-if-cmp-obj",
  "node-if-cmp-opt",
  "node-if-cmp-ref",
  "node-if-cmp-str",
  "node-if-cmp-union",
  "node-if-type-casts",
  "node-if-type-casts-elif"
));

INSTANTIATE_TEST_SUITE_P(NodeLoop, CodegenPassTest, testing::Values(
  "node-loop",
  "node-loop-empty",
  "node-loop-while",
  "node-loop-parenthesized",
  "node-loop-alias",
  "node-loop-array",
  "node-loop-enum",
  "node-loop-map",
  "node-loop-obj",
  "node-loop-opt",
  "node-loop-ref",
  "node-loop-str",
  "node-loop-union",
  "node-loop-complex"
));

INSTANTIATE_TEST_SUITE_P(NodeObjDecl, CodegenPassTest, testing::Values(
  "node-obj-decl",
  "node-obj-decl-empty",
  "node-obj-decl-default",
  "node-obj-decl-default-alias",
  "node-obj-decl-default-any",
  "node-obj-decl-default-array",
  "node-obj-decl-default-enum",
  "node-obj-decl-default-map",
  "node-obj-decl-default-obj",
  "node-obj-decl-default-obj-member",
  "node-obj-decl-default-opt",
  "node-obj-decl-default-str",
  "node-obj-decl-field-alias",
  "node-obj-decl-field-any",
  "node-obj-decl-field-array",
  "node-obj-decl-field-enum",
  "node-obj-decl-field-fn",
  "node-obj-decl-field-map",
  "node-obj-decl-field-obj",
  "node-obj-decl-field-opt",
  "node-obj-decl-field-ref",
  "node-obj-decl-field-self",
  "node-obj-decl-field-str",
  "node-obj-decl-field-union",
  "node-obj-decl-field-variadic",
  "node-obj-decl-forward-decl",
  "node-obj-decl-mut",
  "node-obj-decl-method",
  "node-obj-decl-method-body-empty",
  "node-obj-decl-method-body-before",
  "node-obj-decl-method-body-after",
  "node-obj-decl-method-forward-decl",
  "node-obj-decl-method-forward-decl-with-fn",
  "node-obj-decl-method-variadic"
));

INSTANTIATE_TEST_SUITE_P(NodeReturn, CodegenPassTest, testing::Values(
  "node-return",
  "node-return-scope-cleanup"
));

INSTANTIATE_TEST_SUITE_P(NodeThrow, CodegenPassTest, testing::Values(
  "node-throw",
  "node-throw-custom",
  "node-throw-custom-extended",
  "node-throw-inside-fn-decl",
  "node-throw-inside-if",
  "node-throw-inside-loop",
  "node-throw-inside-obj-decl",
  "node-throw-inside-obj-decl-method",
  "node-throw-inside-try",
  "node-throw-raw"
));

INSTANTIATE_TEST_SUITE_P(NodeTypeDecl, CodegenPassTest, testing::Values(
  "node-type-decl",
  "node-type-decl-alias",
  "node-type-decl-any",
  "node-type-decl-array",
  "node-type-decl-enum",
  "node-type-decl-fn",
  "node-type-decl-map",
  "node-type-decl-obj",
  "node-type-decl-opt",
  "node-type-decl-ref",
  "node-type-decl-str",
  "node-type-decl-union"
));

INSTANTIATE_TEST_SUITE_P(NodeVarDecl, CodegenPassTest, testing::Values(
  "node-var-decl-alias",
  "node-var-decl-alias-init",
  "node-var-decl-alias-const",
  "node-var-decl-alias-mut",
  "node-var-decl-alias-mut-init",
  "node-var-decl-any",
  "node-var-decl-any-init",
  "node-var-decl-any-const",
  "node-var-decl-any-mut",
  "node-var-decl-any-mut-init",
  "node-var-decl-array",
  "node-var-decl-array-init",
  "node-var-decl-array-short",
  "node-var-decl-array-const",
  "node-var-decl-array-mut",
  "node-var-decl-array-mut-init",
  "node-var-decl-array-mut-short",
  "node-var-decl-bool",
  "node-var-decl-bool-init",
  "node-var-decl-bool-short",
  "node-var-decl-bool-const",
  "node-var-decl-bool-mut",
  "node-var-decl-bool-mut-init",
  "node-var-decl-bool-mut-short",
  "node-var-decl-byte",
  "node-var-decl-byte-init",
  "node-var-decl-byte-const",
  "node-var-decl-byte-mut",
  "node-var-decl-byte-mut-init",
  "node-var-decl-char",
  "node-var-decl-char-init",
  "node-var-decl-char-short",
  "node-var-decl-char-const",
  "node-var-decl-char-mut",
  "node-var-decl-char-mut-init",
  "node-var-decl-char-mut-short",
  "node-var-decl-enum",
  "node-var-decl-enum-init",
  "node-var-decl-enum-short",
  "node-var-decl-enum-const",
  "node-var-decl-enum-mut",
  "node-var-decl-enum-mut-init",
  "node-var-decl-enum-mut-short",
  "node-var-decl-f32",
  "node-var-decl-f32-init",
  "node-var-decl-f32-const",
  "node-var-decl-f32-mut",
  "node-var-decl-f32-mut-init",
  "node-var-decl-f64",
  "node-var-decl-f64-init",
  "node-var-decl-f64-const",
  "node-var-decl-f64-mut",
  "node-var-decl-f64-mut-init",
  "node-var-decl-float",
  "node-var-decl-float-init",
  "node-var-decl-float-short",
  "node-var-decl-float-const",
  "node-var-decl-float-mut",
  "node-var-decl-float-mut-init",
  "node-var-decl-float-mut-short",
  "node-var-decl-fn-init",
  "node-var-decl-fn-short",
  "node-var-decl-fn-const",
  "node-var-decl-fn-mut-init",
  "node-var-decl-fn-mut-short",
  "node-var-decl-i8",
  "node-var-decl-i8-init",
  "node-var-decl-i8-const",
  "node-var-decl-i8-mut",
  "node-var-decl-i8-mut-init",
  "node-var-decl-i16",
  "node-var-decl-i16-init",
  "node-var-decl-i16-const",
  "node-var-decl-i16-mut",
  "node-var-decl-i16-mut-init",
  "node-var-decl-i32",
  "node-var-decl-i32-init",
  "node-var-decl-i32-const",
  "node-var-decl-i32-mut",
  "node-var-decl-i32-mut-init",
  "node-var-decl-i64",
  "node-var-decl-i64-init",
  "node-var-decl-i64-const",
  "node-var-decl-i64-mut",
  "node-var-decl-i64-mut-init",
  "node-var-decl-int",
  "node-var-decl-int-init",
  "node-var-decl-int-short",
  "node-var-decl-int-const",
  "node-var-decl-int-mut",
  "node-var-decl-int-mut-init",
  "node-var-decl-int-mut-short",
  "node-var-decl-map",
  "node-var-decl-map-init",
  "node-var-decl-map-short",
  "node-var-decl-map-const",
  "node-var-decl-map-mut",
  "node-var-decl-map-mut-init",
  "node-var-decl-map-mut-short",
  "node-var-decl-obj",
  "node-var-decl-obj-init",
  "node-var-decl-obj-short",
  "node-var-decl-obj-const",
  "node-var-decl-obj-method",
  "node-var-decl-obj-method-init",
  "node-var-decl-obj-method-short",
  "node-var-decl-obj-method-const",
  "node-var-decl-obj-mut",
  "node-var-decl-obj-mut-init",
  "node-var-decl-obj-mut-short",
  "node-var-decl-opt",
  "node-var-decl-opt-init",
  "node-var-decl-opt-const",
  "node-var-decl-opt-mut",
  "node-var-decl-opt-mut-init",
  "node-var-decl-ref-init",
  "node-var-decl-ref-short",
  "node-var-decl-ref-const",
  "node-var-decl-ref-mut-init",
  "node-var-decl-ref-mut-short",
  "node-var-decl-str",
  "node-var-decl-str-init",
  "node-var-decl-str-short",
  "node-var-decl-str-const",
  "node-var-decl-str-mut",
  "node-var-decl-str-mut-init",
  "node-var-decl-str-mut-short",
  "node-var-decl-u8",
  "node-var-decl-u8-init",
  "node-var-decl-u8-const",
  "node-var-decl-u8-mut",
  "node-var-decl-u8-mut-init",
  "node-var-decl-u16",
  "node-var-decl-u16-init",
  "node-var-decl-u16-const",
  "node-var-decl-u16-mut",
  "node-var-decl-u16-mut-init",
  "node-var-decl-u32",
  "node-var-decl-u32-init",
  "node-var-decl-u32-const",
  "node-var-decl-u32-mut",
  "node-var-decl-u32-mut-init",
  "node-var-decl-u64",
  "node-var-decl-u64-init",
  "node-var-decl-u64-const",
  "node-var-decl-u64-mut",
  "node-var-decl-u64-mut-init",
  "node-var-decl-union-init",
  "node-var-decl-union-const",
  "node-var-decl-union-mut-init"
));

INSTANTIATE_TEST_SUITE_P(BuiltinArray, CodegenThrowTest, testing::Values(
  "throw-builtin-array-at-empty",
  "throw-builtin-array-at-high",
  "throw-builtin-array-at-low",
  "throw-builtin-array-first-on-empty",
  "throw-builtin-array-last-on-empty",
  "throw-builtin-array-remove-high",
  "throw-builtin-array-remove-low"
));

INSTANTIATE_TEST_SUITE_P(BuiltinFs, CodegenThrowTest, testing::Values(
  "throw-builtin-fs-append-file-sync-directory",
  "throw-builtin-fs-append-file-sync-protected",
  "throw-builtin-fs-chmod-sync-non-existing",
  "throw-builtin-fs-chown-sync-non-existing",
  "throw-builtin-fs-copy-directory-sync-file",
  "throw-builtin-fs-copy-directory-sync-non-existing",
  "throw-builtin-fs-copy-file-sync-same",
  "throw-builtin-fs-copy-file-sync-directory",
  "throw-builtin-fs-copy-file-sync-destination-directory",
  "throw-builtin-fs-copy-file-sync-non-existing",
  "throw-builtin-fs-link-sync-existing",
  "throw-builtin-fs-link-sync-same-name",
  "throw-builtin-fs-mkdir-sync-existing",
  "throw-builtin-fs-read-file-sync-non-existing",
  "throw-builtin-fs-realpath-sync-non-existing",
  "throw-builtin-fs-rename-sync-non-existing",
  "throw-builtin-fs-rename-sync-protected",
  "throw-builtin-fs-rm-sync-directory",
  "throw-builtin-fs-rm-sync-non-existing",
  "throw-builtin-fs-rmdir-sync-file",
  "throw-builtin-fs-rmdir-sync-non-existing",
  "throw-builtin-fs-scandir-sync-non-directory",
  "throw-builtin-fs-scandir-sync-non-existing",
  "throw-builtin-fs-stat-sync-non-existing",
  "throw-builtin-fs-unlink-sync-non-existing",
  "throw-builtin-fs-write-file-sync-directory"
));

INSTANTIATE_TEST_SUITE_P(BuiltinMap, CodegenThrowTest, testing::Values(
  "throw-builtin-map-get-non-existing",
  "throw-builtin-map-remove-non-existing"
));

INSTANTIATE_TEST_SUITE_P(BuiltinProcess, CodegenThrowTest, testing::Values(
  "throw-builtin-process-run-sync-exit-code"
));

INSTANTIATE_TEST_SUITE_P(BuiltinRequest, CodegenThrowTest, testing::Values(
  "throw-builtin-request-open-invalid-host",
  "throw-builtin-request-open-invalid-port",
  "throw-builtin-request-open-invalid-protocol",
  "throw-builtin-request-open-long-port",
  "throw-builtin-request-open-invalid-cert-cipher"
));

INSTANTIATE_TEST_SUITE_P(BuiltinStr, CodegenThrowTest, testing::Values(
  "throw-builtin-str-at-empty",
  "throw-builtin-str-at-high",
  "throw-builtin-str-at-low",
  "throw-builtin-str-to-float-empty",
  "throw-builtin-str-to-float-string",
  "throw-builtin-str-to-float-invalid",
  "throw-builtin-str-to-float-invalid-exp",
  "throw-builtin-str-to-float-invalid-mantissa",
  "throw-builtin-str-to-float-high",
  "throw-builtin-str-to-float-low",
  "throw-builtin-str-to-f32-empty",
  "throw-builtin-str-to-f32-string",
  "throw-builtin-str-to-f32-invalid",
  "throw-builtin-str-to-f32-invalid-exp",
  "throw-builtin-str-to-f32-invalid-mantissa",
  "throw-builtin-str-to-f32-high",
  "throw-builtin-str-to-f32-low",
  "throw-builtin-str-to-f64-empty",
  "throw-builtin-str-to-f64-string",
  "throw-builtin-str-to-f64-invalid",
  "throw-builtin-str-to-f64-invalid-exp",
  "throw-builtin-str-to-f64-invalid-mantissa",
  "throw-builtin-str-to-f64-high",
  "throw-builtin-str-to-f64-low",
  "throw-builtin-str-to-int-empty",
  "throw-builtin-str-to-int-string",
  "throw-builtin-str-to-int-invalid-2",
  "throw-builtin-str-to-int-invalid-8",
  "throw-builtin-str-to-int-invalid-10",
  "throw-builtin-str-to-int-invalid-16",
  "throw-builtin-str-to-int-high",
  "throw-builtin-str-to-int-low",
  "throw-builtin-str-to-int-wrong-base",
  "throw-builtin-str-to-i8-empty",
  "throw-builtin-str-to-i8-string",
  "throw-builtin-str-to-i8-invalid-2",
  "throw-builtin-str-to-i8-invalid-8",
  "throw-builtin-str-to-i8-invalid-10",
  "throw-builtin-str-to-i8-invalid-16",
  "throw-builtin-str-to-i8-high",
  "throw-builtin-str-to-i8-low",
  "throw-builtin-str-to-i8-wrong-base",
  "throw-builtin-str-to-i16-empty",
  "throw-builtin-str-to-i16-string",
  "throw-builtin-str-to-i16-invalid-2",
  "throw-builtin-str-to-i16-invalid-8",
  "throw-builtin-str-to-i16-invalid-10",
  "throw-builtin-str-to-i16-invalid-16",
  "throw-builtin-str-to-i16-high",
  "throw-builtin-str-to-i16-low",
  "throw-builtin-str-to-i16-wrong-base",
  "throw-builtin-str-to-i32-empty",
  "throw-builtin-str-to-i32-string",
  "throw-builtin-str-to-i32-invalid-2",
  "throw-builtin-str-to-i32-invalid-8",
  "throw-builtin-str-to-i32-invalid-10",
  "throw-builtin-str-to-i32-invalid-16",
  "throw-builtin-str-to-i32-high",
  "throw-builtin-str-to-i32-low",
  "throw-builtin-str-to-i32-wrong-base",
  "throw-builtin-str-to-i64-empty",
  "throw-builtin-str-to-i64-string",
  "throw-builtin-str-to-i64-invalid-2",
  "throw-builtin-str-to-i64-invalid-8",
  "throw-builtin-str-to-i64-invalid-10",
  "throw-builtin-str-to-i64-invalid-16",
  "throw-builtin-str-to-i64-high",
  "throw-builtin-str-to-i64-low",
  "throw-builtin-str-to-i64-wrong-base",
  "throw-builtin-str-to-u8-empty",
  "throw-builtin-str-to-u8-negative",
  "throw-builtin-str-to-u8-string",
  "throw-builtin-str-to-u8-invalid-2",
  "throw-builtin-str-to-u8-invalid-8",
  "throw-builtin-str-to-u8-invalid-10",
  "throw-builtin-str-to-u8-invalid-16",
  "throw-builtin-str-to-u8-high",
  "throw-builtin-str-to-u8-wrong-base",
  "throw-builtin-str-to-u16-empty",
  "throw-builtin-str-to-u16-negative",
  "throw-builtin-str-to-u16-string",
  "throw-builtin-str-to-u16-invalid-2",
  "throw-builtin-str-to-u16-invalid-8",
  "throw-builtin-str-to-u16-invalid-10",
  "throw-builtin-str-to-u16-invalid-16",
  "throw-builtin-str-to-u16-high",
  "throw-builtin-str-to-u16-wrong-base",
  "throw-builtin-str-to-u32-empty",
  "throw-builtin-str-to-u32-negative",
  "throw-builtin-str-to-u32-string",
  "throw-builtin-str-to-u32-invalid-2",
  "throw-builtin-str-to-u32-invalid-8",
  "throw-builtin-str-to-u32-invalid-10",
  "throw-builtin-str-to-u32-invalid-16",
  "throw-builtin-str-to-u32-high",
  "throw-builtin-str-to-u32-wrong-base",
  "throw-builtin-str-to-u64-empty",
  "throw-builtin-str-to-u64-negative",
  "throw-builtin-str-to-u64-string",
  "throw-builtin-str-to-u64-invalid-2",
  "throw-builtin-str-to-u64-invalid-8",
  "throw-builtin-str-to-u64-invalid-10",
  "throw-builtin-str-to-u64-invalid-16",
  "throw-builtin-str-to-u64-high",
  "throw-builtin-str-to-u64-wrong-base"
));

INSTANTIATE_TEST_SUITE_P(BuiltinURL, CodegenThrowTest, testing::Values(
  "throw-builtin-url-parse-auth",
  "throw-builtin-url-parse-empty",
  "throw-builtin-url-parse-empty-hostname",
  "throw-builtin-url-parse-invalid-origin",
  "throw-builtin-url-parse-invalid-port",
  "throw-builtin-url-parse-invalid-protocol",
  "throw-builtin-url-parse-string"
));

INSTANTIATE_TEST_SUITE_P(NodeThrow, CodegenThrowTest, testing::Values(
  "throw-node-throw",
  "throw-node-throw-custom",
  "throw-node-throw-custom-extended",
  "throw-node-throw-inside-fn-decl",
  "throw-node-throw-inside-if",
  "throw-node-throw-inside-loop",
  "throw-node-throw-inside-obj-decl",
  "throw-node-throw-inside-obj-decl-method",
  "throw-node-throw-inside-try",
  "throw-node-throw-raw"
));
