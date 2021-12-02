/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <algorithm>
#include "Codegen.hpp"
#include "Error.hpp"

std::string codegenStmt (Codegen *codegen, const Stmt *stmt, std::size_t indent);

Codegen::~Codegen () {
  for (auto fnDecl : this->fnDecls) {
    delete fnDecl;
  }

  delete this->varMap;
}

std::string codegenBlock (Codegen *codegen, const Block *block, std::size_t indent) {
  auto code = std::string();

  for (auto stmt : block->body) {
    code += codegenStmt(codegen, stmt, indent);
  }

  return code;
}

VarMapItemType codegenIdentifierType (Codegen *codegen, const Identifier *id) {
  if (id->name->val == "bool") {
    codegen->headers.boolean = true;
    return VAR_BOOL;
  }

  if (id->name->val == "byte") return VAR_BYTE;
  if (id->name->val == "char") return VAR_CHAR;
  if (id->name->val == "float") return VAR_FLOAT;
  if (id->name->val == "f32") return VAR_F32;
  if (id->name->val == "f64") return VAR_F64;
  if (id->name->val == "int") return VAR_INT;
  if (id->name->val == "i8") return VAR_I8;
  if (id->name->val == "i16") return VAR_I16;
  if (id->name->val == "i32") return VAR_I32;
  if (id->name->val == "i64") return VAR_I64;
  if (id->name->val == "str") return VAR_STR;
  if (id->name->val == "u8") return VAR_U8;
  if (id->name->val == "u16") return VAR_U16;
  if (id->name->val == "u32") return VAR_U32;
  if (id->name->val == "u64") return VAR_U64;

  throw Error("Tried to access unknown identifier type");
}

std::string codegenIdentifier (const Identifier *id) {
  return "__THE_" + id->name->val;
}

std::string codegenExprMember (const std::string &obj, const std::string &prop) {
  return obj + "MA_95" + prop;
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
      return codegen->varMap->get(name).type;
    } else if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      auto name = codegenExprAccess(exprAssign->left);

      return codegen->varMap->get(name).type;
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

      return codegen->varMap->getFn(name).returnType;
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

    return codegen->varMap->get(name).type;
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    auto lit = std::get<Literal *>(stmtExpr->body);

    if (lit->val->type == TK_KW_FALSE || lit->val->type == TK_KW_TRUE) {
      codegen->headers.boolean = true;
      return VAR_BOOL;
    }

    if (lit->val->type == TK_LIT_CHAR) return VAR_CHAR;
    if (lit->val->type == TK_LIT_FLOAT) return VAR_FLOAT;
    if (lit->val->type == TK_LIT_INT_DEC) return VAR_INT;
    if (lit->val->type == TK_LIT_STR) return VAR_STR;
  }

  throw Error("Tried to access unknown expr type");
}

VarMapItemType codegenType (Codegen *codegen, const StmtFnDeclParam *param) {
  if (param->type != nullptr && param->type->name->val == "bool") {
    codegen->headers.boolean = true;
    return VAR_BOOL;
  }

  if (param->type != nullptr && param->type->name->val == "byte") return VAR_BYTE;
  if (param->type != nullptr && param->type->name->val == "char") return VAR_CHAR;
  if (param->type != nullptr && param->type->name->val == "float") return VAR_FLOAT;
  if (param->type != nullptr && param->type->name->val == "f32") return VAR_F32;
  if (param->type != nullptr && param->type->name->val == "f64") return VAR_F64;
  if (param->type != nullptr && param->type->name->val == "int") return VAR_INT;
  if (param->type != nullptr && param->type->name->val == "i8") return VAR_I8;
  if (param->type != nullptr && param->type->name->val == "i16") return VAR_I16;
  if (param->type != nullptr && param->type->name->val == "i32") return VAR_I32;
  if (param->type != nullptr && param->type->name->val == "i64") return VAR_I64;
  if (param->type != nullptr && param->type->name->val == "str") return VAR_STR;
  if (param->type != nullptr && param->type->name->val == "u8") return VAR_U8;
  if (param->type != nullptr && param->type->name->val == "u16") return VAR_U16;
  if (param->type != nullptr && param->type->name->val == "u32") return VAR_U32;
  if (param->type != nullptr && param->type->name->val == "u64") return VAR_U64;

  return codegenStmtExprType(codegen, param->init);
}

VarMapItemType codegenType (Codegen *codegen, const StmtObjDeclField *field) {
  if (field->type->name->val == "bool") {
    codegen->headers.boolean = true;
    return VAR_BOOL;
  }

  if (field->type->name->val == "byte") return VAR_BYTE;
  if (field->type->name->val == "char") return VAR_CHAR;
  if (field->type->name->val == "float") return VAR_FLOAT;
  if (field->type->name->val == "f32") return VAR_F32;
  if (field->type->name->val == "f64") return VAR_F64;
  if (field->type->name->val == "int") return VAR_INT;
  if (field->type->name->val == "i8") return VAR_I8;
  if (field->type->name->val == "i16") return VAR_I16;
  if (field->type->name->val == "i32") return VAR_I32;
  if (field->type->name->val == "i64") return VAR_I64;
  if (field->type->name->val == "str") return VAR_STR;
  if (field->type->name->val == "u8") return VAR_U8;
  if (field->type->name->val == "u16") return VAR_U16;
  if (field->type->name->val == "u32") return VAR_U32;
  if (field->type->name->val == "u64") return VAR_U64;

  try {
    auto name = codegenIdentifier(field->type);
    codegen->varMap->getObj(name);
    return VAR_OBJ;
  } catch (const Error &err) {
    throw Error("Tried to access unknown type");
  }
}

std::string codegenTypeStr (VarMapItemType type, bool mut = false) {
  if (type == VAR_BOOL) return mut ? "bool " : "const bool ";
  if (type == VAR_BYTE) return mut ? "unsigned char " : "const unsigned char ";
  if (type == VAR_CHAR) return mut ? "char " : "const char ";
  if (type == VAR_FLOAT || type == VAR_F64) return mut ? "double " : "const double ";
  if (type == VAR_F32) return mut ? "float " : "const float ";
  if (type == VAR_INT || type == VAR_I32) return mut ? "long " : "const long ";
  if (type == VAR_I8) return mut ? "char " : "const char ";
  if (type == VAR_I16) return mut ? "short " : "const short ";
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

      if (
        exprCall->callee->type == EXPR_ACCESS_IDENTIFIER &&
        std::get<Identifier *>(exprCall->callee->body)->name->val == "print"
      ) {
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

          if (exprType == VAR_BOOL || exprType == VAR_STR) code += "%s";
          else if (exprType == VAR_BYTE) code += "%x";
          else if (exprType == VAR_CHAR) code += "%c";
          else if (exprType == VAR_FLOAT || exprType == VAR_F32 || exprType == VAR_F64) code += "%f";
          else if (exprType == VAR_INT || exprType == VAR_I32) code += "%ld";
          else if (exprType == VAR_I8 || exprType == VAR_I16) code += "%d";
          else if (exprType == VAR_I64) code += "%lld";
          else if (exprType == VAR_U8 || exprType == VAR_U16) code += "%u";
          else if (exprType == VAR_U32) code += "%lu";
          else if (exprType == VAR_U64) code += "%llu";
          else throw Error("Unknown print argument type");

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
            code += R"() == true ? "true" : "false")";
          }

          code += ", ";

          argIdx++;
        }

        code += terminator + ")";
      } else {
        auto fnName = codegenExprAccess(exprCall->callee);
        auto fnMapIt = codegen->varMap->getFn(fnName);

        code += fnName;
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
    auto id = std::get<Identifier *>(stmtExpr->body);
    code += codegenIdentifier(id);
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

  for (auto field : varObj.fields) {
    auto propFound = std::find_if(exprObj->props.begin(), exprObj->props.end(), [&field] (auto it) -> bool {
      return it->id->name->val == field->name;
    });

    if (propFound == exprObj->props.end()) {
      throw Error("Object literal field is not set");
    }

    auto prop = *propFound;

    if (field->type == VAR_OBJ) {
      code += codegenStmtObjVarDecl(
        codegen,
        std::get<Expr *>(prop->init->body),
        codegenExprMember(id, field->name),
        mut,
        indent
      );
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
    auto returnType = codegenIdentifierType(codegen, stmtFnDecl->returnType);
    auto fnName = codegenIdentifier(stmtFnDecl->id);

    code += std::string(indent, ' ');
    code += codegenTypeStr(returnType);
    code += fnName + " (";

    auto params = std::vector<VarMapFnParam *>();
    auto optionalParams = static_cast<std::size_t>(0);

    if (stmtFnDecl->params.empty()) {
      code += "void";
    } else {
      auto paramsCode = std::string();

      for (auto it = stmtFnDecl->params.begin(); it != stmtFnDecl->params.end(); it++) {
        auto idx = it - stmtFnDecl->params.begin();
        auto param = *it;
        auto paramType = codegenType(codegen, param);
        auto paramName = codegenIdentifier(param->id);
        auto paramRequired = param->init == nullptr;

        paramsCode += idx == 0 ? "" : ", ";
        auto paramCode = codegenTypeStr(paramType);
        paramsCode += paramCode.back() == ' ' ? paramCode.substr(0, paramCode.length() - 1) : paramCode;

        auto itParam = new VarMapFnParam{paramName, codegenType(codegen, param), paramRequired};
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
    codegen->varMap->addFn(fnName, returnType, params, optionalParams);
    codegen->fnDecls.push_back(new CodegenFnDecl{fnName, stmtFnDecl});
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
    auto name = codegenIdentifier(stmtObjDecl->id);
    auto fields = std::vector<VarMapObjField *>();

    for (auto stmtField : stmtObjDecl->fields) {
      auto field = new VarMapObjField{stmtField->id->name->val, codegenType(codegen, stmtField)};
      fields.push_back(field);
    }

    codegen->varMap->addObj(name, fields);
  } else if (stmt->type == STMT_RETURN) {
    auto stmtReturn = std::get<StmtReturn *>(stmt->body);

    code += std::string(indent, ' ');
    code += "return ";
    code += codegenStmtExpr(codegen, stmtReturn->arg);
    code += ";\n";
  } else if (stmt->type == STMT_VAR_DECL) {
    auto stmtVarDecl = std::get<StmtVarDecl *>(stmt->body);
    auto exprType = codegenStmtExprType(codegen, stmtVarDecl->init);

    if (exprType == VAR_OBJ) {
      code += codegenStmtObjVarDecl(
        codegen,
        std::get<Expr *>(stmtVarDecl->init->body),
        codegenIdentifier(stmtVarDecl->id),
        stmtVarDecl->mut,
        indent
      );
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
  auto codegen = new Codegen{{}, "", {}, new VarMap{}};

  for (auto stmt : ast->topLevelStmts) {
    codegen->body += codegenStmt(codegen, stmt, 0);
  }

  if (!codegen->body.empty()) {
    codegen->body += "\n";
  }

  codegen->varMap->save();
  codegen->body += "int main () {\n";

  if (ast->mainPresent) {
    codegen->body += codegenBlock(codegen, ast->mainBody, 2);
  }

  codegen->body += "}\n";
  codegen->varMap->restore();

  for (auto it1 = codegen->fnDecls.begin(); it1 != codegen->fnDecls.end(); it1++) {
    auto fnDecl = *it1;
    auto returnType = codegenIdentifierType(codegen, fnDecl->stmt->returnType);

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
      auto idx = 0;

      for (auto param : fnDecl->stmt->params) {
        auto paramName = codegenIdentifier(param->id);
        auto optionalParamName = "OP_" + std::to_string(optionalParams + 1);
        auto paramType = codegenType(codegen, param);
        auto paramTypeStr = codegenTypeStr(paramType);
        auto paramRequired = param->init == nullptr;

        paramsCode += idx == 0 ? "" : ", ";
        paramsCode += paramTypeStr;
        paramsCode += paramRequired ? paramName : optionalParamName;

        if (!paramRequired) {
          optionalParamsCode += std::string(2, ' ') + paramTypeStr;
          optionalParamsCode += paramName + " = OP[" + std::to_string(optionalParams) + "] == 0 ? ";
          optionalParamsCode += codegenStmtExpr(codegen, param->init) + " : " + optionalParamName;
          optionalParamsCode += ";\n";

          optionalParams++;
        }

        codegen->varMap->add(paramType, paramName);
        idx++;
      }

      if (optionalParams > 0) {
        codegen->body += "const char OP[" + std::to_string(optionalParams) + "], ";
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
    std::string(codegen->headers.boolean ? "#include <stdbool.h>\n" : "") +
    std::string(codegen->headers.stdio ? "#include <stdio.h>\n" : "");

  auto code = (headers.empty() ? headers : headers + "\n") + codegen->body;
  delete codegen;

  return code;
}
