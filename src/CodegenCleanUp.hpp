/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef THE_CORE_CODEGEN_CLEAN_UP_HPP
#define THE_CORE_CODEGEN_CLEAN_UP_HPP

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
