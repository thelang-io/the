/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <utility>
#include "Error.hpp"

Error::Error (std::string message) : _message(std::move(message)) {
}

const char *Error::what () const noexcept {
  return this->_message.c_str();
}
