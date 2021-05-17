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

  auto ch = this->_reader->next();
  this->_val = ch;

  if (ch == '^') return this->_opEq(opCaret, opCaretEq);
  if (ch == ':') return this->_opEq(opColon, opColonEq);
  if (ch == ',') return this->_token(opComma);
  if (ch == '=') return this->_opEq(opEq, opEqEq);
  if (ch == '{') return this->_token(opLBrace);
  if (ch == '[') return this->_token(opLBrack);
  if (ch == '(') return this->_token(opLPar);
  if (ch == '%') return this->_opEq(opPercent, opPercentEq);
  if (ch == '}') return this->_token(opRBrace);
  if (ch == ']') return this->_token(opRBrack);
  if (ch == ')') return this->_token(opRPar);
  if (ch == ';') return this->_token(opSemi);
  if (ch == '~') return this->_token(opTilde);

  if (ch == '&') {
    return this->_opEq2('&', opAnd, opAndEq, opAndAnd, opAndAndEq);
  } else if (ch == '!') {
    return this->_opEqDouble('!', opExcl, opExclEq, opExclExcl);
  } else if (ch == '>') {
    return this->_opEq2('>', opGt, opGtEq, opRShift, opRShiftEq);
  } else if (ch == '<') {
    return this->_opEq2('<', opLt, opLtEq, opLShift, opLShiftEq);
  } else if (ch == '-') {
    return this->_opEqDouble('-', opMinus, opMinusEq, opMinusMinus);
  } else if (ch == '|') {
    return this->_opEq2('|', opOr, opOrEq, opOrOr, opOrOrEq);
  } else if (ch == '+') {
    return this->_opEqDouble('+', opPlus, opPlusEq, opPlusPlus);
  } else if (ch == '/') {
    if (this->_reader->eof()) {
      return this->_token(opSlash);
    }

    const auto loc1 = this->_reader->loc();
    const auto ch1 = this->_reader->next();

    if (ch1 == '=') {
      this->_val += ch1;
      return this->_token(opSlashEq);
    } else if (ch1 != '/' && ch1 != '*') {
      this->_val += ch1;
      return this->_token(opSlash);
    }

    this->_reader->seek(loc1);
  } else if (ch == '?') {
    if (this->_reader->eof()) {
      return this->_token(opQn);
    }

    const auto loc1 = this->_reader->loc();
    const auto ch1 = this->_reader->next();

    if (ch1 == '.') {
      this->_val += ch1;
      return this->_token(opQnDot);
    } else if (ch1 == ch) {
      this->_val += ch1;

      if (this->_reader->eof()) {
        return this->_token(opQnQn);
      }

      const auto loc2 = this->_reader->loc();
      const auto ch2 = this->_reader->next();

      if (ch2 == '=') {
        this->_val += ch2;
        return this->_token(opQnQnEq);
      } else {
        this->_reader->seek(loc2);
        return this->_token(opQnQn);
      }
    } else {
      this->_reader->seek(loc1);
      return this->_token(opQn);
    }
  } else if (ch == '*') {
    return this->_opEq2('*', opStar, opStarEq, opStarStar, opStarStarEq);
  }

  if (Token::isWhitespace(ch)) {
    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isWhitespace(ch)) {
        this->_reader->seek(loc);
        break;
      }

      this->_val += ch;
    }

    return this->_token(whitespace);
  } else if (Token::isIdStart(ch)) {
    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isIdContinue(ch)) {
        this->_reader->seek(loc);
        break;
      }

      this->_val += ch;
    }

    if (this->_val == "as") {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (ch == '?') {
        this->_val += ch;
        return this->_token(kwAsSafe);
      } else {
        this->_reader->seek(loc);
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
  }

  throw SyntaxError(this->_reader, "Unexpected token");
}

Token Lexer::_opEq (const TokenType tt1, const TokenType tt2) {
  if (this->_reader->eof()) {
    return this->_token(tt1);
  }

  const auto loc1 = this->_reader->loc();
  const auto ch1 = this->_reader->next();

  if (ch1 == '=') {
    this->_val += ch1;
    return this->_token(tt2);
  } else {
    this->_reader->seek(loc1);
    return this->_token(tt1);
  }
}

Token Lexer::_opEq2 (
  const char ch,
  const TokenType tt1,
  const TokenType tt2,
  const TokenType tt3,
  const TokenType tt4
) {
  if (this->_reader->eof()) {
    return this->_token(tt1);
  }

  const auto loc1 = this->_reader->loc();
  const auto ch1 = this->_reader->next();

  if (ch1 == '=') {
    this->_val += ch1;
    return this->_token(tt2);
  } else if (ch1 == ch) {
    this->_val += ch1;

    if (this->_reader->eof()) {
      return this->_token(tt3);
    }

    const auto loc2 = this->_reader->loc();
    const auto ch2 = this->_reader->next();

    if (ch2 == '=') {
      this->_val += ch2;
      return this->_token(tt4);
    } else {
      this->_reader->seek(loc2);
      return this->_token(tt3);
    }
  } else {
    this->_reader->seek(loc1);
    return this->_token(tt1);
  }
}

Token Lexer::_opEqDouble (
  const char ch,
  const TokenType tt1,
  const TokenType tt2,
  const TokenType tt3
) {
  if (this->_reader->eof()) {
    return this->_token(tt1);
  }

  const auto loc1 = this->_reader->loc();
  const auto ch1 = this->_reader->next();

  if (ch1 == '=') {
    this->_val += ch1;
    return this->_token(tt2);
  } else if (ch1 == ch) {
    this->_val += ch1;
    return this->_token(tt3);
  } else {
    this->_reader->seek(loc1);
    return this->_token(tt1);
  }
}

Token Lexer::_token (const TokenType tt) {
  this->_end = this->_reader->loc();
  return Token(tt, this->_val, this->_start, this->_end);
}
