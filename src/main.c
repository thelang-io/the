/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ast.h"
#include "parser.h"

int main () {
  reader_t *reader = reader_init("program.adl");
  ast_t *ast = ast_init();

  while (true) {
    stmt_t *stmt = parser_next(reader);

    if (stmt->type == stmtEnd) {
      stmt_free(stmt);
      break;
    }

    ast_add(ast, stmt);
    stmt_free(stmt);
  }

  ast_free(ast);
  reader_free(reader);

  return 0;
}
