/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cctype>
#include <utility>
#include "Token.hpp"

bool Token::isDigit (char ch) {
  return std::isdigit(ch);
}

bool Token::isLitCharEscape (char ch) {
  return std::string("0tnr\"'\\").find(ch) != std::string::npos;
}

bool Token::isLitIdContinue (char ch) {
  return std::isalnum(ch) || ch == '_';
}

bool Token::isLitIdStart (char ch) {
  return std::isalpha(ch) || ch == '_';
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

Token::Token (TokenType t, ReaderLocation s, ReaderLocation e, const std::string &v) : type(t), start(s) {
  this->end = e;
  this->val = v;
}

bool Token::operator== (TokenType t) const {
  return this->type == t;
}

bool Token::operator!= (TokenType t) const {
  return this->type == t;
}

std::string Token::str () const {
  if (this->type == tkWhitespace) {
    return "whitespace";
  } else if (this->type == tkCommentBlock) {
    return "commentBlock";
  } else if (this->type == tkCommentLine) {
    return "commentLine";
  } else if (this->type == tkKwAs) {
    return "kwAs";
  } else if (this->type == tkKwAsSafe) {
    return "kwAsSafe";
  } else if (this->type == tkKwAsync) {
    return "kwAsync";
  } else if (this->type == tkKwAwait) {
    return "kwAwait";
  } else if (this->type == tkKwBreak) {
    return "kwBreak";
  } else if (this->type == tkKwCase) {
    return "kwCase";
  } else if (this->type == tkKwCatch) {
    return "kwCatch";
  } else if (this->type == tkKwClass) {
    return "kwClass";
  } else if (this->type == tkKwConst) {
    return "kwConst";
  } else if (this->type == tkKwContinue) {
    return "kwContinue";
  } else if (this->type == tkKwDefault) {
    return "kwDefault";
  } else if (this->type == tkKwDeinit) {
    return "kwDeinit";
  } else if (this->type == tkKwElif) {
    return "kwElif";
  } else if (this->type == tkKwElse) {
    return "kwElse";
  } else if (this->type == tkKwEnum) {
    return "kwEnum";
  } else if (this->type == tkKwExport) {
    return "kwExport";
  } else if (this->type == tkKwFallthrough) {
    return "kwFallthrough";
  } else if (this->type == tkKwFalse) {
    return "kwFalse";
  } else if (this->type == tkKwFn) {
    return "kwFn";
  } else if (this->type == tkKwFrom) {
    return "kwFrom";
  } else if (this->type == tkKwIf) {
    return "kwIf";
  } else if (this->type == tkKwImport) {
    return "kwImport";
  } else if (this->type == tkKwIn) {
    return "kwIn";
  } else if (this->type == tkKwInit) {
    return "kwInit";
  } else if (this->type == tkKwInterface) {
    return "kwInterface";
  } else if (this->type == tkKwIs) {
    return "kwIs";
  } else if (this->type == tkKwLoop) {
    return "kwLoop";
  } else if (this->type == tkKwMain) {
    return "kwMain";
  } else if (this->type == tkKwMatch) {
    return "kwMatch";
  } else if (this->type == tkKwMut) {
    return "kwMut";
  } else if (this->type == tkKwNew) {
    return "kwNew";
  } else if (this->type == tkKwNil) {
    return "kwNil";
  } else if (this->type == tkKwObj) {
    return "kwObj";
  } else if (this->type == tkKwOp) {
    return "kwOp";
  } else if (this->type == tkKwOverride) {
    return "kwOverride";
  } else if (this->type == tkKwPriv) {
    return "kwPriv";
  } else if (this->type == tkKwProt) {
    return "kwProt";
  } else if (this->type == tkKwPub) {
    return "kwPub";
  } else if (this->type == tkKwReturn) {
    return "kwReturn";
  } else if (this->type == tkKwStatic) {
    return "kwStatic";
  } else if (this->type == tkKwSuper) {
    return "kwSuper";
  } else if (this->type == tkKwThis) {
    return "kwThis";
  } else if (this->type == tkKwThrow) {
    return "kwThrow";
  } else if (this->type == tkKwTrue) {
    return "kwTrue";
  } else if (this->type == tkKwTry) {
    return "kwTry";
  } else if (this->type == tkKwUnion) {
    return "kwUnion";
  } else if (this->type == tkLitChar) {
    return "litChar";
  } else if (this->type == tkLitFloat) {
    return "litFloat";
  } else if (this->type == tkLitId) {
    return "litId";
  } else if (this->type == tkLitIntBin) {
    return "litIntBin";
  } else if (this->type == tkLitIntDec) {
    return "litIntDec";
  } else if (this->type == tkLitIntHex) {
    return "litIntHex";
  } else if (this->type == tkLitIntOct) {
    return "litIntOct";
  } else if (this->type == tkLitStr) {
    return "litStr";
  } else if (this->type == tkOpAnd) {
    return "opAnd";
  } else if (this->type == tkOpAndAnd) {
    return "opAndAnd";
  } else if (this->type == tkOpAndAndEq) {
    return "opAndAndEq";
  } else if (this->type == tkOpAndEq) {
    return "opAndEq";
  } else if (this->type == tkOpCaret) {
    return "opCaret";
  } else if (this->type == tkOpCaretEq) {
    return "opCaretEq";
  } else if (this->type == tkOpColon) {
    return "opColon";
  } else if (this->type == tkOpColonEq) {
    return "opColonEq";
  } else if (this->type == tkOpComma) {
    return "opComma";
  } else if (this->type == tkOpDot) {
    return "opDot";
  } else if (this->type == tkOpDotDot) {
    return "opDotDot";
  } else if (this->type == tkOpDotDotDot) {
    return "opDotDotDot";
  } else if (this->type == tkOpDotDotEq) {
    return "opDotDotEq";
  } else if (this->type == tkOpEq) {
    return "opEq";
  } else if (this->type == tkOpEqEq) {
    return "opEqEq";
  } else if (this->type == tkOpExcl) {
    return "opExcl";
  } else if (this->type == tkOpExclEq) {
    return "opExclEq";
  } else if (this->type == tkOpExclExcl) {
    return "opExclExcl";
  } else if (this->type == tkOpGt) {
    return "opGt";
  } else if (this->type == tkOpGtEq) {
    return "opGtEq";
  } else if (this->type == tkOpLBrace) {
    return "opLBrace";
  } else if (this->type == tkOpLBrack) {
    return "opLBrack";
  } else if (this->type == tkOpLPar) {
    return "opLPar";
  } else if (this->type == tkOpLShift) {
    return "opLShift";
  } else if (this->type == tkOpLShiftEq) {
    return "opLShiftEq";
  } else if (this->type == tkOpLt) {
    return "opLt";
  } else if (this->type == tkOpLtEq) {
    return "opLtEq";
  } else if (this->type == tkOpMinus) {
    return "opMinus";
  } else if (this->type == tkOpMinusEq) {
    return "opMinusEq";
  } else if (this->type == tkOpMinusMinus) {
    return "opMinusMinus";
  } else if (this->type == tkOpOr) {
    return "opOr";
  } else if (this->type == tkOpOrEq) {
    return "opOrEq";
  } else if (this->type == tkOpOrOr) {
    return "opOrOr";
  } else if (this->type == tkOpOrOrEq) {
    return "opOrOrEq";
  } else if (this->type == tkOpPercent) {
    return "opPercent";
  } else if (this->type == tkOpPercentEq) {
    return "opPercentEq";
  } else if (this->type == tkOpPlus) {
    return "opPlus";
  } else if (this->type == tkOpPlusEq) {
    return "opPlusEq";
  } else if (this->type == tkOpPlusPlus) {
    return "opPlusPlus";
  } else if (this->type == tkOpQn) {
    return "opQn";
  } else if (this->type == tkOpQnDot) {
    return "opQnDot";
  } else if (this->type == tkOpQnQn) {
    return "opQnQn";
  } else if (this->type == tkOpQnQnEq) {
    return "opQnQnEq";
  } else if (this->type == tkOpRBrace) {
    return "opRBrace";
  } else if (this->type == tkOpRBrack) {
    return "opRBrack";
  } else if (this->type == tkOpRPar) {
    return "opRPar";
  } else if (this->type == tkOpRShift) {
    return "opRShift";
  } else if (this->type == tkOpRShiftEq) {
    return "opRShiftEq";
  } else if (this->type == tkOpSemi) {
    return "opSemi";
  } else if (this->type == tkOpSlash) {
    return "opSlash";
  } else if (this->type == tkOpSlashEq) {
    return "opSlashEq";
  } else if (this->type == tkOpStar) {
    return "opStar";
  } else if (this->type == tkOpStarEq) {
    return "opStarEq";
  } else if (this->type == tkOpStarStar) {
    return "opStarStar";
  } else if (this->type == tkOpStarStarEq) {
    return "opStarStarEq";
  } else if (this->type == tkOpTilde) {
    return "opTilde";
  } else {
    return "eof";
  }
}
