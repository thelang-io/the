/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <algorithm>
#include "Codegen.hpp"
#include "Error.hpp"

std::string codegenBlock (Codegen &, const ASTBlock &);

std::string codegenName (const std::string &name) {
  return "__THE_" + name;
}

std::string codegenType (Codegen &codegen, const Type &type, bool mut) {
  if (type.isBool()) {
    codegen.headers.stdbool = true;
  }

  if (type.isBool()) return std::string(mut ? "" : "const ") + "bool ";
  if (type.isByte()) return std::string(mut ? "" : "const ") + "unsigned char ";
  if (type.isChar()) return std::string(mut ? "" : "const ") + "char ";
  if (type.isFloat()) return std::string(mut ? "" : "const ") + "double ";
  if (type.isF32()) return std::string(mut ? "" : "const ") + "float ";
  if (type.isF64()) return std::string(mut ? "" : "const ") + "double ";
  if (type.isInt()) return std::string(mut ? "" : "const ") + "long ";
  if (type.isI8()) return std::string(mut ? "" : "const ") + "char ";
  if (type.isI16()) return std::string(mut ? "" : "const ") + "short ";
  if (type.isI32()) return std::string(mut ? "" : "const ") + "long ";
  if (type.isI64()) return std::string(mut ? "" : "const ") + "long long ";
  if (type.isStr()) return std::string(mut ? "" : "const ") + "struct str *";
  if (type.isU8()) return std::string(mut ? "" : "const ") + "unsigned char ";
  if (type.isU16()) return std::string(mut ? "" : "const ") + "unsigned short ";
  if (type.isU32()) return std::string(mut ? "" : "const ") + "unsigned long ";
  if (type.isU64()) return std::string(mut ? "" : "const ") + "unsigned long long ";

  throw Error("Tried to codegen unknown type");
}

std::string codegenTypeFormat (const Type &type) {
  if (type.isBool()) return "%s";
  if (type.isByte()) return "%x";
  if (type.isChar()) return "%c";
  if (type.isFloat()) return "%f";
  if (type.isF32()) return "%f";
  if (type.isF64()) return "%f";
  if (type.isInt()) return "%ld";
  if (type.isI8()) return "%d";
  if (type.isI16()) return "%d";
  if (type.isI32()) return "%ld";
  if (type.isI64()) return "%lld";
  if (type.isFn()) return "%s";
  if (type.isObj()) return "%s";
  if (type.isStr()) return "%s";
  if (type.isU8()) return "%u";
  if (type.isU16()) return "%u";
  if (type.isU32()) return "%lu";
  if (type.isU64()) return "%llu";

  throw Error("Tried to codegen unknown type format");
}

std::string codegenExprAccess (const ASTExprAccess &exprAccess) {
  if (std::holds_alternative<ASTId>(exprAccess)) {
    auto id = std::get<ASTId>(exprAccess);
    return codegenName(id.v.name);
  }

  throw Error("TODO 2");
}

std::tuple<std::string, std::string, std::string> codegenNodeExpr (Codegen &codegen, const ASTNodeExpr &nodeExpr) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (nodeExpr.parenthesized) {
    code += "(";
  }

  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.expr)) {
    code += codegenExprAccess(std::get<ASTExprAccess>(*nodeExpr.expr));
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.expr)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.expr);
    auto leftExprAccess = codegenExprAccess(exprAssign.left);
    auto [rightNodeExprSetUp, rightNodeExprCode, rightNodeExprCleanUp] = codegenNodeExpr(codegen, exprAssign.right);

    if (exprAssign.op == AST_EXPR_ASSIGN_COALESCE) { // TODO
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_AND) {
      code += leftExprAccess + " = " + leftExprAccess + " && " + rightNodeExprCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_OR) {
      code += leftExprAccess + " = " + leftExprAccess + " || " + rightNodeExprCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_POWER) {
      codegen.headers.math = true;
      code += leftExprAccess + " = pow(" + leftExprAccess + ", " + rightNodeExprCode + ")";
    } else {
      code += leftExprAccess;

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

      code += rightNodeExprCode;
    }

    setUp += rightNodeExprSetUp;
    cleanUp += rightNodeExprCleanUp;
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.expr)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.expr);
    auto [leftNodeExprSetUp, leftNodeExprCode, leftNodeExprCleanUp] = codegenNodeExpr(codegen, exprBinary.left);
    auto [rightNodeExprSetUp, rightNodeExprCode, rightNodeExprCleanUp] = codegenNodeExpr(codegen, exprBinary.right);

    if (exprBinary.op == AST_EXPR_BINARY_COALESCE) { // TODO
    } else if (exprBinary.op == AST_EXPR_BINARY_POWER) {
      codegen.headers.math = true;
      code += "pow(" + leftNodeExprCode + ", " + rightNodeExprCode + ")";
    } else {
      code += leftNodeExprCode;

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

      code += rightNodeExprCode;
    }

    setUp += leftNodeExprSetUp;
    cleanUp += leftNodeExprCleanUp;
    setUp += rightNodeExprSetUp;
    cleanUp += rightNodeExprCleanUp;
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.expr)) {
    auto exprCall = std::get<ASTExprCall>(*nodeExpr.expr);

    if (exprCall.fn->builtin) {
      if (exprCall.fn->name == "print") {
        codegen.headers.stdio = true;

        auto separator = std::string(R"(" ")");
        auto terminator = std::string(R"("\n")");
        auto argsCode = std::string();
        auto argIdx = static_cast<std::size_t>(0);

        for (auto arg : exprCall.args) {
          if (arg.id != std::nullopt && arg.id == "separator") {
            auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, arg.expr);

            separator = nodeExprCode + "->c";
            setUp += nodeExprSetUp;
            cleanUp += nodeExprCleanUp;
          } else if (arg.id != std::nullopt && arg.id == "terminator") {
            auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, arg.expr);

            terminator = nodeExprCode + "->c";
            setUp += nodeExprSetUp;
            cleanUp += nodeExprCleanUp;
          } else if (arg.id != std::nullopt) {
            throw Error("Unknown print argument");
          }
        }

        code += "printf(\"";

        for (auto arg : exprCall.args) {
          if (arg.id != std::nullopt) {
            continue;
          }

          code += argIdx == 0 ? "" : "%s";
          code += codegenTypeFormat(arg.expr.type);
          argsCode += argIdx == 0 ? "" : separator + ", ";

          if (arg.expr.type.isBool()) {
            argsCode += "(";
          }

          auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, arg.expr);
          argsCode += nodeExprCode;

          if (arg.expr.type.isBool()) {
            argsCode += R"() == true ? "true" : "false")";
          } else if (arg.expr.type.isStr()) {
            argsCode += "->c";
          }

          argsCode += ", ";
          setUp += nodeExprSetUp;
          cleanUp += nodeExprCleanUp;

          argIdx++;
        }

        code += R"(%s", )" + argsCode + terminator + ")";
      }
    } else {
      auto paramIdx = static_cast<std::size_t>(0);
      code += codegenName(exprCall.fn->name) + "(";

      for (auto param : exprCall.fn->params) {
        auto foundArg = std::optional<ASTExprCallArg>{};

        if (paramIdx < exprCall.args.size() && exprCall.args[paramIdx].id == std::nullopt) {
          foundArg = exprCall.args[paramIdx];
        } else {
          for (auto arg : exprCall.args) {
            if (arg.id == param.name) {
              foundArg = arg;
              break;
            }
          }
        }

        code += paramIdx == 0 ? "" : ", ";

        if (foundArg == std::nullopt) {
          if (param.type.isBool()) {
            code += "false";
          } else if (param.type.isChar()) {
            code += R"('\0')";
          } else if (param.type.isStr()) {
            code += R"("")";
          } else {
            code += "0";
          }
        } else {
          auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, foundArg->expr);

          code += nodeExprCode;
          setUp += nodeExprSetUp;
          cleanUp += nodeExprCleanUp;
        }

        paramIdx++;
      }

      code += ")";
    }
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.expr)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.expr);
    auto [condNodeExprSetUp, condNodeExprCode, condNodeExprCleanUp] = codegenNodeExpr(codegen, exprCond.cond);
    auto [bodyNodeExprSetUp, bodyNodeExprCode, bodyNodeExprCleanUp] = codegenNodeExpr(codegen, exprCond.body);
    auto [altNodeExprSetUp, altNodeExprCode, altNodeExprCleanUp] = codegenNodeExpr(codegen, exprCond.alt);

    code += condNodeExprCode;
    code += " ? ";
    code += bodyNodeExprCode;
    code += " : ";
    code += altNodeExprCode;

    setUp += condNodeExprSetUp;
    setUp += bodyNodeExprSetUp;
    setUp += altNodeExprSetUp;
    cleanUp += condNodeExprCleanUp;
    cleanUp += bodyNodeExprCleanUp;
    cleanUp += altNodeExprCleanUp;
  } else if (std::holds_alternative<ASTExprLit>(*nodeExpr.expr)) {
    auto nodeExprLit = std::get<ASTExprLit>(*nodeExpr.expr);

    if (nodeExprLit.type == AST_EXPR_LIT_INT_OCT) {
      auto val = nodeExprLit.val;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      code += val;
    } else if (nodeExprLit.type == AST_EXPR_LIT_STR) {
      codegen.functions.str_from_cstr = true;
      auto valSize = std::to_string(nodeExprLit.val.size() - 2);
      code += "str_from_cstr(" + nodeExprLit.val + ", " + valSize + ")";
    } else {
      code += nodeExprLit.val;
    }
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.expr)) {
    auto nodeExprUnary = std::get<ASTExprUnary>(*nodeExpr.expr);
    auto [argNodeExprSetUp, argNodeExprCode, argNodeExprCleanUp] = codegenNodeExpr(codegen, nodeExprUnary.arg);

    if (!nodeExprUnary.prefix) {
      code += argNodeExprCode;
    }

    if (nodeExprUnary.op == AST_EXPR_UNARY_BITWISE_NOT) code += "~";
    if (nodeExprUnary.op == AST_EXPR_UNARY_DECREMENT) code += "--";
    if (nodeExprUnary.op == AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT) code += "!!";
    if (nodeExprUnary.op == AST_EXPR_UNARY_INCREMENT) code += "++";
    if (nodeExprUnary.op == AST_EXPR_UNARY_LOGICAL_NOT) code += "!";
    if (nodeExprUnary.op == AST_EXPR_UNARY_NEGATION) code += "-";
    if (nodeExprUnary.op == AST_EXPR_UNARY_PLUS) code += "+";

    if (nodeExprUnary.prefix) {
      code += argNodeExprCode;
    }

    setUp += argNodeExprSetUp;
    cleanUp += argNodeExprCleanUp;
  }

  if (nodeExpr.parenthesized) {
    code += ")";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::string codegenNodeIf (Codegen &codegen, const ASTNodeIf &nodeIf) {
  auto code = std::string();
  auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, nodeIf.cond);

  code += "if (" + nodeExprCode + nodeExprSetUp + ") {\n";
  code += codegenBlock(codegen, nodeIf.body);
  code += nodeExprCleanUp;

  if (nodeIf.alt != std::nullopt) {
    code += std::string(codegen.indent, ' ') + "} else ";

    if (std::holds_alternative<ASTBlock>(*nodeIf.alt.value())) {
      code += "{\n";
      code += codegenBlock(codegen, std::get<ASTBlock>(*nodeIf.alt.value()));
      code += std::string(codegen.indent, ' ') + "}";
    } else if (std::holds_alternative<ASTNodeIf>(*nodeIf.alt.value())) {
      code += codegenNodeIf(codegen, std::get<ASTNodeIf>(*nodeIf.alt.value()));
    }
  } else {
    code += std::string(codegen.indent, ' ') + "}";
  }

  return code;
}

std::tuple<std::string, std::string, std::string> codegenNode (Codegen &codegen, const ASTNode &node) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (std::holds_alternative<ASTNodeBreak>(node)) {
    code += std::string(codegen.indent, ' ') + "break;\n";
  } else if (std::holds_alternative<ASTNodeContinue>(node)) {
    code += std::string(codegen.indent, ' ') + "continue;\n";
  } else if (std::holds_alternative<ASTNodeExpr>(node)) {
    auto nodeExpr = std::get<ASTNodeExpr>(node);
    auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, nodeExpr);

    setUp += nodeExprSetUp;
    code += std::string(codegen.indent, ' ') + nodeExprCode + ";\n";
    cleanUp += nodeExprCleanUp;
  } else if (std::holds_alternative<ASTNodeIf>(node)) {
    auto nodeIf = std::get<ASTNodeIf>(node);

    code += std::string(codegen.indent, ' ');
    code += codegenNodeIf(codegen, nodeIf);
    code += "\n";
  } else if (std::holds_alternative<ASTNodeLoop>(node)) {
    auto nodeLoop = std::get<ASTNodeLoop>(node);
    auto nodeLoopCleanUp = std::string();

    code += std::string(codegen.indent, ' ');

    if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
      code += "while (1)";
    } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
      auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, nodeLoop.cond.value());

      code += "while (";
      code += nodeExprSetUp;
      code += nodeExprCode;
      code += ")";

      nodeLoopCleanUp += nodeExprCleanUp;
    } else {
      code += "for (";

      if (nodeLoop.init != std::nullopt) {
        auto prevIndent = codegen.indent;
        codegen.indent = 0;

        auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, *nodeLoop.init.value());
        codegen.indent = prevIndent;

        code += nodeSetUp;
        code += nodeCode.substr(0, nodeCode.length() - 2);
        nodeLoopCleanUp += nodeCleanUp;
      }

      code += ";";

      if (nodeLoop.cond != std::nullopt) {
        auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, nodeLoop.cond.value());

        code += " " + nodeExprSetUp + nodeExprCode;
        nodeLoopCleanUp += nodeExprCleanUp;
      }

      code += ";";

      if (nodeLoop.upd != std::nullopt) {
        auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, nodeLoop.upd.value());

        code += " " + nodeExprSetUp + nodeExprCode;
        nodeLoopCleanUp += nodeExprCleanUp;
      }

      code += ")";
    }

    if (nodeLoop.body.empty() && nodeLoopCleanUp.empty()) {
      code += ";\n";
    } else {
      code += " {\n";
      code += codegenBlock(codegen, nodeLoop.body);
      code += nodeLoopCleanUp;
      code += std::string(codegen.indent, ' ') + "}\n";
    }
  } else if (std::holds_alternative<ASTNodeMain>(node)) {
    auto nodeMain = std::get<ASTNodeMain>(node);
    code += codegenBlock(codegen, nodeMain.body);
  } else if (std::holds_alternative<ASTNodeReturn>(node)) {
    auto nodeReturn = std::get<ASTNodeReturn>(node);
    auto [nodeExprSetUp, nodeExprCode, nodeExprCleanUp] = codegenNodeExpr(codegen, nodeReturn.arg);

    code += std::string(codegen.indent, ' ') + nodeExprSetUp;
    code += "return " + nodeExprCode + ";\n";
    code += nodeExprCleanUp;
  } else if (std::holds_alternative<ASTNodeVarDecl>(node)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(node);

    code += std::string(codegen.indent, ' ');
    code += codegenType(codegen, nodeVarDecl.v.type, nodeVarDecl.v.mut) + codegenName(nodeVarDecl.v.name) + " = ";

    if (nodeVarDecl.init == std::nullopt) {
      if (nodeVarDecl.v.type.isBool()) {
        codegen.headers.stdbool = true;
        code += "false";
      } else if (nodeVarDecl.v.type.isChar()) {
        code += "'\\0'";
      } else if (nodeVarDecl.v.type.isStr()) {
        codegen.functions.str_init = true;
        code += "str_init(0)";
      } else {
        code += "0";
      }
    } else if (std::holds_alternative<ASTExprAccess>(*nodeVarDecl.init->expr)) {
      auto exprAccess = std::get<ASTExprAccess>(*nodeVarDecl.init->expr);

      if (std::holds_alternative<ASTId>(exprAccess)) {
        auto id = std::get<ASTId>(exprAccess);

        if (id.v.type.isStr()) {
          codegen.functions.str_clone = true;
          code += "str_clone(" + codegenName(id.v.name) + ")";
        } else {
          code += codegenName(id.v.name);
        }
      }
    } else if (std::holds_alternative<ASTExprLit>(*nodeVarDecl.init->expr)) {
      auto exprLit = std::get<ASTExprLit>(*nodeVarDecl.init->expr);

      if (exprLit.type == AST_EXPR_LIT_BOOL) {
        codegen.headers.stdbool = true;
        code += exprLit.val;
      } else if (exprLit.type == AST_EXPR_LIT_STR) {
        codegen.functions.str_from_cstr = true;
        code += "str_from_cstr(" + exprLit.val + ", " + std::to_string(exprLit.val.length() - 2) + ")";
      } else {
        code += exprLit.val;
      }
    } else {
      auto [initNodeExprSetUp, initNodeExprCode, initNodeExprCleanUp] = codegenNodeExpr(codegen, nodeVarDecl.init.value());

      setUp += initNodeExprSetUp;
      code += initNodeExprCode;
      cleanUp += initNodeExprCleanUp;
    }

    code += ";\n";

    if (nodeVarDecl.v.type.isStr()) {
      codegen.functions.str_deinit = true;

      cleanUp += std::string(codegen.indent, ' ');
      cleanUp += "str_deinit((struct str **) &" + codegenName(nodeVarDecl.v.name) + ");\n";
    }
  }

  return std::make_tuple(setUp, code, cleanUp);
}

void codegenDeclarations (Codegen &codegen, const ASTBlock &block) {
  for (auto node : block) {
    if (std::holds_alternative<ASTNodeFnDecl>(node)) {
      auto nodeFnDecl = std::get<ASTNodeFnDecl>(node);

      codegen.functionDeclarationsCode += codegenType(codegen, nodeFnDecl.fn->returnType, true);
      codegen.functionDeclarationsCode += codegenName(nodeFnDecl.fn->name) + " (";

      if (nodeFnDecl.fn->params.empty()) {
        codegen.functionDeclarationsCode += "void";
      } else {
        auto paramsCode = std::string();
        auto paramIdx = static_cast<std::size_t>(0);

        for (auto param : nodeFnDecl.fn->params) {
          auto paramType = codegenType(codegen, param.type, true);

          paramsCode += paramIdx == 0 ? "" : ", ";
          paramsCode += paramType.back() == ' ' ? paramType.substr(0, paramType.length() - 1) : paramType;
          paramIdx++;
        }

        codegen.functionDeclarationsCode += paramsCode;
      }

      codegen.functionDeclarationsCode += ");\n";
    }
  }
}

void codegenDefinitions (Codegen &codegen, const ASTBlock &block) {
  for (auto node : block) {
    auto setUp = std::string();
    auto code = std::string();
    auto cleanUp = std::string();

    if (std::holds_alternative<ASTNodeFnDecl>(node)) {
      auto nodeFnDecl = std::get<ASTNodeFnDecl>(node);

      code += codegenType(codegen, nodeFnDecl.fn->returnType, true);
      code += codegenName(nodeFnDecl.fn->name) + " (";

      if (nodeFnDecl.fn->params.empty()) {
        code += ") {\n";
      } else {
        auto paramsCode = std::string();
        auto paramIdx = static_cast<std::size_t>(0);

        for (auto param : nodeFnDecl.fn->params) {
          paramsCode += paramIdx == 0 ? "" : ", ";
          paramsCode += codegenType(codegen, param.type, true);
          paramsCode += codegenName(param.name);

          paramIdx++;
        }

        code += paramsCode + ") {\n";
      }

      code += setUp;
      code += codegenBlock(codegen, nodeFnDecl.body);
      code += cleanUp;
      code += "}\n\n";

      codegen.functionDefinitionsCode += code;
    }
  }
}

std::string codegenBlock (Codegen &codegen, const ASTBlock &block) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  codegen.indent += 2;

  for (auto node : block) {
    auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, node);

    setUp += nodeSetUp;
    code += nodeCode;
    cleanUp += nodeCleanUp;
  }

  codegen.indent -= 2;

  codegenDeclarations(codegen, block);
  codegenDefinitions(codegen, block);

  return setUp + code + (cleanUp.empty() ? "" : "\n" + cleanUp);
}

Codegen codegen (AST *ast) {
  auto codegen = Codegen{};
  auto topLevelCode = std::string();
  auto mainCode = std::string();

  for (auto node : ast->nodes) {
    if (std::holds_alternative<ASTNodeMain>(node)) {
      continue;
    }

    auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, node);

    if (!nodeSetUp.empty() || !nodeCleanUp.empty()) {
      throw Error("Code generator returned set up or clean up data for top level statement");
    }

    topLevelCode += nodeCode;
  }

  for (auto node : ast->nodes) {
    if (std::holds_alternative<ASTNodeMain>(node)) {
      auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, node);

      if (!nodeSetUp.empty() || !nodeCleanUp.empty()) {
        throw Error("Code generator returned set up or clean up data for main function");
      }

      mainCode += nodeCode;
      break;
    }
  }

  codegenDeclarations(codegen, ast->nodes);
  codegenDefinitions(codegen, ast->nodes);

  auto mainBody = std::string("int main () {\n");
  mainBody += mainCode;
  mainBody += "}\n";

  auto builtinStructDeclarationsCode = std::string();
  auto builtinFunctionDeclarationsCode = std::string();
  auto builtinFunctionDefinitionsCode = std::string();

  if (
    codegen.functions.str_clone ||
    codegen.functions.str_init ||
    codegen.functions.str_from_cstr ||
    codegen.functions.str_deinit
  ) {

    builtinStructDeclarationsCode += "struct str {\n";
    builtinStructDeclarationsCode += "  unsigned char *c;\n";
    builtinStructDeclarationsCode += "  size_t l;\n";
    builtinStructDeclarationsCode += "};\n\n";
  }

  if (codegen.functions.str_clone) {
    codegen.functions.str_init = true;
    codegen.headers.string = true;

    builtinFunctionDeclarationsCode += "struct str *str_clone (const struct str *);\n";

    builtinFunctionDefinitionsCode += "struct str *str_clone (const struct str *n) {\n";
    builtinFunctionDefinitionsCode += "  struct str *s = str_init(n->l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(s->c, n->c, n->l);\n";
    builtinFunctionDefinitionsCode += "  return s;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  if (codegen.functions.str_from_cstr) {
    codegen.functions.str_init = true;
    codegen.headers.string = true;

    builtinFunctionDeclarationsCode += "struct str *str_from_cstr (const char *, size_t);\n";

    builtinFunctionDefinitionsCode += "struct str *str_from_cstr (const char *c, size_t l) {\n";
    builtinFunctionDefinitionsCode += "  struct str *s = str_init(l);\n";
    builtinFunctionDefinitionsCode += "  memcpy(s->c, c, l);\n";
    builtinFunctionDefinitionsCode += "  return s;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  if (codegen.functions.str_init) {
    codegen.headers.stdio = true;
    codegen.headers.stdlib = true;

    builtinFunctionDeclarationsCode += "struct str *str_init (size_t);\n";

    builtinFunctionDefinitionsCode += "struct str *str_init (size_t l) {\n";
    builtinFunctionDefinitionsCode += "  size_t z = sizeof(struct str);\n";
    builtinFunctionDefinitionsCode += "  struct str *s = malloc(z);\n\n";
    builtinFunctionDefinitionsCode += "  if (s == NULL) {\n";
    builtinFunctionDefinitionsCode += R"(    fprintf(stderr, "Error: Failed to allocate %zu bytes for string\n", z);)";
    builtinFunctionDefinitionsCode += "\n";
    builtinFunctionDefinitionsCode += "    exit(EXIT_FAILURE);\n";
    builtinFunctionDefinitionsCode += "  }\n\n";
    builtinFunctionDefinitionsCode += "  s->l = l;\n";
    builtinFunctionDefinitionsCode += "  s->c = malloc(s->l);\n\n";
    builtinFunctionDefinitionsCode += "  if (s->c == NULL) {\n";
    builtinFunctionDefinitionsCode += R"(    fprintf(stderr, "Error: )";
    builtinFunctionDefinitionsCode += "Failed to allocate %zu bytes for string content\\n\", s->l);\n";
    builtinFunctionDefinitionsCode += "    exit(EXIT_FAILURE);\n";
    builtinFunctionDefinitionsCode += "  }\n\n";
    builtinFunctionDefinitionsCode += "  return s;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  if (codegen.functions.str_deinit) {
    codegen.headers.stdlib = true;

    builtinFunctionDeclarationsCode += "void str_deinit (struct str **);\n";

    builtinFunctionDefinitionsCode += "void str_deinit (struct str **s) {\n";
    builtinFunctionDefinitionsCode += "  free((*s)->c);\n";
    builtinFunctionDefinitionsCode += "  free(*s);\n";
    builtinFunctionDefinitionsCode += "  *s = NULL;\n";
    builtinFunctionDefinitionsCode += "}\n\n";
  }

  auto headers = std::string(codegen.headers.math ? "#include <math.h>\n" : "");
  headers += codegen.headers.stdbool ? "#include <stdbool.h>\n" : "";
  headers += codegen.headers.stdio ? "#include <stdio.h>\n" : "";
  headers += codegen.headers.stdlib ? "#include <stdlib.h>\n" : "";
  headers += codegen.headers.string ? "#include <string.h>\n" : "";

  codegen.output += "/*!\n";
  codegen.output += " * Copyright (c) Aaron Delasy\n";
  codegen.output += " *\n";
  codegen.output += " * Unauthorized copying of this file, via any medium is strictly prohibited\n";
  codegen.output += " * Proprietary and confidential\n";
  codegen.output += " */\n\n";
  codegen.output += headers.empty() ? "" : headers + "\n";
  codegen.output += builtinStructDeclarationsCode;
  codegen.output += codegen.functionDeclarationsCode.empty() ? "" : codegen.functionDeclarationsCode + "\n";
  codegen.output += builtinFunctionDeclarationsCode.empty() ? "" : builtinFunctionDeclarationsCode + "\n";
  codegen.output += builtinFunctionDefinitionsCode;
  codegen.output += codegen.functionDefinitionsCode;
  codegen.output += topLevelCode.empty() ? "" : topLevelCode + "\n";
  codegen.output += mainBody;

  return codegen;
}
