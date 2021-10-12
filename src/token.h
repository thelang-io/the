/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_TOKEN_H
#define SRC_TOKEN_H

#include "reader.h"

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

typedef struct token_s token_t;

typedef enum {
  #define GEN_TOKEN_ENUM(x) x,
  FOREACH_TOKEN(GEN_TOKEN_ENUM)
  #undef GEN_TOKEN_ENUM
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
