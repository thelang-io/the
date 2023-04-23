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

class ASTChecker {
 public:
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

  explicit ASTChecker (const ASTBlock &nodes) {
    this->_nodes = nodes;
    this->_isNode = true;
  }

  explicit ASTChecker (const std::vector<ASTNodeExpr> &exprs) {
    this->_exprs = exprs;
  }

  template <typename T>
  bool endsWith () const {
    this->_checkNode();
    return this->_endsWithNode<T>(this->_nodes);
  }

  // todo test
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

  template <typename T>
  bool insideOf () const {
    this->_checkNode();
    return this->_insideOfNode<T>(this->_nodes);
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

  bool throws () const {
    if (!this->_exprs.empty()) {
      return this->_throwsExpr(this->_exprs);
    } else {
      return this->_throwsNode(this->_nodes);
    }
  }

 private:
  bool _isNode = false;
  std::vector<ASTNodeExpr> _exprs;
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
  std::vector<ASTNodeExpr> _getExprOfType (const std::vector<ASTNodeExpr> &exprs) const {
    auto result = std::vector<ASTNodeExpr>{};

    for (const auto &expr : exprs) {
      if (std::holds_alternative<T>(*expr.body)) {
        result.push_back(expr);
        continue;
      }

      if (std::holds_alternative<ASTExprAccess>(*expr.body)) {
        auto exprAccess = std::get<ASTExprAccess>(*expr.body);

        if (exprAccess.expr != std::nullopt && std::holds_alternative<ASTNodeExpr>(*exprAccess.expr)) {
          auto exprAccessExprResult = this->_getExprOfType<T>({ std::get<ASTNodeExpr>(*exprAccess.expr) });
          result.insert(result.end(), exprAccessExprResult.begin(), exprAccessExprResult.end());
        }

        if (exprAccess.elem != std::nullopt) {
          auto exprAccessElemResult = this->_getExprOfType<T>({ *exprAccess.elem });
          result.insert(result.end(), exprAccessElemResult.begin(), exprAccessElemResult.end());
        }
      } else if (std::holds_alternative<ASTExprArray>(*expr.body)) {
        auto exprArray = std::get<ASTExprArray>(*expr.body);

        for (const auto &element : exprArray.elements) {
          auto exprArrayElemResult = this->_getExprOfType<T>({ element });
          result.insert(result.end(), exprArrayElemResult.begin(), exprArrayElemResult.end());
        }
      } else if (std::holds_alternative<ASTExprAssign>(*expr.body)) {
        auto exprAssign = std::get<ASTExprAssign>(*expr.body);

        auto exprAssignLeftResult = this->_getExprOfType<T>({ exprAssign.left });
        result.insert(result.end(), exprAssignLeftResult.begin(), exprAssignLeftResult.end());
        auto exprAssignRightResult = this->_getExprOfType<T>({ exprAssign.right });
        result.insert(result.end(), exprAssignRightResult.begin(), exprAssignRightResult.end());
      } else if (std::holds_alternative<ASTExprBinary>(*expr.body)) {
        auto exprBinary = std::get<ASTExprBinary>(*expr.body);

        auto exprBinaryLeftResult = this->_getExprOfType<T>({ exprBinary.left });
        result.insert(result.end(), exprBinaryLeftResult.begin(), exprBinaryLeftResult.end());
        auto exprBinaryRightResult = this->_getExprOfType<T>({ exprBinary.right });
        result.insert(result.end(), exprBinaryRightResult.begin(), exprBinaryRightResult.end());
      } else if (std::holds_alternative<ASTExprCall>(*expr.body)) {
        auto exprCall = std::get<ASTExprCall>(*expr.body);

        auto exprCallCalleeResult = this->_getExprOfType<T>({ exprCall.callee });
        result.insert(result.end(), exprCallCalleeResult.begin(), exprCallCalleeResult.end());

        for (const auto &arg : exprCall.args) {
          auto exprCallArgResult = this->_getExprOfType<T>({ arg.expr });
          result.insert(result.end(), exprCallArgResult.begin(), exprCallArgResult.end());
        }
      } else if (std::holds_alternative<ASTExprCond>(*expr.body)) {
        auto exprCond = std::get<ASTExprCond>(*expr.body);

        auto exprCondCondResult = this->_getExprOfType<T>({ exprCond.cond });
        result.insert(result.end(), exprCondCondResult.begin(), exprCondCondResult.end());
        auto exprCondBodyResult = this->_getExprOfType<T>({ exprCond.body });
        result.insert(result.end(), exprCondBodyResult.begin(), exprCondBodyResult.end());
        auto exprCondAltResult = this->_getExprOfType<T>({ exprCond.alt });
        result.insert(result.end(), exprCondAltResult.begin(), exprCondAltResult.end());
      } else if (std::holds_alternative<ASTExprIs>(*expr.body)) {
        auto exprIs = std::get<ASTExprIs>(*expr.body);

        auto exprIsExprResult = this->_getExprOfType<T>({ exprIs.expr });
        result.insert(result.end(), exprIsExprResult.begin(), exprIsExprResult.end());
      } else if (std::holds_alternative<ASTExprMap>(*expr.body)) {
        auto exprMap = std::get<ASTExprMap>(*expr.body);

        for (const auto &prop : exprMap.props) {
          auto exprMapPropResult = this->_getExprOfType<T>({ prop.init });
          result.insert(result.end(), exprMapPropResult.begin(), exprMapPropResult.end());
        }
      } else if (std::holds_alternative<ASTExprObj>(*expr.body)) {
        auto exprObj = std::get<ASTExprObj>(*expr.body);

        for (const auto &prop : exprObj.props) {
          auto exprObjPropResult = this->_getExprOfType<T>({ prop.init });
          result.insert(result.end(), exprObjPropResult.begin(), exprObjPropResult.end());
        }
      } else if (std::holds_alternative<ASTExprRef>(*expr.body)) {
        auto exprRef = std::get<ASTExprRef>(*expr.body);

        auto exprRefExprResult = this->_getExprOfType<T>({ exprRef.expr });
        result.insert(result.end(), exprRefExprResult.begin(), exprRefExprResult.end());
      } else if (std::holds_alternative<ASTExprUnary>(*expr.body)) {
        auto exprUnary = std::get<ASTExprUnary>(*expr.body);

        auto exprUnaryArgResult = this->_getExprOfType<T>({ exprUnary.arg });
        result.insert(result.end(), exprUnaryArgResult.begin(), exprUnaryArgResult.end());
      }
    }

    return result;
  }

  template <typename T>
  std::vector<ASTNodeExpr> _getExprOfTypeFromNodes (const std::vector<ASTNode> &nodes) const {
    auto result = std::vector<ASTNodeExpr>{};

    for (const auto &node : nodes) {
      if (std::holds_alternative<ASTNodeEnumDecl>(*node.body)) {
        auto nodeEnumDecl = std::get<ASTNodeEnumDecl>(*node.body);

        for (const auto &member : nodeEnumDecl.members) {
          if (member.init != std::nullopt) {
            auto nodeEnumDeclMemberResult = this->_getExprOfType<T>({ *member.init });
            result.insert(result.end(), nodeEnumDeclMemberResult.begin(), nodeEnumDeclMemberResult.end());
          }
        }
      } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
        auto nodeExpr = std::get<ASTNodeExpr>(*node.body);
        auto nodeExprResult = this->_getExprOfType<T>({ nodeExpr });
        result.insert(result.end(), nodeExprResult.begin(), nodeExprResult.end());
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

        for (const auto &param : nodeFnDecl.params) {
          if (param.init != std::nullopt) {
            auto nodeFnDeclParamResult = this->_getExprOfType<T>({ *param.init });
            result.insert(result.end(), nodeFnDeclParamResult.begin(), nodeFnDeclParamResult.end());
          }
        }

        if (nodeFnDecl.body != std::nullopt) {
          auto nodeFnDeclBodyResult = this->_getExprOfTypeFromNodes<T>(*nodeFnDecl.body);
          result.insert(result.end(), nodeFnDeclBodyResult.begin(), nodeFnDeclBodyResult.end());
        }
      } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
        auto nodeIf = std::get<ASTNodeIf>(*node.body);

        auto nodeIfCondResult = this->_getExprOfType<T>({ nodeIf.cond });
        result.insert(result.end(), nodeIfCondResult.begin(), nodeIfCondResult.end());
        auto nodeIfBodyResult = this->_getExprOfTypeFromNodes<T>(nodeIf.body);
        result.insert(result.end(), nodeIfBodyResult.begin(), nodeIfBodyResult.end());

        if (nodeIf.alt != std::nullopt && std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
          auto nodeIfAltResult = this->_getExprOfTypeFromNodes<T>(std::get<ASTBlock>(*nodeIf.alt));
          result.insert(result.end(), nodeIfAltResult.begin(), nodeIfAltResult.end());
        } else if (nodeIf.alt != std::nullopt && std::holds_alternative<ASTNode>(*nodeIf.alt)) {
          auto nodeIfAltResult = this->_getExprOfTypeFromNodes<T>({ std::get<ASTNode>(*nodeIf.alt) });
          result.insert(result.end(), nodeIfAltResult.begin(), nodeIfAltResult.end());
        }
      } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
        auto nodeLoop = std::get<ASTNodeLoop>(*node.body);

        if (nodeLoop.init != std::nullopt) {
          auto nodeLoopInitResult = this->_getExprOfTypeFromNodes<T>({ *nodeLoop.init });
          result.insert(result.end(), nodeLoopInitResult.begin(), nodeLoopInitResult.end());
        }

        if (nodeLoop.cond != std::nullopt) {
          auto nodeLoopCondResult = this->_getExprOfType<T>({ *nodeLoop.cond });
          result.insert(result.end(), nodeLoopCondResult.begin(), nodeLoopCondResult.end());
        }

        if (nodeLoop.upd != std::nullopt) {
          auto nodeLoopUpdResult = this->_getExprOfType<T>({ *nodeLoop.upd });
          result.insert(result.end(), nodeLoopUpdResult.begin(), nodeLoopUpdResult.end());
        }

        auto nodeLoopBodyResult = this->_getExprOfTypeFromNodes<T>(nodeLoop.body);
        result.insert(result.end(), nodeLoopBodyResult.begin(), nodeLoopBodyResult.end());
      } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
        auto nodeMain = std::get<ASTNodeMain>(*node.body);

        auto nodeMainResult = this->_getExprOfTypeFromNodes<T>(nodeMain.body);
        result.insert(result.end(), nodeMainResult.begin(), nodeMainResult.end());
      } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
        auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);

        for (const auto &method : nodeObjDecl.methods) {
          for (const auto &param : method.params) {
            if (param.init != std::nullopt) {
              auto nodeObjDeclMethodParamResult = this->_getExprOfType<T>({ *param.init });
              result.insert(result.end(), nodeObjDeclMethodParamResult.begin(), nodeObjDeclMethodParamResult.end());
            }
          }

          if (method.body != std::nullopt) {
            auto nodeObjDeclMethodBodyResult = this->_getExprOfTypeFromNodes<T>(*method.body);
            result.insert(result.end(), nodeObjDeclMethodBodyResult.begin(), nodeObjDeclMethodBodyResult.end());
          }
        }
      } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
        auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

        if (nodeReturn.body != std::nullopt) {
          auto nodeReturnResult = this->_getExprOfType<T>({ *nodeReturn.body });
          result.insert(result.end(), nodeReturnResult.begin(), nodeReturnResult.end());
        }
      } else if (std::holds_alternative<ASTNodeThrow>(*node.body)) {
        auto nodeThrow = std::get<ASTNodeThrow>(*node.body);

        auto nodeThrowArgResult = this->_getExprOfType<T>({ nodeThrow.arg });
        result.insert(result.end(), nodeThrowArgResult.begin(), nodeThrowArgResult.end());
      } else if (std::holds_alternative<ASTNodeTry>(*node.body)) {
        auto nodeTry = std::get<ASTNodeTry>(*node.body);

        auto nodeTryBodyResult = this->_getExprOfTypeFromNodes<T>(nodeTry.body);
        result.insert(result.end(), nodeTryBodyResult.begin(), nodeTryBodyResult.end());

        for (const auto &handler : nodeTry.handlers) {
          auto nodeTryHandlerParamResult = this->_getExprOfTypeFromNodes<T>({ handler.param });
          result.insert(result.end(), nodeTryHandlerParamResult.begin(), nodeTryHandlerParamResult.end());
          auto nodeTryHandlerBodyResult = this->_getExprOfTypeFromNodes<T>(handler.body);
          result.insert(result.end(), nodeTryHandlerBodyResult.begin(), nodeTryHandlerBodyResult.end());
        }

        if (nodeTry.finalizer != std::nullopt) {
          auto nodeTryFinalizerResult = this->_getExprOfTypeFromNodes<T>(*nodeTry.finalizer);
          result.insert(result.end(), nodeTryFinalizerResult.begin(), nodeTryFinalizerResult.end());
        }
      } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
        auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);

        if (nodeVarDecl.init != std::nullopt) {
          auto nodeVarDeclInitResult = this->_getExprOfType<T>({ *nodeVarDecl.init });
          result.insert(result.end(), nodeVarDeclInitResult.begin(), nodeVarDeclInitResult.end());
        }
      }
    }

    return result;
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
  bool _insideOfNode (const std::vector<ASTNode> &nodes) const {
    if (nodes.empty()) {
      return false;
    }

    auto parent = nodes[0].parent;

    while (parent != nullptr) {
      if (std::holds_alternative<T>(*parent->body)) {
        return true;
      } else if (
        std::holds_alternative<ASTNodeEnumDecl>(*parent->body) ||
        std::holds_alternative<ASTNodeFnDecl>(*parent->body) ||
        std::holds_alternative<ASTNodeMain>(*parent->body) ||
        std::holds_alternative<ASTNodeObjDecl>(*parent->body)
      ) {
        return false;
      }

      parent = parent->parent;
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

  // todo test
  bool _throwsExpr (const std::vector<ASTNodeExpr> &exprs) const {
    for (const auto &nodeExpr : exprs) {
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
          this->_throwsExpr({ std::get<ASTNodeExpr>(*exprAccess.expr) })
        ) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprArray>(*nodeExpr.body)) {
        auto exprArray = std::get<ASTExprArray>(*nodeExpr.body);

        for (const auto &element : exprArray.elements) {
          if (this->_throwsExpr({ element })) {
            return true;
          }
        }
      } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
        auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);

        if (this->_throwsExpr({ exprAssign.left }) || this->_throwsExpr({ exprAssign.right })) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
        auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

        if (this->_throwsExpr({ exprBinary.left }) || this->_throwsExpr({ exprBinary.right })) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) {
        auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);

        if (std::get<TypeFn>(exprCall.callee.type->body).throws) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
        auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);

        if (
          this->_throwsExpr({ exprCond.cond }) ||
          this->_throwsExpr({ exprCond.body }) ||
          this->_throwsExpr({ exprCond.alt })
        ) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprIs>(*nodeExpr.body)) {
        auto exprIs = std::get<ASTExprIs>(*nodeExpr.body);

        if (this->_throwsExpr({ exprIs.expr })) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprMap>(*nodeExpr.body)) {
        auto exprMap = std::get<ASTExprMap>(*nodeExpr.body);

        for (const auto &prop : exprMap.props) {
          if (this->_throwsExpr({ prop.init })) {
            return true;
          }
        }
      } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
        auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);

        for (const auto &prop : exprObj.props) {
          if (this->_throwsExpr({ prop.init })) {
            return true;
          }
        }
      } else if (std::holds_alternative<ASTExprRef>(*nodeExpr.body)) {
        auto exprRef = std::get<ASTExprRef>(*nodeExpr.body);

        if (this->_throwsExpr({ exprRef.expr })) {
          return true;
        }
      } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
        auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);

        if (this->_throwsExpr({ exprUnary.arg })) {
          return true;
        }
      }
    }

    return false;
  }

  // todo test
  bool _throwsNode (const std::vector<ASTNode> &nodes) const {
    for (const auto &node : nodes) {
      if (std::holds_alternative<ASTNodeThrow>(*node.body) || std::holds_alternative<ASTNodeTry>(*node.body)) {
        return true;
      } else if (std::holds_alternative<ASTNodeEnumDecl>(*node.body)) {
        auto nodeEnumDecl = std::get<ASTNodeEnumDecl>(*node.body);

        for (const auto &member : nodeEnumDecl.members) {
          if (member.init != std::nullopt && this->_throwsExpr({ *member.init })) {
            return true;
          }
        }
      } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
        auto nodeExpr = std::get<ASTNodeExpr>(*node.body);

        if (this->_throwsExpr({ std::get<ASTNodeExpr>(*node.body) })) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

        if (nodeFnDecl.body != std::nullopt && this->_throwsNode(*nodeFnDecl.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
        auto nodeIf = std::get<ASTNodeIf>(*node.body);

        if (this->_throwsExpr({ nodeIf.cond }) || this->_throwsNode(nodeIf.body)) {
          return true;
        } else if (nodeIf.alt != std::nullopt) {
          if (std::holds_alternative<ASTBlock>(*nodeIf.alt) && this->_throwsNode(std::get<ASTBlock>(*nodeIf.alt))) {
            return true;
          } else if (std::holds_alternative<ASTNode>(*nodeIf.alt) && this->_throwsNode({ std::get<ASTNode>(*nodeIf.alt) })) {
            return true;
          }
        }
      } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
        auto nodeLoop = std::get<ASTNodeLoop>(*node.body);

        if (nodeLoop.init != std::nullopt && this->_throwsNode({ *nodeLoop.init })) {
          return true;
        } else if (nodeLoop.cond != std::nullopt && this->_throwsExpr({ *nodeLoop.cond })) {
          return true;
        } else if (nodeLoop.upd != std::nullopt && this->_throwsExpr({ *nodeLoop.upd })) {
          return true;
        } else if (this->_throwsNode(nodeLoop.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
        auto nodeMain = std::get<ASTNodeMain>(*node.body);

        if (this->_throwsNode(nodeMain.body)) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
        auto nodeReturn = std::get<ASTNodeReturn>(*node.body);

        if (nodeReturn.body != std::nullopt && this->_throwsExpr({ *nodeReturn.body })) {
          return true;
        }
      } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
        auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);

        if (nodeVarDecl.init != std::nullopt && this->_throwsExpr({ *nodeVarDecl.init })) {
          return true;
        }
      }
    }

    return false;
  }
};

#endif
