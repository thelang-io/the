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

#include "Token.hpp"
#include <algorithm>
#include "Error.hpp"

std::string tokenTypeToStr (TokenType type) {
  switch (type) {
    case TK_UNKNOWN: return "UNKNOWN";
    case TK_EOF: return "EOF";
    case TK_ID: return "ID";
    case TK_COMMENT_BLOCK: return "COMMENT_BLOCK";
    case TK_COMMENT_LINE: return "COMMENT_LINE";
    case TK_KW_AS: return "KW_AS";
    case TK_KW_ASYNC: return "KW_ASYNC";
    case TK_KW_AWAIT: return "KW_AWAIT";
    case TK_KW_BREAK: return "KW_BREAK";
    case TK_KW_CATCH: return "KW_CATCH";
    case TK_KW_CONST: return "KW_CONST";
    case TK_KW_CONTINUE: return "KW_CONTINUE";
    case TK_KW_ELIF: return "KW_ELIF";
    case TK_KW_ELSE: return "KW_ELSE";
    case TK_KW_ENUM: return "KW_ENUM";
    case TK_KW_FALSE: return "KW_FALSE";
    case TK_KW_FINALLY: return "KW_FINALLY";
    case TK_KW_FN: return "KW_FN";
    case TK_KW_FROM: return "KW_FROM";
    case TK_KW_IF: return "KW_IF";
    case TK_KW_IS: return "KW_IS";
    case TK_KW_LOOP: return "KW_LOOP";
    case TK_KW_MAIN: return "KW_MAIN";
    case TK_KW_MUT: return "KW_MUT";
    case TK_KW_NIL: return "KW_NIL";
    case TK_KW_OBJ: return "KW_OBJ";
    case TK_KW_REF: return "KW_REF";
    case TK_KW_RETURN: return "KW_RETURN";
    case TK_KW_THROW: return "KW_THROW";
    case TK_KW_TRUE: return "KW_TRUE";
    case TK_KW_TRY: return "KW_TRY";
    case TK_KW_TYPE: return "KW_TYPE";
    case TK_LIT_CHAR: return "LIT_CHAR";
    case TK_LIT_FLOAT: return "LIT_FLOAT";
    case TK_LIT_INT_BIN: return "LIT_INT_BIN";
    case TK_LIT_INT_DEC: return "LIT_INT_DEC";
    case TK_LIT_INT_HEX: return "LIT_INT_HEX";
    case TK_LIT_INT_OCT: return "LIT_INT_OCT";
    case TK_LIT_STR: return "LIT_STR";
    case TK_OP_AMP: return "OP_AMP";
    case TK_OP_AMP_EQ: return "OP_AMP_EQ";
    case TK_OP_AMP_AMP: return "OP_AMP_AMP";
    case TK_OP_AMP_AMP_EQ: return "OP_AMP_AMP_EQ";
    case TK_OP_ARROW: return "OP_ARROW";
    case TK_OP_AT: return "OP_AT";
    case TK_OP_BACKSLASH: return "OP_BACKSLASH";
    case TK_OP_BACKTICK: return "OP_BACKTICK";
    case TK_OP_CARET: return "OP_CARET";
    case TK_OP_CARET_EQ: return "OP_CARET_EQ";
    case TK_OP_COLON: return "OP_COLON";
    case TK_OP_COLON_EQ: return "OP_COLON_EQ";
    case TK_OP_COMMA: return "OP_COMMA";
    case TK_OP_DOLLAR: return "OP_DOLLAR";
    case TK_OP_DOT: return "OP_DOT";
    case TK_OP_DOT_DOT_DOT: return "OP_DOT_DOT_DOT";
    case TK_OP_EQ: return "OP_EQ";
    case TK_OP_EQ_EQ: return "OP_EQ_EQ";
    case TK_OP_EXCL: return "OP_EXCL";
    case TK_OP_EXCL_EQ: return "OP_EXCL_EQ";
    case TK_OP_GT: return "OP_GT";
    case TK_OP_GT_EQ: return "OP_GT_EQ";
    case TK_OP_HASH: return "OP_HASH";
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
    case TK_OP_PERCENT: return "OP_PERCENT";
    case TK_OP_PERCENT_EQ: return "OP_PERCENT_EQ";
    case TK_OP_PIPE: return "OP_PIPE";
    case TK_OP_PIPE_EQ: return "OP_PIPE_EQ";
    case TK_OP_PIPE_PIPE: return "OP_PIPE_PIPE";
    case TK_OP_PIPE_PIPE_EQ: return "OP_PIPE_PIPE_EQ";
    case TK_OP_PLUS: return "OP_PLUS";
    case TK_OP_PLUS_EQ: return "OP_PLUS_EQ";
    case TK_OP_PLUS_PLUS: return "OP_PLUS_PLUS";
    case TK_OP_QN: return "OP_QN";
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
    case TK_OP_TILDE: return "OP_TILDE";
  }

  throw Error("tried stringify unknown token");
}

std::string Token::escape (const std::string &val, bool insideAttr) {
  auto result = std::string();
  result.reserve(val.size());

  for (auto idx = static_cast<std::size_t>(0); idx < val.size(); idx++) {
    if (val[idx] == '\f') result += R"(\f)";
    else if (val[idx] == '\n') result += R"(\n)";
    else if (val[idx] == '\r') result += R"(\r)";
    else if (val[idx] == '\t') result += R"(\t)";
    else if (val[idx] == '\v') result += R"(\v)";
    else if (val[idx] == '"' && insideAttr) result += R"(\")";
    else result += val[idx];
  }

  return result;
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
  return std::string("0fnrtv\"'\\").find(ch) != std::string::npos;
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
  return Token::isLitCharEscape(ch);
}

bool Token::isWhitespace (char ch) {
  return std::isspace(ch);
}

std::string Token::upper (const std::string &str) {
  if (str.empty()) {
    return "";
  }

  auto result = std::string();
  result.reserve(str.size());

  for (const auto &ch : str) {
    result.push_back(static_cast<char>(toupper(ch)));
  }

  return result;
}

std::string Token::upperFirst (const std::string &str) {
  if (str.empty()) {
    return "";
  }

  return std::string(1, static_cast<char>(toupper(str[0]))) + str.substr(1);
}

TokenAssociativity Token::associativity (bool unary) const {
  if (
    this->type == TK_KW_AWAIT || this->type == TK_KW_IS || this->type == TK_KW_REF ||
    this->type == TK_OP_LBRACE || this->type == TK_OP_RBRACE ||
    this->type == TK_OP_LPAR || this->type == TK_OP_RPAR ||
    this->type == TK_OP_MINUS_MINUS || this->type == TK_OP_PLUS_PLUS ||
    this->type == TK_OP_ARROW
  ) {
    return TK_ASSOC_NONE;
  } else if (
    (unary && (this->type == TK_OP_EXCL || this->type == TK_OP_MINUS || this->type == TK_OP_PLUS || this->type == TK_OP_TILDE)) ||
    this->type == TK_OP_AMP_EQ ||
    this->type == TK_OP_AMP_AMP_EQ ||
    this->type == TK_OP_CARET_EQ ||
    this->type == TK_OP_EQ ||
    this->type == TK_OP_LSHIFT_EQ ||
    this->type == TK_OP_MINUS_EQ ||
    this->type == TK_OP_PERCENT_EQ ||
    this->type == TK_OP_PIPE_EQ ||
    this->type == TK_OP_PIPE_PIPE_EQ ||
    this->type == TK_OP_PLUS_EQ ||
    this->type == TK_OP_RSHIFT_EQ ||
    this->type == TK_OP_SLASH_EQ ||
    this->type == TK_OP_STAR_EQ ||
    this->type == TK_OP_COLON || this->type == TK_OP_QN
  ) {
    return TK_ASSOC_RIGHT;
  } else if (
    this->type == TK_OP_DOT || this->type == TK_OP_LBRACK || this->type == TK_OP_RBRACK ||
    this->type == TK_OP_PERCENT || this->type == TK_OP_SLASH || this->type == TK_OP_STAR ||
    this->type == TK_OP_MINUS || this->type == TK_OP_PLUS ||
    this->type == TK_OP_LSHIFT || this->type == TK_OP_RSHIFT ||
    this->type == TK_OP_GT || this->type == TK_OP_GT_EQ || this->type == TK_OP_LT || this->type == TK_OP_LT_EQ ||
    this->type == TK_OP_EQ_EQ || this->type == TK_OP_EXCL_EQ ||
    this->type == TK_OP_AMP || this->type == TK_OP_CARET || this->type == TK_OP_PIPE ||
    this->type == TK_OP_AMP_AMP || this->type == TK_OP_PIPE_PIPE ||
    this->type == TK_OP_COMMA
  ) {
    return TK_ASSOC_LEFT;
  }

  throw Error("tried associativity for unknown token");
}

int Token::precedence (bool isUnary) const {
  if (this->type == TK_OP_LBRACE || this->type == TK_OP_RBRACE || this->type == TK_OP_LPAR || this->type == TK_OP_RPAR) {
    return 18;
  } else if (this->type == TK_OP_DOT || this->type == TK_OP_LBRACK || this->type == TK_OP_RBRACK) {
    return 17;
  } else if (this->type == TK_OP_MINUS_MINUS || this->type == TK_OP_PLUS_PLUS) {
    return 16;
  } else if (this->type == TK_KW_AWAIT || this->type == TK_KW_REF) {
    return 15;
  } else if (isUnary && (this->type == TK_OP_EXCL || this->type == TK_OP_MINUS || this->type == TK_OP_PLUS || this->type == TK_OP_TILDE)) {
    return 14;
  } else if (this->type == TK_OP_PERCENT || this->type == TK_OP_SLASH || this->type == TK_OP_STAR) {
    return 12;
  } else if (this->type == TK_OP_MINUS || this->type == TK_OP_PLUS) {
    return 11;
  } else if (this->type == TK_OP_LSHIFT || this->type == TK_OP_RSHIFT) {
    return 10;
  } else if (this->type == TK_OP_GT || this->type == TK_OP_GT_EQ || this->type == TK_OP_LT || this->type == TK_OP_LT_EQ || this->type == TK_KW_IS) {
    return 9;
  } else if (this->type == TK_OP_EQ_EQ || this->type == TK_OP_EXCL_EQ) {
    return 8;
  } else if (this->type == TK_OP_AMP) {
    return 7;
  } else if (this->type == TK_OP_CARET) {
    return 6;
  } else if (this->type == TK_OP_PIPE) {
    return 5;
  } else if (this->type == TK_OP_AMP_AMP) {
    return 4;
  } else if (this->type == TK_OP_PIPE_PIPE) {
    return 3;
  } else if (
    this->type == TK_OP_AMP_EQ ||
    this->type == TK_OP_AMP_AMP_EQ ||
    this->type == TK_OP_CARET_EQ ||
    this->type == TK_OP_EQ ||
    this->type == TK_OP_LSHIFT_EQ ||
    this->type == TK_OP_MINUS_EQ ||
    this->type == TK_OP_PERCENT_EQ ||
    this->type == TK_OP_PIPE_EQ ||
    this->type == TK_OP_PIPE_PIPE_EQ ||
    this->type == TK_OP_PLUS_EQ ||
    this->type == TK_OP_RSHIFT_EQ ||
    this->type == TK_OP_SLASH_EQ ||
    this->type == TK_OP_STAR_EQ ||
    this->type == TK_OP_COLON || this->type == TK_OP_QN ||
    this->type == TK_OP_ARROW
  ) {
    return 2;
  } else if (this->type == TK_OP_COMMA) {
    return 1;
  }

  throw Error("tried precedence for unknown token");
}

std::string Token::str (std::size_t indent) const {
  auto escVal = Token::escape(this->val);
  auto result = std::string(indent, ' ');

  result += tokenTypeToStr(this->type) + "(" + this->start.str() + "-" + this->end.str() + ")";
  result += escVal.empty() ? "" : ": " + escVal;

  return result;
}

std::string Token::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ');

  result += R"(<Token type=")" + tokenTypeToStr(this->type) + R"(")";
  result += R"( val=")" + Token::escape(this->val, true) + R"(")";
  result += R"( start=")" + this->start.str() + R"(")";
  result += R"( end=")" + this->end.str() + R"(")";
  result += " />";

  return result;
}
