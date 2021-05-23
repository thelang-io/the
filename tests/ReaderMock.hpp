/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TESTS_READERMOCK_HPP
#define TESTS_READERMOCK_HPP

#include "../src/Reader.hpp"

class MockReader : public Reader {
 public:
  MockReader () : Reader("/bin/sh") {
    ON_CALL(*this, content).WillByDefault([this] () {
      return this->Reader::content();
    });

    ON_CALL(*this, eof).WillByDefault([this] () {
      return this->Reader::eof();
    });

    ON_CALL(*this, loc).WillByDefault([this] () {
      return this->Reader::loc();
    });

    ON_CALL(*this, next).WillByDefault([this] () {
      return this->Reader::next();
    });

    ON_CALL(*this, path).WillByDefault([this] () {
      return this->Reader::path();
    });

    ON_CALL(*this, seek).WillByDefault([this] (const ReaderLocation &arg1) {
      return this->Reader::seek(arg1);
    });
  }

  explicit MockReader (const std::string &content) : MockReader() {
    this->content_ = content;
  }

  MOCK_METHOD(std::string, content, (), (const, override));
  MOCK_METHOD(bool, eof, (), (const, override));
  MOCK_METHOD(ReaderLocation, loc, (), (const, override));
  MOCK_METHOD(char, next, (), (override));
  MOCK_METHOD(fs::path, path, (), (const, override));
  MOCK_METHOD(void, seek, (const ReaderLocation &loc), (override));
};

#endif
