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

#include "ParserType.hpp"
#include "config.hpp"

std::string ParserType::xml (std::size_t indent) const {
  auto typeName = std::string("Type");

  if (std::holds_alternative<ParserTypeArray>(*this->body)) {
    typeName += "Array";
  } else if (std::holds_alternative<ParserTypeFn>(*this->body)) {
    typeName += "Fn";
  } else if (std::holds_alternative<ParserTypeId>(*this->body)) {
    typeName += "Id";
  } else if (std::holds_alternative<ParserTypeMap>(*this->body)) {
    typeName += "Map";
  } else if (std::holds_alternative<ParserTypeOptional>(*this->body)) {
    typeName += "Optional";
  } else if (std::holds_alternative<ParserTypeRef>(*this->body)) {
    typeName += "Ref";
  } else if (std::holds_alternative<ParserTypeUnion>(*this->body)) {
    typeName += "Union";
  }

  auto attrs = std::string(this->parenthesized ? " parenthesized" : "");

  attrs += R"( start=")" + this->start.str() + R"(")";
  attrs += R"( end=")" + this->end.str() + R"(")";

  auto result = std::string(indent, ' ') + "<" + typeName + attrs + ">" EOL;

  if (std::holds_alternative<ParserTypeArray>(*this->body)) {
    auto typeArray = std::get<ParserTypeArray>(*this->body);
    result += typeArray.elementType.xml(indent + 2) + EOL;
  } else if (std::holds_alternative<ParserTypeFn>(*this->body)) {
    auto typeFn = std::get<ParserTypeFn>(*this->body);

    if (!typeFn.params.empty()) {
      result += std::string(indent + 2, ' ') + "<TypeFnParams>" EOL;

      for (const auto &typeFnParam : typeFn.params) {
        auto paramAttrs = std::string();

        paramAttrs += typeFnParam.mut ? " mut" : "";
        paramAttrs += typeFnParam.variadic ? " variadic" : "";

        result += std::string(indent + 4, ' ') + "<TypeFnParam" + paramAttrs + ">" EOL;

        if (typeFnParam.id != std::nullopt) {
          result += std::string(indent + 6, ' ') + "<TypeFnParamId>" EOL;
          result += typeFnParam.id->xml(indent + 8) + EOL;
          result += std::string(indent + 6, ' ') + "</TypeFnParamId>" EOL;
        }

        result += std::string(indent + 6, ' ') + "<TypeFnParamType>" EOL;
        result += typeFnParam.type.xml(indent + 8) + EOL;
        result += std::string(indent + 6, ' ') + "</TypeFnParamType>" EOL;
        result += std::string(indent + 4, ' ') + "</TypeFnParam>" EOL;
      }

      result += std::string(indent + 2, ' ') + "</TypeFnParams>" EOL;
    }

    result += std::string(indent + 2, ' ') + "<TypeFnReturnType>" EOL;
    result += typeFn.returnType.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeFnReturnType>" EOL;
  } else if (std::holds_alternative<ParserTypeId>(*this->body)) {
    auto typeId = std::get<ParserTypeId>(*this->body);
    result += typeId.id.xml(indent + 2) + EOL;
  } else if (std::holds_alternative<ParserTypeMap>(*this->body)) {
    auto typeMap = std::get<ParserTypeMap>(*this->body);

    result += std::string(indent + 2, ' ') + "<TypeMapKeyType>" EOL;
    result += typeMap.keyType.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeMapKeyType>" EOL;
    result += std::string(indent + 2, ' ') + "<TypeMapValueType>" EOL;
    result += typeMap.valueType.xml(indent + 4) + EOL;
    result += std::string(indent + 2, ' ') + "</TypeMapValueType>" EOL;
  } else if (std::holds_alternative<ParserTypeOptional>(*this->body)) {
    auto typeOptional = std::get<ParserTypeOptional>(*this->body);
    result += typeOptional.type.xml(indent + 2) + EOL;
  } else if (std::holds_alternative<ParserTypeRef>(*this->body)) {
    auto typeRef = std::get<ParserTypeRef>(*this->body);
    result += typeRef.refType.xml(indent + 2) + EOL;
  } else if (std::holds_alternative<ParserTypeUnion>(*this->body)) {
    auto typeUnion = std::get<ParserTypeUnion>(*this->body);

    for (const auto &subType : typeUnion.subTypes) {
      result += subType.xml(indent + 2) + EOL;
    }
  }

  return result + std::string(indent, ' ') + "</" + typeName + ">";
}
