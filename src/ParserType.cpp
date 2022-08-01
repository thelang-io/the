/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ParserType.hpp"
#include "config.hpp"

std::string ParserType::xml (std::size_t indent) const {
  auto typeName = std::string("Type");

  if (std::holds_alternative<ParserTypeFn>(*this->body)) {
    typeName += "Fn";
  } else if (std::holds_alternative<ParserTypeId>(*this->body)) {
    typeName += "Id";
  } else if (std::holds_alternative<ParserTypeRef>(*this->body)) {
    typeName += "Ref";
  }

  auto attrs = std::string(this->parenthesized ? " parenthesized" : "");

  attrs += R"( start=")" + this->start.str() + R"(")";
  attrs += R"( end=")" + this->end.str() + R"(")";

  auto result = std::string(indent, ' ') + "<" + typeName + attrs + ">" EOL;

  if (std::holds_alternative<ParserTypeFn>(*this->body)) {
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
  } else if (std::holds_alternative<ParserTypeRef>(*this->body)) {
    auto typeRef = std::get<ParserTypeRef>(*this->body);
    result += typeRef.type.xml(indent + 2) + EOL;
  }

  return result + std::string(indent, ' ') + "</" + typeName + ">";
}
