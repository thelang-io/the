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
#include "config.hpp"

const auto banner = std::string(
  "/*!" EOL
  " * Copyright (c) Aaron Delasy" EOL
  " *" EOL
  " * Unauthorized copying of this file, via any medium is strictly prohibited" EOL
  " * Proprietary and confidential" EOL
  " */" EOL
  EOL
);

void Codegen::compile (const std::string &path, const std::tuple<std::string, std::string> &result, bool debug) {
  auto code = std::get<0>(result);
  auto flags = std::get<1>(result);

  auto f = std::ofstream("build/output.c");
  f << code;
  f.close();

  auto cmd = "gcc build/output.c -w -o " + path + (debug ? " -g" : "") + (flags.empty() ? "" : " " + flags);
  std::system(cmd.c_str());
  std::filesystem::remove("build/output.c");
}

std::string Codegen::name (const std::string &name) {
  return "__THE_0_" + name;
}

std::string Codegen::typeName (const std::string &name) {
  return "__THE_1_" + name;
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
    this->builtins.fnAlloc = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str cstr_concat_str (const char *, struct str);" EOL;
    builtinFunctionDefinitionsCode += "struct str cstr_concat_str (const char *c, struct str s) {" EOL;
    builtinFunctionDefinitionsCode += "  size_t l = s.l + strlen(c);" EOL;
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(r, s.c, s.l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(&r[s.l], c, l - s.l);" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnCstrEqCstr) {
    this->builtins.libStdbool = true;
    this->builtins.libString = true;

    builtinFunctionDeclarationsCode += "bool cstr_eq_cstr (const char *, const char *);" EOL;
    builtinFunctionDefinitionsCode += "bool cstr_eq_cstr (const char *c1, const char *c2) {" EOL;
    builtinFunctionDefinitionsCode += "  size_t l = strlen(c1);" EOL;
    builtinFunctionDefinitionsCode += "  return l == strlen(c2) && memcmp(c1, c2, l) == 0;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnCstrEqStr) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool cstr_eq_str (const char *, struct str);" EOL;
    builtinFunctionDefinitionsCode += "bool cstr_eq_str (const char *c, struct str s) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s.l == strlen(c) && memcmp(s.c, c, s.l) == 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnCstrNeCstr) {
    this->builtins.libStdbool = true;
    this->builtins.libString = true;

    builtinFunctionDeclarationsCode += "bool cstr_ne_cstr (const char *, const char *);" EOL;
    builtinFunctionDefinitionsCode += "bool cstr_ne_cstr (const char *c1, const char *c2) {" EOL;
    builtinFunctionDefinitionsCode += "  size_t l = strlen(c1);" EOL;
    builtinFunctionDefinitionsCode += "  return l != strlen(c2) || memcmp(c1, c2, l) != 0;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnCstrNeStr) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool cstr_ne_str (const char *, struct str);" EOL;
    builtinFunctionDefinitionsCode += "bool cstr_ne_str (const char *c, struct str s) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s.l != strlen(c) || memcmp(s.c, c, s.l) != 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrAlloc) {
    this->builtins.fnAlloc = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_alloc (const char *);" EOL;
    builtinFunctionDefinitionsCode += "struct str str_alloc (const char *c) {" EOL;
    builtinFunctionDefinitionsCode += "  size_t l = strlen(c);" EOL;
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(r, c, l);" EOL;
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrConcatCstr) {
    this->builtins.fnAlloc = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_concat_cstr (struct str, const char *);" EOL;
    builtinFunctionDefinitionsCode += "struct str str_concat_cstr (struct str s, const char *c) {" EOL;
    builtinFunctionDefinitionsCode += "  size_t l = s.l + strlen(c);" EOL;
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(r, s.c, s.l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(&r[s.l], c, l - s.l);" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrConcatStr) {
    this->builtins.fnAlloc = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_concat_str (struct str, struct str);" EOL;
    builtinFunctionDefinitionsCode += "struct str str_concat_str (struct str s1, struct str s2) {" EOL;
    builtinFunctionDefinitionsCode += "  size_t l = s1.l + s2.l;" EOL;
    builtinFunctionDefinitionsCode += "  char *r = alloc(l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(r, s1.c, s1.l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(&r[s1.l], s2.c, s2.l);" EOL;
    builtinFunctionDefinitionsCode += "  free(s1.c);" EOL;
    builtinFunctionDefinitionsCode += "  free(s2.c);" EOL;
    builtinFunctionDefinitionsCode += "  return (struct str) {r, l};" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrEqCstr) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool str_eq_cstr (struct str, const char *);" EOL;
    builtinFunctionDefinitionsCode += "bool str_eq_cstr (struct str s, const char *c) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s.l == strlen(c) && memcmp(s.c, c, s.l) == 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrEqStr) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool str_eq_str (struct str, struct str);" EOL;
    builtinFunctionDefinitionsCode += "bool str_eq_str (struct str s1, struct str s2) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s1.l == s2.l && memcmp(s1.c, s2.c, s1.l) == 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s1.c);" EOL;
    builtinFunctionDefinitionsCode += "  free(s2.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrCopy) {
    this->builtins.fnAlloc = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_copy (const struct str);" EOL;
    builtinFunctionDefinitionsCode += "struct str str_copy (const struct str s) {" EOL;
    builtinFunctionDefinitionsCode += "  char *r = alloc(s.l);" EOL;
    builtinFunctionDefinitionsCode += "  memcpy(r, s.c, s.l);" EOL;
    builtinFunctionDefinitionsCode += "  return (struct str) {r, s.l};" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrFree) {
    this->builtins.libStdlib = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "void str_free (struct str);" EOL;
    builtinFunctionDefinitionsCode += "void str_free (struct str s) {" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrNeCstr) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool str_ne_cstr (struct str, const char *);" EOL;
    builtinFunctionDefinitionsCode += "bool str_ne_cstr (struct str s, const char *c) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s.l != strlen(c) || memcmp(s.c, c, s.l) != 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrNeStr) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.libString = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool str_ne_str (struct str, struct str);" EOL;
    builtinFunctionDefinitionsCode += "bool str_ne_str (struct str s1, struct str s2) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s1.l != s2.l || memcmp(s1.c, s2.c, s1.l) != 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s1.c);" EOL;
    builtinFunctionDefinitionsCode += "  free(s2.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrNot) {
    this->builtins.libStdbool = true;
    this->builtins.libStdlib = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "bool str_not (struct str);" EOL;
    builtinFunctionDefinitionsCode += "bool str_not (struct str s) {" EOL;
    builtinFunctionDefinitionsCode += "  bool r = s.l == 0;" EOL;
    builtinFunctionDefinitionsCode += "  free(s.c);" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnStrRealloc) {
    this->builtins.libStdlib = true;
    this->builtins.typeStr = true;

    builtinFunctionDeclarationsCode += "struct str str_realloc (struct str, struct str);" EOL;
    builtinFunctionDefinitionsCode += "struct str str_realloc (struct str s1, struct str s2) {" EOL;
    builtinFunctionDefinitionsCode += "  free(s1.c);" EOL;
    builtinFunctionDefinitionsCode += "  return s2;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.fnAlloc) {
    this->builtins.libStdio = true;
    this->builtins.libStdlib = true;

    builtinFunctionDeclarationsCode += "void *alloc (size_t);" EOL;
    builtinFunctionDefinitionsCode += "void *alloc (size_t l) {" EOL;
    builtinFunctionDefinitionsCode += "  void *r = malloc(l);" EOL;
    builtinFunctionDefinitionsCode += "  if (r == NULL) {" EOL;
    builtinFunctionDefinitionsCode += R"(    fprintf(stderr, "Error: failed to allocate %zu bytes)" ESC_EOL R"(", l);)" EOL;
    builtinFunctionDefinitionsCode += "    exit(EXIT_FAILURE);" EOL;
    builtinFunctionDefinitionsCode += "  }" EOL;
    builtinFunctionDefinitionsCode += "  return r;" EOL;
    builtinFunctionDefinitionsCode += "}" EOL;
  }

  if (this->builtins.typeStr) {
    if (!this->builtins.libStdio && !this->builtins.libStdlib && !this->builtins.libString) { // todo test
      this->builtins.libStddef = true;
    }

    builtinStructDefinitionsCode += "struct str {" EOL;
    builtinStructDefinitionsCode += "  char *c;" EOL;
    builtinStructDefinitionsCode += "  size_t l;" EOL;
    builtinStructDefinitionsCode += "};" EOL;
  }

  builtinFunctionDeclarationsCode += builtinFunctionDeclarationsCode.empty() ? "" : EOL;
  builtinFunctionDefinitionsCode += builtinFunctionDefinitionsCode.empty() ? "" : EOL;
  builtinStructDefinitionsCode += builtinStructDefinitionsCode.empty() ? "" : EOL;

  auto headers = std::string(this->builtins.libStdbool ? "#include <stdbool.h>" EOL : "");
  headers += std::string(this->builtins.libStddef ? "#include <stddef.h>" EOL : ""); // todo test
  headers += std::string(this->builtins.libStdint ? "#include <stdint.h>" EOL : "");
  headers += std::string(this->builtins.libStdio ? "#include <stdio.h>" EOL : "");
  headers += std::string(this->builtins.libStdlib ? "#include <stdlib.h>" EOL : "");
  headers += std::string(this->builtins.libString ? "#include <string.h>" EOL : "");
  headers += headers.empty() ? "" : EOL;

  auto output = std::string();
  output += banner;
  output += headers;
  output += builtinStructDefinitionsCode;
  output += builtinFunctionDeclarationsCode;
  output += builtinFunctionDefinitionsCode;
  output += this->objDecl + (this->objDecl.empty() ? "" : EOL);
  output += this->objDef + (this->objDef.empty() ? "" : EOL);
  output += this->fnDecl + (this->fnDecl.empty() ? "" : EOL);
  output += this->fnDef + (this->fnDef.empty() ? "" : EOL);
  output += "int main () {" EOL;
  output += topLevelSetUp;
  output += topLevelCode;
  output += mainSetUp;
  output += mainCode;
  output += mainCleanUp;
  output += topLevelCleanUp;
  output += "}" EOL;

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

  return memberObj + "->" + Codegen::name(member.prop);
}

std::string Codegen::_flags () const {
  auto result = std::string();
  auto idx = static_cast<std::size_t>(0);

  for (const auto &flag : this->flags) {
    result += (idx++ == 0 ? "" : " ") + flag;
  }

  return result;
}

CodegenNode Codegen::_node (const ASTNode &node, bool root) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (std::holds_alternative<ASTNodeBreak>(*node.body)) {
    code = std::string(this->indent, ' ') + "break;" + EOL;
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeContinue>(*node.body)) {
    code = std::string(this->indent, ' ') + "continue;" EOL;
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeExpr>(*node.body)) { // todo test
    auto nodeExpr = std::get<ASTNodeExpr>(*node.body);

    code = (root ? std::string(this->indent, ' ') : "") + this->_nodeExpr(nodeExpr, true) + (root ? ";" EOL : "");
    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) { // todo varMap save/restore
  } else if (std::holds_alternative<ASTNodeIf>(*node.body)) {
    auto nodeIf = std::get<ASTNodeIf>(*node.body);

    this->varMap.save();
    code += std::string(this->indent, ' ') + this->_nodeIf(nodeIf) + EOL;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeLoop>(*node.body)) {
    auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
    this->varMap.save();

    if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (1)";
    } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
      code = std::string(this->indent, ' ') + "while (" + this->_nodeExpr(*nodeLoop.cond, true) + ")";
    } else if (nodeLoop.init != std::nullopt && !std::get<2>(this->_node(*nodeLoop.init)).empty()) {
      code = std::string(this->indent, ' ') + "{" EOL;
      this->indent += 2;

      // todo delete setUp from entire app?
      auto [_, initCode, initCleanUp] = this->_node(*nodeLoop.init);

      code += initCode;
      code += std::string(this->indent, ' ') + "for (;";
      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, true)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, true));
      code += ") {" EOL + this->_block(nodeLoop.body) + std::string(this->indent, ' ') + "}" EOL;
      code += initCleanUp;

      this->indent -= 2;
      code += std::string(this->indent, ' ') + "}" EOL;
      this->varMap.restore();

      return this->_wrapNode(node, setUp, code, cleanUp);
    } else {
      code = std::string(this->indent, ' ') + "for (";
      code += (nodeLoop.init == std::nullopt ? "" : std::get<1>(this->_node(*nodeLoop.init, false))) + ";";
      code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, true)) + ";";
      code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, true)) + ")";
    }

    code += " {" EOL + this->_block(nodeLoop.body) + std::string(this->indent, ' ') + "}" EOL;
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeMain>(*node.body)) {
    auto nodeMain = std::get<ASTNodeMain>(*node.body);

    this->varMap.save();
    code = this->_block(nodeMain.body);
    this->varMap.restore();

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);
    auto obj = std::get<TypeObj>(nodeObjDecl.var->type->body);
    auto type = this->_type(nodeObjDecl.var->type, true);
    auto typeName = Codegen::typeName(nodeObjDecl.var->type->name);

    this->objDecl += "struct " + typeName + ";" EOL;
    this->objDef += "struct " + typeName + " {" EOL;

    for (const auto &objField : obj.fields) {
      this->objDef += "  " + this->_type(objField.type, true) + Codegen::name(objField.name) + ";" EOL;
    }

    this->objDef += "};" EOL;

    this->builtins.fnAlloc = true;
    this->builtins.libStdlib = true;

    auto allocParamTypes = std::string();
    auto allocParams = std::string();
    auto allocCode = std::string("  " + type + "r = alloc(sizeof(struct " + typeName + "));" EOL);
    auto copyCode = std::string("  " + type + "r = alloc(sizeof(struct " + typeName + "));" EOL);
    auto freeCode = std::string();
    auto reallocCode = std::string("  " + typeName + "_free((" + type + ") o1);" EOL "  return o2;" EOL);

    for (const auto &objField : obj.fields) {
      auto objFieldName = Codegen::name(objField.name);
      auto objFieldType = this->_type(objField.type, true);

      if (objField.type->isObj()) {
        auto objFieldTypeName = Codegen::typeName(objField.type->name);

        copyCode += "  r->" + objFieldName + " = " + objFieldTypeName + "_copy(o->" + objFieldName + ");" EOL;
        freeCode += "  " + objFieldTypeName + "_free((" + objFieldType + ") o->" + objFieldName + ");" EOL;
      } else if (objField.type->isStr()) {
        this->builtins.fnStrCopy = true;
        this->builtins.fnStrFree = true;

        copyCode += "  r->" + objFieldName + " = str_copy(o->" + objFieldName + ");" EOL;
        freeCode += "  str_free((struct str) o->" + objFieldName + ");" EOL;
      } else {
        copyCode += "  r->" + objFieldName + " = o->" + objFieldName + ";" EOL;
      }

      allocParamTypes += ", " + (objFieldType.ends_with(" ") ? objFieldType.substr(0, objFieldType.size() - 1) : objFieldType);
      allocParams += ", " + objFieldType + objFieldName;
      allocCode += "  r->" + objFieldName + " = " + objFieldName + ";" EOL;
    }

    allocCode += "  return r;" EOL;
    copyCode += "  return r;" EOL;
    freeCode += "  free(o);" EOL;

    this->fnDecl += type + typeName + "_alloc (" + allocParamTypes.substr(2) + ");" EOL;
    this->fnDecl += type + typeName + "_copy (" + type + ");" EOL;
    this->fnDecl += "void " + typeName + "_free (" + type + ");" EOL;
    this->fnDecl += type + typeName + "_realloc (" + type + ", " + type + ");" EOL;

    this->fnDef += type + typeName + "_alloc (" + allocParams.substr(2) + ") {" EOL + allocCode + "}" EOL;
    this->fnDef += type + typeName + "_copy (" + type + "o) {" EOL + copyCode + "}" EOL;
    this->fnDef += "void " + typeName + "_free (" + type + "o) {" EOL + freeCode + "}" EOL;
    this->fnDef += type + typeName + "_realloc (" + type + "o1, " + type + "o2) {" EOL + reallocCode + "}" EOL;

    return this->_wrapNode(node, setUp, code, cleanUp);
  } else if (std::holds_alternative<ASTNodeReturn>(*node.body)) { // todo
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
      this->builtins.fnStrAlloc = true;
      initCode = R"(str_alloc(""))";
    }

    code = (root ? std::string(this->indent, ' ') : "") + type + name + " = " + initCode + (root ? ";" EOL : "");

    if (nodeVarDecl.var->type->isObj()) {
      auto cleanUpType = this->_type(nodeVarDecl.var->type, true);
      auto cleanUpTypeName = Codegen::typeName(nodeVarDecl.var->type->name);

      cleanUp = std::string(this->indent, ' ') + cleanUpTypeName + "_free((" + cleanUpType + ") " + name + ");" EOL;
    } else if (nodeVarDecl.var->type->isStr()) {
      this->builtins.fnStrFree = true;
      cleanUp = std::string(this->indent, ' ') + "str_free((struct str) " + name + ");" EOL;
    }

    return this->_wrapNode(node, setUp, code, cleanUp);
  }

  throw Error("Error: tried to generate code for unknown node");
}

std::string Codegen::_nodeIf (const ASTNodeIf &nodeIf) {
  auto code = "if (" + this->_nodeExpr(nodeIf.cond) + ") {" EOL + this->_block(nodeIf.body);

  if (nodeIf.alt != std::nullopt) {
    code += std::string(this->indent, ' ') + "} else ";

    if (std::holds_alternative<ASTBlock>(**nodeIf.alt)) {
      code += "{" EOL;
      code += this->_block(std::get<ASTBlock>(**nodeIf.alt));
      code += std::string(this->indent, ' ') + "}";
    } else if (std::holds_alternative<ASTNodeIf>(**nodeIf.alt)) {
      code += this->_nodeIf(std::get<ASTNodeIf>(**nodeIf.alt));
    }
  } else {
    code += std::string(this->indent, ' ') + "}";
  }

  return code;
}

std::string Codegen::_nodeExpr (const ASTNodeExpr &nodeExpr, bool root) {
  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.body)) {
    auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
    auto code = this->_exprAccess(exprAccess.body);

    if (!root && nodeExpr.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);
      code = typeName + "_copy(" + code + ")";
    } else if (!root && nodeExpr.type->isStr()) {
      this->builtins.fnStrCopy = true;
      code = "str_copy(" + code + ")";
    }

    return this->_wrapNodeExpr(nodeExpr, code);
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.body)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
    auto leftCode = this->_exprAccess(exprAssign.left.body);

    if (exprAssign.right.type->isObj()) {
      auto typeName = Codegen::typeName(nodeExpr.type->name);
      auto code = leftCode + " = " + typeName + "_realloc(" + leftCode + ", " + this->_nodeExpr(exprAssign.right) + ")";

      if (!root && nodeExpr.type->isStr()) {
        code = typeName + "_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    } else if (exprAssign.right.type->isStr()) {
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

      this->builtins.fnStrRealloc = true;
      auto code = leftCode + " = str_realloc(" + leftCode + ", " + rightCode + ")";

      if (!root && nodeExpr.type->isStr()) {
        this->builtins.fnStrCopy = true;
        code = "str_copy(" + code + ")";
      }

      return this->_wrapNodeExpr(nodeExpr, code);
    }

    auto opCode = std::string(" = ");
    auto rightCode = this->_nodeExpr(exprAssign.right);

    if (exprAssign.op == AST_EXPR_ASSIGN_AND) {
      rightCode = leftCode + " && " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_OR) {
      rightCode = leftCode + " || " + rightCode;
    } else {
      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) opCode = " += ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BIT_AND) opCode = " &= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BIT_OR) opCode = " |= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_BIT_XOR) opCode = " ^= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_DIV) opCode = " /= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_EQ) opCode = " = ";
      if (exprAssign.op == AST_EXPR_ASSIGN_LSHIFT) opCode = " <<= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_MOD) opCode = " %= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_MUL) opCode = " *= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_RSHIFT) opCode = " >>= ";
      if (exprAssign.op == AST_EXPR_ASSIGN_SUB) opCode = " -= ";
    }

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (exprBinary.left.type->isStr() && exprBinary.right.type->isStr()) {
      if (exprBinary.op == AST_EXPR_BINARY_ADD) {
        if (nodeExpr.isLit()) {
          this->builtins.fnStrAlloc = true;
          return this->_wrapNodeExpr(nodeExpr, "str_alloc(" + nodeExpr.litBody() + ")");
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
      } else if (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) {
        auto opName = std::string(exprBinary.op == AST_EXPR_BINARY_NE ? "ne" : "eq");

        if (nodeExpr.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->builtins.fnCstrEqCstr = true;
          } else {
            this->builtins.fnCstrNeCstr = true;
          }

          auto code = "cstr_" + opName + "_cstr(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")";
          return this->_wrapNodeExpr(nodeExpr, code);
        } else if (exprBinary.left.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->builtins.fnCstrEqStr = true;
          } else {
            this->builtins.fnCstrNeStr = true;
          }

          auto rightCode = this->_nodeExpr(exprBinary.right);
          return this->_wrapNodeExpr(nodeExpr, "cstr_" + opName + "_str(" + exprBinary.left.litBody() + ", " + rightCode + ")");
        } else if (exprBinary.right.isLit()) {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->builtins.fnStrEqCstr = true;
          } else {
            this->builtins.fnStrNeCstr = true;
          }

          auto leftCode = this->_nodeExpr(exprBinary.left);
          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_cstr(" + leftCode + ", " + exprBinary.right.litBody() + ")");
        } else {
          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            this->builtins.fnStrEqStr = true;
          } else {
            this->builtins.fnStrNeStr = true;
          }

          auto leftCode = this->_nodeExpr(exprBinary.left);
          auto rightCode = this->_nodeExpr(exprBinary.right);

          return this->_wrapNodeExpr(nodeExpr, "str_" + opName + "_str(" + leftCode + ", " + rightCode + ")");
        }
      }
    }

    auto leftCode = this->_nodeExpr(exprBinary.left);
    auto rightCode = this->_nodeExpr(exprBinary.right);
    auto opCode = std::string();

    if (exprBinary.op == AST_EXPR_BINARY_ADD) opCode = " + ";
    if (exprBinary.op == AST_EXPR_BINARY_AND) opCode = " && ";
    if (exprBinary.op == AST_EXPR_BINARY_BIT_AND) opCode = " & ";
    if (exprBinary.op == AST_EXPR_BINARY_BIT_OR) opCode = " | ";
    if (exprBinary.op == AST_EXPR_BINARY_BIT_XOR) opCode = " ^ ";
    if (exprBinary.op == AST_EXPR_BINARY_DIV) opCode = " / ";
    if (exprBinary.op == AST_EXPR_BINARY_EQ) opCode = " == ";
    if (exprBinary.op == AST_EXPR_BINARY_GE) opCode = " >= ";
    if (exprBinary.op == AST_EXPR_BINARY_GT) opCode = " > ";
    if (exprBinary.op == AST_EXPR_BINARY_LSHIFT) opCode = " << ";
    if (exprBinary.op == AST_EXPR_BINARY_LE) opCode = " <= ";
    if (exprBinary.op == AST_EXPR_BINARY_LT) opCode = " < ";
    if (exprBinary.op == AST_EXPR_BINARY_MOD) opCode = " % ";
    if (exprBinary.op == AST_EXPR_BINARY_MUL) opCode = " * ";
    if (exprBinary.op == AST_EXPR_BINARY_NE) opCode = " != ";
    if (exprBinary.op == AST_EXPR_BINARY_OR) opCode = " || ";
    if (exprBinary.op == AST_EXPR_BINARY_RSHIFT) opCode = " >> ";
    if (exprBinary.op == AST_EXPR_BINARY_SUB) opCode = " - ";

    return this->_wrapNodeExpr(nodeExpr, leftCode + opCode + rightCode);
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.body)) { // todo
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.body)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.body);
    auto condCode = this->_nodeExpr(exprCond.cond);
    auto bodyCode = this->_nodeExpr(exprCond.body);
    auto altCode = this->_nodeExpr(exprCond.alt);

    if (std::holds_alternative<ASTExprAssign>(*exprCond.alt.body) && !exprCond.alt.parenthesized && !exprCond.alt.type->isStr()) {
      altCode = "(" + altCode + ")";
    }

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
      this->builtins.fnStrAlloc = true;
      return this->_wrapNodeExpr(nodeExpr, "str_alloc(" + exprLit.body + ")");
    }

    return this->_wrapNodeExpr(nodeExpr, exprLit.body);
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.body)) {
    auto exprObj = std::get<ASTExprObj>(*nodeExpr.body);
    auto obj = std::get<TypeObj>(exprObj.type->body);
    auto typeName = Codegen::typeName(exprObj.type->name);
    auto fieldsCode = std::string();

    for (const auto &objField : obj.fields) {
      auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&objField] (const auto &it) -> bool {
        return it.id == objField.name;
      });

      fieldsCode += ", " + this->_nodeExpr(exprObjProp->init);
    }

    return this->_wrapNodeExpr(nodeExpr, typeName + "_alloc(" + fieldsCode.substr(2) + ")");
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.body)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);
    auto argCode = this->_nodeExpr(exprUnary.arg);
    auto opCode = std::string();

    if (exprUnary.op == AST_EXPR_UNARY_BIT_NOT) opCode = "~";
    if (exprUnary.op == AST_EXPR_UNARY_DECREMENT) opCode = "--";
    if (exprUnary.op == AST_EXPR_UNARY_INCREMENT) opCode = "++";
    if (exprUnary.op == AST_EXPR_UNARY_MINUS) opCode = "-";
    if (exprUnary.op == AST_EXPR_UNARY_NOT) opCode = "!";
    if (exprUnary.op == AST_EXPR_UNARY_PLUS) opCode = "+";

    if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isFloatNumber()) {
      this->builtins.libStdbool = true;
      argCode = "((bool) " + argCode + ")";
    } else if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isStr()) {
      this->builtins.fnStrNot = true;
      argCode = "str_not(" + argCode + ")";
      opCode = "";
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
  } else if (type->isObj()) {
    typeName = "struct " + Codegen::typeName(type->name);
    return std::string(mut ? "" : "const ") + typeName + " *";
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
