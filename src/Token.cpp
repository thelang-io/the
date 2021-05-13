/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Token.hpp"

static const char *token_type[] = {
  #define GEN_TOKEN_STR(x) #x,
  FOREACH_TOKEN(GEN_TOKEN_STR)
  #undef GEN_TOKEN_STR
};

bool Token::isWhitespace (char ch) {
  return ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t';
}

Token::Token (const TokenType type, const std::string &val)
  : type(type), val(val) {
}

bool Token::operator== (const TokenType rhs) const {
  return this->type == rhs;
}

bool Token::operator!= (const TokenType rhs) const {
  return this->type != rhs;
}

std::string Token::str () const {
  return token_type[this->type];
}
