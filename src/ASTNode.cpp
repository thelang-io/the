/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ASTNode.hpp"

std::string blockToXml (const ASTBlock &block, std::size_t indent) {
  auto result = std::string();

  for (const auto &node : block) {
    result += node.xml(indent) + "\n";
  }

  return result;
}

std::string ASTNode::xml (std::size_t indent) const {
  auto result = std::string(indent, ' ') + "<Node>\n";
  indent += 2;

  if (std::holds_alternative<ASTNodeBreak>(this->body)) {
    result += std::string(indent, ' ') + "<NodeBreak />\n";
  } else if (std::holds_alternative<ASTNodeContinue>(this->body)) {
    result += std::string(indent, ' ') + "<NodeContinue />\n";
  } else if (std::holds_alternative<ASTNodeExpr>(this->body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(this->body);
    result += nodeExpr.xml(indent) + "\n";
  } else if (std::holds_alternative<ASTNodeFnDecl>(this->body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(this->body);

    result += std::string(indent, ' ') + "<NodeFnDecl>\n";
    result += std::string(indent + 2, ' ') + R"(<slot name="var">)" "\n";
    result += nodeFnDecl.var->xml(indent + 4) + "\n";
    result += std::string(indent + 2, ' ') + "</slot>\n";

    if (!nodeFnDecl.stack.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="stack">)" "\n";

      for (const auto &nodeFnDeclStackItem : nodeFnDecl.stack) {
        result += std::string(indent + 4, ' ') + "<NodeFnDeclStackItem>\n";
        result += nodeFnDeclStackItem->xml(indent + 6) + "\n";
        result += std::string(indent + 4, ' ') + "</NodeFnDeclStackItem>\n";
      }

      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (!nodeFnDecl.params.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="params">)" "\n";

      for (const auto &nodeFnDeclParam : nodeFnDecl.params) {
        result += std::string(indent + 4, ' ') + "<NodeFnDeclParam>\n";
        result += std::string(indent + 6, ' ') + R"(<slot name="var">)" "\n";
        result += nodeFnDeclParam.var->xml(indent + 8) + "\n";
        result += std::string(indent + 6, ' ') + "</slot>\n";

        if (nodeFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + R"(<slot name="init">)" "\n";
          result += (*nodeFnDeclParam.init)->xml(indent + 8) + "\n";
          result += std::string(indent + 6, ' ') + "</slot>\n";
        }

        result += std::string(indent + 4, ' ') + "</NodeFnDeclParam>\n";
      }

      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (!nodeFnDecl.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" "\n";
      result += blockToXml(nodeFnDecl.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</NodeFnDecl>\n";
  } else if (std::holds_alternative<ASTNodeIf>(this->body)) {
    auto nodeIf = std::get<ASTNodeIf>(this->body);
    result += nodeIf.xml(indent) + "\n";
  } else if (std::holds_alternative<ASTNodeLoop>(this->body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(this->body);
    result += std::string(indent, ' ') + "<NodeLoop>\n";

    if (nodeLoop.init != std::nullopt) {
      auto nodeLoopInit = **nodeLoop.init;

      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" "\n";
      result += nodeLoopInit.xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (nodeLoop.cond != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="cond">)" "\n";
      result += (*nodeLoop.cond)->xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (nodeLoop.upd != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="upd">)" "\n";
      result += (*nodeLoop.upd)->xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    if (!nodeLoop.body.empty()) {
      result += std::string(indent + 2, ' ') + R"(<slot name="body">)" "\n";
      result += blockToXml(nodeLoop.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</NodeLoop>\n";
  } else if (std::holds_alternative<ASTNodeMain>(this->body)) {
    auto nodeMain = std::get<ASTNodeMain>(this->body);

    result += std::string(indent, ' ') + "<NodeMain>\n";
    result += blockToXml(nodeMain.body, indent + 2);
    result += std::string(indent, ' ') + "</NodeMain>\n";
  } else if (std::holds_alternative<ASTNodeObjDecl>(this->body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(this->body);

    result += std::string(indent, ' ') + "<NodeObjDecl>\n";
    result += nodeObjDecl.var->xml(indent + 2) + "\n";
    result += std::string(indent, ' ') + "</NodeObjDecl>\n";
  } else if (std::holds_alternative<ASTNodeReturn>(this->body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(this->body);

    if (nodeReturn.body == std::nullopt) {
      result += std::string(indent, ' ') + "<NodeReturn />\n";
    } else {
      result += std::string(indent, ' ') + "<NodeReturn>\n";
      result += (*nodeReturn.body)->xml(indent + 2) + "\n";
      result += std::string(indent, ' ') + "</NodeReturn>\n";
    }
  } else if (std::holds_alternative<ASTNodeVarDecl>(this->body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(this->body);

    result += std::string(indent, ' ') + "<NodeVarDecl>\n";
    result += std::string(indent + 2, ' ') + R"(<slot name="var">)" "\n";
    result += nodeVarDecl.var->xml(indent + 4) + "\n";
    result += std::string(indent + 2, ' ') + "</slot>\n";

    if (nodeVarDecl.init != std::nullopt) {
      result += std::string(indent + 2, ' ') + R"(<slot name="init">)" "\n";
      result += (*nodeVarDecl.init)->xml(indent + 4) + "\n";
      result += std::string(indent + 2, ' ') + "</slot>\n";
    }

    result += std::string(indent, ' ') + "</NodeVarDecl>\n";
  }

  indent -= 2;
  return result + std::string(indent, ' ') + "</Node>";
}

std::string ASTNodeIf::xml (std::size_t indent) const {
  auto result = std::string();

  result += std::string(indent, ' ') + "<NodeIf>\n";
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

    if (std::holds_alternative<ASTBlock>(altBody)) {
      auto altElse = std::get<ASTBlock>(altBody);
      result += blockToXml(altElse, indent + 4);
    } else if (std::holds_alternative<ASTNodeIf>(altBody)) {
      auto altElif = std::get<ASTNodeIf>(altBody);
      result += altElif.xml(indent + 4) + "\n";
    }

    result += std::string(indent + 2, ' ') + "</slot>\n";
  }

  return result + std::string(indent, ' ') + "</NodeIf>";
}
