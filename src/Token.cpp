/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Token.hpp"

bool Token::isDigit (char ch) {
  return std::isdigit(ch);
}

bool Token::isIdContinue (char ch) {
  return std::isalnum(ch) || ch == '_';
}

bool Token::isIdStart (char ch) {
  return std::isalpha(ch) || ch == '_';
}

bool Token::isLitCharEscape (char ch) {
  return std::string("0tnr\"'\\").find(ch) != std::string::npos;
}

bool Token::isLitIntBin (char ch) {
  return ch == '0' || ch == '1';
}

bool Token::isLitIntDec (char ch) {
  return std::isdigit(ch);
}

bool Token::isLitIntHex (char ch) {
  return std::string("ABCDEFabcdef0123456789").find(ch) != std::string::npos;
}

bool Token::isLitIntOct (char ch) {
  return std::string("01234567").find(ch) != std::string::npos;
}

bool Token::isLitStrEscape (char ch) {
  return Token::isLitCharEscape(ch) || ch == '{';
}

bool Token::isNotNewline (char ch) {
  return ch != '\n';
}

bool Token::isWhitespace (char ch) {
  return std::isspace(ch);
}

int Token::precedence () const {
  if (this->type == TK_OP_LPAR || this->type == TK_OP_RPAR) {
    return 18;
  } else if (this->type == TK_OP_DOT || this->type == TK_OP_DOT_DOT_DOT || this->type == TK_OP_LBRACK || this->type == TK_OP_QN_DOT || this->type == TK_OP_RBRACK) {
    return 17;
  } else if (this->type == TK_OP_MINUS_MINUS || this->type == TK_OP_PLUS_PLUS) {
    return 16;
  } else if (this->type == TK_OP_EXCL || this->type == TK_OP_EXCL_EXCL || this->type == TK_OP_TILDE) {
    return 15;
  } else if (this->type == TK_OP_STAR_STAR) {
    return 14;
  } else if (this->type == TK_OP_PERCENT || this->type == TK_OP_SLASH || this->type == TK_OP_STAR) {
    return 13;
  } else if (this->type == TK_OP_MINUS || this->type == TK_OP_PLUS) {
    return 12;
  } else if (this->type == TK_OP_LSHIFT || this->type == TK_OP_RSHIFT) {
    return 11;
  } else if (this->type == TK_OP_GT || this->type == TK_OP_GT_EQ || this->type == TK_OP_LT || this->type == TK_OP_LT_EQ) {
    return 10;
  } else if (this->type == TK_OP_EQ_EQ || this->type == TK_OP_EXCL_EQ) {
    return 9;
  } else if (this->type == TK_OP_AND) {
    return 8;
  } else if (this->type == TK_OP_CARET) {
    return 7;
  } else if (this->type == TK_OP_OR) {
    return 6;
  } else if (this->type == TK_OP_AND_AND) {
    return 5;
  } else if (this->type == TK_OP_OR_OR || this->type == TK_OP_QN_QN) {
    return 4;
  } else if (this->type == TK_OP_COLON || this->type == TK_OP_QN) {
    return 3;
  } else if (
    this->type == TK_OP_AND_AND_EQ ||
    this->type == TK_OP_AND_EQ ||
    this->type == TK_OP_CARET_EQ ||
    this->type == TK_OP_EQ ||
    this->type == TK_OP_LSHIFT_EQ ||
    this->type == TK_OP_MINUS_EQ ||
    this->type == TK_OP_OR_EQ ||
    this->type == TK_OP_OR_OR_EQ ||
    this->type == TK_OP_PERCENT_EQ ||
    this->type == TK_OP_PLUS_EQ ||
    this->type == TK_OP_QN_QN_EQ ||
    this->type == TK_OP_RSHIFT_EQ ||
    this->type == TK_OP_SLASH_EQ ||
    this->type == TK_OP_STAR_EQ ||
    this->type == TK_OP_STAR_STAR_EQ
  ) {
    return 2;
  } else if (this->type == TK_OP_COMMA) {
    return 1;
  }

  return 0;
}

bool operator== (const Token &lhs, const Token &rhs) {
  return lhs.type == rhs.type && lhs.start == rhs.start && lhs.end == rhs.end && lhs.val == rhs.val;
}
