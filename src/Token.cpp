/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Token.hpp"
#include <regex>
#include "Error.hpp"

std::string tokenTypeToStr (TokenType type) {
  switch (type) {
    case TK_EOF: return "EOF";
    case TK_ID: return "ID";
    case TK_KW_AS: return "KW_AS";
    case TK_KW_ASYNC: return "KW_ASYNC";
    case TK_KW_AWAIT: return "KW_AWAIT";
    case TK_KW_BREAK: return "KW_BREAK";
    case TK_KW_CASE: return "KW_CASE";
    case TK_KW_CATCH: return "KW_CATCH";
    case TK_KW_CONST: return "KW_CONST";
    case TK_KW_CONTINUE: return "KW_CONTINUE";
    case TK_KW_DEFAULT: return "KW_DEFAULT";
    case TK_KW_ELIF: return "KW_ELIF";
    case TK_KW_ELSE: return "KW_ELSE";
    case TK_KW_ENUM: return "KW_ENUM";
    case TK_KW_EXPORT: return "KW_EXPORT";
    case TK_KW_FALLTHROUGH: return "KW_FALLTHROUGH";
    case TK_KW_FALSE: return "KW_FALSE";
    case TK_KW_FN: return "KW_FN";
    case TK_KW_FROM: return "KW_FROM";
    case TK_KW_IF: return "KW_IF";
    case TK_KW_IMPORT: return "KW_IMPORT";
    case TK_KW_IS: return "KW_IS";
    case TK_KW_LOOP: return "KW_LOOP";
    case TK_KW_MAIN: return "KW_MAIN";
    case TK_KW_MATCH: return "KW_MATCH";
    case TK_KW_MUT: return "KW_MUT";
    case TK_KW_NIL: return "KW_NIL";
    case TK_KW_OBJ: return "KW_OBJ";
    case TK_KW_RETURN: return "KW_RETURN";
    case TK_KW_THROW: return "KW_THROW";
    case TK_KW_TRUE: return "KW_TRUE";
    case TK_KW_TRY: return "KW_TRY";
    case TK_KW_UNION: return "KW_UNION";
    case TK_LIT_CHAR: return "LIT_CHAR";
    case TK_LIT_FLOAT: return "LIT_FLOAT";
    case TK_LIT_INT_BIN: return "LIT_INT_BIN";
    case TK_LIT_INT_DEC: return "LIT_INT_DEC";
    case TK_LIT_INT_HEX: return "LIT_INT_HEX";
    case TK_LIT_INT_OCT: return "LIT_INT_OCT";
    case TK_LIT_STR: return "LIT_STR";
    case TK_OP_AND: return "OP_AND";
    case TK_OP_AND_AND: return "OP_AND_AND";
    case TK_OP_AND_AND_EQ: return "OP_AND_AND_EQ";
    case TK_OP_AND_EQ: return "OP_AND_EQ";
    case TK_OP_CARET: return "OP_CARET";
    case TK_OP_CARET_EQ: return "OP_CARET_EQ";
    case TK_OP_COLON: return "OP_COLON";
    case TK_OP_COLON_EQ: return "OP_COLON_EQ";
    case TK_OP_COMMA: return "OP_COMMA";
    case TK_OP_DOT: return "OP_DOT";
    case TK_OP_DOT_DOT_DOT: return "OP_DOT_DOT_DOT";
    case TK_OP_EQ: return "OP_EQ";
    case TK_OP_EQ_EQ: return "OP_EQ_EQ";
    case TK_OP_EXCL: return "OP_EXCL";
    case TK_OP_EXCL_EQ: return "OP_EXCL_EQ";
    case TK_OP_EXCL_EXCL: return "OP_EXCL_EXCL";
    case TK_OP_GT: return "OP_GT";
    case TK_OP_GT_EQ: return "OP_GT_EQ";
    case TK_OP_LBRACE: return "OP_LBRACE";
    case TK_OP_LBRACK: return "OP_LBRACK";
    case TK_OP_LPAR: return "OP_LPAR";
    case TK_OP_LSHIFT: return "OP_LSHIFT";
    case TK_OP_LSHIFT_EQ: return "OP_LSHIFT_EQ";
    case TK_OP_LT: return "OP_LT";
    case TK_OP_LT_EQ: return "OP_LT_EQ";
    case TK_OP_MINUS: return "OP_MINUS";
    case TK_OP_MINUS_EQ: return "OP_MINUS_EQ";
    case TK_OP_MINUS_MINUS: return "OP_MINUS_MINUS";
    case TK_OP_OR: return "OP_OR";
    case TK_OP_OR_EQ: return "OP_OR_EQ";
    case TK_OP_OR_OR: return "OP_OR_OR";
    case TK_OP_OR_OR_EQ: return "OP_OR_OR_EQ";
    case TK_OP_PERCENT: return "OP_PERCENT";
    case TK_OP_PERCENT_EQ: return "OP_PERCENT_EQ";
    case TK_OP_PLUS: return "OP_PLUS";
    case TK_OP_PLUS_EQ: return "OP_PLUS_EQ";
    case TK_OP_PLUS_PLUS: return "OP_PLUS_PLUS";
    case TK_OP_QN: return "OP_QN";
    case TK_OP_QN_DOT: return "OP_QN_DOT";
    case TK_OP_QN_QN: return "OP_QN_QN";
    case TK_OP_QN_QN_EQ: return "OP_QN_QN_EQ";
    case TK_OP_RBRACE: return "OP_RBRACE";
    case TK_OP_RBRACK: return "OP_RBRACK";
    case TK_OP_RPAR: return "OP_RPAR";
    case TK_OP_RSHIFT: return "OP_RSHIFT";
    case TK_OP_RSHIFT_EQ: return "OP_RSHIFT_EQ";
    case TK_OP_SEMI: return "OP_SEMI";
    case TK_OP_SLASH: return "OP_SLASH";
    case TK_OP_SLASH_EQ: return "OP_SLASH_EQ";
    case TK_OP_STAR: return "OP_STAR";
    case TK_OP_STAR_EQ: return "OP_STAR_EQ";
    case TK_OP_STAR_STAR: return "OP_STAR_STAR";
    case TK_OP_STAR_STAR_EQ: return "OP_STAR_STAR_EQ";
    case TK_OP_TILDE: return "OP_TILDE";
  }

  throw Error("Error: Tried stringify unknown token");
}

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
  return std::string("0nrt\"'\\").find(ch) != std::string::npos;
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

  throw Error("Error: Tried precedence for unknown token");
}

std::string Token::str () const {
  auto escVal = std::regex_replace(this->val, std::regex("\\\\0"), "\\\\0");
  escVal = std::regex_replace(escVal, std::regex("\\\\n"), "\\\\n");
  escVal = std::regex_replace(escVal, std::regex("\\\\r"), "\\\\r");
  escVal = std::regex_replace(escVal, std::regex("\\\\t"), "\\\\t");
  escVal = std::regex_replace(escVal, std::regex("\\n"), "\\n");
  escVal = std::regex_replace(escVal, std::regex("\\r"), "\\r");
  escVal = std::regex_replace(escVal, std::regex("\\t"), "\\t");

  auto result = std::string();

  result += tokenTypeToStr(this->type) + "(" + this->start.str() + "-" + this->end.str() + ")";
  result += escVal.empty() ? "" : ": " + escVal;

  return result;
}

std::string Token::xml () const {
  auto escVal = std::regex_replace(this->val, std::regex("\\\\0"), "\\\\0");
  escVal = std::regex_replace(escVal, std::regex("\\\\n"), "\\\\n");
  escVal = std::regex_replace(escVal, std::regex("\\\\r"), "\\\\r");
  escVal = std::regex_replace(escVal, std::regex("\\\\t"), "\\\\t");
  escVal = std::regex_replace(escVal, std::regex("\\n"), "\\n");
  escVal = std::regex_replace(escVal, std::regex("\\r"), "\\r");
  escVal = std::regex_replace(escVal, std::regex("\\t"), "\\t");
  escVal = std::regex_replace(escVal, std::regex("\""), "\\\"");

  auto result = std::string();

  result += R"(<Token type=")" + tokenTypeToStr(this->type);
  result += R"(" val=")" + escVal;
  result += R"(" start=")" + this->start.str();
  result += R"(" end=")" + this->end.str();
  result += R"(" />)";

  return result;
}
