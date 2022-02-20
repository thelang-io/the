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
  auto result = std::string(indent, ' ') + R"(<slot name="obj">)" + "\n";

  result += exprAccessBodyToXml(*member.obj, indent + 2);
  result += std::string(indent, ' ') + "</slot>\n";
  result += std::string(indent, ' ') + R"(<slot name="prop">)" + "\n";
  result += std::string(indent + 2, ' ') + member.prop.xml() + "\n";
  result += std::string(indent, ' ') + "</slot>\n";

  return result;
}

std::string ParserStmtExpr::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + R"(<StmtExpr parenthesized=")" + std::string(this->parenthesized ? "true" : "false") + R"(">)" + "\n";

  if (std::holds_alternative<ParserExprAccess>(*this->body)) {
    auto exprAccess = std::get<ParserExprAccess>(*this->body);

    result += std::string(indent + 2, ' ') + "<ExprAccess>\n";
    result += exprAccessBodyToXml(exprAccess.body, indent + 4);
    result += std::string(indent + 2, ' ') + "</ExprAccess>\n";
  } else if (std::holds_alternative<ParserExprAssign>(*this->body)) {
    // todo
  } else if (std::holds_alternative<ParserExprBinary>(*this->body)) {
    // todo
  } else if (std::holds_alternative<ParserExprCall>(*this->body)) {
    // todo
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

    result += std::string(indent + 2, ' ') + R"(<ExprUnary prefix=")" + std::string(exprUnary.prefix ? "true" : "false") + R"(">)" + "\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="arg">)" + "\n";
    result += exprUnary.arg.xml(indent + 6) + "\n";
    result += std::string(indent + 4, ' ') + R"(</slot>)" + "\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="op">)" + "\n";
    result += std::string(indent + 6, ' ') + exprUnary.op.xml() + "\n";
    result += std::string(indent + 4, ' ') + R"(</slot>)" + "\n";
    result += std::string(indent + 2, ' ') + "</ExprUnary>\n";
  }

  result += std::string(indent, ' ') + R"(</StmtExpr>)";
  return result;
}
