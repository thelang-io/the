/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"

Error::Error (const std::string &msg) {
  this->message = this->name + ": " + msg;
}

const char *Error::what () const noexcept {
  return this->message.c_str();
}
