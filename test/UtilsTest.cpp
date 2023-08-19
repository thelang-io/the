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
#include "../src/config.hpp"
#include "../src/utils.hpp"

TEST(UtilsTest, StrTrim) {
  EXPECT_EQ(str_trim(""), "");
  EXPECT_EQ(str_trim("test"), "test");
  EXPECT_EQ(str_trim(" test"), "test");
  EXPECT_EQ(str_trim(" test "), "test");
  EXPECT_EQ(str_trim("\ntest\n"), "test");
  EXPECT_EQ(str_trim("\rtest\r"), "test");
  EXPECT_EQ(str_trim("\n\rtest\r\n"), "test");
  EXPECT_EQ(str_trim("\t\n\rtest\r\n\t"), "test");
}

TEST(UtilsTest, StrLines) {
  auto output1 = std::vector<std::string>{"test1", "test2"};

  EXPECT_EQ(str_lines(""), std::vector<std::string>{});
  EXPECT_EQ(str_lines("test"), std::vector<std::string>{"test"});
  EXPECT_EQ(str_lines(EOL "test"), std::vector<std::string>{"test"});
  EXPECT_EQ(str_lines("test" EOL), std::vector<std::string>{"test"});
  EXPECT_EQ(str_lines(EOL "test" EOL), std::vector<std::string>{"test"});
  EXPECT_EQ(str_lines(EOL "test1" EOL "test2"), output1);
  EXPECT_EQ(str_lines(EOL "test1" EOL "test2" EOL), output1);
}

TEST(UtilsTest, ParsePackageYamlMain) {
  auto cwd = std::filesystem::current_path();
  std::filesystem::current_path(cwd / "test");

  EXPECT_EQ(
    *parse_package_yaml_main("test/package"),
    (std::filesystem::current_path() / ".packages" / "test" / "package" / "package").string()
  );

  std::filesystem::current_path(cwd);
}

TEST(UtilsTest, ParsePackageYamlMainNonExisting) {
  auto cwd = std::filesystem::current_path();
  std::filesystem::current_path(cwd / "test");

  EXPECT_EQ(parse_package_yaml_main("test/no-package"), std::nullopt);

  std::filesystem::current_path(cwd);
}

TEST(UtilsTest, ParsePackageYamlMainWithoutMain) {
  auto cwd = std::filesystem::current_path();
  std::filesystem::current_path(cwd / "test");

  EXPECT_EQ(parse_package_yaml_main("test/without-name"), std::nullopt);

  std::filesystem::current_path(cwd);
}

TEST(UtilsTest, ParsePackageYamlMainPackageFileIsDirectory) {
  auto cwd = std::filesystem::current_path();
  std::filesystem::current_path(cwd / "test");

  EXPECT_EQ(parse_package_yaml_main("test/package-file-is-directory"), std::nullopt);

  std::filesystem::current_path(cwd);
}
