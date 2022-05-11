/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "utils.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "../src/config.hpp"

extern char **environ;

std::tuple<std::string, std::string, int> execCmd (const std::string &cmd, const std::string &tmpName) {
  auto returnCode = -1;

  auto pcloseWrapper = [&returnCode] (FILE *fd) {
    returnCode = pclose(fd);
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
  stderrFile.close();

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

std::map<std::string, std::string> readTestFile (
  const std::string &testName,
  const std::string &filepath,
  const std::vector<std::string> &allowedSections
) {
  auto path = "test" OS_PATH_SEP + testName + "-test" OS_PATH_SEP + filepath + ".txt";
  auto file = std::ifstream(path, std::ios::in | std::ios::binary);

  if (!file.is_open()) {
    throw Error("Unable to open " + testName + R"( test file ")" + path + R"(")");
  }

  auto size = static_cast<std::ptrdiff_t>(std::filesystem::file_size(path));
  auto content = std::string(size, '\0');

  file.read(content.data(), size);
  file.close();

  if (!content.starts_with("======= ")) {
    throw Error(R"(Test file ")" + path + R"(" doesn't look like an actual )" + testName + "test");
  }

  auto sections = std::map<std::string, std::string>{};
  auto eolSize = std::string(EOL).size();
  auto sectionName = std::string();
  auto sectionContent = std::string();

  for (const auto &line : splitFileContent(content)) {
    if (line.starts_with("======= ") && line.ends_with(" =======" EOL)) {
      if (!sectionName.empty()) {
        sections[sectionName] = sectionContent;
      }

      sectionName = line.substr(8, line.size() - 16 - eolSize);
      sectionContent = "";
    } else {
      sectionContent += line;
    }
  }

  sections[sectionName] = sectionContent;

  auto eraseEolSections = std::map<std::string, bool>{
    {"stdin", true},
    {"flags", true},
    {"stderr", true}
  };

  for (auto &item : sections) {
    if (std::find(allowedSections.begin(), allowedSections.end(), item.first) == allowedSections.end()) {
      throw Error("Error: passed unknown " + testName + R"( section ")" + item.first + R"(")");
    }

    if (eraseEolSections.contains(item.first) && !item.second.empty()) {
      item.second.erase(item.second.size() - eolSize, eolSize);
    }
  }

  return sections;
}

std::vector<std::string> splitFileContent (const std::string &content) {
  auto result = std::vector<std::string>{};
  auto eolSize = std::string(EOL).size();
  auto line = std::string();
  auto len = content.size();

  for (auto i = static_cast<std::size_t>(0); i < len; i++) {
    auto ch = content[i];
    line += ch;

    if (content.substr(i + 1, eolSize) == EOL) {
      result.push_back(line + EOL);
      line = "";
      i += eolSize;
    }
  }

  if (!line.empty()) {
    result.push_back(line + (line.ends_with(EOL) ? "" : EOL));
  }

  return result;
}
