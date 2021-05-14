/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ERROR_HPP
#define SRC_ERROR_HPP

#include <string>

class Error : public std::exception {
 public:
  std::string message;
  std::string name = "Error";

  Error () = default;
  Error (const std::string &msg);
  virtual const char *what () const noexcept override;
};

#endif
