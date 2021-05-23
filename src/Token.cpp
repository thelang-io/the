/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <regex>
#include "Token.hpp"

static const char *token_type[] = {
  #define GEN_TOKEN_STR(x) #x,
  FOREACH_TOKEN(GEN_TOKEN_STR)
  #undef GEN_TOKEN_STR
};

bool Token::isDigit (const char ch) {
  return std::string("0123456789").find(ch) != std::string::npos;
}

bool Token::isLitCharEscape (const char ch) {
  return std::string("0tnr\"'\\").find(ch) != std::string::npos;
}

bool Token::isLitIdContinue (const char ch) {
  return isalnum(ch) || ch == '_';
}

bool Token::isLitIdStart (const char ch) {
  return isalpha(ch) || ch == '_';
}

bool Token::isLitIntBin (const char ch) {
  return ch == '0' || ch == '1';
}

bool Token::isLitIntDec (const char ch) {
  return Token::isDigit(ch);
}

bool Token::isLitIntHex (const char ch) {
  return std::string("ABCDEFabcdef0123456789").find(ch) != std::string::npos;
}

bool Token::isLitIntOct (const char ch) {
  return std::string("01234567").find(ch) != std::string::npos;
}

bool Token::isLitStrEscape (const char ch) {
  return Token::isLitCharEscape(ch) || ch == '{';
}

bool Token::isWhitespace (const char ch) {
  return std::string("\r\n\t ").find(ch) != std::string::npos;
}

std::string Token::litIntToStr (TokenType tt) {
  if (tt == litIntBin) return "binary";
  if (tt == litIntHex) return "hexadecimal";
  if (tt == litIntOct) return "octal";
  return "decimal";
}

Token::Token (TokenType t, const std::string &v, const ReaderLocation &s, const ReaderLocation &e)
  : type(t), val(v), start(s), end(e) {
}

bool Token::operator== (const Token &rhs) const {
  return this->end == rhs.end && this->start == rhs.start && this->type == rhs.type && this->val == rhs.val;
}

bool Token::operator!= (const Token &rhs) const {
  return !(*this == rhs);
}

bool Token::operator== (TokenType rhs) const {
  return this->type == rhs;
}

bool Token::operator!= (TokenType rhs) const {
  return !(*this == rhs);
}

std::string Token::str () const {
  auto escVal = std::regex_replace(this->val, std::regex("\\n"), "\\\\n");
  escVal = std::regex_replace(escVal, std::regex("\\r"), "\\\\r");
  escVal = std::regex_replace(escVal, std::regex("\\t"), "\\\\t");
  escVal = std::regex_replace(escVal, std::regex("\n"), "\\n");
  escVal = std::regex_replace(escVal, std::regex("\r"), "\\r");
  escVal = std::regex_replace(escVal, std::regex("\t"), "\\t");

  return std::string(token_type[this->type]) + '(' +
    std::to_string(this->start.line) + ':' + std::to_string(this->start.col + 1) + '-' +
    std::to_string(this->end.line) + ':' + std::to_string(this->end.col + 1) + "): " + escVal;
}
