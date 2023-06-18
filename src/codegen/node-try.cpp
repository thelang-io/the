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

std::string Codegen::_nodeTry (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeTry = std::get<ASTNodeTry>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;

  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp);
  code += std::string(this->indent, ' ') + this->_apiEval("switch (_{setjmp}(_{err_state}.buf[_{err_state}.buf_idx++])) {" EOL);
  this->indent += 2;
  code += std::string(this->indent, ' ') + "case 0: {" EOL;
  this->varMap.save();

  auto blockCleanUp = this->_apiEval(
    "if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);" EOL
    "_{err_state}.buf_idx--;"
  );

  code += this->_block(nodeTry.body, true, blockCleanUp, true);
  this->varMap.restore();
  code += std::string(this->indent + 2, ' ') + "break;" EOL;
  code += std::string(this->indent, ' ') + "}" EOL;
  this->state.cleanUp = initialStateCleanUp;

  for (const auto &handler : nodeTry.handlers) {
    auto handlerVarDecl = std::get<ASTNodeVarDecl>(*handler.param.body);
    auto handlerTypeInfo = this->_typeInfo(handlerVarDecl.var->type);
    auto handlerDef = this->_typeDef(handlerVarDecl.var->type);
    auto handleCodeName = Codegen::name(handlerVarDecl.var->codeName);
    auto paramCode = handlerTypeInfo.typeCodeConst + handleCodeName + " = (" + handlerTypeInfo.typeCodeTrimmed + ") _{err_state}.ctx;";

    code += std::string(this->indent, ' ') + this->_apiEval("case _{" + handlerDef + "}: {" EOL);
    code += std::string(this->indent + 2, ' ') + this->_apiEval("_{err_state}.buf_idx--;" EOL);
    code += std::string(this->indent + 2, ' ') + this->_apiEval("_{error_unset}(&_{err_state});" EOL);
    code += std::string(this->indent + 2, ' ') + this->_apiEval(paramCode) + EOL;

    this->varMap.save();
    code += this->_block(handler.body, true, this->_genFreeFn(handlerTypeInfo.type, handleCodeName) + ";");
    this->varMap.restore();

    code += std::string(this->indent + 2, ' ') + "break;" EOL;
    code += std::string(this->indent, ' ') + "}" EOL;
  }

  code += std::string(this->indent, ' ') + "default: {" EOL;
  code += std::string(this->indent + 2, ' ') + this->_apiEval("_{err_state}.buf_idx--;" EOL);
  code += std::string(this->indent + 2, ' ') + "goto " + this->state.cleanUp.currentLabel() + ";" EOL;
  code += std::string(this->indent, ' ') + "}" EOL;
  this->indent -= 2;
  code += std::string(this->indent, ' ') + "}" EOL;

  return this->_wrapNode(node, root, phase, decl + code);
}

std::string Codegen::_nodeTryAsync (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  return this->_nodeTry(node, root, phase, decl, code);
}
