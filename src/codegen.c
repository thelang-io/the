/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "codegen.h"

typedef struct codegen_headers_s codegen_headers_t;

struct codegen_headers_s {
  bool stdio;
};

char *codegen (ast_t *ast) {
  codegen_headers_t headers = { false };
  size_t main_body_len = 0;
  char *main_body = malloc(1);
  main_body[0] = '\0';

  if (ast->main_present) {
    for (size_t i = 0; i < ast->main_body_len; i++) {
      stmt_t *stmt = ast->main_body[i];

      if (stmt->type == stmtCallExpr) {
        stmt_call_expr_t *call_expr_stmt = (stmt_call_expr_t *) stmt;

        if (strcmp(call_expr_stmt->callee->val, "print") == 0) {
          headers.stdio = true;

          for (size_t j = 0; j < call_expr_stmt->args_len; j++) {
            expr_t *expr = call_expr_stmt->args[j];

            if (expr->type == exprLiteral) {
              expr_literal_t *literal_expr = (expr_literal_t *) expr;

              const char *fmt = "printf(%s);\n";
              size_t buf_len = (size_t) snprintf(NULL, 0, fmt, literal_expr->tok->val);
              main_body_len += buf_len;
              char *buf = malloc(buf_len + 1);
              main_body = realloc(main_body, main_body_len + 1);
              sprintf(buf, fmt, literal_expr->tok->val);
              strcat(main_body, buf);
              free(buf);
            } else {
              // TODO throw
            }
          }
        } else {
          // TODO throw
        }
      } else {
        // TODO throw
      }
    }
  }

  const char *fmt_headers = "%s";
  const char *header_stdio = headers.stdio ? "#include <stdio.h>\n" : "";
  size_t headers_len = (size_t) snprintf(NULL, 0, fmt_headers, header_stdio);
  char *headers_code = malloc(headers_len + 1);
  sprintf(headers_code, fmt_headers, header_stdio);

  const char *fmt = "%s\nint main (int argc, char **argv) {\n%s}\n";
  size_t code_len = (size_t) snprintf(NULL, 0, fmt, headers_code, main_body);
  char *code = malloc(code_len + 1);
  sprintf(code, fmt, headers_code, main_body);

  free(headers_code);
  free(main_body);

  return code;
}
