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

#include <algorithm>
#include "../Codegen.hpp"
#include "../config.hpp"

std::string Codegen::_nodeEnumDecl (const ASTNode &node, bool root, CodegenPhase phase, std::string &decl, std::string &code) {
  auto nodeEnumDecl = std::get<ASTNodeEnumDecl>(*node.body);
  auto typeName = Codegen::typeName(nodeEnumDecl.type->codeName);
  auto enumType = std::get<TypeEnum>(nodeEnumDecl.type->body);

  if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
    this->_apiEntity(typeName, CODEGEN_ENTITY_ENUM, [&] (auto &decl, [[maybe_unused]] auto &def) {
      auto membersCode = std::string();

      for (auto i = static_cast<std::size_t>(0); i < enumType.members.size(); i++) {
        auto member = enumType.members[i];

        auto nodeEnumDeclMember = std::find_if(
          nodeEnumDecl.members.begin(),
          nodeEnumDecl.members.end(),
          [&member] (const auto &it) -> bool {
            return it.id == member->name;
          }
        );

        if (nodeEnumDeclMember != nodeEnumDecl.members.end()) {
          membersCode += "  " + Codegen::name(member->codeName);

          if (nodeEnumDeclMember->init != std::nullopt) {
            membersCode += " = " + this->_nodeExpr(*nodeEnumDeclMember->init, nodeEnumDeclMember->init->type, node, decl, true);
          }

          membersCode += i == enumType.members.size() - 1 ? EOL : "," EOL;
        }
      }

      decl += "enum " + typeName + " {" EOL;
      decl += membersCode;
      decl += "};";

      return 0;
    });

    this->_apiEntity(typeName + "_rawValue", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
      auto typeInfo = this->_typeInfo(nodeEnumDecl.type);

      decl += "_{struct str} " + typeName + "_rawValue (" + typeInfo.typeCodeTrimmed + ");";
      def += "_{struct str} " + typeName + "_rawValue (" + typeInfo.typeCode + "n) {" EOL;

      for (const auto &member : enumType.members) {
        def += "  if (n == " + Codegen::name(member->codeName) + ")" R"( return _{str_alloc}(")" + member->name + R"(");)" EOL;
      }

      def += "}";

      return 0;
    });
  }

  return this->_wrapNode(node, root, phase, decl + code);
}
