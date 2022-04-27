/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Codegen.hpp"

const auto banner = std::string(
  "/*!\n"
  " * Copyright (c) Aaron Delasy\n"
  " *\n"
  " * Unauthorized copying of this file, via any medium is strictly prohibited\n"
  " * Proprietary and confidential\n"
  " */\n\n"
);

std::string Codegen::name (const std::string &name, bool builtin) {
  return (builtin ? "builtin_" : "__THE_0_") + name;
}

Codegen::Codegen (AST *a) {
  this->ast = a;
  this->reader = this->ast->reader;
}

std::tuple<std::string, std::string> Codegen::gen () {
  auto mainSetUp = std::string();
  auto mainCode = std::string();
  auto mainCleanUp = std::string();
  auto topLevelSetUp = std::string();
  auto topLevelCode = std::string();
  auto topLevelCleanUp = std::string();
  auto output = std::string();
  auto nodes = this->ast->gen();

  for (const auto &node : nodes) {
    if (std::holds_alternative<ASTNodeMain>(node.body)) {
      auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);

      mainSetUp = nodeSetUp;
      mainCode = nodeCode;
      mainCleanUp = nodeCleanUp;
    } else {
      this->indent = 2;
      auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);
      this->indent = 0;

      topLevelSetUp += nodeSetUp;
      topLevelCode += nodeCode;
      topLevelCleanUp += nodeCleanUp;
    }
  }

  auto builtinStructDefinitionsCode = std::string();

  if (this->builtins.typeInt) {
    builtinStructDefinitionsCode += "struct builtin_int {\n";
    builtinStructDefinitionsCode += "  int32_t v;\n";
    builtinStructDefinitionsCode += "};\n";
  }

  if (this->builtins.libMath) {
    this->flags.emplace_back("-lm");
  }

  auto headers = std::string(this->builtins.libMath ? "#include <math.h>\n" : "");
  headers += std::string(this->builtins.libStdbool ? "#include <stdbool.h>\n" : "");
  headers += std::string(this->builtins.libStdint ? "#include <stdint.h>\n" : "");

  headers += headers.empty() ? "" : "\n";
  builtinStructDefinitionsCode += builtinStructDefinitionsCode.empty() ? "" : "\n";

  output += banner;
  output += headers;
  output += builtinStructDefinitionsCode;
  output += "int main () {\n";
  output += topLevelSetUp;
  output += topLevelCode;
  output += mainSetUp;
  output += mainCode;
  output += mainCleanUp;
  output += topLevelCleanUp;
  output += "}\n";

  return std::make_tuple(output, this->_flags());
}

std::string Codegen::_block (const ASTBlock &block) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  this->indent += 2;

  for (const auto &node : block) {
    auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);

    setUp += nodeSetUp;
    code += nodeCode;
    cleanUp += nodeCleanUp;
  }

  this->indent -= 2;
  return setUp + code + cleanUp;
}

std::string Codegen::_exprAccess (const ASTMemberObj &exprAccessBody) {
  if (std::holds_alternative<std::shared_ptr<Var>>(exprAccessBody)) {
    auto id = std::get<std::shared_ptr<Var>>(exprAccessBody);
    return Codegen::name(id->codeName);
  }

  auto member = std::get<std::shared_ptr<ASTMember>>(exprAccessBody);
  auto memberObj = this->_exprAccess(member->obj);

  return memberObj + "." + Codegen::name(member->prop);
}

std::string Codegen::_flags () const {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : this->flags) {
    result += (idx++ == 0 ? "" : " ") + flag;
  }

  return result;
}

std::tuple<std::string, std::string, std::string> Codegen::_node (const ASTNode &node) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (std::holds_alternative<ASTNodeBreak>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeContinue>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeExpr>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeFnDecl>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeIf>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeLoop>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeMain>(node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(node.body);
    code += this->_block(nodeMain.body);
  } else if (std::holds_alternative<ASTNodeObjDecl>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeReturn>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeVarDecl>(node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(node.body);
    auto varName = Codegen::name(nodeVarDecl.var->codeName);
    auto varType = this->_type(nodeVarDecl.var->type, nodeVarDecl.var->mut);

    code += std::string(this->indent, ' ') + varType + varName + " = {";

    if (nodeVarDecl.init == std::nullopt) {
      if (nodeVarDecl.var->type->isInt()) {
        code += "0";
      }
    } else {
      auto [initSetUp, initCode, initCleanUp] = this->_nodeExpr(*nodeVarDecl.init);

      setUp += initSetUp;
      code += initCode;
      cleanUp += initCleanUp;
    }

    code += "};\n";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::tuple<std::string, std::string, std::string> Codegen::_nodeExpr (const std::shared_ptr<ASTNodeExpr> &nodeExpr) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (nodeExpr->parenthesized) {
    code += "(";
  }

  if (std::holds_alternative<ASTExprAccess>(nodeExpr->body)) {
    code += this->_exprAccess(std::get<ASTExprAccess>(nodeExpr->body).body) + ".v";
  } else if (std::holds_alternative<ASTExprAssign>(nodeExpr->body)) {
    auto exprAssign = std::get<ASTExprAssign>(nodeExpr->body);
    auto leftCode = this->_exprAccess(exprAssign.left.body) + ".v";
    auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right);

    if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_AND) {
      code += leftCode + " = " + leftCode + " && " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_OR) {
      code += leftCode + " = " + leftCode + " || " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_POWER) {
      this->builtins.libMath = true;
      code += leftCode + " = pow(" + leftCode + ", " + rightCode + ")";
    } else {
      code += leftCode;

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) code += " += ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BITWISE_AND) code += " &= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BITWISE_OR) code += " |= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BITWISE_XOR) code += " ^= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_COALESCE) code += " ?\?= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_DIVIDE) code += " /= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_EQUAL) code += " = ";
      if (exprAssign.op == AST_EXPR_ASSIGN_LEFT_SHIFT) code += " <<= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_MULTIPLY) code += " *= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_REMAINDER) code += " %= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_RIGHT_SHIFT) code += " >>= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_SUBTRACT) code += " -= ";

      code += rightCode;
    }

    setUp += rightSetUp;
    cleanUp += rightCleanUp;
  } else if (std::holds_alternative<ASTExprBinary>(nodeExpr->body)) {
    auto exprBinary = std::get<ASTExprBinary>(nodeExpr->body);
    auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);
    auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);

    if (exprBinary.op == AST_EXPR_BINARY_POWER) {
      this->builtins.libMath = true;
      code += "pow(" + leftCode + ", " + rightCode + ")";
    } else {
      code += leftCode;

      if (exprBinary.op == AST_EXPR_BINARY_ADD) code += " + ";
      if (exprBinary.op == AST_EXPR_BINARY_BITWISE_AND) code += " & ";
      if (exprBinary.op == AST_EXPR_BINARY_BITWISE_OR) code += " | ";
      if (exprBinary.op == AST_EXPR_BINARY_BITWISE_XOR) code += " ^ ";
      if (exprBinary.op == AST_EXPR_BINARY_DIVIDE) code += " / ";
      if (exprBinary.op == AST_EXPR_BINARY_EQUAL) code += " == ";
      if (exprBinary.op == AST_EXPR_BINARY_GREATER_EQUAL) code += " >= ";
      if (exprBinary.op == AST_EXPR_BINARY_GREATER_THAN) code += " > ";
      if (exprBinary.op == AST_EXPR_BINARY_LEFT_SHIFT) code += " << ";
      if (exprBinary.op == AST_EXPR_BINARY_LESS_EQUAL) code += " <= ";
      if (exprBinary.op == AST_EXPR_BINARY_LESS_THAN) code += " < ";
      if (exprBinary.op == AST_EXPR_BINARY_LOGICAL_AND) code += " && ";
      if (exprBinary.op == AST_EXPR_BINARY_LOGICAL_OR) code += " || ";
      if (exprBinary.op == AST_EXPR_BINARY_MULTIPLY) code += " * ";
      if (exprBinary.op == AST_EXPR_BINARY_NOT_EQUAL) code += " != ";
      if (exprBinary.op == AST_EXPR_BINARY_REMAINDER) code += " % ";
      if (exprBinary.op == AST_EXPR_BINARY_RIGHT_SHIFT) code += " >> ";
      if (exprBinary.op == AST_EXPR_BINARY_SUBTRACT) code += " - ";

      code += rightCode;
    }

    setUp += leftSetUp + rightSetUp;
    cleanUp += leftCleanUp + rightCleanUp;
  } else if (std::holds_alternative<ASTExprCall>(nodeExpr->body)) {
    // todo
  } else if (std::holds_alternative<ASTExprCond>(nodeExpr->body)) {
    auto exprCond = std::get<ASTExprCond>(nodeExpr->body);
    auto [condSetUp, condCode, condCleanUp] = this->_nodeExpr(exprCond.cond);
    auto [bodySetUp, bodyCode, bodyCleanUp] = this->_nodeExpr(exprCond.body);
    auto [altSetUp, altCode, altCleanUp] = this->_nodeExpr(exprCond.alt);

    setUp += condSetUp + bodySetUp + altSetUp;
    code += condCode + " ? " + bodyCode + " : " + altCode;
    cleanUp += condCleanUp + bodyCleanUp + altCleanUp;
  } else if (std::holds_alternative<ASTExprLit>(nodeExpr->body)) {
    auto exprLit = std::get<ASTExprLit>(nodeExpr->body);

    if (exprLit.type == AST_EXPR_LIT_BOOL) {
      this->builtins.libStdbool = true;
    }

    if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      auto val = exprLit.body;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      code += val;
    } else {
      code += exprLit.body;
    }
  } else if (std::holds_alternative<ASTExprObj>(nodeExpr->body)) {
    // todo
  } else if (std::holds_alternative<ASTExprUnary>(nodeExpr->body)) {
    auto exprUnary = std::get<ASTExprUnary>(nodeExpr->body);
    auto [argSetUp, argCode, argCleanUp] = this->_nodeExpr(exprUnary.arg);

    if (!exprUnary.prefix) {
      code += argCode;
    }

    if (exprUnary.op == AST_EXPR_UNARY_BITWISE_NOT) code += "~";
    if (exprUnary.op == AST_EXPR_UNARY_DECREMENT) code += "--";
    if (exprUnary.op == AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT) code += "!!";
    if (exprUnary.op == AST_EXPR_UNARY_INCREMENT) code += "++";
    if (exprUnary.op == AST_EXPR_UNARY_LOGICAL_NOT) code += "!";
    if (exprUnary.op == AST_EXPR_UNARY_NEGATION) code += "-";
    if (exprUnary.op == AST_EXPR_UNARY_PLUS) code += "+";

    if (exprUnary.prefix) {
      code += argCode;
    }

    setUp += argSetUp;
    cleanUp += argCleanUp;
  }

  if (nodeExpr->parenthesized) {
    code += ")";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::string Codegen::_type (const std::shared_ptr<Type> &type, bool mut) {
  if (type->isInt()) {
    this->builtins.libStdint = true;
    this->builtins.typeInt = true;

    return std::string(mut ? "" : "const ") + "struct " + Codegen::name("int", true) + " ";
  }

  throw Error("Error: tried to generate code for unknown type");
}
