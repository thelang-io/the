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
  MockReader () : Reader(fs::temp_directory_path() / "test.out") {
  }

  MOCK_METHOD(std::string, content, (), (const, override));
  MOCK_METHOD(bool, eof, (), (const, override));
  MOCK_METHOD(ReaderLocation, loc, (), (const, override));
  MOCK_METHOD(char, next, (), (override));
  MOCK_METHOD(fs::path, path, (), (const, override));
  MOCK_METHOD(void, seek, (const ReaderLocation &loc), (override));
};

#endif
