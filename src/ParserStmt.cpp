/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ParserStmt.hpp"
#include "config.hpp"

std::string blockToXml (const ParserBlock &block, std::size_t indent) {
  auto result = std::string();

  for (const auto &stmt : block) {
    result += stmt.xml(indent) + EOL;
  }

  return result;
}

std::string ParserStmt::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<Stmt";

  result += R"( start=")" + this->start.str();
  result += R"(" end=")" + this->end.str() + R"(">)" EOL;

  indent += 2;

  if (std::holds_alternative<ParserStmtBreak>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtBreak />" EOL;
  } else if (std::holds_alternative<ParserStmtContinue>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtContinue />" EOL;
  } else if (std::holds_alternative<ParserStmtExpr>(*this->body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(*this->body);
    result += stmtExpr.xml(indent) + EOL;
  } else if (std::holds_alternative<ParserStmtFnDecl>(*this->body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtFnDecl>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" EOL;
    result += std::string(indent + 4, ' ') + stmtFnDecl.id.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!stmtFnDecl.params.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="params">)" EOL;

      for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
        result += std::string(indent + 4, ' ') + "<StmtFnDeclParam";
        result += R"( variadic=")" + std::string(stmtFnDeclParam.variadic ? "true" : "false") + R"(">)" EOL;

        result += std::string(indent + 6, ' ') + R"(<slot name="id">)" EOL;
        result += std::string(indent + 8, ' ') + stmtFnDeclParam.id.xml() + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;

        if (stmtFnDeclParam.type != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="type">)" EOL;
          result += (*stmtFnDeclParam.type).xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</slot>" EOL;
        }

        if (stmtFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="init">)" EOL;
          result += (*stmtFnDeclParam.init).xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</slot>" EOL;
        }

        result += std::string(indent + 4, ' ') + "</StmtFnDeclParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (stmtFnDecl.returnType != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="returnType">)" EOL;
      result += (*stmtFnDecl.returnType).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (!stmtFnDecl.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;
      result += blockToXml(stmtFnDecl.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtFnDecl>" EOL;
  } else if (std::holds_alternative<ParserStmtIf>(*this->body)) {
    auto stmtIf = std::get<ParserStmtIf>(*this->body);
    result += stmtIf.xml(indent) + EOL;
  } else if (std::holds_alternative<ParserStmtLoop>(*this->body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*this->body);
    result += std::string(indent, ' ') + "<StmtLoop>" EOL;

    if (stmtLoop.init != std::nullopt) {
      auto stmtLoopInit = *stmtLoop.init;

      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" EOL;
      result += stmtLoopInit.xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (stmtLoop.cond != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" EOL;
      result += (*stmtLoop.cond).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (stmtLoop.upd != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="upd">)" EOL;
      result += (*stmtLoop.upd).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (!stmtLoop.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;
      result += blockToXml(stmtLoop.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtLoop>" EOL;
  } else if (std::holds_alternative<ParserStmtMain>(*this->body)) {
    auto stmtMain = std::get<ParserStmtMain>(*this->body);

    result += std::string(indent, ' ') + "<StmtMain>" EOL;
    result += blockToXml(stmtMain.body, indent + 2);
    result += std::string(indent, ' ') + "</StmtMain>" EOL;
  } else if (std::holds_alternative<ParserStmtObjDecl>(*this->body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtObjDecl>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" EOL;
    result += std::string(indent + 4, ' ') + stmtObjDecl.id.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!stmtObjDecl.fields.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="fields">)" EOL;

      for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
        result += std::string(indent + 4, ' ') + "<StmtObjDeclField>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="id">)" EOL;
        result += std::string(indent + 8, ' ') + stmtObjDeclField.id.xml() + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="type">)" EOL;
        result += stmtObjDeclField.type.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;
        result += std::string(indent + 4, ' ') + "</StmtObjDeclField>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtObjDecl>" EOL;
  } else if (std::holds_alternative<ParserStmtReturn>(*this->body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(*this->body);

    result += std::string(indent, ' ') + "<StmtReturn>" EOL;
    result += stmtReturn.body != std::nullopt ? ((*stmtReturn.body).xml(indent + 2) + EOL) : "";
    result += std::string(indent, ' ') + "</StmtReturn>" EOL;
  } else if (std::holds_alternative<ParserStmtVarDecl>(*this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*this->body);

    result += std::string(indent, ' ');
    result += R"(<StmtVarDecl mut=")" + std::string(stmtVarDecl.mut ? "true" : "false") + R"(">)" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" EOL;
    result += std::string(indent + 4, ' ') + stmtVarDecl.id.xml() + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (stmtVarDecl.type != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="type">)" EOL;
      result += (*stmtVarDecl.type).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (stmtVarDecl.init != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" EOL;
      result += (*stmtVarDecl.init).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtVarDecl>" EOL;
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Stmt>";
}

std::string ParserStmtIf::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<StmtIf>" EOL;
  result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" EOL;
  result += this->cond.xml(indent + 4) + EOL;
  result += std::string(indent + 2, ' ') + "</slot>" EOL;

  if (!this->body.empty()) {
    result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;
    result += blockToXml(this->body, indent + 4);
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
  }

  if (this->alt != std::nullopt) {
    auto altBody = **this->alt;

    if (
      (std::holds_alternative<ParserBlock>(altBody) && !std::get<ParserBlock>(altBody).empty()) ||
      std::holds_alternative<ParserStmtIf>(altBody)
    ) {
      result += std::string(indent + 2, ' ') + R"(<slot name="alt">)" EOL;

      if (std::holds_alternative<ParserBlock>(altBody)) {
        auto altElse = std::get<ParserBlock>(altBody);
        result += blockToXml(altElse, indent + 4);
      } else if (std::holds_alternative<ParserStmtIf>(altBody)) {
        auto altElif = std::get<ParserStmtIf>(altBody);
        result += altElif.xml(indent + 4) + EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }
  }

  return result + std::string(indent, ' ') + "</StmtIf>";
}

std::string ParserType::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<Type";

  result += R"( parenthesized=")" + std::string(this->parenthesized ? "true" : "false");
  result += R"(" start=")" + this->start.str();
  result += R"(" end=")" + this->end.str() + R"(">)" EOL;

  indent += 2;

  if (std::holds_alternative<ParserTypeFn>(*this->body)) {
    auto typeFn = std::get<ParserTypeFn>(*this->body);
    result += std::string(indent, ' ') + "<TypeFn>" EOL;

    if (!typeFn.params.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="params">)" EOL;

      for (const auto &typeFnParam : typeFn.params) {
        result += std::string(indent + 4, ' ') + "<TypeFnParam";
        result += R"( variadic=")" + std::string(typeFnParam.variadic ? "true" : "false") + R"(">)" EOL;

        result += typeFnParam.type.xml(indent + 6) + EOL;
        result += std::string(indent + 4, ' ') + "</TypeFnParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent + 2, ' ') + R"(<slot name="returnType">)" EOL;
    result += typeFn.returnType.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;
    result += std::string(indent, ' ') + "</TypeFn>" EOL;
  } else if (std::holds_alternative<ParserTypeId>(*this->body)) {
    auto typeId = std::get<ParserTypeId>(*this->body);

    result += std::string(indent, ' ') + "<TypeId>" EOL;
    result += std::string(indent + 2, ' ') + typeId.id.xml() + EOL;
    result += std::string(indent, ' ') + "</TypeId>" EOL;
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Type>";
}
