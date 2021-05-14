/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ERROR_HPP
#define SRC_ERROR_HPP

#include <string>

class Error : private std::exception {
 public:
  const std::string message;
  Error (const std::string &message);
};

#endif
