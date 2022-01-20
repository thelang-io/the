/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"
#include "Error.hpp"
#include "Parser.hpp"

void analyzeForwardStmt (AST &, const ParserBlock &);
ASTNode analyzeStmt (AST &, const ParserStmt &);

ASTBlock analyzeBlock (AST &ast, const ParserBlock &block) {
  analyzeForwardStmt(ast, block);
  auto result = ASTBlock{};

  for (const auto &stmt : block) {
    result.push_back(analyzeStmt(ast, stmt));
  }

  return result;
}

std::shared_ptr<Type> analyzeStmtExprAccessType (const AST &ast, const ParserExprAccess &exprAccess) {
  if (std::holds_alternative<ParserId>(exprAccess)) {
    auto parserId = std::get<ParserId>(exprAccess);
    auto var = ast.varMap.get(parserId.name.val);

    return var.type;
  }

  auto parserMember = std::get<ParserMember>(exprAccess);
  auto memberObjType = analyzeStmtExprAccessType(ast, *parserMember.obj);

  if (!memberObjType->isObj()) {
    throw Error("Tried accessing property of non object variable");
  }

  auto memberObj = std::get<TypeObj>(memberObjType->body);

  auto memberObjField = std::find_if(memberObj.fields.begin(), memberObj.fields.end(), [&parserMember] (const auto &it) -> bool {
    return it.name == parserMember.prop.name.val;
  });

  if (memberObjField == memberObj.fields.end()) {
    throw Error("Tried accessing non existing object property");
  }

  return memberObjField->type;
}

std::shared_ptr<Type> analyzeStmtExprType (const AST &ast, const ParserStmtExpr &stmtExpr) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.expr)) {
    auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.expr);
    return analyzeStmtExprAccessType(ast, exprAccess);
  } else if (std::holds_alternative<ParserExprAssign>(*stmtExpr.expr)) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.expr);
    return analyzeStmtExprAccessType(ast, exprAssign.left);
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.expr)) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.expr);
    auto exprBinaryLeftType = analyzeStmtExprType(ast, exprBinary.left);
    auto exprBinaryRightType = analyzeStmtExprType(ast, exprBinary.right);

    if (
      exprBinaryLeftType->isChar() ||
      exprBinaryLeftType->isStr() ||
      exprBinaryRightType->isChar() ||
      exprBinaryRightType->isStr()
    ) {
      if (exprBinary.op.type == TK_OP_PLUS) {
        return ast.typeMap.get("str");
      } else {
        throw Error("Tried operation other than concatenation on string");
      }
    } else if (
      exprBinaryLeftType->isFloat() ||
      exprBinaryLeftType->isF32() ||
      exprBinaryLeftType->isF64() ||
      exprBinaryRightType->isFloat() ||
      exprBinaryRightType->isF32() ||
      exprBinaryRightType->isF64() ||
      exprBinary.op.type == TK_OP_SLASH ||
      exprBinary.op.type == TK_OP_STAR ||
      exprBinary.op.type == TK_OP_STAR_STAR
    ) {
      return ast.typeMap.get("float");
    } else if (
      exprBinary.op.type == TK_OP_EQ_EQ ||
      exprBinary.op.type == TK_OP_EXCL_EQ ||
      exprBinary.op.type == TK_OP_GT ||
      exprBinary.op.type == TK_OP_GT_EQ ||
      exprBinary.op.type == TK_OP_LT ||
      exprBinary.op.type == TK_OP_LT_EQ
    ) {
      return ast.typeMap.get("bool");
    } else {
      return ast.typeMap.get("int");
    }
  } else if (std::holds_alternative<ParserExprCall>(*stmtExpr.expr)) {
    auto exprCall = std::get<ParserExprCall>(*stmtExpr.expr);

    if (!std::holds_alternative<ParserId>(exprCall.callee)) {
      // TODO callee of type member
    }

    auto parserId = std::get<ParserId>(exprCall.callee);
    auto var = ast.varMap.get(parserId.name.val);
    auto fn = std::get<TypeFn>(var.type->body);

    return fn.type;
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.expr)) {
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.expr);
    auto exprCondBodyType = analyzeStmtExprType(ast, exprCond.body);
    auto exprCondAltType = analyzeStmtExprType(ast, exprCond.alt);

    if (exprCondBodyType->name != exprCondAltType->name) {
      throw Error("Incompatible operand types");
    }

    return exprCondBodyType;
  } else if (std::holds_alternative<ParserExprLit>(*stmtExpr.expr)) {
    auto exprLit = std::get<ParserExprLit>(*stmtExpr.expr);

    if (exprLit.val.type == TK_KW_FALSE) return ast.typeMap.get("bool");
    if (exprLit.val.type == TK_KW_TRUE) return ast.typeMap.get("bool");
    if (exprLit.val.type == TK_LIT_CHAR) return ast.typeMap.get("char");
    if (exprLit.val.type == TK_LIT_FLOAT) return ast.typeMap.get("float");
    if (exprLit.val.type == TK_LIT_INT_BIN) return ast.typeMap.get("int");
    if (exprLit.val.type == TK_LIT_INT_DEC) return ast.typeMap.get("int");
    if (exprLit.val.type == TK_LIT_INT_HEX) return ast.typeMap.get("int");
    if (exprLit.val.type == TK_LIT_INT_OCT) return ast.typeMap.get("int");
    if (exprLit.val.type == TK_LIT_STR) return ast.typeMap.get("str");
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr.expr)) {
    auto exprObj = std::get<ParserExprObj>(*stmtExpr.expr);
    auto var = ast.varMap.get(exprObj.id.name.val);

    return var.type;
  } else if (std::holds_alternative<ParserExprUnary>(*stmtExpr.expr)) {
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.expr);
    auto exprType = analyzeStmtExprType(ast, exprUnary.arg);

    if (exprType->isBool() || exprType->isChar() || exprType->isFn() || exprType->isObj() || exprType->isStr()) {
      return ast.typeMap.get("int");
    }

    return exprType;
  }

  throw Error("Tried to analyze unknown expression type");
}

std::shared_ptr<Type> analyzeType (const AST &ast, const std::optional<ParserId> &type, const std::optional<ParserStmtExpr> &init = std::nullopt) {
  if (type != std::nullopt) {
    if (type->name.val == "bool") return ast.typeMap.get("bool");
    if (type->name.val == "byte") return ast.typeMap.get("byte");
    if (type->name.val == "char") return ast.typeMap.get("char");
    if (type->name.val == "float") return ast.typeMap.get("float");
    if (type->name.val == "f32") return ast.typeMap.get("f32");
    if (type->name.val == "f64") return ast.typeMap.get("f64");
    if (type->name.val == "int") return ast.typeMap.get("int");
    if (type->name.val == "i8") return ast.typeMap.get("i8");
    if (type->name.val == "i16") return ast.typeMap.get("i16");
    if (type->name.val == "i32") return ast.typeMap.get("i32");
    if (type->name.val == "i64") return ast.typeMap.get("i64");
    if (type->name.val == "str") return ast.typeMap.get("str");
    if (type->name.val == "u8") return ast.typeMap.get("u8");
    if (type->name.val == "u16") return ast.typeMap.get("u16");
    if (type->name.val == "u32") return ast.typeMap.get("u32");
    if (type->name.val == "u64") return ast.typeMap.get("u64");
    if (type->name.val == "void") return ast.typeMap.get("void");

    if (ast.varMap.has(type->name.val)) {
      auto var = ast.varMap.get(type->name.val);
      return var.type;
    }
  }

  if (init != std::nullopt) {
    return analyzeStmtExprType(ast, *init);
  }

  throw Error("Tried to analyze unknown type");
}

void analyzeForwardStmt (AST &ast, const ParserBlock &block) {
  for (const auto &stmt : block) {
    if (std::holds_alternative<ParserStmtFnDecl>(stmt)) {
      auto stmtFnDecl = std::get<ParserStmtFnDecl>(stmt);
      auto fnName = stmtFnDecl.id.name.val;
      auto fnTypeName = ast.typeMap.name(fnName);
      auto fnReturnType = analyzeType(ast, stmtFnDecl.returnType);
      auto fnParams = std::vector<TypeFnParam>{};

      for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
        auto fnParamType = analyzeType(ast, stmtFnDeclParam.type, stmtFnDeclParam.init);
        fnParams.push_back(TypeFnParam{stmtFnDeclParam.id.name.val, fnParamType, stmtFnDeclParam.init == std::nullopt});
      }

      auto fn = ast.typeMap.add(fnTypeName, fnReturnType, fnParams);
      ast.varMap.add(fnName, fnTypeName, fn, false);
    } else if (std::holds_alternative<ParserStmtObjDecl>(stmt)) {
      auto stmtObjDecl = std::get<ParserStmtObjDecl>(stmt);
      auto objName = stmtObjDecl.id.name.val;
      auto objTypeName = ast.typeMap.name(objName);
      auto obj = ast.typeMap.add(objTypeName, ast.typeMap.obj(), std::vector<TypeObjField>{});

      ast.varMap.add(objName, objTypeName, obj, false);
    }
  }
}

ASTExprAccess analyzeStmtExprAccess (const AST &ast, const ParserExprAccess &exprAccess) {
  if (std::holds_alternative<ParserId>(exprAccess)) {
    auto parserId = std::get<ParserId>(exprAccess);
    auto var = ast.varMap.get(parserId.name.val);
    auto id = ASTId{var};

    return ASTExprAccess{id};
  }

  auto parserMember = std::get<ParserMember>(exprAccess);
  auto memberObj = std::make_shared<ASTExprAccess>(analyzeStmtExprAccess(ast, *parserMember.obj));
  auto member = ASTMember{memberObj, parserMember.prop.name.val};

  return ASTExprAccess{member};
}

ASTNodeExpr analyzeStmtExpr (const AST &ast, const std::optional<ParserStmtExpr> &stmtExpr) {
  if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprAccess>(*stmtExpr->expr)) {
    auto parserExprAccess = std::get<ParserExprAccess>(*stmtExpr->expr);
    auto exprAccess = std::make_shared<ASTExpr>(analyzeStmtExprAccess(ast, parserExprAccess));

    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprAccess, stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprAssign>(*stmtExpr->expr)) {
    auto parserExprAssign = std::get<ParserExprAssign>(*stmtExpr->expr);
    auto op = ASTExprAssignOp{};

    if (parserExprAssign.op.type == TK_OP_AND_AND_EQ) op = AST_EXPR_ASSIGN_LOGICAL_AND;
    if (parserExprAssign.op.type == TK_OP_AND_EQ) op = AST_EXPR_ASSIGN_BITWISE_AND;
    if (parserExprAssign.op.type == TK_OP_CARET_EQ) op = AST_EXPR_ASSIGN_BITWISE_XOR;
    if (parserExprAssign.op.type == TK_OP_EQ) op = AST_EXPR_ASSIGN_EQUAL;
    if (parserExprAssign.op.type == TK_OP_LSHIFT_EQ) op = AST_EXPR_ASSIGN_LEFT_SHIFT;
    if (parserExprAssign.op.type == TK_OP_MINUS_EQ) op = AST_EXPR_ASSIGN_SUBTRACT;
    if (parserExprAssign.op.type == TK_OP_OR_EQ) op = AST_EXPR_ASSIGN_BITWISE_OR;
    if (parserExprAssign.op.type == TK_OP_OR_OR_EQ) op = AST_EXPR_ASSIGN_LOGICAL_OR;
    if (parserExprAssign.op.type == TK_OP_PERCENT_EQ) op = AST_EXPR_ASSIGN_REMAINDER;
    if (parserExprAssign.op.type == TK_OP_PLUS_EQ) op = AST_EXPR_ASSIGN_ADD;
    if (parserExprAssign.op.type == TK_OP_QN_QN_EQ) op = AST_EXPR_ASSIGN_COALESCE;
    if (parserExprAssign.op.type == TK_OP_RSHIFT_EQ) op = AST_EXPR_ASSIGN_RIGHT_SHIFT;
    if (parserExprAssign.op.type == TK_OP_SLASH_EQ) op = AST_EXPR_ASSIGN_DIVIDE;
    if (parserExprAssign.op.type == TK_OP_STAR_EQ) op = AST_EXPR_ASSIGN_MULTIPLY;
    if (parserExprAssign.op.type == TK_OP_STAR_STAR_EQ) op = AST_EXPR_ASSIGN_POWER;

    auto left = analyzeStmtExprAccess(ast, parserExprAssign.left);
    auto right = analyzeStmtExpr(ast, parserExprAssign.right);
    auto exprAssign = std::make_shared<ASTExpr>(ASTExprAssign{left, op, right});

    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprAssign, stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprBinary>(*stmtExpr->expr)) {
    auto parserExprBinary = std::get<ParserExprBinary>(*stmtExpr->expr);
    auto op = ASTExprBinaryOp{};

    if (parserExprBinary.op.type == TK_OP_AND) op = AST_EXPR_BINARY_BITWISE_AND;
    if (parserExprBinary.op.type == TK_OP_AND_AND) op = AST_EXPR_BINARY_LOGICAL_AND;
    if (parserExprBinary.op.type == TK_OP_CARET) op = AST_EXPR_BINARY_BITWISE_XOR;
    if (parserExprBinary.op.type == TK_OP_EQ_EQ) op = AST_EXPR_BINARY_EQUAL;
    if (parserExprBinary.op.type == TK_OP_EXCL_EQ) op = AST_EXPR_BINARY_NOT_EQUAL;
    if (parserExprBinary.op.type == TK_OP_GT) op = AST_EXPR_BINARY_GREATER_THAN;
    if (parserExprBinary.op.type == TK_OP_GT_EQ) op = AST_EXPR_BINARY_GREATER_EQUAL;
    if (parserExprBinary.op.type == TK_OP_LSHIFT) op = AST_EXPR_BINARY_LEFT_SHIFT;
    if (parserExprBinary.op.type == TK_OP_LT) op = AST_EXPR_BINARY_LESS_THAN;
    if (parserExprBinary.op.type == TK_OP_LT_EQ) op = AST_EXPR_BINARY_LESS_EQUAL;
    if (parserExprBinary.op.type == TK_OP_MINUS) op = AST_EXPR_BINARY_SUBTRACT;
    if (parserExprBinary.op.type == TK_OP_OR) op = AST_EXPR_BINARY_BITWISE_OR;
    if (parserExprBinary.op.type == TK_OP_OR_OR) op = AST_EXPR_BINARY_LOGICAL_OR;
    if (parserExprBinary.op.type == TK_OP_PERCENT) op = AST_EXPR_BINARY_REMAINDER;
    if (parserExprBinary.op.type == TK_OP_PLUS) op = AST_EXPR_BINARY_ADD;
    if (parserExprBinary.op.type == TK_OP_QN_QN) op = AST_EXPR_BINARY_COALESCE;
    if (parserExprBinary.op.type == TK_OP_RSHIFT) op = AST_EXPR_BINARY_RIGHT_SHIFT;
    if (parserExprBinary.op.type == TK_OP_SLASH) op = AST_EXPR_BINARY_DIVIDE;
    if (parserExprBinary.op.type == TK_OP_STAR) op = AST_EXPR_BINARY_MULTIPLY;
    if (parserExprBinary.op.type == TK_OP_STAR_STAR) op = AST_EXPR_BINARY_POWER;

    auto left = analyzeStmtExpr(ast, parserExprBinary.left);
    auto right = analyzeStmtExpr(ast, parserExprBinary.right);
    auto exprBinary = std::make_shared<ASTExpr>(ASTExprBinary{left, op, right});

    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprBinary, stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprCall>(*stmtExpr->expr)) {
    auto parserExprCall = std::get<ParserExprCall>(*stmtExpr->expr);
    auto callee = analyzeStmtExprAccess(ast, parserExprCall.callee);

    if (!std::holds_alternative<ASTId>(callee)) {
      // TODO callee of type member
    }

    auto id = std::get<ASTId>(callee);
    auto type = id.v.type;
    auto fn = std::get<TypeFn>(type->body);
    auto args = std::vector<ASTExprCallArg>{};
    auto argIdx = static_cast<std::size_t>(0);

    for (const auto &parserExprCallArg : parserExprCall.args) {
      auto foundParam = std::optional<TypeFnParam>{};

      if (argIdx < fn.params.size() && parserExprCallArg.id == std::nullopt) {
        foundParam = fn.params[argIdx];
      } else if (parserExprCallArg.id != std::nullopt) {
        auto argName = parserExprCallArg.id->name.val;

        for (const auto &exprCallArg : fn.params) {
          if (exprCallArg.name == argName) {
            foundParam = exprCallArg;
            break;
          }
        }
      }

      if (foundParam == std::nullopt && !id.v.builtin) {
        throw Error("Call expression argument wasn't found in function parameters");
      }

      auto argId = std::optional<std::string>{};

      if (parserExprCallArg.id != std::nullopt) {
        argId = parserExprCallArg.id->name.val;
      }

      auto argExpr = analyzeStmtExpr(ast, parserExprCallArg.expr);

      if (!id.v.builtin) {
        argExpr.type = foundParam->type;
      }

      args.push_back(ASTExprCallArg{argId, argExpr});
      argIdx++;
    }

    auto exprCall = std::make_shared<ASTExpr>(ASTExprCall{type, callee, args});
    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprCall, stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprCond>(*stmtExpr->expr)) {
    auto parserExprCond = std::get<ParserExprCond>(*stmtExpr->expr);
    auto cond = analyzeStmtExpr(ast, parserExprCond.cond);
    auto body = analyzeStmtExpr(ast, parserExprCond.body);
    auto alt = analyzeStmtExpr(ast, parserExprCond.alt);
    auto exprCond = std::make_shared<ASTExpr>(ASTExprCond{cond, body, alt});

    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprCond, stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprLit>(*stmtExpr->expr)) {
    auto parserExprLit = std::get<ParserExprLit>(*stmtExpr->expr);
    auto type = ASTExprLitType{};

    if (parserExprLit.val.type == TK_KW_FALSE) type = AST_EXPR_LIT_BOOL;
    if (parserExprLit.val.type == TK_KW_TRUE) type = AST_EXPR_LIT_BOOL;
    if (parserExprLit.val.type == TK_LIT_CHAR) type = AST_EXPR_LIT_CHAR;
    if (parserExprLit.val.type == TK_LIT_FLOAT) type = AST_EXPR_LIT_FLOAT;
    if (parserExprLit.val.type == TK_LIT_INT_BIN) type = AST_EXPR_LIT_INT_BIN;
    if (parserExprLit.val.type == TK_LIT_INT_DEC) type = AST_EXPR_LIT_INT_DEC;
    if (parserExprLit.val.type == TK_LIT_INT_HEX) type = AST_EXPR_LIT_INT_HEX;
    if (parserExprLit.val.type == TK_LIT_INT_OCT) type = AST_EXPR_LIT_INT_OCT;
    if (parserExprLit.val.type == TK_LIT_STR) type = AST_EXPR_LIT_STR;

    auto exprLit = std::make_shared<ASTExpr>(ASTExprLit{type, parserExprLit.val.val});
    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprLit, stmtExpr->parenthesized};
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr->expr)) {
    auto parserExprObj = std::get<ParserExprObj>(*stmtExpr->expr);
    auto var = ast.varMap.get(parserExprObj.id.name.val);
    auto props = std::vector<ASTExprObjProp>();

    for (const auto &parserExprObjProp : parserExprObj.props) {
      props.push_back(ASTExprObjProp{parserExprObjProp.id.name.val, analyzeStmtExpr(ast, parserExprObjProp.init)});
    }

    auto exprObj = std::make_shared<ASTExpr>(ASTExprObj{var.type, props});
    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprObj, stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprUnary>(*stmtExpr->expr)) {
    auto parserExprUnary = std::get<ParserExprUnary>(*stmtExpr->expr);
    auto op = ASTExprUnaryOp{};

    if (parserExprUnary.op.type == TK_OP_EXCL) op = AST_EXPR_UNARY_LOGICAL_NOT;
    if (parserExprUnary.op.type == TK_OP_EXCL_EXCL) op = AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT;
    if (parserExprUnary.op.type == TK_OP_MINUS) op = AST_EXPR_UNARY_NEGATION;
    if (parserExprUnary.op.type == TK_OP_MINUS_MINUS) op = AST_EXPR_UNARY_DECREMENT;
    if (parserExprUnary.op.type == TK_OP_PLUS) op = AST_EXPR_UNARY_PLUS;
    if (parserExprUnary.op.type == TK_OP_PLUS_PLUS) op = AST_EXPR_UNARY_INCREMENT;
    if (parserExprUnary.op.type == TK_OP_TILDE) op = AST_EXPR_UNARY_BITWISE_NOT;

    auto exprUnaryArg = analyzeStmtExpr(ast, parserExprUnary.arg);
    auto exprUnary = std::make_shared<ASTExpr>(ASTExprUnary{exprUnaryArg, op, parserExprUnary.prefix});

    return ASTNodeExpr{analyzeStmtExprType(ast, *stmtExpr), exprUnary, stmtExpr->parenthesized};
  }

  throw Error("Tried to analyze unknown expression statement");
}

ASTNodeIf analyzeStmtIf (AST &ast, const ParserStmtIf &stmtIf) {
  ast.varMap.save();
  auto cond = analyzeStmtExpr(ast, stmtIf.cond);
  auto body = analyzeBlock(ast, stmtIf.body);
  ast.varMap.restore();

  auto alt = std::optional<std::shared_ptr<ASTNodeIfCond>>{};

  if (stmtIf.alt != std::nullopt) {
    if (std::holds_alternative<ParserBlock>(**stmtIf.alt)) {
      ast.varMap.save();
      auto stmtIfAltBlock = std::get<ParserBlock>(**stmtIf.alt);
      auto nodeIfAltElse = analyzeBlock(ast, std::get<ParserBlock>(**stmtIf.alt));
      ast.varMap.restore();

      alt = std::make_shared<ASTNodeIfCond>(nodeIfAltElse);
    } else if (std::holds_alternative<ParserStmtIf>(**stmtIf.alt)) {
      auto nodeIfAltElif = analyzeStmtIf(ast, std::get<ParserStmtIf>(**stmtIf.alt));
      alt = std::make_shared<ASTNodeIfCond>(nodeIfAltElif);
    }
  }

  return ASTNodeIf{cond, body, alt};
}

ASTNode analyzeStmt (AST &ast, const ParserStmt &stmt) {
  if (std::holds_alternative<ParserStmtBreak>(stmt)) {
    auto nodeBreak = ASTNodeBreak{};
    return ASTNode{nodeBreak};
  } else if (std::holds_alternative<ParserStmtContinue>(stmt)) {
    auto nodeContinue = ASTNodeContinue{};
    return ASTNode{nodeContinue};
  } else if (std::holds_alternative<ParserStmtExpr>(stmt)) {
    auto stmtExpr = std::get<ParserStmtExpr>(stmt);
    auto nodeExpr = analyzeStmtExpr(ast, stmtExpr);

    return ASTNode{nodeExpr};
  } else if (std::holds_alternative<ParserStmtFnDecl>(stmt)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(stmt);
    auto fnName = stmtFnDecl.id.name.val;
    auto var = ast.varMap.get(fnName);

    ast.typeMap.stack.emplace_back(var.name);
    ast.varMap.save();

    auto params = std::vector<ASTNodeFnDeclParam>{};

    for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
      auto fnParamInit = std::optional<ASTNodeExpr>{};

      if (stmtFnDeclParam.init != std::nullopt) {
        fnParamInit = analyzeStmtExpr(ast, stmtFnDeclParam.init);
      }

      auto fnParamName = stmtFnDeclParam.id.name.val;
      auto fnParamCodeName = ast.varMap.name(fnParamName);
      auto fnParamType = analyzeType(ast, stmtFnDeclParam.type, stmtFnDeclParam.init);
      auto fnParamVar = ast.varMap.add(fnParamName, fnParamCodeName, fnParamType, false);

      params.push_back(ASTNodeFnDeclParam{fnParamVar, fnParamInit});
    }

    auto body = analyzeBlock(ast, stmtFnDecl.body);

    ast.varMap.restore();
    ast.typeMap.stack.pop_back();

    auto nodeFnDecl = ASTNodeFnDecl{var, params, ast.varMap.stack(), body};
    return ASTNode{nodeFnDecl};
  } else if (std::holds_alternative<ParserStmtIf>(stmt)) {
    auto nodeIf = analyzeStmtIf(ast, std::get<ParserStmtIf>(stmt));
    return ASTNode{nodeIf};
  } else if (std::holds_alternative<ParserStmtLoop>(stmt)) {
    auto stmtLoop = std::get<ParserStmtLoop>(stmt);
    auto init = std::optional<std::shared_ptr<ASTNode>>{};
    auto cond = std::optional<ASTNodeExpr>{};
    auto upd = std::optional<ASTNodeExpr>{};

    ast.varMap.save();

    if (stmtLoop.init != std::nullopt) {
      init = std::make_shared<ASTNode>(analyzeStmt(ast, **stmtLoop.init));
    }

    if (stmtLoop.cond != std::nullopt) {
      cond = analyzeStmtExpr(ast, *stmtLoop.cond);
    }

    if (stmtLoop.upd != std::nullopt) {
      upd = analyzeStmtExpr(ast, *stmtLoop.upd);
    }

    auto body = analyzeBlock(ast, stmtLoop.body);
    ast.varMap.restore();

    auto nodeLoop = ASTNodeLoop{init, cond, upd, body};
    return ASTNode{nodeLoop};
  } else if (std::holds_alternative<ParserStmtMain>(stmt)) {
    auto stmtMain = std::get<ParserStmtMain>(stmt);

    ast.typeMap.stack.emplace_back("main");
    ast.varMap.save();

    auto body = analyzeBlock(ast, stmtMain.body);

    ast.varMap.restore();
    ast.typeMap.stack.pop_back();

    auto nodeMain = ASTNodeMain{body};
    return ASTNode{nodeMain};
  } else if (std::holds_alternative<ParserStmtObjDecl>(stmt)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(stmt);
    auto objName = stmtObjDecl.id.name.val;
    auto var = ast.varMap.get(objName);
    auto &obj = std::get<TypeObj>(var.type->body);

    for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
      auto fieldType = analyzeType(ast, stmtObjDeclField.type);
      obj.fields.push_back(TypeObjField{stmtObjDeclField.id.name.val, fieldType});
    }

    auto nodeObjDecl = ASTNodeObjDecl{var};
    return ASTNode{nodeObjDecl};
  } else if (std::holds_alternative<ParserStmtReturn>(stmt)) {
    auto stmtReturn = std::get<ParserStmtReturn>(stmt);
    auto nodeReturn = ASTNodeReturn{analyzeStmtExpr(ast, stmtReturn.arg)};

    return ASTNode{nodeReturn};
  } else if (std::holds_alternative<ParserStmtVarDecl>(stmt)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(stmt);
    auto varName = stmtVarDecl.id.name.val;
    auto varCodeName = ast.varMap.name(varName);
    auto varType = analyzeType(ast, stmtVarDecl.type, stmtVarDecl.init);
    auto var = ast.varMap.add(varName, varCodeName, varType, stmtVarDecl.mut);
    auto init = std::optional<ASTNodeExpr>{};

    if (stmtVarDecl.init != std::nullopt) {
      init = analyzeStmtExpr(ast, *stmtVarDecl.init);
      init->type = varType;
    }

    auto nodeVarDecl = ASTNodeVarDecl{var, init};
    return ASTNode{nodeVarDecl};
  }

  throw Error("Tried to analyze unknown statement");
}

AST analyze (Reader *reader) {
  auto ast = AST{};

  ast.typeMap.add("any", std::nullopt);
  ast.typeMap.add("str", std::nullopt); // TODO str.concat
  ast.typeMap.add("void", std::nullopt);

  ast.typeMap.add("bool", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("byte", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("char", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("float", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("f32", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("f64", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("int", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("i8", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("i16", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("i32", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("i64", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("u8", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("u16", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("u32", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.typeMap.add("u64", std::nullopt, {
    {"str", TypeMap::fn(ast.typeMap.get("str"))}
  });

  ast.varMap.add("print", "print", TypeMap::fn("print", ast.typeMap.get("void"), {
    {"items", ast.typeMap.get("any"), false},
    {"separator", ast.typeMap.get("str"), false},
    {"terminator", ast.typeMap.get("str"), false}
  }), false, true);

  auto block = ParserBlock{};

  while (true) {
    auto stmt = parse(reader);

    if (std::holds_alternative<ParserStmtEnd>(stmt)) {
      break;
    }

    block.push_back(stmt);
  }

  ast.varMap.save();
  ast.nodes = analyzeBlock(ast, block);
  ast.varMap.restore();

  return ast;
}
