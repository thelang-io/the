/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ASTExpr.hpp"
#include "Error.hpp"
#include "config.hpp"

std::string exprAssignOpStr (enum ASTExprAssignOp op) {
  switch (op) {
    case AST_EXPR_ASSIGN_ADD: return "ADD";
    case AST_EXPR_ASSIGN_BITWISE_AND: return "BITWISE_AND";
    case AST_EXPR_ASSIGN_BITWISE_OR: return "BITWISE_OR";
    case AST_EXPR_ASSIGN_BITWISE_XOR: return "BITWISE_XOR";
    case AST_EXPR_ASSIGN_COALESCE: return "COALESCE";
    case AST_EXPR_ASSIGN_DIVIDE: return "DIVIDE";
    case AST_EXPR_ASSIGN_EQUAL: return "EQUAL";
    case AST_EXPR_ASSIGN_LEFT_SHIFT: return "LEFT_SHIFT";
    case AST_EXPR_ASSIGN_LOGICAL_AND: return "LOGICAL_AND";
    case AST_EXPR_ASSIGN_LOGICAL_OR: return "LOGICAL_OR";
    case AST_EXPR_ASSIGN_MULTIPLY: return "MULTIPLY";
    case AST_EXPR_ASSIGN_POWER: return "POWER";
    case AST_EXPR_ASSIGN_REMAINDER: return "REMAINDER";
    case AST_EXPR_ASSIGN_RIGHT_SHIFT: return "RIGHT_SHIFT";
    case AST_EXPR_ASSIGN_SUBTRACT: return "SUBTRACT";
  }

  throw Error("Error: tried stringify unknown assign expression operator");
}

std::string exprBinaryOpStr (enum ASTExprBinaryOp op) {
  switch (op) {
    case AST_EXPR_BINARY_ADD: return "ADD";
    case AST_EXPR_BINARY_BITWISE_AND: return "BITWISE_AND";
    case AST_EXPR_BINARY_BITWISE_OR: return "BITWISE_OR";
    case AST_EXPR_BINARY_BITWISE_XOR: return "BITWISE_XOR";
    case AST_EXPR_BINARY_COALESCE: return "COALESCE";
    case AST_EXPR_BINARY_DIVIDE: return "DIVIDE";
    case AST_EXPR_BINARY_EQUAL: return "EQUAL";
    case AST_EXPR_BINARY_GREATER_EQUAL: return "GREATER_EQUAL";
    case AST_EXPR_BINARY_GREATER_THAN: return "GREATER_THAN";
    case AST_EXPR_BINARY_LEFT_SHIFT: return "LEFT_SHIFT";
    case AST_EXPR_BINARY_LESS_EQUAL: return "LESS_EQUAL";
    case AST_EXPR_BINARY_LESS_THAN: return "LESS_THAN";
    case AST_EXPR_BINARY_LOGICAL_AND: return "LOGICAL_AND";
    case AST_EXPR_BINARY_LOGICAL_OR: return "LOGICAL_OR";
    case AST_EXPR_BINARY_MULTIPLY: return "MULTIPLY";
    case AST_EXPR_BINARY_NOT_EQUAL: return "NOT_EQUAL";
    case AST_EXPR_BINARY_POWER: return "POWER";
    case AST_EXPR_BINARY_REMAINDER: return "REMAINDER";
    case AST_EXPR_BINARY_RIGHT_SHIFT: return "RIGHT_SHIFT";
    case AST_EXPR_BINARY_SUBTRACT: return "SUBTRACT";
  }

  throw Error("Error: tried stringify unknown binary expression operator");
}

std::string exprLitTypeStr (enum ASTExprLitType type) {
  switch (type) {
    case AST_EXPR_LIT_BOOL: return "BOOL";
    case AST_EXPR_LIT_CHAR: return "CHAR";
    case AST_EXPR_LIT_FLOAT: return "FLOAT";
    case AST_EXPR_LIT_INT_BIN: return "INT_BIN";
    case AST_EXPR_LIT_INT_DEC: return "INT_DEC";
    case AST_EXPR_LIT_INT_HEX: return "INT_HEX";
    case AST_EXPR_LIT_INT_OCT: return "INT_OCT";
    case AST_EXPR_LIT_STR: return "STR";
  }

  throw Error("Error: tried stringify unknown literal expression type");
}

std::string exprUnaryOpStr (enum ASTExprUnaryOp op) {
  switch (op) {
    case AST_EXPR_UNARY_BITWISE_NOT: return "BITWISE_NOT";
    case AST_EXPR_UNARY_DECREMENT: return "DECREMENT";
    case AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT: return "DOUBLE_LOGICAL_NOT";
    case AST_EXPR_UNARY_INCREMENT: return "INCREMENT";
    case AST_EXPR_UNARY_LOGICAL_NOT: return "LOGICAL_NOT";
    case AST_EXPR_UNARY_NEGATION: return "NEGATION";
    case AST_EXPR_UNARY_PLUS: return "PLUS";
  }

  throw Error("Error: tried stringify unknown unary expression operator");
}

std::string memberObjXml (const std::shared_ptr<ASTMemberObj> &exprAccessBody, std::size_t indent) {
  if (std::holds_alternative<std::shared_ptr<Var>>(*exprAccessBody)) {
    auto id = std::get<std::shared_ptr<Var>>(*exprAccessBody);
    return id->xml(indent) + EOL;
  }

  auto member = std::get<ASTMember>(*exprAccessBody);
  auto result = std::string(indent, ' ') + R"(<slot name="obj">)" EOL;

  result += memberObjXml(member.obj, indent + 2);
  result += std::string(indent, ' ') + "</slot>" EOL;
  result += std::string(indent, ' ') + R"(<slot name="prop">)" EOL;
  result += std::string(indent + 2, ' ') + member.prop + EOL;
  result += std::string(indent, ' ') + "</slot>" EOL;

  return result;
}

std::string ASTExprAccess::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<ExprAccess>" EOL;
  result += memberObjXml(this->body, indent + 2);
  result += std::string(indent, ' ') + "</ExprAccess>";

  return result;
}

bool ASTNodeExpr::isLit () const {
  if (std::holds_alternative<ASTExprBinary>(*this->body)) {
    auto exprBinary = std::get<ASTExprBinary>(*this->body);
    return exprBinary.left.isLit() && exprBinary.right.isLit();
  }

  return std::holds_alternative<ASTExprLit>(*this->body);
}

std::string ASTNodeExpr::litBody () const {
  if (std::holds_alternative<ASTExprBinary>(*this->body)) {
    auto exprBinary = std::get<ASTExprBinary>(*this->body);
    auto leftLitBody = exprBinary.left.litBody();
    auto rightLitBody = exprBinary.right.litBody();

    return R"(")" + leftLitBody.substr(1, leftLitBody.size() - 2) + rightLitBody.substr(1, rightLitBody.size() - 2) + R"(")";
  }

  return std::get<ASTExprLit>(*this->body).body;
}

std::string ASTNodeExpr::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + R"(<NodeExpr parenthesized=")" + std::string(this->parenthesized ? "true" : "false") + R"(">)" EOL;

  result += std::string(indent + 2, ' ') + R"(<slot name="type">)" EOL;
  result += this->type->xml(indent + 4) + EOL;
  result += std::string(indent + 2, ' ') + "</slot>" EOL;
  result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;

  indent += 4;

  if (std::holds_alternative<ASTExprAccess>(*this->body)) {
    auto exprAccess = std::get<ASTExprAccess>(*this->body);
    result += exprAccess.xml(indent) + EOL;
  } else if (std::holds_alternative<ASTExprAssign>(*this->body)) {
    auto exprAssign = std::get<ASTExprAssign>(*this->body);

    result += std::string(indent, ' ') + R"(<ExprAssign op=")" + exprAssignOpStr(exprAssign.op) + R"(">)" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="left">)" EOL;
    result += exprAssign.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="right">)" EOL;
    result += exprAssign.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</ExprAssign>" EOL;
  } else if (std::holds_alternative<ASTExprBinary>(*this->body)) {
    auto exprBinary = std::get<ASTExprBinary>(*this->body);

    result += std::string(indent, ' ') + R"(<ExprBinary op=")" + exprBinaryOpStr(exprBinary.op) + R"(">)" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="left">)" EOL;
    result += exprBinary.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="right">)" EOL;
    result += exprBinary.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</ExprBinary>" EOL;
  } else if (std::holds_alternative<ASTExprCall>(*this->body)) {
    auto exprCall = std::get<ASTExprCall>(*this->body);

    result += std::string(indent, ' ') + "<ExprCall>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="callee">)" EOL;
    result += exprCall.callee.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    result += std::string(indent + 2, ' ') + R"(<slot name="calleeType">)" EOL;
    result += exprCall.calleeType->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!exprCall.args.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="args">)" EOL;

      for (const auto &exprCallArg : exprCall.args) {
        result += std::string(indent + 4, ' ') + "<ExprCallArg";
        result += (exprCallArg.id != std::nullopt ? R"( id=")" + *exprCallArg.id + R"(")" : "") + ">" EOL;
        result += exprCallArg.expr.xml(indent + 6)  + EOL;
        result += std::string(indent + 4, ' ') + "</ExprCallArg>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprCall>" EOL;
  } else if (std::holds_alternative<ASTExprCond>(*this->body)) {
    auto exprCond = std::get<ASTExprCond>(*this->body);

    result += std::string(indent, ' ') + "<ExprCond>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" EOL;
    result += exprCond.cond.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;
    result += exprCond.body.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="alt">)" EOL;
    result += exprCond.alt.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</ExprCond>" EOL;
  } else if (std::holds_alternative<ASTExprLit>(*this->body)) {
    auto exprLit = std::get<ASTExprLit>(*this->body);

    result += std::string(indent, ' ') + R"(<ExprLit type=")" + exprLitTypeStr(exprLit.type) + R"(">)" EOL;
    result += std::string(indent + 2, ' ') + exprLit.body + EOL;
    result += std::string(indent, ' ') + "</ExprLit>" EOL;
  } else if (std::holds_alternative<ASTExprObj>(*this->body)) {
    auto exprObj = std::get<ASTExprObj>(*this->body);

    result += std::string(indent, ' ') + "<ExprObj>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="type">)" EOL;
    result += exprObj.type->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!exprObj.props.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="props">)" EOL;

      for (const auto &exprObjProp : exprObj.props) {
        result += std::string(indent + 4, ' ') + "<ExprObjProp>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="id">)" EOL;
        result += std::string(indent + 8, ' ') + exprObjProp.id + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="init">)" EOL;
        result += exprObjProp.init.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 4, ' ') + "</ExprObjProp>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprObj>" EOL;
  } else if (std::holds_alternative<ASTExprUnary>(*this->body)) {
    auto exprUnary = std::get<ASTExprUnary>(*this->body);

    result += std::string(indent, ' ') + R"(<ExprUnary op=")" + exprUnaryOpStr(exprUnary.op);
    result += R"(" prefix=")" + std::string(exprUnary.prefix ? "true" : "false") + R"(">)" EOL;
    result += exprUnary.arg.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprUnary>" EOL;
  }

  indent -= 4;
  result += std::string(indent + 2, ' ') + "</slot>" EOL;

  return result + std::string(indent, ' ') + "</NodeExpr>";
}
