/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"

Error::Error (const std::string &message) {
  this->message_ = message;
}

const char *Error::what () const noexcept {
  return this->message_.c_str();
}
