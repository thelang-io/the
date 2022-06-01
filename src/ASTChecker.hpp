/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_CHECKER_HPP
#define SRC_AST_CHECKER_HPP

#include "ASTNode.hpp"
#include "Error.hpp"

class ASTChecker {
 public:
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

 private:
  bool _isNode = false;
  std::vector<ASTNode> _nodes;

  void _checkNode () const {
    if (!this->_isNode) {
      throw Error("Error: Tried node method on non node");
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
  bool _isNodeMethod (const std::vector<ASTNode> &nodes) const {
    if (nodes.empty()) {
      return false;
    }

    return std::all_of(nodes.cbegin(), nodes.cend(), [] (const auto &node) {
      return std::holds_alternative<T>(*node.body);
    });
  }

  bool _isLastNode (const std::vector<ASTNode> &nodes) const {
    if (nodes.size() != 1) {
      throw Error("Error: Tried isLast on many nodes");
    } else if (nodes[0].parent == nullptr) {
      throw Error("Error: Tried isLast on root node");
    }

    auto node = nodes[0];
    auto parent = *node.parent;

    if (std::holds_alternative<ASTNodeFnDecl>(*parent.body)) {
      auto nodeFnDecl = std::get<ASTNodeFnDecl>(*parent.body);
      return nodeFnDecl.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeIf>(*parent.body)) {
      auto nodeIf = std::get<ASTNodeIf>(*parent.body);
      return nodeIf.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeLoop>(*parent.body)) {
      auto nodeLoop = std::get<ASTNodeLoop>(*parent.body);
      return nodeLoop.body.back().body == node.body;
    } else if (std::holds_alternative<ASTNodeMain>(*parent.body)) {
      auto nodeMain = std::get<ASTNodeMain>(*parent.body);
      return nodeMain.body.back().body == node.body;
    }

    return false;
  }

  template <typename T>
  bool _hasNode (const std::vector<ASTNode> &nodes) const {
    for (const auto &node : nodes) {
      if (std::holds_alternative<T>(*node.body)) {
        return true;
      } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
        auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

        if (this->_hasNode<T>(nodeFnDecl.body)) {
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
      }
    }

    return false;
  }
};

#endif
