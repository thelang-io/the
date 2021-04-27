/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/litfloat.h"
#include "../utilities.h"

#define LEXER_FSE(t) \
  LEXER_FSS(t "E0", LEXER_LITFLOAT); \
  LEXER_FSS(t "E1", LEXER_LITFLOAT); \
  LEXER_FSS(t "E9", LEXER_LITFLOAT); \
  LEXER_FSS(t "E308", LEXER_LITFLOAT); \
  LEXER_FSS(t "E+0", LEXER_LITFLOAT); \
  LEXER_FSS(t "E+1", LEXER_LITFLOAT); \
  LEXER_FSS(t "E+9", LEXER_LITFLOAT); \
  LEXER_FSS(t "E+308", LEXER_LITFLOAT); \
  LEXER_FSS(t "E-0", LEXER_LITFLOAT); \
  LEXER_FSS(t "E-1", LEXER_LITFLOAT); \
  LEXER_FSS(t "E-9", LEXER_LITFLOAT); \
  LEXER_FSS(t "E-308", LEXER_LITFLOAT); \
  LEXER_FSS(t "e0", LEXER_LITFLOAT); \
  LEXER_FSS(t "e1", LEXER_LITFLOAT); \
  LEXER_FSS(t "e9", LEXER_LITFLOAT); \
  LEXER_FSS(t "e308", LEXER_LITFLOAT); \
  LEXER_FSS(t "e+0", LEXER_LITFLOAT); \
  LEXER_FSS(t "e+1", LEXER_LITFLOAT); \
  LEXER_FSS(t "e+9", LEXER_LITFLOAT); \
  LEXER_FSS(t "e+308", LEXER_LITFLOAT); \
  LEXER_FSS(t "e-0", LEXER_LITFLOAT); \
  LEXER_FSS(t "e-1", LEXER_LITFLOAT); \
  LEXER_FSS(t "e-9", LEXER_LITFLOAT); \
  LEXER_FSS(t "e-308", LEXER_LITFLOAT)

#define LEXER_LITFLOAT_F(t) \
  LEXER_FSS(t, LEXER_LITFLOAT); \
  LEXER_FSE(t)

DUC_TEST(lexer_litfloat, works) {
  LEXER_FSE("0");
  LEXER_FSE("1");
  LEXER_FSE("9");
  LEXER_FSE("9223372036854775807");

  LEXER_LITFLOAT_F("0.");
  LEXER_LITFLOAT_F("1.");
  LEXER_LITFLOAT_F("9.");
  LEXER_LITFLOAT_F("9223372036854775807.");

  LEXER_LITFLOAT_F(".0");
  LEXER_LITFLOAT_F(".1");
  LEXER_LITFLOAT_F(".9");
  LEXER_LITFLOAT_F(".9223372036854775807");

  LEXER_LITFLOAT_F("0.0");
  LEXER_LITFLOAT_F("0.1");
  LEXER_LITFLOAT_F("0.9");
  LEXER_LITFLOAT_F("0.9223372036854775807");
  LEXER_LITFLOAT_F("1.0");
  LEXER_LITFLOAT_F("1.1");
  LEXER_LITFLOAT_F("1.9");
  LEXER_LITFLOAT_F("1.9223372036854775807");
  LEXER_LITFLOAT_F("9.0");
  LEXER_LITFLOAT_F("9.1");
  LEXER_LITFLOAT_F("9.9");
  LEXER_LITFLOAT_F("9.9223372036854775807");
  LEXER_LITFLOAT_F("9223372036854775807.0");
  LEXER_LITFLOAT_F("9223372036854775807.1");
  LEXER_LITFLOAT_F("9223372036854775807.9");
  LEXER_LITFLOAT_F("9223372036854775807.9223372036854775807");
}

int main () {
  DUC_TEST_RUN(lexer_litfloat, works);
}
