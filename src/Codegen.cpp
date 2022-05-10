/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Codegen.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>

const auto banner = std::string(
  "/*!\n"
  " * Copyright (c) Aaron Delasy\n"
  " *\n"
  " * Unauthorized copying of this file, via any medium is strictly prohibited\n"
  " * Proprietary and confidential\n"
  " */\n\n"
);

void Codegen::compile (const std::string &path, const std::tuple<std::string, std::string> &result, bool debug) {
  auto code = std::get<0>(result);
  auto flags = std::get<1>(result);

  auto f = std::ofstream("build/output.c");
  f << code;
  f.close();

  auto cmd = "gcc build/output.c -o " + path + (debug ? " -g" : "") + (flags.empty() ? "" : " " + flags);
  std::system(cmd.c_str());
  std::filesystem::remove("build/output.c");
}

std::string Codegen::name (const std::string &name) {
  return "__THE_0_" + name;
}

Codegen::Codegen (AST *a) {
  this->ast = a;
  this->reader = this->ast->reader;
}

std::tuple<std::string, std::string> Codegen::gen () {
  auto nodes = this->ast->gen();
  auto mainSetUp = std::string();
  auto mainCode = std::string();
  auto mainCleanUp = std::string();
  auto topLevelSetUp = std::string();
  auto topLevelCode = std::string();
  auto topLevelCleanUp = std::string();

  for (const auto &node : nodes) {
    if (std::holds_alternative<ASTNodeMain>(*node.body)) {
      auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);

      mainSetUp += nodeSetUp;
      mainCode += nodeCode;
      mainCleanUp = nodeCleanUp + mainCleanUp;
    } else {
      this->indent = 2;
      auto [nodeSetUp, nodeCode, nodeCleanUp] = this->_node(node);
      this->indent = 0;

      topLevelSetUp += nodeSetUp;
      topLevelCode += nodeCode;
      topLevelCleanUp = nodeCleanUp + topLevelCleanUp;
    }
  }

  auto builtinFunctionDeclarationsCode = std::string();
  auto builtinFunctionDefinitionsCode = std::string();
  auto builtinStructDefinitionsCode = std::string();

  if (this->builtins.fnCstrConcatStr) {
    this->builtins.fnStrConcatCstr = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str cstr_concat_str (const char *, struct str);\n";
    builtinFunctionDefinitionsCode += "struct str cstr_concat_str (const char *c, struct str s) {\n";
    builtinFunctionDefinitionsCode += "  return str_concat_cstr(s, c);\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrConcatCstr) {
    this->builtins.fnAlloc = true;
    this->builtins.fnStrDeinit = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_concat_cstr (struct str, const char *);\n";
    builtinFunctionDefinitionsCode += "struct str str_concat_cstr (struct str s, const char *c) {\n";
    builtinFunctionDefinitionsCode += "  size_t l = s.l + strlen(c);\n";
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(r, s.c, s.l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(&r[s.l], c, l - s.l);\n";
    builtinFunctionDefinitionsCode += "  str_deinit(s);\n";
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrConcatStr) {
    this->builtins.fnAlloc = true;
    this->builtins.fnStrDeinit = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_concat_str (struct str, struct str);\n";
    builtinFunctionDefinitionsCode += "struct str str_concat_str (struct str s1, struct str s2) {\n";
    builtinFunctionDefinitionsCode += "  size_t l = s1.l + s2.l;\n";
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(r, s1.c, s1.l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(&r[s1.l], s2.c, s2.l);\n";
    builtinFunctionDefinitionsCode += "  str_deinit(s1);\n";
    builtinFunctionDefinitionsCode += "  str_deinit(s2);\n";
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrCopy) {
    this->builtins.fnAlloc = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_copy (const struct str);\n";
    builtinFunctionDefinitionsCode += "struct str str_copy (const struct str s) {\n";
    builtinFunctionDefinitionsCode += "  char *r = alloc(s.l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(r, s.c, s.l);\n";
    builtinFunctionDefinitionsCode += "  return (struct str) {r, s.l};\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrInit) {
    this->builtins.fnAlloc = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_init (const char *);\n";
    builtinFunctionDefinitionsCode += "struct str str_init (const char *c) {\n";
    builtinFunctionDefinitionsCode += "  size_t l = strlen(c);\n";
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(r, c, l);\n";
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrNot) {
    this->builtins.fnStrDeinit = true;
    this->builtins.libStdbool = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool str_not (struct str);\n";
    builtinFunctionDefinitionsCode += "bool str_not (struct str s) {\n";
    builtinFunctionDefinitionsCode += "  bool r = s.l == 0;\n";
    builtinFunctionDefinitionsCode += "  str_deinit(s);\n";
    builtinFunctionDefinitionsCode += "  return r;\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrReinit) {
    this->builtins.fnStrDeinit = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_reinit (struct str, struct str);\n";
    builtinFunctionDefinitionsCode += "struct str str_reinit (struct str s1, struct str s2) {\n";
    builtinFunctionDefinitionsCode += "  str_deinit(s1);\n";
    builtinFunctionDefinitionsCode += "  return s2;\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrDeinit) {
    this->builtins.libStdlib = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "void str_deinit (struct str);\n";
    builtinFunctionDefinitionsCode += "void str_deinit (struct str s) {\n";
    builtinFunctionDefinitionsCode += "  free(s.c);\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnAlloc) {
    this->builtins.libStdio = true;
    this->builtins.libStdlib = true;

    builtinFunctionDeclarationsCode += "void *alloc (size_t);\n";
    builtinFunctionDefinitionsCode += "void *alloc (size_t l) {\n";
    builtinFunctionDefinitionsCode += "  void *r = malloc(l);\n";
    builtinFunctionDefinitionsCode += "  if (r == NULL) {\n";
    builtinFunctionDefinitionsCode += R"(    fprintf(stderr, "Error: failed to allocate %zu bytes\n", l);)" "\n";
    builtinFunctionDefinitionsCode += "    exit(EXIT_FAILURE);\n";
    builtinFunctionDefinitionsCode += "  }\n";
    builtinFunctionDefinitionsCode += "  return r;\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.typeStr) {
    if (!this->builtins.libStdio && !this->builtins.libStdlib && !this->builtins.libString) {
      this->builtins.libStddef = true;
    }

    builtinStructDefinitionsCode += "struct str {\n";
    builtinStructDefinitionsCode += "  char *c;\n";
    builtinStructDefinitionsCode += "  size_t l;\n";
    builtinStructDefinitionsCode += "};\n";
  }

  builtinFunctionDeclarationsCode += builtinFunctionDeclarationsCode.empty() ? "" : "\n";
  builtinFunctionDefinitionsCode += builtinFunctionDefinitionsCode.empty() ? "" : "\n";
  builtinStructDefinitionsCode += builtinStructDefinitionsCode.empty() ? "" : "\n";

  auto headers = std::string(this->builtins.libMath ? "#include <math.h>\n" : "");
  headers += std::string(this->builtins.libStdbool ? "#include <stdbool.h>\n" : "");
  headers += std::string(this->builtins.libStddef ? "#include <stddef.h>\n" : "");
  headers += std::string(this->builtins.libStdint ? "#include <stdint.h>\n" : "");
  headers += std::string(this->builtins.libStdio ? "#include <stdio.h>\n" : "");
  headers += std::string(this->builtins.libStdlib ? "#include <stdlib.h>\n" : "");
  headers += std::string(this->builtins.libString ? "#include <string.h>\n" : "");
  headers += headers.empty() ? "" : "\n";

  if (this->builtins.libMath) {
    this->flags.emplace_back("-lm");
  }

  auto output = std::string();
  output += banner;
  output += headers;
  output += builtinStructDefinitionsCode;
  output += builtinFunctionDeclarationsCode;
  output += builtinFunctionDefinitionsCode;
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
    cleanUp = nodeCleanUp + cleanUp;
  }

  this->indent -= 2;
  return setUp + code + cleanUp;
}

std::string Codegen::_exprAccess (const std::shared_ptr<ASTMemberObj> &exprAccessBody) {
  if (std::holds_alternative<std::shared_ptr<Var>>(*exprAccessBody)) {
    auto id = std::get<std::shared_ptr<Var>>(*exprAccessBody);
    return Codegen::name(id->codeName);
  }

  auto member = std::get<ASTMember>(*exprAccessBody);
  auto memberObj = this->_exprAccess(member.obj);

  return memberObj + "." + Codegen::name(member.prop);
}

std::string Codegen::_flags () const {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : this->flags) {
    result += (idx++ == 0 ? "" : " ") + flag;
  }

  return result;
}

CodegenNode Codegen::_node (const ASTNode &node) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (std::holds_alternative<ASTNodeBreak>(*node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeContinue>(*node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(*node.body);
    auto nodeExprCode = this->_nodeExpr(nodeExpr, true);

    code = std::string(this->indent, ' ') + nodeExprCode + ";\n";
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
    // todo
    // todo varMap save/restore
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    // todo
    // todo if/else varMap save/restore
  } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
    // todo
    // todo varMap save/restore
  } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(*node.body);

    this->varMap.save();
    code = this->_block(nodeMain.body);
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
    auto name = Codegen::name(nodeVarDecl.var->codeName);
    auto type = this->_type(nodeVarDecl.var->type, nodeVarDecl.var->mut);
    auto initCode = std::string("0");

    if (nodeVarDecl.init != std::nullopt) {
      initCode = this->_nodeExpr(*nodeVarDecl.init);
    } else if (nodeVarDecl.var->type->isBool()) {
      this->builtins.libStdbool = true;
      initCode = "false";
    } else if (nodeVarDecl.var->type->isChar()) {
      initCode = R"('\0')";
    } else if (nodeVarDecl.var->type->isStr()) {
      this->builtins.fnStrInit = true;
      initCode = R"(str_init(""))";
    }

    code = std::string(this->indent, ' ') + type + name + " = " + initCode + ";\n";

    if (nodeVarDecl.var->type->isStr()) {
       this->builtins.fnStrDeinit = true;
       cleanUp = std::string(this->indent, ' ') + "str_deinit((struct str) " + name + ");\n";
    }

    return this->_wrapNode(node, setUp, code, cleanUp);
  }

  throw Error("Error: tried to generate code for unknown node");
}

std::string Codegen::_nodeExpr (const ASTNodeExpr &nodeExpr, bool root) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
    auto code = this->_exprAccess(exprAccess.body);

    if (!root && nodeExpr.type->isStr()) {
      this->builtins.fnStrCopy = true;
      code = "str_copy(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto leftCode = this->_exprAccess(exprAssign.left.body);

    if (exprAssign.right.type->isStr()) {
      auto rightCode = std::string();

      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        if (exprAssign.right.isLit()) {
          this->builtins.fnStrConcatCstr = true;
          this->builtins.fnStrCopy = true;

          rightCode = "str_concat_cstr(str_copy(" + leftCode + "), " + exprAssign.right.litBody() + ")";
        } else {
          this->builtins.fnStrConcatStr = true;
          this->builtins.fnStrCopy = true;

          rightCode = "str_concat_str(str_copy(" + leftCode + "), " + this->_nodeExpr(exprAssign.right) + ")";
        }
      } else {
        rightCode = this->_nodeExpr(exprAssign.right);
      }

      this->builtins.fnStrReinit = true;
      auto code = leftCode + " = str_reinit(" + leftCode + ", " + rightCode + ")";

      if (!root && nodeExpr.type->isStr()) {
        this->builtins.fnStrCopy = true;
        code = "str_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }

    auto opCode = std::string(" = ");
    auto rightCode = this->_nodeExpr(exprAssign.right);

    if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_AND) {
      rightCode = leftCode + " && " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_OR) {
      rightCode = leftCode + " || " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_POWER) {
      this->builtins.libMath = true;
      rightCode = "pow(" + leftCode + ", " + rightCode + ")";
    } else {
      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) opCode = " += ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BITWISE_AND) opCode = " &= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BITWISE_OR) opCode = " |= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BITWISE_XOR) opCode = " ^= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_COALESCE) opCode = " ?\?= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_DIVIDE) opCode = " /= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_EQUAL) opCode = " = ";
      if (exprAssign.op == AST_EXPR_ASSIGN_LEFT_SHIFT) opCode = " <<= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_MULTIPLY) opCode = " *= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_REMAINDER) opCode = " %= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_RIGHT_SHIFT) opCode = " >>= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_SUBTRACT) opCode = " -= ";
    }

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (exprBinary.left.type->isStr() && exprBinary.right.type->isStr()) {
      if (nodeExpr.isLit()) {
        this->builtins.fnStrInit = true;
        return this->_wrapNodeExpr(nodeExpr, "str_init(" + nodeExpr.litBody() + ")");
      } else if (exprBinary.left.isLit()) {
        this->builtins.fnCstrConcatStr = true;

        auto rightCode = this->_nodeExpr(exprBinary.right);
        return this->_wrapNodeExpr(nodeExpr, "cstr_concat_str(" + exprBinary.left.litBody() + ", " + rightCode + ")");
      } else if (exprBinary.right.isLit()) {
        this->builtins.fnStrConcatCstr = true;

        auto leftCode = this->_nodeExpr(exprBinary.left);
        return this->_wrapNodeExpr(nodeExpr, "str_concat_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")");
      } else {
        this->builtins.fnStrConcatStr = true;

        auto leftCode = this->_nodeExpr(exprBinary.left);
        auto rightCode = this->_nodeExpr(exprBinary.right);

        return this->_wrapNodeExpr(nodeExpr, "str_concat_str(" + leftCode + ", " + rightCode + ")");
      }
    }

    auto leftCode = this->_nodeExpr(exprBinary.left);
    auto rightCode = this->_nodeExpr(exprBinary.right);

    if (exprBinary.op == AST_EXPR_BINARY_POWER) {
      this->builtins.libMath = true;
      return this->_wrapNodeExpr(nodeExpr, "pow(" + leftCode + ", " + rightCode + ")");
    }

    auto opCode = std::string();

    if (exprBinary.op == AST_EXPR_BINARY_ADD) opCode = " + ";
    if (exprBinary.op == AST_EXPR_BINARY_BITWISE_AND) opCode = " & ";
    if (exprBinary.op == AST_EXPR_BINARY_BITWISE_OR) opCode = " | ";
    if (exprBinary.op == AST_EXPR_BINARY_BITWISE_XOR) opCode = " ^ ";
    if (exprBinary.op == AST_EXPR_BINARY_DIVIDE) opCode = " / ";
    if (exprBinary.op == AST_EXPR_BINARY_EQUAL) opCode = " == ";
    if (exprBinary.op == AST_EXPR_BINARY_GREATER_EQUAL) opCode = " >= ";
    if (exprBinary.op == AST_EXPR_BINARY_GREATER_THAN) opCode = " > ";
    if (exprBinary.op == AST_EXPR_BINARY_LEFT_SHIFT) opCode = " << ";
    if (exprBinary.op == AST_EXPR_BINARY_LESS_EQUAL) opCode = " <= ";
    if (exprBinary.op == AST_EXPR_BINARY_LESS_THAN) opCode = " < ";
    if (exprBinary.op == AST_EXPR_BINARY_LOGICAL_AND) opCode = " && ";
    if (exprBinary.op == AST_EXPR_BINARY_LOGICAL_OR) opCode = " || ";
    if (exprBinary.op == AST_EXPR_BINARY_MULTIPLY) opCode = " * ";
    if (exprBinary.op == AST_EXPR_BINARY_NOT_EQUAL) opCode = " != ";
    if (exprBinary.op == AST_EXPR_BINARY_REMAINDER) opCode = " % ";
    if (exprBinary.op == AST_EXPR_BINARY_RIGHT_SHIFT) opCode = " >> ";
    if (exprBinary.op == AST_EXPR_BINARY_SUBTRACT) opCode = " - ";

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) {
    // todo
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto condCode = this->_nodeExpr(exprCond.cond);
    auto bodyCode = this->_nodeExpr(exprCond.body);
    auto altCode = this->_nodeExpr(exprCond.alt);

    return this->_wrapNodeExpr(nodeExpr, condCode + " ? " + bodyCode + " : " + altCode);
  } else if (std::holds_alternative<ASTExprLit>(*nodeExpr.body)) {
    auto exprLit = std::get<ASTExprLit>(*nodeExpr.body);

    if (exprLit.type == AST_EXPR_LIT_BOOL) {
      this->builtins.libStdbool = true;
    } else if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
      auto val = std::stoull(exprLit.body);

      if (val > 9223372036854775807) {
        return this->_wrapNodeExpr(nodeExpr, exprLit.body + "U");
      }
    } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      auto val = exprLit.body;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      return this->_wrapNodeExpr(nodeExpr, val);
    } else if (exprLit.type == AST_EXPR_LIT_STR) {
      this->builtins.fnStrInit = true;
      return this->_wrapNodeExpr(nodeExpr, "str_init(" + exprLit.body + ")");
    }

    return this->_wrapNodeExpr(nodeExpr, exprLit.body);
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
    // todo
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);
    auto argCode = this->_nodeExpr(exprUnary.arg);
    auto opCode = std::string();

    if (exprUnary.op == AST_EXPR_UNARY_BITWISE_NOT) opCode = "~";
    if (exprUnary.op == AST_EXPR_UNARY_DECREMENT) opCode = "--";
    if (exprUnary.op == AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT) opCode = "!!";
    if (exprUnary.op == AST_EXPR_UNARY_INCREMENT) opCode = "++";
    if (exprUnary.op == AST_EXPR_UNARY_LOGICAL_NOT) opCode = "!";
    if (exprUnary.op == AST_EXPR_UNARY_NEGATION) opCode = "-";
    if (exprUnary.op == AST_EXPR_UNARY_PLUS) opCode = "+";

    if (
      (exprUnary.op == AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT || exprUnary.op == AST_EXPR_UNARY_LOGICAL_NOT) &&
      (exprUnary.arg.type->isFloat() || exprUnary.arg.type->isF32() || exprUnary.arg.type->isF64())
    ) {
      this->builtins.libStdbool = true;
      argCode = "((bool) " + argCode + ")";
    } else if (
      (exprUnary.op == AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT || exprUnary.op == AST_EXPR_UNARY_LOGICAL_NOT) &&
      exprUnary.arg.type->isStr()
    ) {
      this->builtins.fnStrNot = true;
      argCode = "str_not(" + argCode + ")";
      opCode = exprUnary.op == AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT ? "!" : "";
    }

    return this->_wrapNodeExpr(nodeExpr, exprUnary.prefix ? opCode + argCode : argCode + opCode);
  }

  throw Error("Error: tried to generate code for unknown expression");
}

std::string Codegen::_type (const Type *type, bool mut) {
  auto typeName = std::string();

  if (type->isByte()) typeName = "unsigned char";
  if (type->isChar()) typeName = "char";
  if (type->isF32()) typeName = "float";
  if (type->isF64() || type->isFloat()) typeName = "double";

  if (type->isBool()) {
    this->builtins.libStdbool = true;
    typeName = "bool";
  } else if (type->isI8()) {
    this->builtins.libStdint = true;
    typeName = "int8_t";
  } else if (type->isI16()) {
    this->builtins.libStdint = true;
    typeName = "int16_t";
  } else if (type->isI32() || type->isInt()) {
    this->builtins.libStdint = true;
    typeName = "int32_t";
  } else if (type->isI64()) {
    this->builtins.libStdint = true;
    typeName = "int64_t";
  } else if (type->isStr()) {
    this->builtins.typeStr = true;
    typeName = "struct str";
  } else if (type->isU8()) {
    this->builtins.libStdint = true;
    typeName = "uint8_t";
  } else if (type->isU16()) {
    this->builtins.libStdint = true;
    typeName = "uint16_t";
  } else if (type->isU32()) {
    this->builtins.libStdint = true;
    typeName = "uint32_t";
  } else if (type->isU64()) {
    this->builtins.libStdint = true;
    typeName = "uint64_t";
  }

  return std::string(mut ? "" : "const ") + typeName + " ";
}

CodegenNode Codegen::_wrapNode (
  [[maybe_unused]] const ASTNode &node,
  const std::string &setUp,
  const std::string &code,
  const std::string &cleanUp
) const {
  return std::make_tuple(setUp, code, cleanUp);
}

std::string Codegen::_wrapNodeExpr (const ASTNodeExpr &nodeExpr, const std::string &code) const {
  auto result = code;

  if (nodeExpr.parenthesized) {
    result = "(" + result + ")";
  }

  return result;
}
