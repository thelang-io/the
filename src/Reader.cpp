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

#include "Reader.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Error.hpp"
#include "config.hpp"

std::string ReaderLocation::str () const {
  return std::to_string(this->line) + ":" + std::to_string(this->col + 1);
}

Reader::Reader (const std::string &p) {
  std::string canonicalPath;

  try {
    canonicalPath = std::filesystem::canonical(p).string();
  } catch (const std::exception &) {
    throw Error(R"(no such file ")" + p + R"(")");
  }

  if (!std::filesystem::is_regular_file(canonicalPath)) {
    throw Error(R"(path ")" + p + R"(" is not a file)");
  }

  auto f = std::ifstream(canonicalPath);

  if (!f.is_open() || f.fail()) {
    throw Error(R"(unable to open file ")" + p + R"(")");
  }

  auto c = std::stringstream();
  c << f.rdbuf();

  this->loc = ReaderLocation{};
  this->path = canonicalPath;
  this->content = c.str();
  this->size = this->content.length();
}

bool Reader::eof () const {
  return this->loc.pos >= this->size;
}

std::tuple<ReaderLocation, char> Reader::next () {
  if (this->eof()) {
    throw Error("tried to read on reader eof");
  }

  auto l = this->loc;
  auto ch = this->content[this->loc.pos];

  #if defined(OS_WINDOWS)
    if (ch == '\r' && this->content[this->loc.pos + 1] == '\n') {
      ch = '\n';

      this->loc.col = 0;
      this->loc.line += 1;
      this->loc.pos += 2;
    } else {
      this->loc.col += 1;
      this->loc.pos += 1;
    }
  #else
    if (ch == '\n') {
      this->loc.col = 0;
      this->loc.line += 1;
    } else {
      this->loc.col += 1;
    }

    this->loc.pos += 1;
  #endif

  return {l, ch};
}

void Reader::seek (ReaderLocation l) {
  this->loc = l;
}

bool operator== (const ReaderLocation &lhs, const ReaderLocation &rhs) {
  return lhs.pos == rhs.pos && lhs.line == rhs.line && lhs.col == rhs.col;
}
