/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEST_MOCK_READER_HPP
#define TEST_MOCK_READER_HPP

#include <gmock/gmock.h>
#include "../src/Reader.hpp"

class MockReader : public Reader {
 public:
  explicit MockReader (const std::string &);
  virtual ~MockReader ();

  MOCK_METHOD(bool, eof, (), (override, const));
  MOCK_METHOD(char, next, (), (override));
  MOCK_METHOD(void, seek, (ReaderLocation), (override));
};

#endif
