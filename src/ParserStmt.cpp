/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ParserStmt.hpp"

std::string ParserStmt::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ');
  result += R"(<Stmt start=")" + this->start.str() + R"(" end=")" + this->end.str() + R"(">)" + "\n";

  if (std::holds_alternative<ParserStmtBreak>(this->body)) {
    result += std::string(indent + 2, ' ') + "<StmtBreak />\n";
  } else if (std::holds_alternative<ParserStmtContinue>(this->body)) {
    result += std::string(indent + 2, ' ') + "<StmtContinue />\n";
  } else if (std::holds_alternative<ParserStmtExpr>(this->body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(this->body);
    result += stmtExpr.xml(indent + 2) + "\n";
  } else if (std::holds_alternative<ParserStmtFnDecl>(this->body)) {
    // todo
  } else if (std::holds_alternative<ParserStmtIf>(this->body)) {
    // todo
  } else if (std::holds_alternative<ParserStmtLoop>(this->body)) {
    // todo
  } else if (std::holds_alternative<ParserStmtMain>(this->body)) {
    // todo
  } else if (std::holds_alternative<ParserStmtObjDecl>(this->body)) {
    // todo
  } else if (std::holds_alternative<ParserStmtReturn>(this->body)) {
    result += std::string(indent + 2, ' ') + "<StmtReturn />\n";
  } else if (std::holds_alternative<ParserStmtVarDecl>(this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(this->body);

    result += std::string(indent + 2, ' ');
    result += R"(<StmtVarDecl mut=")" + std::string(stmtVarDecl.mut ? "true" : "false") + R"(">)" + "\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="id">)" + "\n";
    result += std::string(indent + 6, ' ') + stmtVarDecl.id.xml() + "\n";
    result += std::string(indent + 4, ' ') + R"(</slot>)" + "\n";

    if (stmtVarDecl.type != std::nullopt) {
      result += std::string(indent + 4, ' ') + R"(<slot name="type">)" + "\n";
      result += std::string(indent + 6, ' ') + stmtVarDecl.type->xml() + "\n";
      result += std::string(indent + 4, ' ') + R"(</slot>)" + "\n";
    }

    if (stmtVarDecl.init != std::nullopt) {
      result += std::string(indent + 4, ' ') + R"(<slot name="init">)" + "\n";
      result += stmtVarDecl.init->xml(indent + 6) + "\n";
      result += std::string(indent + 4, ' ') + R"(</slot>)" + "\n";
    }

    result += std::string(indent + 2, ' ') + R"(</StmtVarDecl>)" + "\n";
  }

  result += std::string(indent, ' ') + "</Stmt>";
  return result;
}
