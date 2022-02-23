/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
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
