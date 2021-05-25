/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ERROR_HPP
#define SRC_ERROR_HPP

#include <string>

#define E0000 std::string("E0000: Unexpected token")
#define E0001 std::string("E0001: Unterminated block comment")
#define E0002 std::string("E0002: Unterminated character literal")
#define E0003 std::string("E0003: Unterminated string literal")
#define E0004 std::string("E0004: Empty character literal")
#define E0005 std::string("E0005: Illegal character escape sequence")
#define E0006 std::string("E0006: Too many characters in character literal")
#define E0007 std::string("E0007: Integer literals with leading zero are not allowed")
#define E0008 std::string("E0008: Invalid binary integer literal")
#define E0009 std::string("E0009: Invalid decimal integer literal")
#define E0010 std::string("E0010: Invalid hexadecimal integer literal")
#define E0011 std::string("E0011: Invalid octal integer literal")
#define E0012 std::string("E0012: Invalid float literal")
#define E0013 std::string("E0013: Invalid float literal exponent")
#define E0014 std::string("E0014: Binary float literals are not allowed")
#define E0015 std::string("E0015: Hexadecimal float literals are not allowed")
#define E0016 std::string("E0016: Octal float literals are not allowed")

class Error : public std::exception {
 public:
  std::string message;
  std::string name = "Error";

  Error () = default;
  explicit Error (const std::string &);
  [[nodiscard]] const char *what () const noexcept override;
};

#endif
