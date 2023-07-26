/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ASTExpr.hpp"
#include "Error.hpp"
#include "Token.hpp"
#include "config.hpp"

std::string exprAssignOpStr (ASTExprAssignOp op) {
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
  }

  throw Error("tried stringify unknown assign expression operator");
}

std::string exprBinaryOpStr (ASTExprBinaryOp op) {
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
  }

  throw Error("tried stringify unknown binary expression operator");
}

std::string exprLitTypeStr (ASTExprLitType type) {
  switch (type) {
    case AST_EXPR_LIT_BOOL: return "BOOL";
    case AST_EXPR_LIT_CHAR: return "CHAR";
    case AST_EXPR_LIT_FLOAT: return "FLOAT";
    case AST_EXPR_LIT_INT_BIN: return "INT_BIN";
    case AST_EXPR_LIT_INT_DEC: return "INT_DEC";
    case AST_EXPR_LIT_INT_HEX: return "INT_HEX";
    case AST_EXPR_LIT_INT_OCT: return "INT_OCT";
    case AST_EXPR_LIT_NIL: return "NIL";
    case AST_EXPR_LIT_STR: return "STR";
  }

  throw Error("tried stringify unknown literal expression type");
}

std::string exprUnaryOpStr (ASTExprUnaryOp op) {
  switch (op) {
    case AST_EXPR_UNARY_BIT_NOT: return "BIT_NOT";
    case AST_EXPR_UNARY_DECREMENT: return "DECREMENT";
    case AST_EXPR_UNARY_INCREMENT: return "INCREMENT";
    case AST_EXPR_UNARY_MINUS: return "MINUS";
    case AST_EXPR_UNARY_NOT: return "NOT";
    case AST_EXPR_UNARY_PLUS: return "PLUS";
  }

  throw Error("tried stringify unknown unary expression operator");
}

std::string ASTObjProp::xml (const std::string &exprName, std::size_t indent) const {
  auto tagName = "Expr" + exprName + "Prop";
  auto result = std::string(indent, ' ') + "<" + tagName + R"( name=")" + this->name + R"(">)" EOL;

  result += this->init.xml(indent + 2) + EOL;
  result += std::string(indent, ' ') + "</" + tagName + ">";
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
    auto attrs = std::string(exprAccess.prop == std::nullopt ? "" : R"( prop=")" + *exprAccess.prop + R"(")");

    if (exprAccess.expr == std::nullopt) {
      result += std::string(indent, ' ') + "<ExprAccess" + attrs + " />" EOL;
    } else {
      result += std::string(indent, ' ') + "<ExprAccess" + attrs + ">" EOL;

      if (std::holds_alternative<std::shared_ptr<Var>>(*exprAccess.expr)) {
        auto var = std::get<std::shared_ptr<Var>>(*exprAccess.expr);
        result += var->xml(indent + 2) + EOL;
      } else if (std::holds_alternative<ASTNodeExpr>(*exprAccess.expr)) {
        auto nodeExpr = std::get<ASTNodeExpr>(*exprAccess.expr);

        if (exprAccess.elem != std::nullopt) {
          result += std::string(indent + 2, ' ') + "<ExprAccessExpr>" EOL;
          result += nodeExpr.xml(indent + 4) + EOL;
          result += std::string(indent + 2, ' ') + "</ExprAccessExpr>" EOL;
          result += std::string(indent + 2, ' ') + "<ExprAccessElem>" EOL;
          result += exprAccess.elem->xml(indent + 4) + EOL;
          result += std::string(indent + 2, ' ') + "</ExprAccessElem>" EOL;
        } else {
          result += nodeExpr.xml(indent + 2) + EOL;
        }
      }

      result += std::string(indent, ' ') + "</ExprAccess>" EOL;
    }
  } else if (std::holds_alternative<ASTExprArray>(*this->body)) {
    auto exprArray = std::get<ASTExprArray>(*this->body);

    if (exprArray.elements.empty()) {
      result += std::string(indent, ' ') + "<ExprArray />" EOL;
    } else {
      result += std::string(indent, ' ') + "<ExprArray>" EOL;

      for (const auto &element : exprArray.elements) {
        result += element.xml(indent + 2) + EOL;
      }

      result += std::string(indent, ' ') + "</ExprArray>" EOL;
    }
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
  } else if (std::holds_alternative<ASTExprAwait>(*this->body)) {
    auto exprAwait = std::get<ASTExprAwait>(*this->body);

    result += std::string(indent, ' ') + "<ExprAwait>" EOL;
    result += exprAwait.arg.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprAwait>" EOL;
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
  } else if (std::holds_alternative<ASTExprIs>(*this->body)) {
    auto exprIs = std::get<ASTExprIs>(*this->body);

    result += std::string(indent, ' ') + "<ExprIs>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprIsExpr>" EOL;
    result += exprIs.expr.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprIsExpr>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprIsType>" EOL;
    result += exprIs.type->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprIsType>" EOL;
    result += std::string(indent, ' ') + "</ExprIs>" EOL;
  } else if (std::holds_alternative<ASTExprLit>(*this->body)) {
    auto exprLit = std::get<ASTExprLit>(*this->body);

    result += std::string(indent, ' ') + "<ExprLit";
    result += R"( type=")" + exprLitTypeStr(exprLit.type) + R"(")";
    result += R"( value=")" + Token::escape(exprLit.body, true) + R"(")";
    result += " />" EOL;
  } else if (std::holds_alternative<ASTExprMap>(*this->body)) {
    auto exprMap = std::get<ASTExprMap>(*this->body);

    if (exprMap.props.empty()) {
      result += std::string(indent, ' ') + "<ExprMap />" EOL;
    } else {
      result += std::string(indent, ' ') + "<ExprMap>" EOL;

      for (const auto &exprMapProp : exprMap.props) {
        result += exprMapProp.xml("Map", indent + 2) + EOL;
      }

      result += std::string(indent, ' ') + "</ExprMap>" EOL;
    }
  } else if (std::holds_alternative<ASTExprObj>(*this->body)) {
    auto exprObj = std::get<ASTExprObj>(*this->body);

    if (exprObj.props.empty()) {
      result += std::string(indent, ' ') + "<ExprObj />" EOL;
    } else {
      result += std::string(indent, ' ') + "<ExprObj>" EOL;

      for (const auto &exprObjProp : exprObj.props) {
        result += exprObjProp.xml("Obj", indent + 2) + EOL;
      }

      result += std::string(indent, ' ') + "</ExprObj>" EOL;
    }
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
