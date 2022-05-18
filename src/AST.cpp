/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"
#include "Parser.hpp"
#include "config.hpp"

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

    if (std::holds_alternative<ParserStmtEof>(*stmt.body)) {
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

std::string AST::xml () {
  auto result = std::string();

  for (const auto &node : this->gen()) {
    result += node.xml() + EOL;
  }

  return result;
}

ASTBlock AST::_block (const ParserBlock &block, VarStack &varStack) {
  this->_forwardNode(block);
  auto result = ASTBlock{};

  for (const auto &stmt : block) {
    result.push_back(this->_node(stmt, varStack));
  }

  return result;
}

ASTExprAccess AST::_exprAccess (const std::shared_ptr<ParserMemberObj> &exprAccessBody, VarStack &varStack) {
  if (std::holds_alternative<Token>(*exprAccessBody)) {
    auto parserId = std::get<Token>(*exprAccessBody);
    auto var = this->varMap.get(parserId.val);

    if (var == nullptr) {
      throw Error(this->reader, parserId.start, parserId.end, E1011);
    }

    varStack.mark(var);
    return ASTExprAccess{std::make_shared<ASTMemberObj>(var)};
  }

  auto parserMember = std::get<ParserMember>(*exprAccessBody);
  auto memberObj = this->_exprAccess(parserMember.obj, varStack).body;
  auto member = ASTMember{memberObj, parserMember.prop.val};

  if (std::holds_alternative<Token>(*parserMember.obj)) {
    auto parserId = std::get<Token>(*parserMember.obj);
    auto var = this->varMap.get(parserId.val);

    varStack.mark(var);
  }

  return ASTExprAccess{std::make_shared<ASTMemberObj>(member)};
}

Type *AST::_exprAccessType (const std::shared_ptr<ParserMemberObj> &exprAccessBody) {
  if (std::holds_alternative<Token>(*exprAccessBody)) {
    auto parserId = std::get<Token>(*exprAccessBody);
    auto var = this->varMap.get(parserId.val);

    if (var == nullptr) {
      throw Error(this->reader, parserId.start, parserId.end, E1011);
    }

    return var->type;
  }

  auto parserMember = std::get<ParserMember>(*exprAccessBody);
  auto memberObjType = this->_exprAccessType(parserMember.obj);

  if (!memberObjType->isObj()) {
    if (std::holds_alternative<Token>(*parserMember.obj)) {
      auto parserId = std::get<Token>(*parserMember.obj);
      throw Error(this->reader, parserId.start, parserMember.prop.end, E1000);
    } else {
      auto exprAccessProp = std::get<ParserMember>(*parserMember.obj).prop;
      throw Error(this->reader, exprAccessProp.start, parserMember.prop.end, E1000);
    }
  }

  auto memberObj = std::get<TypeObj>(memberObjType->body);

  auto memberObjField = std::find_if(memberObj.fields.begin(), memberObj.fields.end(), [&parserMember] (const auto &it) -> bool {
    return it.name == parserMember.prop.val;
  });

  if (memberObjField == memberObj.fields.end()) {
    throw Error(this->reader, parserMember.prop.start, parserMember.prop.end, E1001);
  }

  return memberObjField->type;
}

void AST::_forwardNode (const ParserBlock &block) {
  for (const auto &stmt : block) {
    if (std::holds_alternative<ParserStmtObjDecl>(*stmt.body)) {
      auto stmtObjDecl = std::get<ParserStmtObjDecl>(*stmt.body);
      auto objName = stmtObjDecl.id.val;
      auto objTypeName = this->typeMap.name(objName);

      auto objType = this->typeMap.add(objTypeName, std::vector<TypeObjField>{});
      this->varMap.add(objName, objTypeName, objType, false);
    }
  }
}

ASTNode AST::_node (const ParserStmt &stmt, VarStack &varStack) {
  if (std::holds_alternative<ParserStmtBreak>(*stmt.body)) {
    auto nodeBreak = ASTNodeBreak{};
    return this->_wrapNode(stmt, nodeBreak);
  } else if (std::holds_alternative<ParserStmtContinue>(*stmt.body)) {
    auto nodeContinue = ASTNodeContinue{};
    return this->_wrapNode(stmt, nodeContinue);
  } else if (std::holds_alternative<ParserStmtExpr>(*stmt.body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(*stmt.body);
    auto nodeExpr = this->_nodeExpr(stmtExpr, varStack);

    return this->_wrapNode(stmt, nodeExpr);
  } else if (std::holds_alternative<ParserStmtFnDecl>(*stmt.body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*stmt.body);
    auto nodeFnDeclParams = std::vector<ASTNodeFnDeclParam>{};
    auto nodeFnDeclVarParams = std::vector<TypeFnParam>{};
    auto nodeFnDeclVarStack = this->varMap.stack();

    this->varMap.save();

    for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
      auto paramName = stmtFnDeclParam.id.val;
      auto paramType = stmtFnDeclParam.type != std::nullopt
        ? this->_type(*stmtFnDeclParam.type)
        : this->_nodeExprType(*stmtFnDeclParam.init);
      auto paramVar = this->varMap.add(paramName, this->varMap.name(paramName), paramType, false);
      auto paramInit = std::optional<ASTNodeExpr>{};

      if (stmtFnDeclParam.init != std::nullopt) {
        paramInit = this->_nodeExpr(*stmtFnDeclParam.init, nodeFnDeclVarStack);
      }

      auto paramVariadic = stmtFnDeclParam.variadic;
      auto paramRequired = paramInit == std::nullopt && !paramVariadic;

      nodeFnDeclParams.push_back(ASTNodeFnDeclParam{paramVar, paramInit});
      nodeFnDeclVarParams.push_back(TypeFnParam{paramVar->name, paramVar->type, paramRequired, paramVariadic});
    }

    auto nodeFnDeclVarName = stmtFnDecl.id.val;
    auto nodeFnDeclVarReturnType = stmtFnDecl.returnType == std::nullopt
      ? this->typeMap.get("void")
      : this->_type(*stmtFnDecl.returnType);
    auto nodeFnDeclVarType = this->typeMap.add(this->typeMap.name(nodeFnDeclVarName), nodeFnDeclVarParams, nodeFnDeclVarReturnType);
    auto nodeFnDeclVar = this->varMap.add(nodeFnDeclVarName, nodeFnDeclVarType->name, nodeFnDeclVarType, false);

    nodeFnDeclVar->frame -= 1;

    this->typeMap.stack.emplace_back(nodeFnDeclVar->name);
    auto nodeFnDeclBody = this->_block(stmtFnDecl.body, nodeFnDeclVarStack);
    this->varMap.restore();
    this->typeMap.stack.pop_back();

    auto nodeFnDeclStack = nodeFnDeclVarStack.snapshot();
    auto nodeFnDecl = ASTNodeFnDecl{nodeFnDeclVar, nodeFnDeclStack, nodeFnDeclParams, nodeFnDeclBody};

    return this->_wrapNode(stmt, nodeFnDecl);
  } else if (std::holds_alternative<ParserStmtIf>(*stmt.body)) {
    auto nodeIf = this->_nodeIf(std::get<ParserStmtIf>(*stmt.body), varStack);
    return this->_wrapNode(stmt, nodeIf);
  } else if (std::holds_alternative<ParserStmtLoop>(*stmt.body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*stmt.body);
    auto nodeLoopInit = std::optional<ASTNode>{};
    auto nodeLoopCond = std::optional<ASTNodeExpr>{};
    auto nodeLoopUpd = std::optional<ASTNodeExpr>{};

    this->varMap.save();

    if (stmtLoop.init != std::nullopt) {
      nodeLoopInit = this->_node(*stmtLoop.init, varStack);
    }

    if (stmtLoop.cond != std::nullopt) {
      nodeLoopCond = this->_nodeExpr(*stmtLoop.cond, varStack);
    }

    if (stmtLoop.upd != std::nullopt) {
      nodeLoopUpd = this->_nodeExpr(*stmtLoop.upd, varStack);
    }

    auto nodeLoopBody = this->_block(stmtLoop.body, varStack);
    this->varMap.restore();

    auto nodeLoop = ASTNodeLoop{nodeLoopInit, nodeLoopCond, nodeLoopUpd, nodeLoopBody};
    return this->_wrapNode(stmt, nodeLoop);
  } else if (std::holds_alternative<ParserStmtMain>(*stmt.body)) {
    auto stmtMain = std::get<ParserStmtMain>(*stmt.body);

    this->typeMap.stack.emplace_back("main");
    this->varMap.save();

    auto nodeMainBody = this->_block(stmtMain.body, varStack);

    this->varMap.restore();
    this->typeMap.stack.pop_back();

    auto nodeMain = ASTNodeMain{nodeMainBody};
    return this->_wrapNode(stmt, nodeMain);
  } else if (std::holds_alternative<ParserStmtObjDecl>(*stmt.body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(*stmt.body);
    auto nodeObjDeclVar = this->varMap.get(stmtObjDecl.id.val);
    auto &nodeObjDeclObj = std::get<TypeObj>(nodeObjDeclVar->type->body);

    for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
      auto fieldType = this->_type(stmtObjDeclField.type);
      nodeObjDeclObj.fields.push_back(TypeObjField{stmtObjDeclField.id.val, fieldType});
    }

    auto nodeObjDecl = ASTNodeObjDecl{nodeObjDeclVar};
    return this->_wrapNode(stmt, nodeObjDecl);
  } else if (std::holds_alternative<ParserStmtReturn>(*stmt.body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(*stmt.body);
    auto nodeReturnBody = std::optional<ASTNodeExpr>{};

    if (stmtReturn.body != std::nullopt) {
      nodeReturnBody = this->_nodeExpr(*stmtReturn.body, varStack);
    }

    auto nodeReturn = ASTNodeReturn{nodeReturnBody};
    return this->_wrapNode(stmt, nodeReturn);
  } else if (std::holds_alternative<ParserStmtVarDecl>(*stmt.body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*stmt.body);
    auto nodeVarDeclName = stmtVarDecl.id.val;
    auto nodeVarDeclType = stmtVarDecl.type != std::nullopt
      ? this->_type(*stmtVarDecl.type)
      : this->_nodeExprType(*stmtVarDecl.init);
    auto nodeVarDeclInit = std::optional<ASTNodeExpr>{};

    if (stmtVarDecl.init != std::nullopt) {
      nodeVarDeclInit = this->_nodeExpr(*stmtVarDecl.init, varStack);
    }

    auto nodeVarDeclVar = this->varMap.add(nodeVarDeclName, this->varMap.name(nodeVarDeclName), nodeVarDeclType, stmtVarDecl.mut);
    auto nodeVarDecl = ASTNodeVarDecl{nodeVarDeclVar, nodeVarDeclInit};

    return this->_wrapNode(stmt, nodeVarDecl);
  }

  throw Error("Error: tried to analyze unknown statement");
}

ASTNodeExpr AST::_nodeExpr (const ParserStmtExpr &stmtExpr, VarStack &varStack) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto parserExprAccess = std::get<ParserExprAccess>(*stmtExpr.body);
    return this->_wrapNodeExpr(stmtExpr, this->_exprAccess(parserExprAccess.body, varStack));
  } else if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body)) {
    auto parserExprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto exprAssignOp = ASTExprAssignOp{};

    if (parserExprAssign.op.type == TK_OP_AMP_EQ) exprAssignOp = AST_EXPR_ASSIGN_BIT_AND;
    if (parserExprAssign.op.type == TK_OP_AMP_AMP_EQ) exprAssignOp = AST_EXPR_ASSIGN_AND;
    if (parserExprAssign.op.type == TK_OP_CARET_EQ) exprAssignOp = AST_EXPR_ASSIGN_BIT_XOR;
    if (parserExprAssign.op.type == TK_OP_EQ) exprAssignOp = AST_EXPR_ASSIGN_EQ;
    if (parserExprAssign.op.type == TK_OP_LSHIFT_EQ) exprAssignOp = AST_EXPR_ASSIGN_LSHIFT;
    if (parserExprAssign.op.type == TK_OP_MINUS_EQ) exprAssignOp = AST_EXPR_ASSIGN_SUB;
    if (parserExprAssign.op.type == TK_OP_PERCENT_EQ) exprAssignOp = AST_EXPR_ASSIGN_MOD;
    if (parserExprAssign.op.type == TK_OP_PIPE_EQ) exprAssignOp = AST_EXPR_ASSIGN_BIT_OR;
    if (parserExprAssign.op.type == TK_OP_PIPE_PIPE_EQ) exprAssignOp = AST_EXPR_ASSIGN_OR;
    if (parserExprAssign.op.type == TK_OP_PLUS_EQ) exprAssignOp = AST_EXPR_ASSIGN_ADD;
    if (parserExprAssign.op.type == TK_OP_RSHIFT_EQ) exprAssignOp = AST_EXPR_ASSIGN_RSHIFT;
    if (parserExprAssign.op.type == TK_OP_SLASH_EQ) exprAssignOp = AST_EXPR_ASSIGN_DIV;
    if (parserExprAssign.op.type == TK_OP_STAR_EQ) exprAssignOp = AST_EXPR_ASSIGN_MUL;

    auto exprAssignLeft = this->_exprAccess(parserExprAssign.left.body, varStack);
    auto exprAssignRight = this->_nodeExpr(parserExprAssign.right, varStack);

    return this->_wrapNodeExpr(stmtExpr, ASTExprAssign{exprAssignLeft, exprAssignOp, exprAssignRight});
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body)) {
    auto parserExprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto exprBinaryOp = ASTExprBinaryOp{};

    if (parserExprBinary.op.type == TK_OP_AMP) exprBinaryOp = AST_EXPR_BINARY_BIT_AND;
    if (parserExprBinary.op.type == TK_OP_AMP_AMP) exprBinaryOp = AST_EXPR_BINARY_AND;
    if (parserExprBinary.op.type == TK_OP_CARET) exprBinaryOp = AST_EXPR_BINARY_BIT_XOR;
    if (parserExprBinary.op.type == TK_OP_EQ_EQ) exprBinaryOp = AST_EXPR_BINARY_EQ;
    if (parserExprBinary.op.type == TK_OP_EXCL_EQ) exprBinaryOp = AST_EXPR_BINARY_NE;
    if (parserExprBinary.op.type == TK_OP_GT) exprBinaryOp = AST_EXPR_BINARY_GT;
    if (parserExprBinary.op.type == TK_OP_GT_EQ) exprBinaryOp = AST_EXPR_BINARY_GE;
    if (parserExprBinary.op.type == TK_OP_LSHIFT) exprBinaryOp = AST_EXPR_BINARY_LSHIFT;
    if (parserExprBinary.op.type == TK_OP_LT) exprBinaryOp = AST_EXPR_BINARY_LT;
    if (parserExprBinary.op.type == TK_OP_LT_EQ) exprBinaryOp = AST_EXPR_BINARY_LE;
    if (parserExprBinary.op.type == TK_OP_MINUS) exprBinaryOp = AST_EXPR_BINARY_SUB;
    if (parserExprBinary.op.type == TK_OP_PERCENT) exprBinaryOp = AST_EXPR_BINARY_MOD;
    if (parserExprBinary.op.type == TK_OP_PIPE) exprBinaryOp = AST_EXPR_BINARY_BIT_OR;
    if (parserExprBinary.op.type == TK_OP_PIPE_PIPE) exprBinaryOp = AST_EXPR_BINARY_OR;
    if (parserExprBinary.op.type == TK_OP_PLUS) exprBinaryOp = AST_EXPR_BINARY_ADD;
    if (parserExprBinary.op.type == TK_OP_RSHIFT) exprBinaryOp = AST_EXPR_BINARY_RSHIFT;
    if (parserExprBinary.op.type == TK_OP_SLASH) exprBinaryOp = AST_EXPR_BINARY_DIV;
    if (parserExprBinary.op.type == TK_OP_STAR) exprBinaryOp = AST_EXPR_BINARY_MUL;

    auto exprBinaryLeft = this->_nodeExpr(parserExprBinary.left, varStack);
    auto exprBinaryRight = this->_nodeExpr(parserExprBinary.right, varStack);

    if ((exprBinaryLeft.type->isStr() || exprBinaryRight.type->isStr()) && parserExprBinary.op.type != TK_OP_PLUS) {
      throw Error(this->reader, parserExprBinary.left.start, parserExprBinary.right.end, E1003);
    }

    return this->_wrapNodeExpr(stmtExpr, ASTExprBinary{exprBinaryLeft, exprBinaryOp, exprBinaryRight});
  } else if (std::holds_alternative<ParserExprCall>(*stmtExpr.body)) {
    auto parserExprCall = std::get<ParserExprCall>(*stmtExpr.body);
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
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr.end, E1005);
        }

        for (const auto &calleeFnParam : exprCallCalleeFn.params) {
          if (calleeFnParam.name == exprCallArgName) {
            foundParam = calleeFnParam;
            break;
          }
        }

        if (foundParam == std::nullopt) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr.end, E1002);
        } else if (foundParam->variadic) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr.end, E1006);
        }

        hasNamedArgs = true;
        isArgVariadic = false;
        variadicArgType = std::nullopt;
      } else if (hasNamedArgs) {
        throw Error(this->reader, parserExprCallArg.expr.start, parserExprCallArg.expr.end, E1007);
      } else if (isArgVariadic) {
        foundParam = variadicArgType;
      } else if (exprCallArgIdx >= exprCallCalleeFn.params.size()) {
        throw Error(this->reader, parserExprCallArg.expr.start, parserExprCallArg.expr.end, E1005);
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

      auto exprCallArgExpr = this->_nodeExpr(parserExprCallArg.expr, varStack);

      if (!foundParam->type->match(exprCallArgExpr.type)) {
        throw Error(this->reader, parserExprCallArg.expr.start, parserExprCallArg.expr.end, E1008);
      }

      exprCallArgExpr.type = foundParam->type;
      exprCallArgs.push_back(ASTExprCallArg{exprCallArgId, exprCallArgExpr});

      if (!isArgVariadic) {
        passedArgs.push_back(foundParam->name);
        exprCallArgIdx++;
      }
    }

    for (const auto &calleeFnParam : exprCallCalleeFn.params) {
      if (calleeFnParam.required && std::find(passedArgs.begin(), passedArgs.end(), calleeFnParam.name) == passedArgs.end()) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1009);
      }
    }

    return this->_wrapNodeExpr(stmtExpr, ASTExprCall{exprCallCallee, exprCallCalleeType, exprCallArgs});
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body)) {
    auto parserExprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto exprCondCond = this->_nodeExpr(parserExprCond.cond, varStack);
    auto exprCondBody = this->_nodeExpr(parserExprCond.body, varStack);
    auto exprCondAlt = this->_nodeExpr(parserExprCond.alt, varStack);

    if (!exprCondBody.type->match(exprCondAlt.type) && !exprCondAlt.type->match(exprCondBody.type)) {
      throw Error(this->reader, parserExprCond.body.start, parserExprCond.alt.end, E1004);
    }

    return this->_wrapNodeExpr(stmtExpr, ASTExprCond{exprCondCond, exprCondBody, exprCondAlt});
  } else if (std::holds_alternative<ParserExprLit>(*stmtExpr.body)) {
    auto parserExprLit = std::get<ParserExprLit>(*stmtExpr.body);
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

    return this->_wrapNodeExpr(stmtExpr, ASTExprLit{exprLitType, parserExprLit.body.val});
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr.body)) {
    auto parserExprObj = std::get<ParserExprObj>(*stmtExpr.body);
    auto exprObjVar = this->varMap.get(parserExprObj.id.val);

    if (exprObjVar == nullptr) {
      throw Error(this->reader, parserExprObj.id.start, parserExprObj.id.end, E1010);
    }

    auto exprObjType = exprObjVar->type;
    auto exprObjProps = std::vector<ASTExprObjProp>{};

    varStack.mark(exprObjVar);

    for (const auto &parserExprObjProp : parserExprObj.props) {
      exprObjProps.push_back(ASTExprObjProp{parserExprObjProp.id.val, this->_nodeExpr(parserExprObjProp.init, varStack)});
    }

    return this->_wrapNodeExpr(stmtExpr, ASTExprObj{exprObjType, exprObjProps});
  } else if (std::holds_alternative<ParserExprUnary>(*stmtExpr.body)) {
    auto parserExprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto op = ASTExprUnaryOp{};

    if (parserExprUnary.op.type == TK_OP_EXCL) op = AST_EXPR_UNARY_NOT;
    if (parserExprUnary.op.type == TK_OP_MINUS) op = AST_EXPR_UNARY_MINUS;
    if (parserExprUnary.op.type == TK_OP_MINUS_MINUS) op = AST_EXPR_UNARY_DECREMENT;
    if (parserExprUnary.op.type == TK_OP_PLUS) op = AST_EXPR_UNARY_PLUS;
    if (parserExprUnary.op.type == TK_OP_PLUS_PLUS) op = AST_EXPR_UNARY_INCREMENT;
    if (parserExprUnary.op.type == TK_OP_TILDE) op = AST_EXPR_UNARY_BIT_NOT;

    auto exprUnaryArg = this->_nodeExpr(parserExprUnary.arg, varStack);
    return this->_wrapNodeExpr(stmtExpr, ASTExprUnary{exprUnaryArg, op, parserExprUnary.prefix});
  }

  throw Error("Error: tried to analyze unknown expression");
}

Type *AST::_nodeExprType (const ParserStmtExpr &stmtExpr) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);
    return this->_exprAccessType(exprAccess.body);
  } else if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body)) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    return this->_exprAccessType(exprAssign.left.body);
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body)) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto exprBinaryLeftType = this->_nodeExprType(exprBinary.left);
    auto exprBinaryRightType = this->_nodeExprType(exprBinary.right);

    if (exprBinaryLeftType->isStr() || exprBinaryRightType->isStr()) {
      return this->typeMap.get("str");
    } else if (
      exprBinary.op.type == TK_OP_EQ_EQ ||
      exprBinary.op.type == TK_OP_EXCL_EQ ||
      exprBinary.op.type == TK_OP_GT ||
      exprBinary.op.type == TK_OP_GT_EQ ||
      exprBinary.op.type == TK_OP_LT ||
      exprBinary.op.type == TK_OP_LT_EQ ||
      (exprBinaryLeftType->isBool() && exprBinaryRightType->isBool())
    ) {
      return this->typeMap.get("bool");
    } else if (exprBinaryLeftType->isNumber() && exprBinaryRightType->isNumber()) {
      return Type::largest(exprBinaryLeftType, exprBinaryRightType);
    } else {
      return exprBinaryLeftType->isNumber() ? exprBinaryLeftType : exprBinaryRightType;
    }
  } else if (std::holds_alternative<ParserExprCall>(*stmtExpr.body)) {
    auto exprCall = std::get<ParserExprCall>(*stmtExpr.body);
    auto exprCallCalleeType = this->_exprAccessType(exprCall.callee.body);

    return std::get<TypeFn>(exprCallCalleeType->body).returnType;
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body)) {
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto exprCondBodyType = this->_nodeExprType(exprCond.body);
    auto exprCondAltType = this->_nodeExprType(exprCond.alt);

    return exprCondBodyType->isNumber() && exprCondAltType->isNumber()
      ? Type::largest(exprCondBodyType, exprCondAltType)
      : exprCondBodyType;
  } else if (std::holds_alternative<ParserExprLit>(*stmtExpr.body)) {
    auto exprLit = std::get<ParserExprLit>(*stmtExpr.body);

    if (exprLit.body.type == TK_KW_FALSE) return this->typeMap.get("bool");
    if (exprLit.body.type == TK_KW_TRUE) return this->typeMap.get("bool");
    if (exprLit.body.type == TK_LIT_CHAR) return this->typeMap.get("char");
    if (exprLit.body.type == TK_LIT_FLOAT) return this->typeMap.get("float");
    if (exprLit.body.type == TK_LIT_INT_BIN) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_INT_DEC) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_INT_HEX) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_INT_OCT) return this->typeMap.get("int");
    if (exprLit.body.type == TK_LIT_STR) return this->typeMap.get("str");
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr.body)) {
    auto exprObj = std::get<ParserExprObj>(*stmtExpr.body);
    auto exprObjVar = this->varMap.get(exprObj.id.val);

    if (exprObjVar == nullptr) {
      return nullptr;
    }

    return exprObjVar->type;
  } else if (std::holds_alternative<ParserExprUnary>(*stmtExpr.body)) {
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto exprUnaryArgType = this->_nodeExprType(exprUnary.arg);

    if (exprUnary.op.type == TK_OP_EXCL) {
      return this->typeMap.get("bool");
    }

    return exprUnaryArgType;
  }

  throw Error("Error: tried to analyze unknown expression type");
}

ASTNodeIf AST::_nodeIf (const ParserStmtIf &stmtIf, VarStack &varStack) {
  this->varMap.save();
  auto cond = this->_node(stmtIf.cond, varStack);
  auto body = this->_block(stmtIf.body, varStack);
  auto alt = std::optional<std::shared_ptr<ASTNodeIfCond>>{};
  this->varMap.restore();

  if (stmtIf.alt != std::nullopt) {
    if (std::holds_alternative<ParserBlock>(**stmtIf.alt)) {
      this->varMap.save();
      auto nodeIfAltElse = this->_block(std::get<ParserBlock>(**stmtIf.alt), varStack);
      this->varMap.restore();

      alt = std::make_shared<ASTNodeIfCond>(nodeIfAltElse);
    } else if (std::holds_alternative<ParserStmtIf>(**stmtIf.alt)) {
      auto nodeIfAltElif = this->_nodeIf(std::get<ParserStmtIf>(**stmtIf.alt), varStack);
      alt = std::make_shared<ASTNodeIfCond>(nodeIfAltElif);
    }
  }

  return ASTNodeIf{cond, body, alt};
}

Type *AST::_type (const ParserType &type) {
  if (std::holds_alternative<ParserTypeFn>(*type.body)) {
    auto typeFn = std::get<ParserTypeFn>(*type.body);
    auto fnReturnType = this->_type(typeFn.returnType);
    auto fnParams = std::vector<TypeFnParam>{};

    for (const auto &typeFnParam : typeFn.params) {
      auto paramType = this->_type(typeFnParam.type);
      fnParams.push_back(TypeFnParam{"@", paramType, !typeFnParam.variadic, typeFnParam.variadic});
    }

    return this->typeMap.fn(fnParams, fnReturnType);
  }

  auto typeId = std::get<ParserTypeId>(*type.body);

  if (typeId.id.val == "any") return this->typeMap.get("any");
  if (typeId.id.val == "bool") return this->typeMap.get("bool");
  if (typeId.id.val == "byte") return this->typeMap.get("byte");
  if (typeId.id.val == "char") return this->typeMap.get("char");
  if (typeId.id.val == "f32") return this->typeMap.get("f32");
  if (typeId.id.val == "f64") return this->typeMap.get("f64");
  if (typeId.id.val == "float") return this->typeMap.get("float");
  if (typeId.id.val == "i8") return this->typeMap.get("i8");
  if (typeId.id.val == "i16") return this->typeMap.get("i16");
  if (typeId.id.val == "i32") return this->typeMap.get("i32");
  if (typeId.id.val == "i64") return this->typeMap.get("i64");
  if (typeId.id.val == "int") return this->typeMap.get("int");
  if (typeId.id.val == "str") return this->typeMap.get("str");
  if (typeId.id.val == "u8") return this->typeMap.get("u8");
  if (typeId.id.val == "u16") return this->typeMap.get("u16");
  if (typeId.id.val == "u32") return this->typeMap.get("u32");
  if (typeId.id.val == "u64") return this->typeMap.get("u64");
  if (typeId.id.val == "void") return this->typeMap.get("void");

  if (this->varMap.has(typeId.id.val)) {
    auto var = this->varMap.get(typeId.id.val);

    if (var != nullptr) {
      return var->type;
    }
  }

  throw Error(this->reader, typeId.id.start, typeId.id.end, E1010);
}

ASTNode AST::_wrapNode ([[maybe_unused]] const ParserStmt &stmt, const ASTNodeBody &body) {
  return ASTNode{std::make_shared<ASTNodeBody>(body)};
}

ASTNodeExpr AST::_wrapNodeExpr (const ParserStmtExpr &stmtExpr, const ASTExpr &expr) {
  auto nodeExprType = this->_nodeExprType(stmtExpr);
  return ASTNodeExpr{nodeExprType, std::make_shared<ASTExpr>(expr), stmtExpr.parenthesized};
}
