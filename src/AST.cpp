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

#include "AST.hpp"
#include <regex>
#include "Parser.hpp"
#include "config.hpp"

bool isExprAccessLvalue (const ParserStmtExpr &stmtExpr) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);

    if (exprAccess.expr != std::nullopt && std::holds_alternative<Token>(*exprAccess.expr)) {
      return true;
    } else if (exprAccess.expr != std::nullopt && std::holds_alternative<ParserStmtExpr>(*exprAccess.expr)) {
      auto exprAccessExpr = std::get<ParserStmtExpr>(*exprAccess.expr);
      return exprAccess.prop != std::nullopt && isExprAccessLvalue(exprAccessExpr);
    }
  }

  return false;
}

std::string stringifyExprAccess (const ParserStmtExpr &stmtExpr) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);

    if (exprAccess.expr != std::nullopt && std::holds_alternative<Token>(*exprAccess.expr)) {
      auto tok = std::get<Token>(*exprAccess.expr);
      return tok.val;
    } else if (exprAccess.expr != std::nullopt && std::holds_alternative<ParserStmtExpr>(*exprAccess.expr)) {
      auto exprAccessExpr = std::get<ParserStmtExpr>(*exprAccess.expr);
      auto objCode = stringifyExprAccess(exprAccessExpr);

      if (exprAccess.prop != std::nullopt) {
        return objCode + "." + exprAccess.prop->val;
      }
    }
  }

  throw Error("Tried stringify non lvalue expr access");
}

void mergeTypeCasts (std::map<std::string, Type *> &m1, const std::map<std::string, Type *> &m2) {
  for (const auto &[key, val] : m2) {
    m1.insert_or_assign(key, val);
  }
}

void AST::populateParents (ASTBlock &nodes, ASTNode *parent) {
  for (auto &node : nodes) {
    AST::populateParent(node, parent);
  }
}

void AST::populateParent (ASTNode &node, ASTNode *parent) {
  node.parent = parent;

  if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
    auto &nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);

    if (nodeFnDecl.body != std::nullopt) {
      AST::populateParents(*nodeFnDecl.body, &node);
    }
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    auto &nodeIf = std::get<ASTNodeIf>(*node.body);
    AST::populateParents(nodeIf.body, &node);

    if (nodeIf.alt != std::nullopt && std::holds_alternative<ASTNode>(*nodeIf.alt)) {
      AST::populateParent(std::get<ASTNode>(*nodeIf.alt), &node);
    } else if (nodeIf.alt != std::nullopt && std::holds_alternative<ASTBlock>(*nodeIf.alt)) {
      AST::populateParents(std::get<ASTBlock>(*nodeIf.alt), &node);
    }
  } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
    auto &nodeLoop = std::get<ASTNodeLoop>(*node.body);
    AST::populateParents(nodeLoop.body, &node);

    if (nodeLoop.init != std::nullopt) {
      nodeLoop.init->parent = &node;
    }
  } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
    auto &nodeMain = std::get<ASTNodeMain>(*node.body);
    AST::populateParents(nodeMain.body, &node);
  } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
    auto &nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);

    for (auto &method : nodeObjDecl.methods) {
      if (method.body != std::nullopt) {
        AST::populateParents(*method.body, &node);
      }
    }
  }
}

AST::AST (Parser *p) {
  this->parser = p;
  this->reader = this->parser->reader;
  this->typeMap.init();
  this->varMap.init(this->typeMap);
}

ASTBlock AST::gen () {
  auto block = ParserBlock{};

  while (true) {
    auto stmt = this->parser->next();

    if (std::holds_alternative<ParserStmtEof>(*stmt.body)) {
      break;
    }

    block.push_back(stmt);
  }

  this->varMap.save();
  auto mainVarStack = VarStack({});
  auto result = this->_block(block, mainVarStack, true);
  AST::populateParents(result);
  this->varMap.restore();

  return result;
}

std::string AST::xml () {
  auto result = std::string();

  for (const auto &node : this->gen()) {
    result += node.xml() + EOL;
  }

  return result;
}

ASTBlock AST::_block (const ParserBlock &block, VarStack &varStack, bool root) {
  this->_forwardNode(block, AST_PHASE_ALLOC);
  this->_forwardNode(block, AST_PHASE_INIT);

  auto result = ASTBlock{};

  for (const auto &stmt : block) {
    if (std::holds_alternative<ParserStmtEmpty>(*stmt.body)) {
      continue;
    }

    auto node = this->_node(stmt, varStack);

    if (!root && std::holds_alternative<ASTNodeVarDecl>(*node.body) && std::get<ASTNodeVarDecl>(*node.body).var->constant) {
      throw Error(this->reader, stmt.start, stmt.end, E1025);
    } else if (
      root &&
      std::holds_alternative<ASTNodeVarDecl>(*node.body) &&
      !std::get<ASTNodeVarDecl>(*node.body).var->constant &&
      !std::get<ASTNodeVarDecl>(*node.body).var->mut
    ) {
      throw Error(this->reader, stmt.start, stmt.end, E1026);
    }

    result.push_back(node);
  }

  return result;
}

std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> AST::_evalTypeCasts (const ParserStmtExpr &stmtExpr) {
  auto bodyTypeCasts = std::map<std::string, Type *>{};
  auto altTypeCasts = std::map<std::string, Type *>{};

  if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body)) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);

    if (exprBinary.op.type == TK_OP_EQ_EQ || exprBinary.op.type == TK_OP_EXCL_EQ) {
      if (
        (std::holds_alternative<ParserExprAccess>(*exprBinary.left.body) && std::holds_alternative<ParserExprLit>(*exprBinary.right.body)) ||
        (std::holds_alternative<ParserExprAccess>(*exprBinary.right.body) && std::holds_alternative<ParserExprLit>(*exprBinary.left.body))
      ) {
        auto exprBinaryLeft = std::holds_alternative<ParserExprAccess>(*exprBinary.left.body) ? exprBinary.left : exprBinary.right;
        auto exprBinaryLeftType = Type::real(this->_nodeExprType(exprBinaryLeft, nullptr));
        auto exprBinaryRight = std::holds_alternative<ParserExprLit>(*exprBinary.right.body) ? exprBinary.right : exprBinary.left;
        auto exprBinaryRightLit = std::get<ParserExprLit>(*exprBinaryRight.body);

        if (isExprAccessLvalue(exprBinaryLeft) && exprBinaryLeftType->isOpt() && exprBinaryRightLit.body.type == TK_KW_NIL) {
          auto exprBinaryLeftAccessCode = stringifyExprAccess(exprBinaryLeft);
          auto exprBinaryLeftAccessTypeOpt = std::get<TypeOptional>(exprBinaryLeftType->body);

          if (exprBinary.op.type == TK_OP_EQ_EQ) {
            altTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          } else {
            bodyTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          }
        }
      } else if (
        (std::holds_alternative<ParserExprAssign>(*exprBinary.left.body) && std::holds_alternative<ParserExprLit>(*exprBinary.right.body)) ||
        (std::holds_alternative<ParserExprAssign>(*exprBinary.right.body) && std::holds_alternative<ParserExprLit>(*exprBinary.left.body))
      ) {
        auto exprBinaryLeft = std::holds_alternative<ParserExprAssign>(*exprBinary.left.body) ? exprBinary.left : exprBinary.right;
        auto exprBinaryLeftAssign = std::get<ParserExprAssign>(*exprBinaryLeft.body);
        auto exprBinaryLeftAssignType = Type::real(this->_nodeExprType(exprBinaryLeftAssign.left, nullptr));
        auto exprBinaryRight = std::holds_alternative<ParserExprLit>(*exprBinary.right.body) ? exprBinary.right : exprBinary.left;
        auto exprBinaryRightLit = std::get<ParserExprLit>(*exprBinaryRight.body);

        if (isExprAccessLvalue(exprBinaryLeftAssign.left) && exprBinaryLeftAssignType->isOpt() && exprBinaryRightLit.body.type == TK_KW_NIL) {
          auto exprBinaryLeftAccessCode = stringifyExprAccess(exprBinaryLeftAssign.left);
          auto exprBinaryLeftAccessTypeOpt = std::get<TypeOptional>(exprBinaryLeftAssignType->body);

          if (exprBinary.op.type == TK_OP_EQ_EQ) {
            altTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          } else {
            bodyTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          }
        }
      }
    }
  } else if (std::holds_alternative<ParserExprIs>(*stmtExpr.body)) {
    auto exprIs = std::get<ParserExprIs>(*stmtExpr.body);
    auto exprIsType = this->_type(exprIs.type);

    if (isExprAccessLvalue(exprIs.expr)) {
      auto exprAccessType = Type::real(this->_nodeExprType(exprIs.expr, nullptr));

      if (exprAccessType->isAny() || exprAccessType->isOpt() || exprAccessType->isUnion()) {
        auto exprAccessCode = stringifyExprAccess(exprIs.expr);
        bodyTypeCasts[exprAccessCode] = exprIsType;

        if (exprAccessType->isUnion()) {
          altTypeCasts[exprAccessCode] = this->typeMap.unionSub(exprAccessType, exprIsType);
        }
      }
    } else if (
      std::holds_alternative<ParserExprAssign>(*exprIs.expr.body) &&
      isExprAccessLvalue(std::get<ParserExprAssign>(*exprIs.expr.body).left)
    ) {
      auto exprAssign = std::get<ParserExprAssign>(*exprIs.expr.body);
      auto exprAccessType = Type::real(this->_nodeExprType(exprAssign.left, nullptr));

      if (exprAccessType->isAny() || exprAccessType->isOpt() || exprAccessType->isUnion()) {
        auto exprAccessCode = stringifyExprAccess(exprAssign.left);
        bodyTypeCasts[exprAccessCode] = exprIsType;

        if (exprAccessType->isUnion()) {
          altTypeCasts[exprAccessCode] = this->typeMap.unionSub(exprAccessType, exprIsType);
        }
      }
    }
  }

  return std::make_tuple(bodyTypeCasts, altTypeCasts);
}

void AST::_forwardNode (const ParserBlock &block, ASTPhase phase) {
  for (const auto &stmt : block) {
    if (std::holds_alternative<ParserStmtEnumDecl>(*stmt.body)) {
      auto stmtEnumDecl = std::get<ParserStmtEnumDecl>(*stmt.body);
      auto enumName = stmtEnumDecl.id.val;

      if (phase == AST_PHASE_ALLOC || phase == AST_PHASE_FULL) {
        auto enumMembers = std::vector<Type *>{};
        this->typeMap.stack.emplace_back(enumName);

        for (const auto &stmtEnumDeclMember : stmtEnumDecl.members) {
          enumMembers.push_back(this->typeMap.enumerator(stmtEnumDeclMember.id.val, this->typeMap.name(stmtEnumDeclMember.id.val)));
        }

        this->typeMap.stack.pop_back();
        auto enumCodeName = this->typeMap.name(enumName);
        auto enumType = this->typeMap.enumeration(enumName, enumCodeName, enumMembers);

        this->varMap.add(enumName, enumCodeName, enumType, false, false, true);
      }
    } else if (std::holds_alternative<ParserStmtFnDecl>(*stmt.body)) {
      auto stmtFnDecl = std::get<ParserStmtFnDecl>(*stmt.body);
      auto nodeFnDeclName = stmtFnDecl.id.val;

      if ((phase == AST_PHASE_ALLOC || phase == AST_PHASE_FULL) && !this->varMap.has(nodeFnDeclName)) {
        if (nodeFnDeclName == "main") {
          throw Error(this->reader, stmtFnDecl.id.start, stmtFnDecl.id.end, E1021);
        }

        auto nodeFnDeclVarParams = std::vector<TypeFnParam>{};
        this->varMap.save();

        for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
          auto paramName = stmtFnDeclParam.id.val;
          auto paramType = stmtFnDeclParam.type != std::nullopt
            ? this->_type(*stmtFnDeclParam.type)
            : this->_nodeExprType(*stmtFnDeclParam.init, nullptr);

          if (paramType->isVoid() && stmtFnDeclParam.type == std::nullopt) {
            throw Error(this->reader, stmtFnDeclParam.init->start, stmtFnDeclParam.init->end, E1022);
          } else if (paramType->isVoid()) {
            throw Error(this->reader, stmtFnDeclParam.type->start, stmtFnDeclParam.type->end, E1022);
          }

          auto paramVariadic = stmtFnDeclParam.variadic;
          auto paramRequired = stmtFnDeclParam.init == std::nullopt && !paramVariadic;

          if (paramVariadic) {
            paramType = this->typeMap.arrayOf(paramType);
          }

          this->varMap.add(paramName, this->varMap.name(paramName), paramType, stmtFnDeclParam.mut);
          nodeFnDeclVarParams.push_back(TypeFnParam{paramName, paramType, stmtFnDeclParam.mut, paramRequired, paramVariadic});
        }

        auto nodeFnDeclVarReturnType = stmtFnDecl.returnType == std::nullopt
          ? this->typeMap.get("void")
          : this->_type(*stmtFnDecl.returnType);

        this->varMap.restore();
        auto nodeFnDeclVarType = this->typeMap.fn(nodeFnDeclVarParams, nodeFnDeclVarReturnType);
        auto nodeFnDeclVarAliasType = this->typeMap.alias(nodeFnDeclName, nodeFnDeclVarType);

        this->varMap.add(nodeFnDeclName, nodeFnDeclVarAliasType->codeName, nodeFnDeclVarType);
      }
    } else if (std::holds_alternative<ParserStmtObjDecl>(*stmt.body)) {
      auto stmtObjDecl = std::get<ParserStmtObjDecl>(*stmt.body);
      auto objName = stmtObjDecl.id.val;

      if (phase == AST_PHASE_ALLOC || phase == AST_PHASE_FULL) {
        this->typeMap.obj(objName, this->typeMap.name(objName));
      }

      if (phase == AST_PHASE_INIT || phase == AST_PHASE_FULL) {
        auto type = this->typeMap.get(stmtObjDecl.id.val);
        auto initialTypeMapSelf = this->typeMap.self;

        this->typeMap.stack.emplace_back(type->name);
        this->typeMap.self = type;

        for (const auto &member : stmtObjDecl.members) {
          auto memberName = std::string();

          if (std::holds_alternative<ParserStmtFnDecl>(*member.body)) {
            memberName = std::get<ParserStmtFnDecl>(*member.body).id.val;
          } else if (std::holds_alternative<ParserStmtVarDecl>(*member.body)) {
            memberName = std::get<ParserStmtVarDecl>(*member.body).id.val;
          }

          if (!memberName.empty()) {
            auto it = std::find_if(type->fields.begin(), type->fields.end(), [&] (const auto &it2) -> bool {
              return it2.name == memberName;
            });

            if (it != type->fields.end()) {
              continue;
            }
          }

          if (std::holds_alternative<ParserStmtFnDecl>(*member.body)) {
            auto stmtFnDecl = std::get<ParserStmtFnDecl>(*member.body);
            auto methodDeclName = stmtFnDecl.id.val;
            auto methodDeclInfo = TypeFnMethodInfo{this->typeMap.name(methodDeclName)};
            auto methodDeclTypeParams = std::vector<TypeFnParam>{};

            this->varMap.save();

            for (auto i = static_cast<std::size_t>(0); i < stmtFnDecl.params.size(); i++) {
              auto stmtFnDeclParam = stmtFnDecl.params[i];
              auto paramType = stmtFnDeclParam.type != std::nullopt
                ? this->_type(*stmtFnDeclParam.type)
                : this->_nodeExprType(*stmtFnDeclParam.init, nullptr);

              if (paramType->isVoid() && stmtFnDeclParam.type == std::nullopt) {
                throw Error(this->reader, stmtFnDeclParam.init->start, stmtFnDeclParam.init->end, E1022);
              } else if (paramType->isVoid()) {
                throw Error(this->reader, stmtFnDeclParam.type->start, stmtFnDeclParam.type->end, E1022);
              }

              auto paramName = stmtFnDeclParam.id.val;
              auto paramCodeName = this->varMap.name(paramName);
              auto paramVariadic = stmtFnDeclParam.variadic;
              auto paramRequired = stmtFnDeclParam.init == std::nullopt && !paramVariadic;

              if (paramVariadic) {
                paramType = this->typeMap.arrayOf(paramType);
              }

              this->varMap.add(paramName, paramCodeName, paramType, stmtFnDeclParam.mut);

              if (i == 0 && this->typeMap.isSelf(paramType)) {
                methodDeclInfo.isSelfFirst = true;
                methodDeclInfo.selfCodeName = paramCodeName;
                methodDeclInfo.selfType = paramType;
                methodDeclInfo.isSelfMut = stmtFnDeclParam.mut;
              } else {
                methodDeclTypeParams.push_back(TypeFnParam{paramName, paramType, stmtFnDeclParam.mut, paramRequired, paramVariadic});
              }
            }

            auto methodDeclReturnType = stmtFnDecl.returnType == std::nullopt
              ? this->typeMap.get("void")
              : this->_type(*stmtFnDecl.returnType);
            auto methodDeclType = this->typeMap.fn(methodDeclTypeParams, methodDeclReturnType, methodDeclInfo);
            auto methodDeclAliasType = this->typeMap.alias(methodDeclName, methodDeclType);

            this->varMap.restore();
            this->varMap.add(type->name + "." + methodDeclName, methodDeclAliasType->codeName, methodDeclType);
            type->fields.push_back(TypeField{methodDeclName, methodDeclType, false, true, false});
          } else if (std::holds_alternative<ParserStmtVarDecl>(*member.body)) {
            auto stmtVarDecl = std::get<ParserStmtVarDecl>(*member.body);
            auto fieldType = this->_type(*stmtVarDecl.type);

            if (fieldType->isVoid()) {
              throw Error(this->reader, stmtVarDecl.type->start, stmtVarDecl.type->end, E1022);
            }

            type->fields.push_back(TypeField{stmtVarDecl.id.val, fieldType, stmtVarDecl.mut, false, false});
          }
        }

        this->typeMap.self = initialTypeMapSelf;
        this->typeMap.stack.pop_back();
      }
    } else if (std::holds_alternative<ParserStmtTypeDecl>(*stmt.body)) {
      auto stmtTypeDecl = std::get<ParserStmtTypeDecl>(*stmt.body);
      auto typeName = stmtTypeDecl.id.val;

      if (phase == AST_PHASE_ALLOC || phase == AST_PHASE_FULL) {
        auto type = this->_type(stmtTypeDecl.type);
        this->typeMap.alias(typeName, type);
      }
    }
  }
}

ASTNode AST::_node (const ParserStmt &stmt, VarStack &varStack) {
  if (std::holds_alternative<ParserStmtBreak>(*stmt.body)) {
    auto nodeBreak = ASTNodeBreak{};
    return this->_wrapNode(stmt, nodeBreak);
  } else if (std::holds_alternative<ParserStmtContinue>(*stmt.body)) {
    auto nodeContinue = ASTNodeContinue{};
    return this->_wrapNode(stmt, nodeContinue);
  } else if (std::holds_alternative<ParserStmtEnumDecl>(*stmt.body)) {
    auto stmtEnumDecl = std::get<ParserStmtEnumDecl>(*stmt.body);
    auto type = this->typeMap.get(stmtEnumDecl.id.val);
    auto nodeEnumDeclMembers = std::vector<ASTNodeEnumDeclMember>{};

    for (const auto &stmtEnumDeclMember : stmtEnumDecl.members) {
      auto memberInit = std::optional<ASTNodeExpr>{};

      if (stmtEnumDeclMember.init != std::nullopt) {
        memberInit = this->_nodeExpr(*stmtEnumDeclMember.init, this->typeMap.get("int"), varStack);
      }

      nodeEnumDeclMembers.push_back(ASTNodeEnumDeclMember{stmtEnumDeclMember.id.val, memberInit});
    }

    auto nodeEnumDecl = ASTNodeEnumDecl{type, nodeEnumDeclMembers};
    return this->_wrapNode(stmt, nodeEnumDecl);
  } else if (std::holds_alternative<ParserStmtExpr>(*stmt.body)) {
    auto stmtExpr = std::get<ParserStmtExpr>(*stmt.body);
    auto nodeExpr = this->_nodeExpr(stmtExpr, nullptr, varStack);

    return this->_wrapNode(stmt, nodeExpr);
  } else if (std::holds_alternative<ParserStmtFnDecl>(*stmt.body)) {
    auto initialState = this->state;
    auto stmtFnDecl = std::get<ParserStmtFnDecl>(*stmt.body);
    auto nodeFnDeclName = stmtFnDecl.id.val;
    auto nodeFnDeclVar = this->varMap.get(nodeFnDeclName);
    auto fnType = std::get<TypeFn>(nodeFnDeclVar->type->body);
    auto nodeFnDeclParams = std::vector<ASTFnDeclParam>{};
    auto nodeFnDeclVarStack = this->varMap.varStack();

    this->varMap.save();

    for (const auto &stmtFnDeclParam : stmtFnDecl.params) {
      auto paramName = stmtFnDeclParam.id.val;
      auto paramType = stmtFnDeclParam.type != std::nullopt
        ? this->_type(*stmtFnDeclParam.type)
        : this->_nodeExprType(*stmtFnDeclParam.init, nullptr);
      auto paramInit = stmtFnDeclParam.init == std::nullopt
        ? std::optional<ASTNodeExpr>{}
        : this->_nodeExpr(*stmtFnDeclParam.init, paramType, nodeFnDeclVarStack);

      if (stmtFnDeclParam.variadic) {
        paramType = this->typeMap.arrayOf(paramType);
      }

      auto paramVar = this->varMap.add(paramName, this->varMap.name(paramName), paramType, stmtFnDeclParam.mut);
      nodeFnDeclParams.push_back(ASTFnDeclParam{paramVar, paramInit});
    }

    this->typeMap.stack.emplace_back(nodeFnDeclVar->name);
    this->state.returnType = fnType.returnType;
    auto nodeFnDeclBody = stmtFnDecl.body == std::nullopt
      ? std::optional<ASTBlock>{}
      : this->_block(*stmtFnDecl.body, nodeFnDeclVarStack);
    this->varMap.restore();
    this->typeMap.stack.pop_back();

    auto nodeFnDeclStack = nodeFnDeclVarStack.snapshot();
    varStack.mark(nodeFnDeclStack);

    auto nodeFnDecl = ASTNodeFnDecl{nodeFnDeclVar, nodeFnDeclStack, nodeFnDeclParams, nodeFnDeclBody};
    this->state = initialState;

    return this->_wrapNode(stmt, nodeFnDecl);
  } else if (std::holds_alternative<ParserStmtIf>(*stmt.body)) {
    auto stmtIf = std::get<ParserStmtIf>(*stmt.body);
    auto initialTypeCasts = this->typeCasts;
    auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(stmtIf.cond);
    auto nodeLoopCond = this->_nodeExpr(stmtIf.cond, this->typeMap.get("bool"), varStack);

    this->varMap.save();

    mergeTypeCasts(this->typeCasts, bodyTypeCasts);
    auto nodeLoopBody = this->_block(stmtIf.body, varStack);
    this->typeCasts = initialTypeCasts;
    auto nodeLoopAlt = std::optional<std::variant<ASTBlock, ASTNode>>{};
    this->varMap.restore();

    if (stmtIf.alt != std::nullopt) {
      mergeTypeCasts(this->typeCasts, altTypeCasts);

      if (std::holds_alternative<ParserBlock>(*stmtIf.alt)) {
        this->varMap.save();
        nodeLoopAlt = this->_block(std::get<ParserBlock>(*stmtIf.alt), varStack);
        this->varMap.restore();
      } else if (std::holds_alternative<ParserStmt>(*stmtIf.alt)) {
        nodeLoopAlt = this->_node(std::get<ParserStmt>(*stmtIf.alt), varStack);
      }

      this->typeCasts = initialTypeCasts;
    }

    return this->_wrapNode(stmt, ASTNodeIf{nodeLoopCond, nodeLoopBody, nodeLoopAlt});
  } else if (std::holds_alternative<ParserStmtLoop>(*stmt.body)) {
    auto stmtLoop = std::get<ParserStmtLoop>(*stmt.body);
    auto nodeLoopInit = std::optional<ASTNode>{};
    auto nodeLoopCond = std::optional<ASTNodeExpr>{};
    auto nodeLoopUpd = std::optional<ASTNodeExpr>{};

    this->varMap.save();

    if (stmtLoop.init != std::nullopt) {
      auto initialState = this->state;

      this->state.insideLoopInit = true;
      nodeLoopInit = this->_node(*stmtLoop.init, varStack);
      this->state = initialState;
    }

    if (stmtLoop.cond != std::nullopt) {
      nodeLoopCond = this->_nodeExpr(*stmtLoop.cond, this->typeMap.get("bool"), varStack);
    }

    if (stmtLoop.upd != std::nullopt) {
      nodeLoopUpd = this->_nodeExpr(*stmtLoop.upd, nullptr, varStack);
    }

    auto nodeLoopBody = this->_block(stmtLoop.body, varStack);
    this->varMap.restore();

    auto nodeLoop = ASTNodeLoop{nodeLoopInit, nodeLoopCond, nodeLoopUpd, nodeLoopBody};
    return this->_wrapNode(stmt, nodeLoop);
  } else if (std::holds_alternative<ParserStmtMain>(*stmt.body)) {
    auto stmtMain = std::get<ParserStmtMain>(*stmt.body);

    this->typeMap.stack.emplace_back("main");
    this->varMap.save();

    auto nodeMainBody = this->_block(stmtMain.body, varStack);

    this->varMap.restore();
    this->typeMap.stack.pop_back();

    auto nodeMain = ASTNodeMain{nodeMainBody};
    return this->_wrapNode(stmt, nodeMain);
  } else if (std::holds_alternative<ParserStmtObjDecl>(*stmt.body)) {
    auto stmtObjDecl = std::get<ParserStmtObjDecl>(*stmt.body);
    auto type = this->typeMap.get(stmtObjDecl.id.val);
    auto initialTypeMapSelf = this->typeMap.self;
    auto nodeObjDeclMethods = std::vector<ASTNodeObjDeclMethod>{};

    this->typeMap.stack.emplace_back(type->name);
    this->typeMap.self = type;

    for (const auto &stmtObjDeclMethod : stmtObjDecl.members) {
      if (!std::holds_alternative<ParserStmtFnDecl>(*stmtObjDeclMethod.body)) {
        continue;
      }

      auto stmtFnDecl = std::get<ParserStmtFnDecl>(*stmtObjDeclMethod.body);
      auto initialState = this->state;

      auto methodDeclName = stmtFnDecl.id.val;
      auto methodDeclVar = this->varMap.get(type->name + "." + methodDeclName);
      auto methodDeclParams = std::vector<ASTFnDeclParam>{};
      auto methodDeclVarStack = this->varMap.varStack();

      this->varMap.save();

      for (auto i = static_cast<std::size_t>(0); i < stmtFnDecl.params.size(); i++) {
        auto stmtFnDeclParam = stmtFnDecl.params[i];
        auto paramName = stmtFnDeclParam.id.val;
        auto paramType = stmtFnDeclParam.type != std::nullopt
          ? this->_type(*stmtFnDeclParam.type)
          : this->_nodeExprType(*stmtFnDeclParam.init, nullptr);
        auto paramInit = stmtFnDeclParam.init == std::nullopt
          ? std::optional<ASTNodeExpr>{}
          : this->_nodeExpr(*stmtFnDeclParam.init, paramType, methodDeclVarStack);

        if (stmtFnDeclParam.variadic) {
          paramType = this->typeMap.arrayOf(paramType);
        }

        auto paramVar = this->varMap.add(paramName, this->varMap.name(paramName), paramType, stmtFnDeclParam.mut);

        if (i != 0 || !this->typeMap.isSelf(paramType)) {
          methodDeclParams.push_back(ASTFnDeclParam{paramVar, paramInit});
        }
      }

      this->typeMap.stack.emplace_back(methodDeclName);
      this->state.returnType = stmtFnDecl.returnType == std::nullopt
        ? this->typeMap.get("void")
        : this->_type(*stmtFnDecl.returnType);
      auto methodDeclBody = stmtFnDecl.body == std::nullopt
        ? std::optional<ASTBlock>{}
        : this->_block(*stmtFnDecl.body, methodDeclVarStack);
      this->varMap.restore();
      this->typeMap.stack.pop_back();

      auto methodDeclStack = methodDeclVarStack.snapshot();
      varStack.mark(methodDeclStack);

      auto methodDeclMethod = ASTNodeObjDeclMethod{methodDeclVar, methodDeclStack, methodDeclParams, methodDeclBody};
      nodeObjDeclMethods.emplace_back(methodDeclMethod);

      this->state = initialState;
    }

    this->typeMap.self = initialTypeMapSelf;
    this->typeMap.stack.pop_back();

    auto nodeObjDecl = ASTNodeObjDecl{type, nodeObjDeclMethods};
    return this->_wrapNode(stmt, nodeObjDecl);
  } else if (std::holds_alternative<ParserStmtReturn>(*stmt.body)) {
    auto stmtReturn = std::get<ParserStmtReturn>(*stmt.body);
    auto nodeReturnBody = std::optional<ASTNodeExpr>{};

    if (stmtReturn.body != std::nullopt) {
      nodeReturnBody = this->_nodeExpr(*stmtReturn.body, this->state.returnType, varStack);
    }

    auto nodeReturn = ASTNodeReturn{nodeReturnBody};
    return this->_wrapNode(stmt, nodeReturn);
  } else if (std::holds_alternative<ParserStmtTypeDecl>(*stmt.body)) {
    auto stmtTypeDecl = std::get<ParserStmtTypeDecl>(*stmt.body);
    auto type = this->typeMap.get(stmtTypeDecl.id.val);
    auto nodeUnionDecl = ASTNodeTypeDecl{type};

    return this->_wrapNode(stmt, nodeUnionDecl);
  } else if (std::holds_alternative<ParserStmtVarDecl>(*stmt.body)) {
    auto stmtVarDecl = std::get<ParserStmtVarDecl>(*stmt.body);
    auto nodeVarDeclName = stmtVarDecl.id.val;
    auto nodeVarDeclType = stmtVarDecl.type != std::nullopt
      ? this->_type(*stmtVarDecl.type)
      : this->_nodeExprType(*stmtVarDecl.init, nullptr);

    if (nodeVarDeclType != nullptr && nodeVarDeclType->isVoid() && stmtVarDecl.type == std::nullopt) {
      throw Error(this->reader, stmtVarDecl.init->start, stmtVarDecl.init->end, E1022);
    } else if (nodeVarDeclType != nullptr && nodeVarDeclType->isVoid()) {
      throw Error(this->reader, stmtVarDecl.type->start, stmtVarDecl.type->end, E1022);
    }

    auto nodeVarDeclInit = std::optional<ASTNodeExpr>{};

    if (stmtVarDecl.init != std::nullopt) {
      nodeVarDeclInit = this->_nodeExpr(*stmtVarDecl.init, nodeVarDeclType, varStack);
    }

    auto nodeVarDeclVar = this->varMap.add(
      nodeVarDeclName,
      this->varMap.name(nodeVarDeclName),
      nodeVarDeclType,
      this->state.insideLoopInit || stmtVarDecl.mut,
      stmtVarDecl.constant
    );

    auto nodeVarDecl = ASTNodeVarDecl{nodeVarDeclVar, nodeVarDeclInit};
    return this->_wrapNode(stmt, nodeVarDecl);
  }

  throw Error("tried to analyze unknown statement");
}

ASTNodeExpr AST::_nodeExpr (const ParserStmtExpr &stmtExpr, Type *targetType, VarStack &varStack) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto parserExprAccess = std::get<ParserExprAccess>(*stmtExpr.body);

    if (parserExprAccess.expr != std::nullopt && std::holds_alternative<Token>(*parserExprAccess.expr)) {
      auto tok = std::get<Token>(*parserExprAccess.expr);
      auto var = this->varMap.get(tok.val);

      if (var == nullptr) {
        throw Error(this->reader, tok.start, tok.end, E1011);
      }

      varStack.mark(var);
      return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprAccess{var, std::nullopt, std::nullopt});
    } else if (parserExprAccess.expr != std::nullopt && std::holds_alternative<ParserStmtExpr>(*parserExprAccess.expr)) {
      auto exprAccessStmtExpr = std::get<ParserStmtExpr>(*parserExprAccess.expr);
      auto exprAccessExpr = ASTNodeExpr{};
      auto initializedExprAccessExpr = false;

      if (parserExprAccess.prop != std::nullopt) {
        auto exprAccessVarStack = VarStack({});
        exprAccessExpr = this->_nodeExpr(exprAccessStmtExpr, nullptr, exprAccessVarStack);

        if (
          exprAccessExpr.type->hasProp(parserExprAccess.prop->val) &&
          exprAccessExpr.type->getProp(parserExprAccess.prop->val)->isMethod()
        ) {
          auto propType = exprAccessExpr.type->getProp(parserExprAccess.prop->val);
          auto propMethod = std::get<TypeFn>(propType->body).methodInfo;

          varStack.mark(propMethod.codeName);
          initializedExprAccessExpr = true;
        }
      }

      if (!initializedExprAccessExpr) {
        exprAccessExpr = this->_nodeExpr(exprAccessStmtExpr, nullptr, varStack);
      }

      if (parserExprAccess.prop != std::nullopt) {
        return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprAccess{exprAccessExpr, std::nullopt, parserExprAccess.prop->val});
      } else if (parserExprAccess.elem != std::nullopt) {
        auto exprAccessElem = this->_nodeExpr(*parserExprAccess.elem, nullptr, varStack);
        return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprAccess{exprAccessExpr, exprAccessElem, std::nullopt});
      }
    } else if (parserExprAccess.expr == std::nullopt && parserExprAccess.prop != std::nullopt) {
      return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprAccess{std::nullopt, std::nullopt, parserExprAccess.prop->val});
    }
  } else if (std::holds_alternative<ParserExprArray>(*stmtExpr.body)) {
    auto parserExprArray = std::get<ParserExprArray>(*stmtExpr.body);
    auto exprArrayElements = std::vector<ASTNodeExpr>{};
    auto elementsType = targetType == nullptr || !targetType->isArray()
      ? nullptr
      : std::get<TypeArray>(targetType->body).elementType;

    for (const auto &element : parserExprArray.elements) {
      auto elementNode = this->_nodeExpr(element, elementsType, varStack);
      exprArrayElements.push_back(elementNode);

      if (elementsType == nullptr) {
        elementsType = elementNode.type;
      }
    }

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprArray{exprArrayElements});
  } else if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body)) {
    auto parserExprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto exprAssignOp = ASTExprAssignOp{};

    if (parserExprAssign.op.type == TK_OP_AMP_EQ) exprAssignOp = AST_EXPR_ASSIGN_BIT_AND;
    else if (parserExprAssign.op.type == TK_OP_AMP_AMP_EQ) exprAssignOp = AST_EXPR_ASSIGN_AND;
    else if (parserExprAssign.op.type == TK_OP_CARET_EQ) exprAssignOp = AST_EXPR_ASSIGN_BIT_XOR;
    else if (parserExprAssign.op.type == TK_OP_EQ) exprAssignOp = AST_EXPR_ASSIGN_EQ;
    else if (parserExprAssign.op.type == TK_OP_LSHIFT_EQ) exprAssignOp = AST_EXPR_ASSIGN_LSHIFT;
    else if (parserExprAssign.op.type == TK_OP_MINUS_EQ) exprAssignOp = AST_EXPR_ASSIGN_SUB;
    else if (parserExprAssign.op.type == TK_OP_PERCENT_EQ) exprAssignOp = AST_EXPR_ASSIGN_MOD;
    else if (parserExprAssign.op.type == TK_OP_PIPE_EQ) exprAssignOp = AST_EXPR_ASSIGN_BIT_OR;
    else if (parserExprAssign.op.type == TK_OP_PIPE_PIPE_EQ) exprAssignOp = AST_EXPR_ASSIGN_OR;
    else if (parserExprAssign.op.type == TK_OP_PLUS_EQ) exprAssignOp = AST_EXPR_ASSIGN_ADD;
    else if (parserExprAssign.op.type == TK_OP_RSHIFT_EQ) exprAssignOp = AST_EXPR_ASSIGN_RSHIFT;
    else if (parserExprAssign.op.type == TK_OP_SLASH_EQ) exprAssignOp = AST_EXPR_ASSIGN_DIV;
    else if (parserExprAssign.op.type == TK_OP_STAR_EQ) exprAssignOp = AST_EXPR_ASSIGN_MUL;

    auto exprAssignLeft = this->_nodeExpr(parserExprAssign.left, nullptr, varStack);
    auto exprAssignRight = this->_nodeExpr(parserExprAssign.right, exprAssignLeft.type, varStack);

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprAssign{exprAssignLeft, exprAssignOp, exprAssignRight});
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body)) {
    auto parserExprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto exprBinaryOp = ASTExprBinaryOp{};

    if (parserExprBinary.op.type == TK_OP_AMP) exprBinaryOp = AST_EXPR_BINARY_BIT_AND;
    else if (parserExprBinary.op.type == TK_OP_AMP_AMP) exprBinaryOp = AST_EXPR_BINARY_AND;
    else if (parserExprBinary.op.type == TK_OP_CARET) exprBinaryOp = AST_EXPR_BINARY_BIT_XOR;
    else if (parserExprBinary.op.type == TK_OP_EQ_EQ) exprBinaryOp = AST_EXPR_BINARY_EQ;
    else if (parserExprBinary.op.type == TK_OP_EXCL_EQ) exprBinaryOp = AST_EXPR_BINARY_NE;
    else if (parserExprBinary.op.type == TK_OP_GT) exprBinaryOp = AST_EXPR_BINARY_GT;
    else if (parserExprBinary.op.type == TK_OP_GT_EQ) exprBinaryOp = AST_EXPR_BINARY_GE;
    else if (parserExprBinary.op.type == TK_OP_LSHIFT) exprBinaryOp = AST_EXPR_BINARY_LSHIFT;
    else if (parserExprBinary.op.type == TK_OP_LT) exprBinaryOp = AST_EXPR_BINARY_LT;
    else if (parserExprBinary.op.type == TK_OP_LT_EQ) exprBinaryOp = AST_EXPR_BINARY_LE;
    else if (parserExprBinary.op.type == TK_OP_MINUS) exprBinaryOp = AST_EXPR_BINARY_SUB;
    else if (parserExprBinary.op.type == TK_OP_PERCENT) exprBinaryOp = AST_EXPR_BINARY_MOD;
    else if (parserExprBinary.op.type == TK_OP_PIPE) exprBinaryOp = AST_EXPR_BINARY_BIT_OR;
    else if (parserExprBinary.op.type == TK_OP_PIPE_PIPE) exprBinaryOp = AST_EXPR_BINARY_OR;
    else if (parserExprBinary.op.type == TK_OP_PLUS) exprBinaryOp = AST_EXPR_BINARY_ADD;
    else if (parserExprBinary.op.type == TK_OP_RSHIFT) exprBinaryOp = AST_EXPR_BINARY_RSHIFT;
    else if (parserExprBinary.op.type == TK_OP_SLASH) exprBinaryOp = AST_EXPR_BINARY_DIV;
    else if (parserExprBinary.op.type == TK_OP_STAR) exprBinaryOp = AST_EXPR_BINARY_MUL;

    auto exprBinaryOperandsType = static_cast<Type *>(nullptr);

    try {
      exprBinaryOperandsType = this->_nodeExprType(parserExprBinary.left, nullptr);
    } catch (const Error &) {
      exprBinaryOperandsType = this->_nodeExprType(parserExprBinary.right, nullptr);
    }

    auto exprBinaryLeft = this->_nodeExpr(parserExprBinary.left, exprBinaryOperandsType, varStack);
    auto exprBinaryRight = this->_nodeExpr(parserExprBinary.right, exprBinaryOperandsType, varStack);

    if (
      (exprBinaryLeft.type->isStr() || exprBinaryRight.type->isStr()) &&
      (parserExprBinary.op.type != TK_OP_PLUS && parserExprBinary.op.type != TK_OP_EQ_EQ && parserExprBinary.op.type != TK_OP_EXCL_EQ)
    ) {
      throw Error(this->reader, parserExprBinary.left.start, parserExprBinary.right.end, E1003);
    }

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprBinary{exprBinaryLeft, exprBinaryOp, exprBinaryRight});
  } else if (std::holds_alternative<ParserExprCall>(*stmtExpr.body)) {
    auto parserExprCall = std::get<ParserExprCall>(*stmtExpr.body);
    auto exprCallCallee = this->_nodeExpr(parserExprCall.callee, nullptr, varStack);
    auto exprCallCalleeRealType = Type::real(exprCallCallee.type);

    if (!exprCallCalleeRealType->isFn()) {
      throw Error(this->reader, parserExprCall.callee.start, parserExprCall.callee.end, E1014);
    }

    auto exprCallCalleeFnType = std::get<TypeFn>(exprCallCalleeRealType->body);
    auto exprCallArgs = std::vector<ASTExprCallArg>{};
    auto exprCallArgIdx = static_cast<std::size_t>(0);
    auto passedArgs = std::vector<std::string>{};
    auto hasNamedArgs = false;
    auto isArgVariadic = false;
    auto variadicArgType = std::optional<TypeFnParam>{};

    for (const auto &parserExprCallArg : parserExprCall.args) {
      auto foundParam = std::optional<TypeFnParam>{};

      if (parserExprCallArg.id != std::nullopt) {
        auto exprCallArgName = parserExprCallArg.id->val;

        if (std::find(passedArgs.begin(), passedArgs.end(), exprCallArgName) != passedArgs.end()) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr.end, E1005);
        }

        for (auto &calleeFnParam : exprCallCalleeFnType.params) {
          if (calleeFnParam.name != std::nullopt && calleeFnParam.name == exprCallArgName) {
            foundParam = calleeFnParam;
            break;
          }
        }

        if (foundParam == std::nullopt) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr.end, E1002);
        } else if (foundParam->variadic) {
          throw Error(this->reader, parserExprCallArg.id->start, parserExprCallArg.expr.end, E1006);
        }

        hasNamedArgs = true;
        isArgVariadic = false;
        variadicArgType = std::nullopt;
      } else if (hasNamedArgs) {
        throw Error(this->reader, parserExprCallArg.expr.start, parserExprCallArg.expr.end, E1007);
      } else if (isArgVariadic) {
        foundParam = variadicArgType;
      } else if (exprCallArgIdx >= exprCallCalleeFnType.params.size()) {
        throw Error(this->reader, parserExprCallArg.expr.start, parserExprCallArg.expr.end, E1005);
      } else {
        foundParam = exprCallCalleeFnType.params[exprCallArgIdx];
      }

      if (!isArgVariadic && foundParam->variadic) {
        isArgVariadic = true;
        variadicArgType = foundParam;
      }

      auto foundParamType = foundParam->variadic ? std::get<TypeArray>(foundParam->type->body).elementType : foundParam->type;
      auto exprCallArgExpr = this->_nodeExpr(parserExprCallArg.expr, foundParamType, varStack);

      if (!foundParamType->matchNice(exprCallArgExpr.type)) {
        throw Error(this->reader, parserExprCallArg.expr.start, parserExprCallArg.expr.end, E1008);
      }

      exprCallArgs.push_back(ASTExprCallArg{foundParam->name, exprCallArgExpr});

      if (!isArgVariadic) {
        if (foundParam->name != std::nullopt) {
          passedArgs.push_back(*foundParam->name);
        }

        exprCallArgIdx++;
      }
    }

    if (exprCallArgIdx < exprCallCalleeFnType.params.size()) {
      auto param = exprCallCalleeFnType.params[exprCallArgIdx];

      if (param.required) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1009);
      }
    }

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprCall{exprCallCallee, exprCallArgs});
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body)) {
    auto parserExprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto initialTypeCasts = this->typeCasts;
    auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(parserExprCond.cond);
    auto exprCondCond = this->_nodeExpr(parserExprCond.cond, this->typeMap.get("bool"), varStack);
    auto exprCondOperandsType = this->_nodeExprType(stmtExpr, targetType);

    mergeTypeCasts(this->typeCasts, bodyTypeCasts);
    auto exprCondBody = this->_nodeExpr(parserExprCond.body, exprCondOperandsType, varStack);
    this->typeCasts = initialTypeCasts;
    mergeTypeCasts(this->typeCasts, altTypeCasts);
    auto exprCondAlt = this->_nodeExpr(parserExprCond.alt, exprCondOperandsType, varStack);
    this->typeCasts = initialTypeCasts;

    if (!exprCondBody.type->matchNice(exprCondAlt.type) && !exprCondAlt.type->matchNice(exprCondBody.type)) {
      throw Error(this->reader, parserExprCond.body.start, parserExprCond.alt.end, E1004);
    }

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprCond{exprCondCond, exprCondBody, exprCondAlt});
  } else if (std::holds_alternative<ParserExprIs>(*stmtExpr.body)) {
    auto parserExprIs = std::get<ParserExprIs>(*stmtExpr.body);
    auto exprIsType = this->_type(parserExprIs.type);
    auto exprIsExpr = this->_nodeExpr(parserExprIs.expr, exprIsType, varStack);

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprIs{exprIsExpr, exprIsType});
  } else if (std::holds_alternative<ParserExprLit>(*stmtExpr.body)) {
    auto parserExprLit = std::get<ParserExprLit>(*stmtExpr.body);
    auto exprLitType = ASTExprLitType{};

    if (parserExprLit.body.type == TK_KW_FALSE || parserExprLit.body.type == TK_KW_TRUE) exprLitType = AST_EXPR_LIT_BOOL;
    else if (parserExprLit.body.type == TK_KW_NIL) exprLitType = AST_EXPR_LIT_NIL;
    else if (parserExprLit.body.type == TK_LIT_CHAR) exprLitType = AST_EXPR_LIT_CHAR;
    else if (parserExprLit.body.type == TK_LIT_FLOAT) exprLitType = AST_EXPR_LIT_FLOAT;
    else if (parserExprLit.body.type == TK_LIT_INT_BIN) exprLitType = AST_EXPR_LIT_INT_BIN;
    else if (parserExprLit.body.type == TK_LIT_INT_DEC) exprLitType = AST_EXPR_LIT_INT_DEC;
    else if (parserExprLit.body.type == TK_LIT_INT_HEX) exprLitType = AST_EXPR_LIT_INT_HEX;
    else if (parserExprLit.body.type == TK_LIT_INT_OCT) exprLitType = AST_EXPR_LIT_INT_OCT;
    else if (parserExprLit.body.type == TK_LIT_STR) exprLitType = AST_EXPR_LIT_STR;

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprLit{exprLitType, parserExprLit.body.val});
  } else if (std::holds_alternative<ParserExprMap>(*stmtExpr.body)) {
    auto parserExprMap = std::get<ParserExprMap>(*stmtExpr.body);
    auto nodeExprType = this->_nodeExprType(stmtExpr, targetType);
    auto valueType = std::get<TypeBodyMap>(nodeExprType->body).valueType;
    auto exprMapProps = std::vector<ASTObjProp>{};

    for (const auto &parserProp : parserExprMap.props) {
      auto propName = std::get<ParserExprLit>(*parserProp.name.body).body.val;
      auto propInit = this->_nodeExpr(parserProp.init, valueType, varStack);

      exprMapProps.push_back(ASTObjProp{propName.substr(1, propName.size() - 2), propInit});
    }

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprMap{exprMapProps});
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr.body)) {
    auto parserExprObj = std::get<ParserExprObj>(*stmtExpr.body);
    auto type = this->typeMap.get(parserExprObj.id.val);

    if (type == nullptr) {
      throw Error(this->reader, parserExprObj.id.start, parserExprObj.id.end, E1010);
    }

    auto exprObjProps = std::vector<ASTObjProp>{};

    for (const auto &parserExprObjProp : parserExprObj.props) {
      if (!type->hasProp(parserExprObjProp.id.val)) {
        throw Error(this->reader, parserExprObjProp.id.start, parserExprObjProp.init.end, E1015);
      }

      auto propType = type->getProp(parserExprObjProp.id.val);

      exprObjProps.push_back(ASTObjProp{
        parserExprObjProp.id.val,
        this->_nodeExpr(parserExprObjProp.init, propType, varStack)
      });
    }

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprObj{exprObjProps});
  } else if (std::holds_alternative<ParserExprRef>(*stmtExpr.body)) {
    auto parserExprRef = std::get<ParserExprRef>(*stmtExpr.body);
    auto exprRefExpr = this->_nodeExpr(parserExprRef.expr, nullptr, varStack);

    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprRef{exprRefExpr});
  } else if (std::holds_alternative<ParserExprUnary>(*stmtExpr.body)) {
    auto parserExprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto op = ASTExprUnaryOp{};

    if (parserExprUnary.op.type == TK_OP_EXCL) op = AST_EXPR_UNARY_NOT;
    else if (parserExprUnary.op.type == TK_OP_MINUS) op = AST_EXPR_UNARY_MINUS;
    else if (parserExprUnary.op.type == TK_OP_MINUS_MINUS) op = AST_EXPR_UNARY_DECREMENT;
    else if (parserExprUnary.op.type == TK_OP_PLUS) op = AST_EXPR_UNARY_PLUS;
    else if (parserExprUnary.op.type == TK_OP_PLUS_PLUS) op = AST_EXPR_UNARY_INCREMENT;
    else if (parserExprUnary.op.type == TK_OP_TILDE) op = AST_EXPR_UNARY_BIT_NOT;

    auto exprUnaryArg = this->_nodeExpr(parserExprUnary.arg, nullptr, varStack);
    return this->_wrapNodeExpr(stmtExpr, targetType, ASTExprUnary{exprUnaryArg, op, parserExprUnary.prefix});
  }

  throw Error("tried to analyze unknown expression");
}

Type *AST::_nodeExprType (const ParserStmtExpr &stmtExpr, Type *targetType) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);

    if (exprAccess.expr != std::nullopt && std::holds_alternative<Token>(*exprAccess.expr)) {
      auto tok = std::get<Token>(*exprAccess.expr);

      if (!this->varMap.has(tok.val)) {
        throw Error(this->reader, tok.start, tok.end, E1011);
      }

      auto type = this->varMap.get(tok.val)->type;

      if (isExprAccessLvalue(stmtExpr)) {
        auto lvalueCode = stringifyExprAccess(stmtExpr);

        if (this->typeCasts.contains(lvalueCode)) {
          type = this->typeCasts[lvalueCode];
        }
      }

      return this->_wrapNodeExprType(stmtExpr, targetType, type);
    } else if (exprAccess.expr != std::nullopt && std::holds_alternative<ParserStmtExpr>(*exprAccess.expr)) {
      auto exprAccessStmtExpr = std::get<ParserStmtExpr>(*exprAccess.expr);
      auto objType = this->_nodeExprType(exprAccessStmtExpr, nullptr);
      auto objRealType = Type::real(objType);

      if (exprAccess.prop != std::nullopt) {
        auto type = static_cast<Type *>(nullptr);

        if (objType->isEnum()) {
          if (objType->hasEnumerator(exprAccess.prop->val)) {
            type = objType;
          } else if (objType->hasProp(exprAccess.prop->val)) {
            type = objType->getProp(exprAccess.prop->val);
          } else {
            throw Error(this->reader, exprAccess.prop->start, exprAccess.prop->end, E1024);
          }
        } else if (!objType->hasProp(exprAccess.prop->val)) {
          throw Error(this->reader, exprAccess.prop->start, exprAccess.prop->end, E1001);
        } else {
          type = objType->getProp(exprAccess.prop->val);
        }

        if (isExprAccessLvalue(stmtExpr)) {
          auto lvalueCode = stringifyExprAccess(stmtExpr);

          if (this->typeCasts.contains(lvalueCode)) {
            type = this->typeCasts[lvalueCode];
          }
        }

        return this->_wrapNodeExprType(stmtExpr, targetType, type);
      } else {
        auto exprAccessElemType = this->_nodeExprType(*exprAccess.elem, nullptr);

        if (!exprAccessElemType->isIntNumber()) {
          throw Error(this->reader, exprAccess.elem->start, exprAccess.elem->end, E1012);
        }

        if (objRealType->isArray()) {
          auto arrayType = std::get<TypeArray>(objRealType->body);
          return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.ref(arrayType.elementType));
        } else if (objRealType->isStr()) {
          return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.ref(this->typeMap.get("char")));
        }

        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1013);
      }
    } else if (exprAccess.expr == std::nullopt && exprAccess.prop != std::nullopt) {
      auto realTargetType = targetType == nullptr ? targetType : Type::real(targetType);

      if (realTargetType == nullptr || !realTargetType->isEnum()) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1023);
      } else if (!realTargetType->hasEnumerator(exprAccess.prop->val)) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1024);
      }

      return this->_wrapNodeExprType(stmtExpr, targetType, realTargetType);
    }
  } else if (std::holds_alternative<ParserExprArray>(*stmtExpr.body)) {
    auto actualTargetType = targetType == nullptr ? static_cast<Type *>(nullptr) : Type::actual(targetType);
    auto realTargetType = static_cast<Type *>(nullptr);

    if (actualTargetType != nullptr && actualTargetType->isArray()) {
      realTargetType = actualTargetType;
    } else if (actualTargetType != nullptr && actualTargetType->isOpt() && std::get<TypeOptional>(actualTargetType->body).type->isArray()) {
      realTargetType = std::get<TypeOptional>(actualTargetType->body).type;
    }

    auto exprArray = std::get<ParserExprArray>(*stmtExpr.body);

    if (exprArray.elements.empty()) {
      if (realTargetType == nullptr) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1016);
      }

      return this->_wrapNodeExprType(stmtExpr, targetType, realTargetType);
    }

    auto elementsType = realTargetType == nullptr
      ? static_cast<Type *>(nullptr)
      : std::get<TypeArray>(realTargetType->body).elementType;

    for (const auto &element : exprArray.elements) {
      auto elementExprType = this->_nodeExprType(element, elementsType);

      if (elementsType == nullptr) {
        elementsType = elementExprType;
      } else if (!elementsType->matchNice(elementExprType)) {
        throw Error(this->reader, element.start, element.end, E1017);
      }
    }

    return this->_wrapNodeExprType(stmtExpr, targetType, realTargetType == nullptr ? this->typeMap.arrayOf(elementsType) : realTargetType);
  } else if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body)) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto leftType = this->_nodeExprType(exprAssign.left, nullptr);
    auto rightType = this->_nodeExprType(exprAssign.right, leftType);

    return this->_wrapNodeExprType(stmtExpr, targetType, leftType->isRef() && !rightType->isRef() ? Type::real(leftType) : leftType);
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body)) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto exprBinaryLeftType = static_cast<Type *>(nullptr);
    auto exprBinaryRightType = static_cast<Type *>(nullptr);

    try {
      exprBinaryLeftType = this->_nodeExprType(exprBinary.left, nullptr);
    } catch (const Error &) {
      try {
        exprBinaryRightType = this->_nodeExprType(exprBinary.right, nullptr);
      } catch (const Error &) {
      }
    }

    if (exprBinaryLeftType == nullptr && exprBinaryRightType == nullptr) {
      throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1020);
    } else if (exprBinaryLeftType == nullptr) {
      exprBinaryLeftType = this->_nodeExprType(exprBinary.left, exprBinaryRightType);
    } else if (exprBinaryRightType == nullptr) {
      exprBinaryRightType = this->_nodeExprType(exprBinary.right, exprBinaryLeftType);
    }

    if (exprBinaryLeftType->isStr() || exprBinaryRightType->isStr()) {
      if (exprBinary.op.type == TK_OP_EQ_EQ || exprBinary.op.type == TK_OP_EXCL_EQ) {
        return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("bool"));
      } else {
        return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("str"));
      }
    } else if (
      exprBinary.op.type == TK_OP_EQ_EQ ||
      exprBinary.op.type == TK_OP_EXCL_EQ ||
      exprBinary.op.type == TK_OP_GT ||
      exprBinary.op.type == TK_OP_GT_EQ ||
      exprBinary.op.type == TK_OP_LT ||
      exprBinary.op.type == TK_OP_LT_EQ ||
      (exprBinaryLeftType->isBool() && exprBinaryRightType->isBool())
    ) {
      return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("bool"));
    } else if (exprBinaryLeftType->isNumber() && exprBinaryRightType->isNumber()) {
      return this->_wrapNodeExprType(stmtExpr, targetType, Type::largest(exprBinaryLeftType, exprBinaryRightType));
    } else {
      return this->_wrapNodeExprType(stmtExpr, targetType, exprBinaryLeftType->isNumber() ? exprBinaryLeftType : Type::real(exprBinaryRightType));
    }
  } else if (std::holds_alternative<ParserExprCall>(*stmtExpr.body)) {
    auto exprCall = std::get<ParserExprCall>(*stmtExpr.body);
    auto exprCallCalleeType = Type::real(this->_nodeExprType(exprCall.callee, nullptr));

    return this->_wrapNodeExprType(stmtExpr, targetType, std::get<TypeFn>(exprCallCalleeType->body).returnType);
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body)) {
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto initialTypeCasts = this->typeCasts;
    auto [bodyTypeCasts, altTypeCasts] = this->_evalTypeCasts(exprCond.cond);

    auto exprCondBodyType = static_cast<Type *>(nullptr);
    auto exprCondAltType = static_cast<Type *>(nullptr);

    try {
      mergeTypeCasts(this->typeCasts, bodyTypeCasts);
      exprCondBodyType = this->_nodeExprType(exprCond.body, targetType);
    } catch (const Error &) {
      try {
        this->typeCasts = initialTypeCasts;
        mergeTypeCasts(this->typeCasts, altTypeCasts);
        exprCondAltType = this->_nodeExprType(exprCond.alt, targetType);
      } catch (const Error &) {
      }
    }

    this->typeCasts = initialTypeCasts;

    if (exprCondBodyType == nullptr && exprCondAltType == nullptr) {
      throw Error(this->reader, exprCond.body.start, exprCond.alt.end, E1020);
    } else if (exprCondBodyType == nullptr) {
      mergeTypeCasts(this->typeCasts, bodyTypeCasts);
      exprCondBodyType = this->_nodeExprType(exprCond.body, exprCondAltType);
      this->typeCasts = initialTypeCasts;
    } else if (exprCondAltType == nullptr) {
      mergeTypeCasts(this->typeCasts, altTypeCasts);
      exprCondAltType = this->_nodeExprType(exprCond.alt, exprCondBodyType);
      this->typeCasts = initialTypeCasts;
    }

    if (Type::real(exprCondBodyType)->isAny() && !Type::real(exprCondAltType)->isAny()) {
      exprCondAltType = this->typeMap.get("any");
    } else if (Type::real(exprCondAltType)->isAny() && !Type::real(exprCondBodyType)->isAny()) {
      exprCondBodyType = this->typeMap.get("any");
    } else if (Type::real(exprCondBodyType)->isOpt() && !Type::real(exprCondAltType)->isOpt()) {
      exprCondAltType = this->typeMap.opt(Type::real(exprCondAltType));
    } else if (Type::real(exprCondAltType)->isOpt() && !Type::real(exprCondBodyType)->isOpt()) {
      exprCondBodyType = this->typeMap.opt(Type::real(exprCondBodyType));
    }

    if (exprCondBodyType->isNumber() && exprCondAltType->isNumber()) {
      return this->_wrapNodeExprType(stmtExpr, targetType, Type::largest(exprCondBodyType, exprCondAltType));
    } else if (exprCondBodyType->isRef() && !exprCondAltType->isRef()) {
      return this->_wrapNodeExprType(stmtExpr, targetType, exprCondAltType);
    }

    return this->_wrapNodeExprType(stmtExpr, targetType, exprCondBodyType);
  } else if (std::holds_alternative<ParserExprIs>(*stmtExpr.body)) {
    return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("bool"));
  } else if (std::holds_alternative<ParserExprLit>(*stmtExpr.body)) {
    auto exprLit = std::get<ParserExprLit>(*stmtExpr.body);

    if (exprLit.body.type == TK_KW_FALSE || exprLit.body.type == TK_KW_TRUE) {
      return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("bool"));
    } else if (exprLit.body.type == TK_KW_NIL) {
      if (targetType == nullptr) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1018);
      }

      auto targetRealType = Type::real(targetType);

      if (!targetRealType->isOpt()) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, std::regex_replace(E1019, std::regex("@type"), targetType->name));
      }

      return this->_wrapNodeExprType(stmtExpr, targetType, targetRealType);
    } else if (exprLit.body.type == TK_LIT_CHAR) {
      return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("char"));
    } else if (exprLit.body.type == TK_LIT_FLOAT) {
      if (
        targetType == nullptr ||
        !this->typeMap.get("f64")->matchNice(targetType) ||
        Type::actual(targetType)->isAny() ||
        Type::actual(targetType)->isUnion()
      ) {
        return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("float"));
      } else {
        return this->_wrapNodeExprType(stmtExpr, targetType, targetType);
      }
    } else if (
      exprLit.body.type == TK_LIT_INT_BIN ||
      exprLit.body.type == TK_LIT_INT_DEC ||
      exprLit.body.type == TK_LIT_INT_HEX ||
      exprLit.body.type == TK_LIT_INT_OCT
    ) {
      if (
        targetType == nullptr ||
        (!this->typeMap.get("i64")->matchNice(targetType) && !this->typeMap.get("u64")->matchNice(targetType)) ||
        Type::actual(targetType)->isAny() ||
        Type::actual(targetType)->isUnion()
      ) {
        return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("int"));
      } else {
        return this->_wrapNodeExprType(stmtExpr, targetType, targetType);
      }
    } else if (exprLit.body.type == TK_LIT_STR) {
      return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("str"));
    }
  } else if (std::holds_alternative<ParserExprMap>(*stmtExpr.body)) {
    auto actualTargetType = targetType == nullptr ? static_cast<Type *>(nullptr) : Type::actual(targetType);
    auto realTargetType = static_cast<Type *>(nullptr);

    if (actualTargetType != nullptr && actualTargetType->isMap()) {
      realTargetType = actualTargetType;
    } else if (actualTargetType != nullptr && actualTargetType->isOpt() && std::get<TypeOptional>(actualTargetType->body).type->isMap()) {
      realTargetType = std::get<TypeOptional>(actualTargetType->body).type;
    }

    auto exprMap = std::get<ParserExprMap>(*stmtExpr.body);

    if (exprMap.props.empty()) {
      if (realTargetType == nullptr) {
        throw Error(this->reader, stmtExpr.start, stmtExpr.end, E1027);
      }

      return this->_wrapNodeExprType(stmtExpr, targetType, realTargetType);
    }

    auto keyType = this->typeMap.get("str");
    auto valueType = realTargetType == nullptr
      ? static_cast<Type *>(nullptr)
      : std::get<TypeBodyMap>(realTargetType->body).valueType;

    for (const auto &prop : exprMap.props) {
      auto propInitType = this->_nodeExprType(prop.init, valueType);

      if (valueType == nullptr) {
        valueType = propInitType;
      } else if (!valueType->matchNice(propInitType)) {
        valueType = this->typeMap.unionAdd(valueType, propInitType);
      }
    }

    auto wrapTargetType = realTargetType == nullptr ? this->typeMap.createMap(keyType, valueType) : realTargetType;
    return this->_wrapNodeExprType(stmtExpr, targetType, wrapTargetType);
  } else if (std::holds_alternative<ParserExprObj>(*stmtExpr.body)) {
    auto exprObj = std::get<ParserExprObj>(*stmtExpr.body);
    return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get(exprObj.id.val));
  } else if (std::holds_alternative<ParserExprRef>(*stmtExpr.body)) {
    auto exprRef = std::get<ParserExprRef>(*stmtExpr.body);
    auto exprRefExprType = this->_nodeExprType(exprRef.expr, nullptr);

    return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.ref(exprRefExprType));
  } else if (std::holds_alternative<ParserExprUnary>(*stmtExpr.body)) {
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto exprUnaryArgType = this->_nodeExprType(exprUnary.arg, nullptr);

    if (exprUnary.op.type == TK_OP_EXCL) {
      return this->_wrapNodeExprType(stmtExpr, targetType, this->typeMap.get("bool"));
    }

    return this->_wrapNodeExprType(stmtExpr, targetType, Type::real(exprUnaryArgType));
  }

  throw Error("tried to analyze unknown expression type");
}

Type *AST::_type (const ParserType &type) {
  if (std::holds_alternative<ParserTypeArray>(*type.body)) {
    auto typeArray = std::get<ParserTypeArray>(*type.body);
    auto elementType = this->_type(typeArray.elementType);

    return this->typeMap.arrayOf(elementType);
  } else if (std::holds_alternative<ParserTypeFn>(*type.body)) {
    auto typeFn = std::get<ParserTypeFn>(*type.body);
    auto fnReturnType = this->_type(typeFn.returnType);
    auto fnParams = std::vector<TypeFnParam>{};

    for (const auto &typeFnParam : typeFn.params) {
      auto paramName = typeFnParam.id == std::nullopt ? std::optional<std::string>{} : typeFnParam.id->val;
      auto paramType = this->_type(typeFnParam.type);

      if (typeFnParam.variadic) {
        paramType = this->typeMap.arrayOf(paramType);
      }

      fnParams.push_back(TypeFnParam{paramName, paramType, typeFnParam.mut, !typeFnParam.variadic, typeFnParam.variadic});
    }

    return this->typeMap.fn(fnParams, fnReturnType);
  } else if (std::holds_alternative<ParserTypeId>(*type.body)) {
    auto typeId = std::get<ParserTypeId>(*type.body);

    if (typeId.id.val == "any") return this->typeMap.get("any");
    else if (typeId.id.val == "bool") return this->typeMap.get("bool");
    else if (typeId.id.val == "byte") return this->typeMap.get("byte");
    else if (typeId.id.val == "char") return this->typeMap.get("char");
    else if (typeId.id.val == "f32") return this->typeMap.get("f32");
    else if (typeId.id.val == "f64") return this->typeMap.get("f64");
    else if (typeId.id.val == "float") return this->typeMap.get("float");
    else if (typeId.id.val == "i8") return this->typeMap.get("i8");
    else if (typeId.id.val == "i16") return this->typeMap.get("i16");
    else if (typeId.id.val == "i32") return this->typeMap.get("i32");
    else if (typeId.id.val == "i64") return this->typeMap.get("i64");
    else if (typeId.id.val == "int") return this->typeMap.get("int");
    else if (typeId.id.val == "str") return this->typeMap.get("str");
    else if (typeId.id.val == "u8") return this->typeMap.get("u8");
    else if (typeId.id.val == "u16") return this->typeMap.get("u16");
    else if (typeId.id.val == "u32") return this->typeMap.get("u32");
    else if (typeId.id.val == "u64") return this->typeMap.get("u64");
    else if (typeId.id.val == "void") return this->typeMap.get("void");

    if (this->typeMap.has(typeId.id.val)) {
      return this->typeMap.get(typeId.id.val);
    }

    throw Error(this->reader, typeId.id.start, typeId.id.end, E1010);
  } else if (std::holds_alternative<ParserTypeMap>(*type.body)) {
    auto typeBodyMap = std::get<ParserTypeMap>(*type.body);
    auto keyType = this->_type(typeBodyMap.keyType);
    auto valueType = this->_type(typeBodyMap.valueType);

    return this->typeMap.createMap(keyType, valueType);
  } else if (std::holds_alternative<ParserTypeOptional>(*type.body)) {
    auto typeOptional = std::get<ParserTypeOptional>(*type.body);
    auto optionalType = this->_type(typeOptional.type);

    return this->typeMap.opt(optionalType);
  } else if (std::holds_alternative<ParserTypeRef>(*type.body)) {
    auto typeRef = std::get<ParserTypeRef>(*type.body);
    auto refType = this->_type(typeRef.refType);

    return this->typeMap.ref(refType);
  } else if (std::holds_alternative<ParserTypeUnion>(*type.body)) {
    auto typeUnion = std::get<ParserTypeUnion>(*type.body);
    auto subTypes = std::vector<Type *>{};

    for (const auto &subType : typeUnion.subTypes) {
      subTypes.push_back(this->_type(subType));
    }

    return this->typeMap.unionType(subTypes);
  }

  throw Error("tried to analyze unknown type");
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
ASTNode AST::_wrapNode ([[maybe_unused]] const ParserStmt &stmt, const ASTNodeBody &body) {
  return ASTNode{std::make_shared<ASTNodeBody>(body), nullptr};
}

ASTNodeExpr AST::_wrapNodeExpr (const ParserStmtExpr &stmtExpr, Type *targetType, const ASTExpr &expr) {
  auto nodeExprType = this->_nodeExprType(stmtExpr, targetType);
  return ASTNodeExpr{nodeExprType, std::make_shared<ASTExpr>(expr), stmtExpr.parenthesized};
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Type *AST::_wrapNodeExprType ([[maybe_unused]] const ParserStmtExpr &stmtExpr, [[maybe_unused]] Type *targetType, Type *resultType) {
  return resultType;
}
