/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Lexer.hpp"
#include "SyntaxError.hpp"

Lexer::Lexer (Reader *reader) {
  this->_reader = reader;
}

Token Lexer::next () {
  this->_start = this->_reader->loc();

  if (this->_reader->eof()) {
    this->_val = static_cast<char>(-1);
    return this->_token(eof);
  }

  const auto ch1 = this->_reader->next();
  this->_val = ch1;

  if (ch1 == '&') return this->_lexOpEq2('&', opAnd, opAndEq, opAndAnd, opAndAndEq);
  if (ch1 == '^') return this->_lexOpEq(opCaret, opCaretEq);
  if (ch1 == ':') return this->_lexOpEq(opColon, opColonEq);
  if (ch1 == ',') return this->_token(opComma);
  if (ch1 == '=') return this->_lexOpEq(opEq, opEqEq);
  if (ch1 == '!') return this->_lexOpEqDouble('!', opExcl, opExclEq, opExclExcl);
  if (ch1 == '{') return this->_token(opLBrace);
  if (ch1 == '[') return this->_token(opLBrack);
  if (ch1 == '(') return this->_token(opLPar);
  if (ch1 == '<') return this->_lexOpEq2('<', opLt, opLtEq, opLShift, opLShiftEq);
  if (ch1 == '-') return this->_lexOpEqDouble('-', opMinus, opMinusEq, opMinusMinus);
  if (ch1 == '|') return this->_lexOpEq2('|', opOr, opOrEq, opOrOr, opOrOrEq);
  if (ch1 == '%') return this->_lexOpEq(opPercent, opPercentEq);
  if (ch1 == '+') return this->_lexOpEqDouble('+', opPlus, opPlusEq, opPlusPlus);
  if (ch1 == '}') return this->_token(opRBrace);
  if (ch1 == ']') return this->_token(opRBrack);
  if (ch1 == ')') return this->_token(opRPar);
  if (ch1 == ';') return this->_token(opSemi);
  if (ch1 == '*') return this->_lexOpEq2('*', opStar, opStarEq, opStarStar, opStarStarEq);
  if (ch1 == '~') return this->_token(opTilde);

  if (ch1 == '.') {
    if (this->_reader->eof()) {
      return this->_token(opDot);
    }

    const auto loc2 = this->_reader->loc();
    const auto ch2 = this->_reader->next();

    if (ch2 == '.') {
      this->_val += ch2;

      if (this->_reader->eof()) {
        return this->_token(opDotDot);
      }

      const auto loc3 = this->_reader->loc();
      const auto ch3 = this->_reader->next();

      if (ch3 == '.') {
        this->_val += ch3;
        return this->_token(opDotDotDot);
      } else if (ch3 == '=') {
        this->_val += ch3;
        return this->_token(opDotDotEq);
      } else {
        this->_reader->seek(loc3);
        return this->_token(opDotDot);
      }
    } else {
      this->_reader->seek(loc2);
      return this->_token(opDot);
    }
  } else if (ch1 == '/') {
    if (this->_reader->eof()) {
      return this->_token(opSlash);
    }

    const auto loc2 = this->_reader->loc();
    const auto ch2 = this->_reader->next();

    if (ch2 == '=') {
      this->_val += ch2;
      return this->_token(opSlashEq);
    } else if (ch2 != '/' && ch2 != '*') {
      this->_reader->seek(loc2);
      return this->_token(opSlash);
    } else {
      this->_reader->seek(loc2);
    }
  } else if (ch1 == '?') {
    if (this->_reader->eof()) {
      return this->_token(opQn);
    }

    const auto loc2 = this->_reader->loc();
    const auto ch2 = this->_reader->next();

    if (ch2 == '.') {
      this->_val += ch2;
      return this->_token(opQnDot);
    } else if (ch2 == '?') {
      this->_val += ch2;

      if (this->_reader->eof()) {
        return this->_token(opQnQn);
      }

      const auto loc3 = this->_reader->loc();
      const auto ch3 = this->_reader->next();

      if (ch3 == '=') {
        this->_val += ch3;
        return this->_token(opQnQnEq);
      } else {
        this->_reader->seek(loc3);
        return this->_token(opQnQn);
      }
    } else {
      this->_reader->seek(loc2);
      return this->_token(opQn);
    }
  }

  if (Token::isWhitespace(ch1)) {
    while (!this->_reader->eof()) {
      const auto loc2 = this->_reader->loc();
      const auto ch2 = this->_reader->next();

      if (!Token::isWhitespace(ch2)) {
        this->_reader->seek(loc2);
        break;
      }

      this->_val += ch2;
    }

    return this->_token(whitespace);
  } else if (Token::isLitIdStart(ch1)) {
    while (!this->_reader->eof()) {
      const auto loc2 = this->_reader->loc();
      const auto ch2 = this->_reader->next();

      if (!Token::isLitIdContinue(ch2)) {
        this->_reader->seek(loc2);
        break;
      }

      this->_val += ch2;
    }

    if (this->_val == "as") {
      const auto loc2 = this->_reader->loc();
      const auto ch2 = this->_reader->next();

      if (ch2 == '?') {
        this->_val += ch2;
        return this->_token(kwAsSafe);
      } else {
        this->_reader->seek(loc2);
        return this->_token(kwAs);
      }
    }

    if (this->_val == "async") return this->_token(kwAsync);
    if (this->_val == "await") return this->_token(kwAwait);
    if (this->_val == "break") return this->_token(kwBreak);
    if (this->_val == "case") return this->_token(kwCase);
    if (this->_val == "catch") return this->_token(kwCatch);
    if (this->_val == "class") return this->_token(kwClass);
    if (this->_val == "const") return this->_token(kwConst);
    if (this->_val == "continue") return this->_token(kwContinue);
    if (this->_val == "default") return this->_token(kwDefault);
    if (this->_val == "deinit") return this->_token(kwDeinit);
    if (this->_val == "elif") return this->_token(kwElif);
    if (this->_val == "else") return this->_token(kwElse);
    if (this->_val == "enum") return this->_token(kwEnum);
    if (this->_val == "export") return this->_token(kwExport);
    if (this->_val == "fallthrough") return this->_token(kwFallthrough);
    if (this->_val == "false") return this->_token(kwFalse);
    if (this->_val == "fn") return this->_token(kwFn);
    if (this->_val == "from") return this->_token(kwFrom);
    if (this->_val == "if") return this->_token(kwIf);
    if (this->_val == "import") return this->_token(kwImport);
    if (this->_val == "in") return this->_token(kwIn);
    if (this->_val == "init") return this->_token(kwInit);
    if (this->_val == "interface") return this->_token(kwInterface);
    if (this->_val == "is") return this->_token(kwIs);
    if (this->_val == "loop") return this->_token(kwLoop);
    if (this->_val == "main") return this->_token(kwMain);
    if (this->_val == "match") return this->_token(kwMatch);
    if (this->_val == "mut") return this->_token(kwMut);
    if (this->_val == "new") return this->_token(kwNew);
    if (this->_val == "nil") return this->_token(kwNil);
    if (this->_val == "obj") return this->_token(kwObj);
    if (this->_val == "op") return this->_token(kwOp);
    if (this->_val == "override") return this->_token(kwOverride);
    if (this->_val == "priv") return this->_token(kwPriv);
    if (this->_val == "prot") return this->_token(kwProt);
    if (this->_val == "pub") return this->_token(kwPub);
    if (this->_val == "return") return this->_token(kwReturn);
    if (this->_val == "static") return this->_token(kwStatic);
    if (this->_val == "super") return this->_token(kwSuper);
    if (this->_val == "this") return this->_token(kwThis);
    if (this->_val == "throw") return this->_token(kwThrow);
    if (this->_val == "true") return this->_token(kwTrue);
    if (this->_val == "try") return this->_token(kwTry);
    if (this->_val == "union") return this->_token(kwUnion);

    return this->_token(litId);
  } else if (isdigit(ch1)) {
    if (ch1 == '0') {
      if (this->_reader->eof()) {
        return this->_token(litIntDec);
      }

      const auto loc2 = this->_reader->loc();
      const auto ch2 = this->_reader->next();

      if (Token::isLitIntDec(ch2)) {
        throw SyntaxError(
          this->_reader,
          this->_start,
          this->_start,
          "Numeric literals with leading zero are not allowed"
        );
      } else if (ch2 == 'X' || ch2 == 'x') {
        this->_val += ch2;

        if (this->_reader->eof()) {
          throw SyntaxError(
            this->_reader,
            this->_start,
            this->_reader->loc(),
            std::string("Hexadecimal literals with trailing '") + ch2 + "' are not allowed"
          );
        }

        const auto loc3 = this->_reader->loc();
        const auto ch3 = this->_reader->next();

        if (!Token::isLitIntHex(ch3)) {
          throw SyntaxError(
            this->_reader,
            this->_start,
            loc3,
            std::string("Hexadecimal literals with trailing '") + ch2 + "' are not allowed"
          );
        }

        this->_val += ch3;

        while (!this->_reader->eof()) {
          const auto loc4 = this->_reader->loc();
          const auto ch4 = this->_reader->next();

          if (!Token::isLitIntHex(ch4)) {
            this->_reader->seek(loc4);
            break;
          }

          this->_val += ch4;
        }

        return this->_token(litIntHex);
      } else {
        this->_reader->seek(loc2);
      }

      return this->_token(litIntDec);
    }

    while (!this->_reader->eof()) {
      const auto loc2 = this->_reader->loc();
      const auto ch2 = this->_reader->next();

      if (!Token::isLitIntDec(ch2)) {
        this->_reader->seek(loc2);
        break;
      }

      this->_val += ch2;
    }

    return this->_token(litIntDec);
  } else if (ch1 == '/' && !this->_reader->eof()) {
    const auto loc2 = this->_reader->loc();
    const auto ch2 = this->_reader->next();

    if (ch2 == '/') {
      this->_val += ch2;

      while (!this->_reader->eof()) {
        const auto loc3 = this->_reader->loc();
        const auto ch3 = this->_reader->next();

        if (ch3 == '\n') {
          this->_reader->seek(loc3);
          break;
        }

        this->_val += ch3;
      }

      return this->_token(commentLine);
    } else if (ch2 == '*') {
      this->_val += ch2;

      while (true) {
        const auto ch3 = this->_reader->next();

        if (this->_reader->eof()) {
          throw SyntaxError(this->_reader, this->_start, this->_start, "Unterminated block comment");
        } else if (ch3 == '*') {
          const auto loc4 = this->_reader->loc();
          const auto ch4 = this->_reader->next();

          if (ch4 == '/') {
            this->_val += ch3;
            this->_val += ch4;
            break;
          } else {
            this->_reader->seek(loc4);
          }
        }

        this->_val += ch3;
      }

      return this->_token(commentBlock);
    } else {
      this->_reader->seek(loc2);
    }
  } else if (ch1 == '\'') {
    if (this->_reader->eof()) {
      throw SyntaxError(this->_reader, this->_start, this->_start, "Unterminated character literal");
    }

    const auto ch2 = this->_reader->next();

    if (ch2 == '\'') {
      throw SyntaxError(this->_reader, this->_start, this->_start, "Empty character literal");
    } else if (this->_reader->eof()) {
      throw SyntaxError(this->_reader, this->_start, this->_start, "Unterminated character literal");
    } else if (ch2 == '\\') {
      const auto ch3 = this->_reader->next();

      if (!Token::isLitCharEscape(ch3)) {
        throw SyntaxError(this->_reader, this->_start, this->_start, "Illegal character escape");
      } else if (this->_reader->eof()) {
        throw SyntaxError(this->_reader, this->_start, this->_start, "Unterminated character literal");
      }

      this->_val += ch2;
      this->_val += ch3;
    } else {
      this->_val += ch2;
    }

    const auto ch4 = this->_reader->next();

    if (ch4 != '\'') {
      throw SyntaxError(this->_reader, this->_start, this->_start, "Too many characters in character literal");
    }

    this->_val += ch4;
    return this->_token(litChar);
  }

  throw SyntaxError(this->_reader, this->_start, this->_reader->loc(), "Unexpected token");
}

Token Lexer::_lexOpEq (const TokenType tt1, const TokenType tt2) {
  if (this->_reader->eof()) {
    return this->_token(tt1);
  }

  const auto loc2 = this->_reader->loc();
  const auto ch2 = this->_reader->next();

  if (ch2 == '=') {
    this->_val += ch2;
    return this->_token(tt2);
  } else {
    this->_reader->seek(loc2);
    return this->_token(tt1);
  }
}

Token Lexer::_lexOpEq2 (
  const char ch1,
  const TokenType tt1,
  const TokenType tt2,
  const TokenType tt3,
  const TokenType tt4
) {
  if (this->_reader->eof()) {
    return this->_token(tt1);
  }

  const auto loc2 = this->_reader->loc();
  const auto ch2 = this->_reader->next();

  if (ch2 == '=') {
    this->_val += ch2;
    return this->_token(tt2);
  } else if (ch2 == ch1) {
    this->_val += ch2;

    if (this->_reader->eof()) {
      return this->_token(tt3);
    }

    const auto loc3 = this->_reader->loc();
    const auto ch3 = this->_reader->next();

    if (ch3 == '=') {
      this->_val += ch3;
      return this->_token(tt4);
    } else {
      this->_reader->seek(loc3);
      return this->_token(tt3);
    }
  } else {
    this->_reader->seek(loc2);
    return this->_token(tt1);
  }
}

Token Lexer::_lexOpEqDouble (const char ch1, const TokenType tt1, const TokenType tt2, const TokenType tt3) {
  if (this->_reader->eof()) {
    return this->_token(tt1);
  }

  const auto loc2 = this->_reader->loc();
  const auto ch2 = this->_reader->next();

  if (ch2 == '=') {
    this->_val += ch2;
    return this->_token(tt2);
  } else if (ch2 == ch1) {
    this->_val += ch2;
    return this->_token(tt3);
  } else {
    this->_reader->seek(loc2);
    return this->_token(tt1);
  }
}

Token Lexer::_token (const TokenType tt) {
  this->_end = this->_reader->loc();
  return Token(tt, this->_val, this->_start, this->_end);
}
