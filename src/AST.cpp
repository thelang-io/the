/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"

void AST::add (const Stmt &stmt) {
  if (stmt.type == stmtMain) {
    auto stmtMain = std::get<StmtMain>(stmt.body);

    this->mainPresent = true;
    this->mainBody = stmtMain.body;
  }
}
