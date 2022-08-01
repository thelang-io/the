/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ASTExpr.hpp"
#include "Error.hpp"
#include "Token.hpp"
#include "config.hpp"

std::string exprAssignOpStr (enum ASTExprAssignOp op) {
  switch (op) {
    case AST_EXPR_ASSIGN_ADD: return "ADD";
    case AST_EXPR_ASSIGN_AND: return "AND";
    case AST_EXPR_ASSIGN_BIT_AND: return "BIT_AND";
    case AST_EXPR_ASSIGN_BIT_OR: return "BIT_OR";
    case AST_EXPR_ASSIGN_BIT_XOR: return "BIT_XOR";
    case AST_EXPR_ASSIGN_DIV: return "DIV";
    case AST_EXPR_ASSIGN_EQ: return "EQ";
    case AST_EXPR_ASSIGN_LSHIFT: return "LSHIFT";
    case AST_EXPR_ASSIGN_MOD: return "MOD";
    case AST_EXPR_ASSIGN_MUL: return "MUL";
    case AST_EXPR_ASSIGN_OR: return "OR";
    case AST_EXPR_ASSIGN_RSHIFT: return "RSHIFT";
    case AST_EXPR_ASSIGN_SUB: return "SUB";
    default: throw Error("tried stringify unknown assign expression operator");
  }
}

std::string exprBinaryOpStr (enum ASTExprBinaryOp op) {
  switch (op) {
    case AST_EXPR_BINARY_ADD: return "ADD";
    case AST_EXPR_BINARY_AND: return "AND";
    case AST_EXPR_BINARY_BIT_AND: return "BIT_AND";
    case AST_EXPR_BINARY_BIT_OR: return "BIT_OR";
    case AST_EXPR_BINARY_BIT_XOR: return "BIT_XOR";
    case AST_EXPR_BINARY_DIV: return "DIV";
    case AST_EXPR_BINARY_EQ: return "EQ";
    case AST_EXPR_BINARY_GE: return "GE";
    case AST_EXPR_BINARY_GT: return "GT";
    case AST_EXPR_BINARY_LSHIFT: return "LSHIFT";
    case AST_EXPR_BINARY_LE: return "LE";
    case AST_EXPR_BINARY_LT: return "LT";
    case AST_EXPR_BINARY_MOD: return "MOD";
    case AST_EXPR_BINARY_MUL: return "MUL";
    case AST_EXPR_BINARY_NE: return "NE";
    case AST_EXPR_BINARY_OR: return "OR";
    case AST_EXPR_BINARY_RSHIFT: return "RSHIFT";
    case AST_EXPR_BINARY_SUB: return "SUB";
    default: throw Error("tried stringify unknown binary expression operator");
  }
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
    default: throw Error("tried stringify unknown literal expression type");
  }
}

std::string exprUnaryOpStr (enum ASTExprUnaryOp op) {
  switch (op) {
    case AST_EXPR_UNARY_BIT_NOT: return "BIT_NOT";
    case AST_EXPR_UNARY_DECREMENT: return "DECREMENT";
    case AST_EXPR_UNARY_INCREMENT: return "INCREMENT";
    case AST_EXPR_UNARY_MINUS: return "MINUS";
    case AST_EXPR_UNARY_NOT: return "NOT";
    case AST_EXPR_UNARY_PLUS: return "PLUS";
    default: throw Error("tried stringify unknown unary expression operator");
  }
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

    return '"' + leftLitBody.substr(1, leftLitBody.size() - 2) + rightLitBody.substr(1, rightLitBody.size() - 2) + '"';
  }

  return std::get<ASTExprLit>(*this->body).body;
}

std::string ASTNodeExpr::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<NodeExpr" + (this->parenthesized ? " parenthesized" : "") + ">" EOL;

  result += std::string(indent + 2, ' ') + "<NodeExprType>" EOL;
  result += this->type->xml(indent + 4) + EOL;
  result += std::string(indent + 2, ' ') + "</NodeExprType>" EOL;
  result += std::string(indent + 2, ' ') + "<NodeExprBody>" EOL;

  indent += 4;

  if (std::holds_alternative<ASTExprAccess>(*this->body)) {
    auto exprAccess = std::get<ASTExprAccess>(*this->body);
    result += std::string(indent, ' ') + "<ExprAccess" + (exprAccess.prop == std::nullopt ? "" : R"( prop=")" + *exprAccess.prop + R"(")") + ">" EOL;

    if (std::holds_alternative<std::shared_ptr<Var>>(exprAccess.expr)) {
      auto var = std::get<std::shared_ptr<Var>>(exprAccess.expr);
      result += var->xml(indent + 2) + EOL;
    } else if (std::holds_alternative<ASTNodeExpr>(exprAccess.expr)) {
      auto nodeExpr = std::get<ASTNodeExpr>(exprAccess.expr);
      result += nodeExpr.xml(indent + 2) + EOL;
    }

    result += std::string(indent, ' ') + "</ExprAccess>" EOL;
  } else if (std::holds_alternative<ASTExprAssign>(*this->body)) {
    auto exprAssign = std::get<ASTExprAssign>(*this->body);

    result += std::string(indent, ' ') + R"(<ExprAssign op=")" + exprAssignOpStr(exprAssign.op) + R"(">)" EOL;
    result += std::string(indent + 2, ' ') + "<ExprAssignLeft>" EOL;
    result += exprAssign.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprAssignLeft>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprAssignRight>" EOL;
    result += exprAssign.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprAssignRight>" EOL;
    result += std::string(indent, ' ') + "</ExprAssign>" EOL;
  } else if (std::holds_alternative<ASTExprBinary>(*this->body)) {
    auto exprBinary = std::get<ASTExprBinary>(*this->body);

    result += std::string(indent, ' ') + R"(<ExprBinary op=")" + exprBinaryOpStr(exprBinary.op) + R"(">)" EOL;
    result += std::string(indent + 2, ' ') + "<ExprBinaryLeft>" EOL;
    result += exprBinary.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprBinaryLeft>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprBinaryRight>" EOL;
    result += exprBinary.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprBinaryRight>" EOL;
    result += std::string(indent, ' ') + "</ExprBinary>" EOL;
  } else if (std::holds_alternative<ASTExprCall>(*this->body)) {
    auto exprCall = std::get<ASTExprCall>(*this->body);

    result += std::string(indent, ' ') + "<ExprCall>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCallCallee>" EOL;
    result += exprCall.callee.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCallCallee>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCallCalleeType>" EOL;
    result += exprCall.calleeType->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCallCalleeType>" EOL;

    if (!exprCall.args.empty()) {
      result += std::string(indent + 2, ' ') + "<ExprCallArgs>" EOL;

      for (const auto &exprCallArg : exprCall.args) {
        auto argAttrs = exprCallArg.id != std::nullopt ? R"( id=")" + *exprCallArg.id + R"(")" : "";

        result += std::string(indent + 4, ' ') + "<ExprCallArg" + argAttrs + ">" EOL;
        result += exprCallArg.expr.xml(indent + 6)  + EOL;
        result += std::string(indent + 4, ' ') + "</ExprCallArg>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</ExprCallArgs>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprCall>" EOL;
  } else if (std::holds_alternative<ASTExprCond>(*this->body)) {
    auto exprCond = std::get<ASTExprCond>(*this->body);

    result += std::string(indent, ' ') + "<ExprCond>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCondCond>" EOL;
    result += exprCond.cond.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCondCond>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCondBody>" EOL;
    result += exprCond.body.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCondBody>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCondAlt>" EOL;
    result += exprCond.alt.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCondAlt>" EOL;
    result += std::string(indent, ' ') + "</ExprCond>" EOL;
  } else if (std::holds_alternative<ASTExprLit>(*this->body)) {
    auto exprLit = std::get<ASTExprLit>(*this->body);

    result += std::string(indent, ' ') + "<ExprLit";
    result += R"( type=")" + exprLitTypeStr(exprLit.type) + R"(")";
    result += R"( value=")" + Token::escape(exprLit.body, true) + R"(")";
    result += " />" EOL;
  } else if (std::holds_alternative<ASTExprObj>(*this->body)) {
    auto exprObj = std::get<ASTExprObj>(*this->body);

    result += std::string(indent, ' ') + "<ExprObj>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprObjType>" EOL;
    result += exprObj.type->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprObjType>" EOL;

    if (!exprObj.props.empty()) {
      result += std::string(indent + 2, ' ') + "<ExprObjProps>" EOL;

      for (const auto &exprObjProp : exprObj.props) {
        result += std::string(indent + 4, ' ') + R"(<ExprObjProp id=")" + exprObjProp.id + R"(">)" EOL;
        result += exprObjProp.init.xml(indent + 6) + EOL;
        result += std::string(indent + 4, ' ') + "</ExprObjProp>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</ExprObjProps>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprObj>" EOL;
  } else if (std::holds_alternative<ASTExprRef>(*this->body)) {
    auto exprRef = std::get<ASTExprRef>(*this->body);

    result += std::string(indent, ' ') + "<ExprRef>" EOL;
    result += exprRef.expr.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprRef>" EOL;
  } else if (std::holds_alternative<ASTExprUnary>(*this->body)) {
    auto exprUnary = std::get<ASTExprUnary>(*this->body);
    auto attrs = R"( op=")" + exprUnaryOpStr(exprUnary.op) + R"(")" + (exprUnary.prefix ? " prefix" : "");

    result += std::string(indent, ' ') + "<ExprUnary" + attrs + ">" EOL;
    result += exprUnary.arg.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprUnary>" EOL;
  }

  indent -= 4;

  result += std::string(indent + 2, ' ') + "</NodeExprBody>" EOL;
  return result + std::string(indent, ' ') + "</NodeExpr>";
}
