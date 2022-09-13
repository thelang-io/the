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

#include "MockReader.hpp"

MockReader::MockReader (const std::string &content) : Reader("CMakeLists.txt") {
  this->path = "/test";
  this->content = content;
  this->size = this->content.length();

  ON_CALL(*this, eof).WillByDefault([this] () {
    return this->Reader::eof();
  });

  ON_CALL(*this, next).WillByDefault([this] () {
    return this->Reader::next();
  });

  ON_CALL(*this, seek).WillByDefault([this] (ReaderLocation l) {
    return this->Reader::seek(l);
  });
}

MockReader::~MockReader () {
}
