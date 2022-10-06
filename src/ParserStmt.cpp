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
  auto result = std::string();
  auto attrs = std::string();

  attrs += R"( start=")" + this->start.str() + R"(")";
  attrs += R"( end=")" + this->end.str() + R"(")";

  if (std::holds_alternative<ParserStmtBreak>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtBreak" + attrs + " />";
  } else if (std::holds_alternative<ParserStmtContinue>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtContinue" + attrs + " />";
  } else if (std::holds_alternative<ParserStmtEmpty>(*this->body)) {
    result += std::string(indent, ' ') + "<StmtEmpty" + attrs + " />";
  } else if (std::holds_alternative<ParserStmtExpr>(*this->body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(*this->body);
    result += stmtExpr.xml(indent);
  } else if (std::holds_alternative<ParserStmtFnDecl>(*this->body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtFnDecl" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<StmtFnDeclId>" EOL;
    result += stmtFnDecl.id.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtFnDeclId>" EOL;

    if (!stmtFnDecl.params.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtFnDeclParams>" EOL;

      for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
        auto paramAttrs = std::string();

        paramAttrs += stmtFnDeclParam.mut ? " mut" : "";
        paramAttrs += stmtFnDeclParam.variadic ? " variadic" : "";

        result += std::string(indent + 4, ' ') + "<StmtFnDeclParam" + paramAttrs + ">" EOL;
        result += std::string(indent + 6, ' ') + "<StmtFnDeclParamId>" EOL;
        result += stmtFnDeclParam.id.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</StmtFnDeclParamId>" EOL;

        if (stmtFnDeclParam.type != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<StmtFnDeclParamType>" EOL;
          result += stmtFnDeclParam.type->xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</StmtFnDeclParamType>" EOL;
        }

        if (stmtFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<StmtFnDeclParamInit>" EOL;
          result += stmtFnDeclParam.init->xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</StmtFnDeclParamInit>" EOL;
        }

        result += std::string(indent + 4, ' ') + "</StmtFnDeclParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</StmtFnDeclParams>" EOL;
    }

    if (stmtFnDecl.returnType != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<StmtFnDeclReturnType>" EOL;
      result += stmtFnDecl.returnType->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</StmtFnDeclReturnType>" EOL;
    }

    if (!stmtFnDecl.body.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtFnDeclBody>" EOL;
      result += blockToXml(stmtFnDecl.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</StmtFnDeclBody>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtFnDecl>";
  } else if (std::holds_alternative<ParserStmtIf>(*this->body)) {
    auto stmtIf = std::get<ParserStmtIf>(*this->body);

    result += std::string(indent, ' ') + "<StmtIf" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<StmtIfCond>" EOL;
    result += stmtIf.cond.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtIfCond>" EOL;

    if (!stmtIf.body.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtIfBody>" EOL;
      result += blockToXml(stmtIf.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</StmtIfBody>" EOL;
    }

    if (stmtIf.alt != std::nullopt) {
      if (
        (std::holds_alternative<ParserBlock>(*stmtIf.alt) && !std::get<ParserBlock>(*stmtIf.alt).empty()) ||
        std::holds_alternative<ParserStmt>(*stmtIf.alt)
      ) {
        result += std::string(indent + 2, ' ') + "<StmtIfAlt>" EOL;

        if (std::holds_alternative<ParserBlock>(*stmtIf.alt)) {
          auto altElse = std::get<ParserBlock>(*stmtIf.alt);
          result += blockToXml(altElse, indent + 4);
        } else if (std::holds_alternative<ParserStmt>(*stmtIf.alt)) {
          auto altElif = std::get<ParserStmt>(*stmtIf.alt);
          result += altElif.xml(indent + 4) + EOL;
        }

        result += std::string(indent + 2, ' ') + "</StmtIfAlt>" EOL;
      }
    }

    result += std::string(indent, ' ') + "</StmtIf>";
  } else if (std::holds_alternative<ParserStmtLoop>(*this->body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*this->body);
    result += std::string(indent, ' ') + "<StmtLoop" + attrs + ">" EOL;

    if (stmtLoop.init != std::nullopt) {
      auto stmtLoopInit = *stmtLoop.init;

      result += std::string(indent + 2, ' ') + "<StmtLoopInit>" EOL;
      result += stmtLoopInit.xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</StmtLoopInit>" EOL;
    }

    if (stmtLoop.cond != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<StmtLoopCond>" EOL;
      result += stmtLoop.cond->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</StmtLoopCond>" EOL;
    }

    if (stmtLoop.upd != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<StmtLoopUpd>" EOL;
      result += stmtLoop.upd->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</StmtLoopUpd>" EOL;
    }

    if (!stmtLoop.body.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtLoopBody>" EOL;
      result += blockToXml(stmtLoop.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</StmtLoopBody>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtLoop>";
  } else if (std::holds_alternative<ParserStmtMain>(*this->body)) {
    auto stmtMain = std::get<ParserStmtMain>(*this->body);

    result += std::string(indent, ' ') + "<StmtMain" + attrs + ">" EOL;
    result += blockToXml(stmtMain.body, indent + 2);
    result += std::string(indent, ' ') + "</StmtMain>";
  } else if (std::holds_alternative<ParserStmtObjDecl>(*this->body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtObjDecl" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<StmtObjDeclId>" EOL;
    result += stmtObjDecl.id.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtObjDeclId>" EOL;

    if (!stmtObjDecl.fields.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtObjDeclFields>" EOL;

      for (const auto &stmtObjDeclField : stmtObjDecl.fields) {
        auto fieldAttrs = std::string(stmtObjDeclField.mut ? " mut" : "");

        result += std::string(indent + 4, ' ') + "<StmtObjDeclField" + fieldAttrs + ">" EOL;
        result += std::string(indent + 6, ' ') + "<StmtObjDeclFieldId>" EOL;
        result += stmtObjDeclField.id.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</StmtObjDeclFieldId>" EOL;
        result += std::string(indent + 6, ' ') + "<StmtObjDeclFieldType>" EOL;
        result += stmtObjDeclField.type.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</StmtObjDeclFieldType>" EOL;
        result += std::string(indent + 4, ' ') + "</StmtObjDeclField>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</StmtObjDeclFields>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtObjDecl>";
  } else if (std::holds_alternative<ParserStmtReturn>(*this->body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(*this->body);

    if (stmtReturn.body == std::nullopt) {
      result += std::string(indent, ' ') + "<StmtReturn" + attrs + " />";
    } else {
      result += std::string(indent, ' ') + "<StmtReturn" + attrs + ">" EOL;
      result += stmtReturn.body->xml(indent + 2) + EOL;
      result += std::string(indent, ' ') + "</StmtReturn>";
    }
  } else if (std::holds_alternative<ParserStmtVarDecl>(*this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtVarDecl" + attrs + (stmtVarDecl.mut ? " mut" : "") + ">" EOL;
    result += std::string(indent + 2, ' ') + "<StmtVarDeclId>" EOL;
    result += stmtVarDecl.id.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtVarDeclId>" EOL;

    if (stmtVarDecl.type != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<StmtVarDeclType>" EOL;
      result += stmtVarDecl.type->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</StmtVarDeclType>" EOL;
    }

    if (stmtVarDecl.init != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<StmtVarDeclInit>" EOL;
      result += stmtVarDecl.init->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</StmtVarDeclInit>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtVarDecl>";
  }

  return result;
}
