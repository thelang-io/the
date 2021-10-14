/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TOKEN_H
#define SRC_TOKEN_H

#include "reader.h"

typedef struct token_s token_t;

typedef enum {
  eof,
  whitespace,

  commentBlock,
  commentLine,

  kwAs,
  kwAsSafe,
  kwAsync,
  kwAwait,
  kwBreak,
  kwCase,
  kwCatch,
  kwClass,
  kwConst,
  kwContinue,
  kwDefault,
  kwDeinit,
  kwElif,
  kwElse,
  kwEnum,
  kwExport,
  kwFallthrough,
  kwFalse,
  kwFn,
  kwFrom,
  kwIf,
  kwImport,
  kwIn,
  kwInit,
  kwInterface,
  kwIs,
  kwLoop,
  kwMain,
  kwMatch,
  kwMut,
  kwNew,
  kwNil,
  kwObj,
  kwOp,
  kwOverride,
  kwPriv,
  kwProt,
  kwPub,
  kwReturn,
  kwStatic,
  kwSuper,
  kwThis,
  kwThrow,
  kwTrue,
  kwTry,
  kwUnion,

  litChar,
  litFloat,
  litId,
  litIntBin,
  litIntDec,
  litIntHex,
  litIntOct,
  litStr,

  opAnd,
  opAndAnd,
  opAndAndEq,
  opAndEq,
  opCaret,
  opCaretEq,
  opColon,
  opColonEq,
  opComma,
  opDot,
  opDotDot,
  opDotDotDot,
  opDotDotEq,
  opEq,
  opEqEq,
  opExcl,
  opExclEq,
  opExclExcl,
  opGt,
  opGtEq,
  opLBrace,
  opLBrack,
  opLPar,
  opLShift,
  opLShiftEq,
  opLt,
  opLtEq,
  opMinus,
  opMinusEq,
  opMinusMinus,
  opOr,
  opOrEq,
  opOrOr,
  opOrOrEq,
  opPercent,
  opPercentEq,
  opPlus,
  opPlusEq,
  opPlusPlus,
  opQn,
  opQnDot,
  opQnQn,
  opQnQnEq,
  opRBrace,
  opRBrack,
  opRPar,
  opRShift,
  opRShiftEq,
  opSemi,
  opSlash,
  opSlashEq,
  opStar,
  opStarEq,
  opStarStar,
  opStarStarEq,
  opTilde
} token_type_t;

struct token_s {
  token_type_t type;
  reader_location_t start;
  reader_location_t end;
  char *val;
  size_t val_len;
};

bool token_is_digit (char ch);
bool token_is_lit_char_escape (char ch);
bool token_is_lit_id_continue (char ch);
bool token_is_lit_id_start (char ch);
bool token_is_lit_int_bin (char ch);
bool token_is_lit_int_dec (char ch);
bool token_is_lit_int_hex (char ch);
bool token_is_lit_int_oct (char ch);
bool token_is_lit_str_escape (char ch);
bool token_is_not_newline (char ch);
bool token_is_whitespace (char ch);

token_t *token_init (reader_location_t loc);
void token_free (token_t *this);

#endif
