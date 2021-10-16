/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_EXPR_HPP
#define SRC_EXPR_HPP

#include <variant>
#include "Token.hpp"

enum ExprType {
  exprLiteral
};

struct ExprLiteral {
  Token tok;
};

struct Expr {
  ExprType type;
  ReaderLocation start;
  ReaderLocation end;
  std::variant<ExprLiteral> body;
};

inline std::string exprStr (const Expr &expr) {
  if (expr.type == exprLiteral) {
    auto exprLiteral = std::get<ExprLiteral>(expr.body);
    return exprLiteral.tok.val;
  }

  return "";
}

#endif
