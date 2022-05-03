/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "utils.hpp"
#include <array>
#include <filesystem>
#include <fstream>

std::string execCmd (const std::string &cmd) {
  auto pipe = std::unique_ptr<FILE, decltype(&pclose)>{popen(cmd.c_str(), "r"), pclose};

  if (!pipe) {
    throw Error("Failed to execute binary file");
  }

  auto buffer = std::array<char, 128>{};
  auto result = std::string();

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  return result;
}

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
