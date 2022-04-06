/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <filesystem>
#include <fstream>
#include "utils.hpp"

std::string readTestFile (const std::string &testName, const std::string &filepath) {
  auto path = "test/" + testName + "-test/" + filepath + ".txt";
  auto file = std::ifstream(path, std::ios::in | std::ios::binary);

  if (!file.is_open()) {
    throw Error("Unable to open " + testName + R"( test file ")" + path + R"(")");
  }

  auto size = static_cast<std::ptrdiff_t>(std::filesystem::file_size(path));
  auto content = std::string(size, '\0');

  file.read(content.data(), size);
  file.close();

  return content;
}
