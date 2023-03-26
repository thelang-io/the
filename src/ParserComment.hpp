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

#ifndef SRC_PARSER_COMMENT_HPP
#define SRC_PARSER_COMMENT_HPP

#include <map>
#include <string>
#include <vector>

struct ParserComment {
  std::string description = "";
  std::string image = "";
  std::vector<std::string> notes = {};
  std::map<std::string, std::string> params = {};
  std::string ret = "";
  std::string sign = "";
};

ParserComment parseComment (const std::string &);
bool operator== (const ParserComment &, const ParserComment &);

#endif
