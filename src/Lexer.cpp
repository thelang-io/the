/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "Lexer.hpp"

void lexWalk (Reader *reader, std::string &val, const std::function<bool (char)> &fn) {
  while (!reader->eof()) {
    auto loc = reader->loc;
    auto ch = reader->next();

    if (!fn(ch)) {
      reader->seek(loc);
      break;
    }

    val += ch;
  }
}

void lexWalkLitFloatExp (Reader *reader, ReaderLocation start, std::string &val) {
  if (reader->eof()) {
    throw SyntaxError(reader, start, E0013);
  }

  auto ch1 = reader->next();

  if (ch1 == '+' || ch1 == '-') {
    if (reader->eof()) {
      throw SyntaxError(reader, start, E0013);
    }

    auto ch2 = reader->next();

    if (!Token::isDigit(ch2)) {
      lexWalk(reader, val, Token::isLitIdContinue);
      throw SyntaxError(reader, start, E0013);
    }

    val += ch1;
    val += ch2;
  } else if (Token::isDigit(ch1)) {
    val += ch1;
  } else {
    lexWalk(reader, val, Token::isLitIdContinue);
    throw SyntaxError(reader, start, E0013);
  }

  while (!reader->eof()) {
    auto loc3 = reader->loc;
    auto ch3 = reader->next();

    if (!Token::isDigit(ch3)) {
      reader->seek(loc3);
      break;
    }

    val += ch3;
  }
}

void lexWalkLitStr (Reader *reader, ReaderLocation start, std::string &val) {
  size_t blocks = 0;
  auto insideChar = false;

  while (true) {
    auto loc1 = reader->loc;
    auto ch1 = reader->next();
    val += ch1;

    if (ch1 == '\\' && blocks == 0) {
      if (reader->eof()) {
        throw SyntaxError(reader, start, E0003);
      }

      auto ch2 = reader->next();

      if (!Token::isLitStrEscape(ch2)) {
        throw SyntaxError(reader, loc1, E0005);
      }

      val += ch2;
    } else if (ch1 == '{' && !insideChar) {
      blocks += 1;
    } else if (ch1 == '}' && blocks != 0 && !insideChar) {
      blocks -= 1;
    } else if (ch1 == '\'' && insideChar) {
      insideChar = false;
    } else if (ch1 == '\'' && blocks != 0) {
      insideChar = true;
    } else if (ch1 == '"' && blocks != 0) {
      lexWalkLitStr(reader, start, val);
    } else if (ch1 == '"') {
      break;
    } else if (reader->eof()) {
      throw SyntaxError(reader, start, E0003);
    }
  }
}

Token lexLitFloat (Reader *reader, ReaderLocation start, std::string &val, TokenType type) {
  if (!reader->eof()) {
    auto loc = reader->loc;
    auto ch = reader->next();

    if (Token::isLitIdContinue(ch)) {
      lexWalk(reader, val, Token::isLitIdContinue);
      throw SyntaxError(reader, start, E0012);
    } else {
      reader->seek(loc);
    }
  }

  if (type == tkLitIntBin) {
    throw SyntaxError(reader, start, E0014);
  } else if (type == tkLitIntHex) {
    throw SyntaxError(reader, start, E0015);
  } else if (type == tkLitIntOct) {
    throw SyntaxError(reader, start, E0016);
  }

  return {tkLitFloat, start, reader->loc, val};
}

Token lexLitNum (
  Reader *reader,
  ReaderLocation start,
  std::string &val,
  const std::function<bool (char)> &fn,
  TokenType type
) {
  auto errCode = type == tkLitIntBin ? E0008 : type == tkLitIntDec ? E0009 : type == tkLitIntHex ? E0010 : E0011;

  if (type != tkLitIntDec) {
    if (reader->eof()) {
      throw SyntaxError(reader, start, errCode);
    }

    auto ch1 = reader->next();

    if (!fn(ch1)) {
      lexWalk(reader, val, Token::isLitIdContinue);
      throw SyntaxError(reader, start, errCode);
    }

    val += ch1;
    lexWalk(reader, val, fn);
  }

  if (reader->eof()) {
    return {type, start, reader->loc, val};
  }

  auto loc2 = reader->loc;
  auto ch2 = reader->next();

  if (ch2 == 'E' || ch2 == 'e') {
    val += ch2;
    lexWalkLitFloatExp(reader, start, val);
    return lexLitFloat(reader, start, val, type);
  } else if (ch2 == '.') {
    if (reader->eof()) {
      throw SyntaxError(reader, start, E0012);
    }

    auto ch3 = reader->next();

    if (ch3 == '.') {
      reader->seek(loc2);
    } else if (ch3 == 'E' || ch3 == 'e') {
      val += ch2;
      val += ch3;
      lexWalkLitFloatExp(reader, start, val);

      return lexLitFloat(reader, start, val, type);
    } else if (!Token::isLitIdContinue(ch3)) {
      if (Token::isLitIdContinue(ch3)) {
        lexWalk(reader, val, Token::isLitIdContinue);
      }

      throw SyntaxError(reader, start, E0012);
    } else {
      auto withExp = false;
      val += ch2;
      val += ch3;

      while (!reader->eof()) {
        auto loc4 = reader->loc;
        auto ch4 = reader->next();

        if (ch4 == 'E' || ch4 == 'e') {
          withExp = true;
        } else if (!Token::isDigit(ch4)) {
          reader->seek(loc4);
          break;
        }

        val += ch4;

        if (withExp) {
          break;
        }
      }

      if (withExp) {
        lexWalkLitFloatExp(reader, start, val);
      }

      return lexLitFloat(reader, start, val, type);
    }
  } else if (Token::isLitIdContinue(ch2)) {
    lexWalk(reader, val, Token::isLitIdContinue);
    throw SyntaxError(reader, start, errCode);
  } else {
    reader->seek(loc2);
  }

  return {type, start, reader->loc, val};
}

Token lexOpEq (Reader *reader, TokenType type1, TokenType type2, ReaderLocation start, std::string &val) {
  if (reader->eof()) {
    return {type1, start, reader->loc, val};
  }

  auto loc = reader->loc;
  auto ch = reader->next();

  if (ch == '=') {
    val += ch;
    return {type2, start, reader->loc, val};
  } else {
    reader->seek(loc);
    return {type1, start, reader->loc, val};
  }
}

Token lexOpEq2 (
  Reader *reader,
  char ch,
  TokenType type1,
  TokenType type2,
  TokenType type3,
  TokenType type4,
  ReaderLocation start,
  std::string &val
) {
  if (reader->eof()) {
    return {type1, start, reader->loc, val};
  }

  auto loc1 = reader->loc;
  auto ch1 = reader->next();

  if (ch1 == '=' || reader->eof()) {
    val += ch1;
    return {ch1 == '=' ? type2 : type3, start, reader->loc, val};
  } else if (ch1 == ch) {
    auto loc2 = reader->loc;
    auto ch2 = reader->next();

    if (ch2 == '=') {
      val += ch1;
      val += ch2;
      return {type4, start, reader->loc, val};
    } else {
      val += ch1;
      reader->seek(loc2);
      return {type3, start, reader->loc, val};
    }
  } else {
    reader->seek(loc1);
    return {type1, start, reader->loc, val};
  }
}

Token lexOpEqDouble (
  Reader *reader,
  char ch,
  TokenType type1,
  TokenType type2,
  TokenType type3,
  ReaderLocation start,
  std::string &val
) {
  if (reader->eof()) {
    return {type1, start, reader->loc, val};
  }

  auto loc2 = reader->loc;
  auto ch1 = reader->next();

  if (ch1 == '=' || ch1 == ch) {
    val += ch1;
    return {ch1 == '=' ? type2 : type3, start, reader->loc, val};
  } else {
    reader->seek(loc2);
    return {type1, start, reader->loc, val};
  }
}

Token lex (Reader *reader) {
  auto start = reader->loc;

  if (reader->eof()) {
    return {tkEof, start, reader->loc, ""};
  }

  auto ch = reader->next();
  auto val = std::string(1, ch);

  if (ch == '&') return lexOpEq2(reader, '&', tkOpAnd, tkOpAndEq, tkOpAndAnd, tkOpAndAndEq, start, val);
  if (ch == '^') return lexOpEq(reader, tkOpCaret, tkOpCaretEq, start, val);
  if (ch == ':') return lexOpEq(reader, tkOpColon, tkOpColonEq, start, val);
  if (ch == ',') return {tkOpComma, start, reader->loc, val};
  if (ch == '=') return lexOpEq(reader, tkOpEq, tkOpEqEq, start, val);
  if (ch == '!') return lexOpEqDouble(reader, '!', tkOpExcl, tkOpExclEq, tkOpExclExcl, start, val);
  if (ch == '>') return lexOpEq2(reader, '>', tkOpGt, tkOpGtEq, tkOpRShift, tkOpRShiftEq, start, val);
  if (ch == '{') return {tkOpLBrace, start, reader->loc, val};
  if (ch == '[') return {tkOpLBrack, start, reader->loc, val};
  if (ch == '(') return {tkOpLPar, start, reader->loc, val};
  if (ch == '<') return lexOpEq2(reader, '<', tkOpLt, tkOpLtEq, tkOpLShift, tkOpLShiftEq, start, val);
  if (ch == '-') return lexOpEqDouble(reader, '-', tkOpMinus, tkOpMinusEq, tkOpMinusMinus, start, val);
  if (ch == '|') return lexOpEq2(reader, '|', tkOpOr, tkOpOrEq, tkOpOrOr, tkOpOrOrEq, start, val);
  if (ch == '%') return lexOpEq(reader, tkOpPercent, tkOpPercentEq, start, val);
  if (ch == '+') return lexOpEqDouble(reader, '+', tkOpPlus, tkOpPlusEq, tkOpPlusPlus, start, val);
  if (ch == '}') return {tkOpRBrace, start, reader->loc, val};
  if (ch == ']') return {tkOpRBrack, start, reader->loc, val};
  if (ch == ')') return {tkOpRPar, start, reader->loc, val};
  if (ch == ';') return {tkOpSemi, start, reader->loc, val};
  if (ch == '*') return lexOpEq2(reader, '*', tkOpStar, tkOpStarEq, tkOpStarStar, tkOpStarStarEq, start, val);
  if (ch == '~') return {tkOpTilde, start, reader->loc, val};

  if (ch == '.') {
    if (reader->eof()) {
      return {tkOpDot, start, reader->loc, val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '.') {
      val += ch1;

      if (reader->eof()) {
        return {tkOpDotDot, start, reader->loc, val};
      }

      auto loc2 = reader->loc;
      auto ch2 = reader->next();

      if (ch2 == '.' || ch2 == '=') {
        val += ch2;
        return {ch2 == '.' ? tkOpDotDotDot : tkOpDotDotEq, start, reader->loc, val};
      } else {
        reader->seek(loc2);
        return {tkOpDotDot, start, reader->loc, val};
      }
    } else {
      reader->seek(loc1);
      return {tkOpDot, start, reader->loc, val};
    }
  } else if (ch == '?') {
    if (reader->eof()) {
      return {tkOpQn, start, reader->loc, val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '.') {
      val += ch1;
      return {tkOpQnDot, start, reader->loc, val};
    } else if (ch1 == '?') {
      val += ch1;

      if (reader->eof()) {
        return {tkOpQnQn, start, reader->loc, val};
      }

      auto loc2 = reader->loc;
      auto ch2 = reader->next();

      if (ch2 == '=') {
        val += ch2;
        return {tkOpQnQnEq, start, reader->loc, val};
      } else {
        reader->seek(loc2);
        return {tkOpQnQn, start, reader->loc, val};
      }
    } else {
      reader->seek(loc1);
      return {tkOpQn, start, reader->loc, val};
    }
  } else if (ch == '/') {
    if (reader->eof()) {
      return {tkOpSlash, start, reader->loc, val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '=') {
      val += ch1;
      return {tkOpSlashEq, start, reader->loc, val};
    } else if (ch1 != '/' && ch1 != '*') {
      reader->seek(loc1);
      return {tkOpSlash, start, reader->loc, val};
    } else {
      reader->seek(loc1);
    }
  }

  if (Token::isWhitespace(ch)) {
    lexWalk(reader, val, Token::isWhitespace);
    return {tkWhitespace, start, reader->loc, val};
  } else if (Token::isLitIdStart(ch)) {
    lexWalk(reader, val, Token::isLitIdContinue);

    if (val == "as") {
      if (reader->eof()) {
        return {tkKwAs, start, reader->loc, val};
      }

      auto loc1 = reader->loc;
      auto ch1 = reader->next();

      if (ch1 == '?') {
        val += ch1;
        return {tkKwAsSafe, start, reader->loc, val};
      } else {
        reader->seek(loc1);
        return {tkKwAs, start, reader->loc, val};
      }
    }

    if (val == "async") return {tkKwAsync, start, reader->loc, val};
    if (val == "await") return {tkKwAwait, start, reader->loc, val};
    if (val == "break") return {tkKwBreak, start, reader->loc, val};
    if (val == "case") return {tkKwCase, start, reader->loc, val};
    if (val == "catch") return {tkKwCatch, start, reader->loc, val};
    if (val == "class") return {tkKwClass, start, reader->loc, val};
    if (val == "const") return {tkKwConst, start, reader->loc, val};
    if (val == "continue") return {tkKwContinue, start, reader->loc, val};
    if (val == "default") return {tkKwDefault, start, reader->loc, val};
    if (val == "deinit") return {tkKwDeinit, start, reader->loc, val};
    if (val == "elif") return {tkKwElif, start, reader->loc, val};
    if (val == "else") return {tkKwElse, start, reader->loc, val};
    if (val == "enum") return {tkKwEnum, start, reader->loc, val};
    if (val == "export") return {tkKwExport, start, reader->loc, val};
    if (val == "fallthrough") return {tkKwFallthrough, start, reader->loc, val};
    if (val == "false") return {tkKwFalse, start, reader->loc, val};
    if (val == "fn") return {tkKwFn, start, reader->loc, val};
    if (val == "from") return {tkKwFrom, start, reader->loc, val};
    if (val == "if") return {tkKwIf, start, reader->loc, val};
    if (val == "import") return {tkKwImport, start, reader->loc, val};
    if (val == "in") return {tkKwIn, start, reader->loc, val};
    if (val == "init") return {tkKwInit, start, reader->loc, val};
    if (val == "interface") return {tkKwInterface, start, reader->loc, val};
    if (val == "is") return {tkKwIs, start, reader->loc, val};
    if (val == "loop") return {tkKwLoop, start, reader->loc, val};
    if (val == "main") return {tkKwMain, start, reader->loc, val};
    if (val == "match") return {tkKwMatch, start, reader->loc, val};
    if (val == "mut") return {tkKwMut, start, reader->loc, val};
    if (val == "new") return {tkKwNew, start, reader->loc, val};
    if (val == "nil") return {tkKwNil, start, reader->loc, val};
    if (val == "obj") return {tkKwObj, start, reader->loc, val};
    if (val == "op") return {tkKwOp, start, reader->loc, val};
    if (val == "override") return {tkKwOverride, start, reader->loc, val};
    if (val == "priv") return {tkKwPriv, start, reader->loc, val};
    if (val == "prot") return {tkKwProt, start, reader->loc, val};
    if (val == "pub") return {tkKwPub, start, reader->loc, val};
    if (val == "return") return {tkKwReturn, start, reader->loc, val};
    if (val == "static") return {tkKwStatic, start, reader->loc, val};
    if (val == "super") return {tkKwSuper, start, reader->loc, val};
    if (val == "this") return {tkKwThis, start, reader->loc, val};
    if (val == "throw") return {tkKwThrow, start, reader->loc, val};
    if (val == "true") return {tkKwTrue, start, reader->loc, val};
    if (val == "try") return {tkKwTry, start, reader->loc, val};
    if (val == "union") return {tkKwUnion, start, reader->loc, val};

    return {tkLitId, start, reader->loc, val};
  } else if (Token::isDigit(ch)) {
    if (ch == '0') {
      if (reader->eof()) {
        return {tkLitIntDec, start, reader->loc, val};
      }

      auto loc1 = reader->loc;
      auto ch1 = reader->next();

      if (Token::isDigit(ch1)) {
        lexWalk(reader, val, Token::isLitIdContinue);
        throw SyntaxError(reader, start, E0007);
      } else if (ch1 == 'B' || ch1 == 'b' || ch1 == 'X' || ch1 == 'x' || ch1 == 'O' || ch1 == 'o') {
        val += ch1;

        if (ch1 == 'B' || ch1 == 'b') {
          return lexLitNum(reader, start, val, Token::isLitIntBin, tkLitIntBin);
        } else if (ch1 == 'X' || ch1 == 'x') {
          return lexLitNum(reader, start, val, Token::isLitIntHex, tkLitIntHex);
        } else {
          return lexLitNum(reader, start, val, Token::isLitIntOct, tkLitIntOct);
        }
      } else {
        reader->seek(loc1);
      }

      return lexLitNum(reader, start, val, Token::isLitIntDec, tkLitIntDec);
    }

    lexWalk(reader, val, Token::isLitIntDec);
    return lexLitNum(reader, start, val, Token::isLitIntDec, tkLitIntDec);
  } else if (ch == '"') {
    if (reader->eof()) {
      throw SyntaxError(reader, start, E0003);
    }

    lexWalkLitStr(reader, start, val);
    return {tkLitStr, start, reader->loc, val};
  } else if (ch == '/') {
    auto ch1 = reader->next();

    if (ch1 == '/') {
      val += ch1;
      lexWalk(reader, val, Token::isNotNewline);
      return {tkCommentLine, start, reader->loc, val};
    } else {
      val += ch1;

      if (reader->eof()) {
        throw SyntaxError(reader, start, E0001);
      }

      while (true) {
        auto ch2 = reader->next();

        if (reader->eof()) {
          throw SyntaxError(reader, start, E0001);
        } else if (ch2 == '*') {
          auto loc3 = reader->loc;
          auto ch3 = reader->next();

          if (ch3 == '/') {
            val += ch2;
            val += ch3;
            break;
          } else {
            reader->seek(loc3);
          }
        }

        val += ch2;
      }

      return {tkCommentBlock, start, reader->loc, val};
    }
  } else if (ch == '\'') {
    if (reader->eof()) {
      throw SyntaxError(reader, start, E0002);
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '\'') {
      throw SyntaxError(reader, start, E0004);
    } else if (reader->eof()) {
      throw SyntaxError(reader, start, E0002);
    } else if (ch1 == '\\') {
      auto ch2 = reader->next();

      if (!Token::isLitCharEscape(ch2)) {
        throw SyntaxError(reader, loc1, E0004);
      } else if (reader->eof()) {
        throw SyntaxError(reader, start, E0002);
      }

      val += ch1;
      val += ch2;
    } else {
      val += ch1;
    }

    auto ch3 = reader->next();

    if (ch3 != '\'') {
      while (!reader->eof()) {
        if (reader->next() == '\'') {
          break;
        }
      }

      throw SyntaxError(reader, start, E0006);
    }

    val += ch3;
    return {tkLitChar, start, reader->loc, val};
  }

  throw SyntaxError(reader, start, E0000);
}
