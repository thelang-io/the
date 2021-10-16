/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <iostream>
#include "AST.hpp"
#include "Error.hpp"
#include "Parser.hpp"

int main () {
  try {
    auto reader = Reader("program.adl");
    auto ast = AST();

    while (!reader.eof()) {
      auto stmt = parse(&reader);

      if (stmt.type == stmtEnd) {
        break;
      }

      std::cout << stmtStr(stmt) << std::endl;
      ast.add(stmt);
    }

    return EXIT_SUCCESS;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
