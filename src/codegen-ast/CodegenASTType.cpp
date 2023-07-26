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

#include "CodegenAST.hpp"

CodegenASTType CodegenASTType::create (const std::string &val) {
  return CodegenASTType{val};
}

std::string CodegenASTType::strDecl () const {
  return this->val + (this->val.ends_with(" ") || this->val.ends_with("*") ? "" : " ");
}

std::string CodegenASTType::strDef () const {
  return this->val.substr(0, this->val.find_last_not_of(' ') + 1);
}
