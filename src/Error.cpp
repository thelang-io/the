/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"

Error::Error (const std::string &message) {
  this->_message = "Error: " + message;
}

const char *Error::what () const noexcept {
  return this->_message.c_str();
}
