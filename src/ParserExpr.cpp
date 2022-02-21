/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ParserExpr.hpp"

std::string exprAccessBodyToXml (const std::variant<Token, ParserMember> &exprAccessBody, std::size_t indent) {
  if (std::holds_alternative<Token>(exprAccessBody)) {
    auto id = std::get<Token>(exprAccessBody);
    return std::string(indent, ' ') + id.xml() + "\n";
  }

  auto member = std::get<ParserMember>(exprAccessBody);
  auto result = std::string(indent, ' ') + R"(<slot name="obj">)" "\n";

  result += exprAccessBodyToXml(*member.obj, indent + 2);
  result += std::string(indent, ' ') + "</slot>\n";
  result += std::string(indent, ' ') + R"(<slot name="prop">)" "\n";
  result += std::string(indent + 2, ' ') + member.prop.xml() + "\n";
  result += std::string(indent, ' ') + "</slot>\n";

  return result;
}

std::string ParserStmtExpr::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + R"(<StmtExpr parenthesized=")" + std::string(this->parenthesized ? "true" : "false") + R"(">)" "\n";

  if (std::holds_alternative<ParserExprAccess>(*this->body)) {
    auto exprAccess = std::get<ParserExprAccess>(*this->body);

    result += std::string(indent + 2, ' ') + "<ExprAccess>\n";
    result += exprAccessBodyToXml(exprAccess.body, indent + 4);
    result += std::string(indent + 2, ' ') + "</ExprAccess>\n";
  } else if (std::holds_alternative<ParserExprAssign>(*this->body)) {
    auto exprAssign = std::get<ParserExprAssign>(*this->body);

    result += std::string(indent + 2, ' ') + "<ExprAssign>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="left">)" "\n";
    result += exprAccessBodyToXml(exprAssign.left.body, indent + 6);
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="op">)" "\n";
    result += std::string(indent + 6, ' ') + exprAssign.op.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="right">)" "\n";
    result += exprAssign.right.xml(indent + 6) + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 2, ' ') + "</ExprAssign>\n";
  } else if (std::holds_alternative<ParserExprBinary>(*this->body)) {
    auto exprBinary = std::get<ParserExprBinary>(*this->body);

    result += std::string(indent + 2, ' ') + "<ExprBinary>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="left">)" "\n";
    result += exprBinary.left.xml(indent + 6) + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="op">)" "\n";
    result += std::string(indent + 6, ' ') + exprBinary.op.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="right">)" "\n";
    result += exprBinary.right.xml(indent + 6) + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 2, ' ') + "</ExprBinary>\n";
  } else if (std::holds_alternative<ParserExprCall>(*this->body)) {
    auto exprCall = std::get<ParserExprCall>(*this->body);

    result += std::string(indent + 2, ' ') + "<ExprCall>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="callee">)" "\n";
    result += exprAccessBodyToXml(exprCall.callee.body, indent + 6);
    result += std::string(indent + 4, ' ') + "</slot>\n";

    if (!exprCall.args.empty()) {
      result += std::string(indent + 4, ' ') + R"(<slot name="args">)" "\n";

      for (const auto &exprCallArg : exprCall.args) {
        result += std::string(indent + 6, ' ') + "<ExprCallArg>\n";

        if (exprCallArg.id != std::nullopt) {
          result += std::string(indent + 8, ' ') + R"(<slot name="id">)" "\n";
          result += std::string(indent + 10, ' ') +  exprCallArg.id->xml()  + "\n";
          result += std::string(indent + 8, ' ') + "</slot>\n";
        }

        result += std::string(indent + 8, ' ') + R"(<slot name="expr">)" "\n";
        result += exprCallArg.expr.xml(indent + 10)  + "\n";
        result += std::string(indent + 8, ' ') + "</slot>\n";

        result += std::string(indent + 6, ' ') + "</ExprCallArg>\n";
      }

      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    result += std::string(indent + 2, ' ') + "</ExprCall>\n";
  } else if (std::holds_alternative<ParserExprCond>(*this->body)) {
    // todo
  } else if (std::holds_alternative<ParserExprLit>(*this->body)) {
    auto exprLit = std::get<ParserExprLit>(*this->body);

    result += std::string(indent + 2, ' ') + "<ExprLit>\n";
    result += std::string(indent + 4, ' ') + exprLit.body.xml() + "\n";
    result += std::string(indent + 2, ' ') + "</ExprLit>\n";
  } else if (std::holds_alternative<ParserExprObj>(*this->body)) {
    // todo
  } else if (std::holds_alternative<ParserExprUnary>(*this->body)) {
    auto exprUnary = std::get<ParserExprUnary>(*this->body);

    result += std::string(indent + 2, ' ') + R"(<ExprUnary prefix=")" + std::string(exprUnary.prefix ? "true" : "false") + R"(">)" "\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="arg">)" "\n";
    result += exprUnary.arg.xml(indent + 6) + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="op">)" "\n";
    result += std::string(indent + 6, ' ') + exprUnary.op.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";
    result += std::string(indent + 2, ' ') + "</ExprUnary>\n";
  }

  result += std::string(indent, ' ') + "</StmtExpr>";
  return result;
}
