/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <algorithm>
#include <climits>
#include "Codegen.hpp"
#include "Error.hpp"

void codegenFunctionDeclarations (Codegen *codegen, const std::vector<Stmt *> &body);
void codegenFunctionDefinitions (Codegen *codegen, const std::vector<Stmt *> &body);
std::string codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent);

Codegen::~Codegen () {
  delete this->varMap;
}

std::string codegenBlock (Codegen *codegen, const Block *block, std::size_t indent) {
  auto code = std::string();

  for (auto stmt : block->body) {
    code += codegenStmt(codegen, stmt, indent);
  }

  codegenFunctionDeclarations(codegen, block->body);
  codegenFunctionDefinitions(codegen, block->body);

  return code;
}

std::string codegenIdentifier (const Identifier *id) {
  return "__THE_" + id->name->val;
}

std::string codegenExprMember (const std::string &obj, const std::string &prop) {
  return obj + "_MA_" + prop;
}

std::string codegenExprAccess (const ExprAccess *exprAccess) {
  if (exprAccess->type == EXPR_ACCESS_EXPR_MEMBER) {
    auto exprMemberObj = std::get<ExprMember *>(exprAccess->body);
    return codegenExprMember(codegenExprAccess(exprMemberObj->obj), exprMemberObj->prop->name->val);
  } else if (exprAccess->type == EXPR_ACCESS_IDENTIFIER) {
    return codegenIdentifier(std::get<Identifier *>(exprAccess->body));
  }

  throw Error("Tried to access unknown expression access");
}

VarMapItemType codegenStmtExprType (Codegen *codegen, const StmtExpr *stmtExpr) {
  if (stmtExpr->type == STMT_EXPR_EXPR) {
    auto expr = std::get<Expr *>(stmtExpr->body);

    if (expr->type == EXPR_ACCESS) {
      auto name = codegenExprAccess(std::get<ExprAccess *>(expr->body));
      return codegen->varMap->get(name)->type;
    } else if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      auto name = codegenExprAccess(exprAssign->left);

      return codegen->varMap->get(name)->type;
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
      auto name = codegenExprAccess(exprCall->callee);

      return codegen->varMap->getFn(name)->fn->returnType;
    } else if (expr->type == EXPR_COND) {
      auto exprCond = std::get<ExprCond *>(expr->body);
      auto exprCondBodyType = codegenStmtExprType(codegen, exprCond->body);
      auto exprCondAltType = codegenStmtExprType(codegen, exprCond->alt);

      if (exprCondBodyType != exprCondAltType) {
        throw Error("Incompatible operand types");
      }

      return exprCondBodyType;
    } else if (expr->type == EXPR_OBJ) {
      return VAR_OBJ;
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
    auto name = codegenIdentifier(id);

    return codegen->varMap->get(name)->type;
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    auto lit = std::get<Literal *>(stmtExpr->body);

    if (lit->val->type == TK_KW_FALSE) return VAR_BOOL;
    if (lit->val->type == TK_KW_TRUE) return VAR_BOOL;
    if (lit->val->type == TK_LIT_CHAR) return VAR_CHAR;
    if (lit->val->type == TK_LIT_FLOAT) return VAR_FLOAT;
    if (lit->val->type == TK_LIT_INT_BIN) return VAR_INT;
    if (lit->val->type == TK_LIT_INT_DEC) return VAR_INT;
    if (lit->val->type == TK_LIT_INT_HEX) return VAR_INT;
    if (lit->val->type == TK_LIT_INT_OCT) return VAR_INT;
    if (lit->val->type == TK_LIT_STR) return VAR_STR;
  }

  throw Error("Tried to access unknown expr type");
}

VarMapItemType codegenType (Codegen *codegen, const Identifier *type, const StmtExpr *init = nullptr) {
  if (type != nullptr) {
    if (type->name->val == "bool") return VAR_BOOL;
    if (type->name->val == "byte") return VAR_BYTE;
    if (type->name->val == "char") return VAR_CHAR;
    if (type->name->val == "float") return VAR_FLOAT;
    if (type->name->val == "f32") return VAR_F32;
    if (type->name->val == "f64") return VAR_F64;
    if (type->name->val == "int") return VAR_INT;
    if (type->name->val == "i8") return VAR_I8;
    if (type->name->val == "i16") return VAR_I16;
    if (type->name->val == "i32") return VAR_I32;
    if (type->name->val == "i64") return VAR_I64;
    if (type->name->val == "str") return VAR_STR;
    if (type->name->val == "u8") return VAR_U8;
    if (type->name->val == "u16") return VAR_U16;
    if (type->name->val == "u32") return VAR_U32;
    if (type->name->val == "u64") return VAR_U64;

    try {
      auto name = codegenIdentifier(type);
      return codegen->varMap->get(name)->type;
    } catch (const Error &err) {
    }
  }

  if (init != nullptr) {
    return codegenStmtExprType(codegen, init);
  }

  throw Error("Tried to access unknown type");
}

std::string codegenTypeFormat (VarMapItemType type) {
  if (type == VAR_BOOL) return "%s";
  if (type == VAR_BYTE) return "%x";
  if (type == VAR_CHAR) return "%c";
  if (type == VAR_FLOAT) return "%f";
  if (type == VAR_F32) return "%f";
  if (type == VAR_F64) return "%f";
  if (type == VAR_INT) return "%ld";
  if (type == VAR_I8) return "%d";
  if (type == VAR_I16) return "%d";
  if (type == VAR_I32) return "%ld";
  if (type == VAR_I64) return "%lld";
  if (type == VAR_STR) return "%s";
  if (type == VAR_U8) return "%u";
  if (type == VAR_U16) return "%u";
  if (type == VAR_U32) return "%lu";
  if (type == VAR_U64) return "%llu";

  throw Error("Unknown print argument type");
}

std::string codegenTypeStr (VarMapItemType type, bool mut = false) {
  if (type == VAR_BOOL) return mut ? "char " : "const char ";
  if (type == VAR_BYTE) return mut ? "unsigned char " : "const unsigned char ";
  if (type == VAR_CHAR) return mut ? "char " : "const char ";
  if (type == VAR_FLOAT) return mut ? "double " : "const double ";
  if (type == VAR_F32) return mut ? "float " : "const float ";
  if (type == VAR_F64) return mut ? "double " : "const double ";
  if (type == VAR_INT) return mut ? "long " : "const long ";
  if (type == VAR_I8) return mut ? "char " : "const char ";
  if (type == VAR_I16) return mut ? "short " : "const short ";
  if (type == VAR_I32) return mut ? "long " : "const long ";
  if (type == VAR_I64) return mut ? "long long " : "const long long ";
  if (type == VAR_STR) return mut ? "char *" : "const char *";
  if (type == VAR_U8) return mut ? "unsigned char " : "const unsigned char ";
  if (type == VAR_U16) return mut ? "unsigned short " : "const unsigned short ";
  if (type == VAR_U32) return mut ? "unsigned long " : "const unsigned long ";
  if (type == VAR_U64) return mut ? "unsigned long long " : "const unsigned long long ";

  throw Error("Tried to access unknown type str");
}

std::string codegenStmtExpr (Codegen *codegen, const StmtExpr *stmtExpr) {
  auto code = std::string();

  if (stmtExpr->parenthesized) {
    code += "(";
  }

  if (stmtExpr->type == STMT_EXPR_EXPR) {
    auto expr = std::get<Expr *>(stmtExpr->body);

    if (expr->type == EXPR_ACCESS) {
      code += codegenExprAccess(std::get<ExprAccess *>(expr->body));
    } else if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      code += codegenExprAccess(exprAssign->left);

      if (exprAssign->op->type == TK_OP_STAR_STAR_EQ) {
        code += " = pow(";
        code += codegenExprAccess(exprAssign->left);
        code += ", ";
        code += codegenStmtExpr(codegen, exprAssign->right);
        code += ")";
      } else if (exprAssign->op->type == TK_OP_AND_AND_EQ) {
        code += " = ";
        code += codegenExprAccess(exprAssign->left);
        code += " && ";
        code += codegenStmtExpr(codegen, exprAssign->right);
      } else if (exprAssign->op->type == TK_OP_OR_OR_EQ) {
        code += " = ";
        code += codegenExprAccess(exprAssign->left);
        code += " || ";
        code += codegenStmtExpr(codegen, exprAssign->right);
      } else {
        code += " " + exprAssign->op->val + " ";
        code += codegenStmtExpr(codegen, exprAssign->right);
      }
    } else if (expr->type == EXPR_BINARY) {
      auto exprBinary = std::get<ExprBinary *>(expr->body);

      if (exprBinary->op->type == TK_OP_STAR_STAR) {
        codegen->polyfills.pow = true;

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

      if (
        exprCall->callee->type == EXPR_ACCESS_IDENTIFIER &&
        std::get<Identifier *>(exprCall->callee->body)->name->val == "print"
      ) {
        codegen->polyfills.printf = true;
        code += R"(printf(")";

        auto separator = std::string(R"(" ")");
        auto terminator = std::string(R"("\n")");
        auto argIdx = static_cast<std::size_t>(0);

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
          code += codegenTypeFormat(exprType);

          argIdx++;
        }

        code += R"(%s", )";
        argIdx = 0;

        for (auto arg : exprCall->args) {
          if (arg->id != nullptr) {
            continue;
          }

          auto exprType = codegenStmtExprType(codegen, arg->expr);
          code += argIdx != 0 ? separator + ", " : "";

          if (exprType == VAR_BOOL) {
            code += "(";
          }

          code += codegenStmtExpr(codegen, arg->expr);

          if (exprType == VAR_BOOL) {
            code += R"() == 1 ? "true" : "false")";
          }

          code += ", ";

          argIdx++;
        }

        code += terminator + ")";
      } else {
        auto fnName = codegenExprAccess(exprCall->callee);
        auto fnMapIt = codegen->varMap->getFn(fnName);

        code += fnMapIt->name;
        code += "(";

        auto argsCode = std::string();
        auto optionalArgsCode = std::string();
        auto idx = static_cast<std::size_t>(0);

        for (auto param : fnMapIt->fn->params) {
          argsCode += idx == 0 ? "" : ", ";

          if (param->required && idx >= exprCall->args.size()) {
            throw Error("Missing required function parameter in function call");
          } else if (param->required) {
            argsCode += codegenStmtExpr(codegen, exprCall->args[idx]->expr);
            idx++;
            continue;
          }

          auto foundArg = static_cast<ExprCallArg *>(nullptr);

          if (idx < exprCall->args.size() && exprCall->args[idx]->id == nullptr) {
            foundArg = exprCall->args[idx];
          } else {
            for (auto arg : exprCall->args) {
              if (arg->id != nullptr && codegenIdentifier(arg->id) == param->name) {
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
          idx++;
        }

        if (fnMapIt->fn->optionalParams > 0) {
          code += "(const char [" + std::to_string(fnMapIt->fn->optionalParams) + "]) {";
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
    auto id = std::get<Identifier *>(stmtExpr->body);
    code += codegenIdentifier(id);
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    auto lit = std::get<Literal *>(stmtExpr->body);

    if (lit->val->type == TK_KW_FALSE) {
      code += "0";
    } else if (lit->val->type == TK_KW_TRUE) {
      code += "1";
    } else if (lit->val->type == TK_LIT_INT_OCT) {
      auto val = lit->val->val;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      code += val;
    } else {
      code += lit->val->val;
    }
  } else {
    throw Error("Tried to access unknown expr");
  }

  if (stmtExpr->parenthesized) {
    code += ")";
  }

  return code;
}

void codegenFunctionDeclarations (Codegen *codegen, const std::vector<Stmt *> &body) {
  auto code = std::string();

  for (auto stmt : body) {
    if (stmt->type != STMT_FN_DECL) {
      continue;
    }

    auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
    auto hiddenName = codegenIdentifier(stmtFnDecl->id);
    auto fnVar = codegen->varMap->getFn(hiddenName);

    code += codegenTypeStr(fnVar->fn->returnType);
    code += fnVar->name + " (";

    if (fnVar->fn->params.empty()) {
      code += "void";
    } else {
      auto paramsCode = std::string();
      auto idx = static_cast<std::size_t>(0);

      for (auto param : fnVar->fn->params) {
        auto paramTypeStr = codegenTypeStr(param->type);

        paramsCode += idx == 0 ? "" : ", ";
        paramsCode += paramTypeStr.back() == ' ' ? paramTypeStr.substr(0, paramTypeStr.length() - 1) : paramTypeStr;
        idx++;
      }

      if (fnVar->fn->optionalParams > 0) {
        code += "const char [" + std::to_string(fnVar->fn->optionalParams) + "], ";
      }

      code += paramsCode;
    }

    code += ");\n";
  }

  codegen->functionDeclarationsCode += code;
}

void codegenFunctionDefinitions (Codegen *codegen, const std::vector<Stmt *> &body) {
  auto code = std::string();

  for (auto stmt : body) {
    if (stmt->type != STMT_FN_DECL) {
      continue;
    }

    auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
    auto hiddenName = codegenIdentifier(stmtFnDecl->id);
    auto fnVar = codegen->varMap->getFn(hiddenName);

    codegen->varMap->save();
    codegen->stack.emplace_back(fnVar->fn->hiddenName);
    code += codegenTypeStr(fnVar->fn->returnType);
    code += fnVar->name + " (";

    if (stmtFnDecl->params.empty()) {
      code += ") {\n";
    } else {
      auto paramsCode = std::string();
      auto optionalParams = static_cast<std::size_t>(0);
      auto optionalParamsCode = std::string();
      auto idx = static_cast<std::size_t>(0);

      for (auto param : fnVar->fn->params) {
        auto stmtParam = stmtFnDecl->params[idx];
        auto optionalParamName = "OP_" + std::to_string(optionalParams + 1);
        auto paramTypeStr = codegenTypeStr(param->type);

        paramsCode += idx == 0 ? "" : ", ";
        paramsCode += paramTypeStr;
        paramsCode += param->required ? param->name : optionalParamName;

        if (!param->required) {
          optionalParamsCode += std::string(2, ' ') + paramTypeStr;
          optionalParamsCode += param->name + " = OP[" + std::to_string(optionalParams) + "] == 0 ? ";
          optionalParamsCode += codegenStmtExpr(codegen, stmtParam->init) + " : " + optionalParamName;
          optionalParamsCode += ";\n";
          optionalParams++;
        }

        codegen->varMap->add(param->type, param->name);
        idx++;
      }

      if (optionalParams > 0) {
        code += "const char OP[" + std::to_string(optionalParams) + "], ";
      }

      code += paramsCode;
      code += ") {\n";
      code += optionalParamsCode;
    }

    code += codegenBlock(codegen, stmtFnDecl->body, 2);
    code += "}\n\n";
    codegen->stack.pop_back();
    codegen->varMap->restore();
  }

  codegen->functionDefinitionsCode += code;
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

std::string codegenStmtObjVarDecl (
  Codegen *codegen,
  const Expr *expr,
  const std::string &id,
  bool mut,
  std::size_t indent
) {
  auto code = std::string();
  auto exprObj = std::get<ExprObj *>(expr->body);
  auto varObj = codegen->varMap->getObj(codegenIdentifier(exprObj->type));

  for (auto field : varObj->obj->fields) {
    auto propFound = std::find_if(exprObj->props.begin(), exprObj->props.end(), [&field] (auto it) -> bool {
      return it->id->name->val == field->name;
    });

    if (propFound == exprObj->props.end()) {
      throw Error("Object literal field is not set");
    }

    auto prop = *propFound;

    if (field->type == VAR_OBJ) {
      auto init = std::get<Expr *>(prop->init->body);
      code += codegenStmtObjVarDecl(codegen, init, codegenExprMember(id, field->name), mut, indent);
    } else {
      auto propName = codegenExprMember(id, field->name);

      code += std::string(indent, ' ');
      code += codegenTypeStr(field->type, mut);
      code += propName;
      code += " = ";
      code += codegenStmtExpr(codegen, prop->init);
      code += ";\n";

      codegen->varMap->add(field->type, propName);
    }
  }

  return code;
}

std::string codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent) {
  auto code = std::string();

  if (stmt->type == STMT_BREAK) {
    code += std::string(indent, ' ');
    code += "break;\n";
  } else if (stmt->type == STMT_CONTINUE) {
    code += std::string(indent, ' ');
    code += "continue;\n";
  } else if (stmt->type == STMT_EXPR) {
    code += std::string(indent, ' ');
    code += codegenStmtExpr(codegen, std::get<StmtExpr *>(stmt->body));
    code += ";\n";
  } else if (stmt->type == STMT_FN_DECL) {
    auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
    auto hiddenName = codegenIdentifier(stmtFnDecl->id);
    auto fnName = codegen->varMap->genId(codegen->stack, hiddenName);
    auto returnType = codegenType(codegen, stmtFnDecl->returnType);
    auto params = std::vector<VarMapFnParam *>();
    auto optionalParams = static_cast<std::size_t>(0);

    for (auto param : stmtFnDecl->params) {
      auto paramName = codegenIdentifier(param->id);
      auto paramType = codegenType(codegen, param->type, param->init);
      auto paramRequired = param->init == nullptr;
      auto fnParam = new VarMapFnParam{paramName, paramType, paramRequired};

      if (!paramRequired) {
        optionalParams++;
      }

      params.push_back(fnParam);
    }

    codegen->varMap->addFn(fnName, hiddenName, returnType, params, optionalParams);
  } else if (stmt->type == STMT_IF) {
    code += std::string(indent, ' ');
    code += codegenStmtIf(codegen, std::get<StmtIf *>(stmt->body), indent);
    code += "\n";
  } else if (stmt->type == STMT_LOOP) {
    auto stmtLoop = std::get<StmtLoop *>(stmt->body);

    codegen->varMap->save();
    code += std::string(indent, ' ');

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
  } else if (stmt->type == STMT_OBJ_DECL) {
    auto stmtObjDecl = std::get<StmtObjDecl *>(stmt->body);
    auto hiddenName = codegenIdentifier(stmtObjDecl->id);
    auto objName = codegen->varMap->genId(codegen->stack, hiddenName);
    auto fields = std::vector<VarMapObjField *>();

    for (auto field : stmtObjDecl->fields) {
      auto fieldType = codegenType(codegen, field->type);
      auto objField = new VarMapObjField{field->id->name->val, fieldType};

      fields.push_back(objField);
    }

    codegen->varMap->addObj(objName, hiddenName, fields);
  } else if (stmt->type == STMT_RETURN) {
    auto stmtReturn = std::get<StmtReturn *>(stmt->body);

    code += std::string(indent, ' ');
    code += "return ";
    code += codegenStmtExpr(codegen, stmtReturn->arg);
    code += ";\n";
  } else if (stmt->type == STMT_VAR_DECL) {
    auto stmtVarDecl = std::get<StmtVarDecl *>(stmt->body);

    auto exprType = stmtVarDecl->type == nullptr
      ? codegenStmtExprType(codegen, stmtVarDecl->init)
      : codegenType(codegen, stmtVarDecl->type);

    if (exprType == VAR_OBJ) {
      auto init = std::get<Expr *>(stmtVarDecl->init->body);
      code += codegenStmtObjVarDecl(codegen, init, codegenIdentifier(stmtVarDecl->id), stmtVarDecl->mut, indent);
    } else {
      auto name = codegenIdentifier(stmtVarDecl->id);

      code += std::string(indent, ' ');
      code += codegenTypeStr(exprType, stmtVarDecl->mut);
      code += name;
      code += " = ";
      code += codegenStmtExpr(codegen, stmtVarDecl->init);
      code += ";\n";

      codegen->varMap->add(exprType, name);
    }
  } else {
    throw Error("Tried to access unknown stmt type");
  }

  return code;
}

std::string codegen (const AST *ast) {
  auto codegen = new Codegen{new VarMap{}};
  codegen->stack.reserve(SHRT_MAX);

  for (auto stmt : ast->topLevelStatements) {
    codegen->topLevelStatementsCode += codegenStmt(codegen, stmt, 0);
  }

  codegenFunctionDeclarations(codegen, ast->topLevelStatements);
  codegenFunctionDefinitions(codegen, ast->topLevelStatements);

  codegen->varMap->save();
  codegen->stack.emplace_back("main");
  codegen->mainBodyCode += "int main () {\n";

  if (ast->mainPresent) {
    codegen->mainBodyCode += codegenBlock(codegen, ast->mainBody, 2);
  }

  codegen->mainBodyCode += "}\n";
  codegen->stack.pop_back();
  codegen->varMap->restore();

  auto banner = std::string("/*!\n") +
    " * Copyright (c) Aaron Delasy\n" +
    " *\n"+
    " * Unauthorized copying of this file, via any medium is strictly prohibited\n"+
    " * Proprietary and confidential\n"+
    " */\n\n";

  auto polyfills = std::string(codegen->polyfills.pow ? "double pow (double, double);\n" : "") +
    std::string(codegen->polyfills.printf ? "int printf (const char *, ...);\n" : "");

  auto functionDeclarationsCode = polyfills + codegen->functionDeclarationsCode;

  auto code = banner +
    (functionDeclarationsCode.empty() ? "" : functionDeclarationsCode + "\n") +
    (codegen->topLevelStatementsCode.empty() ? "" : codegen->topLevelStatementsCode + "\n") +
    codegen->functionDefinitionsCode +
    codegen->mainBodyCode;

  delete codegen;
  return code;
}
