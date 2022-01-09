/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"
#include "Error.hpp"
#include "Parser.hpp"

ASTNode analyzeStmt (AST &, const ParserStmt &);

ASTBlock analyzeBlock (AST &ast, const ParserBlock &block) {
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
  } else if (!memberObjType->fields.contains(parserMember.prop.name.val)) {
    throw Error("Tried accessing non existing object property");
  }

  return memberObjType->fields[parserMember.prop.name.val];
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
    } else {
      return ast.typeMap.get("int");
    }
  } else if (std::holds_alternative<ParserExprCall>(*stmtExpr.expr)) {
    auto exprCall = std::get<ParserExprCall>(*stmtExpr.expr);

    if (!std::holds_alternative<ParserId>(exprCall.callee)) {
      // TODO logic
    }

    auto parserId = std::get<ParserId>(exprCall.callee);
    auto var = ast.varMap.get(parserId.name.val);
    auto fn = std::get<std::shared_ptr<Fn>>(*var.ref);

    return fn->returnType;
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

    return std::get<std::shared_ptr<Type>>(*var.ref);
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

      if (var.type->isObj()) {
        return std::get<std::shared_ptr<Type>>(*var.ref);
      }

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
      auto fnTypeName = ast.fnMap.name(fnName);
      auto fnReturnType = analyzeType(ast, stmtFnDecl.returnType);
      auto fnParams = std::map<std::string, FnParam>{};

      for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
        auto fnParamType = analyzeType(ast, stmtFnDeclParam.type, stmtFnDeclParam.init);
        fnParams.emplace(stmtFnDeclParam.id.name.val, FnParam{fnParamType, stmtFnDeclParam.init == std::nullopt});
      }

      auto fn = ast.fnMap.add(Fn{fnTypeName, fnReturnType, fnParams});
      ast.varMap.add(fnName, ast.typeMap.get("fn"), false, fn);
    } else if (std::holds_alternative<ParserStmtObjDecl>(stmt)) {
      auto stmtObjDecl = std::get<ParserStmtObjDecl>(stmt);
      auto objName = stmtObjDecl.id.name.val;
      auto objTypeName = ast.typeMap.name(objName);
      auto obj = ast.typeMap.add(Type{objTypeName, ast.typeMap.get("obj")});

      ast.varMap.add(objName, ast.typeMap.get("obj"), false, obj);
    }
  }
}

ASTExprAccess analyzeStmtExprAccess (const AST &ast, const ParserExprAccess &exprAccess) {
  if (std::holds_alternative<ParserId>(exprAccess)) {
    auto parserId = std::get<ParserId>(exprAccess);
    auto id = ASTId{ast.varMap.get(parserId.name.val)};

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

    return ASTNodeExpr{exprAccess, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
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

    return ASTNodeExpr{exprAssign, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
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

    return ASTNodeExpr{exprBinary, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprCall>(*stmtExpr->expr)) {
    auto parserExprCall = std::get<ParserExprCall>(*stmtExpr->expr);
    auto callee = analyzeStmtExprAccess(ast, parserExprCall.callee);

    if (!std::holds_alternative<ASTId>(callee)) {
      // TODO logic
    }

    auto id = std::get<ASTId>(callee);
    auto fn = std::get<std::shared_ptr<Fn>>(*id.v.ref);
    auto args = std::vector<ASTExprCallArg>{};

    for (const auto &parserExprCallArg : parserExprCall.args) {
      auto argId = std::optional<std::string>{};

      if (parserExprCallArg.id != std::nullopt) {
        argId = parserExprCallArg.id->name.val;
      }

      auto argExpr = analyzeStmtExpr(ast, parserExprCallArg.expr);
      args.push_back(ASTExprCallArg{argId, argExpr});
    }

    auto exprCall = std::make_shared<ASTExpr>(ASTExprCall{callee, fn, args});
    return ASTNodeExpr{exprCall, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
  } else if (stmtExpr != std::nullopt && std::holds_alternative<ParserExprCond>(*stmtExpr->expr)) {
    auto parserExprCond = std::get<ParserExprCond>(*stmtExpr->expr);
    auto cond = analyzeStmtExpr(ast, parserExprCond.cond);
    auto body = analyzeStmtExpr(ast, parserExprCond.body);
    auto alt = analyzeStmtExpr(ast, parserExprCond.alt);
    auto exprCond = std::make_shared<ASTExpr>(ASTExprCond{cond, body, alt});

    return ASTNodeExpr{exprCond, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
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
    return ASTNodeExpr{exprLit, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr->expr)) {
    auto parserExprObj = std::get<ParserExprObj>(*stmtExpr->expr);
    auto var = ast.varMap.get(parserExprObj.id.name.val);
    auto type = std::get<std::shared_ptr<Type>>(*var.ref);
    auto props = std::map<std::string, ASTNodeExpr>();

    for (const auto &parserExprObjProp : parserExprObj.props) {
      props.emplace(parserExprObjProp.id.name.val, analyzeStmtExpr(ast, parserExprObjProp.init));
    }

    auto exprObj = std::make_shared<ASTExpr>(ASTExprObj{type, props});
    return ASTNodeExpr{exprObj, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
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

    return ASTNodeExpr{exprUnary, analyzeStmtExprType(ast, *stmtExpr), stmtExpr->parenthesized};
  }

  throw Error("Tried to analyze unknown expression statement");
}

ASTNodeIf analyzeStmtIf (AST &ast, const ParserStmtIf &stmtIf) {
  auto cond = analyzeStmtExpr(ast, stmtIf.cond);
  auto body = analyzeBlock(ast, stmtIf.body);
  auto alt = std::optional<std::shared_ptr<ASTNodeIfCond>>{};

  if (stmtIf.alt != std::nullopt) {
    if (std::holds_alternative<ParserBlock>(**stmtIf.alt)) {
      auto stmtIfAltBlock = std::get<ParserBlock>(**stmtIf.alt);
      auto nodeIfAltElse = analyzeBlock(ast, std::get<ParserBlock>(**stmtIf.alt));

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
    auto fn = std::get<std::shared_ptr<Fn>>(*var.ref);

    ast.fnMap.stack.emplace_back(var.name);
    ast.typeMap.stack.emplace_back(var.name);
    ast.varMap.save();

    auto params = std::map<std::string, ASTNodeFnDeclParam>{};

    for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
      auto fnParamInit = std::optional<ASTNodeExpr>{};

      if (stmtFnDeclParam.init != std::nullopt) {
        fnParamInit = analyzeStmtExpr(ast, stmtFnDeclParam.init);
      }

      auto fnParamType = analyzeType(ast, stmtFnDeclParam.type, stmtFnDeclParam.init);

      params.emplace(stmtFnDeclParam.id.name.val, ASTNodeFnDeclParam{fnParamType, fnParamInit});
      ast.varMap.add(stmtFnDeclParam.id.name.val, fnParamType, false);
    }

    analyzeForwardStmt(ast, stmtFnDecl.body);
    auto body = analyzeBlock(ast, stmtFnDecl.body);

    ast.varMap.restore();
    ast.typeMap.stack.pop_back();
    ast.fnMap.stack.pop_back();

    auto nodeFnDecl = ASTNodeFnDecl{fn, params, body};
    return ASTNode{nodeFnDecl};
  } else if (std::holds_alternative<ParserStmtIf>(stmt)) {
    auto nodeIf = analyzeStmtIf(ast, std::get<ParserStmtIf>(stmt));
    return ASTNode{nodeIf};
  } else if (std::holds_alternative<ParserStmtLoop>(stmt)) {
    auto stmtLoop = std::get<ParserStmtLoop>(stmt);
    auto init = std::optional<std::shared_ptr<ASTNode>>{};
    auto cond = std::optional<ASTNodeExpr>{};
    auto upd = std::optional<ASTNodeExpr>{};

    if (stmtLoop.init != std::nullopt) {
      init = std::make_shared<ASTNode>(analyzeStmt(ast, **stmtLoop.init));
    }

    if (stmtLoop.cond != std::nullopt) {
      cond = analyzeStmtExpr(ast, *stmtLoop.cond);
    }

    if (stmtLoop.upd != std::nullopt) {
      upd = analyzeStmtExpr(ast, *stmtLoop.upd);
    }

    auto nodeLoop = ASTNodeLoop{init, cond, upd, analyzeBlock(ast, stmtLoop.body)};
    return ASTNode{nodeLoop};
  } else if (std::holds_alternative<ParserStmtMain>(stmt)) {
    auto stmtMain = std::get<ParserStmtMain>(stmt);

    ast.fnMap.stack.emplace_back("main");
    ast.typeMap.stack.emplace_back("main");
    ast.varMap.save();

    analyzeForwardStmt(ast, stmtMain.body);
    auto body = analyzeBlock(ast, stmtMain.body);

    ast.varMap.restore();
    ast.typeMap.stack.pop_back();
    ast.fnMap.stack.pop_back();

    auto nodeMain = ASTNodeMain{body};
    return ASTNode{nodeMain};
  } else if (std::holds_alternative<ParserStmtObjDecl>(stmt)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(stmt);
    auto objName = stmtObjDecl.id.name.val;
    auto var = ast.varMap.get(objName);
    auto obj = std::get<std::shared_ptr<Type>>(*var.ref);

    for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
      auto fieldType = analyzeType(ast, stmtObjDeclField.type);
      obj->fields.emplace(stmtObjDeclField.id.name.val, fieldType);
    }

    auto nodeObjDecl = ASTNodeObjDecl{objName, obj};
    return ASTNode{nodeObjDecl};
  } else if (std::holds_alternative<ParserStmtReturn>(stmt)) {
    auto stmtReturn = std::get<ParserStmtReturn>(stmt);
    auto nodeReturn = ASTNodeReturn{analyzeStmtExpr(ast, stmtReturn.arg)};

    return ASTNode{nodeReturn};
  } else if (std::holds_alternative<ParserStmtVarDecl>(stmt)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(stmt);
    auto varType = analyzeType(ast, stmtVarDecl.type, stmtVarDecl.init);
    auto var = ast.varMap.add(stmtVarDecl.id.name.val, varType, stmtVarDecl.mut);
    auto init = analyzeStmtExpr(ast, *stmtVarDecl.init);
    auto nodeVarDecl = ASTNodeVarDecl{var, init};

    return ASTNode{nodeVarDecl};
  }

  throw Error("Tried to analyze unknown statement");
}

AST analyze (Reader *reader) {
  auto ast = AST{};

  ast.typeMap.add(Type{"any"});
  ast.typeMap.add(Type{"bool"});
  ast.typeMap.add(Type{"byte"});
  ast.typeMap.add(Type{"char"});
  ast.typeMap.add(Type{"float"});
  ast.typeMap.add(Type{"f32"});
  ast.typeMap.add(Type{"f64"});
  ast.typeMap.add(Type{"fn"});
  ast.typeMap.add(Type{"int"});
  ast.typeMap.add(Type{"i8"});
  ast.typeMap.add(Type{"i16"});
  ast.typeMap.add(Type{"i32"});
  ast.typeMap.add(Type{"i64"});
  ast.typeMap.add(Type{"obj"});
  ast.typeMap.add(Type{"str"});
  ast.typeMap.add(Type{"u8"});
  ast.typeMap.add(Type{"u16"});
  ast.typeMap.add(Type{"u32"});
  ast.typeMap.add(Type{"u64"});
  ast.typeMap.add(Type{"void"});

  auto printParams = std::map<std::string, FnParam>{
    std::pair{"items", FnParam{ast.typeMap.get("any"), false}},
    std::pair{"separator", FnParam{ast.typeMap.get("str"), false}},
    std::pair{"terminator", FnParam{ast.typeMap.get("str"), false}}
  };

  auto printFn = ast.fnMap.add(Fn{"print", ast.typeMap.get("void"), printParams, true});
  ast.varMap.add(printFn->name, ast.typeMap.get("fn"), false, printFn);

  auto block = ParserBlock{};

  while (true) {
    auto stmt = parse(reader);

    if (std::holds_alternative<ParserStmtEnd>(stmt)) {
      break;
    }

    block.push_back(stmt);
  }

  ast.varMap.save();
  analyzeForwardStmt(ast, block);
  ast.nodes = analyzeBlock(ast, block);
  ast.varMap.restore();

  return ast;
}
