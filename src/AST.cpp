/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"

AST::~AST () {
  for (const auto &stmt : this->mainBody) {
    delete stmt;
  }
}

void AST::add (Stmt *stmt) {
  if (stmt->type == STMT_MAIN) {
    auto stmtMain = std::get<StmtMain *>(stmt->body);

    this->mainPresent = true;
    stmtMain->body.swap(this->mainBody);
  }
}
