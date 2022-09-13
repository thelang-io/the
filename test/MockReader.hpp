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

#ifndef TEST_MOCK_READER_HPP
#define TEST_MOCK_READER_HPP

#include <gmock/gmock.h>
#include "../src/Reader.hpp"

class MockReader : public Reader {
 public:
  explicit MockReader (const std::string &);
  virtual ~MockReader ();

  MOCK_METHOD(bool, eof, (), (override, const));
  MOCK_METHOD((std::tuple<ReaderLocation, char>), next, (), (override));
  MOCK_METHOD(void, seek, (ReaderLocation), (override));
};

#endif
