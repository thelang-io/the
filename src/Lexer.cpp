/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Lexer.hpp"

Lexer::Lexer (Reader *reader) {
  this->_reader = reader;
}

Token Lexer::next () {
  if (this->_reader->eof()) {
    return Token(eof, "\0");
  }

  auto ch = this->_reader->next();

  if (ch == '^') return this->_lexOpEq('^', opCaret, opCaretEq);
  if (ch == ':') return this->_lexOpEq(':', opColon, opColonEq);
  if (ch == ',') return Token(opComma, ch);
  if (ch == '=') return this->_lexOpEq('=', opEq, opEqEq);
  if (ch == '{') return Token(opLBrace, ch);
  if (ch == '[') return Token(opLBrack, ch);
  if (ch == '(') return Token(opLPar, ch);
  if (ch == '%') return this->_lexOpEq('%', opPercent, opPercentEq);
  if (ch == '}') return Token(opRBrace, ch);
  if (ch == ']') return Token(opRBrack, ch);
  if (ch == ')') return Token(opRPar, ch);
  if (ch == ';') return Token(opSemi, ch);
  if (ch == '~') return Token(opTilde, ch);

  if (ch == '!') {
    return this->_lexOpEqDouble('!', opExcl, opExclEq, opExclExcl);
  } else if (ch == '-') {
    return this->_lexOpEqDouble('-', opMinus, opMinusEq, opMinusMinus);
  } else if (ch == '+') {
    return this->_lexOpEqDouble('+', opPlus, opPlusEq, opPlusPlus);
  }

  if (ch == '&') {
    return this->_lexOpEq2('&', opAnd, opAndEq, opAndAnd, opAndAndEq);
  } else if (ch == '>') {
    return this->_lexOpEq2('>', opGt, opGtEq, opRShift, opRShiftEq);
  } else if (ch == '<') {
    return this->_lexOpEq2('<', opLt, opLtEq, opLShift, opLShiftEq);
  } else if (ch == '|') {
    return this->_lexOpEq2('|', opOr, opOrEq, opOrOr, opOrOrEq);
  } else if (ch == '*') {
    return this->_lexOpEq2('*', opStar, opStarEq, opStarStar, opStarStarEq);
  }

  auto val = std::string{ch};

  if (Token::isWhitespace(ch)) {
    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isWhitespace(ch)) {
        this->_reader->seek(loc);
        break;
      }

      val += ch;
    }

    return Token(whitespace, val);
  } else if (Token::isIdStart(ch)) {
    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isIdContinue(ch)) {
        this->_reader->seek(loc);
        break;
      }

      val += ch;
    }

    if (val == "as") {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (ch == '?') {
        val += ch;
        return Token(kwAsSafe, val);
      } else {
        this->_reader->seek(loc);
        return Token(kwAs, val);
      }
    }

    if (val == "async") return Token(kwAsync, val);
    if (val == "await") return Token(kwAwait, val);
    if (val == "break") return Token(kwBreak, val);
    if (val == "case") return Token(kwCase, val);
    if (val == "catch") return Token(kwCatch, val);
    if (val == "class") return Token(kwClass, val);
    if (val == "const") return Token(kwConst, val);
    if (val == "continue") return Token(kwContinue, val);
    if (val == "default") return Token(kwDefault, val);
    if (val == "deinit") return Token(kwDeinit, val);
    if (val == "elif") return Token(kwElif, val);
    if (val == "else") return Token(kwElse, val);
    if (val == "enum") return Token(kwEnum, val);
    if (val == "export") return Token(kwExport, val);
    if (val == "fallthrough") return Token(kwFallthrough, val);
    if (val == "false") return Token(kwFalse, val);
    if (val == "fn") return Token(kwFn, val);
    if (val == "from") return Token(kwFrom, val);
    if (val == "if") return Token(kwIf, val);
    if (val == "import") return Token(kwImport, val);
    if (val == "in") return Token(kwIn, val);
    if (val == "init") return Token(kwInit, val);
    if (val == "interface") return Token(kwInterface, val);
    if (val == "is") return Token(kwIs, val);
    if (val == "loop") return Token(kwLoop, val);
    if (val == "main") return Token(kwMain, val);
    if (val == "match") return Token(kwMatch, val);
    if (val == "mut") return Token(kwMut, val);
    if (val == "new") return Token(kwNew, val);
    if (val == "nil") return Token(kwNil, val);
    if (val == "obj") return Token(kwObj, val);
    if (val == "op") return Token(kwOp, val);
    if (val == "override") return Token(kwOverride, val);
    if (val == "priv") return Token(kwPriv, val);
    if (val == "prot") return Token(kwProt, val);
    if (val == "pub") return Token(kwPub, val);
    if (val == "return") return Token(kwReturn, val);
    if (val == "static") return Token(kwStatic, val);
    if (val == "super") return Token(kwSuper, val);
    if (val == "this") return Token(kwThis, val);
    if (val == "throw") return Token(kwThrow, val);
    if (val == "true") return Token(kwTrue, val);
    if (val == "try") return Token(kwTry, val);
    if (val == "union") return Token(kwUnion, val);

    return Token(litId, val);
  }

  return Token(unknown, "");
}

Token Lexer::_lexOpEq (
  const char ch,
  const TokenType tt1,
  const TokenType tt2
) {
  if (this->_reader->eof()) {
    return Token(tt1, ch);
  }

  const auto loc1 = this->_reader->loc();
  const auto ch1 = this->_reader->next();

  if (ch1 == '=') {
    return Token(tt2, std::string{ch} + ch1);
  } else {
    this->_reader->seek(loc1);
    return Token(tt1, ch);
  }
}

Token Lexer::_lexOpEq2 (
  const char ch,
  const TokenType tt1,
  const TokenType tt2,
  const TokenType tt3,
  const TokenType tt4
) {
  if (this->_reader->eof()) {
    return Token(tt1, ch);
  }

  const auto loc1 = this->_reader->loc();
  const auto ch1 = this->_reader->next();

  if (ch1 == '=') {
    return Token(tt2, std::string{ch} + ch1);
  } else if (ch1 == ch) {
    if (this->_reader->eof()) {
      return Token(tt3, std::string{ch} + ch1);
    }

    const auto loc2 = this->_reader->loc();
    const auto ch2 = this->_reader->next();

    if (ch2 != '=') {
      this->_reader->seek(loc2);
      return Token(tt3, std::string{ch} + ch1);
    } else {
      return Token(tt4, std::string{ch} + ch1 + ch2);
    }
  } else {
    this->_reader->seek(loc1);
    return Token(tt1, ch);
  }
}

Token Lexer::_lexOpEqDouble (
  const char ch,
  const TokenType tt1,
  const TokenType tt2,
  const TokenType tt3
) {
  if (this->_reader->eof()) {
    return Token(tt1, ch);
  }

  const auto loc1 = this->_reader->loc();
  const auto ch1 = this->_reader->next();

  if (ch1 == '=') {
    return Token(tt2, std::string{ch} + ch1);
  } else if (ch1 == ch) {
    return Token(tt3, std::string{ch} + ch1);
  } else {
    this->_reader->seek(loc1);
    return Token(tt1, ch);
  }
}
