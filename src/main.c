/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "parser.h"

int main () {
  reader_t *reader = reader_init("program.adl");

  while (true) {
    stmt_t *stmt = parser_next(reader);

    if (stmt->type == stmtEnd) {
      stmt_free(stmt);
      break;
    }

    char *buf = stmt_str(stmt, 0);
    printf("%s\n", buf);
    free(buf);
    stmt_free(stmt);
  }

  reader_free(reader);
  return 0;
}
