/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <random>
#include "Codegen.hpp"
#include "Error.hpp"

std::string codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent);

Codegen::~Codegen () {
  for (auto fnDecl : this->fnDecls) {
    delete fnDecl;
  }

  delete this->varMap;
}

std::string randomId () {
  auto letters = std::string("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  auto result = std::string();
  auto mt = std::mt19937{std::random_device{}()};
  auto dist = std::uniform_real_distribution<double>{0, 61};

  result.reserve(10);

  for (auto i = 0; i < 6; i++) {
    auto idx = dist(mt);
    result += letters[static_cast<std::size_t>(idx)];
  }

  return result;
}

std::string codegenBlock (Codegen *codegen, const Block *block, std::size_t indent) {
  auto code = std::string();

  for (auto stmt : block->body) {
    code += codegenStmt(codegen, stmt, indent);
  }

  return code;
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

std::string codegenIdentifier (const Identifier *id) {
  return id->name->val;
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

std::string codegenTypeStr (VarMapItemType type, bool mut = false) {
  if (type == VAR_CHAR) {
    return mut ? "char " : "const char ";
  } else if (type == VAR_FLOAT) {
    return mut ? "double " : "const double ";
  } else if (type == VAR_INT) {
    return mut ? "long " : "const long ";
  } else if (type == VAR_STR) {
    return mut ? "char *" : "const char *";
  }

  return "none";
}

std::string codegenStmtExpr (Codegen *codegen, const StmtExpr *stmtExpr) {
  auto code = std::string();

  if (stmtExpr->parenthesized) {
    code += "(";
  }

  if (stmtExpr->type == STMT_EXPR_EXPR) {
    auto expr = std::get<Expr *>(stmtExpr->body);

    if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      code += codegenIdentifier(exprAssign->left);

      if (exprAssign->op->type == TK_OP_STAR_STAR_EQ) {
        code += " = pow(";
        code += codegenIdentifier(exprAssign->left);
        code += ", ";
        code += codegenStmtExpr(codegen, exprAssign->right);
        code += ")";
      } else if (exprAssign->op->type == TK_OP_AND_AND_EQ) {
        code += " = ";
        code += codegenIdentifier(exprAssign->left);
        code += " && ";
        code += codegenStmtExpr(codegen, exprAssign->right);
      } else if (exprAssign->op->type == TK_OP_OR_OR_EQ) {
        code += " = ";
        code += codegenIdentifier(exprAssign->left);
        code += " || ";
        code += codegenStmtExpr(codegen, exprAssign->right);
      } else {
        code += " " + exprAssign->op->val + " ";
        code += codegenStmtExpr(codegen, exprAssign->right);
      }
    } else if (expr->type == EXPR_BINARY) {
      auto exprBinary = std::get<ExprBinary *>(expr->body);

      if (exprBinary->op->type == TK_OP_STAR_STAR) {
        codegen->headers.math = true;

        code += "pow(";
        code += codegenStmtExpr(codegen, exprBinary->left);
        code += ", ";
        code += codegenStmtExpr(codegen, exprBinary->right);
        code += ")";
      } else if (exprBinary->op->type == TK_OP_SLASH || exprBinary->op->type == TK_OP_STAR) {
        code += "(double) ";
        code += codegenStmtExpr(codegen, exprBinary->left);
        code += " " + exprBinary->op->val + " (double) ";
        code += codegenStmtExpr(codegen, exprBinary->right);
      } else {
        code += codegenStmtExpr(codegen, exprBinary->left);
        code += " " + exprBinary->op->val + " ";
        code += codegenStmtExpr(codegen, exprBinary->right);
      }
    } else if (expr->type == EXPR_CALL) {
      auto exprCall = std::get<ExprCall *>(expr->body);

      if (exprCall->callee->name->val == "print") {
        codegen->headers.stdio = true;
        code += R"(printf(")";

        auto argIdx = 0;
        auto separator = std::string(R"(" ")");
        auto terminator = std::string(R"("\n")");

        for (auto arg : exprCall->args) {
          if (arg->id == nullptr) {
            continue;
          } else if (arg->id->name->val == "separator") {
            separator = codegenStmtExpr(codegen, arg->expr);
          } else if (arg->id->name->val == "terminator") {
            terminator = codegenStmtExpr(codegen, arg->expr);
          } else {
            throw Error("Unknown print argument");
          }
        }

        for (auto arg : exprCall->args) {
          if (arg->id != nullptr) {
            continue;
          }

          code += (argIdx != 0 ? "%s" : "");
          auto exprType = codegenStmtExprType(codegen, arg->expr);

          if (exprType == VAR_CHAR) {
            code += "%c";
          } else if (exprType == VAR_FLOAT) {
            code += "%f";
          } else if (exprType == VAR_INT) {
            code += "%ld";
          } else if (exprType == VAR_STR) {
            code += "%s";
          }

          argIdx++;
        }

        code += R"(%s", )";
        argIdx = 0;

        for (auto arg : exprCall->args) {
          if (arg->id != nullptr) {
            continue;
          }

          code += argIdx != 0 ? separator + ", " : "";
          code += codegenStmtExpr(codegen, arg->expr);
          code += ", ";

          argIdx++;
        }

        code += terminator + ")";
      } else {
        auto fnMapIt = codegen->varMap->getFn(exprCall->callee->name->val);

        code += fnMapIt.hiddenName;
        code += "(";

        auto argsCode = std::string();
        auto optionalArgsCode = std::string();

        for (auto it = fnMapIt.params.begin(); it != fnMapIt.params.end(); it++) {
          auto paramIdx = static_cast<std::size_t>(it - fnMapIt.params.begin());
          argsCode += paramIdx == 0 ? "" : ", ";
          auto param = *it;

          if (param->required && paramIdx >= exprCall->args.size()) {
            throw Error("Missing required function parameter in function call");
          } else if (param->required) {
            argsCode += codegenStmtExpr(codegen, exprCall->args[paramIdx]->expr);
            continue;
          }

          auto foundArg = static_cast<ExprCallArg *>(nullptr);

          if (paramIdx < exprCall->args.size() && exprCall->args[paramIdx]->id == nullptr) {
            foundArg = exprCall->args[paramIdx];
          } else {
            for (auto arg : exprCall->args) {
              if (arg->id != nullptr && arg->id->name->val == param->name) {
                foundArg = arg;
                break;
              }
            }
          }

          if (foundArg == nullptr) {
            if (param->type == VAR_CHAR) {
              argsCode += R"('\0')";
            } else if (param->type == VAR_FLOAT || param->type == VAR_INT) {
              argsCode += "0";
            } else if (param->type == VAR_STR) {
              argsCode += R"("")";
            }
          } else {
            argsCode += codegenStmtExpr(codegen, foundArg->expr);
          }

          optionalArgsCode += optionalArgsCode.empty() ? "" : ", ";
          optionalArgsCode += foundArg == nullptr ? "0" : "1";
        }

        if (fnMapIt.optionalParams > 0) {
          code += "(const char [" + std::to_string(fnMapIt.optionalParams) + "]) {";
          code += optionalArgsCode;
          code += "}, ";
        }

        code += argsCode;
        code += ")";
      }
    } else if (expr->type == EXPR_COND) {
      auto exprCond = std::get<ExprCond *>(expr->body);

      code += codegenStmtExpr(codegen, exprCond->cond);
      code += " ? ";
      code += codegenStmtExpr(codegen, exprCond->body);
      code += " : ";
      code += codegenStmtExpr(codegen, exprCond->alt);
    } else if (expr->type == EXPR_UNARY) {
      auto exprUnary = std::get<ExprUnary *>(expr->body);

      if (exprUnary->prefix) {
        code += exprUnary->op->val;
        code += codegenStmtExpr(codegen, exprUnary->arg);
      } else {
        code += codegenStmtExpr(codegen, exprUnary->arg);
        code += exprUnary->op->val;
      }
    }
  } else if (stmtExpr->type == STMT_EXPR_IDENTIFIER) {
    code += std::get<Identifier *>(stmtExpr->body)->name->val;
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    code += std::get<Literal *>(stmtExpr->body)->val->val;
  } else {
    throw Error("Tried to access unknown expr");
  }

  if (stmtExpr->parenthesized) {
    code += ")";
  }

  return code;
}

std::string codegenStmtIf (Codegen *codegen, const StmtIf *stmtIf, std::size_t indent) {
  auto code = std::string();

  codegen->varMap->save();
  code += "if (";
  code += codegenStmtExpr(codegen, stmtIf->cond);
  code += ") {\n";
  code += codegenBlock(codegen, stmtIf->body, indent + 2);

  if (stmtIf->alt != nullptr) {
    code += std::string(indent, ' ') + "} else ";

    if (stmtIf->alt->type == COND_BLOCK) {
      code += "{\n";
      code += codegenBlock(codegen, std::get<Block *>(stmtIf->alt->body), indent + 2);
      code += std::string(indent, ' ') + "}";
    } else if (stmtIf->alt->type == COND_STMT_IF) {
      code += codegenStmtIf(codegen, std::get<StmtIf *>(stmtIf->alt->body), indent);
    }
  } else {
    code += std::string(indent, ' ') + "}";
  }

  codegen->varMap->restore();
  return code;
}

std::string codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent) {
  auto code = std::string(indent, ' ');

  if (stmt->type == STMT_BREAK) {
    code += "break;\n";
  } else if (stmt->type == STMT_CONTINUE) {
    code += "continue;\n";
  } else if (stmt->type == STMT_EXPR) {
    code += codegenStmtExpr(codegen, std::get<StmtExpr *>(stmt->body));
    code += ";\n";
  } else if (stmt->type == STMT_FN_DECL) {
    auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
    auto returnType = codegenIdentifierType(stmtFnDecl->type);
    auto hiddenName = "__THE_" + randomId();

    code += codegenTypeStr(returnType);
    code += hiddenName + " (";

    auto params = std::vector<VarMapItemParam *>();
    auto optionalParams = static_cast<std::size_t>(0);

    if (stmtFnDecl->params.empty()) {
      code += "void";
    } else {
      auto paramsCode = std::string();

      for (auto it = stmtFnDecl->params.begin(); it != stmtFnDecl->params.end(); it++) {
        auto idx = it - stmtFnDecl->params.begin();
        auto param = *it;
        auto paramType = codegenType(codegen, param);
        auto paramRequired = param->init == nullptr;

        paramsCode += idx == 0 ? "" : ", ";
        auto paramCode = codegenTypeStr(paramType);
        paramsCode += paramCode.back() == ' ' ? paramCode.substr(0, paramCode.length() - 1) : paramCode;

        auto itParam = new VarMapItemParam{param->name->name->val, codegenType(codegen, param), paramRequired};
        params.push_back(itParam);

        if (!paramRequired) {
          optionalParams++;
        }
      }

      if (optionalParams > 0) {
        code += "const char [" + std::to_string(optionalParams) + "], ";
      }

      code += paramsCode;
    }

    code += ");\n";
    codegen->varMap->addFn(stmtFnDecl->id->name->val, hiddenName, returnType, params, optionalParams);
    codegen->fnDecls.push_back(new CodegenFnDecl{hiddenName, stmtFnDecl});
  } else if (stmt->type == STMT_IF) {
    code += codegenStmtIf(codegen, std::get<StmtIf *>(stmt->body), indent);
    code += "\n";
  } else if (stmt->type == STMT_LOOP) {
    auto stmtLoop = std::get<StmtLoop *>(stmt->body);
    codegen->varMap->save();

    if (stmtLoop->init == nullptr && stmtLoop->cond == nullptr && stmtLoop->upd == nullptr) {
      code += "while (1)";
    } else if (stmtLoop->init == nullptr && stmtLoop->upd == nullptr) {
      code += "while (";
      code += codegenStmtExpr(codegen, stmtLoop->cond);
      code += ")";
    } else {
      code += "for (";

      if (stmtLoop->init != nullptr) {
        auto stmtCode = codegenStmt(codegen, stmtLoop->init, 0);
        code += stmtCode.substr(0, stmtCode.length() - 2);
      }

      code += ";";

      if (stmtLoop->cond != nullptr) {
        code += " ";
        code += codegenStmtExpr(codegen, stmtLoop->cond);
      }

      code += ";";

      if (stmtLoop->upd != nullptr) {
        code += " ";
        code += codegenStmtExpr(codegen, stmtLoop->upd);
      }

      code += ")";
    }

    if (stmtLoop->body->body.empty()) {
      code += ";\n";
    } else {
      code += " {\n";
      code += codegenBlock(codegen, stmtLoop->body, indent + 2);
      code += std::string(indent, ' ') + "}\n";
    }

    codegen->varMap->restore();
  } else if (stmt->type == STMT_RETURN) {
    auto stmtReturn = std::get<StmtReturn *>(stmt->body);

    code += "return ";
    code += codegenStmtExpr(codegen, stmtReturn->arg);
    code += ";\n";
  } else if (stmt->type == STMT_SHORT_VAR_DECL) {
    auto stmtShortVarDecl = std::get<StmtShortVarDecl *>(stmt->body);
    auto exprType = codegenStmtExprType(codegen, stmtShortVarDecl->init);

    code += codegenTypeStr(exprType, stmtShortVarDecl->mut);
    code += codegenIdentifier(stmtShortVarDecl->id);
    code += " = ";
    code += codegenStmtExpr(codegen, stmtShortVarDecl->init);
    code += ";\n";

    codegen->varMap->add(exprType, stmtShortVarDecl->id->name->val);
  } else {
    throw Error("Tried to access unknown stmt type");
  }

  return code;
}

std::string codegen (const AST *ast) {
  auto codegen = new Codegen{{}, "", {}, new VarMap{}};

  for (auto stmt : ast->topLevelStmts) {
    codegen->body += codegenStmt(codegen, stmt, 0);
  }

  codegen->varMap->save();
  codegen->body += "\nint main () {\n";

  if (ast->mainPresent) {
    codegen->body += codegenBlock(codegen, ast->mainBody, 2);
  }

  codegen->body += "}\n";
  codegen->varMap->restore();

  for (auto it1 = codegen->fnDecls.begin(); it1 != codegen->fnDecls.end(); it1++) {
    auto fnDecl = *it1;
    auto returnType = codegenIdentifierType(fnDecl->stmt->type);

    codegen->body += "\n";
    codegen->body += codegenTypeStr(returnType);
    codegen->body += fnDecl->hiddenName + " (";
    codegen->varMap->save();

    if (fnDecl->stmt->params.empty()) {
      codegen->body += "void) {\n";
    } else {
      auto optionalParams = static_cast<std::size_t>(0);
      auto paramsCode = std::string();
      auto optionalParamsCode = std::string();

      for (auto it2 = fnDecl->stmt->params.begin(); it2 != fnDecl->stmt->params.end(); it2++) {
        auto idx = it2 - fnDecl->stmt->params.begin();
        auto param = *it2;
        auto paramName = codegenIdentifier(param->name);
        auto paramType = codegenType(codegen, param);
        auto paramTypeStr = codegenTypeStr(paramType);
        auto paramRequired = param->init == nullptr;

        paramsCode += idx == 0 ? "" : ", ";
        paramsCode += paramTypeStr;
        paramsCode += (paramRequired ? "" : "_") + paramName;

        if (!paramRequired) {
          optionalParamsCode += std::string(2, ' ') + paramTypeStr;
          optionalParamsCode += paramName + " = __THE_opt_params[" + std::to_string(optionalParams) + "] == 0 ? ";
          optionalParamsCode += codegenStmtExpr(codegen, param->init);
          optionalParamsCode += " : _" + paramName;
          optionalParamsCode += ";\n";

          optionalParams++;
        }

        codegen->varMap->add(paramType, paramName);
      }

      if (optionalParams > 0) {
        codegen->body += "const char __THE_opt_params[" + std::to_string(optionalParams) + "], ";
      }

      codegen->body += paramsCode;
      codegen->body += ") {\n";
      codegen->body += optionalParamsCode;
    }

    codegen->body += codegenBlock(codegen, fnDecl->stmt->body, 2);
    codegen->body += "}\n";
    codegen->varMap->restore();
  }

  auto headers = std::string(codegen->headers.math ? "#include <math.h>\n" : "") +
    std::string(codegen->headers.stdio ? "#include <stdio.h>\n" : "");

  auto code = (headers.empty() ? headers : headers + "\n") +
    codegen->body;

  delete codegen;
  return code;
}
