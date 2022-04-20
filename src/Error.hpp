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
#define E0101 "E0101 - Unexpected expression statement"
#define E0102 "E0102 - Expected variable type after colon"
#define E0103 "E0103 - Expected left brace"
#define E0104 "E0104 - Expected right brace"
#define E0105 "E0105 - Unexpected statement in loop initialization"
#define E0106 "E0106 - Expected semicolon after loop initialization"
#define E0107 "E0107 - Unexpected token after loop initialization"
#define E0108 "E0108 - Expected semicolon after loop condition"
#define E0109 "E0109 - Expected right parentheses"
#define E0110 "E0110 - Expected property name after dot"
#define E0111 "E0111 - Expected colon after ternary expression"
#define E0112 "E0112 - Expected property name in object expression"
#define E0113 "E0113 - Expected colon after property name in object expression"
#define E0114 "E0114 - Only identifiers accepted as object expression names"
#define E0115 "E0115 - Expected function identifier after fn keyword"
#define E0116 "E0116 - Expected left parentheses after function identifier"
#define E0117 "E0117 - Expected identifier as function parameter name"
#define E0118 "E0118 - Expected function parameter type"
#define E0119 "E0119 - Expected function parameter type after parameter name"
#define E0120 "E0120 - Expected function return type after parameters list"
#define E0121 "E0121 - Expected object identifier after obj keyword"
#define E0122 "E0122 - Expected left brace after object identifier"
#define E0123 "E0123 - Expected object field name"
#define E0124 "E0124 - Expected colon after object field name"
#define E0125 "E0125 - Expected object field type after field name"
#define E0126 "E0126 - Empty object declarations are not allowed"
#define E0127 "E0127 - Expected right parenthesis"

#define E1000 "E1000 - Tried accessing property of non object variable"
#define E1001 "E1001 - Tried accessing non existing object property"
#define E1002 "E1002 - Extraneous argument passed in call expression"
#define E1003 "E1003 - Tried operation other than concatenation on string"
#define E1004 "E1004 - Incompatible operand types"
#define E1005 "E1005 - Extra argument in call expression"
#define E1006 "E1006 - Variadic argument can't be passed by name"
#define E1007 "E1007 - Named arguments can't be followed by regular arguments"
#define E1008 "E1008 - Argument type doesn't match parameter type"
#define E1009 "E1009 - Missing required arguments"

class Error;

class Error : public std::exception {
 public:
  explicit Error (const std::string &);
  Error (Reader *, ReaderLocation, const std::string &);
  Error (Reader *, ReaderLocation, ReaderLocation, const std::string &);

  const char *what () const noexcept override;

 protected:
  std::string message_;
};

#endif
