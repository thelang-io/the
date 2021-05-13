/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TOKEN_HPP
#define SRC_TOKEN_HPP

#include <string>

#define FOREACH_TOKEN(fn) \
  fn(eof) \
  fn(unknown) \
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
  fn(opAndand) \
  fn(opAndandeq) \
  fn(opAndeq) \
  fn(opCaret) \
  fn(opCareteq) \
  fn(opColon) \
  fn(opColoneq) \
  fn(opComma) \
  fn(opDot) \
  fn(opDotdot) \
  fn(opDotdotdot) \
  fn(opDotdoteq) \
  fn(opEq) \
  fn(opEqeq) \
  fn(opExcl) \
  fn(opExcleq) \
  fn(opExclexcl) \
  fn(opGt) \
  fn(opGteq) \
  fn(opLbrace) \
  fn(opLbrack) \
  fn(opLpar) \
  fn(opLshift) \
  fn(opLshifteq) \
  fn(opLt) \
  fn(opLteq) \
  fn(opMinus) \
  fn(opMinuseq) \
  fn(opMinusminus) \
  fn(opOr) \
  fn(opOreq) \
  fn(opOror) \
  fn(opOroreq) \
  fn(opPercent) \
  fn(opPercenteq) \
  fn(opPlus) \
  fn(opPluseq) \
  fn(opPlusplus) \
  fn(opQn) \
  fn(opQndot) \
  fn(opQnqn) \
  fn(opQnqneq) \
  fn(opRbrace) \
  fn(opRbrack) \
  fn(opRpar) \
  fn(opRshift) \
  fn(opRshifteq) \
  fn(opSemi) \
  fn(opSlash) \
  fn(opSlasheq) \
  fn(opStar) \
  fn(opStareq) \
  fn(opStarstar) \
  fn(opStarstareq) \
  fn(opTilde)

enum TokenType {
  #define GEN_TOKEN_ENUM(x) x,
  FOREACH_TOKEN(GEN_TOKEN_ENUM)
  #undef GEN_TOKEN_ENUM
};

class Token {
 public:
  static bool isWhitespace (char ch);

  const TokenType type;
  const std::string val;

  Token (const TokenType type, const std::string &val);
  bool operator== (const TokenType rhs) const;
  bool operator!= (const TokenType rhs) const;

  std::string str () const;
};

#endif
