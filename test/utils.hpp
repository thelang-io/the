/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <map>
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
std::map<std::string, std::string> getEnvVars ();
std::map<std::string, std::string> readTestFile (const std::string &, const std::string &, const std::vector<std::string> &);
std::vector<std::string> splitFileContent (const std::string &);

#endif
