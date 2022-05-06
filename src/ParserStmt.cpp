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
  auto result = std::string(indent, ' ') + "<Stmt";

  result += R"( start=")" + this->start.str();
  result += R"(" end=")" + this->end.str() + R"(">)" "\n";

  indent += 2;

  if (std::holds_alternative<ParserStmtBreak>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtBreak />\n";
  } else if (std::holds_alternative<ParserStmtContinue>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtContinue />\n";
  } else if (std::holds_alternative<ParserStmtExpr>(*this->body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(*this->body);
    result += stmtExpr.xml(indent) + "\n";
  } else if (std::holds_alternative<ParserStmtFnDecl>(*this->body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtFnDecl>\n";
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" "\n";
    result += std::string(indent + 4, ' ') + stmtFnDecl.id.xml() + "\n";
    result += std::string(indent + 2, ' ') + "</slot>\n";

    if (!stmtFnDecl.params.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="params">)" "\n";

      for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
        result += std::string(indent + 4, ' ') + "<StmtFnDeclParam";
        result += R"( variadic=")" + std::string(stmtFnDeclParam.variadic ? "true" : "false") + R"(">)" "\n";

        result += std::string(indent + 6, ' ') + R"(<slot name="id">)" "\n";
        result += std::string(indent + 8, ' ') + stmtFnDeclParam.id.xml() + "\n";
        result += std::string(indent + 6, ' ') + "</slot>\n";

        if (stmtFnDeclParam.type != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="type">)" "\n";
          result += (*stmtFnDeclParam.type).xml(indent + 8) + "\n";
          result += std::string(indent + 6, ' ') + "</slot>\n";
        }

        if (stmtFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="init">)" "\n";
          result += (*stmtFnDeclParam.init).xml(indent + 8) + "\n";
          result += std::string(indent + 6, ' ') + "</slot>\n";
        }

        result += std::string(indent + 4, ' ') + "</StmtFnDeclParam>\n";
      }

      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (stmtFnDecl.returnType != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="returnType">)" "\n";
      result += (*stmtFnDecl.returnType).xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (!stmtFnDecl.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" "\n";
      result += blockToXml(stmtFnDecl.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</StmtFnDecl>\n";
  } else if (std::holds_alternative<ParserStmtIf>(*this->body)) {
    auto stmtIf = std::get<ParserStmtIf>(*this->body);
    result += stmtIf.xml(indent) + "\n";
  } else if (std::holds_alternative<ParserStmtLoop>(*this->body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*this->body);
    result += std::string(indent, ' ') + "<StmtLoop>\n";

    if (stmtLoop.init != std::nullopt) {
      auto stmtLoopInit = *stmtLoop.init;

      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" "\n";
      result += stmtLoopInit.xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (stmtLoop.cond != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" "\n";
      result += (*stmtLoop.cond).xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (stmtLoop.upd != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="upd">)" "\n";
      result += (*stmtLoop.upd).xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (!stmtLoop.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" "\n";
      result += blockToXml(stmtLoop.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</StmtLoop>\n";
  } else if (std::holds_alternative<ParserStmtMain>(*this->body)) {
    auto stmtMain = std::get<ParserStmtMain>(*this->body);

    result += std::string(indent, ' ') + "<StmtMain>\n";
    result += blockToXml(stmtMain.body, indent + 2);
    result += std::string(indent, ' ') + "</StmtMain>\n";
  } else if (std::holds_alternative<ParserStmtObjDecl>(*this->body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtObjDecl>\n";
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" "\n";
    result += std::string(indent + 4, ' ') + stmtObjDecl.id.xml() + "\n";
    result += std::string(indent + 2, ' ') + "</slot>\n";

    if (!stmtObjDecl.fields.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="fields">)" "\n";

      for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
        result += std::string(indent + 4, ' ') + "<StmtObjDeclField>\n";
        result += std::string(indent + 6, ' ') + R"(<slot name="id">)" "\n";
        result += std::string(indent + 8, ' ') + stmtObjDeclField.id.xml() + "\n";
        result += std::string(indent + 6, ' ') + "</slot>\n";
        result += std::string(indent + 6, ' ') + R"(<slot name="type">)" "\n";
        result += stmtObjDeclField.type.xml(indent + 8) + "\n";
        result += std::string(indent + 6, ' ') + "</slot>\n";
        result += std::string(indent + 4, ' ') + "</StmtObjDeclField>\n";
      }

      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</StmtObjDecl>\n";
  } else if (std::holds_alternative<ParserStmtReturn>(*this->body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(*this->body);

    result += std::string(indent, ' ') + "<StmtReturn>\n";
    result += stmtReturn.body != std::nullopt ? ((*stmtReturn.body).xml(indent + 2) + "\n") : "";
    result += std::string(indent, ' ') + "</StmtReturn>\n";
  } else if (std::holds_alternative<ParserStmtVarDecl>(*this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*this->body);

    result += std::string(indent, ' ');
    result += R"(<StmtVarDecl mut=")" + std::string(stmtVarDecl.mut ? "true" : "false") + R"(">)" "\n";
    result += std::string(indent + 2, ' ') + R"(<slot name="id">)" "\n";
    result += std::string(indent + 4, ' ') + stmtVarDecl.id.xml() + "\n";
    result += std::string(indent + 2, ' ') + "</slot>\n";

    if (stmtVarDecl.type != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="type">)" "\n";
      result += (*stmtVarDecl.type).xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (stmtVarDecl.init != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" "\n";
      result += (*stmtVarDecl.init).xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</StmtVarDecl>\n";
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Stmt>";
}

std::string ParserStmtIf::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<StmtIf>\n";
  result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" "\n";
  result += this->cond.xml(indent + 4) + "\n";
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

  return result + std::string(indent, ' ') + "</StmtIf>";
}

std::string ParserType::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<Type";

  result += R"( parenthesized=")" + std::string(this->parenthesized ? "true" : "false");
  result += R"(" start=")" + this->start.str();
  result += R"(" end=")" + this->end.str() + R"(">)" "\n";

  indent += 2;

  if (std::holds_alternative<ParserTypeFn>(*this->body)) {
    auto typeFn = std::get<ParserTypeFn>(*this->body);
    result += std::string(indent, ' ') + "<TypeFn>\n";

    if (!typeFn.params.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="params">)" "\n";

      for (const auto &typeFnParam : typeFn.params) {
        result += std::string(indent + 4, ' ') + "<TypeFnParam";
        result += R"( variadic=")" + std::string(typeFnParam.variadic ? "true" : "false") + R"(">)" "\n";

        result += typeFnParam.type.xml(indent + 6) + "\n";
        result += std::string(indent + 4, ' ') + "</TypeFnParam>\n";
      }

      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent + 2, ' ') + R"(<slot name="returnType">)" "\n";
    result += typeFn.returnType.xml(indent + 4) + "\n";
    result += std::string(indent + 2, ' ') + "</slot>\n";
    result += std::string(indent, ' ') + "</TypeFn>\n";
  } else if (std::holds_alternative<ParserTypeId>(*this->body)) {
    auto typeId = std::get<ParserTypeId>(*this->body);

    result += std::string(indent, ' ') + "<TypeId>\n";
    result += std::string(indent + 2, ' ') + typeId.id.xml() + "\n";
    result += std::string(indent, ' ') + "</TypeId>\n";
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Type>";
}
