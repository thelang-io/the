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

#include "ASTNode.hpp"
#include <algorithm>
#include "Error.hpp"

// todo test each branch
bool throwsNodeExpr (const ASTNodeExpr &nodeExpr) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);

    if (exprAccess.elem != std::nullopt) {
      return true;
    } else if (exprAccess.prop != std::nullopt) {
      auto typeField = TypeField{};

      if (std::holds_alternative<std::shared_ptr<Var>>(*exprAccess.expr)) {
        auto var = std::get<std::shared_ptr<Var>>(*exprAccess.expr);
        typeField = var->type->getField(*exprAccess.prop);
      } else {
        auto exprAccessExpr = std::get<ASTNodeExpr>(*exprAccess.expr);
        typeField = exprAccessExpr.type->getField(*exprAccess.prop);
      }

      if (!typeField.callInfo.empty() && typeField.callInfo.throws) {
        return true;
      }
    } else if (
      exprAccess.expr != std::nullopt &&
      std::holds_alternative<ASTNodeExpr>(*exprAccess.expr) &&
      throwsNodeExpr(std::get<ASTNodeExpr>(*exprAccess.expr))
    ) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprArray>(*nodeExpr.body)) {
    auto exprArray = std::get<ASTExprArray>(*nodeExpr.body);

    for (const auto &element : exprArray.elements) {
      if (throwsNodeExpr(element)) {
        return true;
      }
    }
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);

    if (throwsNodeExpr(exprAssign.left) || throwsNodeExpr(exprAssign.right)) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (throwsNodeExpr(exprBinary.left) || throwsNodeExpr(exprBinary.right)) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) {
    auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);

    if (std::get<TypeFn>(exprCall.callee.type->body).throws) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);

    if (throwsNodeExpr(exprCond.cond) || throwsNodeExpr(exprCond.body) || throwsNodeExpr(exprCond.alt)) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprIs>(*nodeExpr.body)) {
    auto exprIs = std::get<ASTExprIs>(*nodeExpr.body);

    if (throwsNodeExpr(exprIs.expr)) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprMap>(*nodeExpr.body)) {
    auto exprMap = std::get<ASTExprMap>(*nodeExpr.body);

    for (const auto &prop : exprMap.props) {
      if (throwsNodeExpr(prop.init)) {
        return true;
      }
    }
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
    auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);

    for (const auto &prop : exprObj.props) {
      if (throwsNodeExpr(prop.init)) {
        return true;
      }
    }
  } else if (std::holds_alternative<ASTExprRef>(*nodeExpr.body)) {
    auto exprRef = std::get<ASTExprRef>(*nodeExpr.body);

    if (throwsNodeExpr(exprRef.expr)) {
      return true;
    }
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);

    if (throwsNodeExpr(exprUnary.arg)) {
      return true;
    }
  }

  return false;
}

class ASTChecker {
 public:
  explicit ASTChecker (const ASTExpr &expr) {
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

  explicit ASTChecker (const ASTBlock &nodes) {
    this->_nodes = nodes;
    this->_isNode = true;
  }

  explicit ASTChecker (const std::vector<ASTExpr> &exprs) {
    this->_exprs = exprs;
  }

  template <typename T>
  bool endsWith () const {
    this->_checkNode();
    return this->_endsWithNode<T>(this->_nodes);
  }

  template <typename T>
  bool has () const {
    this->_checkNode();
    return this->_hasNode<T>(this->_nodes);
  }

  template <typename T>
  bool is () const {
    this->_checkNode();
    return this->_isNodeMethod<T>(this->_nodes);
  }

  bool isLast () const {
    this->_checkNode();
    return this->_isLastNode(this->_nodes);
  }

  // todo test
  bool throws () const {
    this->_checkNode();
    return this->_throwsNode(this->_nodes);
  }

 private:
  bool _isNode = false;
  std::vector<ASTExpr> _exprs;
  std::vector<ASTNode> _nodes;

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
  bool _hasNode (const std::vector<ASTNode> &nodes) const {
    for (const auto &node : nodes) {
      if (std::holds_alternative<T>(*node.body)) {
        return true;
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

        if (nodeFnDecl.body != std::nullopt && this->_hasNode<T>(*nodeFnDecl.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
        auto nodeIf = std::get<ASTNodeIf>(*node.body);

        if (this->_hasNode<T>(nodeIf.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
        auto nodeLoop = std::get<ASTNodeLoop>(*node.body);

        if (
          (nodeLoop.init != std::nullopt && this->_hasNode<T>({ *nodeLoop.init })) ||
          this->_hasNode<T>(nodeLoop.body)
        ) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
        auto nodeMain = std::get<ASTNodeMain>(*node.body);

        if (this->_hasNode<T>(nodeMain.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
        auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);

        return std::any_of(nodeObjDecl.methods.begin(), nodeObjDecl.methods.end(), [&] (const auto &it) -> bool {
          return it.body != std::nullopt && this->_hasNode<T>(*it.body);
        });
      }
    }

    return false;
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
  bool _isLastNode (const std::vector<ASTNode> &nodes) const {
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
      return nodeIf.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeLoop>(*parent.body)) {
      auto nodeLoop = std::get<ASTNodeLoop>(*parent.body);
      return nodeLoop.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeMain>(*parent.body)) {
      auto nodeMain = std::get<ASTNodeMain>(*parent.body);
      return nodeMain.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeObjDecl>(*parent.body)) {
      auto nodeObjDecl = std::get<ASTNodeObjDecl>(*parent.body);

      return std::any_of(nodeObjDecl.methods.begin(), nodeObjDecl.methods.end(), [&] (const auto &it) -> bool {
        return it.body != std::nullopt && !it.body->empty() && it.body->back().body == node.body;
      });
    }

    throw Error("tried isLast on unknown node");
  }

  bool _throwsNode (const std::vector<ASTNode> &nodes) const {
    for (const auto &node : nodes) {
      if (std::holds_alternative<ASTNodeThrow>(*node.body)) {
        return true;
      } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
        auto nodeExpr = std::get<ASTNodeExpr>(*node.body);

        if (throwsNodeExpr(std::get<ASTNodeExpr>(*node.body))) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

        if (nodeFnDecl.body != std::nullopt && this->_throwsNode(*nodeFnDecl.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
        auto nodeIf = std::get<ASTNodeIf>(*node.body);

        if (this->_throwsNode(nodeIf.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
        auto nodeLoop = std::get<ASTNodeLoop>(*node.body);

        if (this->_throwsNode(nodeLoop.body)) {
          return true;
        }
      }
    }

    return false;
  }
};

#endif
