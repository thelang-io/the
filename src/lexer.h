/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include "fs.h"
#include "helpers.h"

#define FOREACH_LEXER_TOKEN(f) \
  f(COMMENT_BLOCK) \
  f(COMMENT_LINE) \
  \
  f(KW_AS) \
  f(KW_AS_SAFE) \
  f(KW_ASYNC) \
  f(KW_AWAIT) \
  f(KW_BREAK) \
  f(KW_CASE) \
  f(KW_CATCH) \
  f(KW_CLASS) \
  f(KW_CONST) \
  f(KW_CONTINUE) \
  f(KW_DEFAULT) \
  f(KW_DEINIT) \
  f(KW_ELIF) \
  f(KW_ELSE) \
  f(KW_ENUM) \
  f(KW_EXPORT) \
  f(KW_FALLTHROUGH) \
  f(KW_FALSE) \
  f(KW_FN) \
  f(KW_FROM) \
  f(KW_IF) \
  f(KW_IMPORT) \
  f(KW_IN) \
  f(KW_INIT) \
  f(KW_INTERFACE) \
  f(KW_IS) \
  f(KW_LOOP) \
  f(KW_MAIN) \
  f(KW_MATCH) \
  f(KW_MUT) \
  f(KW_NEW) \
  f(KW_NIL) \
  f(KW_OBJ) \
  f(KW_OP) \
  f(KW_OVERRIDE) \
  f(KW_PRIV) \
  f(KW_PROT) \
  f(KW_PUB) \
  f(KW_RETURN) \
  f(KW_STATIC) \
  f(KW_SUPER) \
  f(KW_THIS) \
  f(KW_THROW) \
  f(KW_TRUE) \
  f(KW_TRY) \
  f(KW_UNION) \
  \
  f(LIT_CHAR) \
  f(LIT_FLOAT) \
  f(LIT_ID) \
  f(LIT_INT_BIN) \
  f(LIT_INT_DEC) \
  f(LIT_INT_HEX) \
  f(LIT_INT_OCT) \
  f(LIT_STR) \
  \
  f(OP_AND) \
  f(OP_ANDAND) \
  f(OP_ANDANDEQ) \
  f(OP_ANDEQ) \
  f(OP_CARET) \
  f(OP_CARETEQ) \
  f(OP_COLON) \
  f(OP_COLONEQ) \
  f(OP_COMMA) \
  f(OP_DOT) \
  f(OP_DOTDOT) \
  f(OP_DOTDOTDOT) \
  f(OP_DOTDOTEQ) \
  f(OP_EQ) \
  f(OP_EQEQ) \
  f(OP_EXCL) \
  f(OP_EXCLEQ) \
  f(OP_EXCLEXCL) \
  f(OP_GT) \
  f(OP_GTEQ) \
  f(OP_LBRACE) \
  f(OP_LBRACK) \
  f(OP_LPAR) \
  f(OP_LSHIFT) \
  f(OP_LSHIFTEQ) \
  f(OP_LT) \
  f(OP_LTEQ) \
  f(OP_MINUS) \
  f(OP_MINUSEQ) \
  f(OP_MINUSMINUS) \
  f(OP_OR) \
  f(OP_OREQ) \
  f(OP_OROR) \
  f(OP_OROREQ) \
  f(OP_PERCENT) \
  f(OP_PERCENTEQ) \
  f(OP_PLUS) \
  f(OP_PLUSEQ) \
  f(OP_PLUSPLUS) \
  f(OP_QN) \
  f(OP_QNDOT) \
  f(OP_QNQN) \
  f(OP_QNQNEQ) \
  f(OP_RBRACE) \
  f(OP_RBRACK) \
  f(OP_RPAR) \
  f(OP_RSHIFT) \
  f(OP_RSHIFTEQ) \
  f(OP_SEMI) \
  f(OP_SLASH) \
  f(OP_SLASHEQ) \
  f(OP_STAR) \
  f(OP_STAREQ) \
  f(OP_STARSTAR) \
  f(OP_STARSTAREQ) \
  f(OP_TILDE) \
  \
  f(UNKNOWN) \
  f(WS)

#define GEN_LEXER_TOKEN_ENUM(x) LEXER_##x,

typedef struct lexer_s lexer_t;

typedef enum {
  FOREACH_LEXER_TOKEN(GEN_LEXER_TOKEN_ENUM)
} lexer_token;

struct lexer_s {
  unsigned char *raw;
  unsigned char *str;
  lexer_token token;
};

extern const char *lexer_token_str[];

void lexer_free (lexer_t *lexer);
void lexer_free_cb (void *it);
lexer_token lexer_lookup (file_t *file);
lexer_t *lexer_new (file_t *file);

#endif
