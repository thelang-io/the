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

#ifndef SRC_CODEGEN_CLEAN_UP_HPP
#define SRC_CODEGEN_CLEAN_UP_HPP

#include <string>
#include <vector>

enum CodegenCleanUpType {
  CODEGEN_CLEANUP_ROOT,
  CODEGEN_CLEANUP_BLOCK,
  CODEGEN_CLEANUP_FN,
  CODEGEN_CLEANUP_LOOP
};

struct CodegenCleanUpItem {
  std::string label;
  std::string content;
  bool labelUsed = false;
};

class CodegenCleanUp {
 public:
  CodegenCleanUpType type = CODEGEN_CLEANUP_ROOT;
  CodegenCleanUp *parent = nullptr;
  std::size_t labelIdx = 0;
  std::size_t breakVarIdx = 0;
  bool breakVarUsed = false;
  bool returnVarUsed = false;
  bool valueVarUsed = false;

  CodegenCleanUp () = default;
  explicit CodegenCleanUp (CodegenCleanUpType, CodegenCleanUp *);

  void add (const std::string &);
  std::string currentBreakVar ();
  std::string currentLabel ();
  std::string currentReturnVar ();
  std::string currentValueVar ();
  bool empty () const;
  std::string gen (std::size_t) const;
  bool hasCleanUp (CodegenCleanUpType) const;

 private:
  std::vector<CodegenCleanUpItem> _data;
};

#endif
