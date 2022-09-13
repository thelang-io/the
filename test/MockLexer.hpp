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

#ifndef TEST_MOCK_LEXER_HPP
#define TEST_MOCK_LEXER_HPP

#include <gmock/gmock.h>
#include "../src/Lexer.hpp"
#include "MockReader.hpp"

class MockLexer : public Lexer {
 public:
  explicit MockLexer (const std::string &);
  virtual ~MockLexer ();

  MOCK_METHOD((std::tuple<ReaderLocation, Token>), next, (), (override));
  MOCK_METHOD(void, seek, (ReaderLocation), (override));

 private:
  testing::NiceMock<MockReader> _r;
};

#endif
