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

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <map>
#include <optional>
#include <vector>
#include "../src/Error.hpp"

#define EXPECT_THROW_WITH_MESSAGE(body, message) EXPECT_THROW({ \
    try { \
      body; \
    } catch (const Error &err) { \
      EXPECT_STREQ(std::string(message).c_str(), err.what()); \
      throw; \
    } \
  }, Error)

std::tuple<std::string, std::string, int> execCmd (const std::string &, const std::string &);
std::optional<std::string> getEnvVar (const std::string &);
std::map<std::string, std::string> readTestFile (const std::string &, const std::string &, const std::vector<std::string> &);
std::vector<std::string> splitFileContent (const std::string &);

#endif
