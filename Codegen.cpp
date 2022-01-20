/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Codegen.hpp"
#include "Error.hpp"
#include "Foundation.hpp"

std::string codegenForwardNode (Codegen &, const ASTNode &);
std::tuple<std::string, std::string, std::string> codegenNode (Codegen &, const ASTNode &);

std::string codegenBuiltinName (const std::string &name) {
  return "builtin_" + name;
}

std::string codegenName (const std::string &name) {
  return "__THE_0_" + name;
}

std::string codegenTypeName (const std::string &name) {
  return "__THE_1_" + name;
}

std::string codegenFnParamsName (const std::string &name) {
  return "__THE_1_" + name + "_p";
}

std::string codegenFnReturnName (const std::string &name) {
  return "__THE_1_" + name + "_r";
}

std::string codegenFnStackName (const std::string &name) {
  return "__THE_1_" + name + "_t";
}

std::string codegenType (const std::shared_ptr<Type> &type, bool mut) {
  if (type->isBool()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("bool") + " *";
  if (type->isByte()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("byte") + " *";
  if (type->isChar()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("char") + " *";
  if (type->isFloat()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("float") + " *";
  if (type->isF32()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("f32") + " *";
  if (type->isF64()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("f64") + " *";
  if (type->isFn()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("fn") + " *";
  if (type->isInt()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("int") + " *";
  if (type->isI8()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("i8") + " *";
  if (type->isI16()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("i16") + " *";
  if (type->isI32()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("i32") + " *";
  if (type->isI64()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("i64") + " *";
  if (type->isObj()) return std::string(mut ? "" : "const ") + "struct " + codegenTypeName(type->name) + " *";
  if (type->isStr()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("str") + " *";
  if (type->isU8()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("u8") + " *";
  if (type->isU16()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("u16") + " *";
  if (type->isU32()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("u32") + " *";
  if (type->isU64()) return std::string(mut ? "" : "const ") + "struct " + codegenBuiltinName("u64") + " *";

  throw Error("Tried to code generate unknown type");
}

std::string codegenTypeStr (const std::shared_ptr<Type> &type) {
  if (type->isBool()) return codegenBuiltinName("bool");
  if (type->isByte()) return codegenBuiltinName("byte");
  if (type->isChar()) return codegenBuiltinName("char");
  if (type->isFloat()) return codegenBuiltinName("float");
  if (type->isF32()) return codegenBuiltinName("f32");
  if (type->isF64()) return codegenBuiltinName("f64");
  if (type->isFn()) return codegenBuiltinName("fn");
  if (type->isInt()) return codegenBuiltinName("int");
  if (type->isI8()) return codegenBuiltinName("i8");
  if (type->isI16()) return codegenBuiltinName("i16");
  if (type->isI32()) return codegenBuiltinName("i32");
  if (type->isI64()) return codegenBuiltinName("i64");
  if (type->isObj()) return codegenTypeName(type->name);
  if (type->isStr()) return codegenBuiltinName("str");
  if (type->isU8()) return codegenBuiltinName("u8");
  if (type->isU16()) return codegenBuiltinName("u16");
  if (type->isU32()) return codegenBuiltinName("u32");
  if (type->isU64()) return codegenBuiltinName("u64");

  throw Error("Tried to code generate unknown type string");
}

std::string codegenExprAccess (const ASTExprAccess &exprAccess) {
  if (std::holds_alternative<ASTId>(exprAccess)) {
    auto id = std::get<ASTId>(exprAccess);
    return codegenName(id.v.name);
  }

  auto member = std::get<ASTMember>(exprAccess);
  auto memberObj = codegenExprAccess(*member.obj);

  return memberObj + "->" + codegenName(member.prop);
}

std::string codegenBlock (Codegen &codegen, const ASTBlock &block) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  codegen.indent += 2;

  for (const auto &node : block) {
    setUp += codegenForwardNode(codegen, node);
  }

  for (const auto &node : block) {
    auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, node);

    setUp += nodeSetUp;
    code += nodeCode;
    cleanUp += nodeCleanUp;
  }

  codegen.indent -= 2;
  return setUp + code + cleanUp;
}

std::string codegenForwardNode (Codegen &codegen, const ASTNode &node) {
  auto code = std::string();

  if (std::holds_alternative<ASTNodeFnDecl>(node)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(node);
    auto varName = codegenName(nodeFnDecl.var.name);
    auto typeName = codegenTypeName(nodeFnDecl.var.type->name);

    code += std::string(codegen.indent, ' ') + "struct " + codegenBuiltinName("fn") + " *" + varName + " = " + codegenBuiltinName("fn_init") + "(" + typeName + ");\n";
  }

  return code;
}

std::string codegenNodeExprInit (const std::shared_ptr<Type> &type, const std::string &nodeExprCode) {
  auto typeStr = codegenTypeStr(type);
  return typeStr + "_init(" + nodeExprCode + ")";
}

std::string codegenNodeExprDeinit (const std::shared_ptr<Type> &type, const std::string &varName) {
  auto typeStr = codegenTypeStr(type);
  return typeStr + "_deinit((struct " + typeStr + " **) &" + varName + ")";
}

std::tuple<std::string, std::string, std::string> codegenNodeExpr (Codegen &codegen, const ASTNodeExpr &nodeExpr) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();

  if (nodeExpr.parenthesized) {
    code += "(";
  }

  if (std::holds_alternative<ASTExprAccess>(*nodeExpr.expr)) {
    code += codegenExprAccess(std::get<ASTExprAccess>(*nodeExpr.expr)) + "->v";
  } else if (std::holds_alternative<ASTExprAssign>(*nodeExpr.expr)) {
    auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.expr);
    auto leftCode = codegenExprAccess(exprAssign.left) + "->v";
    auto [rightSetUp, rightCode, rightCleanUp] = codegenNodeExpr(codegen, exprAssign.right);

    if (exprAssign.op == AST_EXPR_ASSIGN_COALESCE) { // TODO optionals
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_AND) {
      code += leftCode + " = " + leftCode + " && " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_LOGICAL_OR) {
      code += leftCode + " = " + leftCode + " || " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_POWER) {
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
  } else if (std::holds_alternative<ASTExprBinary>(*nodeExpr.expr)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.expr);
    auto [leftSetUp, leftCode, leftCleanUp] = codegenNodeExpr(codegen, exprBinary.left);
    auto [rightSetUp, rightCode, rightCleanUp] = codegenNodeExpr(codegen, exprBinary.right);

    if (exprBinary.op == AST_EXPR_BINARY_COALESCE) { // TODO optionals
    } else if (exprBinary.op == AST_EXPR_BINARY_POWER) {
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
  } else if (std::holds_alternative<ASTExprCall>(*nodeExpr.expr)) {
    auto exprCall = std::get<ASTExprCall>(*nodeExpr.expr);

    if (exprCall.type->name == "print") {
      auto separator = std::string(R"(" ")");
      auto terminator = std::string(R"("\n")");
      auto argsCode = std::string();
      auto argIdx = static_cast<std::size_t>(0);

      for (const auto &exprCallArg : exprCall.args) {
        if (exprCallArg.id != std::nullopt && exprCallArg.id == "separator") {
          if (std::holds_alternative<ASTExprLit>(*exprCallArg.expr.expr)) {
            auto exprLit = std::get<ASTExprLit>(*exprCallArg.expr.expr);
            separator = exprLit.val;

            continue;
          }

          auto [argSetUp, argCode, argCleanUp] = codegenNodeExpr(codegen, exprCallArg.expr);

          setUp += argSetUp;
          separator = argCode + "->c";
          cleanUp += argCleanUp;
        } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "terminator") {
          if (std::holds_alternative<ASTExprLit>(*exprCallArg.expr.expr)) {
            auto exprLit = std::get<ASTExprLit>(*exprCallArg.expr.expr);
            separator = exprLit.val;

            continue;
          }

          auto [argSetUp, argCode, argCleanUp] = codegenNodeExpr(codegen, exprCallArg.expr);

          setUp += argSetUp;
          terminator = argCode + "->c";
          cleanUp += argCleanUp;
        }
      }

      code += "printf(\"";

      for (const auto &exprCallArg : exprCall.args) {
        if (exprCallArg.id != std::nullopt) {
          continue;
        }

        code += std::string(argIdx == 0 ? "" : "%s") + "%s";
        argsCode += argIdx == 0 ? "" : separator + ", ";

        if (std::holds_alternative<ASTExprLit>(*exprCallArg.expr.expr) && std::get<ASTExprLit>(*exprCallArg.expr.expr).type == AST_EXPR_LIT_STR) {
          auto exprLit = std::get<ASTExprLit>(*exprCallArg.expr.expr);
          argsCode += exprLit.val + ", ";

          continue;
        }

        auto printAnonVar = codegen.anonMap.add("p");
        auto exprType = codegenType(exprCallArg.expr.type, false);
        auto exprTypeStr = codegenTypeStr(exprCallArg.expr.type);
        auto [argSetUp, argCode, argCleanUp] = codegenNodeExpr(codegen, exprCallArg.expr);

        setUp += argSetUp;
        cleanUp += argCleanUp;

        if (std::holds_alternative<ASTExprAccess>(*exprCallArg.expr.expr)) {
          setUp += std::string(codegen.indent, ' ') + "char *" + printAnonVar + " = " + exprTypeStr + "_to_cstr(" + argCode.substr(0, argCode.length() - 3) + ");\n";
        } else {
          auto argAnonVar = codegen.anonMap.add("a");

          setUp += std::string(codegen.indent, ' ') + exprType + argAnonVar + " = " + codegenNodeExprInit(exprCallArg.expr.type, argCode) + ";\n";
          setUp += std::string(codegen.indent, ' ') + "char *" + printAnonVar + " = " + exprTypeStr + "_to_cstr(" + argAnonVar + ");\n";
          cleanUp += std::string(codegen.indent, ' ') + codegenNodeExprDeinit(exprCallArg.expr.type, argAnonVar) + ";\n";
        }

        argsCode += printAnonVar + ", ";
        cleanUp += std::string(codegen.indent, ' ') + "free(" + printAnonVar + ");\n";
        cleanUp += std::string(codegen.indent, ' ') + printAnonVar + " = NULL;\n";
        argIdx++;
      }

      code += R"(%s", )" + argsCode + terminator + ")";
    } else {
      auto fn = std::get<TypeFn>(exprCall.type->body);
      auto typeName = codegenType(fn.type, true);
      auto calleeName = codegenExprAccess(exprCall.callee);
      auto callAnonVar = codegen.anonMap.add("c");
      auto fnParamsName = codegenFnParamsName(exprCall.type->name);
      auto fnReturnName = codegenFnReturnName(exprCall.type->name);
      auto paramIdx = static_cast<std::size_t>(0);
      auto callDef = std::string();

      callDef += "const " + typeName + callAnonVar + " = " + codegenBuiltinName("fn_call") + "(" + calleeName + ", ";

      if (fn.params.empty()) {
        callDef += "NULL";
      } else {
        callDef += "&((struct " + fnParamsName + ") {";

        for (const auto &fnParam : fn.params) {
          auto foundArg = std::optional<ASTExprCallArg>{};

          if (paramIdx < exprCall.args.size() && exprCall.args[paramIdx].id == std::nullopt) {
            foundArg = exprCall.args[paramIdx];
          } else {
            for (const auto &exprCallArg : exprCall.args) {
              if (exprCallArg.id == fnParam.name) {
                foundArg = exprCallArg;
                break;
              }
            }
          }

          callDef += paramIdx == 0 ? "" : ", ";

          if (foundArg == std::nullopt) {
            callDef += "NULL";
          } else {
            auto [exprSetUp, exprCode, exprCleanUp] = codegenNodeExpr(codegen, foundArg->expr);

            setUp += exprSetUp;
            callDef += codegenNodeExprInit(foundArg->expr.type, exprCode);
            cleanUp += exprCleanUp;
          }

          paramIdx++;
        }

        callDef += "})";
      }

      setUp += std::string(codegen.indent, ' ') + callDef + ");\n";
      code += callAnonVar;
    }
  } else if (std::holds_alternative<ASTExprCond>(*nodeExpr.expr)) {
    auto exprCond = std::get<ASTExprCond>(*nodeExpr.expr);
    auto [condSetUp, condCode, condCleanUp] = codegenNodeExpr(codegen, exprCond.cond);
    auto [bodySetUp, bodyCode, bodyCleanUp] = codegenNodeExpr(codegen, exprCond.body);
    auto [altSetUp, altCode, altCleanUp] = codegenNodeExpr(codegen, exprCond.alt);

    code += condCode;
    code += " ? ";
    code += bodyCode;
    code += " : ";
    code += altCode;

    setUp += condSetUp + bodySetUp + altSetUp;
    cleanUp += condCleanUp + bodyCleanUp + altCleanUp;
  } else if (std::holds_alternative<ASTExprLit>(*nodeExpr.expr)) {
    auto exprLit = std::get<ASTExprLit>(*nodeExpr.expr);

    if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
      auto val = exprLit.val;

      val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
      val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());

      code += val;
    } else {
      code += exprLit.val;
    }
  } else if (std::holds_alternative<ASTExprObj>(*nodeExpr.expr)) {
    auto exprObj = std::get<ASTExprObj>(*nodeExpr.expr);
    auto obj = std::get<TypeObj>(exprObj.type->body);
    auto fieldIdx = static_cast<std::size_t>(0);
    auto typeName = codegenTypeName(exprObj.type->name);

    code += typeName + "_init((const struct " + typeName + ") {";

    for (const auto &objField : obj.fields) {
      auto exprObjProp = std::find_if(exprObj.props.begin(), exprObj.props.end(), [&objField] (const auto &it) -> bool {
        return it.name == objField.name;
      });

      auto [initSetUp, initCode, initCleanUp] = codegenNodeExpr(codegen, exprObjProp->init);

      setUp += initSetUp;
      code += (fieldIdx == 0 ? "" : ", ") + initCode;
      cleanUp += initCleanUp;

      fieldIdx++;
    }

    code += "})";
  } else if (std::holds_alternative<ASTExprUnary>(*nodeExpr.expr)) {
    auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.expr);
    auto [argSetUp, argCode, argCleanUp] = codegenNodeExpr(codegen, exprUnary.arg);

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

  if (nodeExpr.parenthesized) {
    code += ")";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

std::string codegenNodeIf (Codegen &codegen, const ASTNodeIf &nodeIf) {
  auto setUp = std::string();
  auto code = std::string();
  auto cleanUp = std::string();
  auto [condSetUp, condCode, condCleanUp] = codegenNodeExpr(codegen, nodeIf.cond);

  code += "if (" + condCode + ") {\n";
  code += codegenBlock(codegen, nodeIf.body);
  setUp += condSetUp;
  cleanUp += condCleanUp;

  if (nodeIf.alt != std::nullopt) {
    code += std::string(codegen.indent, ' ') + "} else ";

    if (std::holds_alternative<ASTBlock>(**nodeIf.alt)) {
      code += "{\n";
      code += codegenBlock(codegen, std::get<ASTBlock>(**nodeIf.alt));
      code += std::string(codegen.indent, ' ') + "}";
    } else if (std::holds_alternative<ASTNodeIf>(**nodeIf.alt)) {
      code += codegenNodeIf(codegen, std::get<ASTNodeIf>(**nodeIf.alt));
    }
  } else {
    code += std::string(codegen.indent, ' ') + "}";
  }

  return setUp + code + cleanUp;
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

    code += nodeExprSetUp;
    code += std::string(codegen.indent, ' ') + nodeExprCode + ";\n";
    code += nodeExprCleanUp;
  } else if (std::holds_alternative<ASTNodeFnDecl>(node)) {
    auto nodeFnDecl = std::get<ASTNodeFnDecl>(node);
    auto fn = std::get<TypeFn>(nodeFnDecl.var.type->body);
    auto varName = codegenName(nodeFnDecl.var.name);
    auto typeName = codegenTypeName(nodeFnDecl.var.type->name);
    auto paramsFields = std::vector<std::tuple<std::string, std::string>>{};
    auto defCode = std::string();
    auto defCleanUp = std::string();

    auto prevIndent = codegen.indent;
    codegen.indent = 0;

    if (!fn.params.empty()) {
      auto paramsName = codegenFnParamsName(nodeFnDecl.var.type->name);

      defCode += "  struct " + paramsName + " *p = pp;\n";

      for (const auto &fnParam : fn.params) {
        auto nodeFnDeclParam = std::find_if(nodeFnDecl.params.begin(), nodeFnDecl.params.end(), [&fnParam] (const auto &it) -> bool {
          return it.var.name == fnParam.name;
        });

        auto fnParamVarName = codegenName(fnParam.name);
        auto fnParamVarType = codegenType(fnParam.type, true);
        auto fnParamVarTypeStr = codegenTypeStr(fnParam.type);
        auto fnParamCode = "  " + fnParamVarType + fnParamVarName + " = ";

        if (fnParam.required) {
          fnParamCode += "p->" + fnParamVarName;
        } else {
          auto [initSetUp, initCode, initCleanUp] = codegenNodeExpr(codegen, *nodeFnDeclParam->init);

          defCode += initSetUp;
          defCleanUp += initCleanUp;
          fnParamCode += "p->" + fnParamVarName + " == NULL ? " + codegenNodeExprInit(fnParam.type, initCode) + " : p->" + fnParamVarName;
        }

        defCode += fnParamCode + ";\n";
        paramsFields.emplace_back(std::make_tuple(fnParamVarName, fnParamVarType));
      }


      codegen.structDeclarations.push_back(paramsName);
      codegen.structDefinitions[paramsName] = paramsFields;
    }

    if (!nodeFnDecl.stack.empty()) {
      auto stackName = codegenFnStackName(nodeFnDecl.var.type->name);
      auto stackCode = std::string();
      auto stackFields = std::vector<std::tuple<std::string, std::string>>{};

      defCode += "  struct " + stackName + " *t = pt;\n";

      for (const auto &nodeFnDeclStackVar : nodeFnDecl.stack) {
        auto nodeFnDeclStackVarName = codegenName(nodeFnDeclStackVar.name);
        auto nodeFnDeclStackVarType = codegenType(nodeFnDeclStackVar.type, nodeFnDeclStackVar.mut);

        auto paramsField = std::find_if(paramsFields.begin(), paramsFields.end(), [&nodeFnDeclStackVarName] (const auto &it) -> bool {
          return std::get<0>(it) == nodeFnDeclStackVarName;
        });

        if (paramsField != paramsFields.end()) {
          continue;
        }

        defCode += "  " + nodeFnDeclStackVarType + nodeFnDeclStackVarName + " = t->" + nodeFnDeclStackVarName + ";\n";
        stackCode += ", " + nodeFnDeclStackVarName;
        stackFields.emplace_back(std::make_tuple(nodeFnDeclStackVarName, nodeFnDeclStackVarType));
      }

      code += std::string(codegen.indent, ' ') + varName + "->t = &((struct " + stackName + ") {" + stackCode.substr(2) + "});\n";
      codegen.structDeclarations.push_back(stackName);
      codegen.structDefinitions[stackName] = stackFields;
    }

    defCode += codegenBlock(codegen, nodeFnDecl.body);
    defCode += defCleanUp;
    codegen.indent = prevIndent;

    codegen.functionDeclarations[typeName] = std::make_tuple("void *", "(void *, void *)");
    codegen.functionDefinitions[typeName] = std::make_tuple("void *", "(void *pt, void *pp)", defCode);

    cleanUp += std::string(codegen.indent, ' ') + codegenBuiltinName("fn_deinit") + "((struct " + codegenBuiltinName("fn") + " **) &" + varName + ");\n";
  } else if (std::holds_alternative<ASTNodeIf>(node)) {
    auto nodeIf = std::get<ASTNodeIf>(node);

    code += std::string(codegen.indent, ' ');
    code += codegenNodeIf(codegen, nodeIf);
    code += "\n";
  } else if (std::holds_alternative<ASTNodeLoop>(node)) {
    auto nodeLoop = std::get<ASTNodeLoop>(node);
    auto nodeLoopSetUp = std::string();
    auto nodeLoopCleanUp = std::string();

    code += std::string(codegen.indent, ' ') + "{\n";
    codegen.indent += 2;

    auto nodeLoopCode = std::string(codegen.indent, ' ');

    if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
      nodeLoopCode += "while (true)";
    } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
      auto [condSetUp, condCode, condCleanUp] = codegenNodeExpr(codegen, *nodeLoop.cond);

      nodeLoopSetUp += condSetUp;
      nodeLoopCode += "while (" + condCode + ")";
      nodeLoopCleanUp += condCleanUp;
    } else {
      nodeLoopCode += "for (";

      if (nodeLoop.init != std::nullopt) {
        auto [initSetUp, initCode, initCleanUp] = codegenNode(codegen, **nodeLoop.init);

        nodeLoopSetUp += initSetUp;
        nodeLoopSetUp += initCode;
        nodeLoopCleanUp += initCleanUp;
      }

      nodeLoopCode += ";";

      if (nodeLoop.cond != std::nullopt) {
        auto [condSetUp, condCode, condCleanUp] = codegenNodeExpr(codegen, *nodeLoop.cond);

        nodeLoopSetUp += condSetUp;
        nodeLoopCode += " " + condCode;
        nodeLoopCleanUp += condCleanUp;
      }

      nodeLoopCode += ";";

      if (nodeLoop.upd != std::nullopt) {
        auto [updSetUp, updCode, updCleanUp] = codegenNodeExpr(codegen, *nodeLoop.upd);

        nodeLoopSetUp += updSetUp;
        nodeLoopCode += " " + updCode;
        nodeLoopCleanUp += updCleanUp;
      }

      nodeLoopCode += ")";
    }

    nodeLoopCode += " {\n";
    nodeLoopCode += codegenBlock(codegen, nodeLoop.body);
    nodeLoopCode += std::string(codegen.indent, ' ') + "}\n";

    code += nodeLoopSetUp;
    code += nodeLoopCode;
    code += nodeLoopCleanUp;

    codegen.indent -= 2;
    code += std::string(codegen.indent, ' ') + "}\n";
  } else if (std::holds_alternative<ASTNodeMain>(node)) {
    auto nodeMain = std::get<ASTNodeMain>(node);
    code += codegenBlock(codegen, nodeMain.body);
  } else if (std::holds_alternative<ASTNodeObjDecl>(node)) {
    auto nodeObjDecl = std::get<ASTNodeObjDecl>(node);
    auto obj = std::get<TypeObj>(nodeObjDecl.var.type->body);
    auto typeName = codegenTypeName(nodeObjDecl.var.type->name);
    auto typeNameStruct = "struct " + typeName;
    auto fields = std::vector<std::tuple<std::string, std::string>>{};

    for (const auto &objField : obj.fields) {
      fields.emplace_back(std::make_tuple(codegenName(objField.name), codegenType(objField.type, true)));
    }

    codegen.structDeclarations.push_back(typeName);
    codegen.structDefinitions[typeName] = fields;

    auto initName = typeName + "_init";
    auto initDefCode = std::string();

    initDefCode += "  size_t l = sizeof(" + typeNameStruct + ");\n";
    initDefCode += "  " + codegenType(nodeObjDecl.var.type, true) + "r = malloc(l);\n";
    initDefCode += "  if (r == NULL) {\n";
    initDefCode += R"(    fprintf(stderr, "Error: Failed to allocate %zu bytes for object \")" + nodeObjDecl.var.name + R"(\"\n", l);)" + "\n";
    initDefCode += "    exit(EXIT_FAILURE);\n";
    initDefCode += "  }\n";
    initDefCode += "  memcpy(r, &n, l);\n";
    initDefCode += "  return r;\n";

    codegen.functionDeclarations[initName] = std::make_tuple(typeNameStruct + " *", "(const " + typeNameStruct + ")");
    codegen.functionDefinitions[initName] = std::make_tuple(typeNameStruct + " *", "(const " + typeNameStruct + " n)", initDefCode);

    auto deinitName = typeName + "_deinit";

    codegen.functionDeclarations[deinitName] = std::make_tuple("void ", "(" + typeNameStruct + " **)");
  } else if (std::holds_alternative<ASTNodeReturn>(node)) {
    auto nodeReturn = std::get<ASTNodeReturn>(node);
    auto argTypeName = codegenType(nodeReturn.arg.type, true);
    auto argTypeStr = codegenTypeStr(nodeReturn.arg.type);
    auto [argSetUp, argCode, argCleanUp] = codegenNodeExpr(codegen, nodeReturn.arg);
    auto returnAnonVar = codegen.anonMap.add("r");

    code += argSetUp;
    code += std::string(codegen.indent, ' ') + argTypeName + returnAnonVar + " = " + codegenNodeExprInit(nodeReturn.arg.type, argCode) + ";\n";
    code += argCleanUp;
    cleanUp += std::string(codegen.indent, ' ') + "return " + returnAnonVar + ";\n";
  } else if (std::holds_alternative<ASTNodeVarDecl>(node)) {
    auto nodeVarDecl = std::get<ASTNodeVarDecl>(node);
    auto varName = codegenName(nodeVarDecl.var.codeName);
    auto varType = codegenType(nodeVarDecl.var.type, nodeVarDecl.var.mut);
    auto varTypeStr = codegenTypeStr(nodeVarDecl.var.type);

    code += std::string(codegen.indent, ' ') + codegenType(nodeVarDecl.var.type, nodeVarDecl.var.mut) + varName + " = ";

    if (nodeVarDecl.init == std::nullopt) {
      auto initCode = std::string("0");

      if (nodeVarDecl.var.type->isBool()) {
        initCode = "false";
      } else if (nodeVarDecl.var.type->isChar()) {
        initCode = R"('\0')";
      } else if (nodeVarDecl.var.type->isStr()) {
        initCode = R"("")";
      }

      code += codegenNodeExprInit(nodeVarDecl.var.type, initCode);
    } else {
      auto [initSetUp, initCode, initCleanUp] = codegenNodeExpr(codegen, *nodeVarDecl.init);
      setUp += initSetUp;

      if (std::holds_alternative<ASTExprAccess>(*(*nodeVarDecl.init).expr)) {
        code += varTypeStr + "_copy(" + initCode.substr(0, initCode.length() - 3) + ")";
      } else {
        code += codegenNodeExprInit(nodeVarDecl.var.type, initCode);
      }

      cleanUp += initCleanUp;
    }

    code += ";\n";
    cleanUp += std::string(codegen.indent, ' ') + codegenNodeExprDeinit(nodeVarDecl.var.type, varName) + ";\n";
  }

  return std::make_tuple(setUp, code, cleanUp);
}

Codegen codegen (AST *ast) {
  auto codegen = Codegen{};
  auto topLevelSetUp = std::string();
  auto topLevelCode = std::string();
  auto topLevelCleanUp = std::string();
  auto mainSetUp = std::string();
  auto mainCode = std::string();
  auto mainCleanUp = std::string();

  codegen.indent = 2;

  for (const auto &node : ast->nodes) {
    if (std::holds_alternative<ASTNodeMain>(node)) {
      codegen.indent = 0;
      mainSetUp += codegenForwardNode(codegen, node);
      codegen.indent = 2;
    } else {
      topLevelCode += codegenForwardNode(codegen, node);
    }
  }

  for (const auto &node : ast->nodes) {
    if (std::holds_alternative<ASTNodeMain>(node)) {
      codegen.indent = 0;

      auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, node);

      mainSetUp = nodeSetUp;
      mainCode = nodeCode;
      mainCleanUp = nodeCleanUp;

      codegen.indent = 2;
    } else {
      auto [nodeSetUp, nodeCode, nodeCleanUp] = codegenNode(codegen, node);

      topLevelSetUp += nodeSetUp;
      topLevelCode += nodeCode;
      topLevelCleanUp += nodeCleanUp;
    }
  }

  codegen.indent = 0;

  auto structDeclarationsCode = std::string();
  auto structDefinitionsCode = std::string();
  auto functionDeclarationsCode = std::string();
  auto functionDefinitionsCode = std::string();

  for (const auto &structDeclaration : codegen.structDeclarations) {
    structDeclarationsCode += "struct " + structDeclaration + ";\n";
  }

  for (const auto &[structDefinitionName, structDefinitionFields] : codegen.structDefinitions) {
    structDefinitionsCode += "struct " + structDefinitionName + " {\n";

    for (const auto &[structDefinitionFieldName, structDefinitionFieldType] : structDefinitionFields) {
      structDefinitionsCode += "  " + structDefinitionFieldType + structDefinitionFieldName + ";\n";
    }

    structDefinitionsCode += "};\n";
  }

  for (const auto &[functionDeclarationName, functionDeclarationInfo] : codegen.functionDeclarations) {
    auto [functionDeclarationType, functionDeclarationParams] = functionDeclarationInfo;
    functionDeclarationsCode += functionDeclarationType + functionDeclarationName + " " + functionDeclarationParams + ";\n";
  }

  for (const auto &[functionDefinitionName, functionDefinitionInfo] : codegen.functionDefinitions) {
    auto [functionDefinitionType, functionDefinitionParams, functionDefinitionBody] = functionDefinitionInfo;

    functionDefinitionsCode += functionDefinitionType + functionDefinitionName + " " + functionDefinitionParams + " {\n";
    functionDefinitionsCode += functionDefinitionBody;
    functionDefinitionsCode += "}\n";
  }

  codegen.output += "/*!\n";
  codegen.output += " * Copyright (c) Aaron Delasy\n";
  codegen.output += " *\n";
  codegen.output += " * Unauthorized copying of this file, via any medium is strictly prohibited\n";
  codegen.output += " * Proprietary and confidential\n";
  codegen.output += " */\n\n";
  codegen.output += foundationCode;
  codegen.output += structDeclarationsCode;
  codegen.output += structDeclarationsCode;
  codegen.output += structDefinitionsCode;
  codegen.output += functionDeclarationsCode;
  codegen.output += functionDefinitionsCode;
  codegen.output += "int main () {\n";
  codegen.output += topLevelSetUp;
  codegen.output += topLevelCode;
  codegen.output += mainSetUp;
  codegen.output += mainCode;
  codegen.output += mainCleanUp;
  codegen.output += topLevelCleanUp;
  codegen.output += "}\n";

  return codegen;
}
