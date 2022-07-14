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

struct CodegenCleanUpItem {
  std::string label;
  std::string content;
  bool labelUsed = false;
};

class CodegenCleanUp {
 public:
  std::size_t labelIdx = 0;

  CodegenCleanUp () = default;
  explicit CodegenCleanUp (std::size_t i) : labelIdx(i) {}

  void add (const std::string &);
  std::string currentLabel ();
  bool empty () const;
  std::string gen (std::size_t) const;
  CodegenCleanUp &update (std::size_t);

 private:
  std::vector<CodegenCleanUpItem> _data;
};

#endif
