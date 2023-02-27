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

#include "ParserComment.hpp"
#include <algorithm>
#include "config.hpp"

std::string trim (const std::string &str) {
  auto result = str;

  result.erase(result.begin(), std::find_if(result.begin(), result.end(), [] (auto ch) -> bool {
    return !std::isspace(ch);
  }));

  result.erase(std::find_if(result.rbegin(), result.rend(), [] (auto ch) -> bool {
    return !std::isspace(ch);
  }).base(), result.end());

  return result;
}

std::vector<std::string> splitByLines (const std::string &str) {
  auto delimiter = std::string(EOL);
  auto pos = static_cast<std::string::size_type>(0);
  auto prev = static_cast<std::string::size_type>(0);
  auto result = std::vector<std::string>{};

  while ((pos = str.find(delimiter, prev)) != std::string::npos) {
    result.push_back(str.substr(prev, pos - prev));
    prev = pos + delimiter.size();
  }

  result.push_back(str.substr(prev));

  for (auto &it : result) {
    it = trim(it);

    if (it == "*") {
      it = "";
    } else if (it.starts_with("* ")) {
      it = it.substr(2);
    } else if (it.starts_with("*")) {
      it = it.substr(1);
    }
  }

  result.erase(std::remove_if(result.begin(), result.end(), [] (auto it) -> bool {
    return trim(it).empty();
  }), result.end());

  return result;
}

ParserComment parseComment (const std::string &content) {
  auto lines = splitByLines(content);
  auto description = std::string();
  auto image = std::string();
  auto notes = std::vector<std::string>{};
  auto params = std::map<std::string, std::string>{};
  auto ret = std::string();
  auto sign = std::string();
  auto isDescription = true;
  auto isNote = false;
  auto isParam = false;
  auto lastParamName = std::string();

  for (const auto &line : lines) {
    auto lineTrimmed = trim(line);

    if (lineTrimmed.starts_with('@')) {
      isDescription = false;
      isNote = false;
      isParam = false;

      if (lineTrimmed.starts_with("@note ")) {
        isNote = true;
        notes.push_back(trim(lineTrimmed.substr(6)) + (lineTrimmed.ends_with('\\') ? EOL : ""));
      } else if (lineTrimmed.starts_with("@param ")) {
        isParam = true;
        auto paramNameLen = static_cast<std::size_t>(7);

        while (std::isalnum(lineTrimmed[paramNameLen]) && paramNameLen < lineTrimmed.size()) {
          paramNameLen++;
        }

        auto paramName = trim(lineTrimmed.substr(7, paramNameLen - 7));
        auto afterParamName = trim(lineTrimmed.substr(paramNameLen));
        auto paramVal = trim(afterParamName.starts_with('-') ? afterParamName.substr(1) : afterParamName);

        params.insert_or_assign(paramName, paramVal);
        lastParamName = paramName;
      } else if (lineTrimmed.starts_with("@return ")) {
        ret = trim(lineTrimmed.substr(8));
      } else if (lineTrimmed.starts_with("@signature ")) {
        sign = trim(lineTrimmed.substr(11));
      }
    } else if (lineTrimmed.starts_with("![")) {
      image = lineTrimmed;
    } else if (isDescription && !lineTrimmed.empty() && !std::isalnum(lineTrimmed[0])) {
      description += (description.ends_with(EOL) ? "" : EOL) + lineTrimmed + EOL;
    } else if (isDescription) {
      description += " " + lineTrimmed;
    } else if (isNote) {
      auto &lastNote = notes.back();
      lastNote += (lastNote.ends_with(EOL) ? "" : " ") + lineTrimmed + (lineTrimmed.ends_with('\\') ? EOL : "");
    } else if (isParam && !lineTrimmed.empty() && !std::isalnum(lineTrimmed[0])) {
      params.find(lastParamName)->second += EOL + line;
    } else if (isParam) {
      params.find(lastParamName)->second += " " + lineTrimmed;
    }
  }

  return ParserComment{trim(description), image, notes, params, trim(ret), trim(sign)};
}

bool operator== (const ParserComment &lhs, const ParserComment &rhs) {
  return lhs.description == rhs.description &&
    lhs.image == rhs.image &&
    lhs.params == rhs.params &&
    lhs.notes == rhs.notes &&
    lhs.ret == rhs.ret &&
    lhs.sign == rhs.sign;
}
