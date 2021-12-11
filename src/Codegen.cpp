/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <algorithm>
#include <climits>
#include <tuple>
#include "Codegen.hpp"
#include "Error.hpp"

using CodegenReturn = std::tuple<std::string, std::string, std::string>;

void codegenDeclarations (Codegen *codegen, const std::vector<Stmt *> &body);
void codegenDefinitions (Codegen *codegen, const std::vector<Stmt *> &body);
CodegenReturn codegenStmt (Codegen *codegen, const Stmt *stmt);

Codegen::~Codegen () {
  delete this->varMap;
}

std::string codegenBlock (Codegen *codegen, const Block *block) {
  auto code = std::string();
  auto cleanup = std::string();
  codegen->indent += 2;

  for (auto stmt : block->body) {
    auto [stmtSetUp, stmtCode, stmtCleanUp] = codegenStmt(codegen, stmt);

    code += stmtSetUp + stmtCode;
    cleanup += stmtCleanUp;
  }

  codegen->indent -= 2;
  codegenDeclarations(codegen, block->body);
  codegenDefinitions(codegen, block->body);

  return code + cleanup;
}

std::string codegenIdentifier (const Identifier *id) {
  return "__THE_" + id->name->val;
}

std::string codegenExprAccess (const ExprAccess *exprAccess) {
  if (exprAccess->type == EXPR_ACCESS_EXPR_MEMBER) {
    auto exprMemberObj = std::get<ExprMember *>(exprAccess->body);
    return codegenExprAccess(exprMemberObj->obj) + "->" + exprMemberObj->prop->name->val;
  } else if (exprAccess->type == EXPR_ACCESS_IDENTIFIER) {
    return codegenIdentifier(std::get<Identifier *>(exprAccess->body));
  }

  throw Error("Tried to access unknown expression access");
}

std::string codegenObjInitFn (const std::string &name) {
  return name.substr(6) + "_init";
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

    if (lit->val->type == TK_KW_FALSE || lit->val->type == TK_KW_TRUE) {
      codegen->headers.stdbool = true;
    } else if (lit->val->type == TK_LIT_STR) {
      codegen->functions.str_from_cstr = true;
    }

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
    if (type->name->val == "bool") {
      codegen->headers.stdbool = true;
    }

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
  if (type == VAR_FN) return "%s";
  if (type == VAR_OBJ) return "%s";
  if (type == VAR_STR) return "%s";
  if (type == VAR_U8) return "%u";
  if (type == VAR_U16) return "%u";
  if (type == VAR_U32) return "%lu";
  if (type == VAR_U64) return "%llu";

  throw Error("Unknown print argument type");
}

std::string codegenTypeStr (VarMapItemType type, bool mut = false) {
  if (type == VAR_BOOL) return std::string(mut ? "" : "const ") + "bool ";
  if (type == VAR_BYTE) return std::string(mut ? "" : "const ") + "unsigned char ";
  if (type == VAR_CHAR) return std::string(mut ? "" : "const ") + "char ";
  if (type == VAR_FLOAT) return std::string(mut ? "" : "const ") + "double ";
  if (type == VAR_F32) return std::string(mut ? "" : "const ") + "float ";
  if (type == VAR_F64) return std::string(mut ? "" : "const ") + "double ";
  if (type == VAR_INT) return std::string(mut ? "" : "const ") + "long ";
  if (type == VAR_I8) return std::string(mut ? "" : "const ") + "char ";
  if (type == VAR_I16) return std::string(mut ? "" : "const ") + "short ";
  if (type == VAR_I32) return std::string(mut ? "" : "const ") + "long ";
  if (type == VAR_I64) return std::string(mut ? "" : "const ") + "long long ";
  if (type == VAR_STR) return std::string(mut ? "" : "const ") + "struct str *";
  if (type == VAR_U8) return std::string(mut ? "" : "const ") + "unsigned char ";
  if (type == VAR_U16) return std::string(mut ? "" : "const ") + "unsigned short ";
  if (type == VAR_U32) return std::string(mut ? "" : "const ") + "unsigned long ";
  if (type == VAR_U64) return std::string(mut ? "" : "const ") + "unsigned long long ";

  throw Error("Tried to access unknown type str");
}

std::string codegenTypeStr (VarMapItemType type, const std::string &name, bool mut = false) {
  if (type == VAR_OBJ) {
    return std::string(mut ? "" : "const ") + "struct " + name + " *";
  }

  return codegenTypeStr(type, mut);
}

CodegenReturn codegenStmtExpr (Codegen *codegen, const StmtExpr *stmtExpr) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (stmtExpr->parenthesized) {
    code += "(";
  }

  if (stmtExpr->type == STMT_EXPR_EXPR) {
    auto expr = std::get<Expr *>(stmtExpr->body);

    if (expr->type == EXPR_ACCESS) {
      code += codegenExprAccess(std::get<ExprAccess *>(expr->body));
    } else if (expr->type == EXPR_ASSIGN) {
      auto exprAssign = std::get<ExprAssign *>(expr->body);
      auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, exprAssign->right);

      code += codegenExprAccess(exprAssign->left);

      if (exprAssign->op->type == TK_OP_STAR_STAR_EQ) {
        code += " = pow(" + codegenExprAccess(exprAssign->left) + ", " + stmtExprCode + ")";
      } else if (exprAssign->op->type == TK_OP_AND_AND_EQ) {
        code += " = " + codegenExprAccess(exprAssign->left) + " && " + stmtExprCode;
      } else if (exprAssign->op->type == TK_OP_OR_OR_EQ) {
        code += " = " + codegenExprAccess(exprAssign->left) + " || " + stmtExprCode;
      } else {
        code += " " + exprAssign->op->val + " " + stmtExprCode;
      }

      setUp += stmtExprSetUp;
      cleanUp += stmtExprCleanUp;
    } else if (expr->type == EXPR_BINARY) {
      auto exprBinary = std::get<ExprBinary *>(expr->body);
      auto [stmtExprLeftSetUp, stmtExprLeftCode, stmtExprLeftCleanUp] = codegenStmtExpr(codegen, exprBinary->left);
      auto [stmtExprRightSetUp, stmtExprRightCode, stmtExprRightCleanUp] = codegenStmtExpr(codegen, exprBinary->right);

      if (exprBinary->op->type == TK_OP_STAR_STAR) {
        codegen->headers.math = true;
        code += "pow(" + stmtExprLeftCode + ", " + stmtExprRightCode + ")";
      } else if (exprBinary->op->type == TK_OP_SLASH || exprBinary->op->type == TK_OP_STAR) {
        code += "(double) " + stmtExprLeftCode + " " + exprBinary->op->val + " (double) " + stmtExprRightCode;
      } else {
        code += stmtExprLeftCode + " " + exprBinary->op->val + " " + stmtExprRightCode;
      }

      setUp += stmtExprLeftSetUp;
      setUp += stmtExprRightSetUp;
      cleanUp += stmtExprRightCleanUp;
      cleanUp += stmtExprLeftCleanUp;
    } else if (expr->type == EXPR_CALL) {
      auto exprCall = std::get<ExprCall *>(expr->body);

      if (
        exprCall->callee->type == EXPR_ACCESS_IDENTIFIER &&
        std::get<Identifier *>(exprCall->callee->body)->name->val == "print"
      ) {
        codegen->headers.stdio = true;
        code += R"(printf(")";

        auto separator = std::string(R"(" ")");
        auto terminator = std::string(R"("\n")");
        auto argIdx = static_cast<std::size_t>(0);

        for (auto arg : exprCall->args) {
          if (arg->id != nullptr && arg->id->name->val == "separator") {
            if (arg->expr->type == STMT_EXPR_LITERAL) {
              auto lit = std::get<Literal *>(arg->expr->body);

              if (lit->val->type == TK_LIT_STR) {
                separator = lit->val->val;
                continue;
              }
            }

            auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, arg->expr);

            separator = stmtExprCode;
            setUp += stmtExprSetUp;
            cleanUp += stmtExprCleanUp;
          } else if (arg->id != nullptr && arg->id->name->val == "terminator") {
            if (arg->expr->type == STMT_EXPR_LITERAL) {
              auto lit = std::get<Literal *>(arg->expr->body);

              if (lit->val->type == TK_LIT_STR) {
                terminator = lit->val->val;
                continue;
              }
            }

            auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, arg->expr);

            terminator = stmtExprCode;
            setUp += stmtExprSetUp;
            cleanUp += stmtExprCleanUp;
          } else if (arg->id != nullptr) {
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

          code += argIdx != 0 ? separator + ", " : "";

          if (arg->expr->type == STMT_EXPR_LITERAL) {
            auto lit = std::get<Literal *>(arg->expr->body);

            if (lit->val->type == TK_LIT_STR) {
              code += lit->val->val + ", ";
              argIdx++;

              continue;
            }
          }

          auto exprType = codegenStmtExprType(codegen, arg->expr);
          auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, arg->expr);

          if (exprType == VAR_BOOL || exprType == VAR_STR) {
            code += "(";
          }

          code += stmtExprCode;

          if (exprType == VAR_BOOL) {
            code += R"() == true ? "true" : "false")";
          } else if (exprType == VAR_STR) {
            code += ")->c";
          }

          code += ", ";
          setUp += stmtExprSetUp;
          cleanUp += stmtExprCleanUp;

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
            auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, exprCall->args[idx]->expr);

            argsCode += stmtExprCode;
            setUp += stmtExprSetUp;
            cleanUp += stmtExprCleanUp;

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
            if (param->type == VAR_BOOL) {
              argsCode += "false";
            } else if (
              param->type == VAR_BYTE ||
              param->type == VAR_FLOAT ||
              param->type == VAR_F32 ||
              param->type == VAR_F64 ||
              param->type == VAR_INT ||
              param->type == VAR_I8 ||
              param->type == VAR_I16 ||
              param->type == VAR_I32 ||
              param->type == VAR_I64 ||
              param->type == VAR_U8 ||
              param->type == VAR_U16 ||
              param->type == VAR_U32 ||
              param->type == VAR_U64
            ) {
              argsCode += "0";
            } else if (param->type == VAR_CHAR) {
              argsCode += R"('\0')";
            } else if (param->type == VAR_STR) {
              codegen->functions.str_init = true;
              argsCode += R"(str_init(0))";
            }
          } else {
            auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, foundArg->expr);

            argsCode += stmtExprCode;
            setUp += stmtExprSetUp;
            cleanUp += stmtExprCleanUp;
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
      auto [stmtExprCondSetUp, stmtExprCondCode, stmtExprCondCleanUp] = codegenStmtExpr(codegen, exprCond->cond);
      auto [stmtExprBodySetUp, stmtExprBodyCode, stmtExprBodyCleanUp] = codegenStmtExpr(codegen, exprCond->body);
      auto [stmtExprAltSetUp, stmtExprAltCode, stmtExprAltCleanUp] = codegenStmtExpr(codegen, exprCond->alt);

      code += stmtExprCondCode;
      code += " ? ";
      code += stmtExprBodyCode;
      code += " : ";
      code += stmtExprAltCode;

      setUp += stmtExprCondSetUp + stmtExprBodySetUp + stmtExprAltSetUp;
      cleanUp += stmtExprCondCleanUp + stmtExprBodyCleanUp + stmtExprAltCleanUp;
    } else if (expr->type == EXPR_UNARY) {
      auto exprUnary = std::get<ExprUnary *>(expr->body);
      auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, exprUnary->arg);

      if (exprUnary->prefix) {
        code += exprUnary->op->val;
        code += stmtExprCode;
      } else {
        code += stmtExprCode;
        code += exprUnary->op->val;
      }

      setUp += stmtExprSetUp;
      cleanUp += stmtExprCleanUp;
    }
  } else if (stmtExpr->type == STMT_EXPR_IDENTIFIER) {
    auto id = std::get<Identifier *>(stmtExpr->body);
    code += codegenIdentifier(id);
  } else if (stmtExpr->type == STMT_EXPR_LITERAL) {
    auto lit = std::get<Literal *>(stmtExpr->body);

    if (lit->val->type == TK_LIT_INT_OCT) {
      auto val = lit->val->val;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      code += val;
    } else if (lit->val->type == TK_LIT_STR) {
      auto strVar = codegen->varMap->getNewStr();
      auto valSize = std::to_string(lit->val->val.size() - 2);

      code += strVar->name;
      setUp += std::string(codegen->indent, ' ');
      setUp += "struct str *" + strVar->name + " = str_from_cstr(" + lit->val->val + ", " + valSize + ");\n";
      cleanUp += std::string(codegen->indent, ' ') + "str_deinit(&" + strVar->name + ");\n";
    } else {
      code += lit->val->val;
    }
  } else {
    throw Error("Tried to access unknown expr");
  }

  if (stmtExpr->parenthesized) {
    code += ")";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

void codegenDeclarations (Codegen *codegen, const std::vector<Stmt *> &body) {
  for (auto stmt : body) {
    if (stmt->type == STMT_FN_DECL) {
      auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
      auto hiddenName = codegenIdentifier(stmtFnDecl->id);
      auto fnVar = codegen->varMap->getFn(hiddenName);

      codegen->functionDeclarationsCode += codegenTypeStr(fnVar->fn->returnType);
      codegen->functionDeclarationsCode += fnVar->name + " (";

      if (fnVar->fn->params.empty()) {
        codegen->functionDeclarationsCode += "void";
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
          codegen->functionDeclarationsCode += "const char [" + std::to_string(fnVar->fn->optionalParams) + "], ";
        }

        codegen->functionDeclarationsCode += paramsCode;
      }

      codegen->functionDeclarationsCode += ");\n";
    } else if (stmt->type == STMT_OBJ_DECL) {
      codegen->headers.stdlib = true;

      auto stmtObjDecl = std::get<StmtObjDecl *>(stmt->body);
      auto hiddenName = codegenIdentifier(stmtObjDecl->id);
      auto objVar = codegen->varMap->getObj(hiddenName);

      codegen->structDeclarationsCode += "struct " + objVar->name + " {\n";

      for (auto field : objVar->obj->fields) {
        codegen->structDeclarationsCode += "  " + codegenTypeStr(field->type, field->typeName, true);
        codegen->structDeclarationsCode += field->name;
        codegen->structDeclarationsCode += ";\n";
      }

      codegen->structDeclarationsCode += "};\n\n";
      codegen->functionDeclarationsCode += "struct " + objVar->name + " *";
      codegen->functionDeclarationsCode += codegenObjInitFn(objVar->name) + " (struct " + objVar->name + ");\n";
    }
  }
}

void codegenDefinitions (Codegen *codegen, const std::vector<Stmt *> &body) {
  for (auto stmt : body) {
    if (stmt->type == STMT_FN_DECL) {
      auto stmtFnDecl = std::get<StmtFnDecl *>(stmt->body);
      auto hiddenName = codegenIdentifier(stmtFnDecl->id);
      auto fnVar = codegen->varMap->getFn(hiddenName);
      auto code = std::string();
      auto setUp = std::string();
      auto cleanUp = std::string();

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
            auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtParam->init);

            optionalParamsCode += "  " + paramTypeStr;
            optionalParamsCode += param->name + " = OP[" + std::to_string(optionalParams) + "] == 0 ? ";
            optionalParamsCode += stmtExprCode + " : ";
            optionalParamsCode += optionalParamName + ";\n";

            setUp += stmtExprSetUp;
            cleanUp += stmtExprCleanUp;

            optionalParams++;
          }

          codegen->varMap->add(param->type, param->name);
          idx++;
        }

        if (optionalParams > 0) {
          code += "const char OP[" + std::to_string(optionalParams) + "], ";
        }

        code += paramsCode + ") {\n";
        code += optionalParamsCode;
      }

      code += setUp;
      code += codegenBlock(codegen, stmtFnDecl->body);
      code += cleanUp;
      code += "}\n\n";
      codegen->stack.pop_back();
      codegen->varMap->restore();

      codegen->functionDefinitionsCode += code;
    } else if (stmt->type == STMT_OBJ_DECL) {
      codegen->headers.stdio = true;
      codegen->headers.stdlib = true;
      codegen->headers.string = true;

      auto stmtObjDecl = std::get<StmtObjDecl *>(stmt->body);
      auto hiddenName = codegenIdentifier(stmtObjDecl->id);
      auto objVar = codegen->varMap->getObj(hiddenName);
      auto code = std::string();

      code += "struct " + objVar->name + " *";
      code += codegenObjInitFn(objVar->name) + " (struct " + objVar->name + " x) {\n";
      code += "  size_t l = sizeof(struct " + objVar->name + ");\n";
      code += "  " + codegenTypeStr(objVar->type, objVar->name, true) + "n = malloc(l);\n";
      code += "  if (n == NULL) {\n";
      code += R"(    fprintf(stderr, "Error: Failed to allocate %zu bytes for object \")";
      code += stmtObjDecl->id->name->val + R"(\"\n", l);)" + "\n";
      code += "    exit(EXIT_FAILURE);\n";
      code += "  }\n";
      code += "  memcpy(n, &x, l);\n";
      code += "  return n;\n";
      code += "}\n\n";

      codegen->functionDefinitionsCode += code;
    }
  }
}

std::string codegenStmtIf (Codegen *codegen, const StmtIf *stmtIf) {
  auto code = std::string();
  auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtIf->cond);

  codegen->varMap->save();
  code += "if (";
  code += stmtExprSetUp;
  code += stmtExprCode;
  code += ") {\n";
  code += codegenBlock(codegen, stmtIf->body);
  code += stmtExprCleanUp;
  codegen->varMap->restore();

  if (stmtIf->alt != nullptr) {
    code += std::string(codegen->indent, ' ') + "} else ";

    if (stmtIf->alt->type == COND_BLOCK) {
      code += "{\n";
      codegen->varMap->save();
      code += codegenBlock(codegen, std::get<Block *>(stmtIf->alt->body));
      code += std::string(codegen->indent, ' ') + "}";
      codegen->varMap->restore();
    } else if (stmtIf->alt->type == COND_STMT_IF) {
      code += codegenStmtIf(codegen, std::get<StmtIf *>(stmtIf->alt->body));
    }
  } else {
    code += std::string(codegen->indent, ' ') + "}";
  }

  return code;
}

CodegenReturn codegenStmtObjVarDecl (
  Codegen *codegen,
  bool rootDecl,
  const std::string &id,
  bool mut,
  const ExprObj *exprObj
) {
  auto objName = codegenIdentifier(exprObj->type);
  auto objVar = codegen->varMap->getObj(objName);
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (rootDecl) {
    code += std::string(codegen->indent, ' ');
    code += codegenTypeStr(objVar->type, objVar->name, mut) + id + " = ";
  }

  code += codegenObjInitFn(objVar->name) + "((struct " + objVar->name + ") {";
  auto idx = static_cast<std::size_t>(0);

  for (auto field : objVar->obj->fields) {
    auto propFound = std::find_if(exprObj->props.begin(), exprObj->props.end(), [&field] (auto it) -> bool {
      return it->id->name->val == field->name;
    });

    if (propFound == exprObj->props.end()) {
      throw Error("Object literal field is not set");
    }

    code += idx == 0 ? "" : ", ";
    auto prop = *propFound;

    if (field->type == VAR_OBJ) {
      auto exprInit = std::get<Expr *>(prop->init->body);
      auto exprObjInit = std::get<ExprObj *>(exprInit->body);

      auto [
        stmtObjVarDeclSetUp,
        stmtObjVarDeclCode,
        stmtObjVarDeclCleanUp
      ] = codegenStmtObjVarDecl(codegen, false, id + "->" + field->name, mut, exprObjInit);

      code += stmtObjVarDeclCode;
      setUp += stmtObjVarDeclSetUp;
      cleanUp += stmtObjVarDeclCleanUp;
    } else {
      auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, prop->init);
      auto propName = id + "->" + field->name;

      code += stmtExprCode;
      setUp += stmtExprSetUp;
      cleanUp += stmtExprCleanUp;

      codegen->varMap->add(field->type, propName);
    }

    idx++;
  }

  code += "})";

  if (rootDecl) {
    code += ";\n";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

CodegenReturn codegenStmt (Codegen *codegen, const Stmt *stmt) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (stmt->type == STMT_BREAK) {
    code += std::string(codegen->indent, ' ');
    code += "break;\n";
  } else if (stmt->type == STMT_CONTINUE) {
    code += std::string(codegen->indent, ' ');
    code += "continue;\n";
  } else if (stmt->type == STMT_EXPR) {
    auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, std::get<StmtExpr *>(stmt->body));

    code += std::string(codegen->indent, ' ');
    code += stmtExprCode;
    code += ";\n";

    setUp += stmtExprSetUp;
    cleanUp += stmtExprCleanUp;
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
    code += std::string(codegen->indent, ' ');
    code += codegenStmtIf(codegen, std::get<StmtIf *>(stmt->body));
    code += "\n";
  } else if (stmt->type == STMT_LOOP) {
    auto stmtLoop = std::get<StmtLoop *>(stmt->body);
    auto stmtLoopCleanUp = std::string();

    codegen->varMap->save();
    code += std::string(codegen->indent, ' ');

    if (stmtLoop->init == nullptr && stmtLoop->cond == nullptr && stmtLoop->upd == nullptr) {
      code += "while (1)";
    } else if (stmtLoop->init == nullptr && stmtLoop->upd == nullptr) {
      auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtLoop->cond);
      // TODO How do you clean here?

      code += "while (";
      code += stmtExprSetUp;
      code += stmtExprCode;
      code += ")";

      stmtLoopCleanUp += stmtExprCleanUp;
    } else {
      code += "for (";

      if (stmtLoop->init != nullptr) {
        auto prevIndent = codegen->indent;

        codegen->indent = 0;
        auto [stmtSetUp, stmtCode, stmtCleanUp] = codegenStmt(codegen, stmtLoop->init);
        codegen->indent = prevIndent;
        // TODO How do you clean here?

        code += stmtSetUp;
        code += stmtCode.substr(0, stmtCode.length() - 2);
        stmtLoopCleanUp += stmtCleanUp;
      }

      code += ";";

      if (stmtLoop->cond != nullptr) {
        auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtLoop->cond);
        // TODO How do you clean here?

        code += " " + stmtExprSetUp + stmtExprCode;
        stmtLoopCleanUp += stmtExprCleanUp;
      }

      code += ";";

      if (stmtLoop->upd != nullptr) {
        auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtLoop->upd);
        // TODO How do you clean here?

        code += " " + stmtExprSetUp + stmtExprCode;
        stmtLoopCleanUp += stmtExprCleanUp;
      }

      code += ")";
    }

    if (stmtLoop->body->body.empty() && stmtLoopCleanUp.empty()) {
      code += ";\n";
    } else {
      code += " {\n";
      code += codegenBlock(codegen, stmtLoop->body);
      code += stmtLoopCleanUp;
      code += std::string(codegen->indent, ' ') + "}\n";
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

      if (fieldType == VAR_OBJ) {
        auto fieldObjTypeName = codegenIdentifier(field->type);
        auto objVar = codegen->varMap->getObj(fieldObjTypeName);

        objField->typeName = objVar->name;
      }

      fields.push_back(objField);
    }

    codegen->varMap->addObj(objName, hiddenName, fields);
  } else if (stmt->type == STMT_RETURN) {
    auto stmtReturn = std::get<StmtReturn *>(stmt->body);
    auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtReturn->arg);

    code += std::string(codegen->indent, ' ');
    code += stmtExprSetUp;
    code += "return ";
    code += stmtExprCode;
    code += ";\n";

    cleanUp += stmtExprCleanUp;
  } else if (stmt->type == STMT_VAR_DECL) {
    auto stmtVarDecl = std::get<StmtVarDecl *>(stmt->body);

    auto exprType = stmtVarDecl->type == nullptr
      ? codegenStmtExprType(codegen, stmtVarDecl->init)
      : codegenType(codegen, stmtVarDecl->type);

    if (exprType == VAR_OBJ) {
      auto expr = std::get<Expr *>(stmtVarDecl->init->body);
      auto exprObj = std::get<ExprObj *>(expr->body);

      auto [
        stmtObjVarDeclSetUp,
        stmtObjVarDeclCode,
        stmtObjVarDeclCleanUp
      ] = codegenStmtObjVarDecl(codegen, true, codegenIdentifier(stmtVarDecl->id), stmtVarDecl->mut, exprObj);

      code += stmtObjVarDeclCode;
      setUp += stmtObjVarDeclSetUp;
      cleanUp += stmtObjVarDeclCleanUp;
    } else {
      auto [stmtExprSetUp, stmtExprCode, stmtExprCleanUp] = codegenStmtExpr(codegen, stmtVarDecl->init);
      auto varName = codegenIdentifier(stmtVarDecl->id);

      code += std::string(codegen->indent, ' ');
      code += codegenTypeStr(exprType, varName, stmtVarDecl->mut);
      code += varName;
      code += " = ";
      code += stmtExprCode;
      code += ";\n";

      setUp += stmtExprSetUp;
      cleanUp += stmtExprCleanUp;

      codegen->varMap->add(exprType, varName);
    }
  } else {
    throw Error("Tried to access unknown stmt type");
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::string codegen (const AST *ast) {
  auto codegen = new Codegen{new VarMap{}};
  codegen->stack.reserve(SHRT_MAX);

  for (auto stmt : ast->topLevelStatements) {
    auto [stmtSetUp, stmtCode, stmtCleanUp] = codegenStmt(codegen, stmt);

    if (!stmtSetUp.empty() || !stmtCleanUp.empty()) {
      throw Error("Code generator returned set up or clean up data for global statement");
    }

    codegen->topLevelStatementsCode += stmtCode;
  }

  codegenDeclarations(codegen, ast->topLevelStatements);
  codegenDefinitions(codegen, ast->topLevelStatements);

  codegen->varMap->save();
  codegen->stack.emplace_back("main");
  codegen->mainBodyCode += "int main () {\n";

  if (ast->mainPresent) {
    codegen->mainBodyCode += codegenBlock(codegen, ast->mainBody);
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

  auto builtinStructDeclarationsCode = std::string();
  auto builtinFunctionDeclarationsCode = std::string();
  auto builtinFunctionDefinitionsCode = std::string();

  if (codegen->functions.str_init || codegen->functions.str_from_cstr) {
    codegen->headers.stdlib = true;
    codegen->functions.str_init = true;
    codegen->functions.str_deinit = true;

    builtinStructDeclarationsCode += "struct str {\n";
    builtinStructDeclarationsCode += "  unsigned char *c;\n";
    builtinStructDeclarationsCode += "  size_t l;\n";
    builtinStructDeclarationsCode += "};\n\n";
  }

  if (codegen->functions.str_init) {
    codegen->headers.stdio = true;
    codegen->headers.stdlib = true;

    builtinFunctionDeclarationsCode += "struct str *str_init (size_t);\n";

    builtinFunctionDefinitionsCode += "struct str *str_init (size_t l) {\n";
    builtinFunctionDefinitionsCode += "  size_t z = sizeof(struct str);\n";
    builtinFunctionDefinitionsCode += "  struct str *s = malloc(z);\n";
    builtinFunctionDefinitionsCode += "  if (s == NULL) {\n";
    builtinFunctionDefinitionsCode += R"(    fprintf(stderr, "Error: Failed to allocate %zu bytes for string\n", z);)";
    builtinFunctionDefinitionsCode += "\n";
    builtinFunctionDefinitionsCode += "    exit(EXIT_FAILURE);\n";
    builtinFunctionDefinitionsCode += "  }\n";
    builtinFunctionDefinitionsCode += "  s->c = malloc(l);\n";
    builtinFunctionDefinitionsCode += "  if (s->c == NULL) {\n";
    builtinFunctionDefinitionsCode += R"(    fprintf(stderr, "Error: )";
    builtinFunctionDefinitionsCode += "Failed to allocate %zu bytes for string content\\n\", l);\n";
    builtinFunctionDefinitionsCode += "    exit(EXIT_FAILURE);\n";
    builtinFunctionDefinitionsCode += "  }\n";
    builtinFunctionDefinitionsCode += "  s->l = l;\n";
    builtinFunctionDefinitionsCode += "  return s;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  if (codegen->functions.str_from_cstr) {
    codegen->headers.string = true;

    builtinFunctionDeclarationsCode += "struct str *str_from_cstr (const char *, size_t);\n";

    builtinFunctionDefinitionsCode += "struct str *str_from_cstr (const char *c, size_t l) {\n";
    builtinFunctionDefinitionsCode += "  struct str *s = str_init(l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(s->c, c, l);\n";
    builtinFunctionDefinitionsCode += "  return s;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  if (codegen->functions.str_deinit) {
    codegen->headers.stdlib = true;

    builtinFunctionDeclarationsCode += "void str_deinit (struct str **);\n";

    builtinFunctionDefinitionsCode += "void str_deinit (struct str **s) {\n";
    builtinFunctionDefinitionsCode += "  free((*s)->c);\n";
    builtinFunctionDefinitionsCode += "  free(*s);\n";
    builtinFunctionDefinitionsCode += "  *s = NULL;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  auto headers = std::string(codegen->headers.math ? "#include <math.h>\n" : "") +
    std::string(codegen->headers.stdbool ? "#include <stdbool.h>\n" : "") +
    std::string(codegen->headers.stdio ? "#include <stdio.h>\n" : "") +
    std::string(codegen->headers.stdlib ? "#include <stdlib.h>\n" : "") +
    std::string(codegen->headers.string ? "#include <string.h>\n" : "");

  auto code = banner +
    (headers.empty() ? "" : headers + "\n") +
    builtinStructDeclarationsCode +
    codegen->structDeclarationsCode +
    builtinFunctionDeclarationsCode +
    (codegen->functionDeclarationsCode.empty() ? "" : codegen->functionDeclarationsCode + "\n") +
    builtinFunctionDefinitionsCode +
    codegen->functionDefinitionsCode +
    (codegen->topLevelStatementsCode.empty() ? "" : codegen->topLevelStatementsCode + "\n") +
    codegen->mainBodyCode;

  delete codegen;
  return code;
}
