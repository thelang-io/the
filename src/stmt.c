/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "stmt.h"

const char *stmt_type[] = {
  #define GEN_STMT_STR(x) #x,
  FOREACH_STMT(GEN_STMT_STR)
  #undef GEN_STMT_STR
};

stmt_t *stmt_init (stmt_type_t type, reader_location_t start, reader_location_t end) {
  stmt_t *this;

  if (type == stmtMain) {
    this = malloc(sizeof(stmt_main_t));
  } else if (type == stmtCallExpr) {
    this = malloc(sizeof(stmt_call_expr_t));
  } else {
    this = malloc(sizeof(stmt_t));
  }

  if (this == NULL) {
    throw_error("Unable to allocate memory for stmt");
  }

  this->type = type;
  this->start = start;
  this->end = end;

  return this;
}

void stmt_free (stmt_t *this) {
  if (this->type == stmtCallExpr) {
    stmt_call_expr_t *stmt_call_expr = (stmt_call_expr_t *) this;

    for (size_t i = 0; i < stmt_call_expr->args_len; i++) {
      expr_free(stmt_call_expr->args[i]);
    }

    token_free(stmt_call_expr->callee);
    free(stmt_call_expr->args);
  } else if (this->type == stmtMain) {
    stmt_main_t *stmt_main = (stmt_main_t *) this;

    for (size_t i = 0; i < stmt_main->body_len; i++) {
      stmt_free(stmt_main->body[i]);
    }

    free(stmt_main->body);
  }

  free(this);
}

char *stmt_str (stmt_t *this, size_t indent) {
  char *indent_spaces = malloc(indent + 1);
  char *indent_inside_spaces = malloc(indent + 3);
  for (size_t i = 0; i < indent; i++) indent_spaces[i] = ' ';
  for (size_t i = 0; i < indent + 2; i++) indent_inside_spaces[i] = ' ';
  indent_spaces[indent] = '\0';
  indent_inside_spaces[indent + 2] = '\0';

  if (this->type == stmtCallExpr) {
    stmt_call_expr_t *stmt_call_expr = (stmt_call_expr_t *) this;
    char *fmt = "%s<call_expr callee=\"%s\">%s%s%s</call_expr>";
    char *indent_start = stmt_call_expr->args_len == 0 ? "" : "\n";
    char *indent_end = stmt_call_expr->args_len == 0 ? "" : indent_spaces;
    size_t content_len = 0;
    char *content = malloc(content_len + 1);
    content[0] = '\0';

    for (size_t i = 0; i < stmt_call_expr->args_len; i++) {
      expr_t *expr = stmt_call_expr->args[i];

      if (expr->type == exprLiteral) {
        expr_literal_t *literalExpr = (expr_literal_t *) expr;
        char *expr_fmt = "%s<arg>%s</arg>\n";
        content_len += (size_t) snprintf(NULL, 0, expr_fmt, indent_inside_spaces, literalExpr->tok->val);
        content = realloc(content, content_len + 1);
        strcat(content, indent_inside_spaces);
        strcat(content, "<arg>");
        strcat(content, literalExpr->tok->val);
        strcat(content, "</arg>\n");
      } else {
        content_len += indent + 23;
        content = realloc(content, content_len + 1);
        strcat(content, indent_inside_spaces);
        strcat(content, "%s<arg>unknown</arg>\n");
      }
    }

    size_t len = (size_t) snprintf(
      NULL,
      0,
      fmt,
      indent_spaces,
      stmt_call_expr->callee->val,
      indent_start,
      content,
      indent_end
    );

    char *buf = malloc(len + 1);
    sprintf(buf, fmt, indent_spaces, stmt_call_expr->callee->val, indent_start, content, indent_end);

    free(content);
    free(indent_spaces);
    free(indent_inside_spaces);

    return buf;
  } else if (this->type == stmtMain) {
    stmt_main_t *stmt_main = (stmt_main_t *) this;
    char *fmt = "%s<main>%s%s%s</main>";
    char *indent_start = stmt_main->body_len == 0 ? "" : "\n";
    char *indent_end = stmt_main->body_len == 0 ? "" : indent_spaces;
    size_t content_len = 0;
    char *content = malloc(content_len + 1);
    content[0] = '\0';

    for (size_t i = 0; i < stmt_main->body_len; i++) {
      char *stmt_buf = stmt_str(stmt_main->body[i], indent + 2);
      content_len += strlen(stmt_buf) + 1;
      content = realloc(content, content_len + 1);
      strcat(content, stmt_buf);
      strcat(content, "\n");
      free(stmt_buf);
    }

    size_t len = (size_t) snprintf(NULL, 0, fmt, indent_spaces, indent_start, content, indent_end);
    char *buf = malloc(len + 1);
    sprintf(buf, fmt, indent_spaces, indent_start, content, indent_end);

    free(content);
    free(indent_spaces);
    free(indent_inside_spaces);

    return buf;
  }

  free(indent_spaces);
  free(indent_inside_spaces);

  return NULL;
}
