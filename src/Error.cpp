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

#include "Error.hpp"
#include "config.hpp"

Error::Error (const std::string &message) {
  this->message_ = message;
}

const char *Error::what () const noexcept {
  return this->message_.c_str();
}

Error::Error (Reader *reader, ReaderLocation start, const std::string &message) : Error(reader, start, reader->loc, message) {
}

Error::Error (Reader *reader, ReaderLocation start, ReaderLocation end, const std::string &message) {
  auto line = std::string();
  reader->seek(ReaderLocation{start.pos - start.col, start.line});

  while (!reader->eof()) {
    auto [_, ch] = reader->next();

    if (ch == '\n') {
      break;
    }

    line += ch;
  }

  auto lineNumStr = std::to_string(start.line);

  this->message_ += reader->path + ':' + start.str() + ": " + message + EOL;
  this->message_ += "  " + lineNumStr + " | " + line + EOL;
  this->message_ += "  " + std::string(lineNumStr.size(), ' ') + " | " + std::string(start.col, ' ') + '^';

  auto underlineLen = static_cast<std::size_t>(0);

  if (start.line == end.line && line.size() > end.col) {
    underlineLen = end.col - start.col;
  } else if (line.size() > start.col) {
    underlineLen = line.size() - start.col;
  }

  if (underlineLen != 0) {
    this->message_ += std::string(underlineLen - 1, '~');
  }
}
