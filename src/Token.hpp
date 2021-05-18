/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TOKEN_HPP
#define SRC_TOKEN_HPP

#include "Reader.hpp"

#define FOREACH_TOKEN(fn) \
  fn(eof) \
  fn(whitespace) \
  \
  fn(commentBlock) \
  fn(commentLine) \
  \
  fn(kwAs) \
  fn(kwAsSafe) \
  fn(kwAsync) \
  fn(kwAwait) \
  fn(kwBreak) \
  fn(kwCase) \
  fn(kwCatch) \
  fn(kwClass) \
  fn(kwConst) \
  fn(kwContinue) \
  fn(kwDefault) \
  fn(kwDeinit) \
  fn(kwElif) \
  fn(kwElse) \
  fn(kwEnum) \
  fn(kwExport) \
  fn(kwFallthrough) \
  fn(kwFalse) \
  fn(kwFn) \
  fn(kwFrom) \
  fn(kwIf) \
  fn(kwImport) \
  fn(kwIn) \
  fn(kwInit) \
  fn(kwInterface) \
  fn(kwIs) \
  fn(kwLoop) \
  fn(kwMain) \
  fn(kwMatch) \
  fn(kwMut) \
  fn(kwNew) \
  fn(kwNil) \
  fn(kwObj) \
  fn(kwOp) \
  fn(kwOverride) \
  fn(kwPriv) \
  fn(kwProt) \
  fn(kwPub) \
  fn(kwReturn) \
  fn(kwStatic) \
  fn(kwSuper) \
  fn(kwThis) \
  fn(kwThrow) \
  fn(kwTrue) \
  fn(kwTry) \
  fn(kwUnion) \
  \
  fn(litChar) \
  fn(litFloat) \
  fn(litId) \
  fn(litIntBin) \
  fn(litIntDec) \
  fn(litIntHex) \
  fn(litIntOct) \
  fn(litStr) \
  \
  fn(opAnd) \
  fn(opAndAnd) \
  fn(opAndAndEq) \
  fn(opAndEq) \
  fn(opCaret) \
  fn(opCaretEq) \
  fn(opColon) \
  fn(opColonEq) \
  fn(opComma) \
  fn(opDot) \
  fn(opDotDot) \
  fn(opDotDotDot) \
  fn(opDotDotEq) \
  fn(opEq) \
  fn(opEqEq) \
  fn(opExcl) \
  fn(opExclEq) \
  fn(opExclExcl) \
  fn(opGt) \
  fn(opGtEq) \
  fn(opLBrace) \
  fn(opLBrack) \
  fn(opLPar) \
  fn(opLShift) \
  fn(opLShiftEq) \
  fn(opLt) \
  fn(opLtEq) \
  fn(opMinus) \
  fn(opMinusEq) \
  fn(opMinusMinus) \
  fn(opOr) \
  fn(opOrEq) \
  fn(opOrOr) \
  fn(opOrOrEq) \
  fn(opPercent) \
  fn(opPercentEq) \
  fn(opPlus) \
  fn(opPlusEq) \
  fn(opPlusPlus) \
  fn(opQn) \
  fn(opQnDot) \
  fn(opQnQn) \
  fn(opQnQnEq) \
  fn(opRBrace) \
  fn(opRBrack) \
  fn(opRPar) \
  fn(opRShift) \
  fn(opRShiftEq) \
  fn(opSemi) \
  fn(opSlash) \
  fn(opSlashEq) \
  fn(opStar) \
  fn(opStarEq) \
  fn(opStarStar) \
  fn(opStarStarEq) \
  fn(opTilde)

enum TokenType {
  #define GEN_TOKEN_ENUM(x) x,
  FOREACH_TOKEN(GEN_TOKEN_ENUM)
  #undef GEN_TOKEN_ENUM
};

class Token {
 public:
  static bool isIdContinue (const char ch);
  static bool isIdStart (const char ch);
  static bool isWhitespace (const char ch);

  const TokenType type;
  const std::string val;
  const ReaderLocation start;
  const ReaderLocation end;

  Token (
    const TokenType &t,
    const std::string &v,
    const ReaderLocation &s,
    const ReaderLocation &e
  );

  bool operator== (const TokenType &rhs) const;
  bool operator!= (const TokenType &rhs) const;

  std::string str () const;
};

#endif
