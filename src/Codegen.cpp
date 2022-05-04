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

void Codegen::compile (const std::string &path, const std::tuple<std::string, std::string> &result) {
  auto code = std::get<0>(result);
  auto flags = std::get<1>(result);

  auto f = std::ofstream("build/output.c");
  f << code;
  f.close();

  auto cmd = "gcc build/output.c -o " + path + " " + flags;
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

  auto builtinFunctionDeclarationsCode = std::string();
  auto builtinFunctionDefinitionsCode = std::string();
  auto builtinStructDefinitionsCode = std::string();

  if (this->builtins.fnCstrConcatStr) {
    this->builtins.fnStrConcatStr = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str cstr_concat_str (const char *, const struct str);\n";
    builtinFunctionDefinitionsCode += "struct str cstr_concat_str (const char *c, const struct str s) {\n";
    builtinFunctionDefinitionsCode += "  return str_concat_str((struct str) {(char *) c, strlen(c)}, s);\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrConcatCstr) {
    this->builtins.fnStrConcatStr = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_concat_cstr (const struct str, const char *);\n";
    builtinFunctionDefinitionsCode += "struct str str_concat_cstr (const struct str s, const char *c) {\n";
    builtinFunctionDefinitionsCode += "  return str_concat_str(s, (struct str) {(char *) c, strlen(c)});\n";
    builtinFunctionDefinitionsCode += "}\n";
  }

  if (this->builtins.fnStrConcatStr) {
    this->builtins.fnAlloc = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_concat_str (const struct str, const struct str);\n";
    builtinFunctionDefinitionsCode += "struct str str_concat_str (const struct str s1, const struct str s2) {\n";
    builtinFunctionDefinitionsCode += "  size_t l = s1.l + s2.l;\n";
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(r, s1.c, s1.l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(&r[s1.l], s2.c, s2.l);\n";
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

  if (this->builtins.fnStrDeinit) {
    this->builtins.libStdlib = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "void str_deinit (struct str);\n";
    builtinFunctionDefinitionsCode += "void str_deinit (struct str s) {\n";
    builtinFunctionDefinitionsCode += "  free(s.c);\n";
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

  if (this->builtins.fnStrReinit) {
    this->builtins.libStdlib = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_reinit (struct str, const struct str);\n";
    builtinFunctionDefinitionsCode += "struct str str_reinit (struct str s1, const struct str s2) {\n";
    builtinFunctionDefinitionsCode += "  free(s1.c);\n";
    builtinFunctionDefinitionsCode += "  return (struct str) {s2.c, s2.l};\n";
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

  if (this->builtins.libMath) {
    this->flags.emplace_back("-lm");
  }

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
    code += std::string(this->indent, ' ') + "break;\n";
  } else if (std::holds_alternative<ASTNodeContinue>(node.body)) {
    code += std::string(this->indent, ' ') + "continue;\n";
  } else if (std::holds_alternative<ASTNodeExpr>(node.body)) {
    auto nodeExpr = std::get<ASTNodeExpr>(node.body);
    auto nodeExprPtr = std::make_shared<ASTNodeExpr>(nodeExpr);
    auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = this->_nodeExpr(nodeExprPtr);

    code += nodeExprSetUp;
    code += std::string(this->indent, ' ') + nodeExprCode + ";\n";
    code += nodeExprCleanUp;
  } else if (std::holds_alternative<ASTNodeFnDecl>(node.body)) {
    // todo
    // todo varMap save/restore
  } else if (std::holds_alternative<ASTNodeIf>(node.body)) {
    // todo
    // todo if/else varMap save/restore
  } else if (std::holds_alternative<ASTNodeLoop>(node.body)) {
    // todo
    // todo varMap save/restore
  } else if (std::holds_alternative<ASTNodeMain>(node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(node.body);

    this->varMap.save();
    code += this->_block(nodeMain.body);
    this->varMap.restore();
  } else if (std::holds_alternative<ASTNodeObjDecl>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeReturn>(node.body)) {
    // todo
  } else if (std::holds_alternative<ASTNodeVarDecl>(node.body)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(node.body);
    auto name = Codegen::name(nodeVarDecl.var->codeName);
    auto type = this->_type(nodeVarDecl.var->type, nodeVarDecl.var->mut);

    code += std::string(this->indent, ' ') + type + name + " = ";

    if (nodeVarDecl.init == std::nullopt) {
      auto initCode = std::string("0");

      if (nodeVarDecl.var->type->isBool()) {
        initCode = "false";
      } else if (nodeVarDecl.var->type->isChar()) {
        initCode = R"('\0')";
      } else if (nodeVarDecl.var->type->isStr()) {
        this->builtins.fnStrInit = true;
        initCode = R"(str_init(""))";
      }

      code += initCode + ";\n";
    } else {
      auto [initSetUp, initCode, initCleanUp] = this->_nodeExpr(*nodeVarDecl.init, true);
      code = initSetUp + code + initCode + ";\n" + initCleanUp;
    }

    if (nodeVarDecl.var->type->isStr()) {
      this->builtins.fnStrDeinit = true;
      cleanUp += std::string(this->indent, ' ') + "str_deinit((struct str) " + name + ");\n";
    }
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::tuple<std::string, std::string, std::string> Codegen::_nodeExpr (const std::shared_ptr<ASTNodeExpr> &nodeExpr, bool root) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (nodeExpr->parenthesized) {
    code += "(";
  }

  if (std::holds_alternative<ASTExprAccess>(nodeExpr->body)) {
    auto exprAccess = std::get<ASTExprAccess>(nodeExpr->body);

    if (root && nodeExpr->type->isStr()) {
      this->builtins.fnStrCopy = true;
      code += "str_copy(" + this->_exprAccess(exprAccess.body) + ")";
    } else {
      code += this->_exprAccess(exprAccess.body);
    }
  } else if (std::holds_alternative<ASTExprAssign>(nodeExpr->body)) {
    auto exprAssign = std::get<ASTExprAssign>(nodeExpr->body);
    auto leftCode = this->_exprAccess(exprAssign.left.body);

    if (exprAssign.right->type->isStr()) {
      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
        if (exprAssign.right->isLit()) {
          this->builtins.fnStrConcatCstr = true;
          this->builtins.fnStrReinit = true;

          code += leftCode + " = str_reinit(" + leftCode + ", ";
          code += "str_concat_cstr(" + leftCode + ", " + exprAssign.right->litBody() + "))";
        } else {
          this->builtins.fnStrConcatStr = true;
          this->builtins.fnStrReinit = true;

          auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right);

          setUp += rightSetUp;
          code += leftCode + " = str_reinit(" + leftCode + ", str_concat_str(" + leftCode + ", " + rightCode + "))";
          cleanUp += rightCleanUp;
        }
      } else {
        this->builtins.fnStrReinit = true;
        auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right, true);

        setUp += rightSetUp;
        code += leftCode + " = str_reinit(" + leftCode + ", " + rightCode + ")";
        cleanUp += rightCleanUp;
      }
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_AND) {
      auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right);

      setUp += rightSetUp;
      code += leftCode + " = " + leftCode + " && " + rightCode;
      cleanUp += rightCleanUp;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_OR) {
      auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right);

      setUp += rightSetUp;
      code += leftCode + " = " + leftCode + " || " + rightCode;
      cleanUp += rightCleanUp;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_POWER) {
      this->builtins.libMath = true;
      auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right);

      setUp += rightSetUp;
      code += leftCode + " = pow(" + leftCode + ", " + rightCode + ")";
      cleanUp += rightCleanUp;
    } else {
      auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprAssign.right);

      setUp += rightSetUp;
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
      cleanUp += rightCleanUp;
    }
  } else if (std::holds_alternative<ASTExprBinary>(nodeExpr->body)) {
    auto exprBinary = std::get<ASTExprBinary>(nodeExpr->body);

    if (exprBinary.left->type->isStr() && exprBinary.right->type->isStr()) {
      if (root) {
        if (nodeExpr->isLit()) {
          this->builtins.fnStrInit = true;
          code += "str_init(" + nodeExpr->litBody() + ")";
        } else if (exprBinary.right->isLit()) {
          this->builtins.fnStrConcatCstr = true;
          auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);

          setUp += leftSetUp;
          code += "str_concat_cstr(" + leftCode + ", " + exprBinary.right->litBody() + ")";
          cleanUp += leftCleanUp;
        } else if (exprBinary.left->isLit()) {
          this->builtins.fnCstrConcatStr = true;
          auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);

          setUp += rightSetUp;
          code += "cstr_concat_str(" + exprBinary.left->litBody() + ", " + rightCode + ")";
          cleanUp += rightCleanUp;
        } else {
          this->builtins.fnStrConcatStr = true;

          auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);
          auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);

          setUp += leftSetUp + rightSetUp;
          code += "str_concat_str(" + leftCode + ", " + rightCode + ")";
          cleanUp += leftCleanUp + rightCleanUp;
        }
      } else {
        if (nodeExpr->isLit()) {
          this->builtins.fnStrDeinit = true;
          this->builtins.fnStrInit = true;

          auto randomVar = this->varMap.add("", this->varMap.name(""), nullptr, false);
          auto randomVarName = randomVar->codeName;

          setUp += std::string(this->indent, ' ') + "const struct str " + randomVarName + " = ";
          setUp += "str_init(" + nodeExpr->litBody() + ");\n";
          code += randomVarName;
          cleanUp += std::string(this->indent, ' ') + "str_deinit((struct str) " + randomVarName + ");\n";
        } else if (exprBinary.left->isLit()) {
          this->builtins.fnCstrConcatStr = true;
          this->builtins.fnStrDeinit = true;

          auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);
          auto randomVar = this->varMap.add("", this->varMap.name(""), nullptr, false);
          auto randomVarName = randomVar->codeName;

          setUp += std::string(this->indent, ' ') + "const struct str " + randomVarName + " = ";
          setUp += "cstr_concat_str(" + exprBinary.left->litBody() + ", " + rightCode + ");\n" + rightSetUp;
          code += randomVarName;
          cleanUp += rightCleanUp;
          cleanUp += std::string(this->indent, ' ') + "str_deinit((struct str) " + randomVarName + ");\n";
        } else if (exprBinary.right->isLit()) {
          this->builtins.fnStrConcatCstr = true;
          this->builtins.fnStrDeinit = true;

          auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);
          auto randomVar = this->varMap.add("", this->varMap.name(""), nullptr, false);
          auto randomVarName = randomVar->codeName;

          setUp += leftSetUp + std::string(this->indent, ' ') + "const struct str " + randomVarName + " = ";
          setUp += "str_concat_cstr(" + leftCode + ", " + exprBinary.right->litBody() + ");\n";
          code += randomVarName;
          cleanUp += std::string(this->indent, ' ') + "str_deinit((struct str) " + randomVarName + ");\n" + leftCleanUp;
        } else {
          this->builtins.fnStrConcatStr = true;
          this->builtins.fnStrDeinit = true;

          auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);
          auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);
          auto randomVar = this->varMap.add("", this->varMap.name(""), nullptr, false);
          auto randomVarName = randomVar->codeName;

          setUp += leftSetUp + rightSetUp;
          setUp += std::string(this->indent, ' ') + "const struct str " + randomVarName + " = ";
          setUp += "str_concat_str(" + leftCode + ", " + rightCode + ");\n";
          code += randomVarName;
          cleanUp += std::string(this->indent, ' ') + "str_deinit((struct str) " + randomVarName + ");\n";
          cleanUp += leftCleanUp + rightCleanUp;
        }
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_POWER) {
      this->builtins.libMath = true;

      auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);
      auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);

      setUp += leftSetUp + rightSetUp;
      code += "pow(" + leftCode + ", " + rightCode + ")";
      cleanUp += leftCleanUp + rightCleanUp;
    } else {
      auto [leftSetUp, leftCode, leftCleanUp] = this->_nodeExpr(exprBinary.left);
      auto [rightSetUp, rightCode, rightCleanUp] = this->_nodeExpr(exprBinary.right);

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

      setUp += leftSetUp + rightSetUp;
      code += rightCode;
      cleanUp += leftCleanUp + rightCleanUp;
    }
  } else if (std::holds_alternative<ASTExprCall>(nodeExpr->body)) {
    // todo
  } else if (std::holds_alternative<ASTExprCond>(nodeExpr->body)) {
    auto exprCond = std::get<ASTExprCond>(nodeExpr->body);
    auto [condSetUp, condCode, condCleanUp] = this->_nodeExpr(exprCond.cond);
    auto [bodySetUp, bodyCode, bodyCleanUp] = this->_nodeExpr(exprCond.body, root);
    auto [altSetUp, altCode, altCleanUp] = this->_nodeExpr(exprCond.alt, root);

    setUp += condSetUp + bodySetUp + altSetUp;
    code += condCode + " ? " + bodyCode + " : " + altCode;
    cleanUp += condCleanUp + bodyCleanUp + altCleanUp;
  } else if (std::holds_alternative<ASTExprLit>(nodeExpr->body)) {
    auto exprLit = std::get<ASTExprLit>(nodeExpr->body);

    if (exprLit.type == AST_EXPR_LIT_BOOL) {
      this->builtins.libStdbool = true;
    }

    if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
      auto val = std::stoul(exprLit.body);
      code += exprLit.body;

      if (val > 9223372036854775807) {
        code += "U";
      }
    } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      auto val = exprLit.body;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      code += val;
    } else if (exprLit.type == AST_EXPR_LIT_STR) {
      if (root) {
        this->builtins.fnStrInit = true;
        code += "str_init(" + exprLit.body + ")";
      } else {
        this->builtins.fnStrDeinit = true;
        this->builtins.fnStrInit = true;

        auto randomVar = this->varMap.add("", this->varMap.name(""), nullptr, false);
        auto randomVarName = randomVar->codeName;

        setUp += std::string(this->indent, ' ') + "const struct str " + randomVarName + " = ";
        setUp += "str_init(" + exprLit.body + ");\n";
        code += randomVarName;
        cleanUp += std::string(this->indent, ' ') + "str_deinit((struct str) " + randomVarName + ");\n";
      }
    } else {
      code += exprLit.body;
    }
  } else if (std::holds_alternative<ASTExprObj>(nodeExpr->body)) {
    // todo
  } else if (std::holds_alternative<ASTExprUnary>(nodeExpr->body)) {
    auto exprUnary = std::get<ASTExprUnary>(nodeExpr->body);
    auto [argSetUp, argCode, argCleanUp] = this->_nodeExpr(exprUnary.arg);

    setUp += argSetUp;

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

    cleanUp += argCleanUp;
  }

  if (nodeExpr->parenthesized) {
    code += ")";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::string Codegen::_type (const std::shared_ptr<Type> &type, bool mut) {
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
