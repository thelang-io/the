/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"
#include "Parser.hpp"

AST::AST (Parser *p) {
  this->parser = p;
  this->reader = this->parser->reader;
}

ASTBlock AST::gen () {
  this->typeMap.init();
  this->varMap.init(this->typeMap);

  auto block = ParserBlock{};

  while (true) {
    auto stmt = this->parser->next();

    if (std::holds_alternative<ParserStmtEof>(stmt.body)) {
      break;
    }

    block.push_back(stmt);
  }

  this->varMap.save();
  auto mainVarStack = VarStack({});
  auto result = this->_block(block, mainVarStack);
  this->varMap.restore();

  return result;
}

ASTBlock AST::_block (const ParserBlock &block, VarStack &varStack) {
  this->_forwardStmt(block);
  auto result = ASTBlock{};

  for (const auto &stmt : block) {
    result.push_back(this->_stmt(stmt, varStack));
  }

  return result;
}

ASTExprAccess AST::_exprAccess (const ParserMemberObj &exprAccessBody, VarStack &varStack) const {
  if (std::holds_alternative<Token>(exprAccessBody)) {
    auto parserId = std::get<Token>(exprAccessBody);
    auto var = this->varMap.get(parserId.val);

    if (var == nullptr) {
      throw Error("TODO use of undeclared variable");
    }

    varStack.mark(var);
    return ASTExprAccess{var};
  }

  auto parserMember = std::get<std::shared_ptr<ParserMember>>(exprAccessBody);
  auto memberObj = this->_exprAccess(parserMember->obj, varStack).body;
  auto member = std::make_shared<ASTMember>(ASTMember{memberObj, parserMember->prop.val});

  if (std::holds_alternative<Token>(parserMember->obj)) {
    auto parserId = std::get<Token>(parserMember->obj);
    auto var = this->varMap.get(parserId.val);

    varStack.mark(var);
  }

  return ASTExprAccess{member};
}

std::shared_ptr<Type> AST::_exprAccessType (const ParserMemberObj &exprAccessBody) const {
  if (std::holds_alternative<Token>(exprAccessBody)) {
    auto parserId = std::get<Token>(exprAccessBody);
    auto var = this->varMap.get(parserId.val);

    if (var == nullptr) {
      return nullptr;
    }

    return var->type;
  }

  auto parserMember = std::get<std::shared_ptr<ParserMember>>(exprAccessBody);
  auto memberObjType = this->_exprAccessType(parserMember->obj);

  if (memberObjType == nullptr) {
    return nullptr;
  }

  if (!memberObjType->isObj()) {
    if (std::holds_alternative<Token>(parserMember->obj)) {
      auto parserId = std::get<Token>(parserMember->obj);
      throw Error(this->reader, parserId.start, parserMember->prop.end, E1000);
    } else {
      auto exprAccessProp = std::get<std::shared_ptr<ParserMember>>(parserMember->obj)->prop;
      throw Error(this->reader, exprAccessProp.start, parserMember->prop.end, E1000);
    }
  }

  auto memberObj = std::get<TypeObj>(memberObjType->body);

  auto memberObjField = std::find_if(memberObj.fields.begin(), memberObj.fields.end(), [&parserMember] (const auto &it) -> bool {
    return it.name == parserMember->prop.val;
  });

  if (memberObjField == memberObj.fields.end()) {
    throw Error(this->reader, parserMember->prop.start, parserMember->prop.end, E1001);
  }

  return memberObjField->type;
}

void AST::_forwardStmt (const ParserBlock &block) {
  for (const auto &stmt : block) {
    if (std::holds_alternative<ParserStmtObjDecl>(stmt.body)) {
      auto stmtObjDecl = std::get<ParserStmtObjDecl>(stmt.body);
      auto objName = stmtObjDecl.id.val;
      auto objTypeName = this->typeMap.name(objName);

      auto objType = this->typeMap.add(objTypeName, std::vector<TypeObjField>{});
      this->varMap.add(objName, objTypeName, objType, false);
    }
  }
}

ASTNode AST::_stmt (const ParserStmt &stmt, VarStack &varStack) {
  if (std::holds_alternative<ParserStmtBreak>(stmt.body)) {
    auto nodeBreak = ASTNodeBreak{};
    return ASTNode{nodeBreak};
  } else if (std::holds_alternative<ParserStmtContinue>(stmt.body)) {
    auto nodeContinue = ASTNodeContinue{};
    return ASTNode{nodeContinue};
  } else if (std::holds_alternative<ParserStmtExpr>(stmt.body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(stmt.body);
    auto stmtExprPtr = std::make_shared<ParserStmtExpr>(stmtExpr);
    auto nodeExpr = this->_stmtExpr(stmtExprPtr, varStack);

    return ASTNode{*nodeExpr};
  } else if (std::holds_alternative<ParserStmtFnDecl>(stmt.body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(stmt.body);
    auto nodeFnDeclVarName = stmtFnDecl.id.val;
    auto nodeFnDeclVarCodeName = this->typeMap.name(nodeFnDeclVarName);
    auto nodeFnDeclVarReturnType = stmtFnDecl.returnType == std::nullopt
      ? this->typeMap.get("void")
      : this->_type(stmtFnDecl.returnType);

    if (nodeFnDeclVarReturnType == nullptr) {
      throw Error("TODO use of undeclared type");
    }

    auto nodeFnDeclVarParams = std::vector<TypeFnParam>{};

    for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
      auto paramType = this->_type(stmtFnDeclParam.type, stmtFnDeclParam.init);

      if (paramType == nullptr) {
        throw Error("TODO use of undeclared type");
      }

      nodeFnDeclVarParams.push_back(TypeFnParam{
        stmtFnDeclParam.id.val,
        paramType,
        stmtFnDeclParam.init == std::nullopt && !stmtFnDeclParam.variadic,
        stmtFnDeclParam.variadic
      });
    }

    auto nodeFnDeclVarType = this->typeMap.add(nodeFnDeclVarCodeName, nodeFnDeclVarParams, nodeFnDeclVarReturnType);
    auto nodeFnDeclVar = this->varMap.add(nodeFnDeclVarName, nodeFnDeclVarCodeName, nodeFnDeclVarType, false);
    auto nodeFnDeclParams = std::vector<ASTNodeFnDeclParam>{};
    auto nodeFnDeclVarStack = this->varMap.stack();

    this->typeMap.stack.emplace_back(nodeFnDeclVar->name);
    this->varMap.save();

    for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
      auto paramInit = std::optional<std::shared_ptr<ASTNodeExpr>>{};

      if (stmtFnDeclParam.init != std::nullopt) {
        paramInit = this->_stmtExpr(*stmtFnDeclParam.init, nodeFnDeclVarStack);
      }

      auto paramName = stmtFnDeclParam.id.val;
      auto paramCodeName = this->varMap.name(paramName);
      auto paramType = this->_type(stmtFnDeclParam.type, stmtFnDeclParam.init);

      if (paramType == nullptr) {
        throw Error("TODO use of undeclared type");
      }

      auto paramVar = this->varMap.add(paramName, paramCodeName, paramType, false);
      nodeFnDeclParams.push_back(ASTNodeFnDeclParam{paramVar, paramInit});
    }

    auto nodeFnDeclBody = this->_block(stmtFnDecl.body, nodeFnDeclVarStack);
    this->varMap.restore();
    this->typeMap.stack.pop_back();

    auto nodeFnDeclStack = nodeFnDeclVarStack.snapshot();
    auto nodeFnDecl = ASTNodeFnDecl{nodeFnDeclVar, nodeFnDeclStack, nodeFnDeclParams, nodeFnDeclBody};

    return ASTNode{nodeFnDecl};
  } else if (std::holds_alternative<ParserStmtIf>(stmt.body)) {
    auto nodeIf = this->_stmtIf(std::get<ParserStmtIf>(stmt.body), varStack);
    return ASTNode{nodeIf};
  } else if (std::holds_alternative<ParserStmtLoop>(stmt.body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(stmt.body);
    auto nodeLoopInit = std::optional<std::shared_ptr<ASTNode>>{};
    auto nodeLoopCond = std::optional<std::shared_ptr<ASTNodeExpr>>{};
    auto nodeLoopUpd = std::optional<std::shared_ptr<ASTNodeExpr>>{};

    this->varMap.save();

    if (stmtLoop.init != std::nullopt) {
      nodeLoopInit = std::make_shared<ASTNode>(this->_stmt(**stmtLoop.init, varStack));
    }

    if (stmtLoop.cond != std::nullopt) {
      nodeLoopCond = this->_stmtExpr(*stmtLoop.cond, varStack);
    }

    if (stmtLoop.upd != std::nullopt) {
      nodeLoopUpd = this->_stmtExpr(*stmtLoop.upd, varStack);
    }

    auto nodeLoopBody = this->_block(stmtLoop.body, varStack);
    this->varMap.restore();

    auto nodeLoop = ASTNodeLoop{nodeLoopInit, nodeLoopCond, nodeLoopUpd, nodeLoopBody};
    return ASTNode{nodeLoop};
  } else if (std::holds_alternative<ParserStmtMain>(stmt.body)) {
    auto stmtMain = std::get<ParserStmtMain>(stmt.body);

    this->typeMap.stack.emplace_back("main");
    this->varMap.save();

    auto nodeMainBody = this->_block(stmtMain.body, varStack);

    this->varMap.restore();
    this->typeMap.stack.pop_back();

    auto nodeMain = ASTNodeMain{nodeMainBody};
    return ASTNode{nodeMain};
  } else if (std::holds_alternative<ParserStmtObjDecl>(stmt.body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(stmt.body);
    auto nodeObjDeclVar = this->varMap.get(stmtObjDecl.id.val);
    auto &nodeObjDeclObj = std::get<TypeObj>(nodeObjDeclVar->type->body);

    for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
      auto fieldType = this->_type(stmtObjDeclField.type);

      if (fieldType == nullptr) {
        throw Error("TODO use of undeclared type");
      }

      nodeObjDeclObj.fields.push_back(TypeObjField{stmtObjDeclField.id.val, fieldType});
    }

    auto nodeObjDecl = ASTNodeObjDecl{nodeObjDeclVar};
    return ASTNode{nodeObjDecl};
  } else if (std::holds_alternative<ParserStmtReturn>(stmt.body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(stmt.body);
    auto nodeReturnBody = std::optional<std::shared_ptr<ASTNodeExpr>>{};

    if (stmtReturn.body != std::nullopt) {
      nodeReturnBody = this->_stmtExpr(*stmtReturn.body, varStack);
    }

    auto nodeReturn = ASTNodeReturn{nodeReturnBody};
    return ASTNode{nodeReturn};
  } else if (std::holds_alternative<ParserStmtVarDecl>(stmt.body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(stmt.body);
    auto nodeVarDeclName = stmtVarDecl.id.val;
    auto nodeVarDeclCodeName = this->varMap.name(nodeVarDeclName);
    auto nodeVarDeclType = this->_type(stmtVarDecl.type, stmtVarDecl.init);

    if (nodeVarDeclType == nullptr) {
      throw Error("TODO use of undeclared type");
    }

    auto nodeVarDeclVar = this->varMap.add(nodeVarDeclName, nodeVarDeclCodeName, nodeVarDeclType, stmtVarDecl.mut);
    auto nodeVarDeclInit = std::optional<std::shared_ptr<ASTNodeExpr>>{};

    if (stmtVarDecl.init != std::nullopt) {
      nodeVarDeclInit = this->_stmtExpr(*stmtVarDecl.init, varStack);
      (*nodeVarDeclInit)->type = nodeVarDeclType;
    }

    auto nodeVarDecl = ASTNodeVarDecl{nodeVarDeclVar, nodeVarDeclInit};
    return ASTNode{nodeVarDecl};
  }

  throw Error("Error: tried to analyze unknown statement");
}

std::shared_ptr<ASTNodeExpr> AST::_stmtExpr (const std::shared_ptr<ParserStmtExpr> &stmtExpr, VarStack &varStack) const {
  auto nodeExprType = this->_stmtExprType(stmtExpr);

  if (std::holds_alternative<ParserExprAccess>(stmtExpr->body)) {
    auto parserExprAccess = std::get<ParserExprAccess>(stmtExpr->body);

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      this->_exprAccess(parserExprAccess.body, varStack),
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprAssign>(stmtExpr->body)) {
    auto parserExprAssign = std::get<ParserExprAssign>(stmtExpr->body);
    auto exprAssignOp = ASTExprAssignOp{};

    if (parserExprAssign.op.type == TK_OP_AND_AND_EQ) exprAssignOp = AST_EXPR_ASSIGN_LOGICAL_AND;
    if (parserExprAssign.op.type == TK_OP_AND_EQ) exprAssignOp = AST_EXPR_ASSIGN_BITWISE_AND;
    if (parserExprAssign.op.type == TK_OP_CARET_EQ) exprAssignOp = AST_EXPR_ASSIGN_BITWISE_XOR;
    if (parserExprAssign.op.type == TK_OP_EQ) exprAssignOp = AST_EXPR_ASSIGN_EQUAL;
    if (parserExprAssign.op.type == TK_OP_LSHIFT_EQ) exprAssignOp = AST_EXPR_ASSIGN_LEFT_SHIFT;
    if (parserExprAssign.op.type == TK_OP_MINUS_EQ) exprAssignOp = AST_EXPR_ASSIGN_SUBTRACT;
    if (parserExprAssign.op.type == TK_OP_OR_EQ) exprAssignOp = AST_EXPR_ASSIGN_BITWISE_OR;
    if (parserExprAssign.op.type == TK_OP_OR_OR_EQ) exprAssignOp = AST_EXPR_ASSIGN_LOGICAL_OR;
    if (parserExprAssign.op.type == TK_OP_PERCENT_EQ) exprAssignOp = AST_EXPR_ASSIGN_REMAINDER;
    if (parserExprAssign.op.type == TK_OP_PLUS_EQ) exprAssignOp = AST_EXPR_ASSIGN_ADD;
    if (parserExprAssign.op.type == TK_OP_QN_QN_EQ) exprAssignOp = AST_EXPR_ASSIGN_COALESCE;
    if (parserExprAssign.op.type == TK_OP_RSHIFT_EQ) exprAssignOp = AST_EXPR_ASSIGN_RIGHT_SHIFT;
    if (parserExprAssign.op.type == TK_OP_SLASH_EQ) exprAssignOp = AST_EXPR_ASSIGN_DIVIDE;
    if (parserExprAssign.op.type == TK_OP_STAR_EQ) exprAssignOp = AST_EXPR_ASSIGN_MULTIPLY;
    if (parserExprAssign.op.type == TK_OP_STAR_STAR_EQ) exprAssignOp = AST_EXPR_ASSIGN_POWER;

    auto exprAssignLeft = this->_exprAccess(parserExprAssign.left.body, varStack);
    auto exprAssignRight = this->_stmtExpr(parserExprAssign.right, varStack);

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprAssign{exprAssignLeft, exprAssignOp, exprAssignRight},
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprBinary>(stmtExpr->body)) {
    auto parserExprBinary = std::get<ParserExprBinary>(stmtExpr->body);
    auto exprBinaryOp = ASTExprBinaryOp{};

    if (parserExprBinary.op.type == TK_OP_AND) exprBinaryOp = AST_EXPR_BINARY_BITWISE_AND;
    if (parserExprBinary.op.type == TK_OP_AND_AND) exprBinaryOp = AST_EXPR_BINARY_LOGICAL_AND;
    if (parserExprBinary.op.type == TK_OP_CARET) exprBinaryOp = AST_EXPR_BINARY_BITWISE_XOR;
    if (parserExprBinary.op.type == TK_OP_EQ_EQ) exprBinaryOp = AST_EXPR_BINARY_EQUAL;
    if (parserExprBinary.op.type == TK_OP_EXCL_EQ) exprBinaryOp = AST_EXPR_BINARY_NOT_EQUAL;
    if (parserExprBinary.op.type == TK_OP_GT) exprBinaryOp = AST_EXPR_BINARY_GREATER_THAN;
    if (parserExprBinary.op.type == TK_OP_GT_EQ) exprBinaryOp = AST_EXPR_BINARY_GREATER_EQUAL;
    if (parserExprBinary.op.type == TK_OP_LSHIFT) exprBinaryOp = AST_EXPR_BINARY_LEFT_SHIFT;
    if (parserExprBinary.op.type == TK_OP_LT) exprBinaryOp = AST_EXPR_BINARY_LESS_THAN;
    if (parserExprBinary.op.type == TK_OP_LT_EQ) exprBinaryOp = AST_EXPR_BINARY_LESS_EQUAL;
    if (parserExprBinary.op.type == TK_OP_MINUS) exprBinaryOp = AST_EXPR_BINARY_SUBTRACT;
    if (parserExprBinary.op.type == TK_OP_OR) exprBinaryOp = AST_EXPR_BINARY_BITWISE_OR;
    if (parserExprBinary.op.type == TK_OP_OR_OR) exprBinaryOp = AST_EXPR_BINARY_LOGICAL_OR;
    if (parserExprBinary.op.type == TK_OP_PERCENT) exprBinaryOp = AST_EXPR_BINARY_REMAINDER;
    if (parserExprBinary.op.type == TK_OP_PLUS) exprBinaryOp = AST_EXPR_BINARY_ADD;
    if (parserExprBinary.op.type == TK_OP_QN_QN) exprBinaryOp = AST_EXPR_BINARY_COALESCE;
    if (parserExprBinary.op.type == TK_OP_RSHIFT) exprBinaryOp = AST_EXPR_BINARY_RIGHT_SHIFT;
    if (parserExprBinary.op.type == TK_OP_SLASH) exprBinaryOp = AST_EXPR_BINARY_DIVIDE;
    if (parserExprBinary.op.type == TK_OP_STAR) exprBinaryOp = AST_EXPR_BINARY_MULTIPLY;
    if (parserExprBinary.op.type == TK_OP_STAR_STAR) exprBinaryOp = AST_EXPR_BINARY_POWER;

    auto exprBinaryLeft = this->_stmtExpr(parserExprBinary.left, varStack);
    auto exprBinaryRight = this->_stmtExpr(parserExprBinary.right, varStack);

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprBinary{exprBinaryLeft, exprBinaryOp, exprBinaryRight},
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprCall>(stmtExpr->body)) {
    auto parserExprCall = std::get<ParserExprCall>(stmtExpr->body);
    auto exprCallCallee = this->_exprAccess(parserExprCall.callee.body, varStack);
    auto exprCallCalleeType = this->_exprAccessType(parserExprCall.callee.body);
    auto exprCallCalleeFn = std::get<TypeFn>(exprCallCalleeType->body);
    auto exprCallArgs = std::vector<ASTExprCallArg>{};
    auto exprCallArgIdx = static_cast<std::size_t>(0);
    auto passedArgs = std::vector<std::string>{};
    auto hasNamedArgs = false;
    auto isArgVariadic = false;
    auto variadicArgType = std::optional<TypeFnParam>{};

    for (const auto &parserExprCallArg : parserExprCall.args) {
      auto foundParam = std::optional<TypeFnParam>{};

      if (parserExprCallArg.id != std::nullopt) {
        auto exprCallArgName = parserExprCallArg.id->val;

        if (std::find(passedArgs.begin(), passedArgs.end(), exprCallArgName) != passedArgs.end()) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr->end, E1005);
        }

        for (const auto &calleeFnParam : exprCallCalleeFn.params) {
          if (calleeFnParam.name == exprCallArgName) {
            foundParam = calleeFnParam;
            break;
          }
        }

        if (foundParam == std::nullopt) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr->end, E1002);
        } else if (foundParam->variadic) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr->end, E1006);
        }

        hasNamedArgs = true;
        isArgVariadic = false;
        variadicArgType = std::nullopt;
      } else if (hasNamedArgs) {
        throw Error(this->reader, parserExprCallArg.expr->start, parserExprCallArg.expr->end, E1007);
      } else if (isArgVariadic) {
        foundParam = variadicArgType;
      } else if (exprCallArgIdx >= exprCallCalleeFn.params.size()) {
        throw Error(this->reader, parserExprCallArg.expr->start, parserExprCallArg.expr->end, E1005);
      } else {
        foundParam = exprCallCalleeFn.params[exprCallArgIdx];
      }

      if (!isArgVariadic && foundParam->variadic) {
        isArgVariadic = true;
        variadicArgType = foundParam;
      }

      auto exprCallArgId = std::optional<std::string>{};

      if (parserExprCallArg.id != std::nullopt) {
        exprCallArgId = parserExprCallArg.id->val;
      }

      auto exprCallArgExpr = this->_stmtExpr(parserExprCallArg.expr, varStack);

      if (!foundParam->type->match(exprCallArgExpr->type)) {
        throw Error(this->reader, parserExprCallArg.expr->start, parserExprCallArg.expr->end, E1008);
      }

      exprCallArgExpr->type = foundParam->type;
      exprCallArgs.push_back(ASTExprCallArg{exprCallArgId, exprCallArgExpr});

      if (!isArgVariadic) {
        passedArgs.push_back(foundParam->name);
        exprCallArgIdx++;
      }
    }

    for (const auto &calleeFnParam : exprCallCalleeFn.params) {
      if (calleeFnParam.required && std::find(passedArgs.begin(), passedArgs.end(), calleeFnParam.name) == passedArgs.end()) {
        throw Error(this->reader, stmtExpr->start, stmtExpr->end, E1009);
      }
    }

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprCall{exprCallCallee, exprCallCalleeType, exprCallArgs},
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprCond>(stmtExpr->body)) {
    auto parserExprCond = std::get<ParserExprCond>(stmtExpr->body);
    auto exprCondCond = this->_stmtExpr(parserExprCond.cond, varStack);
    auto exprCondBody = this->_stmtExpr(parserExprCond.body, varStack);
    auto exprCondAlt = this->_stmtExpr(parserExprCond.alt, varStack);

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprCond{exprCondCond, exprCondBody, exprCondAlt},
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprLit>(stmtExpr->body)) {
    auto parserExprLit = std::get<ParserExprLit>(stmtExpr->body);
    auto exprLitType = ASTExprLitType{};

    if (parserExprLit.body.type == TK_KW_FALSE) exprLitType = AST_EXPR_LIT_BOOL;
    if (parserExprLit.body.type == TK_KW_TRUE) exprLitType = AST_EXPR_LIT_BOOL;
    if (parserExprLit.body.type == TK_LIT_CHAR) exprLitType = AST_EXPR_LIT_CHAR;
    if (parserExprLit.body.type == TK_LIT_FLOAT) exprLitType = AST_EXPR_LIT_FLOAT;
    if (parserExprLit.body.type == TK_LIT_INT_BIN) exprLitType = AST_EXPR_LIT_INT_BIN;
    if (parserExprLit.body.type == TK_LIT_INT_DEC) exprLitType = AST_EXPR_LIT_INT_DEC;
    if (parserExprLit.body.type == TK_LIT_INT_HEX) exprLitType = AST_EXPR_LIT_INT_HEX;
    if (parserExprLit.body.type == TK_LIT_INT_OCT) exprLitType = AST_EXPR_LIT_INT_OCT;
    if (parserExprLit.body.type == TK_LIT_STR) exprLitType = AST_EXPR_LIT_STR;

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprLit{exprLitType, parserExprLit.body.val},
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprObj>(stmtExpr->body)) {
    auto parserExprObj = std::get<ParserExprObj>(stmtExpr->body);
    auto exprObjVar = this->varMap.get(parserExprObj.id.val);

    if (exprObjVar == nullptr) {
      throw Error("TODO use of undeclared object");
    }

    auto exprObjType = exprObjVar->type;
    auto exprObjProps = std::vector<ASTExprObjProp>();

    varStack.mark(exprObjVar);

    for (const auto &parserExprObjProp : parserExprObj.props) {
      exprObjProps.push_back(ASTExprObjProp{parserExprObjProp.id.val, this->_stmtExpr(parserExprObjProp.init, varStack)});
    }

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprObj{exprObjType, exprObjProps},
      stmtExpr->parenthesized
    });
  } else if (std::holds_alternative<ParserExprUnary>(stmtExpr->body)) {
    auto parserExprUnary = std::get<ParserExprUnary>(stmtExpr->body);
    auto op = ASTExprUnaryOp{};

    if (parserExprUnary.op.type == TK_OP_EXCL) op = AST_EXPR_UNARY_LOGICAL_NOT;
    if (parserExprUnary.op.type == TK_OP_EXCL_EXCL) op = AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT;
    if (parserExprUnary.op.type == TK_OP_MINUS) op = AST_EXPR_UNARY_NEGATION;
    if (parserExprUnary.op.type == TK_OP_MINUS_MINUS) op = AST_EXPR_UNARY_DECREMENT;
    if (parserExprUnary.op.type == TK_OP_PLUS) op = AST_EXPR_UNARY_PLUS;
    if (parserExprUnary.op.type == TK_OP_PLUS_PLUS) op = AST_EXPR_UNARY_INCREMENT;
    if (parserExprUnary.op.type == TK_OP_TILDE) op = AST_EXPR_UNARY_BITWISE_NOT;

    auto exprUnaryArg = this->_stmtExpr(parserExprUnary.arg, varStack);

    return std::make_shared<ASTNodeExpr>(ASTNodeExpr{
      nodeExprType,
      ASTExprUnary{exprUnaryArg, op, parserExprUnary.prefix},
      stmtExpr->parenthesized
    });
  }

  throw Error("Error: tried to analyze unknown expression");
}

std::shared_ptr<Type> AST::_stmtExprType (const std::shared_ptr<ParserStmtExpr> &stmtExpr) const {
  if (std::holds_alternative<ParserExprAccess>(stmtExpr->body)) {
    auto exprAccess = std::get<ParserExprAccess>(stmtExpr->body);
    return this->_exprAccessType(exprAccess.body);
  } else if (std::holds_alternative<ParserExprAssign>(stmtExpr->body)) {
    auto exprAssign = std::get<ParserExprAssign>(stmtExpr->body);
    return this->_exprAccessType(exprAssign.left.body);
  } else if (std::holds_alternative<ParserExprBinary>(stmtExpr->body)) {
    auto exprBinary = std::get<ParserExprBinary>(stmtExpr->body);
    auto exprBinaryLeftType = this->_stmtExprType(exprBinary.left);
    auto exprBinaryRightType = this->_stmtExprType(exprBinary.right);

    if (exprBinaryLeftType == nullptr || exprBinaryRightType == nullptr) {
      return nullptr;
    } else if (
      exprBinaryLeftType->isChar() ||
      exprBinaryLeftType->isStr() ||
      exprBinaryRightType->isChar() ||
      exprBinaryRightType->isStr()
    ) {
      if (exprBinary.op.type != TK_OP_PLUS) {
        throw Error(this->reader, exprBinary.left->start, exprBinary.right->end, E1003);
      }

      return this->typeMap.get("str");
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
      return this->typeMap.get("float");
    } else if (
      exprBinary.op.type == TK_OP_EQ_EQ ||
      exprBinary.op.type == TK_OP_EXCL_EQ ||
      exprBinary.op.type == TK_OP_GT ||
      exprBinary.op.type == TK_OP_GT_EQ ||
      exprBinary.op.type == TK_OP_LT ||
      exprBinary.op.type == TK_OP_LT_EQ
    ) {
      return this->typeMap.get("bool");
    } else {
      return this->typeMap.get("int");
    }
  } else if (std::holds_alternative<ParserExprCall>(stmtExpr->body)) {
    auto exprCall = std::get<ParserExprCall>(stmtExpr->body);
    auto exprCallCalleeType = this->_exprAccessType(exprCall.callee.body);

    if (exprCallCalleeType == nullptr) {
      return nullptr;
    }

    return std::get<TypeFn>(exprCallCalleeType->body).returnType;
  } else if (std::holds_alternative<ParserExprCond>(stmtExpr->body)) {
    auto exprCond = std::get<ParserExprCond>(stmtExpr->body);
    auto exprCondBodyType = this->_stmtExprType(exprCond.body);
    auto exprCondAltType = this->_stmtExprType(exprCond.alt);

    if (exprCondBodyType == nullptr || exprCondAltType == nullptr) {
      return nullptr;
    } else if (exprCondBodyType->name != exprCondAltType->name) {
      throw Error(this->reader, exprCond.body->start, exprCond.alt->end, E1004);
    }

    return exprCondBodyType;
  } else if (std::holds_alternative<ParserExprLit>(stmtExpr->body)) {
    auto exprLit = std::get<ParserExprLit>(stmtExpr->body);

    if (exprLit.body.type == TK_KW_FALSE) return this->typeMap.get("bool");
    if (exprLit.body.type == TK_KW_TRUE) return this->typeMap.get("bool");
    if (exprLit.body.type == TK_LIT_CHAR) return this->typeMap.get("char");
    if (exprLit.body.type == TK_LIT_FLOAT) return this->typeMap.get("float");
    if (exprLit.body.type == TK_LIT_INT_BIN) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_INT_DEC) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_INT_HEX) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_INT_OCT) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_STR) return this->typeMap.get("str");
  } else if (std::holds_alternative<ParserExprObj>(stmtExpr->body)) {
    auto exprObj = std::get<ParserExprObj>(stmtExpr->body);
    auto exprObjVar = this->varMap.get(exprObj.id.val);

    if (exprObjVar == nullptr) {
      return nullptr;
    }

    return exprObjVar->type;
  } else if (std::holds_alternative<ParserExprUnary>(stmtExpr->body)) {
    auto exprUnary = std::get<ParserExprUnary>(stmtExpr->body);
    auto exprUnaryType = this->_stmtExprType(exprUnary.arg);

    if (exprUnaryType == nullptr) {
      return nullptr;
    } else if (
      exprUnaryType->isBool() ||
      exprUnaryType->isChar() ||
      exprUnaryType->isFn() ||
      exprUnaryType->isObj() ||
      exprUnaryType->isStr()
    ) {
      return this->typeMap.get("int");
    }

    return exprUnaryType;
  }

  throw Error("Error: tried to analyze unknown expression type");
}

ASTNodeIf AST::_stmtIf (const ParserStmtIf &stmtIf, VarStack &varStack) {
  this->varMap.save();
  auto cond = std::make_shared<ASTNode>(this->_stmt(*stmtIf.cond, varStack));
  auto body = this->_block(stmtIf.body, varStack);
  auto alt = std::optional<std::shared_ptr<ASTNodeIfCond>>{};
  this->varMap.restore();

  if (stmtIf.alt != std::nullopt) {
    if (std::holds_alternative<ParserBlock>(**stmtIf.alt)) {
      this->varMap.save();
      auto stmtIfAltBlock = std::get<ParserBlock>(**stmtIf.alt);
      auto nodeIfAltElse = this->_block(std::get<ParserBlock>(**stmtIf.alt), varStack);
      this->varMap.restore();

      alt = std::make_shared<ASTNodeIfCond>(nodeIfAltElse);
    } else if (std::holds_alternative<ParserStmtIf>(**stmtIf.alt)) {
      auto nodeIfAltElif = this->_stmtIf(std::get<ParserStmtIf>(**stmtIf.alt), varStack);
      alt = std::make_shared<ASTNodeIfCond>(nodeIfAltElif);
    }
  }

  return ASTNodeIf{cond, body, alt};
}

std::shared_ptr<Type> AST::_type (
  const std::optional<std::shared_ptr<ParserType>> &type,
  const std::optional<std::shared_ptr<ParserStmtExpr>> &init
) const {
  if (type != std::nullopt && std::holds_alternative<ParserTypeFn>((*type)->body)) {
    auto typeFn = std::get<ParserTypeFn>((*type)->body);
    auto fnReturnType = this->_type(typeFn.returnType);

    if (fnReturnType == nullptr) {
      throw Error("TODO use of undeclared type");
    }

    auto fnParams = std::vector<TypeFnParam>{};

    for (const auto &typeFnParam : typeFn.params) {
      auto paramType = this->_type(typeFnParam.type);

      if (paramType == nullptr) {
        throw Error("TODO use of undeclared type");
      }

      fnParams.push_back(TypeFnParam{"@", paramType, !typeFnParam.variadic, typeFnParam.variadic});
    }

    return TypeMap::fn(fnReturnType, fnParams);
  } else if (type != std::nullopt && std::holds_alternative<ParserTypeId>((*type)->body)) {
    auto typeId = std::get<ParserTypeId>((*type)->body);

    if (typeId.id.val == "any") return this->typeMap.get("any");
    if (typeId.id.val == "bool") return this->typeMap.get("bool");
    if (typeId.id.val == "byte") return this->typeMap.get("byte");
    if (typeId.id.val == "char") return this->typeMap.get("char");
    if (typeId.id.val == "float") return this->typeMap.get("float");
    if (typeId.id.val == "f32") return this->typeMap.get("f32");
    if (typeId.id.val == "f64") return this->typeMap.get("f64");
    if (typeId.id.val == "int") return this->typeMap.get("int");
    if (typeId.id.val == "i8") return this->typeMap.get("i8");
    if (typeId.id.val == "i16") return this->typeMap.get("i16");
    if (typeId.id.val == "i32") return this->typeMap.get("i32");
    if (typeId.id.val == "i64") return this->typeMap.get("i64");
    if (typeId.id.val == "str") return this->typeMap.get("str");
    if (typeId.id.val == "u8") return this->typeMap.get("u8");
    if (typeId.id.val == "u16") return this->typeMap.get("u16");
    if (typeId.id.val == "u32") return this->typeMap.get("u32");
    if (typeId.id.val == "u64") return this->typeMap.get("u64");
    if (typeId.id.val == "void") return this->typeMap.get("void");

    if (this->varMap.has(typeId.id.val)) {
      auto var = this->varMap.get(typeId.id.val);

      if (var == nullptr) {
        return nullptr;
      }

      return var->type;
    }

    throw Error(this->reader, typeId.id.start, typeId.id.end, E1010);
  }

  return this->_stmtExprType(*init);
}
