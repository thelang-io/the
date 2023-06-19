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

#include "../Codegen.hpp"
#include "../config.hpp"

std::string Codegen::_nodeLoop (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
  auto initialCleanUp = this->state.cleanUp;
  auto initialIndent = this->indent;

  this->varMap.save();
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialCleanUp);
  this->state.cleanUp.breakVarIdx += 1;
  this->state.cleanUp.continueVarIdx += 1;

  if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt && nodeLoop.upd == std::nullopt) {
    code = std::string(this->indent, ' ') + "while (1)";
  } else if (nodeLoop.init == std::nullopt && nodeLoop.upd == std::nullopt) {
    code = std::string(this->indent, ' ') + "while (" + this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, decl, true) + ")";
  } else if (nodeLoop.init != std::nullopt) {
    this->indent += 2;
    auto initCode = this->_node(*nodeLoop.init);
    this->indent = initialIndent;

    if (this->state.cleanUp.hasCleanUp(CODEGEN_CLEANUP_BLOCK)) {
      code = std::string(this->indent, ' ') + "{" EOL + initCode;
      code += std::string(this->indent + 2, ' ') + "for (;";

      this->indent += 2;
    } else {
      code = std::string(this->indent, ' ') + "for (" + this->_node(*nodeLoop.init, false) + ";";
    }

    code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, decl, true)) + ";";
    code += (nodeLoop.upd == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, decl, true)) + ")";
  } else {
    code = std::string(this->indent, ' ') + "for (;";
    code += (nodeLoop.cond == std::nullopt ? "" : " " + this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, decl, true)) + ";";
    code += " " + this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, decl, true) + ")";
  }

  auto saveCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveCleanUp);

  auto bodyCode = this->_block(nodeLoop.body);
  code += " {" EOL;

  if (this->state.cleanUp.breakVarUsed) {
    code += std::string(this->indent + 2, ' ') + "unsigned char " + this->state.cleanUp.currentBreakVar() + " = 0;" EOL;
  }

  if (this->state.cleanUp.continueVarUsed) {
    code += std::string(this->indent + 2, ' ') + "unsigned char " + this->state.cleanUp.currentContinueVar() + " = 0;" EOL;
  }

  code += bodyCode;
  code += std::string(this->indent, ' ') + "}" EOL;

  if (nodeLoop.init != std::nullopt && !saveCleanUp.empty()) {
    code += saveCleanUp.gen(this->indent);

    if (saveCleanUp.returnVarUsed) {
      code += std::string(this->indent, ' ') + "if (r == 1) goto " + initialCleanUp.currentLabel() + ";" EOL;
    }

    code += std::string(this->indent - 2, ' ') + "}" EOL;
  }

  this->state.cleanUp.breakVarIdx -= 1;
  this->state.cleanUp.continueVarIdx -= 1;
  this->state.cleanUp = initialCleanUp;
  this->varMap.restore();
  this->indent = initialIndent;

  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeLoopAsync (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeLoop = std::get<ASTNodeLoop>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;
  auto initialIndent = this->indent;

  this->varMap.save();
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp);
  this->state.cleanUp.breakVarIdx += 1;
  this->state.cleanUp.continueVarIdx += 1;

  auto initCode = nodeLoop.init == std::nullopt ? "" : this->_nodeAsync(*nodeLoop.init);
  auto condCode = nodeLoop.cond == std::nullopt ? "" : this->_nodeExpr(*nodeLoop.cond, this->ast->typeMap.get("bool"), node, decl, true);
  auto updCode = nodeLoop.upd == std::nullopt ? "" : this->_nodeExpr(*nodeLoop.upd, nodeLoop.upd->type, node, decl, true);

  auto jumpAsyncCounter = nodeLoop.init != std::nullopt || nodeLoop.cond != std::nullopt
    ? ++this->state.asyncCounter
    : this->state.asyncCounter;

  if (nodeLoop.init == std::nullopt && nodeLoop.cond == std::nullopt) {
    code += std::string(this->indent - 2, ' ') + "}" EOL;
    code += std::string(this->indent - 2, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;
    jumpAsyncCounter = this->state.asyncCounter;
  }

  this->indent -= 2;
  auto saveState = this->state;
  auto saveStateAsyncCounter = this->state.asyncCounter;
  auto saveStateCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveStateCleanUp);
  this->_block(nodeLoop.body);
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &saveStateCleanUp);
  this->state.asyncBuffer.push_back(CodegenAsyncBufferItem{this->state.asyncCounter + 1, CodegenAsyncBufferItemLoopContinue});
  this->state.asyncBuffer.push_back(CodegenAsyncBufferItem{this->state.asyncCounter + 2, CodegenAsyncBufferItemLoopBreak});
  this->state.asyncCounter = saveStateAsyncCounter;
  auto bodyCode = this->_block(nodeLoop.body);
  this->indent += 2;

  code += bodyCode;
  auto resetAsyncCounter = jumpAsyncCounter == 0 ? this->state.asyncCounter : jumpAsyncCounter;

  code += std::string(this->indent - 2, ' ') + "}" EOL;
  code += std::string(this->indent - 2, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;

  if (nodeLoop.upd != std::nullopt) {
    code += std::string(this->indent, ' ') + updCode + ";" EOL;
  }

  code += std::string(this->indent, ' ') + "return " + std::to_string(resetAsyncCounter) + ";" EOL;
  code += std::string(this->indent - 2, ' ') + "}" EOL;
  code += std::string(this->indent - 2, ' ') + "case " + std::to_string(++this->state.asyncCounter) + ": {" EOL;

  auto codeHead = std::string();

  if (nodeLoop.init != std::nullopt || this->state.cleanUp.breakVarUsed || this->state.cleanUp.continueVarUsed) {
    if (this->state.cleanUp.breakVarUsed) {
      codeHead += std::string(this->indent, ' ') + "*" + this->state.cleanUp.currentBreakVar() + " = 0;" EOL;
    }

    if (this->state.cleanUp.continueVarUsed) {
      codeHead += std::string(this->indent, ' ') + "*" + this->state.cleanUp.currentContinueVar() + " = 0;" EOL;
    }

    if (nodeLoop.init != std::nullopt) {
      codeHead += initCode;
    }

    codeHead += std::string(this->indent - 2, ' ') + "}" EOL;
    codeHead += std::string(this->indent - 2, ' ') + "case " + std::to_string(jumpAsyncCounter) + ": {" EOL;
  }

  if (nodeLoop.cond != std::nullopt) {
    if (nodeLoop.init == std::nullopt) {
      codeHead += std::string(this->indent - 2, ' ') + "}" EOL;
      codeHead += std::string(this->indent - 2, ' ') + "case " + std::to_string(jumpAsyncCounter) + ": {" EOL;
    }

    codeHead += std::string(this->indent, ' ') + "if (!(" + condCode + ")) {" EOL;
    codeHead += std::string(this->indent + 2, ' ') + "return " + std::to_string(this->state.asyncCounter) + ";" EOL;
    codeHead += std::string(this->indent, ' ') + "}" EOL;
  }

  code = codeHead + code;

  this->state.cleanUp.breakVarIdx -= 1;
  this->state.cleanUp.continueVarIdx -= 1;
  this->state.cleanUp = initialStateCleanUp;
  this->varMap.restore();
  this->indent = initialIndent;

  return this->_wrapNode(node, root, phase, decl + code);
}
