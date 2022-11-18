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
  auto result = std::string();

  if (std::holds_alternative<ASTNodeBreak>(*this->body)) {
    result += std::string(indent, ' ') + "<NodeBreak />";
  } else if (std::holds_alternative<ASTNodeContinue>(*this->body)) {
    result += std::string(indent, ' ') + "<NodeContinue />";
  } else if (std::holds_alternative<ASTNodeExpr>(*this->body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*this->body);
    result += nodeExpr.xml(indent);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*this->body)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(*this->body);

    result += std::string(indent, ' ') + "<NodeFnDecl>" EOL;
    result += std::string(indent + 2, ' ') + "<NodeFnDeclVar>" EOL;
    result += nodeFnDecl.var->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</NodeFnDeclVar>" EOL;

    if (!nodeFnDecl.stack.empty()) {
      result += std::string(indent + 2, ' ') + "<NodeFnDeclCtx>" EOL;

      for (const auto &ctxVar : nodeFnDecl.stack) {
        result += ctxVar->xml(indent + 4) + EOL;
      }

      result += std::string(indent + 2, ' ') + "</NodeFnDeclCtx>" EOL;
    }

    if (!nodeFnDecl.params.empty()) {
      result += std::string(indent + 2, ' ') + "<FnDeclParams>" EOL;

      for (const auto &nodeFnDeclParam : nodeFnDecl.params) {
        result += std::string(indent + 4, ' ') + "<FnDeclParam>" EOL;
        result += std::string(indent + 6, ' ') + "<FnDeclParamVar>" EOL;
        result += nodeFnDeclParam.var->xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</FnDeclParamVar>" EOL;

        if (nodeFnDeclParam.init != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<FnDeclParamInit>" EOL;
          result += nodeFnDeclParam.init->xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</FnDeclParamInit>" EOL;
        }

        result += std::string(indent + 4, ' ') + "</FnDeclParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</FnDeclParams>" EOL;
    }

    if (!nodeFnDecl.body.empty()) {
      result += std::string(indent + 2, ' ') + "<NodeFnDeclBody>" EOL;
      result += blockToXml(nodeFnDecl.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</NodeFnDeclBody>" EOL;
    }

    result += std::string(indent, ' ') + "</NodeFnDecl>";
  } else if (std::holds_alternative<ASTNodeIf>(*this->body)) {
    auto nodeIf = std::get<ASTNodeIf>(*this->body);

    result += std::string(indent, ' ') + "<NodeIf>" EOL;
    result += std::string(indent + 2, ' ') + "<NodeIfCond>" EOL;
    result += nodeIf.cond.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</NodeIfCond>" EOL;

    if (!nodeIf.body.empty()) {
      result += std::string(indent + 2, ' ') + "<NodeIfBody>" EOL;
      result += blockToXml(nodeIf.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</NodeIfBody>" EOL;
    }

    if (nodeIf.alt != std::nullopt) {
      if (
        (std::holds_alternative<ASTBlock>(*nodeIf.alt) && !std::get<ASTBlock>(*nodeIf.alt).empty()) ||
        std::holds_alternative<ASTNode>(*nodeIf.alt)
      ) {
        result += std::string(indent + 2, ' ') + "<NodeIfAlt>" EOL;

        if (std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
          auto altElse = std::get<ASTBlock>(*nodeIf.alt);
          result += blockToXml(altElse, indent + 4);
        } else if (std::holds_alternative<ASTNode>(*nodeIf.alt)) {
          auto altElif = std::get<ASTNode>(*nodeIf.alt);
          result += altElif.xml(indent + 4) + EOL;
        }

        result += std::string(indent + 2, ' ') + "</NodeIfAlt>" EOL;
      }
    }

    result += std::string(indent, ' ') + "</NodeIf>";
  } else if (std::holds_alternative<ASTNodeLoop>(*this->body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(*this->body);
    result += std::string(indent, ' ') + "<NodeLoop>" EOL;

    if (nodeLoop.init != std::nullopt) {
      auto nodeLoopInit = *nodeLoop.init;

      result += std::string(indent + 2, ' ') + "<NodeLoopInit>" EOL;
      result += nodeLoopInit.xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</NodeLoopInit>" EOL;
    }

    if (nodeLoop.cond != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<NodeLoopCond>" EOL;
      result += nodeLoop.cond->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</NodeLoopCond>" EOL;
    }

    if (nodeLoop.upd != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<NodeLoopUpd>" EOL;
      result += nodeLoop.upd->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</NodeLoopUpd>" EOL;
    }

    if (!nodeLoop.body.empty()) {
      result += std::string(indent + 2, ' ') + "<NodeLoopBody>" EOL;
      result += blockToXml(nodeLoop.body, indent + 4);
      result += std::string(indent + 2, ' ') + "</NodeLoopBody>" EOL;
    }

    result += std::string(indent, ' ') + "</NodeLoop>";
  } else if (std::holds_alternative<ASTNodeMain>(*this->body)) {
    auto nodeMain = std::get<ASTNodeMain>(*this->body);

    result += std::string(indent, ' ') + "<NodeMain>" EOL;
    result += blockToXml(nodeMain.body, indent + 2);
    result += std::string(indent, ' ') + "</NodeMain>";
  } else if (std::holds_alternative<ASTNodeObjDecl>(*this->body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(*this->body);

    result += std::string(indent, ' ') + "<NodeObjDecl>" EOL;
    result += nodeObjDecl.type->xml(indent + 2) + EOL;
    result += std::string(indent, ' ') + "</NodeObjDecl>";
  } else if (std::holds_alternative<ASTNodeReturn>(*this->body)) {
    auto nodeReturn = std::get<ASTNodeReturn>(*this->body);

    if (nodeReturn.body == std::nullopt) {
      result += std::string(indent, ' ') + "<NodeReturn />";
    } else {
      result += std::string(indent, ' ') + "<NodeReturn>" EOL;
      result += nodeReturn.body->xml(indent + 2) + EOL;
      result += std::string(indent, ' ') + "</NodeReturn>";
    }
  } else if (std::holds_alternative<ASTNodeVarDecl>(*this->body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*this->body);

    result += std::string(indent, ' ') + "<NodeVarDecl>" EOL;
    result += std::string(indent + 2, ' ') + "<NodeVarDeclVar>" EOL;
    result += nodeVarDecl.var->xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</NodeVarDeclVar>" EOL;

    if (nodeVarDecl.init != std::nullopt) {
      result += std::string(indent + 2, ' ') + "<NodeVarDeclInit>" EOL;
      result += nodeVarDecl.init->xml(indent + 4) + EOL;
      result += std::string(indent + 2, ' ') + "</NodeVarDeclInit>" EOL;
    }

    result += std::string(indent, ' ') + "</NodeVarDecl>";
  }

  return result;
}
