/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TOKEN_HPP
#define SRC_TOKEN_HPP

#include "Reader.hpp"

enum TokenType {
  tkCommentBlock,
  tkCommentLine,
  tkEof,
  tkWhitespace,

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

struct Token {
  TokenType type;
  ReaderLocation start;
  ReaderLocation end;
  std::string val;
};

inline bool tokenIsDigit (char ch) {
  return std::isdigit(ch);
}

inline bool tokenIsLitCharEscape (char ch) {
  return std::string("0tnr\"'\\").find(ch) != std::string::npos;
}

inline bool tokenIsLitIdContinue (char ch) {
  return std::isalnum(ch) || ch == '_';
}

inline bool tokenIsLitIdStart (char ch) {
  return std::isalpha(ch) || ch == '_';
}

inline bool tokenIsLitIntBin (char ch) {
  return ch == '0' || ch == '1';
}

inline bool tokenIsLitIntDec (char ch) {
  return std::isdigit(ch);
}

inline bool tokenIsLitIntHex (char ch) {
  return std::string("ABCDEFabcdef0123456789").find(ch) != std::string::npos;
}

inline bool tokenIsLitIntOct (char ch) {
  return std::string("01234567").find(ch) != std::string::npos;
}

inline bool tokenIsLitStrEscape (char ch) {
  return tokenIsLitCharEscape(ch) || ch == '{';
}

inline bool tokenIsNotNewline (char ch) {
  return ch != '\n';
}

inline bool tokenIsWhitespace (char ch) {
  return std::isspace(ch);
}

#endif
