/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ASTNode.hpp"
#include "config.hpp"

std::string blockToXml (const ASTBlock &block, std::size_t indent) {
  auto result = std::string();

  for (const auto &node : block) {
    result += node.xml(indent) + EOL;
  }

  return result;
}

std::string ASTNode::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<Node>" EOL;
  indent += 2;

  if (std::holds_alternative<ASTNodeBreak>(*this->body)) {
    result += std::string(indent, ' ') + "<NodeBreak />" EOL;
  } else if (std::holds_alternative<ASTNodeContinue>(*this->body)) {
    result += std::string(indent, ' ') + "<NodeContinue />" EOL;
  } else if (std::holds_alternative<ASTNodeExpr>(*this->body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*this->body);
    result += nodeExpr.xml(indent) + EOL;
  } else if (std::holds_alternative<ASTNodeFnDecl>(*this->body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(*this->body);

    result += std::string(indent, ' ') + "<NodeFnDecl>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="var">)" EOL;
    result += nodeFnDecl.var->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (!nodeFnDecl.stack.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="stack">)" EOL;

      for (const auto &nodeFnDeclStackItem : nodeFnDecl.stack) {
        result += std::string(indent + 4, ' ') + "<NodeFnDeclStackItem>" EOL;
        result += nodeFnDeclStackItem->xml(indent + 6) + EOL;
        result += std::string(indent + 4, ' ') + "</NodeFnDeclStackItem>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (!nodeFnDecl.params.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="params">)" EOL;

      for (const auto &nodeFnDeclParam : nodeFnDecl.params) {
        result += std::string(indent + 4, ' ') + "<NodeFnDeclParam>" EOL;
        result += std::string(indent + 6, ' ') + R"(<slot name="var">)" EOL;
        result += nodeFnDeclParam.var->xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</slot>" EOL;

        if (nodeFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="init">)" EOL;
          result += (*nodeFnDeclParam.init).xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</slot>" EOL;
        }

        result += std::string(indent + 4, ' ') + "</NodeFnDeclParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (!nodeFnDecl.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;
      result += blockToXml(nodeFnDecl.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</NodeFnDecl>" EOL;
  } else if (std::holds_alternative<ASTNodeIf>(*this->body)) {
    auto nodeIf = std::get<ASTNodeIf>(*this->body);
    result += nodeIf.xml(indent) + EOL;
  } else if (std::holds_alternative<ASTNodeLoop>(*this->body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(*this->body);
    result += std::string(indent, ' ') + "<NodeLoop>" EOL;

    if (nodeLoop.init != std::nullopt) {
      auto nodeLoopInit = *nodeLoop.init;

      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" EOL;
      result += nodeLoopInit.xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (nodeLoop.cond != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" EOL;
      result += (*nodeLoop.cond).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (nodeLoop.upd != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="upd">)" EOL;
      result += (*nodeLoop.upd).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    if (!nodeLoop.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" EOL;
      result += blockToXml(nodeLoop.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</NodeLoop>" EOL;
  } else if (std::holds_alternative<ASTNodeMain>(*this->body)) {
    auto nodeMain = std::get<ASTNodeMain>(*this->body);

    result += std::string(indent, ' ') + "<NodeMain>" EOL;
    result += blockToXml(nodeMain.body, indent + 2);
    result += std::string(indent, ' ') + "</NodeMain>" EOL;
  } else if (std::holds_alternative<ASTNodeObjDecl>(*this->body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(*this->body);

    result += std::string(indent, ' ') + "<NodeObjDecl>" EOL;
    result += nodeObjDecl.var->xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</NodeObjDecl>" EOL;
  } else if (std::holds_alternative<ASTNodeReturn>(*this->body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(*this->body);

    if (nodeReturn.body == std::nullopt) {
      result += std::string(indent, ' ') + "<NodeReturn />" EOL;
    } else {
      result += std::string(indent, ' ') + "<NodeReturn>" EOL;
      result += (*nodeReturn.body).xml(indent + 2) + EOL;
      result += std::string(indent, ' ') + "</NodeReturn>" EOL;
    }
  } else if (std::holds_alternative<ASTNodeVarDecl>(*this->body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*this->body);

    result += std::string(indent, ' ') + "<NodeVarDecl>" EOL;
    result += std::string(indent + 2, ' ') + R"(<slot name="var">)" EOL;
    result += nodeVarDecl.var->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</slot>" EOL;

    if (nodeVarDecl.init != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" EOL;
      result += (*nodeVarDecl.init).xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</slot>" EOL;
    }

    result += std::string(indent, ' ') + "</NodeVarDecl>" EOL;
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Node>";
}

std::string ASTNodeIf::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<NodeIf>" EOL;
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
    result += std::string(indent + 2, ' ') + R"(<slot name="alt">)" EOL;

    if (std::holds_alternative<ASTBlock>(altBody)) {
      auto altElse = std::get<ASTBlock>(altBody);
      result += blockToXml(altElse, indent + 4);
    } else if (std::holds_alternative<ASTNodeIf>(altBody)) {
      auto altElif = std::get<ASTNodeIf>(altBody);
      result += altElif.xml(indent + 4) + EOL;
    }

    result += std::string(indent + 2, ' ') + "</slot>" EOL;
  }

  return result + std::string(indent, ' ') + "</NodeIf>";
}
