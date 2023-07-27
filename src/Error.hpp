/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
// #define E1001 "E1001 - Tried accessing non-existing object property"
#define E0102 "E0102 - Expected variable type after colon"
#define E0103 "E0103 - Expected left brace"
#define E0104 "E0104 - Expected right brace"
#define E0105 "E0105 - Unexpected statement in loop initialization"
#define E0106 "E0106 - Expected semicolon after loop initialization"
// #define E1007 "E1007 - Named arguments can't be followed by regular arguments"
#define E0108 "E0108 - Expected semicolon after loop condition"
#define E0109 "E0109 - Expected right parentheses"
#define E0110 "E0110 - Expected property name after dot"
#define E0111 "E0111 - Expected colon after ternary expression"
#define E0112 "E0112 - Expected property name in object expression"
#define E0113 "E0113 - Expected colon after property name in object expression"
// #define E0114 "E0114 - Only identifiers accepted as object expression names"
#define E0115 "E0115 - Expected function identifier after fn keyword"
#define E0116 "E0116 - Expected left parentheses after function identifier"
#define E0117 "E0117 - Expected identifier as function parameter name"
#define E0118 "E0118 - Expected function parameter type"
#define E0119 "E0119 - Expected function parameter type after parameter name"
#define E0120 "E0120 - Expected function return type after parameters list"
#define E0121 "E0121 - Expected object identifier after obj keyword"
#define E0122 "E0122 - Expected left brace after object identifier"
#define E0123 "E0123 - Expected object member declaration"
// #define E0124 "E0124 - Expected colon after object field name"
// #define E0125 "E0125 - Expected object field type after field name"
// #define E0126 "E0126 - Empty object declarations are not allowed"
#define E0127 "E0127 - Expected right parenthesis"
#define E0128 "E0128 - Variadic function parameters can't have default values"
// #define E0129 "E0129 - Expected left parentheses after fn keyword"
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
// #define E0141 "E0141 - Called object expected to be a function"
#define E0142 "E0142 - lvalue required as operand"
#define E0143 "E0143 - Expected if condition expression"
#define E0144 "E0144 - Loop initialization is mutable by default"
#define E0145 "E0145 - Expected parameter name after `mut` keyword in function type"
#define E0146 "E0146 - Expected colon after parameter name in function type"
#define E0147 "E0147 - Expected reference expression body"
#define E0148 "E0148 - Invalid reference to rvalue"
#define E0149 "E0149 - Expected reference expression type"
#define E0150 "E0150 - Expected expression after left bracket"
#define E0151 "E0151 - Expected right bracket"
#define E0152 "E0152 - Expected array expression element"
#define E0153 "E0153 - Expected function type arrow after arguments list"
#define E0154 "E0154 - Expected enumeration identifier after enum keyword"
#define E0155 "E0155 - Expected left brace after enumeration identifier"
#define E0156 "E0156 - Expected enumeration member name"
#define E0157 "E0157 - Expected enumeration member initializer"
#define E0158 "E0158 - Enumerations should have at least one member"
#define E0159 "E0159 - Expected property name after dot"
#define E0160 "E0160 - Expected type declaration name after `type` keyword"
#define E0161 "E0161 - Expected equals after type declaration name"
#define E0162 "E0162 - Expected type declaration type"
#define E0163 "E0163 - Expected union subtype"
#define E0164 "E0164 - Expected type after `is` keyword"
#define E0165 "E0165 - Expected constant name after `const` keyword"
#define E0166 "E0166 - Expected constant type after colon"
#define E0167 "E0167 - Expected constant initializer"
#define E0168 "E0168 - Expected map property name"
#define E0169 "E0169 - Expected colon after map property name"
#define E0170 "E0170 - Expected map property initializer"
#define E0171 "E0171 - Only primitive types are allowed to be used as map key type"
#define E0172 "E0172 - Expected right bracket after map key type"
#define E0173 "E0173 - Object declaration field is not allowed to have initializer"
#define E0174 "E0174 - Try statement requires at least one catch clause to be present"
// #define E0175 "E0175 - Try statement can't have multiple finally blocks"
#define E0176 "E0176 - Unexpected statement after catch keyword"
#define E0177 "E0177 - Catch clause parameter can't be mutable"
#define E0178 "E0178 - Expected throw statement argument"
#define E0179 "E0179 - Expected function declaration after `async` keyword"
#define E0180 "E0180 - Expected function type after `async` keyword"
#define E0181 "E0181 - Expected expression after `await` keyword"
#define E0182 "E0182 - Expected type after `as` keyword"

#define E1001 "E1001 - Tried accessing non-existing object property"
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
#define E1012 "E1012 - Index type expected to be integer"
#define E1013 "E1013 - Indexing allowed only on arrays and strings"
#define E1014 "E1014 - Called object expected to be a function"
#define E1015 "E1015 - Tried setting non-existing object property"
#define E1016 "E1016 - Unable to deduce array literal type"
#define E1017 "E1017 - Incompatible array literal element types"
#define E1018 "E1018 - Unable to deduce optional type"
#define E1019 "E1019 - `nil` literal is not assignable to type `@type`"
#define E1020 "E1020 - Unable to deduce types for expression"
#define E1021 "E1021 - Function declaration with name `main` is not allowed"
#define E1022 "E1022 - Void type can only be used as function return type"
#define E1023 "E1023 - Unable to deduce enumeration member type"
#define E1024 "E1024 - Tried accessing non-existing enumeration member"
#define E1025 "E1025 - Constant are allowed only on top-level scope"
#define E1026 "E1026 - Non-mutable variable declaration is now allowed on top-level scope"
#define E1027 "E1027 - Unable to deduce map literal value type"
#define E1028 "E1028 - Throw argument required to be object with first field string `message` and second field string `stack`"
#define E1029 "E1029 - Catch block can only catch object with first field string `message` and second field string `stack`"
#define E1030 "E1030 - Awaiting is only possible on asynchronous functions"

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
