/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

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

bool token_is_whitespace (char ch) {
  return strchr("\r\n\t ", ch) != NULL;
}

token_t *token_init (token_type_t type, const char *val, reader_location_t start, reader_location_t end) {
  token_t *token = malloc(sizeof(token_t));

  if (token == NULL) {
    fprintf(stderr, "Unable to allocate memory for token\n");
    exit(EXIT_FAILURE);
  }

  const size_t val_length = strlen(val) + 1;
  token->val = malloc(val_length);

  if (token->val == NULL) {
    fprintf(stderr, "Unable to allocate %lu bytes for token value\n", val_length);
    exit(EXIT_FAILURE);
  }

  token->type = type;
  strcpy(token->val, val);
  token->start = start;
  token->end = end;

  return token;
}

void token_free (token_t *token) {
  free(token->val);
  free(token);
}
