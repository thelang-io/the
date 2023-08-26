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

#ifndef SRC_AST_CHECKER_HPP
#define SRC_AST_CHECKER_HPP

#include "AST.hpp"
#include <algorithm>
#include "Error.hpp"

class ASTChecker {
 public:
  static std::vector<ASTNodeExpr> flattenExpr (const std::vector<ASTNodeExpr> &exprs, bool localScope = true) {
    auto result = std::vector<ASTNodeExpr>{};

    for (const auto &expr : exprs) {
      result.push_back(expr);

      if (std::holds_alternative<ASTExprAccess>(*expr.body)) {
        auto exprBody = std::get<ASTExprAccess>(*expr.body);

        if (exprBody.expr != std::nullopt && std::holds_alternative<ASTNodeExpr>(*exprBody.expr)) {
          auto childResult1 = ASTChecker::flattenExpr({ std::get<ASTNodeExpr>(*exprBody.expr) }, localScope);
          result.insert(result.end(), childResult1.begin(), childResult1.end());
        }

        if (exprBody.elem != std::nullopt) {
          auto childResult2 = ASTChecker::flattenExpr({ *exprBody.elem }, localScope);
          result.insert(result.end(), childResult2.begin(), childResult2.end());
        }
      } else if (std::holds_alternative<ASTExprArray>(*expr.body)) {
        auto exprBody = std::get<ASTExprArray>(*expr.body);
        auto childResult = ASTChecker::flattenExpr(exprBody.elements, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprAs>(*expr.body)) {
        auto exprBody = std::get<ASTExprAs>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.expr }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprAssign>(*expr.body)) {
        auto exprBody = std::get<ASTExprAssign>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.left, exprBody.right }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprAwait>(*expr.body)) {
        auto exprBody = std::get<ASTExprAwait>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.arg }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprBinary>(*expr.body)) {
        auto exprBody = std::get<ASTExprBinary>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.left, exprBody.right }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprCall>(*expr.body)) {
        auto exprBody = std::get<ASTExprCall>(*expr.body);
        auto childResult1 = ASTChecker::flattenExpr({ exprBody.callee }, localScope);
        result.insert(result.end(), childResult1.begin(), childResult1.end());

        for (const auto &arg : exprBody.args) {
          auto childResult2 = ASTChecker::flattenExpr({ arg.expr }, localScope);
          result.insert(result.end(), childResult2.begin(), childResult2.end());
        }
      } else if (std::holds_alternative<ASTExprClosure>(*expr.body)) {
        auto exprBody = std::get<ASTExprClosure>(*expr.body);

        if (!localScope) {
          for (const auto &param : exprBody.params) {
            if (param.init != std::nullopt) {
              auto childResult = ASTChecker::flattenExpr({ *param.init }, localScope);
              result.insert(result.end(), childResult.begin(), childResult.end());
            }
          }

          auto childResult = ASTChecker::flattenExpr(ASTChecker::flattenNodeExprs(exprBody.body, localScope), localScope);
          result.insert(result.end(), childResult.begin(), childResult.end());
        }
      } else if (std::holds_alternative<ASTExprCond>(*expr.body)) {
        auto exprBody = std::get<ASTExprCond>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.cond, exprBody.body, exprBody.alt }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprIs>(*expr.body)) {
        auto exprBody = std::get<ASTExprIs>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.expr }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprMap>(*expr.body)) {
        auto exprBody = std::get<ASTExprMap>(*expr.body);

        for (const auto &prop : exprBody.props) {
          auto childResult = ASTChecker::flattenExpr({ prop.init }, localScope);
          result.insert(result.end(), childResult.begin(), childResult.end());
        }
      } else if (std::holds_alternative<ASTExprObj>(*expr.body)) {
        auto exprBody = std::get<ASTExprObj>(*expr.body);

        for (const auto &prop : exprBody.props) {
          auto childResult = ASTChecker::flattenExpr({ prop.init }, localScope);
          result.insert(result.end(), childResult.begin(), childResult.end());
        }
      } else if (std::holds_alternative<ASTExprRef>(*expr.body)) {
        auto exprBody = std::get<ASTExprRef>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.expr }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTExprUnary>(*expr.body)) {
        auto exprBody = std::get<ASTExprUnary>(*expr.body);
        auto childResult = ASTChecker::flattenExpr({ exprBody.arg }, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      }
    }

    return result;
  }

  static std::vector<ASTNode> flattenNode (const std::vector<ASTNode> &nodes, bool localScope = true) {
    auto result = std::vector<ASTNode>{};

    for (const auto &node : nodes) {
      result.push_back(node);

      if (std::holds_alternative<ASTNodeExportDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeExportDecl>(*node.body);

        if (nodeBody.declaration != std::nullopt) {
          auto childResult = ASTChecker::flattenNode({ *nodeBody.declaration }, localScope);
          result.insert(result.end(), childResult.begin(), childResult.end());
        }
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeFnDecl>(*node.body);

        if (!localScope && nodeBody.body != std::nullopt) {
          auto childResult = ASTChecker::flattenNode(*nodeBody.body, localScope);
          result.insert(result.end(), childResult.begin(), childResult.end());
        }
      } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
        auto nodeBody = std::get<ASTNodeIf>(*node.body);
        auto childResult1 = ASTChecker::flattenNode(nodeBody.body, localScope);
        result.insert(result.end(), childResult1.begin(), childResult1.end());

        if (nodeBody.alt != std::nullopt && std::holds_alternative<ASTBlock>(*nodeBody.alt)) {
          auto nodeBodyAlt = std::get<ASTBlock>(*nodeBody.alt);
          auto childResult2 = ASTChecker::flattenNode(nodeBodyAlt, localScope);
          result.insert(result.end(), childResult2.begin(), childResult2.end());
        } else if (nodeBody.alt != std::nullopt && std::holds_alternative<ASTNode>(*nodeBody.alt)) {
          auto childResult3 = ASTChecker::flattenNode({ std::get<ASTNode>(*nodeBody.alt) }, localScope);
          result.insert(result.end(), childResult3.begin(), childResult3.end());
        }
      } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
        auto nodeBody = std::get<ASTNodeLoop>(*node.body);

        if (nodeBody.init != std::nullopt) {
          auto childResult1 = ASTChecker::flattenNode({ *nodeBody.init }, localScope);
          result.insert(result.end(), childResult1.begin(), childResult1.end());
        }

        auto childResult2 = ASTChecker::flattenNode(nodeBody.body, localScope);
        result.insert(result.end(), childResult2.begin(), childResult2.end());
      } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
        auto nodeBody = std::get<ASTNodeMain>(*node.body);
        auto childResult = ASTChecker::flattenNode(nodeBody.body, localScope);
        result.insert(result.end(), childResult.begin(), childResult.end());
      } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeObjDecl>(*node.body);

        if (!localScope) {
          for (const auto &method : nodeBody.methods) {
            if (method.body != std::nullopt) {
              auto childResult = ASTChecker::flattenNode(*method.body, localScope);
              result.insert(result.end(), childResult.begin(), childResult.end());
            }
          }
        }
      } else if (std::holds_alternative<ASTNodeTry>(*node.body)) {
        auto nodeBody = std::get<ASTNodeTry>(*node.body);
        auto childResult1 = ASTChecker::flattenNode(nodeBody.body, localScope);
        result.insert(result.end(), childResult1.begin(), childResult1.end());

        for (const auto &handler : nodeBody.handlers) {
          auto childResult2 = ASTChecker::flattenNode({ handler.param }, localScope);
          result.insert(result.end(), childResult2.begin(), childResult2.end());
          auto childResult3 = ASTChecker::flattenNode(handler.body, localScope);
          result.insert(result.end(), childResult3.begin(), childResult3.end());
        }
      }
    }

    return result;
  }

  static std::vector<ASTNodeExpr> flattenNodeExprs (const std::vector<ASTNode> &nodes, bool localScope = true) {
    auto flattenNodes = ASTChecker::flattenNode(nodes, localScope);
    auto result = std::vector<ASTNodeExpr>{};

    for (const auto &node : flattenNodes) {
      if (std::holds_alternative<ASTNodeEnumDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeEnumDecl>(*node.body);

        if (!localScope) {
          for (const auto &member : nodeBody.members) {
            if (member.init != std::nullopt) {
              result.push_back(*member.init);
            }
          }
        }
      } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
        auto nodeBody = std::get<ASTNodeExpr>(*node.body);
        result.push_back(nodeBody);
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeFnDecl>(*node.body);

        if (!localScope) {
          for (const auto &param : nodeBody.params) {
            if (param.init != std::nullopt) {
              result.push_back(*param.init);
            }
          }
        }
      } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
        auto nodeBody = std::get<ASTNodeIf>(*node.body);
        result.push_back(nodeBody.cond);
      } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
        auto nodeBody = std::get<ASTNodeLoop>(*node.body);

        if (nodeBody.cond != std::nullopt) {
          result.push_back(*nodeBody.cond);
        }

        if (nodeBody.upd != std::nullopt) {
          result.push_back(*nodeBody.upd);
        }
      } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeObjDecl>(*node.body);

        if (!localScope) {
          for (const auto &method : nodeBody.methods) {
            for (const auto &param : method.params) {
              if (param.init != std::nullopt) {
                result.push_back(*param.init);
              }
            }
          }
        }
      } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
        auto nodeBody = std::get<ASTNodeReturn>(*node.body);

        if (nodeBody.body != std::nullopt) {
          result.push_back(*nodeBody.body);
        }
      } else if (std::holds_alternative<ASTNodeThrow>(*node.body)) {
        auto nodeBody = std::get<ASTNodeThrow>(*node.body);
        result.push_back(nodeBody.arg);
      } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
        auto nodeBody = std::get<ASTNodeVarDecl>(*node.body);

        if (nodeBody.init != std::nullopt) {
          result.push_back(*nodeBody.init);
        }
      }
    }

    return result;
  }

  explicit ASTChecker (const ASTNodeExpr &expr) {
    this->_exprs = {expr};
  }

  explicit ASTChecker (const ASTNode &node) {
    this->_nodes = {node};
    this->_isNode = true;
  }

  explicit ASTChecker (const ASTNode *node) {
    if (node != nullptr) {
      this->_nodes = {*node};
    }

    this->_isNode = true;
  }

  explicit ASTChecker (const std::vector<ASTNodeExpr> &exprs) {
    this->_exprs = exprs;
  }

  explicit ASTChecker (const ASTBlock &nodes) {
    this->_nodes = nodes;
    this->_isNode = true;
  }

  bool async (bool localScope = true) const {
    if (!this->_exprs.empty()) {
      return this->_asyncExpr(this->_exprs, localScope);
    } else {
      return this->_asyncNode(this->_nodes, localScope);
    }
  }

  template <typename T>
  bool endsWith () const {
    this->_checkNode();
    return this->_endsWithNode<T>(this->_nodes);
  }

  template <typename T>
  std::vector<ASTNodeExpr> getExprOfType () const {
    if (!this->_exprs.empty()) {
      return this->_getExprOfType<T>(this->_exprs);
    } else {
      return this->_getExprOfTypeFromNodes<T>(this->_nodes);
    }
  }

  template <typename T>
  bool has () const {
    this->_checkNode();
    return this->_hasNode<T>(this->_nodes);
  }

  bool hasAwait () const {
    return this->hasExpr<ASTExprAwait>();
  }

  template <typename T>
  bool hasExpr () const {
    if (!this->_exprs.empty()) {
      return !this->_getExprOfType<T>(this->_exprs).empty();
    } else {
      return !this->_getExprOfTypeFromNodes<T>(this->_nodes).empty();
    }
  }

  bool hoistingFriendly () const {
    this->_checkNode();

    return std::all_of(this->_nodes.begin(), this->_nodes.end(), [] (const auto &it) {
      return std::holds_alternative<ASTNodeEnumDecl>(*it.body) ||
        std::holds_alternative<ASTNodeExportDecl>(*it.body) ||
        std::holds_alternative<ASTNodeFnDecl>(*it.body) ||
        std::holds_alternative<ASTNodeObjDecl>(*it.body) ||
        std::holds_alternative<ASTNodeTypeDecl>(*it.body);
    });
  }

  bool insideMain () {
    this->_checkNode();

    if (this->_nodes.empty()) {
      return false;
    }

    auto p = this->_nodes[0].parent;

    while (p != nullptr) {
      if (std::holds_alternative<ASTNodeMain>(*p->body)) {
        return !std::get<ASTNodeMain>(*p->body).async;
      } else if (std::holds_alternative<ASTNodeFnDecl>(*p->body) || std::holds_alternative<ASTNodeObjDecl>(*p->body)) {
        return false;
      }

      p = p->parent;
    }

    return true;
  }

  template <typename T>
  bool is () const {
    this->_checkNode();
    return this->_isNodeMethod<T>(this->_nodes);
  }

  bool isLast (const std::vector<ASTNode> &backupNodes = {}) const {
    this->_checkNode();
    return this->_isLastNode(this->_nodes, backupNodes);
  }

  template <typename T>
  bool parentIs () const {
    this->_checkNode();

    if (this->_nodes.empty()) {
      return false;
    }

    auto p = this->_nodes[0].parent;

    while (p != nullptr) {
      if (std::holds_alternative<T>(*p->body)) {
        return true;
      } else if (
        std::holds_alternative<ASTNodeFnDecl>(*p->body) ||
        std::holds_alternative<ASTNodeMain>(*p->body) ||
        std::holds_alternative<ASTNodeObjDecl>(*p->body)
      ) {
        return false;
      }

      p = p->parent;
    }

    return false;
  }

  bool throws (bool localScope = true) const {
    if (!this->_exprs.empty()) {
      return this->_throwsExpr(this->_exprs, localScope);
    } else {
      return this->_throwsNode(this->_nodes, localScope);
    }
  }

  bool throwsPossible () const {
    if (!this->_exprs.empty()) {
      return this->_throwsPossibleExpr(this->_exprs);
    } else {
      auto result = this->_nodes;

      result.erase(std::remove_if(result.begin(), result.end(), [] (const auto &it) -> bool {
        return !std::holds_alternative<ASTNodeExpr>(*it.body) && !std::holds_alternative<ASTNodeVarDecl>(*it.body);
      }), result.end());

      return this->_throwsPossibleExpr(ASTChecker::flattenNodeExprs(result));
    }
  }

 private:
  bool _isNode = false;
  std::vector<ASTNodeExpr> _exprs;
  std::vector<ASTNode> _nodes;

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  bool _asyncExpr (const std::vector<ASTNodeExpr> &exprs, bool localScope) const {
    auto result = ASTChecker::flattenExpr(exprs, localScope);

    return std::any_of(result.begin(), result.end(), [&] (const auto &it) -> bool {
      if (std::holds_alternative<ASTExprAwait>(*it.body)) {
        return true;
      } else if (!localScope && std::holds_alternative<ASTExprCall>(*it.body)) {
        auto exprBody = std::get<ASTExprCall>(*it.body);
        auto calleeRealType = Type::real(exprBody.callee.type);

        return std::get<TypeFn>(calleeRealType->body).async;
      }

      return false;
    });
  }

  bool _asyncNode (const std::vector<ASTNode> &nodes, bool localScope) const {
    return this->_asyncExpr(ASTChecker::flattenNodeExprs(nodes, localScope), localScope);
  }

  void _checkNode () const {
    if (!this->_isNode) {
      throw Error("tried node method on non node");
    }
  }

  template <typename T>
  bool _endsWithNode (const std::vector<ASTNode> &nodes) const {
    if (nodes.empty()) {
      return false;
    }

    return std::holds_alternative<T>(*nodes.back().body);
  }

  template <typename T>
  std::vector<ASTNodeExpr> _getExprOfType (const std::vector<ASTNodeExpr> &exprs) const {
    auto result = ASTChecker::flattenExpr(exprs);

    result.erase(std::remove_if(result.begin(), result.end(), [] (const auto &it) -> bool {
      return !std::holds_alternative<T>(*it.body);
    }), result.end());

    return result;
  }

  template <typename T>
  std::vector<ASTNodeExpr> _getExprOfTypeFromNodes (const std::vector<ASTNode> &nodes) const {
    return this->_getExprOfType<T>(ASTChecker::flattenNodeExprs(nodes));
  }

  template <typename T>
  bool _hasNode (const std::vector<ASTNode> &nodes) const {
    auto flattenNodes = ASTChecker::flattenNode(nodes);

    return std::any_of(flattenNodes.begin(), flattenNodes.end(), [] (const auto &it) -> bool {
      return std::holds_alternative<T>(*it.body);
    });
  }

  template <typename T>
  bool _isNodeMethod (const std::vector<ASTNode> &nodes) const {
    if (nodes.empty()) {
      return false;
    }

    return std::all_of(nodes.begin(), nodes.end(), [] (const auto &node) {
      return std::holds_alternative<T>(*node.body);
    });
  }

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  bool _isLastNode (const std::vector<ASTNode> &nodes, const std::vector<ASTNode> &backupNodes) const {
    if (nodes.size() != 1) {
      throw Error("tried isLast on many nodes");
    } else if (nodes[0].parent == nullptr) {
      throw Error("tried isLast on root node");
    }

    auto node = nodes[0];
    auto parent = *node.parent;

    if (std::holds_alternative<ASTNodeFnDecl>(*parent.body)) {
      auto nodeFnDecl = std::get<ASTNodeFnDecl>(*parent.body);
      return nodeFnDecl.body != std::nullopt && !nodeFnDecl.body->empty() && nodeFnDecl.body->back().body == node.body;
    } else if (std::holds_alternative<ASTNodeIf>(*parent.body)) {
      auto nodeIf = std::get<ASTNodeIf>(*parent.body);

      if (!nodeIf.body.empty() && nodeIf.body.back().body == node.body) {
        return true;
      } else if (nodeIf.alt != std::nullopt && std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
        auto nodeIfAlt = std::get<ASTBlock>(*nodeIf.alt);
        return !nodeIfAlt.empty() && nodeIfAlt.back().body == node.body;
      }

      return false;
    } else if (std::holds_alternative<ASTNodeLoop>(*parent.body)) {
      auto nodeLoop = std::get<ASTNodeLoop>(*parent.body);
      return !nodeLoop.body.empty() && nodeLoop.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeMain>(*parent.body)) {
      auto nodeMain = std::get<ASTNodeMain>(*parent.body);
      return !nodeMain.body.empty() && nodeMain.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeObjDecl>(*parent.body)) {
      auto nodeObjDecl = std::get<ASTNodeObjDecl>(*parent.body);

      return std::any_of(nodeObjDecl.methods.begin(), nodeObjDecl.methods.end(), [&] (const auto &it) -> bool {
        return it.body != std::nullopt && !it.body->empty() && it.body->back().body == node.body;
      });
    } else if (std::holds_alternative<ASTNodeTry>(*parent.body)) {
      auto nodeTry = std::get<ASTNodeTry>(*parent.body);

      if (!nodeTry.body.empty() && nodeTry.body.back().body == node.body) {
        return true;
      }

      return std::any_of(nodeTry.handlers.begin(), nodeTry.handlers.end(), [&] (const auto &it) -> bool {
        return !it.body.empty() && it.body.back().body == node.body;
      });
    }

    if (!backupNodes.empty()) {
      return backupNodes.back().body == node.body;
    }

    throw Error("tried isLast on unknown node");
  }

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  bool _throwsExpr (const std::vector<ASTNodeExpr> &exprs, bool localScope) const {
    auto result = ASTChecker::flattenExpr(exprs, localScope);

    return std::any_of(result.begin(), result.end(), [] (const auto &it) -> bool {
      if (std::holds_alternative<ASTExprAccess>(*it.body)) {
        auto exprBody = std::get<ASTExprAccess>(*it.body);

        if (exprBody.elem != std::nullopt) {
          return true;
        } else if (
          exprBody.expr != std::nullopt &&
          std::holds_alternative<ASTNodeExpr>(*exprBody.expr) &&
          exprBody.prop != std::nullopt
        ) {
          auto exprAccessExpr = std::get<ASTNodeExpr>(*exprBody.expr);

          if (!exprAccessExpr.type->isEnum()) {
            auto typeField = exprAccessExpr.type->getField(*exprBody.prop);
            return !typeField.callInfo.empty() && typeField.callInfo.throws;
          }
        }
      } else if (std::holds_alternative<ASTExprCall>(*it.body)) {
        auto exprBody = std::get<ASTExprCall>(*it.body);
        auto calleeRealType = Type::real(exprBody.callee.type);

        return std::get<TypeFn>(calleeRealType->body).throws;
      }

      return false;
    });
  }

  bool _throwsNode (const std::vector<ASTNode> &nodes, bool localScope) const {
    auto flattenNodes = ASTChecker::flattenNode(nodes, localScope);

    for (const auto &node : flattenNodes) {
      if (std::holds_alternative<ASTNodeThrow>(*node.body) || std::holds_alternative<ASTNodeTry>(*node.body)) {
        return true;
      }
    }

    return this->_throwsExpr(ASTChecker::flattenNodeExprs(flattenNodes, localScope), localScope);
  }

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  bool _throwsPossibleExpr (const std::vector<ASTNodeExpr> &exprs) const {
    auto result = ASTChecker::flattenExpr(exprs);

    return std::any_of(result.begin(), result.end(), [] (const auto &it) -> bool {
      if (std::holds_alternative<ASTExprAccess>(*it.body)) {
        auto exprBody = std::get<ASTExprAccess>(*it.body);

        if (exprBody.elem != std::nullopt) {
          return true;
        } else if (
          exprBody.expr != std::nullopt &&
          std::holds_alternative<ASTNodeExpr>(*exprBody.expr) &&
          exprBody.prop != std::nullopt
        ) {
          auto exprAccessExpr = std::get<ASTNodeExpr>(*exprBody.expr);

          if (!exprAccessExpr.type->isEnum()) {
            auto typeField = exprAccessExpr.type->getField(*exprBody.prop);
            return !typeField.callInfo.empty() && typeField.callInfo.throws;
          }
        }
      } else if (std::holds_alternative<ASTExprCall>(*it.body)) {
        auto exprBody = std::get<ASTExprCall>(*it.body);
        auto calleeRealType = Type::real(exprBody.callee.type);

        return !std::get<TypeFn>(calleeRealType->body).async;
      }

      return false;
    });
  }
};

#endif
