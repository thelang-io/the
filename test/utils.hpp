/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#define EXPECT_THROW_WITH_MESSAGE(body, message) EXPECT_THROW({ \
    try { \
      body; \
    } catch (const Error &err) { \
      EXPECT_STREQ(std::string(message).c_str(), err.what()); \
      throw; \
    } \
  }, Error)

#endif
