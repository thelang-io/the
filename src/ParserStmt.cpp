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

#include "Parser.hpp"
#include <sstream>
#include "ParserComment.hpp"
#include "config.hpp"
#include "utils.hpp"

std::string blockToXml (const ParserBlock &block, std::size_t indent) {
  auto test = std::string();
  auto result = std::string();

  for (const auto &stmt : block) {
    result += stmt.xml(indent) + EOL;
  }

  return result;
}

std::string normalizeNote (const std::string &note) {
  auto lines = str_lines(note);
  auto result = std::string();

  for (const auto &line : lines) {
    result += "  " + line + EOL;
  }

  return result;
}

bool paramIsSelf (const std::string &prefix, const ParserFnParam &param) {
  if (prefix.empty() || param.type == std::nullopt) {
    return false;
  }

  auto typeCode = param.type->stringify();
  return typeCode == "Self" || typeCode == "ref Self";
}

std::string fnDeclDocParams (
  const std::string &prefix,
  const std::vector<ParserFnParam> &params,
  const std::string &trailing,
  const std::string &separator
) {
  auto code = std::string();
  auto paramIdx = static_cast<std::size_t>(0);

  for (auto i = static_cast<std::size_t>(0); i < params.size(); i++) {
    auto param = params[i];

    if (i == 0 && paramIsSelf(prefix, param)) {
      continue;
    }

    code += paramIdx == 0 ? "" : separator;
    code += trailing;
    code += param.mut ? "mut " : "";
    code += param.id.val;
    code += param.type == std::nullopt ? " := " : (": " + param.type->stringify());
    code += param.variadic ? "..." : "";
    code += param.type != std::nullopt && param.init != std::nullopt ? " = " : "";
    code += param.init == std::nullopt ? "" : param.init->stringify();
    paramIdx++;
  }

  return code;
}

std::string ParserCatchClause::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<CatchClause>" EOL;
  result += std::string(indent + 2, ' ') + "<CatchClauseParam>" EOL;
  result += this->param.xml(indent + 4) + EOL;
  result += std::string(indent + 2, ' ') + "</CatchClauseParam>" EOL;
  result += std::string(indent + 2, ' ') + "<CatchClauseBody>" EOL;
  result += blockToXml(this->body, indent + 4);
  result += std::string(indent + 2, ' ') + "</CatchClauseBody>" EOL;
  result += std::string(indent, ' ') + "</CatchClause>";

  return result;
}

std::string ParserStmt::doc (const std::string &prefix) const {
  auto nextSiblingCanBeDocumented = this->nextSibling != std::nullopt && (
    std::holds_alternative<ParserStmtFnDecl>(*(*this->nextSibling)->body) ||
    std::holds_alternative<ParserStmtObjDecl>(*(*this->nextSibling)->body) ||
    std::holds_alternative<ParserStmtVarDecl>(*(*this->nextSibling)->body)
  );

  auto result = std::string();

  if (std::holds_alternative<ParserStmtComment>(*this->body) && !nextSiblingCanBeDocumented && !prefix.empty()) {
    auto stmtComment = std::get<ParserStmtComment>(*this->body);
    auto comment = parseComment(stmtComment.content);

    if (!comment.sign.empty()) {
      auto fullName = prefix + "." + (comment.sign.starts_with('[') ? "[]" : "()");

      result += "## `" + fullName + "`" EOL;
      result += comment.description.empty() ? "" : comment.description + EOL;
      result += EOL;
      result += "```the" EOL;
      result += prefix + "." + comment.sign + " " + (comment.ret.empty() ? "void" : comment.ret) + EOL;
      result += "```" EOL EOL;
    }
  } else if (std::holds_alternative<ParserStmtFnDecl>(*this->body) && std::get<ParserStmtFnDecl>(*this->body).body == std::nullopt) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*this->body);
    auto fullName = (prefix.empty() ? "" : prefix + ".") + stmtFnDecl.id.val;
    auto comment = ParserComment{};

    if (this->prevSibling != std::nullopt && std::holds_alternative<ParserStmtComment>(*(*this->prevSibling)->body)) {
      auto stmtComment = std::get<ParserStmtComment>(*(*this->prevSibling)->body);
      comment = parseComment(stmtComment.content);
    }

    auto returnTypeCode = stmtFnDecl.returnType->stringify();
    auto actualFnCode = std::string(stmtFnDecl.async ? "async " : "");
    actualFnCode += "fn " + fullName + " (" + fnDeclDocParams(prefix, stmtFnDecl.params, "", ", ") + ") " + returnTypeCode;

    if (actualFnCode.size() > 80) {
      actualFnCode = std::string(stmtFnDecl.async ? "async " : "");
      actualFnCode += "fn " + fullName + " (" EOL + fnDeclDocParams(prefix, stmtFnDecl.params, "  ", "," EOL) + EOL ") " + returnTypeCode;
    }

    result += "## `" + fullName + "()`" EOL;
    result += comment.description.empty() ? "" : comment.description + EOL;
    result += EOL;
    result += "```the" EOL;
    result += actualFnCode + EOL;
    result += "```" EOL EOL;

    if (!stmtFnDecl.params.empty()) {
      auto actualParamsCode = std::string();
      auto paramIdx = static_cast<std::size_t>(0);
      auto lastParamCommentMultiline = false;

      for (auto i = static_cast<std::size_t>(0); i < stmtFnDecl.params.size(); i++) {
        auto param = stmtFnDecl.params[i];
        auto paramComment = comment.params.contains(param.id.val) ? comment.params.find(param.id.val)->second : "";

        if ((i == 0 && paramIsSelf(prefix, param)) || paramComment.empty()) {
          continue;
        }

        actualParamsCode += paramIdx == 0 ? "" : (std::string(lastParamCommentMultiline ? EOL : " \\") + EOL);
        actualParamsCode += "**" + param.id.val + "** - " + paramComment;
        lastParamCommentMultiline = paramComment.find(EOL) != std::string::npos;
        paramIdx++;
      }

      if (!actualParamsCode.empty()) {
        result += "### Parameters" EOL;
        result += actualParamsCode;
        result += EOL EOL;
      }
    }

    if (!comment.image.empty()) {
      result += comment.image + EOL EOL;
    }

    if (!comment.notes.empty()) {
      for (const auto &note : comment.notes) {
        result += "> ### NOTE:" EOL;
        result += normalizeNote(note) + EOL;
      }
    }
  } else if (std::holds_alternative<ParserStmtFnDecl>(*this->body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*this->body);

    if (stmtFnDecl.body != std::nullopt) {
      for (const auto &it : *stmtFnDecl.body) {
        result += it.doc();
      }
    }
  } else if (std::holds_alternative<ParserStmtIf>(*this->body)) {
    auto stmtIf = std::get<ParserStmtIf>(*this->body);

    for (const auto &it : stmtIf.body) {
      result += it.doc();
    }

    if (stmtIf.alt != std::nullopt && std::holds_alternative<ParserBlock>(*stmtIf.alt)) {
      auto stmtIfAlt = std::get<ParserBlock>(*stmtIf.alt);

      for (const auto &it : stmtIfAlt) {
        result += it.doc();
      }
    }
  } else if (std::holds_alternative<ParserStmtLoop>(*this->body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*this->body);

    for (const auto &it : stmtLoop.body) {
      result += it.doc();
    }
  } else if (std::holds_alternative<ParserStmtMain>(*this->body)) {
    auto stmtMain = std::get<ParserStmtMain>(*this->body);

    for (const auto &it : stmtMain.body) {
      result += it.doc();
    }
  } else if (std::holds_alternative<ParserStmtObjDecl>(*this->body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(*this->body);
    auto comment = ParserComment{};

    if (this->prevSibling != std::nullopt && std::holds_alternative<ParserStmtComment>(*(*this->prevSibling)->body)) {
      auto stmtComment = std::get<ParserStmtComment>(*(*this->prevSibling)->body);
      comment = parseComment(stmtComment.content);
    }

    result += "## `" + stmtObjDecl.id.val + "`" EOL;
    result += comment.description.empty() ? "" : comment.description + EOL;
    result += EOL;
    result += "```the" EOL;
    result += "obj " + stmtObjDecl.id.val + " {" + EOL;

    for (const auto &member : stmtObjDecl.members) {
      if (!std::holds_alternative<ParserStmtVarDecl>(*member.body)) {
        continue;
      }

      auto stmtVarDecl = std::get<ParserStmtVarDecl>(*member.body);
      result += "  " + std::string(stmtVarDecl.mut ? "mut " : "");
      result += stmtVarDecl.id.val + ": ";
      result += stmtVarDecl.type->stringify() + EOL;
    }

    result += "}" EOL;
    result += "```" EOL EOL;

    if (!stmtObjDecl.members.empty()) {
      auto actualFieldsCode = std::string();
      auto memberIdx = static_cast<std::size_t>(0);

      for (const auto &member : stmtObjDecl.members) {
        if (!std::holds_alternative<ParserStmtVarDecl>(*member.body)) {
          continue;
        }

        auto stmtVarDecl = std::get<ParserStmtVarDecl>(*member.body);
        auto memberComment = ParserComment{};

        if (member.prevSibling != std::nullopt && std::holds_alternative<ParserStmtComment>(*(*member.prevSibling)->body)) {
          auto stmtComment = std::get<ParserStmtComment>(*(*member.prevSibling)->body);
          memberComment = parseComment(stmtComment.content);
        }

        if (memberComment.description.empty()) {
          continue;
        }

        actualFieldsCode += memberIdx == 0 ? "" : " \\" EOL;
        actualFieldsCode += "**" + stmtVarDecl.id.val + "** - " + memberComment.description;
        memberIdx++;
      }

      if (!actualFieldsCode.empty()) {
        result += "### Fields" EOL;
        result += actualFieldsCode;
        result += EOL EOL;
      }
    }

    if (!comment.image.empty()) {
      result += comment.image + EOL EOL;
    }

    if (!comment.notes.empty()) {
      for (const auto &note : comment.notes) {
        result += "> ### NOTE:" EOL;
        result += normalizeNote(note) + EOL;
      }
    }

    if (!stmtObjDecl.members.empty()) {
      for (const auto &member : stmtObjDecl.members) {
        if (!std::holds_alternative<ParserStmtVarDecl>(*member.body)) {
          result += member.doc(stmtObjDecl.id.val);
        }
      }
    }
  } else if (std::holds_alternative<ParserStmtVarDecl>(*this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*this->body);
    auto comment = ParserComment{};

    if (this->prevSibling != std::nullopt && std::holds_alternative<ParserStmtComment>(*(*this->prevSibling)->body)) {
      auto stmtComment = std::get<ParserStmtComment>(*(*this->prevSibling)->body);
      comment = parseComment(stmtComment.content);
    }

    result += "## `" + stmtVarDecl.id.val + "`" EOL;
    result += comment.description.empty() ? "" : comment.description + EOL;
    result += EOL;
    result += "```the" EOL;

    result += stmtVarDecl.constant ? "const " : "";
    result += stmtVarDecl.mut ? "mut " : "";
    result += stmtVarDecl.id.val;
    result += stmtVarDecl.type == std::nullopt ? " := " : (": " + stmtVarDecl.type->stringify());
    result += stmtVarDecl.type != std::nullopt && stmtVarDecl.init != std::nullopt ? " = " : "";
    result += stmtVarDecl.init == std::nullopt ? "" : stmtVarDecl.init->stringify();
    result += EOL;
    result += "```" EOL EOL;

    if (!comment.image.empty()) {
      result += comment.image + EOL EOL;
    }

    if (!comment.notes.empty()) {
      for (const auto &note : comment.notes) {
        result += "> ### NOTE:" EOL;
        result += normalizeNote(note) + EOL;
      }
    }
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
  } else if (std::holds_alternative<ParserStmtEnumDecl>(*this->body)) {
    auto stmtEnumDecl = std::get<ParserStmtEnumDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtEnumDecl" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<StmtEnumDeclId>" EOL;
    result += stmtEnumDecl.id.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtEnumDeclId>" EOL;

    if (!stmtEnumDecl.members.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtEnumDeclMembers>" EOL;

      for (const auto &stmtEnumDeclMember : stmtEnumDecl.members) {
        result += std::string(indent + 4, ' ') + "<StmtEnumDeclMember>" EOL;
        result += std::string(indent + 6, ' ') + "<StmtEnumDeclMemberId>" EOL;
        result += stmtEnumDeclMember.id.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</StmtEnumDeclMemberId>" EOL;

        if (stmtEnumDeclMember.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<StmtEnumDeclMemberInit>" EOL;
          result += stmtEnumDeclMember.init->xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</StmtEnumDeclMemberInit>" EOL;
        }

        result += std::string(indent + 4, ' ') + "</StmtEnumDeclMember>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</StmtEnumDeclMembers>" EOL;
    }

    result += std::string(indent, ' ') + "</StmtEnumDecl>";
  } else if (std::holds_alternative<ParserStmtExportDecl>(*this->body)) {
    auto stmtExportDecl = std::get<ParserStmtExportDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtExportDecl" + attrs + ">" EOL;
    result += stmtExportDecl.declaration.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</StmtExportDecl>";
  } else if (std::holds_alternative<ParserStmtExpr>(*this->body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(*this->body);
    result += stmtExpr.xml(indent);
  } else if (std::holds_alternative<ParserStmtFnDecl>(*this->body)) {
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*this->body);

    attrs += stmtFnDecl.async ? " async" : "";
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

    if (stmtFnDecl.body != std::nullopt && !stmtFnDecl.body->empty()) {
      result += std::string(indent + 2, ' ') + "<StmtFnDeclBody>" EOL;
      result += blockToXml(*stmtFnDecl.body, indent + 4);
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
  } else if (std::holds_alternative<ParserStmtImportDecl>(*this->body)) {
    auto stmtImportDecl = std::get<ParserStmtImportDecl>(*this->body);
    result += std::string(indent, ' ') + "<StmtImportDecl" + attrs + ">" EOL;

    if (!stmtImportDecl.specifiers.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtImportDeclSpecifiers>" EOL;

      for (const auto &specifier : stmtImportDecl.specifiers) {
        result += std::string(indent + 4, ' ') + "<StmtImportDeclSpecifier>" EOL;

        if (specifier.imported != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<StmtImportDeclSpecifierImported>" EOL;
          result += specifier.imported->xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</StmtImportDeclSpecifierImported>" EOL;
        }

        result += std::string(indent + 6, ' ') + "<StmtImportDeclSpecifierLocal>" EOL;
        result += specifier.local.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</StmtImportDeclSpecifierLocal>" EOL;
        result += std::string(indent + 4, ' ') + "</StmtImportDeclSpecifier>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</StmtImportDeclSpecifiers>" EOL;
    }

    result += std::string(indent + 2, ' ') + "<StmtImportDeclSource>" EOL;
    result += stmtImportDecl.source.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtImportDeclSource>" EOL;
    result += std::string(indent, ' ') + "</StmtImportDecl>";
  } else if (std::holds_alternative<ParserStmtLoop>(*this->body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*this->body);
    result += std::string(indent, ' ') + "<StmtLoop" + attrs + (stmtLoop.parenthesized ? " parenthesized" : "") + ">" EOL;

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

    if (!stmtObjDecl.members.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtObjDeclMembers>" EOL;

      for (const auto &member : stmtObjDecl.members) {
        result += member.xml(indent + 4) + EOL;
      }

      result += std::string(indent + 2, ' ') + "</StmtObjDeclMembers>" EOL;
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
  } else if (std::holds_alternative<ParserStmtThrow>(*this->body)) {
    auto stmtThrow = std::get<ParserStmtThrow>(*this->body);

    result += std::string(indent, ' ') + "<StmtThrow" + attrs + ">" EOL;
    result += stmtThrow.arg.xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</StmtThrow>";
  } else if (std::holds_alternative<ParserStmtTry>(*this->body)) {
    auto stmtTry = std::get<ParserStmtTry>(*this->body);

    result += std::string(indent, ' ') + "<StmtTry" + attrs + ">" EOL;

    if (!stmtTry.body.empty()) {
      result += std::string(indent + 2, ' ') + "<StmtTryBody>" EOL;
      result += blockToXml(stmtTry.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</StmtTryBody>" EOL;
    }

    result += std::string(indent + 2, ' ') + "<StmtTryHandlers>" EOL;

    for (const auto &handler : stmtTry.handlers) {
      result += handler.xml(indent + 4) + EOL;
    }

    result += std::string(indent + 2, ' ') + "</StmtTryHandlers>" EOL;
    result += std::string(indent, ' ') + "</StmtTry>";
  } else if (std::holds_alternative<ParserStmtTypeDecl>(*this->body)) {
    auto stmtTypeDecl = std::get<ParserStmtTypeDecl>(*this->body);

    result += std::string(indent, ' ') + "<StmtTypeDecl" + attrs + ">" EOL;
    result += std::string(indent + 2, ' ') + "<StmtTypeDeclId>" EOL;
    result += stmtTypeDecl.id.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtTypeDeclId>" EOL;
    result += std::string(indent + 2, ' ') + "<StmtTypeDeclType>" EOL;
    result += stmtTypeDecl.type.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</StmtTypeDeclType>" EOL;

    result += std::string(indent, ' ') + "</StmtTypeDecl>";
  } else if (std::holds_alternative<ParserStmtVarDecl>(*this->body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*this->body);

    attrs += stmtVarDecl.constant ? " const" : "";
    attrs += stmtVarDecl.mut ? " mut" : "";

    result += std::string(indent, ' ') + "<StmtVarDecl" + attrs + ">" EOL;
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
