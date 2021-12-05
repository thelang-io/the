/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "AST.hpp"

AST::~AST () {
  for (auto topLevelStmt : this->topLevelStatements) {
    delete topLevelStmt;
  }

  delete this->mainBody;
}

void AST::add (Stmt *stmt) {
  if (stmt->type == STMT_MAIN) {
    auto stmtMain = std::get<StmtMain *>(stmt->body);

    this->mainPresent = true;
    this->mainBody = stmtMain->body;
    stmtMain->body = nullptr;
    delete stmt;
  } else {
    this->topLevelStatements.push_back(stmt);
  }
}
