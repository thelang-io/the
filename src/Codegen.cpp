/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Codegen.hpp"
#include "Error.hpp"

void codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent);

Codegen::~Codegen () {
  delete this->varMap;
}

void codegenBlock (Codegen *codegen, const Block *block, std::size_t indent) {
  for (const auto &stmt : block->body) {
    codegenStmt(codegen, stmt, indent);
  }
}

VarMapItemType codegenIdentifierType (const Identifier *id) {
  if (id->name->val == "char") {
    return VAR_CHAR;
  } else if (id->name->val == "float") {
    return VAR_FLOAT;
  } else if (id->name->val == "int") {
    return VAR_INT;
  } else {
    return VAR_STR;
  }
}

void codegenIdentifier (Codegen *codegen, const Identifier *id) {
  codegen->body += id->name->val;
}

VarMapItemType codegenStmtExprType (const Codegen *codegen, const StmtExpr *stmtExpr) {
  if (stmtExpr->type == STMT_EXPR_EXPR) {
    auto expr = std::get<Expr *>(stmtExpr->body);

    if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      return codegen->varMap->get(exprAssign->left->name->val).type;
    } else if (expr->type == EXPR_BINARY) {
      auto exprBinary = std::get<ExprBinary *>(expr->body);
      auto exprBinaryLeftType = codegenStmtExprType(codegen, exprBinary->left);
      auto exprBinaryRightType = codegenStmtExprType(codegen, exprBinary->right);

      if (
        exprBinaryLeftType == VAR_CHAR ||
        exprBinaryLeftType == VAR_STR ||
        exprBinaryRightType == VAR_CHAR ||
        exprBinaryRightType == VAR_STR
      ) {
        if (exprBinary->op->type == TK_OP_PLUS) {
          return VAR_STR;
        } else {
          throw Error("Tried operation other than concatenation on string");
        }
      } else if (
        exprBinaryLeftType == VAR_FLOAT ||
        exprBinaryRightType == VAR_FLOAT ||
        exprBinary->op->type == TK_OP_SLASH ||
        exprBinary->op->type == TK_OP_STAR ||
        exprBinary->op->type == TK_OP_STAR_STAR
      ) {
        return VAR_FLOAT;
      } else {
        return VAR_INT;
      }
    } else if (expr->type == EXPR_CALL) {
      auto exprCall = std::get<ExprCall *>(expr->body);
      return codegen->varMap->getFn(exprCall->callee->name->val).returnType;
    } else if (expr->type == EXPR_COND) {
      auto exprCond = std::get<ExprCond *>(expr->body);
      auto exprCondBodyType = codegenStmtExprType(codegen, exprCond->body);
      auto exprCondAltType = codegenStmtExprType(codegen, exprCond->alt);

      if (exprCondBodyType != exprCondAltType) {
        throw Error("Incompatible operand types");
      }

      return exprCondBodyType;
    } else if (expr->type == EXPR_UNARY) {
      auto exprUnary = std::get<ExprUnary *>(expr->body);
      auto exprType = codegenStmtExprType(codegen, exprUnary->arg);

      if (exprType != VAR_FLOAT && exprType != VAR_INT) {
        return VAR_INT;
      }

      return exprType;
    }
  } else if (stmtExpr->type == STMT_EXPR_IDENTIFIER) {
    auto id = std::get<Identifier *>(stmtExpr->body);
    return codegen->varMap->get(id->name->val).type;
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    auto lit = std::get<Literal *>(stmtExpr->body);

    if (lit->val->type == TK_LIT_CHAR) {
      return VAR_CHAR;
    } else if (lit->val->type == TK_LIT_FLOAT) {
      return VAR_FLOAT;
    } else if (lit->val->type == TK_LIT_INT_DEC) {
      return VAR_INT;
    } else if (lit->val->type == TK_LIT_STR) {
      return VAR_STR;
    }
  }

  throw Error("Tried to access unknown expr type");
}

VarMapItemType codegenType (const Codegen *codegen, const StmtFnDeclParam *param) {
  if (param->type != nullptr && param->type->name->val == "char") {
    return VAR_CHAR;
  } else if (param->type != nullptr && param->type->name->val == "float") {
    return VAR_FLOAT;
  } else if (param->type != nullptr && param->type->name->val == "int") {
    return VAR_INT;
  } else if (param->type != nullptr) {
    return VAR_STR;
  }

  return codegenStmtExprType(codegen, param->init);
}

void codegenTypeStr (Codegen *codegen, VarMapItemType type, bool mut = false) {
  if (type == VAR_CHAR) {
    codegen->body += mut ? "char " : "const char ";
  } else if (type == VAR_FLOAT) {
    codegen->body += mut ? "double " : "const double ";
  } else if (type == VAR_INT) {
    codegen->body += mut ? "long " : "const long ";
  } else if (type == VAR_STR) {
    codegen->body += mut ? "char *" : "const char *";
  }
}

void codegenStmtExpr (Codegen *codegen, const StmtExpr *stmtExpr) {
  if (stmtExpr->parenthesized) {
    codegen->body += "(";
  }

  if (stmtExpr->type == STMT_EXPR_EXPR) {
    auto expr = std::get<Expr *>(stmtExpr->body);

    if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      codegenIdentifier(codegen, exprAssign->left);

      if (exprAssign->op->type == TK_OP_STAR_STAR_EQ) {
        codegen->body += " = pow(";
        codegenIdentifier(codegen, exprAssign->left);
        codegen->body += ", ";
        codegenStmtExpr(codegen, exprAssign->right);
        codegen->body += ")";
      } else if (exprAssign->op->type == TK_OP_AND_AND_EQ) {
        codegen->body += " = ";
        codegenIdentifier(codegen, exprAssign->left);
        codegen->body += " && ";
        codegenStmtExpr(codegen, exprAssign->right);
      } else if (exprAssign->op->type == TK_OP_OR_OR_EQ) {
        codegen->body += " = ";
        codegenIdentifier(codegen, exprAssign->left);
        codegen->body += " || ";
        codegenStmtExpr(codegen, exprAssign->right);
      } else {
        codegen->body += " " + exprAssign->op->val + " ";
        codegenStmtExpr(codegen, exprAssign->right);
      }
    } else if (expr->type == EXPR_BINARY) {
      auto exprBinary = std::get<ExprBinary *>(expr->body);

      if (exprBinary->op->type == TK_OP_STAR_STAR) {
        codegen->headers.math = true;
        codegen->body += "pow(";
        codegenStmtExpr(codegen, exprBinary->left);
        codegen->body += ", ";
        codegenStmtExpr(codegen, exprBinary->right);
        codegen->body += ")";
      } else if (exprBinary->op->type == TK_OP_SLASH || exprBinary->op->type == TK_OP_STAR) {
        codegen->body += "(double) ";
        codegenStmtExpr(codegen, exprBinary->left);
        codegen->body += " " + exprBinary->op->val + " (double) ";
        codegenStmtExpr(codegen, exprBinary->right);
      } else {
        codegenStmtExpr(codegen, exprBinary->left);
        codegen->body += " " + exprBinary->op->val + " ";
        codegenStmtExpr(codegen, exprBinary->right);
      }
    } else if (expr->type == EXPR_CALL) {
      auto exprCall = std::get<ExprCall *>(expr->body);

      if (exprCall->callee->name->val == "print") {
        codegen->headers.stdio = true;
        codegen->body += R"(printf(")";

        auto argIdx = 0;

        for (const auto &arg : exprCall->args) {
          codegen->body += (argIdx != 0 ? "%s" : "");
          auto exprType = codegenStmtExprType(codegen, arg);

          if (exprType == VAR_CHAR) {
            codegen->body += "%c";
          } else if (exprType == VAR_FLOAT) {
            codegen->body += "%f";
          } else if (exprType == VAR_INT) {
            codegen->body += "%ld";
          } else if (exprType == VAR_STR) {
            codegen->body += "%s";
          }

          argIdx++;
        }

        codegen->body += R"(%s", )";
        argIdx = 0;

        for (const auto &arg : exprCall->args) {
          codegen->body += (argIdx != 0 ? R"(" ", )" : "");
          codegenStmtExpr(codegen, arg);
          codegen->body += ", ";

          argIdx++;
        }

        codegen->body += R"("\n"))";
      } else {
        auto fnMapIt = codegen->varMap->getFn(exprCall->callee->name->val);

        codegen->body += fnMapIt.name;
        codegen->body += "(";

        for (auto it = fnMapIt.params.begin(); it != fnMapIt.params.end(); it++) {
          auto paramIdx = static_cast<std::size_t>(it - fnMapIt.params.begin());
          codegen->body += paramIdx == 0 ? "" : ", ";
          auto param = *it;

          if (paramIdx >= exprCall->args.size()) {
            if (param->required) {
              throw Error("Missing required function parameter in function call");
            }

            if (param->type == VAR_CHAR) {
              codegen->body += R"('\0')";
            } else if (param->type == VAR_FLOAT || param->type == VAR_INT) {
              codegen->body += "0";
            } else if (param->type == VAR_STR) {
              codegen->body += R"("")";
            }
          } else {
            codegenStmtExpr(codegen, exprCall->args[paramIdx]);
          }
        }

        codegen->body += ")";
      }
    } else if (expr->type == EXPR_COND) {
      auto exprCond = std::get<ExprCond *>(expr->body);

      codegenStmtExpr(codegen, exprCond->cond);
      codegen->body += " ? ";
      codegenStmtExpr(codegen, exprCond->body);
      codegen->body += " : ";
      codegenStmtExpr(codegen, exprCond->alt);
    } else if (expr->type == EXPR_UNARY) {
      auto exprUnary = std::get<ExprUnary *>(expr->body);

      if (exprUnary->prefix) {
        codegen->body += exprUnary->op->val;
        codegenStmtExpr(codegen, exprUnary->arg);
      } else {
        codegenStmtExpr(codegen, exprUnary->arg);
        codegen->body += exprUnary->op->val;
      }
    }
  } else if (stmtExpr->type == STMT_EXPR_IDENTIFIER) {
    auto id = std::get<Identifier *>(stmtExpr->body);
    codegen->body += id->name->val;
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    auto lit = std::get<Literal *>(stmtExpr->body);
    codegen->body += lit->val->val;
  } else {
    throw Error("Tried to access unknown expr");
  }

  if (stmtExpr->parenthesized) {
    codegen->body += ")";
  }
}

void codegenStmtIf (Codegen *codegen, const StmtIf *stmtIf, std::size_t indent) {
  codegen->varMap->save();
  codegen->body += "if (";
  codegenStmtExpr(codegen, stmtIf->cond);
  codegen->body += ") {\n";
  codegenBlock(codegen, stmtIf->body, indent + 2);

  if (stmtIf->alt != nullptr) {
    codegen->body += std::string(indent, ' ') + "} else ";

    if (stmtIf->alt->type == COND_BLOCK) {
      codegen->body += "{\n";
      codegenBlock(codegen, std::get<Block *>(stmtIf->alt->body), indent + 2);
      codegen->body += std::string(indent, ' ') + "}";
    } else if (stmtIf->alt->type == COND_STMT_IF) {
      codegenStmtIf(codegen, std::get<StmtIf *>(stmtIf->alt->body), indent);
    }
  } else {
    codegen->body += std::string(indent, ' ') + "}";
  }

  codegen->varMap->restore();
}

void codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent) {
  codegen->body += std::string(indent, ' ');

  if (stmt->type == STMT_BREAK) {
    codegen->body += "break;\n";
  } else if (stmt->type == STMT_CONTINUE) {
    codegen->body += "continue;\n";
  } else if (stmt->type == STMT_EXPR) {
    codegenStmtExpr(codegen, std::get<StmtExpr *>(stmt->body));
    codegen->body += ";\n";
  } else if (stmt->type == STMT_FN_DECL) {
    auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
    auto returnType = codegenIdentifierType(stmtFnDecl->type);

    codegenTypeStr(codegen, returnType);
    codegenIdentifier(codegen, stmtFnDecl->id);
    codegen->body += " (";

    auto params = std::vector<VarMapItemParam *>();

    for (const auto &param : stmtFnDecl->params) {
      params.push_back(new VarMapItemParam{codegenType(codegen, param), param->init == nullptr});
    }

    codegen->varMap->addFn(stmtFnDecl->id->name->val, returnType, params);
    codegen->varMap->save();

    for (auto it = stmtFnDecl->params.begin(); it != stmtFnDecl->params.end(); it++) {
      auto idx = it - stmtFnDecl->params.begin();
      auto param = *it;
      auto paramType = codegenType(codegen, param);

      codegen->body += idx == 0 ? "" : ", ";
      codegenTypeStr(codegen, paramType);
      codegenIdentifier(codegen, param->name);
      codegen->varMap->add(paramType, param->name->name->val);
    }

    codegen->body += ") {\n";
    codegenBlock(codegen, stmtFnDecl->body, indent + 2);
    codegen->body += std::string(indent, ' ') + "}\n";
    codegen->varMap->restore();
  } else if (stmt->type == STMT_IF) {
    codegenStmtIf(codegen, std::get<StmtIf *>(stmt->body), indent);
    codegen->body += "\n";
  } else if (stmt->type == STMT_LOOP) {
    auto stmtLoop = std::get<StmtLoop *>(stmt->body);

    codegen->varMap->save();
    codegen->body += "for (";

    if (stmtLoop->init != nullptr) {
      codegenStmt(codegen, stmtLoop->init, 0);
      codegen->body.pop_back();
      codegen->body.pop_back();
    }

    codegen->body += ";";

    if (stmtLoop->cond != nullptr) {
      codegen->body += " ";
      codegenStmtExpr(codegen, stmtLoop->cond);
    }

    codegen->body += ";";

    if (stmtLoop->upd != nullptr) {
      codegen->body += " ";
      codegenStmtExpr(codegen, stmtLoop->upd);
    }

    codegen->body += ") {\n";
    codegenBlock(codegen, stmtLoop->body, indent + 2);
    codegen->body += std::string(indent, ' ') + "}\n";
    codegen->varMap->restore();
  } else if (stmt->type == STMT_RETURN) {
    auto stmtReturn = std::get<StmtReturn *>(stmt->body);

    codegen->body += "return ";
    codegenStmtExpr(codegen, stmtReturn->arg);
    codegen->body += ";\n";
  } else if (stmt->type == STMT_SHORT_VAR_DECL) {
    auto stmtShortVarDecl = std::get<StmtShortVarDecl *>(stmt->body);
    auto exprType = codegenStmtExprType(codegen, stmtShortVarDecl->init);

    codegenTypeStr(codegen, exprType, stmtShortVarDecl->mut);
    codegenIdentifier(codegen, stmtShortVarDecl->id);
    codegen->body += " = ";
    codegenStmtExpr(codegen, stmtShortVarDecl->init);
    codegen->body += ";\n";

    codegen->varMap->add(exprType, stmtShortVarDecl->id->name->val);
  } else {
    throw Error("Tried to access unknown stmt type");
  }
}

std::string codegen (const AST *ast) {
  auto codegen = new Codegen{{}, "", new VarMap{}};

  for (const auto &stmt : ast->topLevelStmts) {
    codegenStmt(codegen, stmt, 0);
  }

  codegen->varMap->save();
  codegen->body += "\nint main () {\n";

  if (ast->mainPresent) {
    codegenBlock(codegen, ast->mainBody, 2);
  }

  codegen->body += "}\n";
  codegen->varMap->restore();

  auto headers = std::string(codegen->headers.math ? "#include <math.h>\n" : "") +
    std::string(codegen->headers.stdio ? "#include <stdio.h>\n" : "");

  auto code = (headers.empty() ? headers : headers + "\n") +
    codegen->body;

  delete codegen;
  return code;
}
