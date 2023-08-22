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

#include "utils.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "../src/config.hpp"

std::tuple<std::string, std::string, int> execCmd (const std::string &cmd, const std::string &tmpName) {
  auto returnCode = -1;

  auto pcloseWrapper = [&] (FILE *fd) {
    #if defined(OS_WINDOWS)
      returnCode = _pclose(fd);
    #else
      returnCode = pclose(fd) / 256;
    #endif
  };

  auto stderrFileName = tmpName + "-stderr.txt";
  auto actualCmd = cmd + " 2>" + stderrFileName;
  auto buffer = std::array<char, 128>{};
  auto stdoutOutput = std::string();

  {
    auto stderrFile = std::ofstream(stderrFileName);
    stderrFile.close();

    #if defined(OS_WINDOWS)
      auto pipe = std::unique_ptr<FILE, decltype(pcloseWrapper)>{_popen(actualCmd.c_str(), "r"), pcloseWrapper};
    #else
      auto pipe = std::unique_ptr<FILE, decltype(pcloseWrapper)>{popen(actualCmd.c_str(), "r"), pcloseWrapper};
    #endif

    if (!pipe) {
      throw Error("Error: failed to execute binary file");
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
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

std::optional<std::string> getEnvVar (const std::string &name) {
  #if defined(OS_WINDOWS)
    auto buf = static_cast<char *>(nullptr);
    auto size = static_cast<std::size_t>(0);

    if (_dupenv_s(&buf, &size, name.c_str()) != 0 || buf == nullptr) {
      return std::nullopt;
    }

    auto result = std::string(buf);
    free(buf);
    return result;
  #else
    const char *result = std::getenv(name.c_str());
    return result == nullptr ? std::optional<std::string>{} : std::string(result);
  #endif
}

std::string prepareTestOutput (const std::string &output) {
  auto result = output;
  auto from = std::string("/test");
  auto to = (std::filesystem::current_path() / "test").string();
  auto startPos = static_cast<std::size_t>(0);

  while ((startPos = result.find(from, startPos)) != std::string::npos) {
    if (
      (startPos <= 0 || (result[startPos - 1] != '.' && result[startPos - 1] != '/')) &&
      std::isalnum(result[startPos + from.length()]) == 0 &&
      result[startPos + from.length()] != '.' &&
      result[startPos + from.length()] != '/'
    ) {
      result.replace(startPos, from.length(), to);
      startPos += to.length();
    } else {
      startPos += from.length();
    }
  }

  from = std::string("/fixtures/");
  to = (std::filesystem::current_path() / "fixtures").string() + OS_PATH_SEP;
  startPos = 0;

  while ((startPos = result.find(from, startPos)) != std::string::npos) {
    if (
      startPos <= 0 ||
      (result[startPos - 1] != '.' && result[startPos - 1] != '/' && !std::isalnum(result[startPos - 1]))
    ) {
      result.replace(startPos, from.length(), to);
      startPos += to.length();
    } else {
      startPos += from.length();
    }
  }

  return result;
}

std::string prepareTestOutputFrom (const std::string &output) {
  auto result = output;
  auto from = (std::filesystem::current_path() / "test").string();
  auto to = std::string("/test");
  auto startPos = static_cast<std::size_t>(0);

  while ((startPos = result.find(from, startPos)) != std::string::npos) {
    result.replace(startPos, from.length(), to);
    startPos += to.length();
  }

  if (from.find('\\') != std::string::npos) {
    auto fromPos = static_cast<std::size_t>(0);

    while ((fromPos = from.find('\\', fromPos)) != std::string::npos) {
      from.replace(fromPos, 1, "\\\\");
      fromPos += 2;
    }

    startPos = 0;

    while ((startPos = result.find(from, startPos)) != std::string::npos) {
      result.replace(startPos, from.length(), to);
      startPos += to.length();
    }
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
    throw Error("Error: unable to open " + testName + R"( test file ")" + path + R"(")");
  }

  auto size = std::filesystem::file_size(path);
  auto content = std::string(size, '\0');

  file.read(content.data(), static_cast<std::ptrdiff_t>(size));
  file.close();

  if (!content.starts_with("======= ")) {
    throw Error(R"(Error: test file ")" + path + R"(" doesn't look like an actual )" + testName + "test");
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

  for (auto &it : sections) {
    if (std::find(allowedSections.begin(), allowedSections.end(), it.first) == allowedSections.end()) {
      throw Error("Error: passed unknown " + testName + R"( section ")" + it.first + R"(")");
    }

    if (eraseEolSections.contains(it.first) && !it.second.empty()) {
      it.second.erase(it.second.size() - eolSize, eolSize);
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
