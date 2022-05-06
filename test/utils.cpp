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
#include <sstream>

extern char **environ;

std::tuple<std::string, std::string, int> execCmd (const std::string &cmd, const std::string &tmpName) {
  auto returnCode = -1;

  auto pcloseWrapper = [&returnCode] (FILE *fd) {
    returnCode = pclose(fd);
    returnCode = WEXITSTATUS(returnCode);
  };

  auto stderrFileName = tmpName + "-stderr.txt";
  auto actualCmd = cmd + " 2>" + stderrFileName;
  auto buffer = std::array<char, 128>{};
  auto stdoutOutput = std::string();

  {
    auto stderrFile = std::ofstream(stderrFileName);
    stderrFile.close();

    auto pipe = std::unique_ptr<FILE, decltype(pcloseWrapper)>{popen(actualCmd.c_str(), "r"), pcloseWrapper};

    if (!pipe) {
      throw Error("Failed to execute binary file");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      stdoutOutput += buffer.data();
    }
  }

  auto stderrFile = std::ifstream(stderrFileName);
  auto stderrOutput = std::stringstream();

  stderrOutput << stderrFile.rdbuf();
  std::filesystem::remove(stderrFileName);

  return std::make_tuple(stdoutOutput, stderrOutput.str(), returnCode);
}

std::map<std::string, std::string> getEnvVars () {
  auto result = std::map<std::string, std::string>{};
  char **s = environ;

  for (; *s; s++) {
    auto var = std::string(*s);
    result[var.substr(0, var.find('='))] = var.substr(var.find('=') + 1);
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
