/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ParserStmt.hpp"

std::string blockToXml (const ParserBlock &block, std::size_t indent) {
  auto result = std::string();

  for (const auto &stmt : block) {
    result += stmt.xml(indent) + "\n";
  }

  return result;
}

std::string ParserStmt::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ');
  result += R"(<Stmt start=")" + this->start.str() + R"(" end=")" + this->end.str() + R"(">)" "\n";

  if (std::holds_alternative<ParserStmtBreak>(this->body)) {
    result += std::string(indent + 2, ' ') + "<StmtBreak />\n";
  } else if (std::holds_alternative<ParserStmtContinue>(this->body)) {
    result += std::string(indent + 2, ' ') + "<StmtContinue />\n";
  } else if (std::holds_alternative<ParserStmtExpr>(this->body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(this->body);
    result += stmtExpr.xml(indent + 2) + "\n";
  } else if (std::holds_alternative<ParserStmtFnDecl>(this->body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(this->body);

    result += std::string(indent + 2, ' ') + "<StmtFnDecl>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="id">)" "\n";
    result += std::string(indent + 6, ' ') + stmtFnDecl.id.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";

    if (!stmtFnDecl.params.empty()) {
      result += std::string(indent + 4, ' ') + R"(<slot name="params">)" "\n";

      for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
        result += std::string(indent + 6, ' ') + "<StmtFnDeclParam>\n";
        result += std::string(indent + 8, ' ') + R"(<slot name="id">)" "\n";
        result += std::string(indent + 10, ' ') + stmtFnDeclParam.id.xml() + "\n";
        result += std::string(indent + 8, ' ') + "</slot>\n";

        if (stmtFnDeclParam.type != std::nullopt) {
          result += std::string(indent + 8, ' ') + R"(<slot name="type">)" "\n";
          result += std::string(indent + 10, ' ') + stmtFnDeclParam.type->xml() + "\n";
          result += std::string(indent + 8, ' ') + "</slot>\n";
        }

        if (stmtFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 8, ' ') + R"(<slot name="init">)" "\n";
          result += stmtFnDeclParam.init->xml(indent + 10) + "\n";
          result += std::string(indent + 8, ' ') + "</slot>\n";
        }

        result += std::string(indent + 6, ' ') + "</StmtFnDeclParam>\n";
      }

      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    result += std::string(indent + 4, ' ') + R"(<slot name="returnType">)" "\n";
    result += std::string(indent + 6, ' ') + stmtFnDecl.returnType.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";

    if (!stmtFnDecl.body.empty()) {
      result += std::string(indent + 4, ' ') + R"(<slot name="body">)" "\n";
      result += blockToXml(stmtFnDecl.body, indent + 6);
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    result += std::string(indent + 2, ' ') + "</StmtFnDecl>\n";
  } else if (std::holds_alternative<ParserStmtIf>(this->body)) {
    auto stmtIf = std::get<ParserStmtIf>(this->body);
    result += stmtIf.xml(indent + 2) + "\n";
  } else if (std::holds_alternative<ParserStmtLoop>(this->body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(this->body);

    result += std::string(indent + 2, ' ') + "<StmtLoop>\n";

    if (stmtLoop.init != std::nullopt) {
      auto stmtLoopInit = **stmtLoop.init;

      result += std::string(indent + 4, ' ') + R"(<slot name="init">)" "\n";
      result += stmtLoopInit.xml(indent + 6) + "\n";
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    if (stmtLoop.cond != std::nullopt) {
      result += std::string(indent + 4, ' ') + R"(<slot name="cond">)" "\n";
      result += stmtLoop.cond->xml(indent + 6) + "\n";
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    if (stmtLoop.upd != std::nullopt) {
      result += std::string(indent + 4, ' ') + R"(<slot name="upd">)" "\n";
      result += stmtLoop.upd->xml(indent + 6) + "\n";
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    if (!stmtLoop.body.empty()) {
      result += std::string(indent + 4, ' ') + R"(<slot name="body">)" "\n";
      result += blockToXml(stmtLoop.body, indent + 6);
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    result += std::string(indent + 2, ' ') + "</StmtLoop>\n";
  } else if (std::holds_alternative<ParserStmtMain>(this->body)) {
    auto stmtMain = std::get<ParserStmtMain>(this->body);

    result += std::string(indent + 2, ' ') + "<StmtMain>\n";
    result += blockToXml(stmtMain.body, indent + 4);
    result += std::string(indent + 2, ' ') + "</StmtMain>\n";
  } else if (std::holds_alternative<ParserStmtObjDecl>(this->body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(this->body);

    result += std::string(indent + 2, ' ') + "<StmtObjDecl>\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="id">)" "\n";
    result += std::string(indent + 6, ' ') + stmtObjDecl.id.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";

    if (!stmtObjDecl.fields.empty()) {
      result += std::string(indent + 4, ' ') + R"(<slot name="fields">)" "\n";

      for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
        result += std::string(indent + 6, ' ') + "<StmtObjDeclField>\n";
        result += std::string(indent + 8, ' ') + R"(<slot name="id">)" "\n";
        result += std::string(indent + 10, ' ') + stmtObjDeclField.id.xml() + "\n";
        result += std::string(indent + 8, ' ') + "</slot>\n";
        result += std::string(indent + 8, ' ') + R"(<slot name="type">)" "\n";
        result += std::string(indent + 10, ' ') + stmtObjDeclField.type.xml() + "\n";
        result += std::string(indent + 8, ' ') + "</slot>\n";
        result += std::string(indent + 6, ' ') + "</StmtObjDeclField>\n";
      }

      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    result += std::string(indent + 2, ' ') + "</StmtObjDecl>\n";
  } else if (std::holds_alternative<ParserStmtReturn>(this->body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(this->body);

    result += std::string(indent + 2, ' ') + "<StmtReturn>\n";
    result += stmtReturn.body != std::nullopt ? (stmtReturn.body->xml(indent + 4) + "\n") : "";
    result += std::string(indent + 2, ' ') + "</StmtReturn>\n";
  } else if (std::holds_alternative<ParserStmtVarDecl>(this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(this->body);

    result += std::string(indent + 2, ' ');
    result += R"(<StmtVarDecl mut=")" + std::string(stmtVarDecl.mut ? "true" : "false") + R"(">)" "\n";
    result += std::string(indent + 4, ' ') + R"(<slot name="id">)" "\n";
    result += std::string(indent + 6, ' ') + stmtVarDecl.id.xml() + "\n";
    result += std::string(indent + 4, ' ') + "</slot>\n";

    if (stmtVarDecl.type != std::nullopt) {
      result += std::string(indent + 4, ' ') + R"(<slot name="type">)" "\n";
      result += std::string(indent + 6, ' ') + stmtVarDecl.type->xml() + "\n";
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    if (stmtVarDecl.init != std::nullopt) {
      result += std::string(indent + 4, ' ') + R"(<slot name="init">)" "\n";
      result += stmtVarDecl.init->xml(indent + 6) + "\n";
      result += std::string(indent + 4, ' ') + "</slot>\n";
    }

    result += std::string(indent + 2, ' ') + "</StmtVarDecl>\n";
  }

  result += std::string(indent, ' ') + "</Stmt>";
  return result;
}

std::string ParserStmtIf::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<StmtIf>\n";
  result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" "\n";
  result += this->cond->xml(indent + 4) + "\n";
  result += std::string(indent + 2, ' ') + "</slot>\n";

  if (!this->body.empty()) {
    result += std::string(indent + 2, ' ') + R"(<slot name="body">)" "\n";
    result += blockToXml(this->body, indent + 4);
    result += std::string(indent + 2, ' ') + "</slot>\n";
  }

  if (this->alt != std::nullopt) {
    auto altBody = **this->alt;
    result += std::string(indent + 2, ' ') + R"(<slot name="alt">)" "\n";

    if (std::holds_alternative<ParserBlock>(altBody)) {
      auto altElse = std::get<ParserBlock>(altBody);
      result += blockToXml(altElse, indent + 4);
    } else if (std::holds_alternative<ParserStmtIf>(altBody)) {
      auto altElif = std::get<ParserStmtIf>(altBody);
      result += altElif.xml(indent + 4) + "\n";
    }

    result += std::string(indent + 2, ' ') + "</slot>\n";
  }

  result += std::string(indent, ' ') + "</StmtIf>";
  return result;
}
