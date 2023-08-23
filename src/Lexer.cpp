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

#include "Lexer.hpp"
#include "config.hpp"

Lexer::Lexer (Reader *r) {
  this->reader = r;
}

// NOLINTNEXTLINE(google-default-arguments)
std::tuple<ReaderLocation, Token> Lexer::next (bool keepComments) {
  auto startLoc = this->reader->loc;
  auto tk = this->_whitespace(keepComments);

  if (tk != std::nullopt) {
    return {startLoc, *tk};
  }

  this->loc = this->reader->loc;
  this->val = "";

  if (this->reader->eof()) {
    return {startLoc, this->_tok(TK_EOF)};
  }

  auto [loc0, ch0] = this->reader->next();
  this->loc = loc0;
  this->val = ch0;

  if (ch0 == '&') return {startLoc, this->_opEq2('&', TK_OP_AMP, TK_OP_AMP_EQ, TK_OP_AMP_AMP, TK_OP_AMP_AMP_EQ)};
  else if (ch0 == '@') return {startLoc, this->_tok(TK_OP_AT)};
  else if (ch0 == '`') return {startLoc, this->_tok(TK_OP_BACKTICK)};
  else if (ch0 == '\\') return {startLoc, this->_tok(TK_OP_BACKSLASH)};
  else if (ch0 == '^') return {startLoc, this->_opEq(TK_OP_CARET, TK_OP_CARET_EQ)};
  else if (ch0 == ':') return {startLoc, this->_opEq(TK_OP_COLON, TK_OP_COLON_EQ)};
  else if (ch0 == ',') return {startLoc, this->_tok(TK_OP_COMMA)};
  else if (ch0 == '$') return {startLoc, this->_tok(TK_OP_DOLLAR)};
  else if (ch0 == '=') return {startLoc, this->_opEq(TK_OP_EQ, TK_OP_EQ_EQ)};
  else if (ch0 == '!') return {startLoc, this->_opEq(TK_OP_EXCL, TK_OP_EXCL_EQ)};
  else if (ch0 == '>') return {startLoc, this->_opEq2('>', TK_OP_GT, TK_OP_GT_EQ, TK_OP_RSHIFT, TK_OP_RSHIFT_EQ)};
  else if (ch0 == '#') return {startLoc, this->_tok(TK_OP_HASH)};
  else if (ch0 == '{') return {startLoc, this->_tok(TK_OP_LBRACE)};
  else if (ch0 == '[') return {startLoc, this->_tok(TK_OP_LBRACK)};
  else if (ch0 == '(') return {startLoc, this->_tok(TK_OP_LPAR)};
  else if (ch0 == '<') return {startLoc, this->_opEq2('<', TK_OP_LT, TK_OP_LT_EQ, TK_OP_LSHIFT, TK_OP_LSHIFT_EQ)};
  else if (ch0 == '|') return {startLoc, this->_opEq2('|', TK_OP_PIPE, TK_OP_PIPE_EQ, TK_OP_PIPE_PIPE, TK_OP_PIPE_PIPE_EQ)};
  else if (ch0 == '%') return {startLoc, this->_opEq(TK_OP_PERCENT, TK_OP_PERCENT_EQ)};
  else if (ch0 == '+') return {startLoc, this->_opEq2('+', TK_OP_PLUS, TK_OP_PLUS_EQ, TK_OP_PLUS_PLUS)};
  else if (ch0 == '?') return {startLoc, this->_tok(TK_OP_QN)};
  else if (ch0 == '}') return {startLoc, this->_tok(TK_OP_RBRACE)};
  else if (ch0 == ']') return {startLoc, this->_tok(TK_OP_RBRACK)};
  else if (ch0 == ')') return {startLoc, this->_tok(TK_OP_RPAR)};
  else if (ch0 == ';') return {startLoc, this->_tok(TK_OP_SEMI)};
  else if (ch0 == '/') return {startLoc, this->_opEq(TK_OP_SLASH, TK_OP_SLASH_EQ)};
  else if (ch0 == '*') return {startLoc, this->_opEq(TK_OP_STAR, TK_OP_STAR_EQ)};
  else if (ch0 == '~') return {startLoc, this->_tok(TK_OP_TILDE)};

  if (ch0 == '-') {
    if (this->reader->eof()) {
      return {startLoc, this->_tok(TK_OP_MINUS)};
    }

    auto [loc1, ch1] = this->reader->next();

    if (ch1 == '>') {
      this->val += ch1;
      return {startLoc, this->_tok(TK_OP_ARROW)};
    } else if (ch1 == '=') {
      this->val += ch1;
      return {startLoc, this->_tok(TK_OP_MINUS_EQ)};
    } else if (ch1 == '-') {
      this->val += ch1;
      return {startLoc, this->_tok(TK_OP_MINUS_MINUS)};
    } else {
      this->reader->seek(loc1);
      return {startLoc, this->_tok(TK_OP_MINUS)};
    }
  }

  if (ch0 == '.') {
    if (this->reader->eof()) {
      return {startLoc, this->_tok(TK_OP_DOT)};
    }

    auto [loc1, ch1] = this->reader->next();

    if (ch1 == '.' && !this->reader->eof()) {
      auto [_2, ch2] = this->reader->next();

      if (ch2 == '.') {
        this->val += ch1;
        this->val += ch2;

        return {startLoc, this->_tok(TK_OP_DOT_DOT_DOT)};
      }
    }

    this->reader->seek(loc1);
    return {startLoc, this->_tok(TK_OP_DOT)};
  }

  if (Token::isIdStart(ch0)) {
    this->_walk(Token::isIdContinue);

    if (this->val == "as") return {startLoc, this->_tok(TK_KW_AS)};
    else if (this->val == "async") return {startLoc, this->_tok(TK_KW_ASYNC)};
    else if (this->val == "await") return {startLoc, this->_tok(TK_KW_AWAIT)};
    else if (this->val == "break") return {startLoc, this->_tok(TK_KW_BREAK)};
    else if (this->val == "catch") return {startLoc, this->_tok(TK_KW_CATCH)};
    else if (this->val == "const") return {startLoc, this->_tok(TK_KW_CONST)};
    else if (this->val == "continue") return {startLoc, this->_tok(TK_KW_CONTINUE)};
    else if (this->val == "elif") return {startLoc, this->_tok(TK_KW_ELIF)};
    else if (this->val == "else") return {startLoc, this->_tok(TK_KW_ELSE)};
    else if (this->val == "enum") return {startLoc, this->_tok(TK_KW_ENUM)};
    else if (this->val == "export") return {startLoc, this->_tok(TK_KW_EXPORT)};
    else if (this->val == "false") return {startLoc, this->_tok(TK_KW_FALSE)};
    else if (this->val == "finally") return {startLoc, this->_tok(TK_KW_FINALLY)};
    else if (this->val == "fn") return {startLoc, this->_tok(TK_KW_FN)};
    else if (this->val == "from") return {startLoc, this->_tok(TK_KW_FROM)};
    else if (this->val == "if") return {startLoc, this->_tok(TK_KW_IF)};
    else if (this->val == "import") return {startLoc, this->_tok(TK_KW_IMPORT)};
    else if (this->val == "is") return {startLoc, this->_tok(TK_KW_IS)};
    else if (this->val == "loop") return {startLoc, this->_tok(TK_KW_LOOP)};
    else if (this->val == "main") return {startLoc, this->_tok(TK_KW_MAIN)};
    else if (this->val == "mut") return {startLoc, this->_tok(TK_KW_MUT)};
    else if (this->val == "nil") return {startLoc, this->_tok(TK_KW_NIL)};
    else if (this->val == "obj") return {startLoc, this->_tok(TK_KW_OBJ)};
    else if (this->val == "ref") return {startLoc, this->_tok(TK_KW_REF)};
    else if (this->val == "return") return {startLoc, this->_tok(TK_KW_RETURN)};
    else if (this->val == "throw") return {startLoc, this->_tok(TK_KW_THROW)};
    else if (this->val == "true") return {startLoc, this->_tok(TK_KW_TRUE)};
    else if (this->val == "try") return {startLoc, this->_tok(TK_KW_TRY)};
    else if (this->val == "type") return {startLoc, this->_tok(TK_KW_TYPE)};

    return {startLoc, this->_tok(TK_ID)};
  }

  if (Token::isDigit(ch0)) {
    if (ch0 == '0') {
      if (this->reader->eof()) {
        return {startLoc, this->_tok(TK_LIT_INT_DEC)};
      }

      auto [loc1, ch1] = this->reader->next();

      if (Token::isDigit(ch1)) {
        this->_walk(Token::isIdContinue);
        throw Error(this->reader, this->loc, E0007);
      }

      if (ch1 == 'B' || ch1 == 'b' || ch1 == 'X' || ch1 == 'x' || ch1 == 'O' || ch1 == 'o') {
        this->val += ch1;

        if (ch1 == 'B' || ch1 == 'b') {
          return {startLoc, this->_litNum(Token::isLitIntBin, TK_LIT_INT_BIN)};
        } else if (ch1 == 'X' || ch1 == 'x') {
          return {startLoc, this->_litNum(Token::isLitIntHex, TK_LIT_INT_HEX)};
        } else {
          return {startLoc, this->_litNum(Token::isLitIntOct, TK_LIT_INT_OCT)};
        }
      }

      this->reader->seek(loc1);
      return {startLoc, this->_litNum(Token::isLitIntDec, TK_LIT_INT_DEC)};
    }

    this->_walk(Token::isLitIntDec);
    return {startLoc, this->_litNum(Token::isLitIntDec, TK_LIT_INT_DEC)};
  }

  if (ch0 == '"') {
    while (true) {
      if (this->reader->eof()) {
        throw Error(this->reader, this->loc, E0003);
      }

      auto [loc1, ch1] = this->reader->next();
      this->val += (ch1 == '\n') ? EOL : std::string(1, ch1);

      if (ch1 == '\\') {
        if (this->reader->eof()) {
          throw Error(this->reader, this->loc, E0003);
        }

        auto [_2, ch2] = this->reader->next();
        this->val += ch2;

        if (!Token::isLitStrEscape(ch2)) {
          throw Error(this->reader, loc1, E0005);
        }
      } else if (ch1 == '"') {
        break;
      }
    }

    return {startLoc, this->_tok(TK_LIT_STR)};
  }

  if (ch0 == '\'') {
    if (this->reader->eof()) {
      throw Error(this->reader, this->loc, E0002);
    }

    auto [loc1, ch1] = this->reader->next();
    this->val += ch1;

    if (ch1 == '\n') {
      throw Error(this->reader, this->loc, E0017);
    } else if (ch1 == '\'') {
      throw Error(this->reader, this->loc, E0004);
    } else if (this->reader->eof()) {
      throw Error(this->reader, this->loc, E0002);
    } else if (ch1 == '\\') {
      auto [_2, ch2] = this->reader->next();
      this->val += ch2;

      if (!Token::isLitCharEscape(ch2)) {
        throw Error(this->reader, loc1, E0005);
      } else if (this->reader->eof()) {
        throw Error(this->reader, this->loc, E0002);
      }
    }

    auto [_3, ch3] = this->reader->next();
    this->val += ch3;

    if (ch3 != '\'') {
      while (!this->reader->eof()) {
        auto [_4, ch4] = this->reader->next();

        if (ch4 == '\'') {
          break;
        }
      }

      throw Error(this->reader, this->loc, E0006);
    }

    return {startLoc, this->_tok(TK_LIT_CHAR)};
  }

  return {startLoc, this->_tok(TK_UNKNOWN)};
}

void Lexer::seek (ReaderLocation l) {
  this->loc = l;
  this->reader->seek(l);
}

Token Lexer::_litFloat (TokenType type) {
  if (!this->reader->eof()) {
    auto [loc1, ch1] = this->reader->next();

    if (Token::isIdContinue(ch1)) {
      this->_walk(Token::isIdContinue);
      throw Error(this->reader, this->loc, E0012);
    }

    this->reader->seek(loc1);
  }

  if (type == TK_LIT_INT_BIN) {
    throw Error(this->reader, this->loc, E0014);
  } else if (type == TK_LIT_INT_HEX) {
    throw Error(this->reader, this->loc, E0015);
  } else if (type == TK_LIT_INT_OCT) {
    throw Error(this->reader, this->loc, E0016);
  }

  return this->_tok(TK_LIT_FLOAT);
}

Token Lexer::_litNum (const std::function<bool (char)> &fn, TokenType type) {
  auto errCode = std::string();

  if (type == TK_LIT_INT_BIN) {
    errCode = E0008;
  } else if (type == TK_LIT_INT_DEC) {
    errCode = E0009;
  } else if (type == TK_LIT_INT_HEX) {
    errCode = E0010;
  } else {
    errCode = E0011;
  }

  if (type != TK_LIT_INT_DEC) {
    if (this->reader->eof()) {
      throw Error(this->reader, this->loc, errCode);
    }

    auto [_1, ch1] = this->reader->next();
    this->val += ch1;

    if (!fn(ch1)) {
      this->_walk(Token::isIdContinue);
      throw Error(this->reader, this->loc, errCode);
    }

    this->_walk(fn);
  }

  if (this->reader->eof()) {
    return this->_tok(type);
  }

  auto [loc2, ch2] = this->reader->next();

  if (Token::isIdContinue(ch2) && ch2 != 'E' && ch2 != 'e') {
    this->_walk(Token::isIdContinue);
    throw Error(this->reader, this->loc, errCode);
  }

  if (ch2 == 'E' || ch2 == 'e') {
    this->val += ch2;
    this->_walkLitFloatExp(loc2);

    return this->_litFloat(type);
  } else if (ch2 == '.') {
    if (this->reader->eof()) {
      throw Error(this->reader, this->loc, E0012);
    }

    auto [loc3, ch3] = this->reader->next();

    if (Token::isIdContinue(ch3) && !Token::isDigit(ch3) && ch3 != 'E' && ch3 != 'e') {
      this->_walk(Token::isIdContinue);
      throw Error(this->reader, this->loc, E0012);
    }

    if (ch3 == 'E' || ch3 == 'e') {
      this->val += ch2;
      this->val += ch3;

      this->_walkLitFloatExp(loc3);
      return this->_litFloat(type);
    } else if (ch3 == '.') {
      this->reader->seek(loc2);
      return this->_tok(type);
    }

    this->val += ch2;
    this->val += ch3;

    while (!this->reader->eof()) {
      auto [loc4, ch4] = this->reader->next();

      if (!Token::isDigit(ch4) && ch4 != 'E' && ch4 != 'e') {
        this->reader->seek(loc4);
        break;
      }

      this->val += ch4;

      if (ch4 == 'E' || ch4 == 'e') {
        this->_walkLitFloatExp(loc4);
        break;
      }
    }

    return this->_litFloat(type);
  }

  this->reader->seek(loc2);
  return this->_tok(type);
}

Token Lexer::_opEq (TokenType type1, TokenType type2) {
  if (this->reader->eof()) {
    return this->_tok(type1);
  }

  auto [loc1, ch1] = this->reader->next();

  if (ch1 == '=') {
    this->val += ch1;
    return this->_tok(type2);
  }

  this->reader->seek(loc1);
  return this->_tok(type1);
}

Token Lexer::_opEq2 (char ch, TokenType type1, TokenType type2, TokenType type3, const std::optional<TokenType> &type4) {
  if (this->reader->eof()) {
    return this->_tok(type1);
  }

  auto [loc1, ch1] = this->reader->next();

  if (ch1 != '=' && ch1 != ch) {
    this->reader->seek(loc1);
    return this->_tok(type1);
  } else if (ch1 == '=') {
    this->val += ch1;
    return this->_tok(type2);
  } else if (this->reader->eof() || type4 == std::nullopt) {
    this->val += ch1;
    return this->_tok(type3);
  }

  auto [loc2, ch2] = this->reader->next();

  if (ch2 == '=') {
    this->val += ch1;
    this->val += ch2;

    return this->_tok(*type4);
  } else {
    this->val += ch1;
    this->reader->seek(loc2);

    return this->_tok(type3);
  }
}

Token Lexer::_tok (TokenType type) {
  auto tok = Token{type, this->val, this->loc, this->reader->loc};
  this->loc = this->reader->loc;

  return tok;
}

void Lexer::_walk (const std::function<bool (char)> &fn) {
  while (!this->reader->eof()) {
    auto [loc1, ch1] = this->reader->next();

    if (!fn(ch1)) {
      this->reader->seek(loc1);
      break;
    }

    this->val += ch1;
  }
}

void Lexer::_walkLitFloatExp (ReaderLocation loc0) {
  if (this->reader->eof()) {
    throw Error(this->reader, loc0, E0013);
  }

  auto [loc1, ch1] = this->reader->next();

  if (!Token::isDigit(ch1) && ch1 != '+' && ch1 != '-') {
    if (Token::isIdContinue(ch1)) {
      this->_walk(Token::isIdContinue);
    } else {
      this->reader->seek(loc1);
    }

    throw Error(this->reader, loc0, E0013);
  }

  this->val += ch1;

  if (ch1 == '+' || ch1 == '-') {
    if (this->reader->eof()) {
      throw Error(this->reader, loc0, E0013);
    }

    auto [loc2, ch2] = this->reader->next();

    if (!Token::isDigit(ch2)) {
      if (Token::isIdContinue(ch2)) {
        this->_walk(Token::isIdContinue);
      } else {
        this->reader->seek(loc2);
      }

      throw Error(this->reader, loc0, E0013);
    }

    this->val += ch2;
  }

  while (!this->reader->eof()) {
    auto [loc3, ch3] = this->reader->next();

    if (!Token::isDigit(ch3)) {
      this->reader->seek(loc3);
      break;
    }

    this->val += ch3;
  }
}

std::optional<Token> Lexer::_whitespace (bool keepComments) {
  if (this->reader->eof()) {
    return std::nullopt;
  }

  auto [loc1, ch1] = this->reader->next();

  if (Token::isWhitespace(ch1)) {
    this->_walk(Token::isWhitespace);
    return this->_whitespace(keepComments);
  } else if (ch1 == '/' && !this->reader->eof()) {
    auto [_2, ch2] = this->reader->next();

    if (ch2 == '/') {
      while (!this->reader->eof()) {
        auto [loc3, ch3] = this->reader->next();

        if (ch3 == '\n') {
          this->reader->seek(loc3);
          break;
        }
      }

      if (!keepComments) {
        return this->_whitespace();
      }

      this->loc = loc1;
      this->val = this->reader->content.substr(this->loc.pos, this->reader->loc.pos - this->loc.pos);
      return this->_tok(TK_COMMENT_LINE);
    } else if (ch2 == '*') {
      if (this->reader->eof()) {
        throw Error(this->reader, this->loc, E0001);
      }

      while (true) {
        auto [_3, ch3] = this->reader->next();

        if (this->reader->eof()) {
          throw Error(this->reader, this->loc, E0001);
        }

        if (ch3 == '*') {
          auto [loc4, ch4] = this->reader->next();

          if (ch4 == '/') {
            break;
          }

          this->reader->seek(loc4);
        }
      }

      if (!keepComments) {
        return this->_whitespace();
      }

      this->loc = loc1;
      this->val = this->reader->content.substr(this->loc.pos, this->reader->loc.pos - this->loc.pos);
      return this->_tok(TK_COMMENT_BLOCK);
    }
  }

  this->reader->seek(loc1);
  return std::nullopt;
}
