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

#ifndef SRC_UTILS_HPP
#define SRC_UTILS_HPP

#include <algorithm>
#include <string>
#include <vector>
#include "config.hpp"

inline std::string str_trim (const std::string &str) {
  auto result = str;

  result.erase(result.begin(), std::find_if(result.begin(), result.end(), [] (auto ch) -> bool {
    return !std::isspace(ch);
  }));

  result.erase(std::find_if(result.rbegin(), result.rend(), [] (auto ch) -> bool {
    return !std::isspace(ch);
  }).base(), result.end());

  return result;
}

inline std::vector<std::string> str_lines (const std::string &str) {
  auto delimiter = std::string(EOL);
  auto pos = static_cast<std::string::size_type>(0);
  auto prev = static_cast<std::string::size_type>(0);
  auto result = std::vector<std::string>{};

  while ((pos = str.find(delimiter, prev)) != std::string::npos) {
    result.push_back(str.substr(prev, pos - prev));
    prev = pos + delimiter.size();
  }

  result.push_back(str.substr(prev));

  result.erase(std::remove_if(result.begin(), result.end(), [] (auto it) {
    return str_trim(it).empty();
  }), result.end());

  return result;
}

[[noreturn]] inline void unreachable () {
  #ifdef __GNUC__
    __builtin_unreachable();
  #elif defined(_MSC_VER)
    __assume(false);
  #endif
}

#endif
