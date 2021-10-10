/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "token.h"

const char *token_type[] = {
  #define GEN_TOKEN_STR(x) #x,
  FOREACH_TOKEN(GEN_TOKEN_STR)
  #undef GEN_TOKEN_STR
};

bool token_is_digit (char ch) {
  return strchr("0123456789", ch) != NULL;
}

bool token_is_lit_char_escape (char ch) {
  return strchr("0tnr\"'\\", ch) != NULL;
}

bool token_is_lit_id_continue (char ch) {
  return isalnum(ch) || ch == '_';
}

bool token_is_lit_id_start (char ch) {
  return isalpha(ch) || ch == '_';
}

bool token_is_lit_int_bin (char ch) {
  return ch == '0' || ch == '1';
}

bool token_is_lit_int_dec (char ch) {
  return token_is_digit(ch);
}

bool token_is_lit_int_hex (char ch) {
  return strchr("ABCDEFabcdef0123456789", ch) != NULL;
}

bool token_is_lit_int_oct (char ch) {
  return strchr("01234567", ch) != NULL;
}

bool token_is_lit_str_escape (char ch) {
  return token_is_lit_char_escape(ch) || ch == '{';
}

bool token_is_not_newline (char ch) {
  return ch != '\n';
}

bool token_is_whitespace (char ch) {
  return strchr("\r\n\t ", ch) != NULL;
}

token_t *token_init (reader_location_t loc) {
  token_t *this = malloc(sizeof(token_t));

  if (this == NULL) {
    throw_error("Unable to allocate memory for token");
  }

  this->start = loc;
  this->end = loc;
  this->val_len = 0;
  this->val = malloc(this->val_len + 1);

  if (this->val == NULL) {
    throw_error("Unable to allocate memory for token value");
  }

  return this;
}

void token_free (token_t *this) {
  free(this->val);
  free(this);
}

char *token_str (token_t *this) {
  char *fmt = "%s(%lu:%lu-%lu:%lu): %s";

  size_t len = (size_t) snprintf(
    NULL,
    0,
    fmt,
    token_type[this->type],
    this->start.line,
    this->start.col + 1,
    this->end.line,
    this->end.col + 1,
    this->val
  );

  char *buf = malloc(len + 1);

  sprintf(
    buf,
    fmt,
    token_type[this->type],
    this->start.line,
    this->start.col + 1,
    this->end.line,
    this->end.col + 1,
    this->val
  );

  return buf;
}
