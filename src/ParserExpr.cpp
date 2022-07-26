/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ParserExpr.hpp"
#include "config.hpp"

std::string memberObjXml (const std::shared_ptr<ParserMemberObj> &exprAccessBody, std::size_t indent) {
  if (std::holds_alternative<Token>(*exprAccessBody)) {
    auto id = std::get<Token>(*exprAccessBody);
    return std::string(indent, ' ') + id.xml() + EOL;
  }

  auto member = std::get<ParserMember>(*exprAccessBody);
  auto result = std::string(indent, ' ') + R"(<slot name="obj">)" EOL;

  result += memberObjXml(member.obj, indent + 2);
  result += std::string(indent, ' ') + "</slot>" EOL;
  result += std::string(indent, ' ') + R"(<slot name="prop">)" EOL;
  result += std::string(indent + 2, ' ') + member.prop.xml() + EOL;
  result += std::string(indent, ' ') + "</slot>" EOL;

  return result;
}

std::string ParserExprAccess::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<ExprAccess>" EOL;
  result += memberObjXml(this->body, indent + 2);
  result += std::string(indent, ' ') + "</ExprAccess>";

  return result;
}

std::string ParserStmtExpr::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<StmtExpr" + (this->parenthesized ? " parenthesized" : "");

  result += R"( start=")" + this->start.str() + R"(")";
  result += R"( end=")" + this->end.str() + R"(">)" EOL;

  indent += 2;

  if (std::holds_alternative<ParserExprAccess>(*this->body)) {
    auto exprAccess = std::get<ParserExprAccess>(*this->body);
    result += exprAccess.xml(indent) + EOL;
  } else if (std::holds_alternative<ParserExprAssign>(*this->body)) {
    auto exprAssign = std::get<ParserExprAssign>(*this->body);

    result += std::string(indent, ' ') + "<ExprAssign>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="left">)" EOL;
    result += exprAssign.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="op">)" EOL;
    result += std::string(indent + 4, ' ') + exprAssign.op.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="right">)" EOL;
    result += exprAssign.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</ExprAssign>" EOL;
  } else if (std::holds_alternative<ParserExprBinary>(*this->body)) {
    auto exprBinary = std::get<ParserExprBinary>(*this->body);

    result += std::string(indent, ' ') + "<ExprBinary>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="left">)" EOL;
    result += exprBinary.left.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="op">)" EOL;
    result += std::string(indent + 4, ' ') + exprBinary.op.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="right">)" EOL;
    result += exprBinary.right.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</ExprBinary>" EOL;
  } else if (std::holds_alternative<ParserExprCall>(*this->body)) {
    auto exprCall = std::get<ParserExprCall>(*this->body);

    result += std::string(indent, ' ') + "<ExprCall>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="callee">)" EOL;
    result += exprCall.callee.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!exprCall.args.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="args">)" EOL;

      for (const auto &exprCallArg : exprCall.args) {
        result += std::string(indent + 4, ' ') + "<ExprCallArg>" EOL;

        if (exprCallArg.id != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="id">)" EOL;
          result += std::string(indent + 8, ' ') +  exprCallArg.id->xml()  + EOL;
          result += std::string(indent + 6, ' ') + "</slot>" EOL;
        }

        result += std::string(indent + 6, ' ') + R"(<slot name="expr">)" EOL;
        result += exprCallArg.expr.xml(indent + 8)  + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 4, ' ') + "</ExprCallArg>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprCall>" EOL;
  } else if (std::holds_alternative<ParserExprCond>(*this->body)) {
    auto exprCond = std::get<ParserExprCond>(*this->body);

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
  } else if (std::holds_alternative<ParserExprLit>(*this->body)) {
    auto exprLit = std::get<ParserExprLit>(*this->body);

    result += std::string(indent, ' ') + "<ExprLit>" EOL;
    result += std::string(indent + 2, ' ') + exprLit.body.xml() + EOL;
    result += std::string(indent, ' ') + "</ExprLit>" EOL;
  } else if (std::holds_alternative<ParserExprObj>(*this->body)) {
    auto exprObj = std::get<ParserExprObj>(*this->body);

    result += std::string(indent, ' ') + "<ExprObj>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" EOL;
    result += std::string(indent + 4, ' ') + exprObj.id.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!exprObj.props.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="props">)" EOL;

      for (const auto &exprObjProp : exprObj.props) {
        result += std::string(indent + 4, ' ') + "<ExprObjProp>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="id">)" EOL;
        result += std::string(indent + 8, ' ') + exprObjProp.id.xml() + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="init">)" EOL;
        result += exprObjProp.init.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 4, ' ') + "</ExprObjProp>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</ExprObj>" EOL;
  } else if (std::holds_alternative<ParserExprUnary>(*this->body)) {
    auto exprUnary = std::get<ParserExprUnary>(*this->body);

    result += std::string(indent, ' ') + "<ExprUnary" + (exprUnary.prefix ? " prefix" : "") + ">" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="arg">)" EOL;
    result += exprUnary.arg.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="op">)" EOL;
    result += std::string(indent + 4, ' ') + exprUnary.op.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</ExprUnary>" EOL;
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</StmtExpr>";
}
