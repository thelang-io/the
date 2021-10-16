/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Codegen.hpp"
#include "Error.hpp"
#include "VarMap.hpp"

std::string codegenExpr (const Expr &expr) {
  if (expr.type == exprLiteral) {
    auto exprLiteral = std::get<ExprLiteral>(expr.body);
    return exprLiteral.tok.val;
  }

  throw Error("Tried to access unknown expr type");
}

VarMapItemType codegenExprType (const VarMap &varMap, const Expr &expr) {
  if (expr.type == exprLiteral) {
    auto exprLiteral = std::get<ExprLiteral>(expr.body);

    if (exprLiteral.tok.type == tkLitChar) {
      return varChar;
    } else if (exprLiteral.tok.type == tkLitFloat) {
      return varFloat;
    } else if (exprLiteral.tok.type == tkLitId) {
      return varMap.get(exprLiteral.tok.val).type;
    } else if (exprLiteral.tok.type == tkLitIntDec) {
      return varIntDec;
    } else if (exprLiteral.tok.type == tkLitStr) {
      return varStr;
    }
  }

  throw Error("Tried to access unknown expr type");
}

std::string codegenExprTypeFmt (const VarMap &varMap, const Expr &expr) {
  auto type = codegenExprType(varMap, expr);

  switch (type) {
    case varChar: {
      return "%c";
    }
    case varFloat: {
      return "%f";
    }
    case varIntDec: {
      return "%lu";
    }
    default: {
      return "%s";
    }
  }
}

const char *codegenExprTypeStr (VarMapItemType type, bool mut) {
  switch (type) {
    case varChar: {
      return mut ? "char " : "const char ";
    }
    case varFloat: {
      return mut ? "float " : "const float ";
    }
    case varIntDec: {
      return mut ? "unsigned long " : "const unsigned long ";
    }
    default: {
      return mut ? "char *" : "const char *";
    }
  }
}

std::string codegen (const AST &ast) {
  auto varMap = VarMap();
  auto headers = CodegenHeaders{false};
  auto mainBody = std::string();

  if (ast.mainPresent) {
    for (const auto &stmt : ast.mainBody) {
      if (stmt.type == stmtAssignExpr) {
        auto stmtAssignExpr = std::get<StmtAssignExpr>(stmt.body);
        auto varName = stmtAssignExpr.left.val;

        mainBody += varName + " = " + codegenExpr(stmtAssignExpr.right) + ";\n";
        continue;
      } else if (stmt.type == stmtCallExpr) {
        auto stmtCallExpr = std::get<StmtCallExpr>(stmt.body);

        if (stmtCallExpr.callee.val == "print") {
          headers.stdio = true;
          mainBody += R"(printf(")";

          auto argIdx = 0;

          for (const auto &arg : stmtCallExpr.args) {
            mainBody += (argIdx != 0 ? "%s" : "") + codegenExprTypeFmt(varMap, arg);
            argIdx++;
          }

          mainBody += R"(%s", )";
          argIdx = 0;

          for (const auto &arg : stmtCallExpr.args) {
            mainBody += (argIdx != 0 ? R"(" ", )" : "") + codegenExpr(arg) + ", ";
            argIdx++;
          }

          mainBody += R"("\n");)";
          mainBody += "\n";

          continue;
        }

        throw Error("Tried to access unknown built-in function");
      } else if (stmt.type == stmtShortVarDecl) {
        auto stmtShortVarDecl = std::get<StmtShortVarDecl>(stmt.body);
        auto varType = codegenExprType(varMap, stmtShortVarDecl.init);
        auto varTypeStr = codegenExprTypeStr(varType, stmtShortVarDecl.mut);
        auto varName = stmtShortVarDecl.id.val;

        mainBody += varTypeStr + varName + " = " + codegenExpr(stmtShortVarDecl.init) + ";\n";
        varMap.add(varType, varName);

        continue;
      }

      throw Error("Tried to access unknown stmt type");
    }
  }

  auto headersCode = std::string(headers.stdio ? "#include <stdio.h>\n" : "");
  return headersCode + "\nint main (const int argc, const char **argv) {\n" + mainBody + "return 0;\n}\n";
}
