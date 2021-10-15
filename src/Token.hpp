/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TOKEN_HPP
#define SRC_TOKEN_HPP

#include <string>
#include "Reader.hpp"

enum TokenType {
  tkEof,
  tkWhitespace,

  tkCommentBlock,
  tkCommentLine,

  tkKwAs,
  tkKwAsSafe,
  tkKwAsync,
  tkKwAwait,
  tkKwBreak,
  tkKwCase,
  tkKwCatch,
  tkKwClass,
  tkKwConst,
  tkKwContinue,
  tkKwDefault,
  tkKwDeinit,
  tkKwElif,
  tkKwElse,
  tkKwEnum,
  tkKwExport,
  tkKwFallthrough,
  tkKwFalse,
  tkKwFn,
  tkKwFrom,
  tkKwIf,
  tkKwImport,
  tkKwIn,
  tkKwInit,
  tkKwInterface,
  tkKwIs,
  tkKwLoop,
  tkKwMain,
  tkKwMatch,
  tkKwMut,
  tkKwNew,
  tkKwNil,
  tkKwObj,
  tkKwOp,
  tkKwOverride,
  tkKwPriv,
  tkKwProt,
  tkKwPub,
  tkKwReturn,
  tkKwStatic,
  tkKwSuper,
  tkKwThis,
  tkKwThrow,
  tkKwTrue,
  tkKwTry,
  tkKwUnion,

  tkLitChar,
  tkLitFloat,
  tkLitId,
  tkLitIntBin,
  tkLitIntDec,
  tkLitIntHex,
  tkLitIntOct,
  tkLitStr,

  tkOpAnd,
  tkOpAndAnd,
  tkOpAndAndEq,
  tkOpAndEq,
  tkOpCaret,
  tkOpCaretEq,
  tkOpColon,
  tkOpColonEq,
  tkOpComma,
  tkOpDot,
  tkOpDotDot,
  tkOpDotDotDot,
  tkOpDotDotEq,
  tkOpEq,
  tkOpEqEq,
  tkOpExcl,
  tkOpExclEq,
  tkOpExclExcl,
  tkOpGt,
  tkOpGtEq,
  tkOpLBrace,
  tkOpLBrack,
  tkOpLPar,
  tkOpLShift,
  tkOpLShiftEq,
  tkOpLt,
  tkOpLtEq,
  tkOpMinus,
  tkOpMinusEq,
  tkOpMinusMinus,
  tkOpOr,
  tkOpOrEq,
  tkOpOrOr,
  tkOpOrOrEq,
  tkOpPercent,
  tkOpPercentEq,
  tkOpPlus,
  tkOpPlusEq,
  tkOpPlusPlus,
  tkOpQn,
  tkOpQnDot,
  tkOpQnQn,
  tkOpQnQnEq,
  tkOpRBrace,
  tkOpRBrack,
  tkOpRPar,
  tkOpRShift,
  tkOpRShiftEq,
  tkOpSemi,
  tkOpSlash,
  tkOpSlashEq,
  tkOpStar,
  tkOpStarEq,
  tkOpStarStar,
  tkOpStarStarEq,
  tkOpTilde
};

class Token {
 public:
  static bool isDigit (char ch);
  static bool isLitCharEscape (char ch);
  static bool isLitIdContinue (char ch);
  static bool isLitIdStart (char ch);
  static bool isLitIntBin (char ch);
  static bool isLitIntDec (char ch);
  static bool isLitIntHex (char ch);
  static bool isLitIntOct (char ch);
  static bool isLitStrEscape (char ch);
  static bool isNotNewline (char ch);
  static bool isWhitespace (char ch);

  const TokenType type;
  const ReaderLocation start;
  ReaderLocation end;
  std::string val;

  Token (TokenType type, ReaderLocation start, ReaderLocation end, const std::string &val);
  bool operator== (TokenType type) const;
  bool operator!= (TokenType type) const;

  std::string str () const;
};

#endif
