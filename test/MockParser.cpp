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

#include "MockParser.hpp"

MockParser::MockParser (const std::string &content) : Parser(&this->_l), _l(content) {
  this->reader = this->_l.reader;

  ON_CALL(*this, doc).WillByDefault([&] () {
    return this->Parser::doc();
  });

  ON_CALL(*this, next).WillByDefault([&] (bool _1, bool _2) {
    return this->Parser::next(_1, _2);
  });

  ON_CALL(*this, xml).WillByDefault([&] () {
    return this->Parser::xml();
  });
}

MockParser::~MockParser () {
}
