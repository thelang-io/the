/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "var_map.h"

typedef struct codegen_headers_s codegen_headers_t;

struct codegen_headers_s {
  bool stdio;
};

char *codegen_expr (expr_t *expr) {
  if (expr->type == exprLiteral) {
    expr_literal_t *expr_literal = (expr_literal_t *) expr;
    char *buf = malloc(expr_literal->tok->val_len + 1);
    strcpy(buf, expr_literal->tok->val);
    return buf;
  } else {
    char *buf = malloc(1);
    buf[0] = '\0';
    return buf;
  }
}

const char *codegen_expr_fmt (var_map_t *var_map, expr_t *expr) {
  if (expr->type == exprLiteral) {
    expr_literal_t *expr_literal = (expr_literal_t *) expr;

    if (expr_literal->tok->type == litChar) {
      return "%c";
    } else if (expr_literal->tok->type == litFloat) {
      return "%f";
    } else if (expr_literal->tok->type == litId) {
      const var_map_item_t *var = var_map_get(var_map, expr_literal->tok->val);

      if (var == NULL) {
        // TODO throw
      } else if (var->type == varChar) {
        return "%c";
      } else if (var->type == varFloat) {
        return "%f";
      } else if (var->type == varIntDec) {
        return "%lu";
      } else if (var->type == varStr) {
        return "%s";
      }
    } else if (expr_literal->tok->type == litIntDec) {
      return "%lu";
    } else if (expr_literal->tok->type == litStr) {
      return "%s";
    }
  }

  return "%p";
}

var_map_item_type_t codegen_expr_type (var_map_t *var_map, expr_t *expr) {
  if (expr->type == exprLiteral) {
    expr_literal_t *expr_literal = (expr_literal_t *) expr;

    if (expr_literal->tok->type == litChar) {
      return varChar;
    } else if (expr_literal->tok->type == litFloat) {
      return varFloat;
    } else if (expr_literal->tok->type == litId) {
      const var_map_item_t *var = var_map_get(var_map, expr_literal->tok->val);

      if (var == NULL) {
        // TODO throw
      }

      return var->type;
    } else if (expr_literal->tok->type == litIntDec) {
      return varIntDec;
    } else if (expr_literal->tok->type == litStr) {
      return varStr;
    }
  }

  return varUnknown;
}

const char *codegen_expr_type_str (var_map_t *var_map, expr_t *expr) {
  if (expr->type == exprLiteral) {
    expr_literal_t *expr_literal = (expr_literal_t *) expr;

    if (expr_literal->tok->type == litChar) {
      return "const char ";
    } else if (expr_literal->tok->type == litFloat) {
      return "const float ";
    } else if (expr_literal->tok->type == litId) {
      const var_map_item_t *var = var_map_get(var_map, expr_literal->tok->val);

      if (var == NULL) {
        // TODO throw
      } else if (var->type == varChar) {
        return "const char ";
      } else if (var->type == varFloat) {
        return "const float ";
      } else if (var->type == varIntDec) {
        return "const unsigned long ";
      } else if (var->type == varStr) {
        return "const char *";
      }
    } else if (expr_literal->tok->type == litIntDec) {
      return "const unsigned long ";
    } else if (expr_literal->tok->type == litStr) {
      return "const char *";
    }
  }

  return "const void *";
}

char *codegen (ast_t *ast) {
  var_map_t *var_map = var_map_init();
  codegen_headers_t headers = { false };
  size_t main_body_len = 0;
  char *main_body = malloc(1);
  main_body[0] = '\0';

  if (ast->main_present) {
    for (size_t i = 0; i < ast->main_body_len; i++) {
      stmt_t *stmt = ast->main_body[i];

      if (stmt->type == stmtCallExpr) {
        stmt_call_expr_t *stmt_call_expr = (stmt_call_expr_t *) stmt;

        if (strcmp(stmt_call_expr->callee->val, "print") == 0) {
          headers.stdio = true;
          size_t print_buf_len = 0;
          char *print_buf = malloc(print_buf_len + 1);
          print_buf[0] = '\0';

          for (size_t j = 0; j < stmt_call_expr->args_len; j++) {
            char *expr_buf = codegen_expr(stmt_call_expr->args[j]);
            print_buf_len += strlen(expr_buf) + (j == 0 ? 0 : 5) + 2;
            print_buf = realloc(print_buf, print_buf_len + 1);

            if (j != 0) {
              strcat(print_buf, "\" \", ");
            }

            strcat(print_buf, expr_buf);
            strcat(print_buf, ", ");
            free(expr_buf);
          }

          print_buf_len += 4;
          print_buf = realloc(print_buf, print_buf_len + 1);
          strcat(print_buf, "\"\\n\"");

          size_t buf_len = 0;

          for (size_t j = 0; j < stmt_call_expr->args_len; j++) {
            if (j != 0) {
              buf_len += 2;
            }

            buf_len += strlen(codegen_expr_fmt(var_map, stmt_call_expr->args[j]));
          }

          char *buf = malloc(16 + buf_len + print_buf_len + 1);
          strcpy(buf, "printf(\"");

          for (size_t j = 0; j < stmt_call_expr->args_len; j++) {
            if (j != 0) {
              strcat(buf, "%s");
            }

            strcat(buf, codegen_expr_fmt(var_map, stmt_call_expr->args[j]));
          }

          strcat(buf, "%s\", ");
          strcat(buf, print_buf);
          strcat(buf, ");\n");

          main_body_len += strlen(buf);
          main_body = realloc(main_body, main_body_len + 1);
          strcat(main_body, buf);

          free(buf);
          free(print_buf);
        } else {
          // TODO throw
        }
      } else if (stmt->type == stmtShortVarDecl) {
        stmt_short_var_decl_t *stmt_short_var_decl = (stmt_short_var_decl_t *) stmt;
        char *var_name = stmt_short_var_decl->id->val;
        var_map_item_type_t var_type = codegen_expr_type(var_map, stmt_short_var_decl->init);
        const char *var_type_str = codegen_expr_type_str(var_map, stmt_short_var_decl->init);
        char *expr_buf = codegen_expr(stmt_short_var_decl->init);
        const char *fmt = "%s%s = %s;\n";
        size_t buf_len = (size_t) snprintf(NULL, 0, fmt, var_type_str, var_name, expr_buf);
        main_body_len += buf_len;
        char *buf = malloc(buf_len + 1);
        main_body = realloc(main_body, main_body_len + 1);
        sprintf(buf, fmt, var_type_str, var_name, expr_buf);
        strcat(main_body, buf);

        free(buf);
        free(expr_buf);
        var_map_add(var_map, var_type, var_name, stmt_short_var_decl->init);
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

  const char *fmt = "%s\nint main (const int argc, const char **argv) {\n%s}\n";
  size_t code_len = (size_t) snprintf(NULL, 0, fmt, headers_code, main_body);
  char *code = malloc(code_len + 1);
  sprintf(code, fmt, headers_code, main_body);

  free(headers_code);
  free(main_body);
  var_map_free(var_map);

  return code;
}
