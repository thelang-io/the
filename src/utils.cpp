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
#include <filesystem>
#include <fstream>
#include <sstream>
#include "config.hpp"

// todo
std::optional<std::string> convert_path_to_namespace (const std::string &p) {
  auto cwd = std::filesystem::current_path().string();

  if (!p.starts_with(cwd)) {
    return std::nullopt;
  }

  auto s = p.substr(cwd.size());

  for (auto i = static_cast<std::size_t>(0); i < s.size(); i++) {
    if (!std::isalnum(s[i])) {
      s[i] = '_';
    }
  }

  auto prevIsUnderscore = false;

  auto iterator = std::remove_if(s.begin(), s.end(), [&] (auto it) -> bool {
    if (it == '_' && prevIsUnderscore) {
      return true;
    }

    prevIsUnderscore = it == '_';
    return false;
  });

  s.erase(iterator, s.end());

  if (s.starts_with('_') && s.ends_with('_')) {
    s = s.substr(1, s.size() - 2);
  } else if (s.starts_with('_')) {
    s = s.substr(1);
  } else if (s.starts_with('_')) {
    s = s.substr(0, s.size() - 1);
  }

  return s;
}

std::string str_trim (const std::string &str) {
  auto result = str;

  result.erase(result.begin(), std::find_if(result.begin(), result.end(), [] (auto ch) -> bool {
    return !std::isspace(ch);
  }));

  result.erase(std::find_if(result.rbegin(), result.rend(), [] (auto ch) -> bool {
    return !std::isspace(ch);
  }).base(), result.end());

  return result;
}

std::vector<std::string> str_lines (const std::string &str) {
  auto result = std::vector<std::string>{""};

  for (auto i = static_cast<std::size_t>(0); i < str.size(); i++) {
    auto ch = str[i];

    if (ch == '\r') {
      if (i + 1 < str.size() && str[i + 1] == '\n') {
        i++;
      }

      result.push_back("");
    } else if (ch == '\n') {
      result.push_back("");
    } else {
      result.back() += ch;
    }
  }

  result.erase(std::remove_if(result.begin(), result.end(), [] (auto it) {
    return str_trim(it).empty();
  }), result.end());

  return result;
}

std::optional<std::string> parse_package_yaml_main (const std::string &packageName) {
  try {
    auto d = std::filesystem::current_path() / ".packages" / packageName;
    auto f = std::ifstream(std::filesystem::canonical(d / "package.yml").string());

    if (f.is_open() && !f.fail()) {
      auto c = std::stringstream();
      c << f.rdbuf();
      auto contentLines = str_lines(c.str());

      for (const auto &line : contentLines) {
        if (line.starts_with("main:")) {
          return std::filesystem::canonical(d / str_trim(line.substr(5))).string();
        }
      }
    }

    return std::nullopt;
  } catch (const std::exception &) {
    return std::nullopt;
  }
}
