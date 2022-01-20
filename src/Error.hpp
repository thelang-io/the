/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ERROR_HPP
#define SRC_ERROR_HPP

#include <exception>
#include <string>

class Error : public std::exception {
 public:
  explicit Error (const std::string &);
  const char *what () const noexcept override;

 private:
  std::string _message;
};

#endif
