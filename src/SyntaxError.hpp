/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_SYNTAXERROR_HPP
#define SRC_SYNTAXERROR_HPP

#include "Error.hpp"
#include "Reader.hpp"

class SyntaxError : public Error {
 public:
  SyntaxError (Reader *, const ReaderLocation &, const ReaderLocation &, const std::string &);
};

#endif
