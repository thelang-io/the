/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

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
      lexWalk(reader, lexer, tokenIsLitIdContinue);
      throw SyntaxError(reader, lexer.start, E0013);
    }

    lexer.val += ch1;
    lexer.val += ch2;
  } else if (tokenIsDigit(ch1)) {
    lexer.val += ch1;
  } else {
    lexWalk(reader, lexer, tokenIsLitIdContinue);
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

void lexWalkLitStr (Reader *reader, Lexer &lexer) {
  auto blocks = 0;
  auto insideChar = false;

  while (true) {
    auto loc1 = reader->loc;
    auto ch1 = reader->next();
    lexer.val += ch1;

    if (ch1 == '\\' && blocks == 0) {
      if (reader->eof()) {
        throw SyntaxError(reader, lexer.start, E0003);
      }

      auto ch2 = reader->next();

      if (!tokenIsLitStrEscape(ch2)) {
        throw SyntaxError(reader, loc1, E0005);
      }

      lexer.val += ch2;
    } else if (ch1 == '{' && !insideChar) {
      blocks += 1;
    } else if (ch1 == '}' && blocks != 0 && !insideChar) {
      blocks -= 1;
    } else if (ch1 == '\'' && insideChar) {
      insideChar = false;
    } else if (ch1 == '\'' && blocks != 0) {
      insideChar = true;
    } else if (ch1 == '"' && blocks != 0) {
      lexWalkLitStr(reader, lexer);
    } else if (ch1 == '"') {
      break;
    } else if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0003);
    }
  }
}

Token lexLitFloat (Reader *reader, Lexer &lexer, TokenType type) {
  if (!reader->eof()) {
    auto loc = reader->loc;
    auto ch = reader->next();

    if (tokenIsLitIdContinue(ch)) {
      lexWalk(reader, lexer, tokenIsLitIdContinue);
      throw SyntaxError(reader, lexer.start, E0012);
    } else {
      reader->seek(loc);
    }
  }

  if (type == tkLitIntBin) {
    throw SyntaxError(reader, lexer.start, E0014);
  } else if (type == tkLitIntHex) {
    throw SyntaxError(reader, lexer.start, E0015);
  } else if (type == tkLitIntOct) {
    throw SyntaxError(reader, lexer.start, E0016);
  }

  return {tkLitFloat, lexer.start, reader->loc, lexer.val};
}

Token lexLitNum (Reader *reader, Lexer &lexer, const std::function<bool (char)> &fn, TokenType type) {
  auto errCode = type == tkLitIntBin ? E0008 : type == tkLitIntDec ? E0009 : type == tkLitIntHex ? E0010 : E0011;

  if (type != tkLitIntDec) {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, errCode);
    }

    auto ch1 = reader->next();

    if (!fn(ch1)) {
      lexWalk(reader, lexer, tokenIsLitIdContinue);
      throw SyntaxError(reader, lexer.start, errCode);
    }

    lexer.val += ch1;
    lexWalk(reader, lexer, fn);
  }

  if (reader->eof()) {
    return {type, lexer.start, reader->loc, lexer.val};
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
    } else if (!tokenIsLitIdContinue(ch3)) {
      if (tokenIsLitIdContinue(ch3)) {
        lexWalk(reader, lexer, tokenIsLitIdContinue);
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
  } else if (tokenIsLitIdContinue(ch2)) {
    lexWalk(reader, lexer, tokenIsLitIdContinue);
    throw SyntaxError(reader, lexer.start, errCode);
  } else {
    reader->seek(loc2);
  }

  return {type, lexer.start, reader->loc, lexer.val};
}

Token lexOpEq (Reader *reader, Lexer &lexer, TokenType type1, TokenType type2) {
  if (reader->eof()) {
    return {type1, lexer.start, reader->loc, lexer.val};
  }

  auto loc = reader->loc;
  auto ch = reader->next();

  if (ch == '=') {
    lexer.val += ch;
    return {type2, lexer.start, reader->loc, lexer.val};
  } else {
    reader->seek(loc);
    return {type1, lexer.start, reader->loc, lexer.val};
  }
}

Token lexOpEq2 (
  Reader *reader,
  Lexer &lexer,
  char ch,
  TokenType type1,
  TokenType type2,
  TokenType type3,
  TokenType type4
) {
  if (reader->eof()) {
    return {type1, lexer.start, reader->loc, lexer.val};
  }

  auto loc1 = reader->loc;
  auto ch1 = reader->next();

  if (ch1 == '=' || reader->eof()) {
    lexer.val += ch1;
    return {ch1 == '=' ? type2 : type3, lexer.start, reader->loc, lexer.val};
  } else if (ch1 == ch) {
    auto loc2 = reader->loc;
    auto ch2 = reader->next();

    if (ch2 == '=') {
      lexer.val += ch1;
      lexer.val += ch2;
      return {type4, lexer.start, reader->loc, lexer.val};
    } else {
      lexer.val += ch1;
      reader->seek(loc2);
      return {type3, lexer.start, reader->loc, lexer.val};
    }
  } else {
    reader->seek(loc1);
    return {type1, lexer.start, reader->loc, lexer.val};
  }
}

Token lexOpEqDouble (Reader *reader, Lexer &lexer, char ch, TokenType type1, TokenType type2, TokenType type3) {
  if (reader->eof()) {
    return {type1, lexer.start, reader->loc, lexer.val};
  }

  auto loc2 = reader->loc;
  auto ch1 = reader->next();

  if (ch1 == '=' || ch1 == ch) {
    lexer.val += ch1;
    return {ch1 == '=' ? type2 : type3, lexer.start, reader->loc, lexer.val};
  } else {
    reader->seek(loc2);
    return {type1, lexer.start, reader->loc, lexer.val};
  }
}

Token lex (Reader *reader) {
  auto lexer = Lexer{reader->loc, ""};

  if (reader->eof()) {
    return {tkEof, lexer.start, reader->loc, lexer.val};
  }

  auto ch = reader->next();
  lexer.val += ch;

  if (ch == '&') return lexOpEq2(reader, lexer, '&', tkOpAnd, tkOpAndEq, tkOpAndAnd, tkOpAndAndEq);
  if (ch == '^') return lexOpEq(reader, lexer, tkOpCaret, tkOpCaretEq);
  if (ch == ':') return lexOpEq(reader, lexer, tkOpColon, tkOpColonEq);
  if (ch == ',') return {tkOpComma, lexer.start, reader->loc, lexer.val};
  if (ch == '=') return lexOpEq(reader, lexer, tkOpEq, tkOpEqEq);
  if (ch == '!') return lexOpEqDouble(reader, lexer, '!', tkOpExcl, tkOpExclEq, tkOpExclExcl);
  if (ch == '>') return lexOpEq2(reader, lexer, '>', tkOpGt, tkOpGtEq, tkOpRShift, tkOpRShiftEq);
  if (ch == '{') return {tkOpLBrace, lexer.start, reader->loc, lexer.val};
  if (ch == '[') return {tkOpLBrack, lexer.start, reader->loc, lexer.val};
  if (ch == '(') return {tkOpLPar, lexer.start, reader->loc, lexer.val};
  if (ch == '<') return lexOpEq2(reader, lexer, '<', tkOpLt, tkOpLtEq, tkOpLShift, tkOpLShiftEq);
  if (ch == '-') return lexOpEqDouble(reader, lexer, '-', tkOpMinus, tkOpMinusEq, tkOpMinusMinus);
  if (ch == '|') return lexOpEq2(reader, lexer, '|', tkOpOr, tkOpOrEq, tkOpOrOr, tkOpOrOrEq);
  if (ch == '%') return lexOpEq(reader, lexer, tkOpPercent, tkOpPercentEq);
  if (ch == '+') return lexOpEqDouble(reader, lexer, '+', tkOpPlus, tkOpPlusEq, tkOpPlusPlus);
  if (ch == '}') return {tkOpRBrace, lexer.start, reader->loc, lexer.val};
  if (ch == ']') return {tkOpRBrack, lexer.start, reader->loc, lexer.val};
  if (ch == ')') return {tkOpRPar, lexer.start, reader->loc, lexer.val};
  if (ch == ';') return {tkOpSemi, lexer.start, reader->loc, lexer.val};
  if (ch == '*') return lexOpEq2(reader, lexer, '*', tkOpStar, tkOpStarEq, tkOpStarStar, tkOpStarStarEq);
  if (ch == '~') return {tkOpTilde, lexer.start, reader->loc, lexer.val};

  if (ch == '.') {
    if (reader->eof()) {
      return {tkOpDot, lexer.start, reader->loc, lexer.val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '.') {
      lexer.val += ch1;

      if (reader->eof()) {
        return {tkOpDotDot, lexer.start, reader->loc, lexer.val};
      }

      auto loc2 = reader->loc;
      auto ch2 = reader->next();

      if (ch2 == '.' || ch2 == '=') {
        lexer.val += ch2;
        return {ch2 == '.' ? tkOpDotDotDot : tkOpDotDotEq, lexer.start, reader->loc, lexer.val};
      } else {
        reader->seek(loc2);
        return {tkOpDotDot, lexer.start, reader->loc, lexer.val};
      }
    } else {
      reader->seek(loc1);
      return {tkOpDot, lexer.start, reader->loc, lexer.val};
    }
  } else if (ch == '?') {
    if (reader->eof()) {
      return {tkOpQn, lexer.start, reader->loc, lexer.val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '.') {
      lexer.val += ch1;
      return {tkOpQnDot, lexer.start, reader->loc, lexer.val};
    } else if (ch1 == '?') {
      lexer.val += ch1;

      if (reader->eof()) {
        return {tkOpQnQn, lexer.start, reader->loc, lexer.val};
      }

      auto loc2 = reader->loc;
      auto ch2 = reader->next();

      if (ch2 == '=') {
        lexer.val += ch2;
        return {tkOpQnQnEq, lexer.start, reader->loc, lexer.val};
      } else {
        reader->seek(loc2);
        return {tkOpQnQn, lexer.start, reader->loc, lexer.val};
      }
    } else {
      reader->seek(loc1);
      return {tkOpQn, lexer.start, reader->loc, lexer.val};
    }
  } else if (ch == '/') {
    if (reader->eof()) {
      return {tkOpSlash, lexer.start, reader->loc, lexer.val};
    }

    auto loc1 = reader->loc;
    auto ch1 = reader->next();

    if (ch1 == '=') {
      lexer.val += ch1;
      return {tkOpSlashEq, lexer.start, reader->loc, lexer.val};
    } else if (ch1 != '/' && ch1 != '*') {
      reader->seek(loc1);
      return {tkOpSlash, lexer.start, reader->loc, lexer.val};
    } else {
      reader->seek(loc1);
    }
  }

  if (tokenIsWhitespace(ch)) {
    lexWalk(reader, lexer, tokenIsWhitespace);
    return {tkWhitespace, lexer.start, reader->loc, lexer.val};
  } else if (tokenIsLitIdStart(ch)) {
    lexWalk(reader, lexer, tokenIsLitIdContinue);

    if (lexer.val == "as") {
      if (reader->eof()) {
        return {tkKwAs, lexer.start, reader->loc, lexer.val};
      }

      auto loc1 = reader->loc;
      auto ch1 = reader->next();

      if (ch1 == '?') {
        lexer.val += ch1;
        return {tkKwAsSafe, lexer.start, reader->loc, lexer.val};
      } else {
        reader->seek(loc1);
        return {tkKwAs, lexer.start, reader->loc, lexer.val};
      }
    }

    if (lexer.val == "async") return {tkKwAsync, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "await") return {tkKwAwait, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "break") return {tkKwBreak, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "case") return {tkKwCase, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "catch") return {tkKwCatch, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "class") return {tkKwClass, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "const") return {tkKwConst, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "continue") return {tkKwContinue, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "default") return {tkKwDefault, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "deinit") return {tkKwDeinit, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "elif") return {tkKwElif, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "else") return {tkKwElse, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "enum") return {tkKwEnum, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "export") return {tkKwExport, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "fallthrough") return {tkKwFallthrough, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "false") return {tkKwFalse, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "fn") return {tkKwFn, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "from") return {tkKwFrom, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "if") return {tkKwIf, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "import") return {tkKwImport, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "in") return {tkKwIn, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "init") return {tkKwInit, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "interface") return {tkKwInterface, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "is") return {tkKwIs, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "loop") return {tkKwLoop, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "main") return {tkKwMain, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "match") return {tkKwMatch, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "mut") return {tkKwMut, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "new") return {tkKwNew, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "nil") return {tkKwNil, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "obj") return {tkKwObj, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "op") return {tkKwOp, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "override") return {tkKwOverride, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "priv") return {tkKwPriv, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "prot") return {tkKwProt, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "pub") return {tkKwPub, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "return") return {tkKwReturn, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "static") return {tkKwStatic, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "super") return {tkKwSuper, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "this") return {tkKwThis, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "throw") return {tkKwThrow, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "true") return {tkKwTrue, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "try") return {tkKwTry, lexer.start, reader->loc, lexer.val};
    if (lexer.val == "union") return {tkKwUnion, lexer.start, reader->loc, lexer.val};

    return {tkLitId, lexer.start, reader->loc, lexer.val};
  } else if (tokenIsDigit(ch)) {
    if (ch == '0') {
      if (reader->eof()) {
        return {tkLitIntDec, lexer.start, reader->loc, lexer.val};
      }

      auto loc1 = reader->loc;
      auto ch1 = reader->next();

      if (tokenIsDigit(ch1)) {
        lexWalk(reader, lexer, tokenIsLitIdContinue);
        throw SyntaxError(reader, lexer.start, E0007);
      } else if (ch1 == 'B' || ch1 == 'b' || ch1 == 'X' || ch1 == 'x' || ch1 == 'O' || ch1 == 'o') {
        lexer.val += ch1;

        if (ch1 == 'B' || ch1 == 'b') {
          return lexLitNum(reader, lexer, tokenIsLitIntBin, tkLitIntBin);
        } else if (ch1 == 'X' || ch1 == 'x') {
          return lexLitNum(reader, lexer, tokenIsLitIntHex, tkLitIntHex);
        } else {
          return lexLitNum(reader, lexer, tokenIsLitIntOct, tkLitIntOct);
        }
      } else {
        reader->seek(loc1);
      }

      return lexLitNum(reader, lexer, tokenIsLitIntDec, tkLitIntDec);
    }

    lexWalk(reader, lexer, tokenIsLitIntDec);
    return lexLitNum(reader, lexer, tokenIsLitIntDec, tkLitIntDec);
  } else if (ch == '"') {
    if (reader->eof()) {
      throw SyntaxError(reader, lexer.start, E0003);
    }

    lexWalkLitStr(reader, lexer);
    return {tkLitStr, lexer.start, reader->loc, lexer.val};
  } else if (ch == '/') {
    auto ch1 = reader->next();

    if (ch1 == '/') {
      lexer.val += ch1;
      lexWalk(reader, lexer, tokenIsNotNewline);
      return {tkCommentLine, lexer.start, reader->loc, lexer.val};
    } else {
      lexer.val += ch1;

      if (reader->eof()) {
        throw SyntaxError(reader, lexer.start, E0001);
      }

      while (true) {
        auto ch2 = reader->next();

        if (reader->eof()) {
          throw SyntaxError(reader, lexer.start, E0001);
        } else if (ch2 == '*') {
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

      return {tkCommentBlock, lexer.start, reader->loc, lexer.val};
    }
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
    return {tkLitChar, lexer.start, reader->loc, lexer.val};
  }

  throw SyntaxError(reader, lexer.start, E0000);
}
