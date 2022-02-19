/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ERROR_HPP
#define SRC_ERROR_HPP

#include "Reader.hpp"

#define E0000 "E0000 - Unexpected token"
#define E0001 "E0001 - Unterminated block comment"
#define E0002 "E0002 - Unterminated character literal"
#define E0003 "E0003 - Unterminated string literal"
#define E0004 "E0004 - Empty character literal"
#define E0005 "E0005 - Illegal character escape sequence"
#define E0006 "E0006 - Too many characters in character literal"
#define E0007 "E0007 - Integer literals with leading zero are not allowed"
#define E0008 "E0008 - Invalid binary integer literal"
#define E0009 "E0009 - Invalid decimal integer literal"
#define E0010 "E0010 - Invalid hexadecimal integer literal"
#define E0011 "E0011 - Invalid octal integer literal"
#define E0012 "E0012 - Invalid float literal"
#define E0013 "E0013 - Invalid float literal exponent"
#define E0014 "E0014 - Binary float literals are not allowed"
#define E0015 "E0015 - Hexadecimal float literals are not allowed"
#define E0016 "E0016 - Octal float literals are not allowed"

#define E0100 "E0100 - Unexpected statement"
#define E0101 "E0101 - Unknown expression statement"
#define E0102 "E0102 - Expected variable type after colon"
#define E0103 "E0103 - Expected left brace"
#define E0104 "E0104 - Expected right brace"
#define E0105 "E0105 - Unexpected statement in loop condition"
#define E0106 "E0106 - Expected semicolon after loop initialization"
#define E0107 "E0107 - Unexpected token after loop initialization"
#define E0108 "E0108 - Expected semicolon after loop condition"
#define E0109 "E0109 - Expected right parentheses"
#define E0110 "E0110 - Expected property name after dot"
#define E0111 "E0111 - Expected colon after ternary expression"

class Error;

class Error : public std::exception {
 public:
  explicit Error (const std::string &);
  Error (Reader *, ReaderLocation, const std::string &);

  const char *what () const noexcept override;

 protected:
  std::string message_;
};

#endif
