/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_STMT_HPP
#define SRC_STMT_HPP

#include <vector>
#include "Expr.hpp"

struct Stmt;

enum StmtType {
  stmtAssignExpr,
  stmtCallExpr,
  stmtEnd,
  stmtMain,
  stmtShortVarDecl
};

struct StmtAssignExpr {
  Token left;
  Expr right;
};

struct StmtCallExpr {
  std::vector<Expr> args;
  Token callee;
};

struct StmtEnd {
  char reserved = '\0';
};

struct StmtMain {
  std::vector<Stmt> body;
};

struct StmtShortVarDecl {
  Token id;
  Expr init;
  bool mut = false;
};

struct Stmt {
  StmtType type;
  ReaderLocation start;
  ReaderLocation end;
  std::variant<StmtAssignExpr, StmtCallExpr, StmtEnd, StmtMain, StmtShortVarDecl> body;
};

inline std::string stmtStr (const Stmt &stmt, std::size_t indent = 0) {
  auto indentStr = std::string(indent, ' ');

  if (stmt.type == stmtAssignExpr) {
    auto stmtAssignExpr = std::get<StmtAssignExpr>(stmt.body);
    return indentStr + "<assign_expr left=\"" + stmtAssignExpr.left.val + "\" " +
      "right=\"" + exprStr(stmtAssignExpr.right) + "\" />";
  } else if (stmt.type == stmtCallExpr) {
    auto stmtCallExpr = std::get<StmtCallExpr>(stmt.body);
    auto result = indentStr + "<call_expr callee=\"" + stmtCallExpr.callee.val + "\" ";
    auto argIdx = 0;

    for (const auto &arg : stmtCallExpr.args) {
      result += "arg" + std::to_string(argIdx) + "=\"" + exprStr(arg) + "\" ";
      argIdx++;
    }

    return result + "/>";
  } else if (stmt.type == stmtMain) {
    auto stmtMain = std::get<StmtMain>(stmt.body);
    auto result = indentStr + "<main>" + (stmtMain.body.size() > 0 ? "\n" : "");

    for (const auto &bodyStmt : stmtMain.body) {
      result += stmtStr(bodyStmt, indent + 2) + "\n";
    }

    return result + indentStr + "</main>";
  } else if (stmt.type == stmtShortVarDecl) {
    auto stmtShortVarDecl = std::get<StmtShortVarDecl>(stmt.body);
    return indentStr + "<short_var_decl mut=\"" + (stmtShortVarDecl.mut ? "true" : "false") + "\" " +
      "id=\"" + stmtShortVarDecl.id.val + "\" init=\"" + exprStr(stmtShortVarDecl.init) + "\" />";
  }

  return "";
}

#endif
