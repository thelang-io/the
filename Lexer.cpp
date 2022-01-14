/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <functional>
#include "Error.hpp"
#include "Lexer.hpp"

void lexWalk (Reader *reader, Lexer &lexer, const std::function<bool (char)> &fn) {
  while (!reader->eof()) {
    auto loc = reader->loc;
    auto ch = reader->next();

    if (!fn(ch)) {
      reader->seek(loc);
      break;
    }

    lexer.val += ch;
  }
}

void lexWhitespace (Reader *reader) {
  while (true) {
    if (reader->eof()) {
      break;
    }

    auto lexer = Lexer{reader->loc};
    auto ch = reader->next();

    if (tokenIsWhitespace(ch)) {
      lexWalk(reader, lexer, tokenIsWhitespace);
      continue;
    } else if (ch == '/') {
      auto ch1 = reader->next();

      if (ch1 == '/') {
        lexer.val += ch1;
        lexWalk(reader, lexer, tokenIsNotNewline);

        continue;
      } else if (ch1 == '*') {
        lexer.val += ch1;

        if (reader->eof()) {
          throw SyntaxError(reader, lexer.start, E0001);
        }

        while (true) {
          auto ch2 = reader->next();

          if (reader->eof()) {
            throw SyntaxError(reader, lexer.start, E0001);
          }

          if (ch2 == '*') {
            auto loc3 = reader->loc;
            auto ch3 = reader->next();

            if (ch3 == '/') {
              lexer.val += ch2;
              lexer.val += ch3;

              break;
            } else {
              reader->seek(loc3);
            }
          }

          lexer.val += ch2;
        }

        continue;
      }
    }

    reader->seek(lexer.start);
    break;
  }
}

void lexWalkLitFloatExp (Reader *reader, Lexer &lexer) {
  if (reader->eof()) {
    throw SyntaxError(reader, lexer.start, E0013);
  }

  auto ch1 = reader->next();

  if (ch1 == '+' || ch1 == '-') {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0013);
    }

    auto ch2 = reader->next();

    if (!tokenIsDigit(ch2)) {
      lexWalk(reader, lexer, tokenIsIdContinue);
      throw SyntaxError(reader, lexer.start, E0013);
    }

    lexer.val += ch1;
    lexer.val += ch2;
  } else if (tokenIsDigit(ch1)) {
    lexer.val += ch1;
  } else {
    lexWalk(reader, lexer, tokenIsIdContinue);
    throw SyntaxError(reader, lexer.start, E0013);
  }

  while (!reader->eof()) {
    auto loc3 = reader->loc;
    auto ch3 = reader->next();

    if (!tokenIsDigit(ch3)) {
      reader->seek(loc3);
      break;
    }

    lexer.val += ch3;
  }
}

Token lexLitFloat (Reader *reader, Lexer &lexer, TokenType type) {
  if (!reader->eof()) {
    auto loc = reader->loc;
    auto ch = reader->next();

    if (tokenIsIdContinue(ch)) {
      lexWalk(reader, lexer, tokenIsIdContinue);
      throw SyntaxError(reader, lexer.start, E0012);
    } else {
      reader->seek(loc);
    }
  }

  if (type == TK_LIT_INT_BIN) {
    throw SyntaxError(reader, lexer.start, E0014);
  } else if (type == TK_LIT_INT_HEX) {
    throw SyntaxError(reader, lexer.start, E0015);
  } else if (type == TK_LIT_INT_OCT) {
    throw SyntaxError(reader, lexer.start, E0016);
  }

  return Token{TK_LIT_FLOAT, lexer.start, reader->loc, lexer.val};
}

Token lexLitNum (Reader *reader, Lexer &lexer, const std::function<bool (char)> &fn, TokenType type) {
  auto errCode = type == TK_LIT_INT_BIN
    ? E0008
    : type == TK_LIT_INT_DEC
      ? E0009
      : type == TK_LIT_INT_HEX
        ? E0010
        : E0011;

  if (type != TK_LIT_INT_DEC) {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, errCode);
    }

    auto ch1 = reader->next();

    if (!fn(ch1)) {
      lexWalk(reader, lexer, tokenIsIdContinue);
      throw SyntaxError(reader, lexer.start, errCode);
    }

    lexer.val += ch1;
    lexWalk(reader, lexer, fn);
  }

  if (reader->eof()) {
    return Token{type, lexer.start, reader->loc, lexer.val};
  }

  auto loc2 = reader->loc;
  auto ch2 = reader->next();

  if (ch2 == 'E' || ch2 == 'e') {
    lexer.val += ch2;
    lexWalkLitFloatExp(reader, lexer);

    return lexLitFloat(reader, lexer, type);
  } else if (ch2 == '.') {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0012);
    }

    auto ch3 = reader->next();

    if (ch3 == '.') {
      reader->seek(loc2);
    } else if (ch3 == 'E' || ch3 == 'e') {
      lexer.val += ch2;
      lexer.val += ch3;
      lexWalkLitFloatExp(reader, lexer);

      return lexLitFloat(reader, lexer, type);
    } else if (!tokenIsIdContinue(ch3)) {
      if (tokenIsIdContinue(ch3)) {
        lexWalk(reader, lexer, tokenIsIdContinue);
      }

      throw SyntaxError(reader, lexer.start, E0012);
    } else {
      auto withExp = false;

      lexer.val += ch2;
      lexer.val += ch3;

      while (!reader->eof()) {
        auto loc4 = reader->loc;
        auto ch4 = reader->next();

        if (ch4 == 'E' || ch4 == 'e') {
          withExp = true;
        } else if (!tokenIsDigit(ch4)) {
          reader->seek(loc4);
          break;
        }

        lexer.val += ch4;

        if (withExp) {
          break;
        }
      }

      if (withExp) {
        lexWalkLitFloatExp(reader, lexer);
      }

      return lexLitFloat(reader, lexer, type);
    }
  } else if (tokenIsIdContinue(ch2)) {
    lexWalk(reader, lexer, tokenIsIdContinue);
    throw SyntaxError(reader, lexer.start, errCode);
  } else {
    reader->seek(loc2);
  }

  return Token{type, lexer.start, reader->loc, lexer.val};
}

Token lexOpEq (Reader *reader, Lexer &lexer, TokenType type1, TokenType type2) {
  if (reader->eof()) {
    return Token{type1, lexer.start, reader->loc, lexer.val};
  }

  auto loc = reader->loc;
  auto ch = reader->next();

  if (ch == '=') {
    lexer.val += ch;
    return Token{type2, lexer.start, reader->loc, lexer.val};
  } else {
    reader->seek(loc);
    return Token{type1, lexer.start, reader->loc, lexer.val};
  }
}

Token lexOpEq2 (Reader *reader, Lexer &lexer, char ch, TokenType type1, TokenType type2, TokenType type3, TokenType type4) {
  if (reader->eof()) {
    return Token{type1, lexer.start, reader->loc, lexer.val};
  }

  auto loc1 = reader->loc;
  auto ch1 = reader->next();

  if (ch1 == '=' || reader->eof()) {
    lexer.val += ch1;
    return Token{ch1 == '=' ? type2 : type3, lexer.start, reader->loc, lexer.val};
  } else if (ch1 == ch) {
    auto loc2 = reader->loc;
    auto ch2 = reader->next();

    if (ch2 == '=') {
      lexer.val += ch1;
      lexer.val += ch2;

      return Token{type4, lexer.start, reader->loc, lexer.val};
    } else {
      lexer.val += ch1;
      reader->seek(loc2);

      return Token{type3, lexer.start, reader->loc, lexer.val};
    }
  } else {
    reader->seek(loc1);
    return Token{type1, lexer.start, reader->loc, lexer.val};
  }
}

Token lexOpEqDouble (Reader *reader, Lexer &lexer, char ch, TokenType type1, TokenType type2, TokenType type3) {
  if (reader->eof()) {
    return Token{type1, lexer.start, reader->loc, lexer.val};
  }

  auto loc2 = reader->loc;
  auto ch1 = reader->next();

  if (ch1 == '=' || ch1 == ch) {
    lexer.val += ch1;
    return Token{ch1 == '=' ? type2 : type3, lexer.start, reader->loc, lexer.val};
  } else {
    reader->seek(loc2);
    return Token{type1, lexer.start, reader->loc, lexer.val};
  }
}

Token lex (Reader *reader) {
  lexWhitespace(reader);

  auto lexer = Lexer{reader->loc};

  if (reader->eof()) {
    return Token{TK_EOF, lexer.start, reader->loc, lexer.val};
  }

  auto ch = reader->next();
  lexer.val += ch;

  if (ch == '&') return lexOpEq2(reader, lexer, '&', TK_OP_AND, TK_OP_AND_EQ, TK_OP_AND_AND, TK_OP_AND_AND_EQ);
  if (ch == '^') return lexOpEq(reader, lexer, TK_OP_CARET, TK_OP_CARET_EQ);
  if (ch == ':') return lexOpEq(reader, lexer, TK_OP_COLON, TK_OP_COLON_EQ);
  if (ch == ',') return Token{TK_OP_COMMA, lexer.start, reader->loc, lexer.val};
  if (ch == '=') return lexOpEq(reader, lexer, TK_OP_EQ, TK_OP_EQ_EQ);
  if (ch == '!') return lexOpEqDouble(reader, lexer, '!', TK_OP_EXCL, TK_OP_EXCL_EQ, TK_OP_EXCL_EXCL);
  if (ch == '>') return lexOpEq2(reader, lexer, '>', TK_OP_GT, TK_OP_GT_EQ, TK_OP_RSHIFT, TK_OP_RSHIFT_EQ);
  if (ch == '{') return Token{TK_OP_LBRACE, lexer.start, reader->loc, lexer.val};
  if (ch == '[') return Token{TK_OP_LBRACK, lexer.start, reader->loc, lexer.val};
  if (ch == '(') return Token{TK_OP_LPAR, lexer.start, reader->loc, lexer.val};
  if (ch == '<') return lexOpEq2(reader, lexer, '<', TK_OP_LT, TK_OP_LT_EQ, TK_OP_LSHIFT, TK_OP_LSHIFT_EQ);
  if (ch == '-') return lexOpEqDouble(reader, lexer, '-', TK_OP_MINUS, TK_OP_MINUS_EQ, TK_OP_MINUS_MINUS);
  if (ch == '|') return lexOpEq2(reader, lexer, '|', TK_OP_OR, TK_OP_OR_EQ, TK_OP_OR_OR, TK_OP_OR_OR_EQ);
  if (ch == '%') return lexOpEq(reader, lexer, TK_OP_PERCENT, TK_OP_PERCENT_EQ);
  if (ch == '+') return lexOpEqDouble(reader, lexer, '+', TK_OP_PLUS, TK_OP_PLUS_EQ, TK_OP_PLUS_PLUS);
  if (ch == '}') return Token{TK_OP_RBRACE, lexer.start, reader->loc, lexer.val};
  if (ch == ']') return Token{TK_OP_RBRACK, lexer.start, reader->loc, lexer.val};
  if (ch == ')') return Token{TK_OP_RPAR, lexer.start, reader->loc, lexer.val};
  if (ch == ';') return Token{TK_OP_SEMI, lexer.start, reader->loc, lexer.val};
  if (ch == '*') return lexOpEq2(reader, lexer, '*', TK_OP_STAR, TK_OP_STAR_EQ, TK_OP_STAR_STAR, TK_OP_STAR_STAR_EQ);
  if (ch == '~') return Token{TK_OP_TILDE, lexer.start, reader->loc, lexer.val};

  if (ch == '.') {
    if (reader->eof()) {
      return Token{TK_OP_DOT, lexer.start, reader->loc, lexer.val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '.') {
      lexer.val += ch1;

      if (reader->eof()) {
        return Token{TK_OP_DOT_DOT, lexer.start, reader->loc, lexer.val};
      }

      auto loc2 = reader->loc;
      auto ch2 = reader->next();

      if (ch2 == '.' || ch2 == '=') {
        lexer.val += ch2;
        return Token{ch2 == '.' ? TK_OP_DOT_DOT_DOT : TK_OP_DOT_DOT_EQ, lexer.start, reader->loc, lexer.val};
      } else {
        reader->seek(loc2);
        return Token{TK_OP_DOT_DOT, lexer.start, reader->loc, lexer.val};
      }
    } else {
      reader->seek(loc1);
      return Token{TK_OP_DOT, lexer.start, reader->loc, lexer.val};
    }
  } else if (ch == '?') {
    if (reader->eof()) {
      return Token{TK_OP_QN, lexer.start, reader->loc, lexer.val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '.') {
      lexer.val += ch1;
      return Token{TK_OP_QN_DOT, lexer.start, reader->loc, lexer.val};
    } else if (ch1 == '?') {
      lexer.val += ch1;

      if (reader->eof()) {
        return Token{TK_OP_QN_QN, lexer.start, reader->loc, lexer.val};
      }

      auto loc2 = reader->loc;
      auto ch2 = reader->next();

      if (ch2 == '=') {
        lexer.val += ch2;
        return Token{TK_OP_QN_QN_EQ, lexer.start, reader->loc, lexer.val};
      } else {
        reader->seek(loc2);
        return Token{TK_OP_QN_QN, lexer.start, reader->loc, lexer.val};
      }
    } else {
      reader->seek(loc1);
      return Token{TK_OP_QN, lexer.start, reader->loc, lexer.val};
    }
  } else if (ch == '/') {
    if (reader->eof()) {
      return Token{TK_OP_SLASH, lexer.start, reader->loc, lexer.val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '=') {
      lexer.val += ch1;
      return Token{TK_OP_SLASH_EQ, lexer.start, reader->loc, lexer.val};
    } else {
      reader->seek(loc1);
      return Token{TK_OP_SLASH, lexer.start, reader->loc, lexer.val};
    }
  }

  if (tokenIsIdStart(ch)) {
    lexWalk(reader, lexer, tokenIsIdContinue);

    if (lexer.val == "as") return Token{TK_KW_AS, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "async") return Token{TK_KW_ASYNC, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "await") return Token{TK_KW_AWAIT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "break") return Token{TK_KW_BREAK, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "case") return Token{TK_KW_CASE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "catch") return Token{TK_KW_CATCH, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "class") return Token{TK_KW_CLASS, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "const") return Token{TK_KW_CONST, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "continue") return Token{TK_KW_CONTINUE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "default") return Token{TK_KW_DEFAULT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "deinit") return Token{TK_KW_DEINIT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "elif") return Token{TK_KW_ELIF, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "else") return Token{TK_KW_ELSE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "enum") return Token{TK_KW_ENUM, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "export") return Token{TK_KW_EXPORT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "fallthrough") return Token{TK_KW_FALLTHROUGH, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "false") return Token{TK_KW_FALSE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "fn") return Token{TK_KW_FN, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "from") return Token{TK_KW_FROM, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "if") return Token{TK_KW_IF, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "import") return Token{TK_KW_IMPORT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "in") return Token{TK_KW_IN, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "init") return Token{TK_KW_INIT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "interface") return Token{TK_KW_INTERFACE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "is") return Token{TK_KW_IS, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "loop") return Token{TK_KW_LOOP, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "main") return Token{TK_KW_MAIN, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "match") return Token{TK_KW_MATCH, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "mut") return Token{TK_KW_MUT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "new") return Token{TK_KW_NEW, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "nil") return Token{TK_KW_NIL, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "obj") return Token{TK_KW_OBJ, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "op") return Token{TK_KW_OP, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "override") return Token{TK_KW_OVERRIDE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "priv") return Token{TK_KW_PRIV, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "prot") return Token{TK_KW_PROT, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "pub") return Token{TK_KW_PUB, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "return") return Token{TK_KW_RETURN, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "static") return Token{TK_KW_STATIC, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "super") return Token{TK_KW_SUPER, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "this") return Token{TK_KW_THIS, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "throw") return Token{TK_KW_THROW, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "true") return Token{TK_KW_TRUE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "try") return Token{TK_KW_TRY, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "type") return Token{TK_KW_TYPE, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "union") return Token{TK_KW_UNION, lexer.start, reader->loc, lexer.val};

    return Token{TK_ID, lexer.start, reader->loc, lexer.val};
  } else if (tokenIsDigit(ch)) {
    if (ch == '0') {
      if (reader->eof()) {
        return Token{TK_LIT_INT_DEC, lexer.start, reader->loc, lexer.val};
      }

      auto loc1 = reader->loc;
      auto ch1 = reader->next();

      if (tokenIsDigit(ch1)) {
        lexWalk(reader, lexer, tokenIsIdContinue);
        throw SyntaxError(reader, lexer.start, E0007);
      } else if (ch1 == 'B' || ch1 == 'b' || ch1 == 'X' || ch1 == 'x' || ch1 == 'O' || ch1 == 'o') {
        lexer.val += ch1;

        if (ch1 == 'B' || ch1 == 'b') {
          return lexLitNum(reader, lexer, tokenIsLitIntBin, TK_LIT_INT_BIN);
        } else if (ch1 == 'X' || ch1 == 'x') {
          return lexLitNum(reader, lexer, tokenIsLitIntHex, TK_LIT_INT_HEX);
        } else {
          return lexLitNum(reader, lexer, tokenIsLitIntOct, TK_LIT_INT_OCT);
        }
      } else {
        reader->seek(loc1);
      }

      return lexLitNum(reader, lexer, tokenIsLitIntDec, TK_LIT_INT_DEC);
    }

    lexWalk(reader, lexer, tokenIsLitIntDec);
    return lexLitNum(reader, lexer, tokenIsLitIntDec, TK_LIT_INT_DEC);
  } else if (ch == '"') {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0003);
    }

    while (true) {
      auto loc1 = reader->loc;
      auto ch1 = reader->next();

      lexer.val += ch1;

      if (ch1 == '\\') {
        if (reader->eof()) {
          throw SyntaxError(reader, lexer.start, E0003);
        }

        auto ch2 = reader->next();

        if (!tokenIsLitStrEscape(ch2)) {
          throw SyntaxError(reader, loc1, E0005);
        }

        lexer.val += ch2;
      } else if (ch1 == '"') {
        break;
      }
    }

    return Token{TK_LIT_STR, lexer.start, reader->loc, lexer.val};
  } else if (ch == '\'') {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0002);
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '\'') {
      throw SyntaxError(reader, lexer.start, E0004);
    } else if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0002);
    } else if (ch1 == '\\') {
      auto ch2 = reader->next();

      if (!tokenIsLitCharEscape(ch2)) {
        throw SyntaxError(reader, loc1, E0004);
      } else if (reader->eof()) {
        throw SyntaxError(reader, lexer.start, E0002);
      }

      lexer.val += ch1;
      lexer.val += ch2;
    } else {
      lexer.val += ch1;
    }

    auto ch3 = reader->next();

    if (ch3 != '\'') {
      while (!reader->eof()) {
        if (reader->next() == '\'') {
          break;
        }
      }

      throw SyntaxError(reader, lexer.start, E0006);
    }

    lexer.val += ch3;
    return Token{TK_LIT_CHAR, lexer.start, reader->loc, lexer.val};
  }

  throw SyntaxError(reader, lexer.start, E0000);
}
