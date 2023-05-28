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

#include "ParserExpr.hpp"
#include "config.hpp"

std::string ParserStmtExpr::stringify () const {
  auto code = std::string();

  if (std::holds_alternative<ParserExprAccess>(*this->body)) {
    auto exprBody = std::get<ParserExprAccess>(*this->body);

    if (exprBody.expr != std::nullopt && std::holds_alternative<Token>(*exprBody.expr)) {
      code += std::get<Token>(*exprBody.expr).val;
    } else if (exprBody.expr != std::nullopt && std::holds_alternative<ParserStmtExpr>(*exprBody.expr)) {
      code += std::get<ParserStmtExpr>(*exprBody.expr).stringify();
    }

    if (exprBody.elem != std::nullopt) {
      code += "[" + exprBody.elem->stringify() + "]";
    } else if (exprBody.prop != std::nullopt) {
      code += "." + exprBody.prop->val;
    }
  } else if (std::holds_alternative<ParserExprArray>(*this->body)) {
    auto exprBody = std::get<ParserExprArray>(*this->body);
    code += "[";

    for (auto i = static_cast<std::size_t>(0); i < exprBody.elements.size(); i++) {
      code += i == 0 ? "" : ", ";
      code += exprBody.elements[i].stringify();
    }

    code += "]";
  } else if (std::holds_alternative<ParserExprAssign>(*this->body)) {
    auto exprBody = std::get<ParserExprAssign>(*this->body);
    code += exprBody.left.stringify() + " " + exprBody.op.val + " " + exprBody.right.stringify();
  } else if (std::holds_alternative<ParserExprAwait>(*this->body)) {
    // todo test
    auto exprBody = std::get<ParserExprAwait>(*this->body);
    code += "await " + exprBody.arg.stringify();
  } else if (std::holds_alternative<ParserExprBinary>(*this->body)) {
    auto exprBody = std::get<ParserExprBinary>(*this->body);
    code += exprBody.left.stringify() + " " + exprBody.op.val + " " + exprBody.right.stringify();
  } else if (std::holds_alternative<ParserExprCall>(*this->body)) {
    auto exprBody = std::get<ParserExprCall>(*this->body);
    code += exprBody.callee.stringify() + "(";

    for (auto i = static_cast<std::size_t>(0); i < exprBody.args.size(); i++) {
      auto arg = exprBody.args[i];

      code += i == 0 ? "" : ", ";
      code += arg.id == std::nullopt ? "" : arg.id->val + ": ";
      code += arg.expr.stringify();
    }

    code += ")";
  } else if (std::holds_alternative<ParserExprCond>(*this->body)) {
    auto exprBody = std::get<ParserExprCond>(*this->body);
    code += exprBody.cond.stringify() + " ? " + exprBody.body.stringify() + " : " + exprBody.alt.stringify();
  } else if (std::holds_alternative<ParserExprIs>(*this->body)) {
    auto exprBody = std::get<ParserExprIs>(*this->body);
    code += exprBody.expr.stringify() + " is " + exprBody.type.stringify();
  } else if (std::holds_alternative<ParserExprLit>(*this->body)) {
    auto exprBody = std::get<ParserExprLit>(*this->body);
    code += exprBody.body.val;
  } else if (std::holds_alternative<ParserExprMap>(*this->body)) {
    auto exprBody = std::get<ParserExprMap>(*this->body);
    code += "{";

    for (auto i = static_cast<std::size_t>(0); i < exprBody.props.size(); i++) {
      code += i == 0 ? "" : ", ";
      code += exprBody.props[i].name.stringify() + ": " + exprBody.props[i].init.stringify();
    }

    code += "}";
  } else if (std::holds_alternative<ParserExprObj>(*this->body)) {
    auto exprBody = std::get<ParserExprObj>(*this->body);
    code += exprBody.id.val + "{";

    for (auto i = static_cast<std::size_t>(0); i < exprBody.props.size(); i++) {
      auto prop = exprBody.props[i];
      code += i == 0 ? "" : ", ";
      code += prop.id.val + ": " + prop.init.stringify();
    }

    code += "}";
  } else if (std::holds_alternative<ParserExprRef>(*this->body)) {
    auto exprBody = std::get<ParserExprRef>(*this->body);
    code += "ref " + exprBody.expr.stringify();
  } else if (std::holds_alternative<ParserExprUnary>(*this->body)) {
    auto exprBody = std::get<ParserExprUnary>(*this->body);

    if (exprBody.prefix) {
      code += exprBody.op.val;
    }

    code += exprBody.arg.stringify();

    if (!exprBody.prefix) {
      code += exprBody.op.val;
    }
  }

  return this->parenthesized ? "(" + code + ")" : code;
}

std::string ParserStmtExpr::xml (std::size_t indent) const {
  auto result = std::string();
  auto attrs = std::string(this->parenthesized ? " parenthesized" : "");

  attrs += R"( start=")" + this->start.str() + R"(")";
  attrs += R"( end=")" + this->end.str() + R"(")";

  if (std::holds_alternative<ParserExprAccess>(*this->body)) {
    auto exprAccess = std::get<ParserExprAccess>(*this->body);
    result += std::string(indent, ' ') + "<ExprAccess" + attrs + ">" EOL;

    if (exprAccess.expr != std::nullopt && std::holds_alternative<Token>(*exprAccess.expr)) {
      auto tok = std::get<Token>(*exprAccess.expr);
      result += tok.xml(indent + 2) + EOL;
    } else if (exprAccess.expr != std::nullopt && std::holds_alternative<ParserStmtExpr>(*exprAccess.expr)) {
      auto stmtExpr = std::get<ParserStmtExpr>(*exprAccess.expr);

      result += std::string(indent + 2, ' ') + "<ExprAccessExpr>" EOL;
      result += stmtExpr.xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</ExprAccessExpr>" EOL;
    }

    if (exprAccess.elem != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<ExprAccessElem>" EOL;
      result += exprAccess.elem->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</ExprAccessElem>" EOL;
    } else if (exprAccess.prop != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<ExprAccessProp>" EOL;
      result += exprAccess.prop->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</ExprAccessProp>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprAccess>";
  } else if (std::holds_alternative<ParserExprArray>(*this->body)) {
    auto exprArray = std::get<ParserExprArray>(*this->body);

    if (exprArray.elements.empty()) {
      result += std::string(indent, ' ') + "<ExprArray" + attrs + " />";
    } else {
      result += std::string(indent, ' ') + "<ExprArray" + attrs + ">" EOL;

      for (const auto &element : exprArray.elements) {
        result += element.xml(indent + 2) + EOL;
      }

      result += std::string(indent, ' ') + "</ExprArray>";
    }
  } else if (std::holds_alternative<ParserExprAssign>(*this->body)) {
    auto exprAssign = std::get<ParserExprAssign>(*this->body);

    result += std::string(indent, ' ') + "<ExprAssign" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprAssignLeft>" EOL;
    result += exprAssign.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprAssignLeft>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprAssignOp>" EOL;
    result += exprAssign.op.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprAssignOp>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprAssignRight>" EOL;
    result += exprAssign.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprAssignRight>" EOL;
    result += std::string(indent, ' ') + "</ExprAssign>";
  } else if (std::holds_alternative<ParserExprAwait>(*this->body)) {
    // todo test
    auto exprAwait = std::get<ParserExprAwait>(*this->body);

    result += std::string(indent, ' ') + "<ExprAwait" + attrs + ">" EOL;
    result += exprAwait.arg.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprAwait>";
  } else if (std::holds_alternative<ParserExprBinary>(*this->body)) {
    auto exprBinary = std::get<ParserExprBinary>(*this->body);

    result += std::string(indent, ' ') + "<ExprBinary" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprBinaryLeft>" EOL;
    result += exprBinary.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprBinaryLeft>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprBinaryOp>" EOL;
    result += exprBinary.op.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprBinaryOp>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprBinaryRight>" EOL;
    result += exprBinary.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprBinaryRight>" EOL;
    result += std::string(indent, ' ') + "</ExprBinary>";
  } else if (std::holds_alternative<ParserExprCall>(*this->body)) {
    auto exprCall = std::get<ParserExprCall>(*this->body);

    result += std::string(indent, ' ') + "<ExprCall" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCallCallee>" EOL;
    result += exprCall.callee.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCallCallee>" EOL;

    if (!exprCall.args.empty()) {
      result += std::string(indent + 2, ' ') + "<ExprCallArgs>" EOL;

      for (const auto &exprCallArg : exprCall.args) {
        result += std::string(indent + 4, ' ') + "<ExprCallArg>" EOL;

        if (exprCallArg.id != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<ExprCallArgId>" EOL;
          result += exprCallArg.id->xml(indent + 8)  + EOL;
          result += std::string(indent + 6, ' ') + "</ExprCallArgId>" EOL;
        }

        result += std::string(indent + 6, ' ') + "<ExprCallArgExpr>" EOL;
        result += exprCallArg.expr.xml(indent + 8)  + EOL;
        result += std::string(indent + 6, ' ') + "</ExprCallArgExpr>" EOL;
        result += std::string(indent + 4, ' ') + "</ExprCallArg>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</ExprCallArgs>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprCall>";
  } else if (std::holds_alternative<ParserExprCond>(*this->body)) {
    auto exprCond = std::get<ParserExprCond>(*this->body);

    result += std::string(indent, ' ') + "<ExprCond" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCondCond>" EOL;
    result += exprCond.cond.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCondCond>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCondBody>" EOL;
    result += exprCond.body.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCondBody>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprCondAlt>" EOL;
    result += exprCond.alt.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprCondAlt>" EOL;
    result += std::string(indent, ' ') + "</ExprCond>";
  } else if (std::holds_alternative<ParserExprIs>(*this->body)) {
    auto exprIs = std::get<ParserExprIs>(*this->body);

    result += std::string(indent, ' ') + "<ExprIs" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprIsExpr>" EOL;
    result += exprIs.expr.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprIsExpr>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprIsType>" EOL;
    result += exprIs.type.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprIsType>" EOL;
    result += std::string(indent, ' ') + "</ExprIs>";
  } else if (std::holds_alternative<ParserExprLit>(*this->body)) {
    auto exprLit = std::get<ParserExprLit>(*this->body);

    result += std::string(indent, ' ') + "<ExprLit" + attrs + ">" EOL;
    result += exprLit.body.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprLit>";
  } else if (std::holds_alternative<ParserExprMap>(*this->body)) {
    auto exprMap = std::get<ParserExprMap>(*this->body);

    if (exprMap.props.empty()) {
      result += std::string(indent, ' ') + "<ExprMap" + attrs + " />" EOL;
    } else {
      result += std::string(indent, ' ') + "<ExprMap" + attrs + ">" EOL;

      for (const auto &exprMapProp : exprMap.props) {
        result += std::string(indent + 2, ' ') + "<ExprMapProp>" EOL;
        result += std::string(indent + 4, ' ') + "<ExprMapPropName>" EOL;
        result += exprMapProp.name.xml(indent + 6) + EOL;
        result += std::string(indent + 4, ' ') + "</ExprMapPropName>" EOL;
        result += std::string(indent + 4, ' ') + "<ExprMapPropInit>" EOL;
        result += exprMapProp.init.xml(indent + 6) + EOL;
        result += std::string(indent + 4, ' ') + "</ExprMapPropInit>" EOL;
        result += std::string(indent + 2, ' ') + "</ExprMapProp>" EOL;
      }

      result += std::string(indent, ' ') + "</ExprMap>";
    }
  } else if (std::holds_alternative<ParserExprObj>(*this->body)) {
    auto exprObj = std::get<ParserExprObj>(*this->body);

    result += std::string(indent, ' ') + "<ExprObj" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprObjId>" EOL;
    result += exprObj.id.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprObjId>" EOL;

    if (!exprObj.props.empty()) {
      result += std::string(indent + 2, ' ') + "<ExprObjProps>" EOL;

      for (const auto &exprObjProp : exprObj.props) {
        result += std::string(indent + 4, ' ') + "<ExprObjProp>" EOL;
        result += std::string(indent + 6, ' ') + "<ExprObjPropId>" EOL;
        result += exprObjProp.id.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</ExprObjPropId>" EOL;
        result += std::string(indent + 6, ' ') + "<ExprObjPropInit>" EOL;
        result += exprObjProp.init.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</ExprObjPropInit>" EOL;
        result += std::string(indent + 4, ' ') + "</ExprObjProp>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</ExprObjProps>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprObj>";
  } else if (std::holds_alternative<ParserExprRef>(*this->body)) {
    auto exprRef = std::get<ParserExprRef>(*this->body);

    result += std::string(indent, ' ') + "<ExprRef" + attrs + ">" EOL;
    result += exprRef.expr.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</ExprRef>";
  } else if (std::holds_alternative<ParserExprUnary>(*this->body)) {
    auto exprUnary = std::get<ParserExprUnary>(*this->body);

    result += std::string(indent, ' ') + "<ExprUnary" + attrs + (exprUnary.prefix ? " prefix" : "") + ">" EOL;
    result += std::string(indent + 2, ' ') + "<ExprUnaryArg>" EOL;
    result += exprUnary.arg.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprUnaryArg>" EOL;
    result += std::string(indent + 2, ' ') + "<ExprUnaryOp>" EOL;
    result += exprUnary.op.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</ExprUnaryOp>" EOL;
    result += std::string(indent, ' ') + "</ExprUnary>";
  }

  return result;
}
