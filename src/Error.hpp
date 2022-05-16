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
#define E0017 "E0017 - New line is not allowed as character literal"

#define E0100 "E0100 - Unexpected statement"
// #define E0101 "E0101 - Unexpected expression statement"
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
#define E0128 "E0128 - Variadic function parameters can't have default values"
#define E0129 "E0129 - Expected left parentheses after fn keyword"
#define E0130 "E0130 - Expected function param initializer"
#define E0131 "E0131 - Expected variable initializer"
#define E0132 "E0132 - Expected unary expression argument"
#define E0133 "E0133 - Expected assignment value"
#define E0134 "E0134 - Expected object property initializer"
#define E0135 "E0135 - Expected call expression argument"
#define E0136 "E0136 - Expected expression after left parenthesis"
#define E0137 "E0137 - Expected right-hand binary expression"
#define E0138 "E0138 - Expected ternary body expression"
#define E0139 "E0139 - Expected ternary alternative expression"
#define E0140 "E0140 - Expected lvalue as left operand of assignment"
#define E0141 "E0141 - Called object expected to be a function"
#define E0142 "E0142 - lvalue required as operand"

#define E1000 "E1000 - Tried accessing property of non object variable"
#define E1001 "E1001 - Tried accessing non existing object property"
#define E1002 "E1002 - Extraneous argument passed in call expression"
#define E1003 "E1003 - Binary operator cannot be applied to these operands"
#define E1004 "E1004 - Incompatible operand types"
#define E1005 "E1005 - Extra argument in call expression"
#define E1006 "E1006 - Variadic argument can't be passed by name"
#define E1007 "E1007 - Named arguments can't be followed by regular arguments"
#define E1008 "E1008 - Argument type doesn't match parameter type"
#define E1009 "E1009 - Missing required arguments"
#define E1010 "E1010 - Use of undeclared type"
#define E1011 "E1011 - Use of undeclared variable"

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
