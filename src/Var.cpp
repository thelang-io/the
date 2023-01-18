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

#include "Var.hpp"
#include "config.hpp"

std::string Var::xml (std::size_t indent) const {
  if (this->builtin) {
    return std::string(indent, ' ') + R"(<BuiltinVar name=")" + this->name + R"(" />)";
  }

  auto attrs = R"( name=")" + this->name + R"(")";

  attrs += R"( codeName=")" + this->codeName + R"(")";
  attrs += this->mut ? " mut" : "";
  attrs += this->ctxIgnored ? " ctxIgnored" : "";
  attrs += R"( frame=")" + std::to_string(this->frame) + R"(")";

  auto result = std::string(indent, ' ') + "<Var" + attrs + ">" EOL;
  result += this->type->xml(indent + 2) + EOL;
  return result + std::string(indent, ' ') + "</Var>";
}
